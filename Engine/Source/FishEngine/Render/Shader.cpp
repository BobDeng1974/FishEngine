#include <FishEngine/Shader.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <set>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <FishEngine/Texture.hpp>
#include <FishEngine/Common.hpp>
#include <FishEngine/Debug.hpp>
#include <FishEngine/Pipeline.hpp>
#include <FishEngine/ShaderCompiler.hpp>

//#include EnumHeader(CullFace)
#include <FishEngine/Generated/Enum_Cullface.hpp>

#include <FishEngine/Render/RenderQueue.hpp>

using namespace std;
using namespace FishEngine;

GLuint
CompileShader(
	GLenum             shader_type,
	const std::string& shader_str)
{
	const GLchar* shader_c_str = shader_str.c_str();
	GLuint shader = glCreateShader(shader_type);
	assert(shader > 0);
	glShaderSource(shader, 1, &shader_c_str, NULL);
	glCompileShader(shader);
	GLint success = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> infoLog(infoLogLength + 1);
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog.data());
		throw std::runtime_error(infoLog.data());
	}
	return shader;
};


GLuint
LinkShader(GLuint vs,
	GLuint tcs,
	GLuint tes,
	GLuint gs,
	GLuint fs)
{
	glCheckError();
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	if (gs != 0)
		glAttachShader(program, gs);
	if (tes != 0) {
		if (tcs != 0) glAttachShader(program, tcs);
		glAttachShader(program, tes);
	}
	glLinkProgram(program);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> infoLog(infoLogLength + 1);
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog.data());
		throw std::runtime_error(infoLog.data());
	}

	glDetachShader(program, vs);
	glDetachShader(program, fs);
	if (gs != 0) {
		glDetachShader(program, gs);
	}
	if (tes != 0) {
		if (tcs != 0) glDetachShader(program, tcs);
		glDetachShader(program, tes);
	}

	glCheckError();
	return program;
}

GLuint
LinkShader_tf(GLuint vs,
		   GLuint tcs,
		   GLuint tes,
		   GLuint gs,
		   GLuint fs)
{
	glCheckError();
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	if (gs != 0)
		glAttachShader(program, gs);
	if (tes != 0) {
		if (tcs != 0) glAttachShader(program, tcs);
		glAttachShader(program, tes);
	}
	const char* const varyings[] = {"OutputPosition", "OutputNormal", "OutputTangent"};
	glCheckError();
	glTransformFeedbackVaryings(program, 3, varyings, GL_SEPARATE_ATTRIBS);
	glCheckError();
	glLinkProgram(program);
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> infoLog(infoLogLength + 1);
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog.data());
		throw std::runtime_error(infoLog.data());
	}
	
	glDetachShader(program, vs);
	glDetachShader(program, fs);
	if (gs != 0) {
		glDetachShader(program, gs);
	}
	if (tes != 0) {
		if (tcs != 0) glDetachShader(program, tcs);
		glDetachShader(program, tes);
	}
	
	glCheckError();
	return program;
}

std::string AddLineNumber(const std::string& str)
{
	stringstream ss;
	int line_number = 2;
	ss << "#1\t";
	string::size_type last_pos = 0;
	auto pos = str.find('\n');
	while (pos != string::npos) {
		ss << str.substr(last_pos, pos - last_pos) << "\n#" << line_number << "\t";
		last_pos = pos + 1;
		pos = str.find('\n', last_pos);
		line_number++;
	};
	ss << str.substr(last_pos);
	return ss.str();
};


const char* GLenumToString(GLenum e)
{
	switch (e) {
	case GL_FLOAT:
		return "GL_FLOAT";
	case GL_FLOAT_VEC2:
		return "GL_FLOAT_VEC2";
	case GL_FLOAT_VEC3:
		return "GL_FLOAT_VEC3";
	case GL_FLOAT_VEC4:
		return "GL_FLOAT_VEC4";
	case GL_FLOAT_MAT4:
		return "GL_FLOAT_MAT4";
	case GL_SAMPLER_2D:
		return "GL_SAMPLER_2D";
	case GL_SAMPLER_2D_ARRAY:
		return "GL_SAMPLER_2D_ARRAY";
	case GL_SAMPLER_3D:
		return "GL_SAMPLER_3D";
	case GL_SAMPLER_CUBE:
		return "GL_SAMPLER_CUBE";
	case GL_SAMPLER_2D_ARRAY_SHADOW:
		return "GL_SAMPLER_2D_ARRAY_SHADOW";
	default:
		abort();
		return "UNKNOWN";
		break;
	}
}

