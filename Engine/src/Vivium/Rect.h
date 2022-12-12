#pragma once

#include "Core.h"
#include "Vector2.h"
#include "Math.h"
#include "Serialiser.h"

namespace Vivium {
	class Physics;

	struct VIVIUM_API Rect : public Streamable {
	protected:
		static const Vector2<float> m_VertexOffsets[4]; // For calculating vertices

		struct Manifold {
			Vector2<float> intersecting_vertex = NAN;
			Vector2<float> edge_vector = NAN;
			// TODO: fix face normal not always pointing outwards?
			Vector2<float> face_normal = NAN;
			Vector2<float> intersecting_face_v0 = NAN;
			Vector2<float> intersecting_face_v1 = NAN;

			bool collisionOccured = false;
		};

	public:
		// For indexing the return value of GetVertices
		enum VERTEX : int {
			BOTTOMLEFT = 0,
			BOTTOMRIGHT = 1,
			TOPRIGHT = 2,
			TOPLEFT = 3
		};

		// Store as vector2, or x, y
		union {
			Vector2<float> center;
			struct { float x, y; };
		};

		// Store as vector2, or width, height
		union {
			Vector2<float> dim;
			struct { float width, height; };
		};

		float angle;

		std::array<Vector2<float>, 4> GetVertices() const;
		Vector2<float> BottomLeft() const;
		Vector2<float> BottomRight() const;
		Vector2<float> TopRight() const;
		Vector2<float> TopLeft() const;

		// NOTE: ignores rotation
		float Left() const;
		float Right() const;
		float Bottom() const;
		float Top() const;

		bool ContainsAnyOf(const std::array<Vector2<float>, 4>& vertices) const;
		bool ContainsAnyOf(const Rect& rect) const;
		bool Contains(const Vector2<float>& point) const;

		bool IsIntersecting(const Rect& rect) const;

		// TODO: ugly
		static bool Contains(float width, float height, const std::array<Vector2<float>, 4>& my_vertices, const Vector2<float>& point);
		static Manifold GetIntersection(float width1, float height1, float width2, float height2, const std::array<Vector2<float>, 4>& my_vertices, const std::array<Vector2<float>, 4>& their_vertices);

		Rect();
		Rect(float x, float y, float width, float height, float angle = 0.0f);
		Rect(const Vector2<float>& center, const Vector2<float>& dim, float angle = 0.0f);
		Rect(const Rect& other);
		Rect(Rect&& other) noexcept;
		~Rect() = default;

		Rect& operator=(const Rect& other) {
			x = other.x;
			y = other.y;
			width = other.width;
			height = other.height;
			angle = other.angle;

			return *this;
		}
		
		Rect& operator=(Rect&& other) noexcept {
			x = std::move(other.x);
			y = std::move(other.y);
			width = std::move(other.width);
			height = std::move(other.height);
			angle = std::move(other.angle);

			return *this;
		}

		void Write(Serialiser& s) const override;
		void Read(Serialiser& s) override;

		friend Physics;
	};
}

bool operator==(const Vivium::Rect& a, const Vivium::Rect& b);