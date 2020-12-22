#ifndef APT_H
#define APT_H

#include <string>
#include <vector>
#include <functional>
#include <list>


namespace sproc {
/**
 * ALL FUNCTIONS IN THIS NAMESPACE MARKED AS "MUTATOR" DO CHANGES IN WORKING DIR
 */
namespace apt {

static const inline std::string download_meta_file = "download.list";
static const inline std::string install_meta_file  = "install.list";
static const inline std::string download_directory = "./deb";
static const inline std::string up_directory       = "..";

namespace cmds {
static const inline std::string download            = "apt-get download ";
static const inline std::string extract             = "dpkg-deb --extract ";
static const inline std::string search_in_root      = "dpkg -s ";
static const inline std::string filter_sort         = " | grep \"^\\w\" | sort -u";
static const inline std::string list_dependencies   = "apt-cache depends ";
const static std::vector<std::string> cmd_not_found = {
    "/usr/lib/command-not-found",
    "/usr/share/command-not-found/command-not-found"
};
}

namespace matchings {
static inline std::string search_in_root        = "Status: install ok installed";
static inline std::string cmd_not_found_apt     = "apt";
static inline std::string cmd_not_found_install = "install";
}

namespace default_values {
static const inline bool forced = true;
static const inline bool debug = true;
}

namespace debug {
static inline std::string downloading_package            = "Downloading package | ";
static inline std::string downloading_package_plug       = "                    | ";
static inline std::string package_found_in_foot          = "Found in root.";
static inline std::string package_already_downloaded     = "Already downloaded.";
static inline std::string listing_dependencies_failed    = "Getting dependencies failed.";
static inline std::string package_downloading_compleated = "Success.";
static inline std::string package_downloading_failed     = "Downloading failed.";

static inline std::string extracting_package             = "Extracting pack: ";
static inline std::string extracting_package_success     = "OK";
static inline std::string extracting_package_failed      = "FAILED";
}

namespace meta {
bool contains(const std::string& package, const std::string &path);
/**
 * @brief append (MUTATOR)
 * @param package
 * @param path
 * @return
 */
bool append(const std::string& package, const std::string &path);
/**
 * @brief clear (MUTATOR)
 * @param path
 * @return
 */
bool clear(const std::string &path);
}

namespace info {
bool list_dependencies(const std::string& package, std::list<std::string> *list, const std::vector<std::string> &args = {
        "--recurse",
        "--no-recommends",
        "--no-suggests",
        "--no-conflicts",
        "--no-breaks",
        "--no-replaces",
        "--no-enhances"
});
bool is_installed_in_root(const std::string& package);
std::string pack_name_by_cmd(const std::string& cmd);
}

/**
 * @brief download_deb (MUTATOR) - only download one .deb
 * @param package
 * @return
 */
bool download_deb(const std::string& package);
/**
 * @brief download_package (MUTATOR) - download package with its dependencies
 * @param package
 * @param forced - TRUE: funk will download package anyway, FALSE: funk will not download package if it is installed in root dir by trivial apt session
 * @param with_dependencies
 * @param debug - if true print to console info about downloading
 * @return true if pack is downloaded succesfully or already installed.
 */
bool download_package(const std::string& package, bool forced = default_values::forced, bool with_dependencies = true, bool debug = default_values::debug);
/**
 * @brief local_extract (MUTATOR) - extract all downloaded packages
 * @param debug - if true print to console info about extraction
 */
void local_extract(bool debug = default_values::debug);
/**
 * @brief install_package (MUTATOR) - will call "download_package" funk, than "local_extract"
 * @param package
 * @param forced - the same as "forced" argument in "download_package" funk
 * @param debug - if true print to console info about installation
 * @return
 */
bool install_package(const std::string &package, bool forced = default_values::forced, bool debug = default_values::debug);

}
}

#endif // APT_H
