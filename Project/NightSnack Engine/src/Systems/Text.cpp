#include "Text.h"
#include "Graphics.h"

namespace NightSnack {

    void TextSystem::Init() {
        std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();

        // Use the Resource Manager to load the fonts
        size_t fontKeyDefault = 0;
        rm->LoadFont("assets/Font/buggy-font.ttf", fontKeyDefault);
        m_fonts["default"] = fontKeyDefault;

        size_t fontKeyLiberation = 0;
        rm->LoadFont("assets/Font/liberation-mono.ttf", fontKeyLiberation);
        m_fonts["liberation"] = fontKeyLiberation;

        LoadFontGlyphs(fontKeyDefault, "default");
        LoadFontGlyphs(fontKeyLiberation, "liberation");

        // Configure VAO/VBO for texture quads
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        auto& shaders = m_coord.GetSystem<GraphicsSystem>()->GetShadersMap();
        m_shader = shaders.find("text")->second.get();
    }

    void TextSystem::LoadFontGlyphs(size_t fontKey, const std::string& name) {
        std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();

        // Get the face for the respective font
        FT_Face face = rm->GetFont(fontKey);

        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);
        // Disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load first 128 characters of ASCII set only
        for (unsigned char c = 0; c < 128; ++c) {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "Error: Failed to load Glyph\n";
                continue;
            }
            // Generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED,
                face->glyph->bitmap.width, face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            m_fontCharacters[name][c] = character;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TextSystem::Update(int steps) {
        (void)steps;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (Entity e : m_entities) {
            auto& pos = m_coord.GetComponent<Transform>(e).pos;
            auto& txt = m_coord.GetComponent<Font>(e);

            // Get camera matrices
            Entity mainCam = m_coord.GetSystem<GraphicsSystem>()->FindCamera();
            if (mainCam == INVALID_ENTITY) return;

            auto& camPos = m_coord.GetComponent<Transform>(mainCam).pos;
            auto& camInfo = m_coord.GetComponent<Camera>(mainCam);

            glm::mat4 proj = m_coord.GetSystem<GraphicsSystem>()->GetCameraProjectionMatrix(camInfo.width, camInfo.height, 1.f);
            glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, -camPos.z));

            RenderText(txt.text, pos.x, pos.y, txt.fontSize, glm::vec3{ txt.textColor.x, txt.textColor.y, txt.textColor.z }, proj, view, txt.fontType);
        }

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    void TextSystem::RenderText(std::string text, float x, float y, float scale, glm::vec3 color, const glm::mat4& proj, const glm::mat4& view, const std::string& fontName)
    {
        if (m_fontCharacters.find(fontName) == m_fontCharacters.end()) {
            std::cout << "ERROR: Font '" << fontName << "' not found!" << std::endl;
            return;
        }

        auto& chars = m_fontCharacters[fontName];
        
        m_shader->Use();
        m_shader->SetMat4("proj", proj);
        m_shader->SetMat4("view", view);
        m_shader->SetVec3("uTextColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_vao);

        // iterate through all characters
        for (char c : text) {
            auto& ch = chars[c];

            float xpos = x + ch.bearing.x * scale;
            float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}