#include "Panel.h"

namespace Vivium {
	void Panel::Data::SetPos(const Vector2<float>& pos)
	{
		switch (m_Type) {
		case Type::QUAD:
			quad.lock()->SetCenter(pos); break;
		case Type::POINT:
		{
			auto locked = point.lock();
			locked->x = pos.x; locked->y = pos.y;
		} break;
		case Type::PANEL:
			panel.lock()->SetPos(pos); break;
		}

		last_pos = pos;
	}

	Vector2<float> Panel::Data::GetPos() const
	{
		Vector2<float> pos;

		switch (m_Type) {
		case Type::QUAD:
			pos = quad.lock()->GetCenter(); break;
		case Type::POINT:
		{
			auto locked = point.lock();
			pos = Vector2<float>(locked->x, locked->y);
		}  break;
		case Type::PANEL:
			pos = panel.lock()->GetQuad()->GetCenter(); break;
		}

		return pos;
	}

	Panel::Data::Data(ANCHOR x, ANCHOR y, std::shared_ptr<Quad> quad, const Vector2<float>& offset)
		: x_anchor(x), y_anchor(y), quad(quad), m_Type(Type::QUAD), offset(offset)
	{
		if (offset.x == FLT_MAX) {
			this->offset.x = quad->GetX();
		}

		if (offset.y == FLT_MAX) {
			this->offset.y = quad->GetY();
		}

		last_pos = this->offset;
	}

	Panel::Data::Data(ANCHOR x, ANCHOR y, std::shared_ptr<Vector2<float>> point, const Vector2<float>& offset)
		: x_anchor(x), y_anchor(y), point(point), m_Type(Type::POINT), offset(offset)
	{
		if (offset.x == FLT_MAX) {
			this->offset.x = point->x;
		}

		if (offset.y == FLT_MAX) {
			this->offset.y = point->y;
		}

		last_pos = this->offset;
	}

	Panel::Data::Data(ANCHOR x, ANCHOR y, std::shared_ptr<Panel> panel, const Vector2<float>& offset)
		: x_anchor(x), y_anchor(y), panel(panel), m_Type(Type::PANEL), offset(offset)
	{
		if (offset.x == FLT_MAX) {
			this->offset.x = panel->GetQuad()->GetX();
		}

		if (offset.y == FLT_MAX) {
			this->offset.y = panel->GetQuad()->GetY();
		}

		last_pos = this->offset;
	}

	Panel::Data::Data(const Data& other)
		: x_anchor(other.x_anchor), y_anchor(other.y_anchor), m_Type(other.m_Type)
	{
		switch (m_Type) {
		case Type::PANEL: panel = other.panel; break;
		case Type::QUAD: quad = other.quad; break;
		case Type::POINT: point = other.point; break;
		}
	}

	Panel::Data::~Data()
	{}

	Panel::Data& Panel::Data::operator=(const Data& other)
	{
		x_anchor = other.x_anchor; y_anchor = other.y_anchor;
		m_Type = other.m_Type;

		switch (m_Type) {
		case Type::PANEL: panel = other.panel; break;
		case Type::QUAD: quad = other.quad; break;
		case Type::POINT: point = other.point; break;
		}

		return *this;
	}

	void Panel::Update()
	{
		VIVIUM_SCOPE;

		float scroll = Input::GetYScroll();
		if (scroll == 0.0f) return;
		Vector2<float> cursor = Input::GetCursorPos();

		Vector2<float> current_pos = m_Quad->GetCenter();
		float half_height = m_Quad->GetHeight() * 0.5f;

		float new_y = current_pos.y - scroll;

		if (m_ScrollYMax == INT_MAX || m_ScrollYMin == INT_MIN) {
			return;
		}

		if (new_y <= m_ScrollYMin && new_y >= m_ScrollYMax) {
			SetPos({ current_pos.x, new_y });
		}
	}

	const std::shared_ptr<Quad> Panel::GetQuad() const { return m_Quad; }

	bool Panel::m_VerifyObject(Data& panel_object)
	{
		VIVIUM_SCOPE;

		switch (panel_object.m_Type) {
		case Data::Type::PANEL: return !panel_object.panel.expired();
		case Data::Type::QUAD: return !panel_object.quad.expired();
		case Data::Type::POINT: return !panel_object.point.expired();
		default:
			LogWarn("Invalid data type"); return false;
		}
	}

