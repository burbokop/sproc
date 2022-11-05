#include "cache.h"
#ifdef __linux__
#include <unistd.h>
#include <pwd.h>
#else
// TODO add for other platforms
#endif

std::string sproc::cache_directory() {
#ifdef __linux__
    struct passwd *pw = getpwuid(getuid());
    if(pw) return pw->pw_dir;
#else
// TODO add for other platforms
#endif
    return {};
}
