#pragma once

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#else
	#include <GL/glew.h>
#endif

