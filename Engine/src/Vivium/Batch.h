#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vector2.h"

// TODO: system for sending raw vertices without indices?
// TODO: specification
// TODO: per buffer element data submission

namespace Vivium {
	class Batch;

	class Batchable {
	public:
		virtual void Submit(Batch* batch) const = 0;
	};

	template <typename T> concept IsBatchable = std::is_base_of_v<Batchable, T>;

	class VIVIUM_API Batch {
	private:
		float* m_Vertices;		// Vertices and vertex data
		uint16_t* m_Indices;	// All vertex indices for index buffer

		std::size_t m_VerticesIndex = 0;
		std::size_t m_IndicesIndex = 0;

		std::size_t m_Count = 0; // Total **shape** count
		
		const std::size_t m_MaxCount; // Max shapes
		const BufferLayout* m_Layout;

	public:
		struct VIVIUM_API RenderData {
			// TODO: Isn't necessary for shared ptr?
			std::shared_ptr<VertexBuffer> vertex_buffer;
			std::shared_ptr<IndexBuffer>  index_buffer;
			std::size_t count;

			RenderData();

			RenderData(const RenderData& other);
			RenderData(RenderData&& other) noexcept;

			inline bool IsValid()  { return count > 0; }
			inline operator bool() { return IsValid(); }
		};

		Batch(const std::size_t& max_count, const BufferLayout* layout);
		
		void Submit(const Vector2<float>& pos, const Vector2<float>& size);
		void Submit(const Vector2<float>& pos, const Vector2<float>& size, const float tex_coords[8]);
		void Submit(const Vector2<float>& pos, const Vector2<float>& size, float tex_left, float tex_right, float tex_bottom, float tex_top);
		void Submit(float left, float right, float bottom, float top, float tex_left, float tex_right, float tex_bottom, float tex_top);
		void Submit(const Vector2<float>& pos, const Vector2<float>& size, float tex_left, float tex_right, float tex_bottom, float tex_top, float* vertex_data, unsigned int vertex_data_count);
		void Submit(float* data, std::size_t count);

		template <typename T> requires IsBatchable<T>
		void Submit(const T* batchable_object)
		{
			batchable_object->Submit(this);
		}

		RenderData End() const;

		~Batch();

		friend Batchable;
	};
}
