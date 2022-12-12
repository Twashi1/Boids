#pragma once

#include "Core.h"

namespace Vivium {
	template <typename T> requires Arithmetic<T>
	struct Vector2;

	namespace Math {
		const float PI = 3.14159265f;
		const float ROOT2 = 1.41421356f;

		VIVIUM_API Vector2<float> Rotate(const Vector2<float>& point, float angle);
		VIVIUM_API Vector2<float> Rotate(const Vector2<float>& point, float cos_angle, float sin_angle);
		VIVIUM_API Vector2<float> Rotate(const Vector2<float>& point, float angle, const Vector2<float>& pivot);
		VIVIUM_API Vector2<float> Rotate(const Vector2<float>& point, float cos_angle, float sin_angle, const Vector2<float>& pivot);

		VIVIUM_API unsigned int Factorial(unsigned int n);

		VIVIUM_API float TriangleArea(const Vector2<float>& a, const Vector2<float>& b, const Vector2<float>& c);

		VIVIUM_API bool EqualBias(float a, float b, float bias);
		VIVIUM_API double Round(double val, int decimal_places);
		VIVIUM_API Vector2<float> Round(Vector2<float> val, int decimal_places);

		VIVIUM_API float DegreesToRadians(float degrees);
		VIVIUM_API float RadiansToDegrees(float radians);

		struct VIVIUM_API AABB {
			float x, y, hw, hh; // center and half dimensions

			AABB(float x, float y, float w, float h);
			AABB(const AABB& other) = default;

			bool Contains(const Vector2<float>& other);
			bool IsIntersecting(const AABB& other);
		};
	}
}