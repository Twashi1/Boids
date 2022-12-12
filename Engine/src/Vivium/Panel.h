#pragma once

#include "Logger.h"
#include "Quad.h"
#include "Slider.h"
#include "Button.h"
#include "TextInput.h"
#include "Text.h"
#include "Sprite.h"

namespace Vivium {
	class VIVIUM_API Panel {
	public:
		enum class VIVIUM_API ANCHOR : uint8_t {
			LEFT, RIGHT,
			BOTTOM, TOP,
			CENTER
		};

	private:
		struct VIVIUM_API Data {
		private:
			enum class VIVIUM_API Type : uint8_t {
				PANEL,
				QUAD,
				POINT
			};

			Type m_Type;

		public:
			union {
				std::weak_ptr<Panel> panel;
				std::weak_ptr<Quad> quad;
				std::weak_ptr<Vector2<float>> point;
			};

			Vector2<float> last_pos;
			Vector2<float> offset;

			// TODO implement for slider, button, etc.
			void SetPos(const Vector2<float>& pos);
			Vector2<float> GetPos() const;

			// Assuming object passed has position relative to anchor coordinates
			Data(ANCHOR x, ANCHOR y, std::shared_ptr<Quad> quad, const Vector2<float>& offset = Vector2<float>(FLT_MAX, FLT_MAX));
			Data(ANCHOR x, ANCHOR y, std::shared_ptr<Vector2<float>> point, const Vector2<float>& offset = Vector2<float>(FLT_MAX, FLT_MAX));
			Data(ANCHOR x, ANCHOR y, std::shared_ptr<Panel> panel, const Vector2<float>& offset = Vector2<float>(FLT_MAX, FLT_MAX));
			Data(const Data& other);
			~Data();

			Data& operator=(const Data& other);

			ANCHOR x_anchor, y_anchor;

			friend Panel;
		};

		// All quads/points we're editing
		std::vector<Data*> m_PanelObjects;

		Vector2<float> m_OldDim; // Last update dimensions of m_Quad
		Vector2<float> m_OldPos; // Last update position of m_Quad
		std::shared_ptr<Quad> m_Quad; // Position and dimension of panel

		int m_ScrollYMax; // Maximum y scroll distance
		int m_ScrollYMin; // Minimum y scroll distance

		bool m_VerifyObject(Data& panel_object);
		void m_Update(Data& panel_object, const Rect& new_rect);

	public:
		// Set panel coordinates/size
		Panel(const Quad& panel_quad, int scroll_max = INT_MAX, int scroll_min = INT_MIN);
		Panel(std::shared_ptr<Quad> panel_quad, int scroll_max = INT_MAX, int scroll_min = INT_MIN);
		~Panel();

		void SetPos(const Vector2<float>& new_pos);
		void SetDim(const Vector2<float>& new_dim);
		void SetRect(const Rect& new_rect);

		void SetScrollLimits(float y_max, float y_min);

		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Quad> quad);
		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Vector2<float>> point);
		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Panel> panel);

		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Button> button);
		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Slider> slider);
		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<TextInput> text_input);
		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Text> text);
		void Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Sprite> sprite);

		// For moving/resizing panels (although this requires the panel to be drawn through something)
		void Update();

		// Get quad for debug rendering
		const std::shared_ptr<Quad> GetQuad() const;
	};
}