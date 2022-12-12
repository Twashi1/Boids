#pragma once

#include "Core.h"
#include "Batch.h"
#include "Shader.h"
#include "Renderer.h"

namespace Vivium {
	struct VIVIUM_API Particle {
	protected:
		virtual void m_UpdateMotion(const Vector2<float>& accel);
		virtual void m_Update(const Vector2<float>& accel);

	public:
		Vector2<float> position = 0.0f;
		Vector2<float> velocity = 0.0f;

		float angular_velocity = 0.0f;
		float angle = 0.0f;

		float lifespan = 0.0f;
		float time_alive = 0.0f;
		Timer timer;

		virtual bool IsAlive() const;
		virtual void Kill();
		virtual void Update(const Vector2<float>& accel);

		Particle() = default;
		Particle(const Particle& other);
		Particle(Particle&& other) noexcept;
		virtual ~Particle() = default;

		Particle& operator=(const Particle& other);
		Particle& operator=(Particle&& other) noexcept;
	};

	// TODO: IRenderable, Batchable ideal as well
	class VIVIUM_API ParticleSystem {
	protected:
		static const BufferLayout m_Layout;
		std::shared_ptr<Shader> m_Shader;

		Vector2<float> m_Acceleration; // General acceleration of all particles

		Particle** m_Particles;  // 1D array of Particle* (cyclical buffer kinda thing?)
		std::size_t m_MaxSize;	 // Maximum amount of particles that can exist at once
		std::size_t m_Index = 0; // Current index of the rear of the particles queue

		virtual void m_EmitParticle(float lifespan, const Vector2<float>& pos, const Vector2<float>& vel, const Vector2<float>& var, float angle, float angular_vel, float angular_var);

		virtual void m_RenderParticle(Batch* batch, const Particle* particle) = 0;
		virtual void m_UpdateParticle(Particle* particle);

		virtual void m_RenderBatch(Batch* batch);

	public:
		ParticleSystem(const std::size_t& max_size, std::shared_ptr<Shader> shader, const Vector2<float>& acceleration = 0.0f);
		virtual ~ParticleSystem();

		Particle** GetParticles();
		std::size_t GetMaxSize() const;

		// TODO: Emit specification might be useful
		// TODO: Maybe emit just returns the particle* instead? and user assigns values (impossible because inside emit can't allocate?)
		// TODO: Maybe user gives particle* to emit? but then how does emit allocate the ptr? user shouldn't control lifespan
		virtual void Emit(std::size_t count, float lifespan = 1.0f, const Vector2<float>& pos = 0.0f, const Vector2<float>& vel = 0.0f, const Vector2<float>& var = 0.0f, float angle = 0.0f, float angular_vel = 0.0f, float angular_var = 0.0f);
		
		virtual void Render();
	};
}
