#pragma once

#include <GL/glew.h>
#include <vector>

#include "Debug.h"

class BufferLayout
{
public:
	struct LayoutElement
	{
		unsigned int count;
		unsigned int type;
		unsigned char normalized;

		LayoutElement(unsigned int c, unsigned int t, unsigned char n) : count(c), type(t), normalized(n) {}

		static unsigned int GetSizeofType(unsigned int type) 
		{
			switch (type)
			{
			case GL_FLOAT:
				return sizeof(float);
			case GL_UNSIGNED_INT:
				return sizeof(unsigned int);
			case GL_UNSIGNED_BYTE:
				return sizeof(unsigned char);
			default:
				Assert(false);
				return 0;
			}

			return 0;
		}
	};
	
private:
	std::vector<LayoutElement> m_BufferStack;
	unsigned int m_Stride;

public:
	BufferLayout() : m_Stride(0)
	{
	}

	template<typename T>
	void Push(unsigned int count)
	{
		Assert(false)
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_BufferStack.emplace_back(count, GL_FLOAT, GL_FALSE);
		m_Stride += count * sizeof(float);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_BufferStack.emplace_back(count, GL_UNSIGNED_INT, GL_FALSE);
		m_Stride += count * sizeof(unsigned int);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_BufferStack.emplace_back(count, GL_UNSIGNED_BYTE, GL_TRUE);
		m_Stride += count * sizeof(unsigned char);
	}

	const std::vector<LayoutElement>& GetBufferStack() const { return m_BufferStack; }
	unsigned int GetStride() const { return m_Stride; }

};