	void Panel::m_Update(Data& panel_object, const Rect& new_rect)
	{
		VIVIUM_SCOPE;

		if (m_VerifyObject(panel_object)) {
			// Find difference between current and last position to find change in offset, then add to old offset to get new offset
			Vector2<float> current_offset = panel_object.GetPos() - panel_object.last_pos + panel_object.offset;
			// Update new offset
			panel_object.offset = current_offset;

			Vector2<float> new_pos = current_offset;

			// Do calculations to fix to side
			switch (panel_object.x_anchor) {
			case ANCHOR::LEFT:
				new_pos.x += m_Quad->Left(); break;
			case ANCHOR::RIGHT:
				new_pos.x += m_Quad->Right(); break;
			case ANCHOR::CENTER:
				new_pos.x += m_Quad->GetX(); break;
			}

			switch (panel_object.y_anchor) {
			case ANCHOR::BOTTOM:
				new_pos.y += m_Quad->Bottom(); break;
			case ANCHOR::TOP:
				new_pos.y += m_Quad->Top(); break;
			case ANCHOR::CENTER:
				new_pos.y += m_Quad->GetY(); break;
			}

			// Set pos of panel object
			panel_object.SetPos(new_pos);
		}
	}

	Panel::Panel(std::shared_ptr<Quad> quad, int scroll_max, int scroll_min)
		: m_Quad(quad), m_OldPos(quad->GetCenter()), m_OldDim(quad->GetDim()), m_ScrollYMax(scroll_max), m_ScrollYMin(scroll_min)
	{}

	Panel::Panel(const Quad& quad, int scroll_max, int scroll_min)
		: m_Quad(std::make_shared<Quad>(quad)), m_OldPos(quad.GetCenter()), m_OldDim(quad.GetDim()), m_ScrollYMax(scroll_max), m_ScrollYMin(scroll_min)
	{}

	Panel::~Panel()
	{
		for (Data* data : m_PanelObjects) {
			delete data;
		}
	}

	void Panel::SetScrollLimits(float y_max, float y_min)
	{
		float halfheight = m_Quad->GetHeight() * 0.5f;
		m_ScrollYMax = halfheight + y_max;
		m_ScrollYMin = halfheight + y_min;
	}

	void Panel::SetPos(const Vector2<float>& new_pos)
	{
		VIVIUM_SCOPE;

		m_Quad->SetCenter(new_pos);

		for (Data* panel_object : m_PanelObjects) {
			m_Update(*panel_object, Rect(new_pos, m_OldDim));
		}

		m_OldPos = new_pos;
	}

	void Panel::SetDim(const Vector2<float>& new_dim)
	{
		VIVIUM_SCOPE;

		m_Quad->SetDim(new_dim);

		for (Data* panel_object : m_PanelObjects) {
			m_Update(*panel_object, Rect(m_OldPos, new_dim));
		}

		m_OldDim = new_dim;
	}

	void Panel::SetRect(const Rect& new_rect)
	{
		VIVIUM_SCOPE;

		m_Quad->SetRect(new_rect);

		for (Data* panel_object : m_PanelObjects) {
			m_Update(*panel_object, new_rect);
		}

		m_OldDim = new_rect.dim;
		m_OldPos = new_rect.center;
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Quad> quad)
	{
		Data* new_data = new Data(x, y, quad);
		m_Update(*new_data, m_Quad->GetRect());
		m_PanelObjects.push_back(new_data);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Vector2<float>> point)
	{
		Data* new_data = new Data(x, y, point);
		m_Update(*new_data, m_Quad->GetRect());
		m_PanelObjects.push_back(new_data);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Panel> panel)
	{
		Data* new_data = new Data(x, y, panel);
		m_Update(*new_data, m_Quad->GetRect());
		m_PanelObjects.push_back(new_data);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Button> button)
	{
		Anchor(x, y, button->quad);
		Anchor(x, y, button->text);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Slider> slider)
	{
		Anchor(x, y, slider->m_BarQuad);
		Anchor(x, y, slider->m_SliderQuad);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Text> text)
	{
		Anchor(x, y, text->pos);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<TextInput> text_input)
	{
		Anchor(x, y, text_input->m_Text);
		Anchor(x, y, text_input->quad);
	}

	void Panel::Anchor(ANCHOR x, ANCHOR y, std::shared_ptr<Sprite> sprite)
	{
		Anchor(x, y, sprite->quad);
	}
}