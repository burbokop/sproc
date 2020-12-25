#ifndef FORK_H
#define FORK_H

#include <string>
#include <functional>

namespace sproc {

constexpr int cmd_not_found_code = 32512;
std::function<int()> system_handler(const std::string& cmd);
    
struct process_result {
    std::string out;
    std::string err;
    int ext_code;
    int code;
    bool valid = false;
    bool cmd_not_found = false;
};

std::ostream &operator<<(std::ostream& stream, const process_result& r);
process_result fork(const std::function<int()> &callback);
        
inline process_result system(const std::string &cmd) { return fork(system_handler(cmd)); }

}


#endif // FORK_H
