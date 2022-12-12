#pragma once

#include "Logger.h"
#include "Vector2.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Rect.h"
#include "Texture.h"
#include "Serialiser.h"

namespace Vivium {
	class Application;
	class TextureAtlas;

	struct VIVIUM_API Quad : protected Rect, public Streamable {
	private:
		std::vector<float> m_TexCoords; // TODO: probably not working

		// TODO: names
		static IndexBuffer* ib;
		static BufferLayout* layout;
		static bool s_ShouldSuppressVB;

		// Buffers
		// TODO: name, unique?
		std::shared_ptr<VertexBuffer> vb;

		// Update vertex buffer
		void m_Update();

		// Construct buffers
		void m_Construct();

		static void m_Init();
		static void m_Terminate();

	public:
		// Make methods public
		using Rect::Contains;
		using Rect::ContainsAnyOf;
		using Rect::BottomLeft;
		using Rect::BottomRight;
		using Rect::TopRight;
		using Rect::TopLeft;
		using Rect::GetVertices;
		using Rect::Left;
		using Rect::Right;
		using Rect::Bottom;
		using Rect::Top;

		const std::shared_ptr<VertexBuffer> GetVertexBuffer() const;
		static const IndexBuffer* GetIndexBuffer();

		static void SetVBSuppression(bool value);

		Quad();
		Quad(float x, float y, float width, float height, float angle = 0.0f);
		Quad(const Vector2<float>& center, const Vector2<float>& dim, float angle = 0.0f);
		Quad(float x, float y, float width, float height, const std::vector<float>& tex_coords, float angle = 0.0f);
		Quad(const Vector2<float>& center, const Vector2<float>& dim, const std::vector<float>& tex_coords, float angle = 0.0f);
		Quad(const Rect& rect);
		Quad(const Quad& other);
		Quad(Quad&& other) noexcept;
		~Quad();

		Quad& operator=(const Quad& other);
		Quad& operator=(Quad&& other) noexcept;

		Vector2<float> GetCenter() const;
		Vector2<float> GetDim() const;

		float GetX() const;
		float GetY() const;
		float GetWidth() const;
		float GetHeight() const;
		float GetAngle() const;

		Rect GetRect() const;

		void SetCenter(const Vector2<float>& ncenter);
		void SetCenter(float nx, float ny);
		void SetDim(const Vector2<float>& ndim);
		void SetDim(float nwidth, float nheight);

		void SetX(float nx);
		void SetY(float ny);
		void SetWidth(float nwidth);
		void SetHeight(float nheight);
		void SetAngle(float nangle);

		void SetRect(const Rect& nrect);

		std::vector<float>* GetTexCoords();

		bool ContainsAnyOf(const Quad& quad) const;
		bool IsIntersecting(const Quad& quad) const;

		void Write(Serialiser& s) const override;
		void Read(Serialiser& s) override;

		friend bool operator==(const Quad& a, const Quad& b);

		friend Application;
		friend TextureAtlas;
	};

	bool operator==(const Quad& a, const Quad& b);
}
