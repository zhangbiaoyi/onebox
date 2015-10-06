#include "onebox-common.h"
#include "util-path.h"

/**
 *  \brief Check if a path is absolute
 *
 *  \param path string with the path
 *
 *  \retval 1 absolute
 *  \retval 0 not absolute
 */
int PathIsAbsolute(const char *path) 
{
	if (strlen(path) > 1 && path[0] == '/') {
		return 1;
	}

	return 0;
}

/**
 *  \brief Check if a path is relative
 *
 *  \param path string with the path
 *
 *  \retval 1 relative
 *  \retval 0 not relative
 */
int PathIsRelative(const char *path) 
{
	return PathIsAbsolute(path) ? 0 : 1;
}

