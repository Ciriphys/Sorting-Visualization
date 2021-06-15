#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
	IndexBuffer(unsigned int sizeInBytes, unsigned int* data, unsigned int usage = GL_STATIC_DRAW) : m_Count(sizeInBytes / sizeof(unsigned int))
	{
		glGenBuffers(1, &m_BufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, data, usage);
	}
	~IndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferId);
	}

	unsigned int GetCount() const { return m_Count; };

	void Enable() const {  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId); };
	void Disable() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };

private:
	unsigned int m_BufferId;
	unsigned int m_Count;
};