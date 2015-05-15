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
 * Gets the path to the application's local data / settings / config folder
 */
folly::Optional<std::string> getAppDataPath();

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

bool directoryExists(const std::string &dir);

bool createDirectory(const std::string &dir);

bool createAppDataDirectory();

} // namespace OSUtils

#endif
