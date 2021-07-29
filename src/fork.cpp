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

enum {
    Write = 0,
    Read = 1,
    ReadErr = 2,
};

sproc::process_result sproc::fork(const std::function<int()> &callback) {
    process_result result;
    constexpr static size_t PIPES_COUNT = 3;


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
    pipes.close();
    return result;
}

sproc::non_blocking::process sproc::non_blocking::fork(const std::function<int ()> &callback) {
    constexpr static size_t PIPES_COUNT = 3;

    pipe_container pipes(PIPES_COUNT);
    const auto pid = c_interface::__fork();
    if(pid) {
        pipes.rclose(Write);
        pipes.wclose(Read);
        pipes.wclose(ReadErr);

        return process(pid, pipes);
    } else {
        pipes.rdup2(Write, STDIN_FILENO);
        pipes.wdup2(Read, STDOUT_FILENO);
        pipes.wdup2(ReadErr, STDERR_FILENO);
        const auto code = callback();
        pipes.close();
        exit(code);
    }
    return process(0, pipe_container(0));
}

sproc::non_blocking::process::process(int pid, pipe_container pipes)
    : m_pid(pid), m_pipes(pipes) {}

sproc::process_result sproc::non_blocking::process::wait() {
    process_result result;
    int status;
    waitpid(m_pid, &status, 0);
    if (WIFEXITED(status)) {
        result.valid = true;
        result.code = WEXITSTATUS(status);
        result.out = m_pipes.read_all(Read);
        result.err = m_pipes.read_all(ReadErr);
    }
    m_pipes.close();
    return result;
}

std::optional<sproc::process_result> sproc::non_blocking::process::result() {
    if(m_result.has_value()) {
        return m_result;
    } else {
        int status;
        waitpid(m_pid, &status, WNOHANG);
        if (WIFEXITED(status)) {
            m_result = process_result {
                .out = m_pipes.read_all(Read),
                        .err = m_pipes.read_all(ReadErr),
                        .code = WEXITSTATUS(status),
                        .valid = true
            };
            m_pipes.close();
            return m_result;
        }
        return std::nullopt;
    }
}

std::string sproc::non_blocking::process::read_out() {
    return m_pipes.read_all(Read);
}

std::string sproc::non_blocking::process::read_err() {
    return m_pipes.read_all(ReadErr);
}
