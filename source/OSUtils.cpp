#include "OSUtils.h"

#ifdef __APPLE__

#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#endif

namespace OSUtils {

folly::Optional<std::string> getExecutablePath() {
#ifdef __APPLE__
   uint32_t size = MAXPATHLEN;
   char rawPath[size];
   if (_NSGetExecutablePath(rawPath, &size) != 0) {
      return folly::none;
   }

   char realPath[size];
   if (!realpath(rawPath, realPath)) {
      return folly::none;
   }

   return std::string(realPath);
#endif
}

folly::Optional<std::string> getDirectoryFromPath(const std::string &path) {
   size_t pos = path.find_last_of("/\\");
   if (pos == std::string::npos) {
      return folly::none;
   }

   return path.substr(0, pos);
}

bool setWorkingDirectory(const std::string &dir) {
#ifdef __APPLE__
   return chdir(dir.c_str()) == 0;
#endif
}

bool fixWorkingDirectory() {
   folly::Optional<std::string> path(getExecutablePath());
   if (!path) {
      return false;
   }

   folly::Optional<std::string> dir(getDirectoryFromPath(*path));
   if (!dir) {
      return false;
   }

   if (!setWorkingDirectory(*dir)) {
      return false;
   }

   return true;
}

} // namespace OSUtils
