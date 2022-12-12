#include "Quad.h"

#include "Application.h"
#include "TextureAtlas.h"

namespace Vivium {
	IndexBuffer* Quad::ib = nullptr;
	BufferLayout* Quad::layout = nullptr;
	bool Quad::s_ShouldSuppressVB = true;

	void Quad::m_Update()
	{
		VIVIUM_SCOPE;

		if (s_ShouldSuppressVB && vb != nullptr) {
			std::array<Vector2<float>, 4> vertices = GetVertices();

			float vertex_data[16] = {
				vertices[Rect::BOTTOMLEFT].x,  vertices[Rect::BOTTOMLEFT].y,  m_TexCoords[0], m_TexCoords[1],
				vertices[Rect::BOTTOMRIGHT].x, vertices[Rect::BOTTOMRIGHT].y, m_TexCoords[2], m_TexCoords[3],
				vertices[Rect::TOPRIGHT].x,    vertices[Rect::TOPRIGHT].y,    m_TexCoords[4], m_TexCoords[5],
				vertices[Rect::TOPLEFT].x,     vertices[Rect::TOPLEFT].y,     m_TexCoords[6], m_TexCoords[7]
			};

			vb->Set(&vertex_data[0], 16 * sizeof(float));
		}
	}

	void Quad::m_Construct()
	{
		VIVIUM_SCOPE;

		m_TexCoords = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};


