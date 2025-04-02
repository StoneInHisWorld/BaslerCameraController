#include "pathDeclaration.h"

fs::path get_directory(string path)
{
	fs::path p(path);
	return p.parent_path();
}

