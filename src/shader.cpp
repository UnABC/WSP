#include <GL/glew.h>
#include "shader.hpp"
using namespace std;

GLuint ShaderUtil::compileShader(GLenum type, const char* source, const string& shaderName) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// Check for compilation errors
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		throw ShaderException("Error compiling " + shaderName + ": " + string(infoLog));
	}
	return shader;
}

GLuint ShaderUtil::createShaderProgram(const char* vsSource, const char* fsSource) {
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vsSource, "Vertex Shader");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource, "Fragment Shader");

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Check for linking errors
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
		throw ShaderException("Error linking shader program: " + string(infoLog));
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
