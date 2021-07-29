#ifndef FORK_H
#define FORK_H

#include "pipe_container.h"
#include <functional>

namespace sproc {

constexpr int cmd_not_found_code = 32512;
std::function<int()> system_handler(const std::string& cmd);

enum streams {
    write_stream = 0,
    read_stream = 1,
    read_err_stream = 2,
    read_meta_stream = 3,
};
    
struct process_result {
    std::string out;
    std::string err;
    int ext_code;
    int code;
    bool valid = false;
    bool cmd_not_found = false;
};

std::ostream &operator<<(std::ostream& stream, const process_result& r);

namespace non_blocking {

class process {
    int m_pid;
    pipe_container m_pipes;
    std::optional<process_result> m_result;
    process(int pid, pipe_container pipes);
public:
    process_result wait();
    std::optional<process_result> result();
    std::string read_all_out();
    std::string read_all_err();

    template<typename T>
    T read_out() { return m_pipes.read<T>(read_stream); }
    template<typename T>
    T read_err() { return m_pipes.read<T>(read_err_stream); }

    friend process fork(const std::function<int()> &callback);
};

process fork(const std::function<int()> &callback);

}



process_result fork(const std::function<int()> &callback);
        
inline process_result system(const std::string &cmd) { return fork(system_handler(cmd)); }

}


#endif // FORK_H
