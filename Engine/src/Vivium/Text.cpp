#include "Text.h"

namespace Vivium {
    std::shared_ptr<Shader> Text::m_DefaultShader = nullptr;
    std::shared_ptr<Font> Text::m_DefaultFont = nullptr;

    void Text::m_Construct(const std::string& input_str)
    {
        // Splitting string by newlines, since they will need to be rendered on different y
        strings = Utils::SplitString(input_str, "\n");

        m_FontTexture = std::make_shared<Vivium::Texture>(m_DefaultFont.get());
        m_FontTextureAtlas = std::make_shared<Vivium::TextureAtlas>(m_FontTexture, Vector2<int>{64, 64});
    }

    void Text::m_Init()
    {
        m_DefaultShader = std::make_shared<Shader>("text_vertex", "text_frag");
        m_DefaultFont = std::make_shared<Font>("consola.ttf");
    }

    float Text::m_GetWidth(const std::string& str) const
    {
        float width = 0.0f;

        // Iterate each character
        for (char c : str) {
            // Get character data for font
            Font::Character font_char = font->character_map.at(c);
            // Get width (we're including spacing so we have to use the advance) and add to our running total
            width += float(font_char.advance >> 6) * scale;
        }

        return width;
    }

    std::shared_ptr<Font> Text::GetDefaultFont()
    {
        if (m_DefaultFont == nullptr) {
            LogWarn("Default font not initialised yet");
        }

        return m_DefaultFont;
    }

    void Text::SetPos(const Vector2<float>& pos)
    {
        *this->pos = pos;
    }

    Vector2<float> Text::GetPos() const
    {
        return *pos;
    }

    void Text::FixToWidth(float max_width)
    {
        std::vector<std::string> new_strings; // Each entry is a string on a new line
        new_strings.emplace_back(""); // Empty string we're gonna start appending to

        int current_new_string_index = 0;

        // Running width of the string/line we're currently appending to
        float current_width = 0.0f;

        for (std::size_t str_idx = 0; str_idx < strings.size(); str_idx++) {
            std::string& current_string = strings[str_idx];

            for (std::size_t char_idx = 0; char_idx < current_string.size(); char_idx++) {
                char& current_char = current_string[char_idx];

                Font::Character& font_char = font->character_map.at(current_char);

                float char_width = float(font_char.advance >> 6) * scale;

                current_width += char_width;

                // The character we're about to add will be out of the bounds
                if (current_width >= max_width) {
                    new_strings.emplace_back(1, current_char);
                    ++current_new_string_index;
                    current_width = char_width;
                }
                // Append to current string/line
                else {
                    new_strings[current_new_string_index] += current_char;
                }
            }
        }

        strings.clear();
        strings = new_strings;
    }

    float Text::GetWidth() const
    {
        float max_width = 0.0f;

        for (const std::string& str : strings) {
            max_width = std::max(max_width, m_GetWidth(str));
        }

        return max_width;
    }

    float Text::GetHeight() const
    {
        return strings.size() * font->max_height * scale;
    }

    std::string Text::GetText() const
    {
        // Join all the strings into one
        // TODO: end_str.reserve or something like that?
        std::string end_str;
        for (const std::string& str : strings) {
            end_str += str;
        }

        return end_str;
    }

    const std::shared_ptr<TextureAtlas> Text::GetAtlas() const { return m_FontTextureAtlas; }
    const std::shared_ptr<Texture> Text::GetTexture() const { return m_FontTexture; }

    void Text::SetText(const std::string& str)
    {
        strings.clear();
        strings = Utils::SplitString(str, "\n");
    }

    void Text::Render() const
    {
        std::size_t total_char_count = 0;

        for (const std::string& str : strings) {
            total_char_count += str.size();
        }

        static const BufferLayout layout{
            GLSLDataType::VEC4
        };

        Batch batch(total_char_count, &layout);

        Submit(&batch);

        Batch::RenderData data = batch.End();

        if (data.count > 0) {
            Renderer::Submit(data.vertex_buffer.get(), data.index_buffer.get(), shader.get(), m_FontTexture.get());
        }
    }

    void Text::Submit(Batch* batch) const
    {
        float height = GetHeight();
        float width = GetWidth();

        Vector2<float> rendering_pos = *pos;
        rendering_pos.y += (height - (font->max_height * scale)) * 0.5f;

        for (const std::string& str : strings) {
            if (alignment == Alignment::CENTER) {
                float line_width = m_GetWidth(str);

                rendering_pos.x = pos->x - line_width * 0.5f;
            }
            else {
                rendering_pos.x = pos->x; // - width * 0.5f
            }

            for (char c : str) {
                Font::Character& font_char = font->character_map.at(c);

                float x = rendering_pos.x + font_char.bearing.x * scale;
                float y = rendering_pos.y - (font_char.size.y - font_char.bearing.y) * scale;
                float w = font_char.size.x * scale;
                float h = font_char.size.y * scale;

                std::array<float, 8> tex_coords = m_FontTextureAtlas->GetCoordsArray(c);

                // Texture coordinates for character
                float left = tex_coords[0];
                float top = tex_coords[1];
                float right = left + ((float)font_char.size.x / (float)m_FontTextureAtlas->GetAtlas()->GetWidth());
                float bottom = top + ((float)font_char.size.y / (float)m_FontTextureAtlas->GetAtlas()->GetHeight());

                batch->Submit(x, x + w, y, y + h, left, right, bottom, top);

                rendering_pos.x += float(font_char.advance >> 6) * scale;
            }

            rendering_pos.y -= font->max_height * scale;
        }
    }

    Text::Text()
        : pos(), font(nullptr), shader(nullptr), scale(0.0f)
    {
        LogWarn("Default constructor called! Values will not be properly initialised");
        m_Construct("");
    }

    Text::Text(const Text& other)
        : strings(other.strings), pos(other.pos), font(other.font), shader(other.shader), scale(other.scale), alignment(other.alignment)
    {
        
    }

    Text::Text(const std::string& text, const Vector2<float>& pos, const Alignment& alignment, float scale)
        : pos(std::make_shared<Vector2<float>>(pos)), font(m_DefaultFont), shader(m_DefaultShader), scale(scale), alignment(alignment)
    {
        m_Construct(text);
    }

    Text::Text(const std::string& text, const Vector2<float>& pos, std::shared_ptr<Font> font, const Alignment& alignment, float scale)
        : pos(std::make_shared<Vector2<float>>(pos)), font(font), shader(m_DefaultShader), scale(scale), alignment(alignment)
    {
        m_Construct(text);
    }

    Text::Text(const std::string& text, const Vector2<float>& pos, std::shared_ptr<Shader> shader, const Alignment& alignment, float scale)
        : pos(std::make_shared<Vector2<float>>(pos)), font(m_DefaultFont), shader(shader), scale(scale), alignment(alignment)
    {
        m_Construct(text);
    }

    Text::Text(const std::string& text, const Vector2<float>& pos, std::shared_ptr<Font> font, std::shared_ptr<Shader> shader, const Alignment& alignment, float scale)
        : pos(std::make_shared<Vector2<float>>(pos)), font(font), shader(shader), scale(scale), alignment(alignment)
    {
        m_Construct(text);
    }

    Text::~Text()
    {
    }
}