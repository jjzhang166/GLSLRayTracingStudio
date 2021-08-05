#include "Misc/FileMisc.h"

#include <algorithm>
#include <cctype>

static std::string s_RootPath;

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

std::string GetFileName(const std::string& file)
{
    std::string filename = file;

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

void SetExePath(const std::string& exePath)
{
    std::string dirPath = exePath.substr(0, exePath.find_last_of("/\\")) + "/";
	for (int i = 0; i < dirPath.size(); ++i) {
		if (dirPath[i] == '\\') {
			dirPath[i] = '/';
		}
	}
    s_RootPath = dirPath;
}

const std::string& GetRootPath()
{
    return s_RootPath;
}
