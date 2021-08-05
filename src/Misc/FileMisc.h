#pragma once

#include <string>

std::string GetFileExtension(const std::string& path);

std::string GetFileName(const std::string& filename);

void SetExePath(const std::string& rootPath);

const std::string& GetRootPath();
