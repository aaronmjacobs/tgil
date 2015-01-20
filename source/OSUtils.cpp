#include "OSUtils.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>
#endif // __APPLE__

#ifdef __linux__
#include <cstring>
#include <limits.h>
#include <unistd.h>
#endif // __linux__

#ifdef _WIN32
#include <cstring>
#include <Windows.h>
#endif // _WIN32

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
#endif // __APPLE__

#ifdef __linux__
   char path[PATH_MAX];
   memset(path, 0, sizeof(char) * PATH_MAX);

   if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
      return folly::none;
   }

   return std::string(path);
#endif // __linux__

#ifdef _WIN32
   TCHAR buffer[MAX_PATH];
   memset(buffer, 0, sizeof(TCHAR) * MAX_PATH);
   int success = GetModuleFileName(NULL, buffer, MAX_PATH - 1);
   int error = GetLastError();

   if (!success || error == ERROR_INSUFFICIENT_BUFFER) {
      const DWORD REALLY_BIG = 32768;
      TCHAR unreasonablyLargeBuffer[REALLY_BIG];
      memset(unreasonablyLargeBuffer, 0, sizeof(TCHAR) * REALLY_BIG);
      success = GetModuleFileName(NULL, unreasonablyLargeBuffer, REALLY_BIG - 1);
      error = GetLastError();

      if (!success || error == ERROR_INSUFFICIENT_BUFFER) {
         return folly::none;
      }

      return std::string(unreasonablyLargeBuffer);
   }

   return std::string(buffer);
#endif // _WIN32
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
#endif // __APPLE__

#ifdef __linux__
   return chdir(dir.c_str()) == 0;
#endif // __linux__

#ifdef _WIN32
   return SetCurrentDirectory(dir.c_str()) != 0;
#endif // _WIN32
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
