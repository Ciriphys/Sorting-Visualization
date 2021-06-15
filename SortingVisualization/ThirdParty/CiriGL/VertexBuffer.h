#pragma once

#include <GL/glew.h>

class VertexBuffer
{
public:
	 VertexBuffer(unsigned int sizeInBytes, const void * data, unsigned int usage = GL_STATIC_DRAW)
	 {
		 glGenBuffers(1, &m_BufferId);
		 glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
		 glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, usage);
	 }

	~VertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferId);
	}

	void Enable() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
	}

	void Disable() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	unsigned int m_BufferId;
};