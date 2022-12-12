#pragma once

#include "Core.h"

namespace Vivium {
	template <typename T> requires Arithmetic<T>
	struct VIVIUM_API Vector2 {
	public:
		T x, y;

		Vector2() : x(T()), y(T()) {};

		Vector2(T v) : x(v), y(v) {};
		Vector2(T x, T y) : x(x), y(y) {};

		Vector2(const Vector2& other) = default;
		Vector2(Vector2&& other) noexcept = default;

		Vector2& operator=(const Vector2& other) = default;
		Vector2& operator=(Vector2& other) noexcept = default;

		Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
		Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
		Vector2 operator*(const Vector2& other) const { return Vector2(x * other.x, y * other.y); }
		Vector2 operator/(const Vector2& other) const { return Vector2(x / other.x, y / other.y); }

		friend Vector2 operator*(const T& s, const Vector2& v) { return Vector2(s * v.x, s * v.y); }
		friend Vector2 operator/(const T& s, const Vector2& v) { return Vector2(v.x / s, v.y / s); }
		friend Vector2 operator*(const Vector2& v, const T& s) { return Vector2(s * v.x, s * v.y); }
		friend Vector2 operator/(const Vector2& v, const T& s) { return Vector2(v.x / s, v.y / s); }

		void operator+=(const Vector2& other) { x += other.x; y += other.y; }
		void operator-=(const Vector2& other) { x -= other.x; y -= other.y; }
		void operator*=(const Vector2& other) { x *= other.x; y *= other.y; }
		void operator/=(const Vector2& other) { x /= other.x; y /= other.y; }

		void operator*=(const T& other) { x *= other; y *= other; }
		void operator/=(const T& other) { x /= other; y /= other; }

		Vector2 operator-() const requires Signed<T> { return Vector2(-x, -y); }

		Vector2 operator%(const T& other) const requires Integral<T> { return Vector2(x % other, y % other); }
		void operator%=(const T& other) requires Integral<T> { x %= other; y %= other; }

		friend bool operator<(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x ? a.y < b.y : a.x < b.x; }
		friend bool operator>(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x ? a.y > b.y : a.x > b.x; }
		friend bool operator<=(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x ? a.y <= b.y : a.x <= b.x; }
		friend bool operator>=(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x ? a.y >= b.y : a.x >= b.x; }
		friend bool operator==(const Vector2<T>& a, const Vector2<T>& b) { return a.x == b.x && a.y == b.y; }
		friend bool operator!=(const Vector2<T>& a, const Vector2<T>& b) { return a.x != b.x || a.y != b.y; }

		explicit operator glm::vec2() const { return glm::vec2(x, y); }

		Vector2 floor() const requires FloatingPoint<T> { return Vector2(std::floor(x), std::floor(y)); }
		Vector2 ceil() const requires FloatingPoint<T> { return Vector2(std::ceil(x), std::ceil(y)); }
		// TODO: edit this as well
		Vector2 normalise() const requires FloatingPoint<T> { return Normalise(*this); }
		Vector2 fract() const requires FloatingPoint<T>
		{
			float int_part;
			float fx = std::modf(x, &int_part);
			float fy = std::modf(y, &int_part);

			return Vector2<float>(fx, fy);
		}

		Vector2 abs() const requires Signed<T> { return Vector2(std::abs(x), std::abs(y)); }

		T magnitude() const { return std::sqrt(x * x + y * y); }

		T distance(const Vector2& other) const
		{
			T dx = x - other.x;
			T dy = y - other.y;
			return std::sqrt(dx * dx + dy * dy);
		}

		T sqr_distance(const Vector2& other) const
		{
			T dx = x - other.x;
			T dy = y - other.y;
			return dx * dx + dy * dy;
		}

		T MaxComponent() const { return std::max(x, y); }

		static T Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
		static T Magnitude(const Vector2& v) {
			return std::sqrt(v.x * v.x + v.y * v.y);
		}
		static T Distance(const Vector2& a, const Vector2& b)
		{
			T dx = a.x - b.x;
			T dy = a.y - b.y;
			return std::sqrt(dx * dx + dy * dy);
		}
		static T SqrDistance(const Vector2& a, const Vector2& b)
		{
			T dx = a.x - b.x;
			T dy = a.y - b.y;
			return dx * dx + dy * dy;
		}
		static Vector2 Fract(const Vector2& v) requires FloatingPoint<T> {
			float int_part;
			float fx = std::modf(v.x, &int_part);
			float fy = std::modf(v.y, &int_part);

			return Vector2<float>(fx, fy);
		}
		static Vector2 Normalise(const Vector2& v) requires FloatingPoint<T> {
			float magnitude = Magnitude(v);

			if (magnitude == 0.0f) { return 0.0f; }
			else { return v / magnitude; }
		}
		static Vector2 Floor(const Vector2& v) requires FloatingPoint<T> {
			return Vector2<T>(std::floor(v.x), std::floor(v.y));
		}
		static Vector2 Ceil(const Vector2& v) requires FloatingPoint<T> {
			return Vector2<T>(std::ceil(v.x), std::ceil(v.y));
		}
		static Vector2 Abs(const Vector2& v) requires Signed<T> {
			return Vector2<T>(::std::abs(v.x), ::std::abs(v.y));
		}

		// Cast to arithmetic type
		template <typename C>
		operator Vector2<C>() const requires Arithmetic<C> { return Vector2<C>((C)x, (C)y); }

		friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
			os << "[" << v.x << ", " << v.y << "]";

			return os;
		}
	};

	// Stolen from boost
	template <typename T>
	inline void hash_combine(std::size_t& seed, const T& v) {
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}

namespace std {
	template <typename T>
	struct hash<Vivium::Vector2<T>> {
		std::size_t operator()(const Vivium::Vector2<T>& k) const noexcept {
			std::size_t hash = 0;
			Vivium::hash_combine<T>(hash, k.x);
			Vivium::hash_combine<T>(hash, k.y);

			return hash;
		}
	};

	template <typename T>
	struct formatter<Vivium::Vector2<T>> : formatter<string> {
		auto format(Vivium::Vector2<T> v, format_context& ctx) {
			return formatter<string>::format(
				std::format("[{}, {}]", v.x, v.y), ctx);
		}
	};
}