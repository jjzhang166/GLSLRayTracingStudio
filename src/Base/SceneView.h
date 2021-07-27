#pragma once

#include "Common/Common.h"
#include "Core/Scene.h"

#include <memory>

class GLWindow;

class SceneView
{
public:

    SceneView() = delete;

    SceneView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene);

    virtual ~SceneView();

    virtual bool Init() = 0;

    virtual void Destroy() = 0;

    virtual void OnUpdate() = 0;

    virtual void OnRender() = 0;

    FORCEINLINE std::shared_ptr<GLWindow> Window() const
    {
        return m_Window;
    }

    FORCEINLINE std::shared_ptr<GLScene> Scene() const
    {
        return m_Scene;
    }

protected:

    std::shared_ptr<GLWindow>       m_Window;
    std::shared_ptr<GLScene>        m_Scene;
};
