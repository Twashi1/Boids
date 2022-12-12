#include "Rect.h"
#include "Physics.h"

namespace Vivium {
	const Vector2<float> Rect::m_VertexOffsets[4] = {
		{-1.0f, -1.0f }, // Bottom left corner
		{ 1.0f, -1.0f }, // Bottom right corner
		{ 1.0f,  1.0f }, // Top right corner
		{-1.0f,  1.0f }  // Top left corner
	};

	std::array<Vector2<float>, 4> Rect::GetVertices() const
	{
		std::array<Vector2<float>, 4> vertices;

		Vector2<float> halfdim = dim * 0.5f; // Calculate half width and height

		// Precalc some trig
		float cos_angle = std::cos(angle);
		float sin_angle = std::sin(angle);

		for (int i = 0; i < 4; i++) {
			vertices[i] = Math::Rotate(
				center + (halfdim * m_VertexOffsets[i]),
				cos_angle, sin_angle,
				center
			);
		}

		return vertices;
	}

	Vector2<float> Rect::BottomLeft() const
	{
		return Math::Rotate(center + (dim * 0.5f * m_VertexOffsets[VERTEX::BOTTOMLEFT]), angle, center);
	}

	Vector2<float> Rect::BottomRight() const
	{
		return Math::Rotate(center + (dim * 0.5f * m_VertexOffsets[VERTEX::BOTTOMRIGHT]), angle, center);
	}

	Vector2<float> Rect::TopRight() const
	{
		return Math::Rotate(center + (dim * 0.5f * m_VertexOffsets[VERTEX::TOPRIGHT]), angle, center);
	}

	Vector2<float> Rect::TopLeft() const
	{
		return Math::Rotate(center + (dim * 0.5f * m_VertexOffsets[VERTEX::TOPLEFT]), angle, center);
	}

	float Rect::Left() const
	{
		return x - width * 0.5f;
	}

	float Rect::Right() const
	{
		return x + width * 0.5f;
	}

	float Rect::Bottom() const
	{
		return y - height * 0.5f;
	}

	float Rect::Top() const
	{
		return y + height * 0.5f;
	}

	bool Rect::Contains(const Vector2<float>& point) const
	{
		// Equation from:
		// https://stackoverflow.com/questions/17136084/checking-if-a-point-is-inside-a-rotated-rectangle
		// Area = abs( (Bx * Ay - Ax * By) + (Cx * By - Bx * Cy) + (Ax * Cy - Cx * Ay) ) / 2

		static const float epsilon = 0.1f;

		std::array<Vector2<float>, 4> vertices = GetVertices();

		// Calculate triangle areas (where A, B, C, D = 0, 1, 2, 3)
		float apd = Math::TriangleArea(vertices[0], point, vertices[3]); // Triangle APD
		float dpc = Math::TriangleArea(vertices[3], point, vertices[2]); // Triangle DPC
		float cpb = Math::TriangleArea(vertices[2], point, vertices[1]); // Triangle CPB
		float pba = Math::TriangleArea(point, vertices[1], vertices[0]); // Triangle PBA

		// Sum triangle areas
		float total_area = apd + dpc + cpb + pba;

		// If sum of triangle areas is equal to the area of the rectangle, then the point is within the rectangle
		return Math::EqualBias(total_area, width * height, epsilon);
	}

	Rect::Rect()
		: x(0.0f), y(0.0f), width(0.0f), height(0.0f), angle(0.0f)
	{
	}

	Rect::Rect(float x, float y, float width, float height, float angle)
		: x(x), y(y), width(width), height(height), angle(angle)
	{}

	Rect::Rect(const Rect& other)
		: x(other.x), y(other.y), width(other.width), height(other.height), angle(other.angle)
	{}

	Rect::Rect(Rect&& other) noexcept
		: x(std::move(other.x)), y(std::move(other.y)), width(std::move(other.width)), height(std::move(other.height)), angle(std::move(other.angle))
	{}

	void Rect::Write(Serialiser& s) const
	{
		s.Write(x);
		s.Write(y);
		s.Write(width);
		s.Write(height);
		s.Write(angle);
	}

	void Rect::Read(Serialiser& s)
	{
		s.Read(&x);
		s.Read(&y);
		s.Read(&width);
		s.Read(&height);
		s.Read(&angle);
	}

	Rect::Rect(const Vector2<float>& center, const Vector2<float>& dim, float angle)
		: center(center), dim(dim), angle(angle)
	{}

