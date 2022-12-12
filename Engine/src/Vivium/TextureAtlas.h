#pragma once

#include "Core.h"
#include "Texture.h"
#include "Quad.h"

namespace Vivium {
	// Allows easy access to texture coordinates of a specific texture, or textures in an atlas
	// Top left corner is (0, 0); increasing as you go down and to the right
	class VIVIUM_API TextureAtlas {
	private:
		std::shared_ptr<Texture> m_Atlas;
		Vector2<int> m_SpriteSize;		 // Size in px of each sprite
		Vector2<int> m_AtlasDimRelative; // Size of atlas in terms of the amount of sprites on x and y

		void m_Construct();

	public:
		// TODO: Create
		struct VIVIUM_API Index {
		private:
			// TODO: top left, bottom right
			Vector2<int> vec;
			int val;

		public:
			Index(int index, TextureAtlas* atlas);
			Index(const Vector2<int>& index, TextureAtlas* atlas);

			operator int() const { return val; }
			operator Vector2<int>() const { return vec; }

			Vector2<int> GetVector() const;
			int GetIndex() const;

			friend TextureAtlas;
		};

		std::vector<float> GetCoords(int index) const;
		std::vector<float> GetCoords(const Vector2<int>& index) const;
		std::vector<float> GetCoords(const Vector2<int>& top_left, const Vector2<int>& bottom_right) const;
		std::array<float, 8> GetCoordsArray(int index) const;
		std::array<float, 8> GetCoordsArray(const Vector2<int>& index) const;
		std::array<float, 8> GetCoordsArray(const Vector2<int>& top_left, const Vector2<int>& bottom_right) const;

		Vector2<int> GetVectorIndex(int index) const;
		int GetIndex(const Vector2<int>& vec) const;

		const std::shared_ptr<Texture> GetAtlas() const;
		Vector2<int> GetSpriteSize() const;

		void Set(Quad* quad, int index) const;
		void Set(Quad* quad, const Vector2<int>& index) const;
		void Set(Quad* quad, const Vector2<int>& top_left, const Vector2<int>& bottom_right) const;

		TextureAtlas(std::shared_ptr<Texture> atlas, const Vector2<int>& sprite_size);
		TextureAtlas(const Texture& atlas, const Vector2<int>& sprite_size);
		TextureAtlas(const std::string& atlas_path, const Vector2<int>& sprite_size);
	};
}
