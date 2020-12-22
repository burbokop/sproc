#include "cache.h"
#include <unistd.h>
#include <pwd.h>


std::string sproc::cache_directory() {
#ifdef __unix__
    struct passwd *pw = getpwuid(getuid());
    if(pw)
        return pw->pw_dir;
#endif
    return {};
}
