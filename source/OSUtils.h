#ifndef OS_UTILS_H
#define OS_UTILS_H

#include <folly/Optional.h>

#include <string>

namespace OSUtils {

/**
 * Gets the path to the running executable
 */
folly::Optional<std::string> getExecutablePath();

/**
 * Extracts the directory from the given path
 */
folly::Optional<std::string> getDirectoryFromPath(const std::string &path);

/**
 * Sets the working directory of the application to the given directory, returning true on success
 */
bool setWorkingDirectory(const std::string &dir);

/**
 * Sets the working directory of the application to the directory that the executable is in, returning true on success
 */
bool fixWorkingDirectory();

} // namespace OSUtils

#endif
