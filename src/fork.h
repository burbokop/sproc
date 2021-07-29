#ifndef FORK_H
#define FORK_H

#include "pipe_container.h"
#include <functional>

namespace sproc {
struct process_result {
    std::string out;
    std::string err;
    int code;
    bool valid = false;
    bool cmd_not_found = false;
};

std::ostream &operator<<(std::ostream& stream, const process_result& r);
process_result fork(const std::function<int()> &callback);

namespace non_blocking {

class process {
    int m_pid;
    pipe_container m_pipes;
    std::optional<process_result> m_result;
    process(int pid, pipe_container pipes);
public:
    process_result wait();
    std::optional<process_result> result();
    std::string read_out();
    std::string read_err();

    friend process fork(const std::function<int()> &callback);
};

process fork(const std::function<int()> &callback);

}

inline process_result system(const std::string &cmd) { return fork(std::bind(std::system, cmd.c_str())); }

}


#endif // FORK_H
