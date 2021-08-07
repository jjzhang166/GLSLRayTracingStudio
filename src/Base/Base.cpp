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
    Vector3 at = m_Transform.GetOrigin() + m_Transform.GetForward();
    Vector3 p  = m_Transform.GetOrigin();
    Vector3 up = m_Transform.GetUp();
    GizmoLookAt((float*)&p, (float*)&at, (float*)&up, view);

    float ratio = m_Aspect;
    float fov = (1.f / ratio) * MMath::Tan(m_Fov / 2.f);
    GizmoPerspective(MMath::RadiansToDegrees(fov), ratio, m_Near, m_Far, projection);
}

void Camera::OnMousePos(const Vector2 mousePos, bool forceLast)
{
    m_CurrMouse = mousePos;

    if (forceLast)
    {
        m_LastMouse = mousePos;
    }
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
        m_Transform.TranslateX(-mouseSpeedX * m_Transform.GetOrigin().Size() / 300);
        m_Transform.TranslateY(+mouseSpeedY * m_Transform.GetOrigin().Size() / 300);
    }
    else if (m_RMouseDown)
    {
        m_SpinX += +mouseSpeedX * smooth * speedFactor;
        m_SpinY += +mouseSpeedY * smooth * speedFactor;
    }

    if (m_MouseWheel != 0.0f) 
    {
        m_SpinZ = (m_Transform.GetOrigin().Size() + 0.1f) * speedFactor * m_MouseWheel / 20.0f;
    }

    m_Transform.TranslateZ(m_SpinZ);
    m_Transform.RotateY(m_SpinX, false, &Vector3::ZeroVector);
    m_Transform.RotateX(m_SpinY, true,  &Vector3::ZeroVector);

    if ((m_SpinX != 0.0f || m_SpinY != 0.0f || m_SpinZ != 0.0f) || m_MMouseDown) 
    {
        isMoving = true;
    }
    else
    {
        isMoving = false;
    }

    m_SpinX *= (1 - smooth);
    m_SpinY *= (1 - smooth);
    m_SpinZ *= (1 - smooth);

    m_LastMouse = m_CurrMouse;
}

void Camera::TranslateX(float distance)
{
    m_Transform.TranslateX(distance);
}

void Camera::TranslateY(float distance)
{
    m_Transform.TranslateY(distance);
}

void Camera::TranslateZ(float distance)
{
    m_Transform.TranslateZ(distance);
}

void Camera::RotateX(float angle)
{
    m_Transform.RotateX(angle);
}

void Camera::RotateY(float angle)
{
    m_Transform.RotateY(angle);
}

void Camera::RotateZ(float angle)
{
    m_Transform.RotateZ(angle);
}

void Camera::LookAt(float x, float y, float z, float smooth)
{
    LookAt(Vector3(x, y, z), smooth);
}

void Camera::LookAt(const Vector3& target, float smooth)
{
    m_Transform.LookAt(target, nullptr, smooth);
}

void Camera::LookAt(const Vector3& target, const Vector3& up, float smooth)
{
    m_Transform.LookAt(target, &up, smooth);
}

Vector3 Camera::GetPosition()
{
    return m_Transform.GetOrigin();
}

void Camera::SetPosition(const Vector3& pos)
{
    m_Transform.SetPosition(pos);
}

void Camera::SetPosition(float x, float y, float z)
{
    m_Transform.SetPosition(Vector3(x, y, z));
}

void Camera::SetOrientation(const Vector3& dir)
{
    m_Transform.SetOrientation(dir, &Vector3::UpVector, 1.0f);
}

void Camera::SetRotation(const Vector3& rotation)
{
    m_Transform.SetRotation(rotation);
}

void Camera::SetRotation(float eulerX, float eulerY, float eulerZ)
{
    m_Transform.SetRotation(Vector3(eulerX, eulerY, eulerZ));
}

Vector3 Camera::GetRight() const
{
    return m_Transform.GetRight();
}

Vector3 Camera::GetUp() const
{
    return m_Transform.GetUp();
}

Vector3 Camera::GetForward() const
{
    return m_Transform.GetForward();
}

Vector3 Camera::GetLeft() const
{
    return m_Transform.GetLeft();
}

Vector3 Camera::GetBackward() const
{
    return m_Transform.GetBackward();
}

Vector3 Camera::GetDown() const
{
    return m_Transform.GetDown();
}

const Matrix4x4& Camera::GetView()
{
    m_View = m_Transform.Inverse();
    return m_View;
}

const Matrix4x4& Camera::GetProjection()
{
    return m_Projection;
}

const Matrix4x4& Camera::GetViewProjection()
{
    m_View = m_Transform.Inverse();
    m_ViewProjection = m_View * m_Projection;
    return m_ViewProjection;
}

void Camera::SetTransform(const Matrix4x4& world)
{
    m_Transform = world;
}

Matrix4x4 Camera::GetTransform()
{
    return m_Transform;
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
