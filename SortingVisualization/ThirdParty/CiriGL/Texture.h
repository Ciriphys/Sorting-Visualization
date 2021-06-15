#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include "Debug.h"

#include <GL/glew.h>
#include <iostream>

// The only supported texture is the 2D one.
// Jpg and Png only are the supported image formats.
 
class Texture
{
public: 
	Texture() {}
	~Texture() 
	{
		glDeleteTextures(1, &m_TextureId);
	}

	void SetParameter(unsigned int parameterName, int value) const
	{
		glTexParameteri(GL_TEXTURE_2D, parameterName, value);
	}

	void Active(unsigned int index) const
	{
		Assert(index < 32)
		glActiveTexture(GL_TEXTURE0 + index);
		
		Enable();
	}

	void Enable() const
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
	}

	void Disable() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Load(const std::string& path, bool shouldFlipOnLoad = false)
	{
		m_Path = path;

		if (path != "notex")
		{
			GenerateTexture();
			LoadTexture(shouldFlipOnLoad);
		}
	}

	void GenerateEmpty()
	{
		glGenTextures(1, &m_TextureId);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 12870, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	operator unsigned int() const { return m_TextureId; }

private:
	void GenerateTexture()
	{
		glGenTextures(1, &m_TextureId);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);

		SetParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		SetParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void LoadTexture(bool shouldFlipOnLoad = false) const
	{
		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(shouldFlipOnLoad);
		unsigned char* data = stbi_load(m_Path.c_str(), &width, &height, &nrChannels, 0);

		if (!data) { std::cout << "Failed to load texture data! {Check path: " << m_Path << "}\n\n"; Assert(false); }
		else std::cout << "Texture image successfully loaded!\n";

		unsigned int format = m_Path.find(".jpg") != std::string::npos ? GL_RGB : GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	std::string m_Path;

	unsigned int m_TextureId;
};