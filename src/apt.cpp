#include "apt.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include "fork.h"

bool sproc::apt::meta::contains(const std::string &package, const std::string &path) {
    std::ifstream imeta(path, std::ios::in);
    if(imeta.is_open()) {
        while(!imeta.eof()) {
            std::string line;
            std::getline(imeta, line, '\n');
            if(line == package) {
                imeta.close();
                return true;
            }
        }
    }
    imeta.close();
    return false;
}

bool sproc::apt::meta::append(const std::string &package, const std::string &path) {
    if(meta::contains(package, path))
        return false;

    std::ofstream (path, std::ios::app) << package << std::endl;
    return true;
}

bool sproc::apt::meta::clear(const std::string &path) {
    return std::filesystem::remove(path);
}


bool sproc::apt::install_package(const std::string &package, bool forced, bool debug) {
    if(download_package(package, forced, true, debug)) {
        local_extract(debug);
        return true;
    }
    return false;
}


bool sproc::apt::download_package(const std::string &package, bool forced, bool with_dependencies, bool debug) {
    if(debug) std::cout
            << debug::downloading_package
            << package
            << std::endl;
    if(info::is_installed_in_root(package) && !forced) {
        if(debug) std::cout
                << debug::downloading_package_plug
                << debug::package_found_in_foot
                << std::endl;
        return true;
    }
    if(meta::contains(package, download_meta_file)) {
        if(debug) std::cout
                << debug::downloading_package_plug
                << debug::package_already_downloaded
                << std::endl;
        return true;
    }

    if(with_dependencies) {
        std::list<std::string> packs_to_install;
        if(info::list_dependencies(package, &packs_to_install)) {
            for(const auto &p : packs_to_install) {
                if(!download_package(p, forced, false)) {
                    return false;
                }
            }
        } else {
            if(debug) std::cerr
                    << debug::downloading_package_plug
                    << debug::listing_dependencies_failed
                    << std::endl;
            return false;
        }
    }

    if(download_deb(package)) {
        if(debug) std::cout
                << debug::downloading_package_plug
                << debug::package_downloading_compleated
                << std::endl;
        return true;
    } else {
        if(debug) std::cerr
                << debug::downloading_package_plug
                << debug::package_downloading_failed
                << std::endl;
        return false;
    }
}




void sproc::apt::local_extract(bool debug) {
    if(std::filesystem::exists(download_directory) && std::filesystem::is_directory(download_directory)) {
        std::filesystem::recursive_directory_iterator it(download_directory), end;
        while (it != end) {
            if(it->exists()) {
                const auto deb_path = it->path().string();
                if(!meta::contains(deb_path, install_meta_file)) {
                    if(debug) std::cout
                            << debug::extracting_package
                            << deb_path
                            << ' ';
                    const auto res = sproc::system(cmds::extract + deb_path + " ./");
                    if(res.err.size() == 0) {
                        meta::append(deb_path, install_meta_file);
                        if(debug) std::cout
                                << debug::extracting_package_success
                                << std::endl;
                    } else {
                        if(debug) std::cout
                                << debug::extracting_package_failed
                                << std::endl
                                << res.err
                                << std::endl;
                    }
                }
            }
            ++it;
        }
    }
}

bool sproc::apt::info::is_installed_in_root(const std::string &package) {
    const auto out = sproc::system(cmds::search_in_root + package).out;
    const auto i = out.find(matchings::search_in_root);
    return i >= 0 && i < out.size();
}

bool sproc::apt::info::list_dependencies(const std::string &package, std::list<std::string> *list, const std::vector<std::string> &args) {
    std::string joined_args;
    for(const auto& a : args)
        joined_args += a + " ";

    const auto search_res = sproc::system(cmds::list_dependencies + joined_args + package + cmds::filter_sort);
    if(search_res.err.size() == 0) {
        std::istringstream ss(search_res.out);
        while (!ss.eof()) {
            std::string line;
            std::getline(ss, line, '\n');
            if(line.size() > 0) {
                if(line != package) {
                    list->push_back(line);
                }
            }
        }
        return true;
    }
    return false;
}

bool sproc::apt::download_deb(const std::string &package) {
    std::filesystem::create_directories(download_directory);
    std::filesystem::current_path(download_directory);
    const auto download_res = sproc::system(cmds::download + package);
    std::filesystem::current_path(up_directory);
    if(download_res.err.size() == 0) {
        meta::append(package, download_meta_file);
        return true;
    }
    return false;
}

std::string sproc::apt::info::pack_name_by_cmd(const std::string &cmd) {
    for(const auto &c : cmds::cmd_not_found) {
        if(std::filesystem::exists(c)) {
            const auto res = sproc::system(c + " " + cmd);
            if(res.err.size() > 0) {
                std::istringstream ss(res.err);
                while (!ss.eof()) {
                    std::string line;
                    std::getline(ss, line, '\n');
                    const auto pos = line.find(matchings::cmd_not_found_apt);
                    if(pos >= 0 && pos < line.size()) {
                        const auto begin
                                = line.find(matchings::cmd_not_found_install)
                                + matchings::cmd_not_found_install.size()
                                + 1 /* one space char */;
                        if(begin >= 0 && begin < line.size()) {
                            const auto end = line.find(' ', begin);
                            if(end >= 0 && end <= line.size()) {
                                return line.substr(begin, end - begin);
                            } else {
                                return line.substr(begin, line.size() - begin);
                            }
                        }
                    }
                }
            }
        }
    }
    return {};
}
