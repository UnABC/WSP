#ifndef SHADER_HPP
#define SHADER_HPP

#include "exception.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

struct AllVertexData {
    std::vector<float> all_vertices;
    int gmode;
    // 0: テキスト,1:画像,2:三角形,3:角丸四角形,4:線分,5:楕円,6:FBO,7:パーティクル
    int ID;
    int division;
    glm::mat4 projection;
    glm::mat4 view;
    GLuint vao, vbo;
    GLuint shaderProgram;
    GLenum graphics_mode = GL_TRIANGLES; // 描画モード
    int projectionID = 0;
};


class ShaderUtil {
private:
    static GLuint compileShader(GLenum type, const char* source, const std::string& shaderName = "");
public:
    ShaderUtil() = delete;
    static GLuint createShaderProgram(const char* vsSource, const char* fsSource);
    static glm::mat4 recalcProjection(int width, int height) {
        glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
        glViewport(0, 0, width, height);
        return projection;
    };
};


#endif
