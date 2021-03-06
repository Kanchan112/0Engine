#include "Graphics/shader.h"
#include "Math/matrix3.h"
#include "Math/matrix4.h"
#include "Math/vector2.h"
#include "Math/vector3.h"
#include "Debugger/logger.h"
#include "GL/glew.h"

namespace s00nya
{

	Shader::Shader(const Character* vertex, const Character* fragment, const Character* geometry)
	{
		UInteger vertexID(glCreateShader(GL_VERTEX_SHADER));
		UInteger fragmentID(glCreateShader(GL_FRAGMENT_SHADER));
		UInteger geometryID;

		glShaderSource(vertexID, 1, &vertex, nullptr);
		glCompileShader(vertexID);
		Integer sucess;
		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &sucess);
		if (!sucess)
		{
			char info[512];
			glGetShaderInfoLog(vertexID, sizeof(info), nullptr, info);
			Debug::Add("Vertex Shader not compiled - " + std::string(info), Debug::S00NYA_LOG_ERROR);
			return;
		}

		glShaderSource(fragmentID, 1, &fragment, nullptr);
		glCompileShader(fragmentID);
		glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &sucess);
		if (!sucess)
		{
			char info[512];
			glGetShaderInfoLog(fragmentID, sizeof(info), nullptr, info);
			Debug::Add("Fragment Shader not compiled - " + std::string(info), Debug::S00NYA_LOG_ERROR);
			return;
		}

		if (geometry)
		{
			geometryID = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryID, 1, &geometry, nullptr);
			glCompileShader(geometryID);
			glGetShaderiv(geometryID, GL_COMPILE_STATUS, &sucess);
			if (!sucess)
			{
				char info[512];
				glGetShaderInfoLog(geometryID, sizeof(info), nullptr, info);
				Debug::Add("Geometry Shader not compiled - " + std::string(info), Debug::S00NYA_LOG_ERROR);
				return;
			}
		}

		m_id = glCreateProgram();
		glAttachShader(m_id, vertexID);
		glAttachShader(m_id, fragmentID);
		if (geometry) glAttachShader(m_id, geometryID);
		glLinkProgram(m_id);

		glGetProgramiv(m_id, GL_LINK_STATUS, &sucess);
		if (!sucess)
		{
			char info[512];
			glGetProgramInfoLog(m_id, sizeof(info), nullptr, info);
			Debug::Add("Shader Program Linking Error - " + std::string(info), Debug::S00NYA_LOG_ERROR);
			return;
		}

		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);
		if (geometry) glDeleteShader(geometryID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_id);
	}

	void Shader::SetMatrix3(const Character* name, const Matrix3& matrix) const
	{
		glUniformMatrix3fv(
			glGetUniformLocation(m_id, name),
			1,
			GL_TRUE,
			&matrix.elements[0]
		);
	}

	void Shader::SetMatrix4(const Character* name, const Matrix4& matrix) const
	{
		glUniformMatrix4fv(
			glGetUniformLocation(m_id, name),
			1,
			GL_TRUE,
			&matrix.elements[0]
		);
	}

	void Shader::SetVector3(const Character* name, const Vector3& vector) const
	{
		glUniform3fv(
			glGetUniformLocation(m_id, name),
			1,
			&vector.values[0]
		);
	}

	void Shader::SetVector2(const Character* name, const Vector2& vector) const
	{
		glUniform2fv(
			glGetUniformLocation(m_id, name),
			1,
			&vector.values[0]
		);
	}

	void Shader::SetSwitches(const Character * name, const Boolean & value) const
	{
		glUniform1i(
			glGetUniformLocation(m_id, name),
			value
		);
	}

	void Shader::SetTexture2D(const Character* name, const Integer& id) const
	{
		glUniform1i(
			glGetUniformLocation(m_id, name),
			id
		);
	}

	Shader* Shader::ParseShader(const Character* path)
	{
		// Read all content from the file
		std::ifstream file(path, std::ios::in);
		std::stringstream stream;
		stream << file.rdbuf();
		file.close();
		std::string content(stream.str());

		/*
		* All shader code are expected in a single file
		* Three types of shaders are suppported : Vertex, Fragment and Geometry
		* Geometry shader is optional
		* Shader code must be in the order: Vertex, Geometry and Fragment with Goemetry being optional
		* Every shader must begin with a declaration of their respective type
		- `@Vertex Shader` : Vertex Shader
		- `@Fragment Shader` : Fragment Shader
		- `@Geometry Shader` : Geometry Shader
		*/

		// Extract codes for Vertex, Geometry (if available) and Fragment Shader
		PDUInteger vertexPos = content.find("@Vertex Shader");
		PDUInteger fragmentPos = content.find("@Fragment Shader");
		PDUInteger geometryPos = content.find("@Geometry Shader");
		// Given file content error / unsatisfied
		if (vertexPos == std::string::npos || fragmentPos == std::string::npos)
		{
			Debug::Add("Could not load Shader - " + std::string(path), Debug::S00NYA_LOG_WARNING);
			return nullptr;
		}

		// Return the Shader
		if (geometryPos == std::string::npos)
		{
			std::string vertex(content.substr(vertexPos + 14, fragmentPos - vertexPos - 14));
			std::string fragment(content.substr(fragmentPos + 16, content.length() - fragmentPos - 16));
			const Character* vertexCstr(vertex.c_str());
			const Character* fragmentCstr(fragment.c_str());
			return new Shader(vertexCstr, fragmentCstr);
		}
		else
		{
			std::string vertex(content.substr(vertexPos + 14, geometryPos - vertexPos - 14));
			std::string geometry(content.substr(geometryPos + 16, fragmentPos - geometryPos - 16));
			std::string fragment(content.substr(fragmentPos + 16, content.length() - fragmentPos - 16));
			const Character* vertexCstr(vertex.c_str());
			const Character* geometryCstr(geometry.c_str());
			const Character* fragmentCstr(fragment.c_str());
			return new Shader(vertexCstr, fragmentCstr, geometryCstr);
		}
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_id);
	}

}