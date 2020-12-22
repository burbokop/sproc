#ifndef CACHE_H
#define CACHE_H

#include <string>

namespace sproc {
/**
 * @brief cache_directory
 * @return directory to save cache data (on linux returns home dir | windown - empty string)
 */
std::string cache_directory();
}
#endif // CACHE_H
