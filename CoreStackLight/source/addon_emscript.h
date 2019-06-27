#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#else
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions. You may use another OpenGL loader/header such as: glew, glext, glad, glLoadGen, etc.
#endif