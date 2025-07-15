#ifndef SHAPE_HPP
#define SHAPE_HPP
#include "Image.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <vector>

struct Texture_color {
	float enable = 0.0f;
	float tex_x, tex_y, tex_width, tex_height;
	GLuint64 handle;
};

class Shape {
	int width, height;
	// 三角形の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao, vbo, shaderProgram_triangle;
	// 角丸四角形の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao_roundrect, vbo_roundrect;
	GLuint shaderProgram_roundrect;
	// 線分の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao_line, vbo_line;
	// (楕)円の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao_ellipse, vbo_ellipse;
	GLuint shaderProgram_ellipse;

	Texture_color texture;
	glm::mat4 *projection;
	glm::mat4 *view;
	//三角形のシェーダー
	const char* vertexShaderSource = R"glsl(
		#version 450 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec4 color;
		layout (location = 2) in float isTexture;
		layout (location = 3) in vec2 texCoord;
		layout (location = 4) in uvec2 aTexHandle;
		out float outIsTexture;
		out vec4 outColor;
		out vec2 TexCoords;
		flat out uvec2 TexHandle;
		uniform mat4 projection;
		uniform mat4 view;

		void main() {
			gl_Position = projection * view * vec4(position, 1.0);
			outColor = color;
			outIsTexture = isTexture;
			TexCoords = texCoord;
			TexHandle = uvec2(aTexHandle);
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 450 core
		#extension GL_ARB_bindless_texture : require
		out vec4 FragColor;
		in vec4 outColor;
		in float outIsTexture;
		in vec2 TexCoords;
		flat in uvec2 TexHandle;
		void main() {
			if (outIsTexture > 0.5) {
				sampler2D image = sampler2D(TexHandle);
				FragColor = texture(image, TexCoords)* outColor;
			} else {
				FragColor = outColor;
			}
		}
	)glsl";

	// 角丸四角形のシェーダー
	const char* vertexShaderSource_roundrect = R"glsl(
		#version 450 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in float radius; // 角の半径
		layout (location = 2) in vec4 color;
		layout (location = 3) in vec4 box_info; // 四角形の位置とサイズ (x,y,width, height)
		layout (location = 4) in float isTexture;
		layout (location = 5) in vec2 texCoord;
		layout (location = 6) in uvec2 aTexHandle;
		out vec2 v_pixelPos;
		out float outIsTexture;
		out vec4 outColor;
		out vec2 TexCoords;
		flat out uvec2 TexHandle;
		out float v_radius;
		out vec4 u_box;
		uniform mat4 projection;
		uniform mat4 view;

