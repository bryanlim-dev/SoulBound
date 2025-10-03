/******************************************************************************
 * File:        Material.h
 * Project:     NightSnack
 * Author(s):   Goh Bei Lai (Primary, 100%) - Created Material class
 *
 * Description:
 * The Material class encapsulates shader and texture data.
 * It manages base color, texture binding.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class Material {
public:

	/************************************************************************/
	/*!
	\brief
	Constructor for the Material class.
	Initializes a Material with a shader and optional texture. 
	If the texture is a .png, alpha blending is enabled for it.

	\param[in] shader
	Pointer to the Shader object used for this material

	\param[in] texture
	Pointer to the Texture object bound to this material
	If this material has no texture, this can be nullptr
	*/
	/************************************************************************/
    Material(Shader* shader, Texture* texture = nullptr);

	/************************************************************************/
	/*!
	\brief
	Retrieves the Shader object associated with this Material

	\return
	Pointer to the Shader used by this Material
	*/
	/************************************************************************/
    Shader* GetShader() const;

	/************************************************************************/
	/*!
	\brief
	Retrieves the Texture object associated with this Material

	\return
	Pointer to the Texture used by this Material.
	Nullptr if this Material has no Texture
	*/
	/************************************************************************/
    Texture* GetTexture() const;

	/************************************************************************/
	/*!
	\brief
	Sets the base color of the Material.
	This color will be passed to the shader as "uBaseColor"

	\param[in] r
	Red component of the color

	\param[in] g
	Green component of the color

	\param[in] b
	Blue component of the color

	\param[in] a
	Alpha component of the color
	*/
	/************************************************************************/
    void SetBaseColor(float r, float g, float b, float a);

	/************************************************************************/
	/*!
	\brief
	Applies the material's properties before rendering.
	This includes setting shader uniform variables, binding textures, and 
	configuring OpenGL states such as alpha blending and depth mask.

	\param[in] wireframe
	Represents whether rendering is in wireframe mode.
	If true, textures will not be binded.
	/************************************************************************/
    void ApplyUniforms(bool wireframe);

private:
    Shader* m_shader;
    Texture* m_texture;
    glm::vec4 m_baseColor{ 1.f, 1.f, 1.f, 1.f};
    bool m_enableAlphaBlending{ false };
};
