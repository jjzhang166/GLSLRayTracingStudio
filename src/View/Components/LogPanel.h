#pragma once

#include "imgui.h"

#include "Common/Common.h"

class LogPanel
{
public:

    LogPanel();

    void Clear();

    void Info(const char* msg);

    void Error(const char* msg);

    void Warning(const char* msg);

    void Draw();

private:

    void FormatLines(int32 oldSize);

private:

    ImGuiTextBuffer     m_TextBuffer;
    ImVector<int32>     m_LineOffsets;
};

LogPanel& Logger();