bool UniformIsTexture(GLenum type)
{
	return (type == GL_SAMPLER_2D || type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_3D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_ARRAY_SHADOW);
}


namespace FishEngine
{
	class ShaderImpl
	{
	public:

		ShaderImpl() = default;

		~ShaderImpl()
		{
			for (auto& e : m_keywordToGLPrograms)
			{
				glDeleteProgram(e.second);
			}
		}

		void set(const std::string& shaderText)
		{
			m_shaderTextRaw = shaderText;
		}

		GLuint CompileAndLink(ShaderKeywords keywords)
		{
			//Debug::LogWarning("CompileAndLink %s", m_filePath.c_str());
			auto vs = Compile(ShaderType::VertexShader, keywords);
			GLuint gs = 0;
			if (m_hasGeometryShader)
				gs = Compile(ShaderType::GeometryShader, keywords);
			auto fs = Compile(ShaderType::FragmentShader, keywords);
			//auto glsl_program = LinkShader(vs, 0, 0, gs, fs);
			GLuint glsl_program = LinkShader(vs, 0, 0, gs, fs);
			if (m_transformFeedback)
			{
				glsl_program = LinkShader_tf(vs, 0, 0, gs, fs);
			}
			else
			{
				glsl_program = LinkShader(vs, 0, 0, gs, fs);
			}
			m_keywordToGLPrograms[keywords] = glsl_program;
			GetAllUniforms(glsl_program);
			glDeleteShader(vs);
			glDeleteShader(fs);
			if (gs != 0) glDeleteShader(gs);
			glCheckError();
			return glsl_program;
		}

		GLuint glslProgram(ShaderKeywords keywords, std::vector<UniformInfo>& uniforms)
		{
			GLuint program = 0;
			auto it = m_keywordToGLPrograms.find(keywords);
			if (it != m_keywordToGLPrograms.end())
			{
				program = it->second;
			}
			else
			{
				program = CompileAndLink(keywords);
			}
			uniforms = m_GLProgramToUniforms[program];
			return program;
		}

		const std::string& shaderTextRaw() const
		{
			return m_shaderTextRaw;
		}

		bool m_transformFeedback = false;
		
		bool m_hasGeometryShader = false;
		uint32_t m_lineCount;   // for error message

	//private:
		//std::string                         m_filePath;
		std::string                         m_shaderTextRaw;
		std::map<ShaderKeywords, GLuint>    m_keywordToGLPrograms;
		std::map<GLuint, std::vector<UniformInfo>>
			m_GLProgramToUniforms;
		int m_renderQueue = -1;


		GLuint Compile(ShaderType type, ShaderKeywords keywords)
		{
			std::string text = "#version 410 core\n";
			m_lineCount = 1;

			auto add_macro_definition = [&text, this](const string& d)
			{
				text += "#define " + d + "\n";
				this->m_lineCount++;
			};

			GLenum t = GL_VERTEX_SHADER;

			if (type == ShaderType::VertexShader)
			{
				add_macro_definition("VERTEX_SHADER");
			}
			else if (type == ShaderType::FragmentShader)
			{
				add_macro_definition("FRAGMENT_SHADER");
				t = GL_FRAGMENT_SHADER;
			}
			else if (type == ShaderType::GeometryShader)
			{
				t = GL_GEOMETRY_SHADER;
				add_macro_definition("GEOMETRY_SHADER");
			}

//			if (keywords & static_cast<ShaderKeywords>(ShaderKeyword::SkinnedAnimation))
//			{
//				add_macro_definition("_SKINNED");
//			}
			if (keywords & static_cast<ShaderKeywords>(ShaderKeyword::AmbientIBL))
			{
				add_macro_definition("_AMBIENT_IBL");
			}

			text += m_shaderTextRaw;

			return CompileShader(t, text);
		}

