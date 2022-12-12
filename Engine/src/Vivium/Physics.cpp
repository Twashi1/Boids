#include "Physics.h"
#include "Application.h"

namespace Vivium {
	Timer Physics::m_Timer;
	Physics::LayerMap_t Physics::m_Layers;

	void Physics::m_ResolveInterLayer(Layer* layer0, Layer* layer1)
	{
		std::size_t layer0_size = layer0->bodies.size();
		std::size_t layer1_size = layer1->bodies.size();

		for (std::size_t i = 0; i < layer0_size; i++) {
			for (std::size_t j = 0; j < layer1_size; j++) {
				m_TestCollision(layer0->bodies[i].lock(), layer1->bodies[j].lock());
			}
		}
	}

	void Physics::m_ResolveIntraLayer(Layer* layer)
	{
		std::size_t layer_size = layer->bodies.size();

		for (std::size_t i = 0; i < layer_size; i++) {
			for (std::size_t j = i + 1; j < layer_size; j++) {
				m_TestCollision(layer->bodies[i].lock(), layer->bodies[j].lock());
			}
		}
	}

	void Physics::m_TestCollision(const std::shared_ptr<Body>& a, const std::shared_ptr<Body>& b)
	{
		float elapsed = m_Timer.GetElapsedNoReset();

		Vector2<float> future_pos_a = a->PeekPos(elapsed);
		Vector2<float> future_pos_b = b->PeekPos(elapsed);

		// TODO: some sort of cache for a given shape?
		Vector2<float> a_dim = a->quad->GetDim();
		Vector2<float> b_dim = b->quad->GetDim();

		float radius_a = 0.5f * a_dim.magnitude();
		float radius_b = 0.5f * b_dim.magnitude();

		// DEBUG 
		if (Flag::Test(VIVIUM_FLAG_DRAW_PHYSICS_CIRCLES)) {
			Renderer::DrawCircle(future_pos_a, radius_a, RGBColor::RED);
			Renderer::DrawCircle(future_pos_b, radius_b, RGBColor::BLUE);
		}

		// Test if there is a collision between them
		// https://stackoverflow.com/questions/697188/fast-circle-collision-detection
		float radius_sum = radius_a + radius_b;
		float dx = future_pos_a.x - future_pos_b.x;
		float dy = future_pos_a.y - future_pos_b.y;

		// If the sum of our radius is more than the distance between the centers of the circles, they are overlapping
		// NOTE: squared here since we're using squares distance for speed
		if (radius_sum * radius_sum >= (dx * dx + dy * dy)) {
			// Calculate future positions of bodies
			Rect future_rect_a = a->PeekRect(elapsed); // TODO: derive from previous peeked information + extra stuff
			Rect future_rect_b = b->PeekRect(elapsed);

			std::array<Vector2<float>, 4> a_vertices = future_rect_a.GetVertices();
			std::array<Vector2<float>, 4> b_vertices = future_rect_b.GetVertices();

			// If either quad contains any points of the other quad (intersecting)
			Rect::Manifold manifold = Rect::GetIntersection(
				future_rect_a.width, future_rect_a.height,
				future_rect_b.width, future_rect_b.height,
				a_vertices, b_vertices
			);

			if (manifold.collisionOccured) {
				m_ResolveCollision(a, b, future_rect_a, future_rect_b, a_vertices, b_vertices, manifold);

				a->m_InvokeCollisionCallback();
				b->m_InvokeCollisionCallback();
			}
		}
	}

