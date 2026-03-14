#pragma once

#include <GL/glew.h>

namespace plexy::ui {


GLuint canvas_compile_shader(GLenum type, const char* src);
GLuint canvas_link_program(GLuint vs, GLuint fs);
GLuint canvas_create_program(const char* vs_src, const char* fs_src);


extern const char* kUIKitSdfVS;
extern const char* kUIKitSdfFS;
extern const char* kUIKitTextVS;
extern const char* kUIKitTextFS;
extern const char* kUIKitTextBatchVS;
extern const char* kUIKitImageFS;
extern const char* kUIKitGlassFS;
extern const char* kUIKitLineVS;
extern const char* kUIKitLineFS;

} 
