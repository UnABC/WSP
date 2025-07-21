#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "exception.hpp"
#include "shader.hpp"
#include "Image.hpp"
#include <vector>
#include <map>

struct particle_data {
	GLuint fbo, texture, rbo;
	GLuint handle1, handle2; // テクスチャハンドル
};

class Particle {
	//パーティクル生成用
	GLuint vao, vbo;
	GLuint shaderProgram_particle;
	//描画用
	GLuint vao_draw, vbo_draw;
	GLuint shaderProgram_draw;
	glm::mat4* projection;
	glm::mat4* view;
	int* projectionID;
	std::map<unsigned int, particle_data>* particles; // パーティクルデータのマップ
	const int PCACHE_D = 18;
	const int PTS_D = (PCACHE_D + 1) * PCACHE_D / 2;
	const int PTS_DM1 = PTS_D - 1;
	const int TMP_R = PTS_D;
	const int TMP_D = TMP_R * 2;
	const long double TwoM_PI = 6.283185307179586476925286766559L;
	const glm::mat4 particle_projection = glm::ortho(0.0f, (float)TMP_D, (float)TMP_D, 0.0f, -1.0f, 1.0f);

	float current_x = 0.0f, current_y = 0.0f, current_z = 0.0f; // 現在のパーティクル位置

	// パーティクル生成用
	const char* vertexShaderSource = R"glsl(
		#version 450 core
		layout (location = 0) in vec2 position;
		layout (location = 1) in vec4 color;
		out vec4 outColor;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * vec4(position, 0.0, 1.0);
			outColor = color;
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 450 core
		in vec4 outColor;
		out vec4 FragColor;

		void main() {
			FragColor = outColor;
		}
	)glsl";

	// パーティクル描画用
	const char* vertexShaderSource_draw = R"glsl(
		#version 450 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in float angle;
		layout (location = 2) in vec2 texpos;
		layout (location = 3) in float radius;
		layout (location = 4) in uvec2 aTexHandle;
		layout (location = 5) in float alpha;
		out vec2 vTexCoord;
		out float outAlpha;
		flat out uvec2 TexHandle;
		uniform mat4 projection;
		uniform mat4 view;

		void main() {
			vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
			vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);
			float length = radius * 2.0f;
			float c = cos(angle);
			float s = sin(angle);
			vec2 rotated_texpos = vec2(
				texpos.x * c - texpos.y * s,
				texpos.x * s + texpos.y * c
			);
			vec3 view_pos = position + camera_right * rotated_texpos.x * length + camera_up * rotated_texpos.y * length;
			gl_Position = projection * view * vec4(view_pos, 1.0);
			vTexCoord = texpos + 0.5f; // テクスチャ座標を[0, 1]に変換
			TexHandle = aTexHandle;
			outAlpha = alpha;
		}
	)glsl";
	const char* fragmentShaderSource_draw = R"glsl(
		#version 450 core
		#extension GL_ARB_bindless_texture : require
		in vec2 vTexCoord;
		in float outAlpha;
		out vec4 FragColor;
		flat in uvec2 TexHandle;

		void main() {
			sampler2D image = sampler2D(TexHandle);
			vec4 texColor = texture(image, vTexCoord);
			texColor.a *= outAlpha; // アルファ値を適用
			if (texColor.a < 0.1) 
				discard; // アルファ値が低い場合は描画しない
			FragColor = texColor;
		}
	)glsl";


	inline Uint8 limit(Uint8 value, Uint8 min, Uint8 max) {
		return (value < min) ? min : (value > max) ? max : value;
	}
	struct normalized_color {
		float r, g, b, a;
	};
	void createFBO(int id);
public:
	~Particle();
	void Init(std::map<unsigned int, particle_data>* particles, glm::mat4* proj, glm::mat4* global_view,int * projID);
	void mkParticle(int id, SDL_Color particle_color, std::vector<long long> array);
	void ldParticle(int id, int img_id, std::map<unsigned int, image_data>& images);
	void drawParticler(int id, float x, float y, float z, float r, float angle,  int gmode, std::map<float, AllVertexData>& all_vertices, glm::vec3 cameraPos, float alpha = 1.0f);
	void drawParticle(int id, float x, float y, float z, float r,  int gmode, std::map<float, AllVertexData>& all_vertices, glm::vec3 cameraPos, float alpha = 1.0f) {
		drawParticler(id, x, y, z, r, 0.0f, gmode, all_vertices, cameraPos, alpha);
	}
	void drawParticlemr(int id, float r, float angle,  int gmode, std::map<float, AllVertexData>& all_vertices, glm::vec3 cameraPos, float alpha = 1.0f) {
		drawParticler(id, current_x, current_y, current_z, r, angle, gmode, all_vertices, cameraPos, alpha);
	}
	void drawParticlem(int id, float r,  int gmode, std::map<float, AllVertexData>& all_vertices, glm::vec3 cameraPos, float alpha = 1.0f) {
		drawParticler(id, current_x, current_y, current_z, r, 0.0f, gmode, all_vertices, cameraPos, alpha);
	}

};


#endif