#pragma once

#include "Rect.h"
#include "Vector2.h"
#include "Quad.h"

namespace Vivium {
	class Physics;

	struct VIVIUM_API Body : public Streamable {
	private:
		Timer m_Timer;

		// Technically doesn't make sense for this to be private while collision_callback and user_params are public
		virtual void m_InvokeCollisionCallback();

	public:
		// TODO: callback should take both bodies which are colliding
		typedef void (*CallbackFunc_t)(Body*, void*); // Collision callback function shorthand

		// TODO: user params is weird, maybe something general for a callback function
		CallbackFunc_t collision_callback = nullptr;
		void* user_params = nullptr;

		std::shared_ptr<Quad> quad = nullptr;	// Pointer to quad which describes location and dimensions of Body
		bool isImmovable = false;	// If an object is immovable, collisions will not change velocity/acceleration of this body (infinite mass)
		float restitution = 0.0f;	// Represents amount of velocity transferred to this object in collision, but has no link to actual coefficient of restitution in physics (basically just a bad name)
		float mass = 1.0f;			// Mass of an object, also included in calculation of how much velocity is transferred to this object in a collision
		float imass = 0.0f;			// Inverse mass

		Vector2<float> vel = 0.0f;	// Current velocity of object
		Vector2<float> acc = 0.0f;	// Current acceleration of object

		float angular_vel = 0.0f;	// Current angular velocity of object
		float angular_acc = 0.0f;	// Current angular acceleration of object

		bool isPhysical = true;		// If is physical collider or not (non-physical only check for collisions, don't actually change velocity/acceleration)

		Body() = default;
		Body(std::shared_ptr<Quad> quad, bool isImmovable, float restitution, float mass, bool isPhysical = true, CallbackFunc_t callback = nullptr, void* user_params = nullptr);
		Body(const Body& other);
		Body(Body&& other) noexcept;

		Body& operator=(const Body& other);
		Body& operator=(Body&& other) noexcept;

		virtual ~Body() = default;

		// Updates position and velocity, takes time since last update as parameter
		virtual void Update();
		// Returns a quad which predicts the location the object will be at "dt" seconds into the future
		virtual Quad Peek(float dt);
		virtual Rect PeekRect(float dt);
		virtual Math::AABB PeekAABB(float dt);
		virtual Vector2<float> PeekPos(float dt);

		friend bool operator==(const Body& a, const Body& b) {
			return *a.quad == *b.quad
				&& a.isImmovable == b.isImmovable
				&& a.vel == b.vel && a.acc == b.acc
				&& a.mass == b.mass
				&& a.restitution == b.restitution
				&& a.angular_vel == b.angular_vel && a.angular_acc == b.angular_acc;
		}

		virtual void Write(Serialiser& s) const override;
		virtual void Read(Serialiser& s) override;

		friend Physics;
	};
}
