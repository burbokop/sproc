#include "fork.h"

#include <ostream>
#include <unistd.h>
#include <sys/wait.h>

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
    return stream << "{ v: " << r.valid
                  << ", c: " << r.code
                  << ", e: " << r.ext_code
                  << ", out: " << r.out
                  << ", err: " << r.err << " }";
}

sproc::process_result sproc::fork(const std::function<int()> &callback) {
    process_result result;
    constexpr static size_t PIPES_COUNT = 4;

    pipe_container pipes(PIPES_COUNT);
    const auto pid = c_interface::__fork();
    if(pid) {
        pipes.rclose(write_stream);
        pipes.wclose(read_stream);
        pipes.wclose(read_err_stream);
        pipes.wclose(read_meta_stream);

        ///TO DO write(pipes.pipe(PARENT_WRITE_PIPE, WRITE_FD), "gogadoda", 8);

        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            result.valid = true;
            result.code = WEXITSTATUS(status);
            result.out = pipes.read_all(read_stream);
            result.err = pipes.read_all(read_err_stream);
            result.ext_code = pipes.read<uint64_t>(read_meta_stream);
            result.cmd_not_found = (result.ext_code == cmd_not_found_code);
        } else {
            result.valid = false;
        }
    } else {
        pipes.rdup2(write_stream, STDIN_FILENO);
        pipes.wdup2(read_stream, STDOUT_FILENO);
        pipes.wdup2(read_err_stream, STDERR_FILENO);
        const auto code = callback();
        pipes.write(read_meta_stream, static_cast<int>(code));
        pipes.close();
        std::exit(WEXITSTATUS(code));
    }
    return result;
}

sproc::non_blocking::process::process(int pid, pipe_container pipes)
    : m_pid(pid), m_pipes(pipes) {}

sproc::process_result sproc::non_blocking::process::wait() {
    int status;
    waitpid(m_pid, &status, 0);
    if (WIFEXITED(status)) {
        process_result result;
        result.valid = true;
        result.code = WEXITSTATUS(status);
        result.out = m_pipes.read_all(read_stream);
        result.err = m_pipes.read_all(read_err_stream);
        result.ext_code = m_pipes.read<uint64_t>(read_meta_stream);
        result.cmd_not_found = (result.ext_code == cmd_not_found_code);
        m_pipes.close();
        return result;
    }
    return process_result();
}

std::optional<sproc::process_result> sproc::non_blocking::process::result() {
    if(m_result.has_value()) {
        return m_result;
    } else {
        int status;
        waitpid(m_pid, &status, WNOHANG);
        if (WIFEXITED(status)) {
            const auto meta = static_cast<int>(m_pipes.read<uint64_t>(read_meta_stream));
            m_result = process_result {
                    .out = m_pipes.read_all(read_stream),
                    .err = m_pipes.read_all(read_err_stream),
                    .ext_code = meta,
                    .code = WEXITSTATUS(status),
                    .valid = true,
                    .cmd_not_found = (meta == cmd_not_found_code)
            };
            m_pipes.close();
            return m_result;
        }
        return std::nullopt;
    }
}

std::string sproc::non_blocking::process::read_all_out() {
    return m_pipes.read_all(read_stream);
}

std::string sproc::non_blocking::process::read_all_err() {
    return m_pipes.read_all(read_err_stream);
}

sproc::non_blocking::process sproc::non_blocking::fork(const std::function<int ()> &callback) {
    constexpr static size_t PIPES_COUNT = 4;

    pipe_container pipes(PIPES_COUNT);
    const auto pid = c_interface::__fork();
    if(pid) {
        pipes.rclose(write_stream);
        pipes.wclose(read_stream);
        pipes.wclose(read_err_stream);
        pipes.wclose(read_meta_stream);

        return process(pid, pipes);
    } else {
        pipes.rdup2(write_stream, STDIN_FILENO);
        pipes.wdup2(read_stream, STDOUT_FILENO);
        pipes.wdup2(read_err_stream, STDERR_FILENO);
        const auto code = callback();
        pipes.write(read_meta_stream, static_cast<int>(code));
        pipes.close();
        std::exit(WEXITSTATUS(code));
    }
    return process(0, pipe_container(0));
}
