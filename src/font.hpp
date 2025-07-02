#ifndef FONT_HPP
#define FONT_HPP

#include <ft2build.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include "exception.hpp"
#include "shader.hpp"
#include <map>
#include <boost/locale.hpp>
#include FT_FREETYPE_H

struct Character {
    //GLuint TextureID;	//グリフのテクスチャID
    int LayerIndex;
    glm::ivec2 Size;	//グリフのサイズ
    glm::ivec2 Bearing;	//グリフのベアリング
    float Advance;		//グリフのアドバンス
};

class Font {
private:
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    glm::mat4 projection;
    GLuint vao, vbo;

    // font関連
    GLuint textureArrayID;
    int nextLayerIndex; // レイヤーインデックスの管理
    unsigned long long maxGlyphWidth, maxGlyphHeight; // 最大グリフの大きさ
    int maxTextureSize = 1024;
    //頂点シェーダー
    const char* vertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec2 texCoord;
        layout (location = 2) in float layerIndex;
        layout (location = 3) in vec3 color;
        out VS_OUT {
            vec2 TexCoords;
            float LayerIndex;
        } vs_out;
        out vec3 outColor;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * vec4(position, 1.0);
            vs_out.TexCoords = texCoord;
            vs_out.LayerIndex = layerIndex;
            outColor = color;
        }
    )glsl";
    //色の計算
    const char* fragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;
        in VS_OUT {
            vec2 TexCoords;
            float LayerIndex;
        } fs_in;
        in vec3 outColor;
        uniform sampler2DArray textTextureArray;
        void main() {
            float alpha = texture(textTextureArray, vec3(fs_in.TexCoords, fs_in.LayerIndex)).r;
            if(alpha < 0.1)
                discard;
            FragColor = vec4(outColor, alpha);
        }
    )glsl";
    GLuint shaderProgram;

    //フォントのキャッシュ
    std::unordered_map<char16_t, Character> characters;
    int font_size = 24;
    int old_font_size = 0; // フォントのサイズ
    std::string old_font_path; // フォントのパス
    std::vector<float> all_vertices; // 全ての頂点データを保持するベクター

    //void recalcProjection(int width, int height) { projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);glViewport(0, 0, width, height); }
public:
    Font() : library(nullptr), face(nullptr), slot(nullptr), vao(0), vbo(0), shaderProgram(0), nextLayerIndex(0), maxGlyphWidth(0), maxGlyphHeight(0), textureArrayID(0) {};
    ~Font();
    void Init(int width, int height);
    void SetFont(const char* font_path, int size);
    void SetTexts(std::string text, float x, float y, float scale, int width, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth);
    void DrawTexts();
    void Clear();
};



#endif