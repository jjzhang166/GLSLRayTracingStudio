#include "Common/Log.h"
#include "Common/Common.h"

#include "Core/Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

static void GetShaderFilePath(const std::string& fullPath, std::string& pathWithoutFileName)
{
    size_t found = fullPath.find_last_of("/\\");
    pathWithoutFileName = fullPath.substr(0, found + 1);
}

static std::string LoadIncludeShader(const std::string& path, std::string includeIndentifier = "#include")
{
    includeIndentifier += ' ';

    std::string fullSourceCode = "";
    std::ifstream file(path);

    if (!file.is_open())
    {
        LOGE("Could not open the shader : %s", path.c_str());
        return fullSourceCode;
    }

    std::string lineBuffer;

    while (std::getline(file, lineBuffer))
    {
        if (lineBuffer.find(includeIndentifier) != lineBuffer.npos)
        {
            lineBuffer.erase(0, includeIndentifier.size());

            std::string pathOfThisFile;
            GetShaderFilePath(path, pathOfThisFile);
            lineBuffer.insert(0, pathOfThisFile);

            fullSourceCode += LoadIncludeShader(lineBuffer);
        }
        else
        {
            fullSourceCode += lineBuffer + '\n';
        }
    }

    file.close();

    return fullSourceCode;
}

static std::string LoadShader(const std::string& path, std::string includeIndentifier = "#include")
{
    std::string sourceCode = LoadIncludeShader(path, includeIndentifier) + '\0';
    return sourceCode;
}

GLShader::GLShader(const std::string& filePath, GLuint shaderType)
{
    std::string source = LoadShader(filePath);

    m_Object = glCreateShader(shaderType);
    LOGI("Compiling Shader %s -> %d\n", filePath.c_str(), int32(m_Object));

    const GLchar *src = (const GLchar *)source.c_str();
    glShaderSource(m_Object, 1, &src, 0);
    glCompileShader(m_Object);

    GLint success = 0;
    glGetShaderiv(m_Object, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        std::string msg;
        GLint logSize = 0;
        glGetShaderiv(m_Object, GL_INFO_LOG_LENGTH, &logSize);
        char *info = new char[logSize + 1];
        glGetShaderInfoLog(m_Object, logSize, NULL, info);
        msg += filePath;
        msg += "\n";
        msg += info;
        delete[] info;
        glDeleteShader(m_Object);
        m_Object = 0;
        LOGE("Shader compilation error %s\n", msg.c_str());
        throw std::runtime_error(msg);
    }
}

GLuint GLShader::Object() const
{
    return m_Object;
}
