#include "cache.h"
#include <unistd.h>
#ifdef __linux__
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
