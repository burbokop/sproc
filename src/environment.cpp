#include "environment.h"

#include <filesystem>

std::string sproc::environment::root_dir() const {
    return m_root_dir;
}

bool sproc::environment::clear_meta() const {
    temporary_path tmp_p(m_root_dir);
    return sproc::apt::meta::clear(sproc::apt::download_meta_file) && sproc::apt::meta::clear(sproc::apt::install_meta_file);
}

bool sproc::environment::install_package(const std::string &package, bool forced, bool debug) const {
    temporary_path tmp_p(m_root_dir);
    return sproc::apt::install_package(package, forced, debug);
}

bool sproc::environment::install_package_from_deb(const std::string &deb) const {
    temporary_path tmp_p(m_root_dir);
    if(std::filesystem::copy_file(deb, apt::download_directory)) {
        sproc::apt::local_extract();
        return true;
    }
    return false;
}

sproc::process_result sproc::environment::system(const std::string &cmd, const std::string &package, bool forced, bool debug) const {
    if(install_package(package, forced, debug)) {
        return system(cmd);
    } else {
        return {};
    }
}

sproc::process_result sproc::environment::auto_system(const std::string &cmd, bool forced, bool debug) const {
    const auto result = system(cmd);
    if(result.cmd_not_found) {
        const auto pos = cmd.find(' ');
        std::string cmd_name;
        if(pos >= 0 && pos < cmd.size()) {
            cmd_name = cmd.substr(0, pos);
        } else {
            cmd_name = cmd;
        }

        const auto package = sproc::apt::info::pack_name_by_cmd(cmd_name);
        if(package.size() > 0)
            return system(cmd, package, forced, debug);
    }
    return result;
}

sproc::process_result sproc::environment::system(const std::string &cmd) const {
    const auto path_setup_cmd = "export PATH=" + m_root_dir + "/usr/bin:" + m_root_dir + "/bin:$PATH";
    return sproc::system(path_setup_cmd + " && " + cmd);
}

sproc::temporary_path::temporary_path(const std::string &new_path) {
    std::filesystem::create_directory(new_path);
    m_prev_path = std::filesystem::current_path().generic_string();
    std::filesystem::current_path(new_path);
}

sproc::temporary_path::~temporary_path() { std::filesystem::current_path(m_prev_path); }

