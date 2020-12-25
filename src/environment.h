#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "apt.h"
#include "fork.h"
#include "cache.h"

namespace sproc {


class temporary_path {
    std::string m_prev_path;
public:
    temporary_path(const std::string &new_path);
    ~temporary_path();
};

/**
 * @brief All functions in this class marked as "SPROC::APT" have analogs with same names as in sproc::apt namespace
 * but "environment" class contains specific path and all mutators do its job in this path
 */
class environment {
    std::string m_root_dir;
public:
    /**
     * @brief environment
     * @param root_dir
     */
    environment(const std::string &root_dir) { m_root_dir = root_dir; }
    environment(const char* root_dir) { m_root_dir = root_dir; }
    /**
     * @brief install_package (SPROC::APT)
     */
    bool install_package(const std::string &package, bool forced = apt::default_values::forced, bool debug = apt::default_values::debug) const;
    /**
     * @brief install_package_from_deb
     * @param deb - path to .deb file
     * @return
     */
    bool install_package_from_deb(const std::string &deb) const;
    /**
     * @brief system - same as sproc::system but includes bin path of this environment
     */
    process_result system(const std::string &cmd) const;
    /**
     * @brief system - run cmd and install if not exists
     */
    process_result system(const std::string &cmd, const std::string &package, bool forced = apt::default_values::forced, bool debug = apt::default_values::debug) const;
    /**
     * @brief auto_system - auto detects package that this cmd came from and try to install it.
     * @param cmd
     * @return
     */
    process_result auto_system(const std::string &cmd, bool forced = apt::default_values::forced, bool debug = apt::default_values::debug) const;

    std::string root_dir() const;
    /**
     * @brief clear_meta - cleares meta data files which contain info about downloaded and installed packages
     * @note do not delete installed data and packages
     * @return true if success
     */
    bool clear_meta() const;
};

static inline const environment home = environment(cache_directory() + "/.sproc");

}



#endif // ENVIRONMENT_H