	void Physics::m_ResolveCollision(std::shared_ptr<Body> a, std::shared_ptr<Body> b, const Rect& a_rect, const Rect& b_rect, const std::array<Vector2<float>, 4>& a_vertices, const std::array<Vector2<float>, 4>& b_vertices, const Rect::Manifold& manifold)
	{
		if (a->isPhysical && b->isPhysical) {
			float restitution = std::min(a->restitution, b->restitution);

			//#define EXPERIMENTAL_IMPULSE_RESOLUTION
#ifdef EXPERIMENTAL_IMPULSE_RESOLUTION
			// Ripped formula for resolution from here, but manifold stuff is mine
			// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

			Vector2<float> rel_vel = b->vel - a->vel;

			float vel_along_normal = Vector2<float>::Dot(rel_vel, manifold.face_normal);

			// If moving away from each other
			if (vel_along_normal > 0.0f) return;

			// Impulse scalar
			float j = -(1.0f + restitution) * vel_along_normal;
			j /= a->imass + b->imass;

			Vector2<float> impulse = j * manifold.face_normal;
			// Not perfect
			a->vel = a->isImmovable ? 0.0f : a->vel - a->imass * impulse;
			b->vel = b->isImmovable ? 0.0f : b->vel + b->imass * impulse;

			Vector2<float> correction = Vector2<float>(1.0f) / (a->imass + b->imass) * 0.2f * manifold.face_normal;
			a->quad->SetCenter(a->quad->GetCenter() - a->imass * correction);
			b->quad->SetCenter(b->quad->GetCenter() + b->imass * correction);

			if (a->isImmovable) a->acc = 0.0f;
			if (b->isImmovable) b->acc = 0.0f;
#else
			// NOTE: did this algebra myself so its probably super inefficient
			// e(u1-u2)
			Vector2<float> k = (a->vel - b->vel) * restitution;

			// [m1u1 + m2(u2-k)]/(m1+m2) = v1
			Vector2<float> v1 = (a->vel * a->mass + (b->vel - k) * b->mass) / (a->mass + b->mass);
			// k+v1=v2
			Vector2<float> v2 = v1 + k;

			a->vel = a->isImmovable ? 0.0f : v1;
			b->vel = b->isImmovable ? 0.0f : v2;

			if (a->isImmovable) a->acc = 0.0f;
			if (b->isImmovable) b->acc = 0.0f;
#endif
		}
	}

	void Physics::Update()
	{
		std::set<Vector2<uint32_t>> tested_collisions;

		for (auto& [layer_index, layer] : m_Layers) {
			for (auto& other_layer_index : layer.collision_matrix) {
				// Create vector2 to represent the collision between the layers
				// Commutative (a, b == b, a)
				Vector2<uint32_t> possible_collision = layer_index > other_layer_index ?
					Vector2<uint32_t>(layer_index, other_layer_index) : Vector2<uint32_t>(other_layer_index, layer_index);

				auto it = tested_collisions.insert(possible_collision);

				// Indicates that it was successful, thus the two layers should collide since they haven't been checked against each other
				if (it.second) {
					if (layer_index == other_layer_index) {
						layer.m_RemoveExpiredBodies();
						m_ResolveIntraLayer(&layer);
					}
					else {
						// Fetch other layer
						auto it = m_Layers.find(other_layer_index);

						if (it != m_Layers.end()) {
							layer.m_RemoveExpiredBodies();
							it->second.m_RemoveExpiredBodies();
							m_ResolveInterLayer(&layer, &it->second);
						}
						else {
							LogWarn("Invalid collision matrix for layer {}, no layer by id {}", layer_index, other_layer_index);
						}
					}
				}
			}
		}

		m_Timer.Start();
	}

	Physics::Layer* Physics::CreateLayer(const uint32_t& id, const std::set<uint32_t>& collision_matrix)
	{
		auto it = m_Layers.insert({ id, Layer(collision_matrix) });
		return &(it.first->second);
	}

	void Physics::DeleteLayer(const uint32_t& id)
	{
		auto it = m_Layers.find(id);

		if (it != m_Layers.end()) {
			m_Layers.erase(it);
		}
		else {
			LogWarn("Couldn't delete layer: couldn't find layer by id {}", id);
		}
	}

	void Physics::m_Init()
	{
		m_Timer.Start();
	}

	void Physics::Layer::m_RemoveExpiredBodies()
	{
		for (auto it = bodies.begin(); it != bodies.end();) {
			// If its an expired body
			if (it->expired()) {
				// Remove
				it = bodies.erase(it);
			}
			// If its not an expired body
			else {
				// Go to next
				++it;
			}
		}
	}

	Physics::Layer::Layer(const std::set<uint32_t>& collision_matrix)
		: collision_matrix(collision_matrix)
	{}
}