		void GetAllUniforms(GLuint program) noexcept
		{
			std::vector<UniformInfo> uniforms;
			GLuint blockID = glGetUniformBlockIndex(program, "PerCameraUniforms");
			GLint blockSize = 0;
			//assert(blockID != GL_INVALID_INDEX);
			if (blockID != GL_INVALID_INDEX)
			{
				//Debug::Log("%s", "use PerCameraUniforms");
				glUniformBlockBinding(program, blockID, Pipeline::PerCameraUBOBindingPoint);
				glGetActiveUniformBlockiv(program, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
				assert(blockSize == sizeof(PerCameraUniforms));
			}

			blockID = glGetUniformBlockIndex(program, "PerDrawUniforms");
			blockSize = 0;
			//assert(blockID != GL_INVALID_INDEX);
			if (blockID != GL_INVALID_INDEX)
			{
				//Debug::Log("%s", "use PerDrawUniforms");
				glUniformBlockBinding(program, blockID, Pipeline::PerDrawUBOBindingPoint);
				glGetActiveUniformBlockiv(program, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
				assert(blockSize == sizeof(PerDrawUniforms));
			}

			blockID = glGetUniformBlockIndex(program, "LightingUniforms");
			//assert(blockID != GL_INVALID_INDEX);
			if (blockID != GL_INVALID_INDEX)
			{
				//Debug::Log("%s", "use LightingUniforms");
				glUniformBlockBinding(program, blockID, Pipeline::LightingUBOBindingPoint);
				glGetActiveUniformBlockiv(program, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
				assert(blockSize == sizeof(LightingUniforms));
			}

			blockID = glGetUniformBlockIndex(program, "Bones");
			//assert(blockID != GL_INVALID_INDEX);
			if (blockID != GL_INVALID_INDEX)
			{
				//Debug::Log("%s", "use Bones");
				glUniformBlockBinding(program, blockID, Pipeline::BonesUBOBindingPoint);
				glGetActiveUniformBlockiv(program, blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
				assert(blockSize == sizeof(Bones));
			}

			GLint count;
			GLint size; // size of the variable
			GLenum type; // type of the variable (float, vec3 or mat4, etc)
			const GLsizei bufSize = 32; // maximum name length
			GLchar name[bufSize]; // variable name in GLSL
			GLsizei length; // name length

			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

			int texture_count = 0;
			for (int i = 0; i < count; i++)
			{
				glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type, name);
				GLint loc = glGetUniformLocation(program, name);
				if (loc != GL_INVALID_INDEX)
				{
					//Debug::Log("Uniform #%d Type: %s Name: %s Loc: %d", i, GLenumToString(type), name, loc);
					UniformInfo u;
					u.type = type;
					u.name = name;
					u.location = loc;
					if (UniformIsTexture(type))
					{
						u.textureBindPoint = texture_count;
						texture_count++;
					}
					else {
						u.textureBindPoint = -1;
					}
					u.binded = false;
					uniforms.emplace_back(u);
				}
			}
			m_GLProgramToUniforms[program] = uniforms;
		}
	};

} /* FishEngine */



namespace FishEngine
{

	std::map<std::string, ShaderPtr> Shader::m_builtinShaders;

	Shader::Shader()
	{
		m_impl = std::make_unique<ShaderImpl>();
	}

	ShaderPtr Shader::CreateFromFile(const Path& path)
	{
		LogInfo("Compiling " + path.string());
		auto s = MakeShared<Shader>();
		if (s->FromFile(path))
			return s;
		return nullptr;
	}

	bool Shader::FromFile(const Path& path)
	{
		try
		{
			ShaderCompiler compiler(path);
			if (path.stem() == "Internal-GPUSkinning")
			{
				m_impl->m_transformFeedback = true;
			}
			std::string parsed_shader_text = compiler.Preprocess();
			std::map<std::string, std::string> settings = compiler.m_settings;
			m_impl->m_hasGeometryShader = compiler.m_hasGeometryShader;
			m_cullface = ToEnum<Cullface>(Capitalize(GetValueOrDefault<string, string>(settings, "cull", "back")));
			m_ZWrite = GetValueOrDefault<string, string>(settings, "zwrite", "on") == "on";
			//m_blend = GetValueOrDefault<string, string>(settings, "blend", "off") == "on";
			m_deferred = GetValueOrDefault<string, string>(settings, "deferred", "off") == "on";
			m_blend = compiler.m_blendEnabled;
			m_blendFactorCount = compiler.m_blendFactorCount;
			for (int i = 0; i < m_blendFactorCount; ++i)
			{
				m_blendFactors[i] = compiler.m_blendFactors[i];
			}
			m_savedProperties = compiler.m_savedProperties;

			m_impl->set(parsed_shader_text);
			//m_impl->CompileAndLink(m_keywords);
			//m_GLNativeProgram = m_impl->glslProgram(m_keywords, m_uniforms);
		}
		catch (const std::exception& e)
		{
			PrintErrorMessage(e.what());
			return false;
		}
		return true;
	}

	void Shader::PrintErrorMessage(std::string const & errorMessage) noexcept
	{
		LogError(errorMessage);

		std::vector<std::string> lines;
		boost::split(lines, errorMessage, boost::is_any_of("\n"));
		for (auto & line : lines)
		{
#if FISHENGINE_PLATFORM_WINDOWS
			auto begin = line.find_first_of('(');
			auto end = line.find_first_of(')');
#else
			auto begin = line.find_first_of(':');
			begin = line.find_first_of(':', begin + 1);
			auto end = line.find_first_of(':', begin + 1);
#endif
			uint32_t line_number = 0;
			try
			{
				line_number = boost::lexical_cast<uint32_t>(line.substr(begin + 1, end - begin - 1));
			}
			catch(exception const &)
			{
				continue;
			}
			//cout << line_number << endl;
			uint32_t start_line = m_impl->m_lineCount;
			auto& text = m_impl->shaderTextRaw();

			string::size_type cursor = 0;

			int first = line_number - start_line - 5;
			if (first < 0)
				first = 0;

			for (int i = 0; i < first - 1; ++i)
			{
				cursor = text.find_first_of('\n', cursor) + 1;
			}

			int total = static_cast<int>(std::count(text.begin(), text.end(), '\n') + 1);
			int last = static_cast<int>(line_number - start_line + 5);
			if (last >= total)
				last = total - 1;

			std::ostringstream context_lines;
			for (int i = first; i < last; ++i)
			{
				auto new_cursor = text.find_first_of('\n', cursor);
				context_lines << '#' << i + start_line << '\t' << text.substr(cursor, new_cursor - cursor) << '\n';
				cursor = new_cursor + 1;
			}
			LogError(line);
			LogInfo(context_lines.str());
			cout << endl;
		}
	}

	Shader::~Shader()
	{
	}

	void Shader::Use() noexcept
	{
		if (m_GLNativeProgram == 0)
		{
			try {
				m_impl->CompileAndLink(m_keywords);
				m_GLNativeProgram = m_impl->glslProgram(m_keywords, m_uniforms);
			}
			catch (const std::exception & e)
			{
				PrintErrorMessage(e.what());
				m_GLNativeProgram = 0;
			}
		}
		//if (m_GLNativeProgram == 0)
		//	abort();
		//assert(m_GLNativeProgram != 0);
		glUseProgram(m_GLNativeProgram);
		for (auto& u : m_uniforms)
		{
			if (u.textureBindPoint >= 0)
			{
				glProgramUniform1i(m_GLNativeProgram, u.location, u.textureBindPoint);
			}
		}
	}

	bool Shader::HasUniform(const std::string& name)
	{
		for (auto& u : m_uniforms)
		{
			if (u.name == name)
				return true;
		}
		return false;
	}

	void Shader::BindUniformVec4(const char* name, const Vector4& value)
	{
		for (auto& u : m_uniforms)
		{
			if (u.name == name)
			{
				glProgramUniform4fv(m_GLNativeProgram, u.location, 1, value.data());
				u.binded = true;
				return; 
			}
		}
		LogWarning(Format( "Uniform %1% not found!", name ));
	}

	void Shader::BindUniformMat4(const char* name, const Matrix4x4& value)
	{
		for (auto& u : m_uniforms)
		{
			if (boost::starts_with(u.name, name))
			{
				glProgramUniformMatrix4fv(m_GLNativeProgram, u.location, 1, GL_TRUE, value.data());
				u.binded = true;
				return;
			}
		}
		LogWarning(Format("Uniform %1% not found!", name));
	}

	void Shader::BindMatrixArray(const std::string& name, const std::vector<Matrix4x4>& matrixArray)
	{
		for (auto& u : m_uniforms)
		{
			if (boost::starts_with(u.name, name))
			{
				glProgramUniformMatrix4fv(m_GLNativeProgram, u.location, static_cast<GLsizei>(matrixArray.size()), GL_TRUE, matrixArray.data()->data());
				u.binded = true;
				return;
			}
		}
		LogWarning(Format("Uniform %1% not found!", name));
	}

	void Shader::BindUniforms(const ShaderUniforms& uniforms)
	{
		for (auto& u : m_uniforms)
		{
			if (u.type == GL_FLOAT_MAT4)
			{
				auto it = uniforms.mat4s.find(u.name);
				if (it != uniforms.mat4s.end())
				{
					glProgramUniformMatrix4fv(m_GLNativeProgram, u.location, 1, GL_TRUE, it->second.data());
					u.binded = true;
				}
				else {
					LogWarning(Format( "%1% of type %2% not found", u.name, u.type));
				}
			}
			else if (u.type == GL_FLOAT_VEC2)
			{
				auto it = uniforms.vec2s.find(u.name);
				if (it != uniforms.vec2s.end())
				{
					glProgramUniform2fv(m_GLNativeProgram, u.location, 1, it->second.data());
					u.binded = true;
				}
				else
				{
					LogWarning(Format("%1% of type %2% not found", u.name, u.type));
				}
			}
			else if (u.type == GL_FLOAT_VEC3)
			{
				auto it = uniforms.vec3s.find(u.name);
				if (it != uniforms.vec3s.end())
				{
					glProgramUniform3fv(m_GLNativeProgram, u.location, 1, it->second.data());
					u.binded = true;
				}
				else
				{
					LogWarning(Format("%1% of type %2% not found", u.name, u.type));
				}
			}
			else if (u.type == GL_FLOAT)
			{
				auto it = uniforms.floats.find(u.name);
				if (it != uniforms.floats.end())
				{
					glProgramUniform1f(m_GLNativeProgram, u.location, it->second);
					u.binded = true;
				}
			}
			else if (u.type == GL_FLOAT_VEC4)
			{
				auto it = uniforms.vec4s.find(u.name);
				if (it != uniforms.vec4s.end())
				{
					glProgramUniform4fv(m_GLNativeProgram, u.location, 1, it->second.data());
					u.binded = true;
				}
			}
		}
		glCheckError();
	}

	void Shader::BindTexture(const std::string& name, TexturePtr texture)
	{
		//abort();
		std::map<std::string, TexturePtr> dict;
		dict[name] = texture;
		BindTextures(dict);
	}

	void Shader::BindTextures(const std::map<std::string, TexturePtr>& textures)
	{
		for (auto& u : m_uniforms)
		{
			if (!(u.type == GL_SAMPLER_2D || u.type == GL_SAMPLER_CUBE || u.type == GL_SAMPLER_2D_ARRAY || u.type == GL_SAMPLER_2D_ARRAY_SHADOW))
				continue;
			auto it = textures.find(u.name);
			if (it != textures.end())
			{
				GLenum type = GL_TEXTURE_2D;
				if (u.type == GL_SAMPLER_CUBE)
					type = GL_TEXTURE_CUBE_MAP;
				else if (u.type == GL_SAMPLER_2D_ARRAY || u.type == GL_SAMPLER_2D_ARRAY_SHADOW)
					type = GL_TEXTURE_2D_ARRAY;
				//BindUniformTexture(u.name.c_str(), it->second->GLTexuture(), texture_id, type);
				glActiveTexture(GLenum(GL_TEXTURE0 + u.textureBindPoint));
				glCheckError();
				glBindTexture(type, it->second->GetNativeTexturePtr());
				u.binded = true;
				glCheckError();
			}
//            else
//            {
//                Debug::LogWarning("%s of type %s not found", u.name.c_str(), GLenumToString(u.type));
//            }
		}
	}
				
				
	static GLuint ShaderBlendFactorToGL(ShaderBlendFactor factor)
	{
		switch (factor)
		{
		case ShaderBlendFactor::One:
			return GL_ONE;
		case ShaderBlendFactor::Zero:
			return GL_ZERO;
		case ShaderBlendFactor::SrcColor:
			return GL_SRC_COLOR;
		case ShaderBlendFactor::SrcAlpha:
			return GL_SRC_ALPHA;
		case ShaderBlendFactor::DstColor:
			return GL_DST_COLOR;
		case ShaderBlendFactor::DstAlpha:
			return GL_DST_ALPHA;
		case ShaderBlendFactor::OneMinusSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		case ShaderBlendFactor::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case ShaderBlendFactor::OneMinusDstColor:
			return GL_ONE_MINUS_DST_COLOR;
		default:
		case ShaderBlendFactor::OneMinusDstAlpha:
			return GL_ONE_MINUS_DST_ALPHA;
		}
	}

	void Shader::PreRender() const
	{
		if (m_cullface == Cullface::Off)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glCullFace((GLenum)m_cullface);
		}
		glDepthMask(m_ZWrite);
		if (m_blend)
		{
			glEnable(GL_BLEND);
			if (m_blendFactorCount == 2)
			{
				auto f1 = ShaderBlendFactorToGL(m_blendFactors[0]);
				auto f2 = ShaderBlendFactorToGL(m_blendFactors[1]);
				glBlendFunc(f1, f2);
			}
			else
			{
				auto f1 = ShaderBlendFactorToGL(m_blendFactors[0]);
				auto f2 = ShaderBlendFactorToGL(m_blendFactors[1]);
				auto f3 = ShaderBlendFactorToGL(m_blendFactors[2]);
				auto f4 = ShaderBlendFactorToGL(m_blendFactors[3]);
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendFuncSeparate(f1, f2, f3, f4);
			}
		}
	}

	void Shader::PostRender() const
	{
		glDepthMask(GL_TRUE);
		if (m_cullface == Cullface::Off)
			glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		if (m_blend)
			glDisable(GL_BLEND);
	}

	void Shader::CheckStatus() const
	{
		for (auto& u : m_uniforms)
		{
			if (!u.binded)
			{
				LogWarning(Format( "Uniform %1%[%2%] not binded!", u.name.c_str(), GLenumToString(u.type) ));
			}
		}
	}

	ShaderPtr Shader::FindBuiltin(const std::string& name)
	{
		auto it = m_builtinShaders.find(name);
		if (it != m_builtinShaders.end())
		{
			return it->second;
		}
		LogWarning("No built-in shader called " + name);
		abort();
		return nullptr;
	}

	void Shader::SetLocalKeywords(ShaderKeyword keyword, bool value)
	{
		auto k = static_cast<ShaderKeywords>(keyword);
		if (value)
		{
			EnableLocalKeywords(k);
		}
		else
		{
			DisableLocalKeywords(k);
		}
	}

	void Shader::EnableLocalKeywords(ShaderKeywords keyword)
	{
		m_keywords |= keyword;
		m_GLNativeProgram = m_impl->glslProgram(m_keywords, m_uniforms);
	}

	void Shader::DisableLocalKeywords(ShaderKeywords keyword)
	{
		m_keywords &= ~keyword;
		m_GLNativeProgram = m_impl->glslProgram(m_keywords, m_uniforms);
	}

	int Shader::renderQueue()
	{
		if (m_impl->m_renderQueue < 0)
		{
			if (m_blend)
				return static_cast<int>(Rendering::RenderQueue::Transparent);
			return static_cast<int>(Rendering::RenderQueue::Geometry);
		}
		else
		{
			return m_impl->m_renderQueue;
		}
	}

	bool Shader::IsValid()
	{
		return m_GLNativeProgram != 0;
	}

	//========== Static Region ==========

	void Shader::Init(std::string const & rootDir)
	{
		Path root_dir = rootDir;
		for (auto& n : { "PBR", "PBR-Reference", "Diffuse", "DebugCSM", "Texture", "Transparent" })
		{
			m_builtinShaders[n] = Shader::CreateFromFile(root_dir / (string(n) + ".surf"));
			m_builtinShaders[n]->setName(n);
		}

		for (auto& n : { "ScreenTexture", "Deferred", "CascadedShadowMap",
			"DisplayCSM", "DrawQuad", "GatherScreenSpaceShadow", "SolidColor",
			"PostProcessShadow", "PostProcessGaussianBlur", "PostProcessSelectionOutline", "Internal-GPUSkinning" })
		{
			m_builtinShaders[n] = Shader::CreateFromFile(root_dir / (string(n) + ".shader"));
			m_builtinShaders[n]->setName(n);
		}

		m_builtinShaders["SkyboxCubed"] = Shader::CreateFromFile(root_dir / "Skybox-Cubed.shader");
		m_builtinShaders["SkyboxProcedural"] = Shader::CreateFromFile(root_dir / "Skybox-Procedural.shader");
		m_builtinShaders["SolidColor-Internal"] = Shader::CreateFromFile(root_dir / "Editor/SolidColor.shader");
		m_builtinShaders["SkyboxProcedural"]->setName("SkyboxProcedural");
		m_builtinShaders["SkyboxCubed"]->setName("SkyboxCubed");
		m_builtinShaders["SolidColor-Internal"]->setName("SolidColor-Internal");
	}

}
