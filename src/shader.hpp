#ifndef SHADER_HPP
#define SHADER_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include "exception.hpp"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderUtil {
private:
    static GLuint compileShader(GLenum type, const char* source, const std::string& shaderName = "");
public:
    ShaderUtil() = delete;
    static GLuint createShaderProgram(const char* vsSource, const char* fsSource);
    static glm::mat4 recalcProjection(int width, int height) {
        glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);glViewport(0, 0, width, height); return projection;
    };
};


#endif
