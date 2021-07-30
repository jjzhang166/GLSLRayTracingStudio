﻿#pragma once

#include "Common/Common.h"

#include "Math/Rectangle2D.h"
#include "Math/Vector2.h"

class UISceneView;

class MainMenuBar
{
public:

    MainMenuBar(UISceneView* uiview);

    ~MainMenuBar();

    void Draw();

    FORCEINLINE Rectangle2D GetMenuBarRect() const
    {
        return m_MenuBarRect;
    }

    void HandleMoving();

private:

    UISceneView*    m_UIView;
    bool            m_ShowingAbout;

    Vector2         m_MenuBarMousePos;
    bool            m_MenuBarDragging;

    Rectangle2D     m_MenuBarRect;
};
