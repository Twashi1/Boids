#pragma once

#define VIVIUM_EXPOSE_CORE
#include "../Engine/src/Vivium.h"

struct Boid {
	static constexpr float size = 32.0f;
	static constexpr float speed = 128.0f;

	Vivium::Vector2<float> center;
	float angle;
	float cos_angle;
	float sin_angle;

	inline void UpdateAngle(float n) {
		angle = n;
		cos_angle = cos(n);
		sin_angle = sin(n);
	}

	Boid(Vivium::Vector2<float> center, float angle)
		: center(center), angle(angle) {
		cos_angle = cos(angle);
		sin_angle = sin(angle);
	}

	void UpdateVelocity(float elapsed) {
		center.x += cos_angle * speed * elapsed;
		center.y += sin_angle * speed * elapsed;
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
			boid.UpdateVelocity(elapsed);
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
};
