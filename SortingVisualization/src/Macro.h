#pragma once

/* Window Macros */
#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	720
#define HALF_WIDTH		WINDOW_WIDTH  / 2
#define HALF_HEIGHT		WINDOW_HEIGHT / 2

#define WINDOW_WIDTH_F	(float)WINDOW_WIDTH
#define WINDOW_HEIGHT_F (float)WINDOW_HEIGHT
#define HALF_WIDTH_F	WINDOW_WIDTH_F  / 2.0f
#define HALF_HEIGHT_F	WINDOW_HEIGHT_F / 2.0f

#define SCREEN_RATIO	WINDOW_WIDTH_F / WINDOW_HEIGHT_F

/* Utility macros */
#define RGB(r, g, b) r > 255.0f ? 1.0f : (float)r / 255.0f, g > 255.0f ? 1.0f : (float)g / 255.0f, b > 255.0f ? 1.0f : (float)b / 255.0f
#define RGBA(r, g, b, a) RGB(r, g, b), a > 255.0f ? 1.0f : (float)a / 255.0f

#define BIND(func) std::bind(func, this, std::placeholders::_1)

#define skipStep 	if (counter >= m_SkipStep)\
					{\
						m_RendererQueue.push({ vector, s_DataAnalyzer });\
						counter = 0;\
					}\
					else\
					{\
						counter++;\
					}\
