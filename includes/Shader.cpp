#include "Shader.h"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filepath) : Shader(filepath + ".vert", filepath + ".frag")
{
}

Shader::Shader(const std::string& vertFilepath, const std::string& fragFilepath)
	: m_VFilepath(vertFilepath), m_FFilepath(fragFilepath)
{
	ShaderProgramSource source = ParseShader(vertFilepath, fragFilepath);

	std::cout << "Compiling shader " << vertFilepath.substr(vertFilepath.find_last_of("/\\") + 1) << "..." << std::endl;
	m_ID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

ShaderProgramSource Shader::ParseShader(const std::string& vertexFilePath, const std::string& fragFilepath)
{
	std::ifstream streamVertex(vertexFilePath);
	std::ifstream streamFrag(fragFilepath);

	std::string line;
	std::stringstream ss[2];

	while (getline(streamVertex, line))
	{
		ss[0] << line << '\n';
	}

	while (getline(streamFrag, line))
	{
		ss[1] << line << '\n';
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "\t Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << "\t" << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	if (type == GL_VERTEX_SHADER)
		std::cout << "\t Compiled vertexShader correctly!" << std::endl;
	else
		std::cout << "\t Compiled fragmentShader correctly!" << std::endl;

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void Shader::Reload()
{
	int oldID = m_ID;
	glDeleteProgram(m_ID);
	ShaderProgramSource source = ParseShader(m_VFilepath, m_FFilepath);
	m_ID = CreateShader(source.VertexSource, source.FragmentSource);
	std::cout << "Reloaded Shaders " << m_VFilepath << ", " << m_FFilepath << std::endl
		<< " OldID: " << oldID << " NewID: " << m_ID << std::endl;

	m_UniformLocationCache.clear();
}

void Shader::Bind() const
{
	glUseProgram(m_ID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform3f(const std::string& name, int count, float* v)
{
	glUniform3fv(GetUniformLocation(name), count, v);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_ID, name.c_str());

	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' dosen't exist! For vShader: " << m_VFilepath << " , fShader: " << m_FFilepath << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}