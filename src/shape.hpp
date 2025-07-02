#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "shader.hpp"
#include <vector>

class Shape {
	int width, height;
	// 三角形の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao, vbo, shaderProgram_triangle;
	// 角丸四角形の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao_roundrect, vbo_roundrect;
	GLuint shaderProgram_roundrect;
	// 線分の頂点配列オブジェクトとバッファオブジェクト
	GLuint vao_line, vbo_line;
	// すべての頂点を格納するベクター
	std::vector<float> all_triangle_vertices;
	std::vector<float> all_roundrect_vertices;
	std::vector<float> all_line_vertices;

	glm::mat4 projection;
	//三角形のシェーダー
	const char* vertexShaderSource = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;
		out vec3 outColor;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * vec4(position, 1.0);
			outColor = color;
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 330 core
		out vec4 FragColor;
		in vec3 outColor;
		void main() {
			FragColor = vec4(outColor, 1.0);
		}
	)glsl";

	// 角丸四角形のシェーダー
	const char* vertexShaderSource_roundrect = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 position;
		layout (location = 2) in float radius; // 角の半径
		layout (location = 3) in vec3 color;
		layout (location = 4) in vec4 box_info; // 四角形の位置とサイズ (x,y,width, height)
		out vec2 v_pixelPos;
		out vec3 outColor;
		out float v_radius;
		out vec4 u_box;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * vec4(position, 1.0);
			outColor = color;
			v_pixelPos = position.xy;
			v_radius = radius;
			u_box = box_info;
		}
	)glsl";
	const char* fragmentShaderSource_roundrect = R"glsl(
		#version 330 core
		in vec3 outColor;
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
			FragColor = vec4(outColor.rgb, alpha);
	})glsl";

public:
	Shape() : width(640), height(480), vao(0), vbo(0), shaderProgram_triangle(0) {};
	~Shape();
	void Init(int w, int h);
	void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_Color color1, SDL_Color color2, SDL_Color color3, float depth);
	void draw_round_rectangle(float x, float y, float width, float height, float radius, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth);
	void draw_line(float x1, float y1, float x2, float y2, SDL_Color color1, SDL_Color color2, float depth);

	void draw_shapes();
	void draw_roundrect();
	void draw_lines();
	void Clear();
};

#endif