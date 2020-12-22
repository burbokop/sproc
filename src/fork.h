#ifndef FORK_H
#define FORK_H

#include <string>
#include <functional>

namespace sproc {
    
std::function<int()> wes_system_handler(const std::string& cmd);
    
struct process_result {
    std::string out;
    std::string err;
    int code;
    bool valid = false;
};

std::ostream &operator<<(std::ostream& stream, const process_result& r);
process_result fork(const std::function<int()> &callback);

inline process_result system(const std::string &cmd) { return fork(std::bind(std::system, cmd.c_str())); }

}


#endif // FORK_H
