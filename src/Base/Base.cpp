#include <iostream>

#include "Base/Base.h"
#include "Math/Math.h"

static int32 s_InstanceID = 0;

Object3D::Object3D()
    : instanceID(s_InstanceID++)
{

}

// -----------------------------------------------------

void Cross(const float* a, const float* b, float* r)
{
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

float Dot(const float* a, const float* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void Normalize(const float* a, float* r)
{
    float il = 1.f / (MMath::Sqrt(Dot(a, a)) + 0.0001f);
    r[0] = a[0] * il;
    r[1] = a[1] * il;
    r[2] = a[2] * il;
}

void GizmoFrustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    m16[0] = temp / temp2;
    m16[1] = 0.0;
    m16[2] = 0.0;
    m16[3] = 0.0;
    m16[4] = 0.0;
    m16[5] = temp / temp3;
    m16[6] = 0.0;
    m16[7] = 0.0;
    m16[8] = (right + left) / temp2;
    m16[9] = (top + bottom) / temp3;
    m16[10] = (-zfar - znear) / temp4;
    m16[11] = -1.0f;
    m16[12] = 0.0;
    m16[13] = 0.0;
    m16[14] = (-temp * zfar) / temp4;
    m16[15] = 0.0;
}

void GizmoPerspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16)
{
    float ymax, xmax;
    ymax = znear * tanf(fovyInDegrees * 3.141592f / 180.0f);
    xmax = ymax * aspectRatio;
    GizmoFrustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
}

void GizmoLookAt(const float* eye, const float* at, const float* up, float* m16)
{
    float X[3], Y[3], Z[3], tmp[3];

    tmp[0] = eye[0] - at[0];
    tmp[1] = eye[1] - at[1];
    tmp[2] = eye[2] - at[2];
    //Z.normalize(eye - at);
    Normalize(tmp, Z);
    Normalize(up, Y);
    //Y.normalize(up);

    Cross(Y, Z, tmp);
    //tmp.cross(Y, Z);
    Normalize(tmp, X);
    //X.normalize(tmp);

    Cross(Z, X, tmp);
    //tmp.cross(Z, X);
    Normalize(tmp, Y);
    //Y.normalize(tmp);

    m16[0] = X[0];
    m16[1] = Y[0];
    m16[2] = Z[0];
    m16[3] = 0.0f;
    m16[4] = X[1];
    m16[5] = Y[1];
    m16[6] = Z[1];
    m16[7] = 0.0f;
    m16[8] = X[2];
    m16[9] = Y[2];
    m16[10] = Z[2];
    m16[11] = 0.0f;
    m16[12] = -Dot(X, eye);
    m16[13] = -Dot(Y, eye);
    m16[14] = -Dot(Z, eye);
    m16[15] = 1.0f;
}

Camera::Camera()
{

}

void Camera::GetGizmoViewProjection(float* view, float* projection)
{
    Matrix4x4 global = node->GlobalTransform();

    Vector3 at = global.GetOrigin() + global.GetForward();
    Vector3 p  = global.GetOrigin();
    Vector3 up = global.GetUp();
    GizmoLookAt((float*)&p, (float*)&at, (float*)&up, view);

    float ratio = m_Aspect;
    float fov = (1.f / ratio) * MMath::Tan(m_Fov / 2.f);
    GizmoPerspective(MMath::RadiansToDegrees(fov), ratio, m_Near, m_Far, projection);
}

void Camera::OnMousePos(const Vector2 mousePos)
{
    m_CurrMouse  = mousePos;
}

void Camera::OnRMouse(bool down)
{
    m_RMouseDown = down;
}

void Camera::OnMMouse(bool down)
{
    m_MMouseDown = down;
}

void Camera::OnMouseWheel(float wheel)
{
    m_MouseWheel = wheel;
}

void Camera::Update(float delta)
{
    float mouseSpeedX = m_CurrMouse.x - m_LastMouse.x;
    float mouseSpeedY = m_CurrMouse.y - m_LastMouse.y;

    if (m_MMouseDown)
    {
        node->transform.TranslateX(+mouseSpeedX * node->transform.GetOrigin().Size() / 300);
        node->transform.TranslateY(+mouseSpeedY * node->transform.GetOrigin().Size() / 300);
    }
    else if (m_RMouseDown)
    {
        m_SpinX += -mouseSpeedX * smooth * speedFactor;
        m_SpinY += +mouseSpeedY * smooth * speedFactor;
    }

    if (m_MouseWheel != 0.0f) 
    {
        m_SpinZ = (node->transform.GetOrigin().Size() + 0.1f) * speedFactor * m_MouseWheel / 20.0f;
    }

    node->transform.TranslateZ(m_SpinZ);
    node->transform.RotateY(m_SpinX, false, &Vector3::ZeroVector);
    node->transform.RotateX(m_SpinY, true,  &Vector3::ZeroVector);

    if ((m_SpinX != 0.0f || m_SpinY != 0.0f || m_SpinZ != 0.0f) || m_MMouseDown) 
    {
        isMoving = true;
    }

    m_SpinX *= (1 - smooth);
    m_SpinY *= (1 - smooth);
    m_SpinZ *= (1 - smooth);

    m_LastMouse = m_CurrMouse;
}

