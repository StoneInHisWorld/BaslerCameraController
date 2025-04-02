#pragma once

#include "declaration.h"
#include <filesystem>

namespace fs = std::filesystem;

fs::path get_directory(string path);