	bool Rect::IsIntersecting(const Rect& rect) const
	{
		return ContainsAnyOf(rect.GetVertices()) || rect.ContainsAnyOf(GetVertices());
	}

	bool Rect::Contains(float width, float height, const std::array<Vector2<float>, 4>& my_vertices, const Vector2<float>& point)
	{
		static const float epsilon = 0.1f;

		for (const Vector2<float>& vertex : my_vertices) {
			// Calculate triangle areas (where A, B, C, D = 0, 1, 2, 3)
			float apd = Math::TriangleArea(my_vertices[0], point, my_vertices[3]); // Triangle APD
			float dpc = Math::TriangleArea(my_vertices[3], point, my_vertices[2]); // Triangle DPC
			float cpb = Math::TriangleArea(my_vertices[2], point, my_vertices[1]); // Triangle CPB
			float pba = Math::TriangleArea(point, my_vertices[1], my_vertices[0]); // Triangle PBA

			// Sum triangle areas
			float total_area = apd + dpc + cpb + pba;

			// If sum of triangle areas is equal to the area of the rectangle, then the point is within the rectangle
			if (Math::EqualBias(total_area, width * height, epsilon))
			{
				return true;
			}
		}

		return false;
	}

	Rect::Manifold Rect::GetIntersection(float width1, float height1, float width2, float height2, const std::array<Vector2<float>, 4>& my_vertices, const std::array<Vector2<float>, 4>& their_vertices)
	{
		// TODO: feels like something could be simplified

		for (int i = 0; i < 4; i++) {
			if (Rect::Contains(width1, height1, my_vertices, their_vertices[i])) {
				Manifold manifold;
				manifold.collisionOccured = true;
				manifold.intersecting_vertex = their_vertices[i];

				std::array<Vector2<float>, 4> shape_a(my_vertices);

				std::sort(
					shape_a.begin(), shape_a.end(),
					[&](const Vector2<float>& a, const Vector2<float>& b)
					{
						float a_dist = a.sqr_distance(their_vertices[i]);
						float b_dist = b.sqr_distance(their_vertices[i]);

						return a_dist < b_dist;
					}
				);

				manifold.intersecting_face_v0 = shape_a[0];
				manifold.intersecting_face_v1 = shape_a[1];
				manifold.edge_vector = manifold.intersecting_face_v0 - manifold.intersecting_face_v1;
				manifold.face_normal = Vector2<float>(manifold.edge_vector.y, -manifold.edge_vector.x).normalise();

				return manifold;
			}
		}

		for (int i = 0; i < 4; i++) {
			if (Rect::Contains(width2, height2, their_vertices, my_vertices[i])) {
				Manifold manifold;
				manifold.collisionOccured = true;
				manifold.intersecting_vertex = my_vertices[i];

				std::array<Vector2<float>, 4> shape_b(their_vertices);

				std::sort(
					shape_b.begin(), shape_b.end(),
					[&](const Vector2<float>& a, const Vector2<float>& b)
					{
						float a_dist = a.sqr_distance(my_vertices[i]);
						float b_dist = b.sqr_distance(my_vertices[i]);

						return a_dist < b_dist;
					}
				);

				manifold.intersecting_face_v0 = shape_b[0];
				manifold.intersecting_face_v1 = shape_b[1];
				manifold.edge_vector = manifold.intersecting_face_v0 - manifold.intersecting_face_v1;
				manifold.face_normal = Vector2<float>(manifold.edge_vector.y, -manifold.edge_vector.x).normalise();

				return manifold;
			}
		}

		return Manifold();
	}

	bool Rect::ContainsAnyOf(const Rect& rect) const
	{
		std::array<Vector2<float>, 4> vertices = rect.GetVertices();

		for (const Vector2<float>& v : vertices) {
			// If we contain the vertex
			if (Contains(v)) return true;
		}

		return false;
	}

	bool Rect::ContainsAnyOf(const std::array<Vector2<float>, 4>& vertices) const
	{
		for (const Vector2<float>& v : vertices) {
			// If we contain the vertex
			if (Contains(v)) return true;
		}

		return false;
	}

	bool operator==(const Rect& a, const Rect& b)
	{
		return a.x == b.x && a.y == b.y
			&& a.width == b.width && a.height == b.height
			&& a.angle == b.angle;
	}
}

bool operator==(const Vivium::Rect& a, const Vivium::Rect& b)
{
	return a.x == b.x && a.y == b.y
		&& a.width == b.width && a.height == b.height
		&& a.angle == b.angle;
}