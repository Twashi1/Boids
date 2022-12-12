#pragma once

#include "Utils.h"
#include "Quad.h"
#include "Body.h"

namespace Vivium {
	class Application;

	class VIVIUM_API Physics {
	public:
		struct VIVIUM_API Layer {
		private:
			void m_RemoveExpiredBodies();

		public:
			std::vector<std::weak_ptr<Body>> bodies;		// Store reference to all bodies being tracked in this layer
			std::set<uint32_t> collision_matrix;	// Stores which other layer indices this one interacts/doesn't interact with (NOTE: name is the taken from Unity's collision matrix, but implemented slightly differently)

			Layer() = default;
			Layer(const std::set<uint32_t>& collision_matrix);

			friend Physics;
		};

	private:	
		static Timer m_Timer;

		typedef std::unordered_map<uint32_t, Layer> LayerMap_t;

		static LayerMap_t m_Layers;

		static void m_ResolveInterLayer(Layer* layer0, Layer* layer1);
		static void m_ResolveIntraLayer(Layer* layer);
		static void m_TestCollision(const std::shared_ptr<Body>& a, const std::shared_ptr<Body>& b);
		static void m_ResolveCollision(std::shared_ptr<Body> a, std::shared_ptr<Body> b, const Rect& a_rect, const Rect& b_rect, const std::array<Vector2<float>, 4>& a_vertices, const std::array<Vector2<float>, 4>& b_vertices, const Rect::Manifold& manifold);

		static void m_Init();

	public:
		// TODO: private?
		// Resolve collisions (ONLY! Doesn't update bodies for you)
		static void Update();

		// NOTE: must declare if you want the layer to interact with itself
		static Layer* CreateLayer(const uint32_t& id, const std::set<uint32_t>& collision_matrix);
		static void DeleteLayer(const uint32_t& id);
		// TODO: Delete by ptr

		friend Application;
	};
}
