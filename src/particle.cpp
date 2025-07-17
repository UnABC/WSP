#include <GL/glew.h>
#include "particle.hpp"
using namespace std;

Particle::~Particle() {
	// リソースの解放
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
	if (shaderProgram_particle) glDeleteProgram(shaderProgram_particle);
	if (shaderProgram_draw) glDeleteProgram(shaderProgram_draw);
}

void Particle::Init(map<unsigned int, particle_data>* particles_data, glm::mat4* proj, glm::mat4* global_view) {
	projection = proj;
	view = global_view;
	particles = particles_data;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 7, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 色属性 (vec4)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//パーティクル描画用
	glGenVertexArrays(1, &vao_draw);
	glGenBuffers(1, &vbo_draw);
	glBindVertexArray(vao_draw);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_draw);
	// 頂点バッファを初期化(pos,texpos,color,radius)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 9, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 角度属性 (float)
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// テクスチャ座標 (vec2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 半径属性 (float)
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// テクスチャハンドル属性 (uvec2)
	glVertexAttribIPointer(4, 2, GL_UNSIGNED_INT, 9 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// シェーダープログラムの作成
	shaderProgram_particle = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	if (!shaderProgram_particle)
		throw ParticleException("Failed to create particle shader program.");
	shaderProgram_draw = ShaderUtil::createShaderProgram(vertexShaderSource_draw, fragmentShaderSource_draw);
	if (!shaderProgram_draw)
		throw ParticleException("Failed to create particle draw shader program.");
}

void Particle::createFBO(int id) {
	glGenFramebuffers(1, &(*particles)[id].fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, (*particles)[id].fbo);
	glGenTextures(1, &(*particles)[id].texture);
	glBindTexture(GL_TEXTURE_2D, (*particles)[id].texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, PTS_D, PTS_D, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenRenderbuffers(1, &(*particles)[id].rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, (*particles)[id].rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, PTS_D, PTS_D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (*particles)[id].texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (*particles)[id].rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw ParticleException("Failed to create framebuffer for particles.");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, PTS_D, PTS_D);

	GLuint64 texture_handle = glGetTextureHandleARB((*particles)[id].texture);
	glMakeTextureHandleResidentARB(texture_handle);
	(*particles)[id].handle1 = static_cast<GLuint>(texture_handle & 0xFFFFFFFF);
	(*particles)[id].handle2 = static_cast<GLuint>(texture_handle >> 32);
}

void Particle::mkParticle(int id, SDL_Color particle_color, std::vector<long long> array) {
	if (!shaderProgram_particle || !vbo || !vao)
		throw ParticleException("Particle not initialized.");
	if (array.size() < 3)
		throw ParticleException("Insufficient data for particle creation.");
	all_vertices.clear();
	const long long polys = array.at(0) * array.at(1);
	const long long n = TMP_R * polys;
	float current_x = 0.0f, current_y = 0.0f;
	for (long long i = 0; i < n; i++) {
		long double f = (1.0 + i) / n;
		f = (f * f + f) / 2.0;
		SDL_Color color = {
			limit(f * particle_color.r, 0, 255),
			limit(f * particle_color.g, 0, 255),
			limit(f * particle_color.b, 0, 255),
			255//limit(particle_color.a, 0, 255)
		};
		normalized_color norm_color = {
			color.r / 255.0f,
			color.g / 255.0f,
			color.b / 255.0f,
			color.a / 255.0f
		};
		f = TwoM_PI * i / polys;
		long long r = TMP_R * (n - i) * array.at(i % array.at(1) + 2) / 100 / n;
		float end_x = sin(f) * r + TMP_R;
		float end_y = cos(f) * r + TMP_R;
		float vertices[2][7] = {
			{ current_x, current_y, 0.0f, norm_color.r, norm_color.g, norm_color.b, norm_color.a },
			{ end_x    , end_y    , 0.0f, norm_color.r, norm_color.g, norm_color.b, norm_color.a }
		};
		current_x = end_x;
		current_y = end_y;
		// キャッシュ作成
		all_vertices.insert(all_vertices.end(), &vertices[0][0], &vertices[0][0] + 2 * 7);
	}
	// fboに描画
	createFBO(id);
	glBindFramebuffer(GL_FRAMEBUFFER, (*particles)[id].fbo);
	glViewport(0, 0, PTS_D, PTS_D);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram_particle);
	glBindVertexArray(vao);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_particle, "projection"), 1, GL_FALSE, glm::value_ptr(particle_projection));
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(float), all_vertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_LINES, 0, all_vertices.size() / 7);
	glBindVertexArray(0);
}

void Particle::drawParticler(int id, float x, float y, float z, float r, float angle, SDL_Color color, int gmode, std::vector<AllVertexData>& all_vertices) {
	if (!shaderProgram_draw || !vbo_draw || !vao_draw)
		throw ParticleException("Particle draw not initialized.");
	// パーティクルの位置を設定
	float vertices[6][9] = {
		{ x, y, z, angle, -0.5, -0.5, r, *(float*)&(*particles)[id].handle1, *(float*)&(*particles)[id].handle2 },
		{ x, y, z, angle,  0.5, -0.5, r, *(float*)&(*particles)[id].handle1, *(float*)&(*particles)[id].handle2 },
		{ x, y, z, angle,  0.5,  0.5, r, *(float*)&(*particles)[id].handle1, *(float*)&(*particles)[id].handle2 },

		{ x, y, z, angle, -0.5, -0.5, r, *(float*)&(*particles)[id].handle1, *(float*)&(*particles)[id].handle2 },
		{ x, y, z, angle,  0.5,  0.5, r, *(float*)&(*particles)[id].handle1, *(float*)&(*particles)[id].handle2 },
		{ x, y, z, angle, -0.5,  0.5, r, *(float*)&(*particles)[id].handle1, *(float*)&(*particles)[id].handle2 }
	};
	int local_gmode = gmode - (gmode % 2);
	if (all_vertices.empty() || (all_vertices.back().gmode != local_gmode) || (all_vertices.back().ID != 7)) {
		AllVertexData new_data;
		new_data.all_vertices = std::vector<float>();
		new_data.gmode = local_gmode;
		new_data.ID = 7; // 7: パーティクル
		new_data.division = 9;
		new_data.projection = *projection;
		new_data.view = *view;
		new_data.vao = vao_draw;
		new_data.vbo = vbo_draw;
		new_data.shaderProgram = shaderProgram_draw;
		all_vertices.push_back(new_data);
	}
	// 既存の頂点データに新しい頂点を追加
	all_vertices.back().all_vertices.insert(all_vertices.back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 9);
}
