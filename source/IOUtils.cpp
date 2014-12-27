#include "IOUtils.h"

#include <fstream>

namespace IOUtils {

bool canRead(const std::string &fileName) {
   return !!std::ifstream(fileName);
}

bool canReadData(const std::string &fileName) {
   return canRead(dataPath(fileName));
}

folly::Optional<std::string> readFromFile(const std::string& fileName) {
   std::ifstream in(fileName);
   if (!in) {
      return folly::none;
   }

   return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

bool writeToFile(const std::string &fileName, const std::string &data) {
   std::ofstream out(fileName);
   if (!out) {
      return false;
   }

   out << data;
   return true;
}

folly::Optional<std::string> readFromDataFile(const std::string &fileName) {
   return readFromFile(dataPath(fileName));
}

bool writeToDataFile(const std::string &fileName, const std::string &data) {
   return writeToFile(dataPath(fileName), data);
}

} // namespace IOUtils
