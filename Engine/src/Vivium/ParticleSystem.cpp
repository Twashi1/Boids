#include "ParticleSystem.h"

namespace Vivium {
	const BufferLayout ParticleSystem::m_Layout = {
		Vivium::GLSLDataType::VEC2,	  // Position
		Vivium::GLSLDataType::FLOAT,  // Alpha
		Vivium::GLSLDataType::FLOAT   // Rotation
	};

	Particle::Particle(const Particle& other)
		: position(other.position), velocity(other.velocity),
		angle(other.angle), angular_velocity(other.angular_velocity),
		lifespan(other.lifespan), time_alive(other.time_alive), timer(other.timer)
	{}

	Particle::Particle(Particle&& other) noexcept
		: position(std::move(other.position)), velocity(std::move(other.velocity)),
		angle(std::move(other.angle)), angular_velocity(std::move(other.angular_velocity)),
		lifespan(std::move(other.lifespan)), time_alive(std::move(other.time_alive)), timer(std::move(other.timer))
	{}

	Particle& Particle::operator=(const Particle& other)
	{
		position =			other.position;
		velocity =			other.velocity;

		angle =				other.angle;
		angular_velocity =	other.angular_velocity;
		
		lifespan =			other.lifespan;
		time_alive =		other.time_alive;
		timer =				other.timer;

		return *this;
	}

	Particle& Particle::operator=(Particle&& other) noexcept
	{
		position =				std::move(other.position);
		velocity =				std::move(other.velocity);

		angle =					std::move(other.angle);
		angular_velocity =		std::move(other.angular_velocity);

		lifespan =				std::move(other.lifespan);
		time_alive =			std::move(other.time_alive);
		timer =					std::move(other.timer);

		return *this;
	}

	bool Particle::IsAlive() const
	{
		return lifespan > time_alive;
	}

	void Particle::Kill()
	{
		time_alive = lifespan;
	}

	void Particle::m_UpdateMotion(const Vector2<float>& accel)
	{
		float elapsed = timer.GetElapsed();

		velocity += accel * elapsed;
		position += velocity * elapsed;
		angle += angular_velocity * elapsed;

		time_alive += elapsed;
	}

	void Particle::m_Update(const Vector2<float>& accel)
	{
	}

	void Particle::Update(const Vector2<float>& accel)
	{
		if (IsAlive()) {
			m_UpdateMotion(accel);
			m_Update(accel);
		}
	}

	void ParticleSystem::m_EmitParticle(float lifespan, const Vector2<float>& pos, const Vector2<float>& vel, const Vector2<float>& var, float angle, float angular_vel, float angular_var)
	{
		Particle* particle = new Particle();

		particle->position = pos;
		particle->velocity = vel + Vector2<float>(Random::GetFloat(-var.x, var.x), Random::GetFloat(-var.y, var.y));

		particle->angle = angle;
		particle->angular_velocity = angular_vel + Random::GetFloat(-angular_var, angular_var);

		particle->lifespan = lifespan;

		Particle* replacement = m_Particles[m_Index];

		if (replacement != nullptr) {
			delete replacement;
		}

		m_Particles[m_Index] = std::move(particle);
	}

	void ParticleSystem::m_UpdateParticle(Particle* particle)
	{
		particle->Update(m_Acceleration);
	}

	void ParticleSystem::Emit(std::size_t count, float lifespan, const Vector2<float>& pos, const Vector2<float>& vel, const Vector2<float>& var, float angle, float angular_vel, float angular_var)
	{
		for (std::size_t i = 0; i < count; i++) {
			// TODO: m_EmitParticle returns some Particle*, and we handle adding to data with m_AddParticle(...), which also deals with the cyclical bounds of the data
			m_EmitParticle(lifespan, pos, vel, var, angle, angular_vel, angular_var);
			++m_Index;

			if (m_Index >= m_MaxSize) { m_Index -= m_MaxSize; }
		}
	}

	void ParticleSystem::Render()
	{
		Batch batch(m_MaxSize, &m_Layout);

		for (std::size_t i = 0; i < m_MaxSize; i++) {
			Particle* particle = m_Particles[i];

			if (particle == nullptr) { continue; }

			if (particle->IsAlive()) {
				m_UpdateParticle(particle);
				m_RenderParticle(&batch, particle);
			}
		}

		m_RenderBatch(&batch);
	}

	ParticleSystem::ParticleSystem(const std::size_t& max_size, std::shared_ptr<Shader> shader, const Vector2<float>& acceleration)
		: m_MaxSize(max_size), m_Shader(shader), m_Acceleration(acceleration)
	{
		m_Particles = new Particle*[max_size];

		std::fill(m_Particles, m_Particles + max_size, nullptr);
	}

	ParticleSystem::~ParticleSystem()
	{
		for (std::size_t i = 0; i < m_MaxSize; i++) {
			Particle* particle = m_Particles[i];

			if (particle != nullptr) delete particle;
		}

		delete[] m_Particles;
	}

	Particle** ParticleSystem::GetParticles()
	{
		return m_Particles;
	}

	std::size_t ParticleSystem::GetMaxSize() const
	{
		return m_MaxSize;
	}

	void ParticleSystem::m_RenderBatch(Batch* batch)
	{
		auto result = batch->End();

		if (result.count > 0) {
			Renderer::Submit(result.vertex_buffer.get(), result.index_buffer.get(), m_Shader.get());
		}
	}
}
