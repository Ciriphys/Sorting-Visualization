#pragma once

#include "VertexBuffer.h"
#include "BufferLayout.h"

#include "IndexBuffer.h"

class VertexArray
{
public: 
	 VertexArray(IndexBuffer* ibo = nullptr) 
	 {
		 glGenVertexArrays(1, &m_VertexArrayId);
		 glBindVertexArray(m_VertexArrayId);

		 AttachIndexBuffer(ibo);
	 }

	~VertexArray()
	{
		delete m_Ibo;
		glDeleteVertexArrays(1, &m_VertexArrayId);
	}

	void Enable() const
	{
		glBindVertexArray(m_VertexArrayId);
	}

	void Disable() const
	{
		glBindVertexArray(0);
	}

	void AddLayout(const VertexBuffer& vbo, const BufferLayout& blo)
	{
		vbo.Enable();
		auto bufferStack = blo.GetBufferStack();

		int counter = 0;
		unsigned int offset = 0;
		for (const auto& element : bufferStack)
		{
			glVertexAttribPointer(counter, element.count, element.type, element.normalized, blo.GetStride(), (const void*)offset);
			glEnableVertexAttribArray(counter);

			offset += element.count * BufferLayout::LayoutElement::GetSizeofType(element.type);
			counter++;
		}
	}

	void AttachIndexBuffer(IndexBuffer* ibo)
	{
		m_Ibo = ibo;
	}

	void DetachIndexBuffer()
	{
		delete m_Ibo;
	}

	operator IndexBuffer*() const { return m_Ibo; }
	IndexBuffer* GetIndexBuffer() const { return this->operator IndexBuffer*(); }

private: 
	unsigned int m_VertexArrayId;
	IndexBuffer* m_Ibo;
};