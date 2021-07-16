#pragma once

#include "Common/Common.h"

#include <memory>

class GLWindow;

class SceneView
{
public:

    SceneView() = delete;

    SceneView(std::shared_ptr<GLWindow> window);

    virtual ~SceneView();

    virtual bool Init() = 0;

    virtual void Destroy() = 0;

    virtual void OnUpdate() = 0;

    virtual void OnRender() = 0;

    FORCEINLINE std::shared_ptr<GLWindow> Window() const
    {
        return m_Window;
    }

protected:

    std::shared_ptr<GLWindow>       m_Window;
};
