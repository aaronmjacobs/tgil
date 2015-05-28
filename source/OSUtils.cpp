#include "Constants.h"
#include "OSUtils.h"

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif // __APPLE__

#ifdef __linux__
#include <cstring>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif // __linux__

#ifdef _WIN32
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <Windows.h>
#endif // _WIN32

namespace OSUtils {

#ifdef __APPLE__
folly::Optional<std::string> getExecutablePath() {
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
}

folly::Optional<std::string> getAppDataPath() {
   FSRef ref;
   FSFindFolder(kUserDomain, kApplicationSupportFolderType, kCreateFolder, &ref);

   char path[PATH_MAX];
   FSRefMakePath(&ref, (UInt8*)&path, PATH_MAX);

   std::string appDataPath(path);
   appDataPath += "/" PROJECT_NAME;

   return appDataPath;
}

bool setWorkingDirectory(const std::string &dir) {
   return chdir(dir.c_str()) == 0;
}

bool createDirectory(const std::string &dir) {
   return mkdir(dir.c_str(), 0755) == 0;
}
#endif // __APPLE__

#ifdef __linux__
folly::Optional<std::string> getExecutablePath() {
   char path[PATH_MAX + 1];

   ssize_t numBytes = readlink("/proc/self/exe", path, PATH_MAX);
   if (numBytes == -1) {
      return folly::none;
   }
   path[numBytes] = '\0';

   return std::string(path);
}

folly::Optional<std::string> getAppDataPath() {
   // First, check the HOME environment variable
   char *homePath = secure_getenv("HOME");

   // If it isn't set, grab the directory from the password entry file
   if (!homePath) {
      homePath = getpwuid(getuid())->pw_dir;
   }

   if (!homePath) {
      return folly::none;
   }

   std::string appDataPath(homePath);
   appDataPath += "/.config/" PROJECT_NAME;

   return appDataPath;
}

bool setWorkingDirectory(const std::string &dir) {
   return chdir(dir.c_str()) == 0;
}

bool createDirectory(const std::string &dir) {
   return false;
}
#endif // __linux__

#ifdef _WIN32
folly::Optional<std::string> getExecutablePath() {
   TCHAR buffer[MAX_PATH + 1];
   DWORD length = GetModuleFileName(NULL, buffer, MAX_PATH);
   buffer[length] = '\0';
   int error = GetLastError();

   if (length == 0 || length == MAX_PATH || error == ERROR_INSUFFICIENT_BUFFER) {
      const DWORD REALLY_BIG = 32767;
      TCHAR unreasonablyLargeBuffer[REALLY_BIG + 1];
      length = GetModuleFileName(NULL, unreasonablyLargeBuffer, REALLY_BIG);
      unreasonablyLargeBuffer[length] = '\0';
      error = GetLastError();

      if (length == 0 || length == REALLY_BIG || error == ERROR_INSUFFICIENT_BUFFER) {
         return folly::none;
      }

      return std::string(unreasonablyLargeBuffer);
   }

   return std::string(buffer);
}

folly::Optional<std::string> getAppDataPath() {
   PWSTR path;
   if (SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, nullptr, &path) != S_OK) {
      return folly::none;
   }
}

bool setWorkingDirectory(const std::string &dir) {
   return SetCurrentDirectory(dir.c_str()) != 0;
}

bool createDirectory(const std::string &dir) {
   return false;
}
#endif // _WIN32

bool createAppDataDirectory() {
   folly::Optional<std::string> appDataPath(getAppDataPath());

   if (!appDataPath) {
      return false;
   }

   if (directoryExists(*appDataPath)) {
      return true;
   }

   return createDirectory(*appDataPath);
}

folly::Optional<std::string> getDirectoryFromPath(const std::string &path) {
   size_t pos = path.find_last_of("/\\");
   if (pos == std::string::npos) {
      return folly::none;
   }

   return path.substr(0, pos);
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

bool directoryExists(const std::string &dir) {
   struct stat info;

   if(stat(dir.c_str(), &info) != 0) {
      return false;
   }

   return (info.st_mode & S_IFDIR) != 0;
}

} // namespace OSUtils
