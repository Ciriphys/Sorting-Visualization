#pragma once

#include <cstdio>
#include <GL/glew.h>

#ifdef _DEBUG
	#define Assert(x) if(!(x)) __debugbreak();

	enum class DebugLevel : int
	{
		None = 0,
		Fatal = 1,
		High = 2,
		Medium = 3,
		Low = 4,
		Notification = 5,
	};

	static DebugLevel s_DebugLevel = DebugLevel::Fatal;

	void SetDebugLevel(DebugLevel level) { s_DebugLevel = level; }

	void StartOpenGLDebugging(const void* userParam = nullptr)
	{
		auto OpenGLMessageLog = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:
				if ((int)s_DebugLevel > 0)
				{
					printf("[OpenGL::High]\n %s\n", message);
					if (s_DebugLevel == DebugLevel::Fatal)
						Assert(false);
				}
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				if ((int)s_DebugLevel > 2)
				{
					printf("[OpenGL::Medium]\n %s\n", message);
					if (s_DebugLevel == DebugLevel::Fatal)
						Assert(false);
				}
				break;
			case GL_DEBUG_SEVERITY_LOW:
				if ((int)s_DebugLevel > 3)
				{
					printf("[OpenGL::Low]\n %s\n", message);
					if (s_DebugLevel == DebugLevel::Fatal)
						Assert(false);
				}
				break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				if ((int)s_DebugLevel > 4)
				{
					printf("[OpenGL::Notification]\n %s\n", message);
					if (s_DebugLevel == DebugLevel::Fatal)
						Assert(false);
				}
				break;
			}
		};

		glDebugMessageCallback(OpenGLMessageLog, userParam);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

#else
	#define Assert(x) 

	void StartOpenGLDebugging()
	{

	}

	enum class DebugLevel;

	void SetDebugLevel(DebugLevel level)
	{

	}

#endif