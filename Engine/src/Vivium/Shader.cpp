#include "Shader.h"

namespace Vivium {
    GLuint Shader::m_CompileShader(const std::string& source, unsigned int type)
    {
        VIVIUM_SCOPE;

        // Create shader
        GLuint id = glCreateShader(type);
        // Pass text into shader
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        // Compile shader
        glCompileShader(id);

        GLint result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);

        // Check for compilation errors
        if (result == GL_FALSE) {
            GLint length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            // Create buffer to store compilation error in
            char* message = new char[length];

            glGetShaderInfoLog(id, length, &length, message);

            // Check on which shader the error occured
            const char* shader_type;
            shader_type = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";

            LogError("{} shader compilation failed: {}", shader_type, message);

            delete[] message;

            // Delete shader since compilation failed
            glDeleteShader(id);

            return 0;
        }

        return id;
    }

    GLuint Shader::m_CreateShader(const char* vertex_name, const char* frag_name)
    {
        VIVIUM_SCOPE;

        // TODO: use application path
        const std::string PATH = "../Resources/shaders/";
        // TODO: this extension has caused 1000 bugs just remove it
        const std::string EXT = ".glsl";

        if (Vivium::Flag::Test(VIVIUM_FLAG_PRINT_SHADER_COMPILATION)) {
            LogTrace("Compiling shaders: {}, {}", vertex_name, frag_name);
        }

        GLuint program = glCreateProgram();

        // Read files and return pointer to string
        std::string* vertex_shader = Utils::ReadFile(PATH + vertex_name + EXT);
        std::string* fragment_shader = Utils::ReadFile(PATH + frag_name + EXT);

        // Compile the shaders
        GLuint vs = m_CompileShader(*vertex_shader, GL_VERTEX_SHADER);
        GLuint fs = m_CompileShader(*fragment_shader, GL_FRAGMENT_SHADER);

        // Delete the text
        delete vertex_shader;
        delete fragment_shader;

        // Attach vertex and fragment shader to program
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        // Compile program
        glValidateProgram(program);

        return program;
    }

    Shader::Shader(const char* vs_file, const char* fs_file)
    {
        m_ID = m_CreateShader(
            vs_file,
            fs_file
        );
        Bind();
    }

    Shader::~Shader()
    {
        // Delete program
        glDeleteProgram(m_ID);
    }

    void Shader::Unbind()
    {
        glUseProgram(NULL);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_ID);
    }

    void Shader::SetUniform1i(const char* name, int x)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform1i(location, x);
    }

    void Shader::SetUniform1ui(const char* name, unsigned int x)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform1ui(location, x);
    }

    void Shader::SetUniform1f(const char* name, float x)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform1f(location, x);
    }

    void Shader::SetUniform2f(const char* name, float x, float y)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform2f(location, x, y);
    }

    void Shader::SetUniform2f(const char* name, Vector2<float> v)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform2f(location, v.x, v.y);
    }

    void Shader::SetUniform3f(const char* name, float x, float y, float z)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform3f(location, x, y, z);
    }

    void Shader::SetUniform3f(const char* name, const RGBColor& color)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform3f(location, color.r, color.g, color.b);
    }

    void Shader::SetUniform4f(const char* name, float x, float y, float z, float w)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform4f(location, x, y, z, w);
    }

    void Shader::SetUniform4f(const char* name, const RGBColor& color, float a)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform4f(location, color.r, color.g, color.b, a);
    }

    void Shader::SetUniformMat4fv(const char* name, const glm::mat4& mat)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniformMatrix4fv(location, 1, false, &mat[0][0]);
    }

    void Shader::SetUniform1fv(const char* name, unsigned int count, const float* values)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform1fv(location, count, values);
    }

    void Shader::SetUniform1iv(const char* name, unsigned int count, const int* values)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform1iv(location, count, values);
    }

    void Shader::SetUniform1uiv(const char* name, unsigned int count, const unsigned int* values)
    {
        GLuint location = glGetUniformLocation(m_ID, name);
        glUniform1uiv(location, count, values);
    }
}
