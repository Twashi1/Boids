#pragma once

#define VIVIUM_EXPOSE_CORE
#include "../Engine/src/Vivium.h"

// TODO: can really speed up rule calculation
// TODO: take into account wrapping of position into distance calculations
// TODO: naming conventions

struct Boid {
	static uint16_t identifier_counter;

	static constexpr float size = 32.0f;
	static constexpr float speed = 128.0f;
	static constexpr float range = 300.0f;

	uint16_t identifier;
	Vivium::Vector2<float> center;
	Vivium::Vector2<float> vel;
	float angle;

	inline void UpdateAngle() {
		angle = std::atan2(vel.y, vel.x);
	}

	inline void UpdateVel() {
		float cos_angle = cos(angle);
		float sin_angle = sin(angle);

		vel.x = cos_angle * speed;
		vel.y = sin_angle * speed;
	}

	bool operator==(const Boid& other) {
		return identifier == other.identifier;
	}

	bool operator!=(const Boid& other) {
		return identifier != other.identifier;
	}

	Boid(Vivium::Vector2<float> center, float angle)
		: center(center), angle(angle), identifier(identifier_counter++) {
		UpdateVel();
	}

	void UpdatePosition(float elapsed) {
		center += vel * elapsed;
	}

	void WrapPosition(Vivium::Vector2<float> screen_dim) {
		if (center.x > screen_dim.x) {
			// On RHS of screen
			center.x -= screen_dim.x;
		}
		else if (center.x < 0) {
			// On LHS of screen
			center.x += screen_dim.x;
		}

		if (center.y > screen_dim.y) {
			// On top of screen
			center.y -= screen_dim.y;
		}
		else if (center.y < 0) {
			center.y += screen_dim.y;
		}
	}
};

uint16_t Boid::identifier_counter = 0;

class World {
public:
	std::vector<Boid> boids;
	std::unique_ptr<Vivium::Shader> shader;
	Vivium::Timer timer;
	std::unique_ptr<Vivium::Texture> tex;

	World() {
		timer.Start();

		shader = std::make_unique<Vivium::Shader>("boid_vertex", "boid_frag");
		tex = std::make_unique<Vivium::Texture>("boid.png");

		for (int i = 0; i < 50; i++) {
			boids.push_back(Boid(
				Vivium::Random::GetVector2f(200.0f),
				Vivium::Random::GetFloat(-Vivium::Math::PI, Vivium::Math::PI)
			));
		}
	}

	void Render() {
		static Vivium::BufferLayout layout = {
			Vivium::GLSLDataType::VEC2,  // pos
			Vivium::GLSLDataType::VEC2,  // tex coord
			Vivium::GLSLDataType::FLOAT, // rot
			Vivium::GLSLDataType::VEC2   // center
		};

		Vivium::Batch batch(boids.size(), &layout);

		float elapsed = timer.GetElapsed();

		auto screen_dim = Vivium::Application::GetScreenDim();

		for (Boid& boid : boids) {
			Vivium::Vector2<float> v1 = Cohesion(boid);
			Vivium::Vector2<float> v2 = Separation(boid);
			Vivium::Vector2<float> v3 = Alignment(boid);

			boid.vel += v1 + v2 + v3;
			boid.vel = Vivium::Vector2<float>::Normalise(boid.vel) * boid.speed;
			boid.UpdateAngle();
			boid.UpdatePosition(elapsed);
			boid.WrapPosition(screen_dim);

			float* vertex_data = new float[3];
			vertex_data[0] = boid.angle;
			vertex_data[1] = boid.center.x;
			vertex_data[2] = boid.center.y;

			batch.Submit(boid.center, Vivium::Vector2<float>(boid.size), 0.0f, 1.0f, 0.0f, 1.0f, vertex_data, 3);
		}

		auto res = batch.End();

		if (res) {
			tex->Bind(0);
			Vivium::Renderer::Submit(res.vertex_buffer.get(), res.index_buffer.get(), shader.get(), tex.get(), 0);
		}
	}

	Vivium::Vector2<float> Cohesion(const Boid& my_boid) {
		static const float weight = 0.005f;

		Vivium::Vector2<float> perceived_center {0.0f};

		int boids_evaluted = 0;

		for (Boid& boid : boids) {
			if (boid != my_boid) {
				if (Vivium::Vector2<float>::Distance(boid.center, my_boid.center) < Boid::range) {
					++boids_evaluted;
					perceived_center += boid.center;
				}
			}
		}

		perceived_center /= boids_evaluted;

		// Calculate vector of boid to center, multiply by weight
		return (perceived_center - my_boid.center) * weight;
	}

	Vivium::Vector2<float> Separation(const Boid& my_boid) {
		static const float separation = 30.0f;
		static const float weight = 0.2f;

		Vivium::Vector2<float> center { 0.0f };

		for (Boid& boid : boids) {
			if (boid != my_boid) {
				// No need to check range, if they're close enough for the magnitude check, they're in range
				if (Vivium::Vector2<float>::Magnitude(boid.center - my_boid.center) < separation) {
					center -= (boid.center - my_boid.center);
				}
			}
		}

		return center * weight;
	}

	Vivium::Vector2<float> Alignment(const Boid& my_boid) {
		static const float weight = 0.125f;

		Vivium::Vector2<float> perceived_velocity {0.0f};
		int boids_evaluted = 0;

		for (Boid& boid : boids) {
			if (boid != my_boid) {
				if (Vivium::Vector2<float>::Distance(boid.center, my_boid.center) < Boid::range) {
					++boids_evaluted;
					perceived_velocity += boid.vel;
				}
			}
		}

		perceived_velocity /= boids_evaluted;

		return (perceived_velocity - my_boid.vel) * weight;
	}
};
