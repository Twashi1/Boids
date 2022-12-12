#include "Math.h"
#include "Vector2.h"

namespace Vivium {
	namespace Math {
		Vector2<float> Rotate(const Vector2<float>& point, float angle)
		{
			// Calculate trig values
			float cos_angle = cos(angle);
			float sin_angle = sin(angle);

			// Multiply by rotation matrix
			float rotated_x = (point.x * cos_angle) - (point.y * sin_angle);
			float rotated_y = (point.x * sin_angle) + (point.y * cos_angle);

			return Vector2<float>(rotated_x, rotated_y);
		}

		Vector2<float> Rotate(const Vector2<float>& point, float cos_angle, float sin_angle)
		{
			// Multiply by rotation matrix
			float rotated_x = (point.x * cos_angle) - (point.y * sin_angle);
			float rotated_y = (point.x * sin_angle) + (point.y * cos_angle);

			return Vector2<float>(rotated_x, rotated_y);
		}

		Vector2<float> Rotate(const Vector2<float>& point, float angle, const Vector2<float>& pivot)
		{
			Vector2<float> centered = point - pivot;

			// Calculate trig values
			float cos_angle = cos(angle);
			float sin_angle = sin(angle);

			// Multiply by rotation matrix
			float rotated_x = (centered.x * cos_angle) - (centered.y * sin_angle);
			float rotated_y = (centered.x * sin_angle) + (centered.y * cos_angle);

			return Vector2<float>(rotated_x, rotated_y) + pivot;
		}

		Vector2<float> Rotate(const Vector2<float>& point, float cos_angle, float sin_angle, const Vector2<float>& pivot)
		{
			Vector2<float> centered = point - pivot;

			// Multiply by rotation matrix
			float rotated_x = (centered.x * cos_angle) - (centered.y * sin_angle);
			float rotated_y = (centered.x * sin_angle) + (centered.y * cos_angle);

			return Vector2<float>(rotated_x, rotated_y) + pivot;
		}

		unsigned int Factorial(unsigned int n)
		{
			unsigned int val = 1;

			for (int i = 1; i <= n; ++i) {
				val *= i;
			}

			return val;
		}

		float TriangleArea(const Vector2<float>& a, const Vector2<float>& b, const Vector2<float>& c)
		{
			return std::abs(
				a.x * (b.y - c.y) +
				b.x * (c.y - a.y) +
				c.x * (a.y - b.y)
			) * 0.5f;
		}

		bool EqualBias(float a, float b, float bias)
		{
			return std::abs(a - b) <= bias;
		}

		double Round(double val, int decimal_places)
		{
			double ten_to_decimal_places = pow(10, decimal_places);
			double tmp = val * ten_to_decimal_places;

			if (tmp < 0) return ceil(tmp - 0.5) / ten_to_decimal_places;
			else return floor(tmp + 0.5) / ten_to_decimal_places;
		}

		Vector2<float> Round(Vector2<float> val, int decimal_places)
		{
			float ten_to_decimal_places = pow(10, decimal_places);
			float inverse_ten_to_decimal_places = 1.0 / ten_to_decimal_places;
			Vector2<float> tmp = val * ten_to_decimal_places;

			if (tmp.x < 0) tmp.x = ceil(tmp.x - 0.5) * inverse_ten_to_decimal_places;
			else tmp.x = floor(tmp.x + 0.5) * inverse_ten_to_decimal_places;

			if (tmp.y < 0) tmp.y = ceil(tmp.y - 0.5) * inverse_ten_to_decimal_places;
			else tmp.y = floor(tmp.y + 0.5) * inverse_ten_to_decimal_places;

			return tmp;
		}

		float DegreesToRadians(float degrees)
		{
			const float conversion = PI / 180.0f;
			return degrees * conversion;
		}

		float RadiansToDegrees(float radians)
		{
			const float conversion = 180.0f / PI;
			return radians * conversion;
		}

		AABB::AABB(float x, float y, float w, float h)
			: x(x), y(y), hw(w * 0.5f), hh(h * 0.5f)
		{}

		bool AABB::Contains(const Vector2<float>& other)
		{
			if (x - hw > other.x || x + hw < other.x) return false;
			if (y - hh > other.y || y + hh < other.y) return false;

			return true;
		}

		bool AABB::IsIntersecting(const AABB& other)
		{
			// Our left > Their right / Our right < Their left
			if (x - hw > other.x + hw || x + hw < other.x - hw) return false;
			// Our bottom > Their top / Our top < Their bottom
			if (y - hh > other.y + hh || y + hh < other.y - hh) return false;

			return true;
		}
	}
}
