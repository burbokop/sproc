#include "fork.h"
#include "pipe_container.h"

#include <ostream>
#include <unistd.h>
#include <sys/wait.h>

#include "../../src/color.h"
#include <iostream>

namespace c_interface {
decltype (fork) *__fork = fork;
};

std::function<int()> sproc::system_handler(const std::string& cmd) {
    return [cmd] {
        return std::system(cmd.c_str());
    };
}

std::ostream &sproc::operator<<(std::ostream &stream, const sproc::process_result &r) {
    return stream << "{v: " << r.valid
                  << ", c: " << r.code
                  << ", e: " << r.ext_code
                  << ", out: " << r.out
                  << ", err: " << r.err << "}";
}

sproc::process_result sproc::fork(const std::function<int()> &callback) {
    process_result result;
    constexpr static size_t PIPES_COUNT = 4;
    enum {
        Write = 0,
        Read = 1,
        ReadErr = 2,
        ReadMeta = 3,
    };

    pipe_container pipes(PIPES_COUNT);
    const auto pid = c_interface::__fork();
    if(pid) {
        pipes.rclose(Write);
        pipes.wclose(Read);
        pipes.wclose(ReadErr);
        pipes.wclose(ReadMeta);

        ///TO DO write(pipes.pipe(PARENT_WRITE_PIPE, WRITE_FD), "gogadoda", 8);

        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            result.valid = true;
            result.code = WEXITSTATUS(status);
            result.out = pipes.read_all(Read);
            result.err = pipes.read_all(ReadErr);
            result.ext_code = pipes.read<uint64_t>(ReadMeta);
            result.cmd_not_found = (result.ext_code == cmd_not_found_code);
        } else {
            result.valid = false;
        }
    } else {
        pipes.rdup2(Write, STDIN_FILENO);
        pipes.wdup2(Read, STDOUT_FILENO);
        pipes.wdup2(ReadErr, STDERR_FILENO);
        const auto code = callback();
        pipes.write(ReadMeta, static_cast<int>(code));
        pipes.close();
        std::exit(WEXITSTATUS(code));
    }
    return result;
}