		if (s_ShouldSuppressVB) {
			std::array<Vector2<float>, 4> vertices = GetVertices();

			float vertex_data[16] = {
				vertices[Rect::BOTTOMLEFT].x,  vertices[Rect::BOTTOMLEFT].y,  m_TexCoords[0], m_TexCoords[1],
				vertices[Rect::BOTTOMRIGHT].x, vertices[Rect::BOTTOMRIGHT].y, m_TexCoords[2], m_TexCoords[3],
				vertices[Rect::TOPRIGHT].x,    vertices[Rect::TOPRIGHT].y,    m_TexCoords[4], m_TexCoords[5],
				vertices[Rect::TOPLEFT].x,     vertices[Rect::TOPLEFT].y,     m_TexCoords[6], m_TexCoords[7]
			};

			// Create buffer for vertex coords and tex coords
			vb = std::make_shared<VertexBuffer>(vertex_data, 16, *layout);
		}
		else {
			vb = nullptr;
		}
	}

	void Quad::m_Init()
	{
		ib = new IndexBuffer(std::vector<uint8_t>{ 0, 1, 2, 2, 3, 0 });
		layout = new BufferLayout({
			GLSLDataType::VEC2, // position
			GLSLDataType::VEC2  // tex coords
		});
	}

	void Quad::m_Terminate()
	{
		delete ib;
		delete layout;
	}

	void Quad::SetCenter(float nx, float ny)
	{
		x = nx;
		y = ny;
		m_Update();
	}

	void Quad::SetCenter(const Vector2<float>& ncenter)
	{
		center = ncenter;
		m_Update();
	}

	void Quad::SetDim(float nwidth, float nheight)
	{
		width = nwidth;
		height = nheight;
		m_Update();
	}

	void Quad::SetDim(const Vector2<float>& ndim)
	{
		dim = ndim;
		m_Update();
	}

	void Quad::SetX(float nx)
	{
		x = nx;
		m_Update();
	}

	void Quad::SetY(float ny)
	{
		y = ny;
		m_Update();
	}

	void Quad::SetWidth(float nwidth)
	{
		width = nwidth;
		m_Update();
	}

	void Quad::SetHeight(float nheight)
	{
		height = nheight;
		m_Update();
	}

	void Quad::SetAngle(float nangle)
	{
		angle = nangle;
		m_Update();
	}

	void Quad::SetRect(const Rect& nrect)
	{
		x = nrect.x; y = nrect.y;
		width = nrect.width; height = nrect.height;
		angle = nrect.angle;
		m_Update();
	}

	std::vector<float>* Quad::GetTexCoords()
	{
		return &m_TexCoords;
	}

	bool Quad::ContainsAnyOf(const Quad& quad) const
	{
		return ContainsAnyOf(quad.GetVertices());
	}

	bool Quad::IsIntersecting(const Quad& quad) const
	{
		return ContainsAnyOf(quad.GetVertices()) || quad.ContainsAnyOf(GetVertices());
	}

	void Quad::Write(Serialiser& s) const
	{
		s.Write(x);
		s.Write(y);
		s.Write(width);
		s.Write(height);
		s.Write(angle);
	}

	void Quad::Read(Serialiser& s)
	{
		s.Read(&x);
		s.Read(&y);
		s.Read(&width);
		s.Read(&height);
		s.Read(&angle);

		m_Construct();
	}

	Quad& Quad::operator=(const Quad& other) {
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;
		angle = other.angle;

		vb = other.vb;

		return *this;
	}

	Quad& Quad::operator=(Quad&& other) noexcept {
		x = std::move(other.x);
		y = std::move(other.y);
		width = std::move(other.width);
		height = std::move(other.height);
		angle = std::move(other.angle);

		vb = std::move(other.vb);

		return *this;
	}

	const std::shared_ptr<VertexBuffer> Quad::GetVertexBuffer() const
	{
		return vb;
	}

	const IndexBuffer* Quad::GetIndexBuffer()
	{
		return ib;
	}

	void Quad::SetVBSuppression(bool value)
	{
		s_ShouldSuppressVB = value;
	}

	Quad::Quad()
		: m_TexCoords()
	{
		m_Construct();
	}

	Quad::Quad(float x, float y, float width, float height, float angle)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->angle = angle;

		m_Construct();
	}

	Quad::Quad(const Vector2<float>& center, const Vector2<float>& dim, float angle)
	{
		this->center = center;
		this->dim = dim;
		this->angle = angle;

		m_Construct();
	}

	Quad::Quad(float x, float y, float width, float height, const std::vector<float>& tex_coords, float angle)
		: m_TexCoords(tex_coords)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->angle = angle;

		m_Construct();
	}

	Quad::Quad(const Vector2<float>& center, const Vector2<float>& dim, const std::vector<float>& tex_coords, float angle)
		: m_TexCoords(tex_coords)
	{
		this->center = center;
		this->dim = dim;
		this->angle = angle;

		m_Construct();
	}

	Quad::Quad(const Rect& rect)
	{
		x = rect.x; y = rect.y;
		width = rect.width; height = rect.height;
		angle = rect.angle;

		m_Construct();
	}

	Quad::Quad(const Quad& other)
		: vb(other.vb), m_TexCoords(other.m_TexCoords)
	{
		x = other.x; y = other.y;
		width = other.width; height = other.height;
		angle = other.angle;
	}

	Quad::Quad(Quad&& other) noexcept
		: vb(std::move(other.vb))
	{
		x = std::move(other.x);
		y = std::move(other.y);
		width = std::move(other.width);
		height = std::move(other.height);
		angle = std::move(other.angle);
	}

	Quad::~Quad()
	{}

	Vector2<float> Quad::GetCenter() const
	{
		return center;
	}

	Vector2<float> Quad::GetDim() const
	{
		return dim;
	}

	float Quad::GetX() const
	{
		return x;
	}

	float Quad::GetY() const
	{
		return y;
	}

	float Quad::GetWidth() const
	{
		return width;
	}

	float Quad::GetHeight() const
	{
		return height;
	}

	float Quad::GetAngle() const
	{
		return angle;
	}

	Rect Quad::GetRect() const
	{
		return Rect(x, y, width, height, angle);
	}

	bool operator==(const Quad& a, const Quad& b)
	{
		return a.x == b.x && a.y == b.y &&
			a.width == b.width && a.height == b.height &&
			a.angle == b.angle;
	}
}