void Camera::TranslateX(float distance)
{
    node->transform.TranslateX(distance);
}

void Camera::TranslateY(float distance)
{
    node->transform.TranslateY(distance);
}

void Camera::TranslateZ(float distance)
{
    node->transform.TranslateZ(distance);
}

void Camera::RotateX(float angle)
{
    node->transform.RotateX(angle);
}

void Camera::RotateY(float angle)
{
    node->transform.RotateY(angle);
}

void Camera::RotateZ(float angle)
{
    node->transform.RotateZ(angle);
}

void Camera::LookAt(float x, float y, float z, float smooth)
{
    LookAt(Vector3(x, y, z), smooth);
}

void Camera::LookAt(const Vector3& target, float smooth)
{
    node->transform.LookAt(target, nullptr, smooth);
}

void Camera::LookAt(const Vector3& target, const Vector3& up, float smooth)
{
    node->transform.LookAt(target, &up, smooth);
}

Vector3 Camera::GetPosition(bool local)
{
    if (local)
    {
        return node->transform.GetOrigin();
    }
    else
    {
        return node->GlobalTransform().GetOrigin();
    }
}

void Camera::SetPosition(const Vector3& pos)
{
    node->transform.SetPosition(pos);
}

void Camera::SetPosition(float x, float y, float z)
{
    node->transform.SetPosition(Vector3(x, y, z));
}

void Camera::SetOrientation(const Vector3& dir)
{
    node->transform.SetOrientation(dir, &Vector3::UpVector, 1.0f);
}

void Camera::SetRotation(const Vector3& rotation)
{
    node->transform.SetRotation(rotation);
}

void Camera::SetRotation(float eulerX, float eulerY, float eulerZ)
{
    node->transform.SetRotation(Vector3(eulerX, eulerY, eulerZ));
}

Vector3 Camera::GetRight(bool local) const
{
    if (local)
    {
        return node->transform.GetRight();
    }
    else
    {
        return node->GlobalTransform().GetRight();
    }
}

Vector3 Camera::GetUp(bool local) const
{
    if (local)
    {
        return node->transform.GetUp();
    }
    else
    {
        return node->GlobalTransform().GetUp();
    }
}

Vector3 Camera::GetForward(bool local) const
{
    if (local)
    {
        return node->transform.GetForward();
    }
    else
    {
        return node->GlobalTransform().GetForward();
    }
}

Vector3 Camera::GetLeft(bool local) const
{
    if (local)
    {
        return node->transform.GetLeft();
    }
    else
    {
        return node->GlobalTransform().GetLeft();
    }
}

Vector3 Camera::GetBackward(bool local) const
{
    if (local)
    {
        return node->transform.GetBackward();
    }
    else
    {
        return node->GlobalTransform().GetBackward();
    }
}

Vector3 Camera::GetDown(bool local) const
{
    if (local)
    {
        return node->transform.GetDown();
    }
    else
    {
        return node->GlobalTransform().GetDown();
    }
}

const Matrix4x4& Camera::GetView()
{
    m_View = node->GlobalTransform().Inverse();
    return m_View;
}

const Matrix4x4& Camera::GetProjection()
{
    return m_Projection;
}

const Matrix4x4& Camera::GetViewProjection()
{
    m_View = node->GlobalTransform().Inverse();
    m_ViewProjection = m_View * m_Projection;
    return m_ViewProjection;
}

void Camera::SetTransform(const Matrix4x4& world)
{
    node->transform = world;
}

Matrix4x4 Camera::GetTransform(bool local)
{
    if (local)
    {
        return node->transform;
    }
    else
    {
        return node->GlobalTransform();
    }
}

void Camera::Perspective(float fovy, float aspect, float zNear, float zFar)
{
    m_Fov    = fovy;
    m_Near   = zNear;
    m_Far    = zFar;
    m_Aspect = aspect;

    m_Projection.Perspective(fovy, aspect, zNear, zFar);
}

void Camera::SetFov(float fov)
{
    m_Fov = fov;
    m_Projection.Perspective(m_Fov, m_Aspect, m_Near, m_Far);
}

void Camera::SetAspect(float aspect)
{
    m_Aspect = aspect;
    m_Projection.Perspective(m_Fov, m_Aspect, m_Near, m_Far);
}
