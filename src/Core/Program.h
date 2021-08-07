#pragma once

#include "Common/Common.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Core/Shader.h"

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <string>

class GLProgram
{
public:

    GLProgram(const std::vector<std::shared_ptr<GLShader>>& shaders);

    virtual ~GLProgram();

    void Active();

    void Deactive();

    void SetUniform1i(const char* name, int32 val);

    void SetUniform1f(const char* name, float val);

    void SetUniform2f(const char* name, const Vector2& val);

    void SetUniform3f(const char* name, const Vector3& val);

    void SetUniform4f(const char* name, const Vector4& val);

    void SetUniform4x4f(const char* name, const Matrix4x4& val);

    void SetTexture(const char* name, GLenum target, GLuint texture, int32 slot);

    GLuint Object();

private:

    GLuint m_Object;
};