		void main() {
			gl_Position = projection * view * vec4(position, 1.0);
			outColor = color;
			v_pixelPos = position.xy;
			v_radius = radius;
			u_box = box_info;
			outIsTexture = isTexture;
			TexCoords = texCoord;
			TexHandle = uvec2(aTexHandle);
		}
	)glsl";
	const char* fragmentShaderSource_roundrect = R"glsl(
		#version 450 core
		#extension GL_ARB_bindless_texture : require
		in vec4 outColor;
		flat in uvec2 TexHandle;
		in float outIsTexture;
		in vec2 TexCoords;
		out vec4 FragColor;
		in vec2 v_pixelPos;
		in float v_radius; // フラグメントシェーダーへ渡された角の半径
		in vec4 u_box; // 四角形の位置とサイズ (x, y, width, height)

		// 角丸四角形の符号付き距離関数(SDF)
		float sdRoundedBox(vec2 p, vec2 b, float r) {
			vec2 d = abs(p) - b;
			return length(max(d, vec2(0.0))) - r;
		}

		void main() {
			// 1. 描画情報の準備
			vec2 box_size = u_box.zw;
			vec2 box_center = u_box.xy + box_size / 2.0;

			// 2. ローカル座標への変換
			vec2 p_local = v_pixelPos - box_center;

			// 3. SDF関数のパラメータ準備
			vec2 b = box_size / 2.0;
			float r = min(v_radius, min(b.x, b.y));

			// 4. SDFの計算
			float dist = sdRoundedBox(p_local, b, r);

			// 5. アンチエイリアシングとアルファ値の計算
			float aa_width = fwidth(dist);
			float alpha = 1.0 - smoothstep(-aa_width, aa_width, dist);

			// 6. 最適化: 透明なピクセルを破棄
			if (alpha < 0.01)
				discard;

			// 7. 最終的な色の出力
			if (outIsTexture > 0.5) {
				// テクスチャを使用する場合
				sampler2D image = sampler2D(TexHandle);
				vec4 texColor = texture(image, TexCoords);
				FragColor = vec4(texColor.rgb, alpha * texColor.a* outColor.a);
			} else {
				// テクスチャを使用しない場合
				FragColor = vec4(outColor.rgb, alpha * outColor.a);
			}
		})glsl";

	// (楕)円のシェーダー
	const char* vertexShaderSource_ellipse = R"glsl(
		#version 450 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec2 center_pos;
		layout (location = 2) in float major_axis;
		layout (location = 3) in float minor_axis;
		layout (location = 4) in float angle;
		layout (location = 5) in vec4 color;
		layout (location = 6) in float isTexture;
		layout (location = 7) in vec2 texCoord;
		layout (location = 8) in uvec2 aTexHandle;
		out float outIsTexture;
		out vec4 outColor;
		out vec2 TexCoords;
		flat out uvec2 TexHandle;
		out vec2 PixelPos;
		out vec2 Center;
		out float MajorAxis;
		out float MinorAxis;
		out float Angle;
		uniform mat4 projection;
		uniform mat4 view;

		void main() {
			gl_Position = projection * view * vec4(position, 1.0);
			outColor = color;
			PixelPos = position.xy;
			Center = center_pos;
			MajorAxis = major_axis;
			MinorAxis = minor_axis;
			Angle = angle;
			outIsTexture = isTexture;
			TexCoords = texCoord;
			TexHandle = uvec2(aTexHandle);
		}
	)glsl";
	const char* fragmentShaderSource_ellipse = R"glsl(
		#version 450 core
		#extension GL_ARB_bindless_texture : require
		out vec4 FragColor;
		in vec4 outColor;
		flat in uvec2 TexHandle;
		in float outIsTexture;
		in vec2 TexCoords;
		in vec2 PixelPos;
		in vec2 Center;
		in float MajorAxis;
		in float MinorAxis;
		in float Angle;
		void main() {
			// 1. ローカル座標への変換
			vec2 localPos = PixelPos - Center;
			// 2. 楕円の方程式を使用して距離を計算
			float a = MajorAxis / 2.0; // 長軸の半分
			float b = MinorAxis / 2.0; // 短軸の半分
			// 回転行列を適用
			float cosAngle = cos(Angle);
			float sinAngle = sin(Angle);
			vec2 rotatedPos = vec2(
				localPos.x * cosAngle + localPos.y * sinAngle,
				-localPos.x * sinAngle + localPos.y * cosAngle
			);
			// 楕円の方程式に基づく距離
			float dist = (rotatedPos.x * rotatedPos.x) / (a * a) + (rotatedPos.y * rotatedPos.y) / (b * b) - 1.0;
			// 3. アンチエイリアシングの幅を設定
			float aa_width = fwidth(dist);
			// 4. アルファ値の計算
			float alpha = 1.0 - smoothstep(-aa_width, aa_width, dist);
			// 5. 透明なピクセルを破棄
			if (alpha < 0.01)
				discard;
			// 6. 最終的な色の出力
			if (outIsTexture > 0.5) {
				// テクスチャを使用する場合
				sampler2D image = sampler2D(TexHandle);
				vec4 texColor = texture(image, TexCoords);
				FragColor = vec4(texColor.rgb, alpha * texColor.a * outColor.a);
			} else {
				// テクスチャを使用しない場合
				FragColor = vec4(outColor.rgb, alpha * outColor.a);
			}
		}
	)glsl";
public:
	Shape() : width(640), height(480), vao(0), vbo(0), shaderProgram_triangle(0) {};
	~Shape();
	void Init(int w, int h,glm::mat4* proj, glm::mat4* global_view);
	void SetTexture(image_data* image, bool enable = true, float tex_x = 0.0f, float tex_y = 0.0f, float tex_width = -1.0f, float tex_height = -1.0f);
	void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_Color color1, SDL_Color color2, SDL_Color color3, int gmode, std::vector<AllVertexData>& all_vertices, int isRect = 0);
	void draw_round_rectangle(float x, float y, float width, float height, float radius, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode, std::vector<AllVertexData>& all_vertices);
	void draw_line(float x1, float y1, float x2, float y2, SDL_Color color1, SDL_Color color2, int gmode, std::vector<AllVertexData>& all_vertices);
	void draw_ellipse(float center_x, float center_y, float major_axis, float minor_axis, float angle, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode, std::vector<AllVertexData>& all_vertices);
};

#endif