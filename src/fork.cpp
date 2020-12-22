#include "fork.h"
#include "pipe_container.h"

#include <ostream>
#include <unistd.h>
#include <sys/wait.h>

namespace c_interface {
decltype (fork) *__fork = fork;
};


std::ostream &sproc::operator<<(std::ostream &stream, const sproc::process_result &r) {
    if(r.err.size() > 0) {
        stream << r.err;
    } else if(r.out.size() > 0) {
        stream << r.out;
    } else {
        stream << "code: " << r.code << "\n";
    }
    return stream;
}

sproc::process_result sproc::fork(const std::function<int()> &callback) {
    process_result result;
    constexpr static size_t PIPES_COUNT = 3;
    enum {
        Write = 0,
        Read = 1,
        ReadErr = 2,
    };

    pipe_container pipes(PIPES_COUNT);
    const auto pid = c_interface::__fork();
    if(pid) {
        pipes.rclose(Write);
        pipes.wclose(Read);
        pipes.wclose(ReadErr);

        ///TO DO write(pipes.pipe(PARENT_WRITE_PIPE, WRITE_FD), "gogadoda", 8);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            result.valid = true;
            result.code = WEXITSTATUS(status);
            result.out = pipes.read_all(Read);
            result.err = pipes.read_all(ReadErr);
        }
    } else {
        pipes.rdup2(Write, STDIN_FILENO);
        pipes.wdup2(Read, STDOUT_FILENO);
        pipes.wdup2(ReadErr, STDERR_FILENO);
        const auto code = callback();
        pipes.close();
        exit(code);
    }
    return result;
}
