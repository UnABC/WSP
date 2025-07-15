#ifndef FONT_HPP
#define FONT_HPP

#include "exception.hpp"
#include "shader.hpp"
#include "BLTexture.hpp"
#include <ft2build.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include FT_FREETYPE_H
#include <boost/locale.hpp>
#include <map>

struct Character {
    glm::ivec2 Size;	//グリフのサイズ
    glm::ivec2 Bearing;	//グリフのベアリング
    float Advance;		//グリフのアドバンス
    glm::vec2 uv_top_left; // UV座標の左上
    glm::vec2 uv_bottom_right; // UV座標の右下
};

class Font {
private:
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    glm::mat4* projection;
    glm::mat4* view;
    GLuint vao, vbo;

    // font関連
    //頂点シェーダー
    const char* vertexShaderSource = R"glsl(
        #version 450 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec2 texCoord;
        layout (location = 2) in uvec2 aTexHandle;
        layout (location = 3) in vec4 color;
        out vec2 TexCoords;
        out vec4 outColor;
        flat out uvec2 TexHandle;
        uniform mat4 projection;
        uniform mat4 view;

        void main() {
            gl_Position = projection * view * vec4(position, 1.0);
            TexCoords = texCoord;
            outColor = color;
            TexHandle = uvec2(aTexHandle);
        }
    )glsl";
    //色の計算
    const char* fragmentShaderSource = R"glsl(
        #version 450 core
        #extension GL_ARB_bindless_texture : require
        out vec4 FragColor;
        in vec2 TexCoords;
        in vec4 outColor;
        flat in uvec2 TexHandle;
        void main() {
			sampler2D image = sampler2D(TexHandle);
            float alpha = texture(image, TexCoords).r;
            if (alpha < 0.01)
                discard;
			FragColor = vec4(outColor.rgb, alpha*outColor.a);
        }
    )glsl";
    GLuint shaderProgram;

    //フォントのキャッシュ
    std::unordered_map<char16_t, Character>* characters;
    BLTexture* glyph_atlas;
    glm::uvec2 atlas_cursor = { 0, 0 };
    unsigned int atlas_line_height = 0;
    const unsigned int atlas_max_size = 2048;

    int font_size = 24;
    int old_font_size = 0; // フォントのサイズ
    std::string old_font_path; // フォントのパス

    void MakeCache(char16_t c);
public:
    Font() : library(nullptr), face(nullptr), slot(nullptr), vao(0), vbo(0), shaderProgram(0) {};
    ~Font();
    void Init(std::unordered_map<char16_t, Character>& global_char, BLTexture& global_texture, glm::mat4* proj, glm::mat4* global_view);
    void SetFont(const char* font_path, int size);
    void SetTexts(std::string text, float x, float y, int width, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode, std::vector<AllVertexData>& all_vertices);
};
#endif