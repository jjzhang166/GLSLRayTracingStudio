#include "Misc/FileMisc.h"

#include <algorithm>
#include <cctype>

std::string GetFileExtension(const std::string& path)
{
    std::string extension;
    std::string::size_type idx = path.rfind('.');
    if (idx != std::string::npos)
    {
        extension = path.substr(idx + 1);
    }

    std::transform(extension.begin(), extension.end(), extension.begin(),
        [] (unsigned char c)
        {
            return std::tolower(c);
        }
    );

    return extension;
}

std::string GetFileName(std::string filename)
{
    const size_t lastSlashIdx = filename.find_last_of("\\/");
    if (std::string::npos != lastSlashIdx)
    {
        filename.erase(0, lastSlashIdx + 1);
    }

    const size_t periodIdx = filename.rfind('.');
    if (std::string::npos != periodIdx)
    {
        filename.erase(periodIdx);
    }

    return filename;
}
