/******************************************************************************
 * File:        Text.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Text System
 *
 * Description:
 * This file contains the definitions of the Text System.
 * This Text System is in charge of initializing OpenGL buffers for text rendering.
 * Loading of font files using the ResourceManager.
 * Preparing the glyph textures for ASCII characters.
 * Rendering of text belonging to entities in in-game and UI layers.
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Text.h"
#include "Graphics.h"
#include "Physics/Physics.h"
#include "Factory/EntityFactory.h"
#include "Systems/PlayerController.h"

namespace NightSnack {

	void TextSystem::Init() {
		// Configure VAO/VBO for rendering character quads
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Only will use the text shader file from the Graphics System
		auto& shaders = m_coord.GetSystem<GraphicsSystem>()->GetShadersMap();
		m_shader = shaders.find("text")->second.get();

		LoadFonts();
	}

	void TextSystem::LoadFonts() {
		std::filesystem::path assetsFilePath = std::filesystem::canonical(
			RESOURCE_PATH + std::string{ "Font" }
		);

		if (!std::filesystem::exists(assetsFilePath)) {
			std::cerr << "[TextSystem] Error: Assets Font directory cannot be found!\n";
			return;
		}

		// Iterate through entries in directory
		for (auto const& entry : std::filesystem::directory_iterator(assetsFilePath)) {
			// Skip through folders
			if (entry.is_directory()) {
				continue;
			}

			// Only load the fonts that are in this assets folder
			std::string extension = entry.path().extension().string();

			// Only load fonts with the correct extensions
			if (extension != ".ttf") {
				continue;
			}
			std::string filePath = std::string{ "Font/" } + entry.path().filename().string();

			CreateFont(filePath);
		}
	}

	void TextSystem::CreateFont(const std::string& fontFilePath) {
		std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();

		// Use the Resource Manager to load the fonts
		size_t fontKey = 0;
		rm->LoadFont(fontFilePath, fontKey);

		// Generate glyphs for the loaded fonts
		std::filesystem::path font = fontFilePath;
		LoadFontGlyphs(fontKey, font.stem().string());
	}

	void TextSystem::UpdateFont(const std::string& newFontFilePath, Entity e) {

		// Check whether the font exists first
		if (m_fontCharacters.find(newFontFilePath) == m_fontCharacters.end()) {
			// Create the new font if it does not exist yet
			CreateFont(newFontFilePath);
		}

		// Change the font type accordingly
		auto& font = m_coord.ModifyComponent<Font>(e);
		font.fontType = newFontFilePath;
	}

	void TextSystem::LoadFontGlyphs(size_t fontKey, const std::string& name) {
		std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();

		// Get the face for the respective font
		FT_Face face = rm->GetFont(fontKey);

		// Set the glyph pixel size (height = 48)
		FT_Set_Pixel_Sizes(face, 0, 48);
		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Load first 128 characters of ASCII set only
		for (unsigned char c = 0; c < 128; ++c) {
			// Load and render character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				//std::cout << "Error: Failed to load Glyph\n";
				continue;
			}
			// Generate texture from glyph bitmap
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RED,
				face->glyph->bitmap.width, face->glyph->bitmap.rows,
				0, GL_RED, GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// Configure texture parameters for text rendering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Store character information
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

	void TextSystem::Update() {
		// Find the main camera and render accordingly
		Entity mainCam = m_coord.GetSystem<GraphicsSystem>()->FindCamera();
		if (mainCam == INVALID_ENTITY) return;
		Draw(mainCam, true);
	}

	void TextSystem::Draw(Entity camera, bool isGameCamera) {
		(void)isGameCamera; // To be used in the future, to differentiate game scene and level editor

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto& camPos = m_coord.GetComponent<Transform>(camera).pos;
		auto& camInfo = m_coord.GetComponent<Camera>(camera);

		// Get the updated window resolution from the ApplicationSystem first
		float screenW = static_cast<float>(m_coord.GetSystem<ApplicationSystem>()->GetWidth());
		float screenH = static_cast<float>(m_coord.GetSystem<ApplicationSystem>()->GetHeight());

		// Default proj and view matrix for GAMELAYER entities
		glm::mat4 proj = m_coord.GetSystem<GraphicsSystem>()->GetCameraProjectionMatrix(camInfo.width, camInfo.height, camInfo.zoom);
		glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, -camPos.z));

		// Currently, only iterating through GAMELAYER and GAMEUI layers
		for (Layer& l : m_layers) {
			if (l.Empty() || !l.IsVisible() || !l.IsActive()) { continue; }
			for (Entity e : l.Entities()) {
				// Reached the end of Valid Entities
				if (e == INVALID_ENTITY) { break; }
				if (!m_coord.GetComponent<Transform>(e).isActive) { continue; }

				// Displaying of player's text
				if (m_coord.HasComponent<PlayerController>(e)) {
					std::string playerText = m_coord.GetComponent<Font>(e).text;

					// Player font component's text is currently displaying a text, start the timer countdown
					if (!playerText.empty() && !m_startDisplayPlayerTimer) {
						m_startDisplayPlayerTimer = true;

					} 

					// Currently displaying the player text until the timer is done
					if (m_startDisplayPlayerTimer) {
						m_currDisplayPlayerTextTimer += FixedDT;

						// Timer is done, reset
						if (m_currDisplayPlayerTextTimer >= m_displayPlayerTextDuration) {
							m_coord.ModifyComponent<Font>(e).text = "";
							m_startDisplayPlayerTimer = false;
							m_currDisplayPlayerTextTimer = 0.f;
						}
					}
				}

				// Check whether the player is colliding with a Signboard entity (it has collider + font components)
				if (m_coord.HasComponent<Collider>(e) && m_coord.GetComponent<Transform>(e).name.find("Signboard") != std::string::npos) {
					//std::cout << "This entity is: " << m_coord.GetComponent<Transform>(e).name << std::endl;

					// Collider must be TRIGGER type
					if (m_coord.GetComponent<Collider>(e).type == ColliderType::Trigger) {

						Entity player = m_coord.GetSystem<PlayerControllerSystem>()->FindPlayer(m_coord);

						// Player is colliding with this Signboard entity
						if (m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, player)) {
							//std::cout << "Collided with: " << m_coord.GetComponent<Transform>(e).name << std::endl;

							// Make the child Signboard Message entity active
							// Modify the first child with a Transform component
							auto result = m_coord.ModifyComponentInChild<Transform>(e);
							if (result) {
								auto& [transform, childEntity] = result.value();
								transform.isActive = true;
								//std::cout << "Made active: " << m_coord.GetComponent<Transform>(childEntity).name << std::endl;
							}
						} else {
							// Player is not colliding with this Signboard entity
							// Make the child Signboard Message entity inactive
							auto result = m_coord.ModifyComponentInChild<Transform>(e);
							if (result) {
								auto& [transform, childEntity] = result.value();
								transform.isActive = false;;
								//std::cout << "Made inactive: " << m_coord.GetComponent<Transform>(childEntity).name << std::endl;
							}
						}

					}
				}

				bool isUILayer = (l.Type() == LayerType::GAMEUI);
				// Flip the y axis for orthographic proj if using UI Layer (screen coords)
				glm::mat4 projToUse = isUILayer ? glm::ortho(0.f, screenW, 0.f, screenH) : proj;
				glm::mat4 viewToUse = isUILayer ? glm::mat4(1.f) : view;

				auto& pos = m_coord.GetComponent<Transform>(e).pos;
				//auto& scl = m_coord.GetComponent<Transform>(e).scale;
				auto& txt = m_coord.ModifyComponent<Font>(e);
				auto& UIpos = m_coord.GetComponent<Transform>(e).UIbasePos;
				auto& UIscale = m_coord.GetComponent<Transform>(e).UIbaseScale;

				// Translate the position and scale of UI layer objects based on the current screen resolution
				if (isUILayer) {
					// By default, the base screen resolution is 1600, 900
					float ratioPosX = UIpos.x / camInfo.width;
					float ratioPosY = UIpos.y / camInfo.height;
					float ratioScaleX = UIscale.x / camInfo.width;
					float ratioScaleY = UIscale.y / camInfo.height;

					// Get the new position and scaling based on the current screen resolution
					m_coord.ModifyComponent<Transform>(e).pos.x = ratioPosX * screenW;
					m_coord.ModifyComponent<Transform>(e).pos.y = ratioPosY * screenH;
					m_coord.ModifyComponent<Transform>(e).scale.x = ratioScaleX * screenW;
					m_coord.ModifyComponent<Transform>(e).scale.y = ratioScaleY * screenH;
				}

				// Flip the y pos for entities on the UI layer
				float finalPosY = isUILayer ? screenH - pos.y : pos.y;

				// Offset the base position based on the offset position
				float finalPosX = pos.x + txt.offsetPosition.x;
				finalPosY += txt.offsetPosition.y;

				// Calculate the effective wrap width of the text
				txt.textWrapRatio = std::clamp(txt.textWrapRatio, 0.f, 1.f);
				float effectiveWrapWidth = txt.textWrapRatio * screenW; // initial wrap width in relation to the current screen width
				float totalTextWidth = effectiveWrapWidth + finalPosX; // total text width consisting of x position and original wrap width
				// Total text width exceeds the screen, trim the initial wrap width accordingly to fit in the screen
				if (totalTextWidth > screenW) {
					effectiveWrapWidth -= totalTextWidth - screenW;
				}

				// If it is empty, make it use the default font, which is the first font in the map
				if (txt.fontType.empty()) {
					txt.fontType = m_fontCharacters.begin()->first;
				}

				// Render the actual text string
				RenderText(txt.text, finalPosX, finalPosY, txt.fontSize, glm::vec3{ txt.textColor.x, txt.textColor.y, txt.textColor.z }, projToUse, viewToUse, txt.fontType, effectiveWrapWidth);
			}
		}
		glDisable(GL_BLEND);
	}

	std::vector<std::string> TextSystem::WrapText(const std::string& text, const std::string& fontName, float maxWidth, float scale) {
		std::vector<std::string> result;
		if (m_fontCharacters.find(fontName) == m_fontCharacters.end()) {
			return result;
		}

		// Get the space width and helper function to measure the words
		auto& chars = m_fontCharacters[fontName];
		float spaceWidth = (chars[' '].advance >> 6) * scale;

		auto MeasureWord = [&](const std::string& w) -> float {
			float width = 0.f;
			for (char c : w) {
				width += (chars[c].advance >> 6) * scale;
			}
			return width;
			};

		// First, split the full text on \n to follow intentional line breaks
		std::vector<std::string> hardLines;
		std::istringstream lineStream(text);
		std::string hardLine;
		while (std::getline(lineStream, hardLine)) {
			hardLines.push_back(hardLine);
		}

		// Word wrap each hard line separately
		for (const std::string& segment : hardLines) {
			// Empty line (designed pressed Enter twice) - preserve as blank line
			if (segment.empty()) {
				result.push_back("");
				continue;
			}

			std::istringstream wordStream(segment);
			std::string word;
			std::string currentLine;
			float currentLineWidth = 0.f;

			while (wordStream >> word) {
				float wordWidth = MeasureWord(word);

				if (currentLine.empty()) {
					// First word on this line, always place it regardless of width
					currentLine = word;
					currentLineWidth = wordWidth;
				} else if (currentLineWidth + spaceWidth + wordWidth <= maxWidth) {
					// Word fits on the current line
					currentLine += " " + word;
					currentLineWidth += spaceWidth + wordWidth;
				} else {
					// Word doesnt fit, push the current line and start a new line
					result.push_back(currentLine);
					currentLine = word;
					currentLineWidth = wordWidth;
				}
			}

			// Push in the last line
			if (!currentLine.empty()) {
				result.push_back(currentLine);
			}
		}

		return result;
	}

	void TextSystem::RenderText(std::string text, float x, float y, float scale, glm::vec3 color, const glm::mat4& proj, const glm::mat4& view, const std::string& fontName, float maxWidth)
	{
		// Make sure the font exists
		if (m_fontCharacters.find(fontName) == m_fontCharacters.end()) {
			//std::cout << "ERROR: Font '" << fontName << "' not found!" << std::endl;
			return;
		}

		auto& chars = m_fontCharacters[fontName];

		m_shader->Use();
		m_shader->SetMat4("proj", proj);
		m_shader->SetMat4("view", view);
		m_shader->SetVec3("uTextColor", color);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(m_vao);

		// Start x position of text is left-anchored by default (wrap text mode for dialogue)
		float cursorX = x;

		// Split into wrapped lines if maxWidth is set
		std::vector<std::string> lines;
		if (maxWidth > 0.f) {
			lines = WrapText(text, fontName, maxWidth, scale);
		} else {
			lines.push_back(text); // a single line

			float textWidth{};
			for (char c : text) {
				auto& ch = chars[c];
				textWidth += (ch.advance >> 6) * scale;
			}
			cursorX = x - (textWidth * 0.5f); // centers the whole string if there is no wrap text
		}

		// Line height = glyph size * scale * leading
		float lineHeight = 48.f * scale * 1.2f;

		float startY = y;

		// Render each line one by one
		for (const std::string& line : lines) {
			// If there are multiple lines, for every line, reset the starting x position to the initial value
			if (maxWidth > 0.f) {
				cursorX = x;
			}
			float cursorY = startY;

			// Render each character in the line
			for (char c : line) {
				auto& ch = chars[c];

				float xpos = cursorX + ch.bearing.x * scale;
				float ypos = cursorY - (ch.size.y - ch.bearing.y) * scale;
				float w = ch.size.x * scale;
				float h = ch.size.y * scale;

				// Define vertices for the character quad
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.f, 0.f },
					{ xpos,     ypos,       0.f, 1.f },
					{ xpos + w, ypos,       1.f, 1.f },

					{ xpos,     ypos + h,   0.f, 0.f },
					{ xpos + w, ypos,       1.f, 1.f },
					{ xpos + w, ypos + h,   1.f, 0.f }
				};
				// Bind the glyph texture and update the VBO
				glBindTexture(GL_TEXTURE_2D, ch.textureID);
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // use glBufferSubData and not glBufferData
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// Draw quad
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// Advance cursors for next glyph (note that advance is number of 1/64 pixels)
				cursorX += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
			}
			// Move down the y-axis for the next line
			startY -= lineHeight;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}