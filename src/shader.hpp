#ifndef SHADER_HPP
#define SHADER_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include "exception.hpp"
#include <string>

class ShaderUtil {
private:
    static GLuint compileShader(GLenum type, const char* source, const std::string& shaderName = "");
public:
    ShaderUtil() = delete;
    static GLuint createShaderProgram(const char* vsSource, const char* fsSource);
};


#endif
