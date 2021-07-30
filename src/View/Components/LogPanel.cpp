#include "Common/Log.h"
#include "View/Components/LogPanel.h"

static LogPanel s_LogPanel;

LogPanel& Logger()
{
    return s_LogPanel;
}

LogPanel::LogPanel()
{
    Clear();
}

void LogPanel::Clear()
{
    m_TextBuffer.clear();
    m_LineOffsets.clear();
    m_LineOffsets.push_back(0);
}

void LogPanel::FormatLines(int32 oldSize)
{
    for (int32 newSize = m_TextBuffer.size(); oldSize < newSize; ++oldSize)
    {
        if (m_TextBuffer[oldSize] == '\n')
        {
            m_LineOffsets.push_back(oldSize + 1);
        }
    }
}

void LogPanel::Info(const char* msg)
{
    int32 oldSize = m_TextBuffer.size();

    m_TextBuffer.append("[  INFO ]");
    m_TextBuffer.append(msg);

    FormatLines(oldSize);
}

void LogPanel::Error(const char* msg)
{
    int32 oldSize = m_TextBuffer.size();

    m_TextBuffer.append("[ ERROR ]");
    m_TextBuffer.append(msg);

    FormatLines(oldSize);
}

void LogPanel::Warning(const char* msg)
{
    int32 oldSize = m_TextBuffer.size();

    m_TextBuffer.append("[WARNING]");
    m_TextBuffer.append(msg);

    FormatLines(oldSize);
}

void LogPanel::Draw()
{
    ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    const char* bufBegin = m_TextBuffer.begin();
    const char* bufEnd   = m_TextBuffer.end();

    ImGuiListClipper clipper;
    clipper.Begin(m_LineOffsets.Size);
    while (clipper.Step())
    {
        for (int32 lineNo = clipper.DisplayStart; lineNo < clipper.DisplayEnd; ++lineNo)
        {
            const char* lineStart = bufBegin + m_LineOffsets[lineNo];
            const char* lineEnd   = (lineNo + 1 < m_LineOffsets.Size) ? (bufBegin + m_LineOffsets[lineNo + 1] - 1) : bufEnd;

            ImVec4 color;

            if (strstr(lineStart, "[ ERROR ]"))
            {
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                lineStart = lineStart + 9;
            }
            else if (strstr(lineStart, "[WARNING]"))
            {
                color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                lineStart = lineStart + 9;
            }
            else if (strstr(lineStart, "[  INFO ]"))
            {
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                lineStart = lineStart + 9;
            }
            else
            {
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(lineStart, lineEnd);
            ImGui::PopStyleColor();
        }
    }
    clipper.End();

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
}

void LogTrace::LogToConsole(Type type, const ANSICHAR* msg)
{
    if (type == Type::Info)
    {
        Logger().Info(msg);
        fprintf(stdout, "[ DEBUG ]:%s", msg);
    }
    else if (type == Type::Debug)
    {
        Logger().Info(msg);
        fprintf(stdout, "[ INFO  ]:%s", msg);
    }
    else if (type == Type::Warning)
    {
        Logger().Warning(msg);
        fprintf(stdout, "[WARNING]:%s", msg);
    }
    else if (type == Type::Error)
    {
        Logger().Error(msg);
        fprintf(stdout, "[ ERROR ]:%s", msg);
    }
    else if (type == Type::Fatal)
    {
        Logger().Error(msg);
        fprintf(stdout, "[ FAULT ]:%s", msg);
    }
}
