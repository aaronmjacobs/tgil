#ifndef IOUTILS_H
#define IOUTILS_H

#include "Constants.h"
#include "Types.h"

#include <folly/Optional.h>

#include <string>

/**
 * IO utility / helper functions
 */
namespace IOUtils {

/**
 * Gets the absolute path of a resource, given a relative path
 */
inline std::string dataPath(const std::string &fileName) {
   return DATA_DIR "/" + fileName;
}

/**
 * Determines if the file with the given name can be read
 */
bool canRead(const std::string &fileName);

/**
 * Determines if the file with the given (relative to the data directory) name can be read
 */
bool canReadData(const std::string &fileName);

/**
 * Reads the entire contents of the text file with the given name
 */
folly::Optional<std::string> readFromFile(const std::string &fileName);

/**
 * Reads the entire contents of the text file with the given name, relative to the data directory
 */
folly::Optional<std::string> readFromDataFile(const std::string &fileName);

/**
 * Reads the entire contents of the binary file with the given name
 */
UPtr<const unsigned char[]> readFromBinaryFile(const std::string &fileName);

/**
 * Reads the entire contents of the binary file with the given name, relative to the data directory
 */
UPtr<const unsigned char[]> readFromBinaryDataFile(const std::string &fileName);

/**
 * Writes the contents of the given text to the file with the given name
 */
bool writeToFile(const std::string &fileName, const std::string &data);

/**
 * Writes the contents of the given text to the file with the given name, relative to the data directory
 */
bool writeToDataFile(const std::string &fileName, const std::string &data);

} // namespace IOUtils

#endif
