#include "pipe_container.h"

#include <unistd.h>

namespace c_interface {
decltype (close) *__close = close;
decltype (pipe) *__pipe = pipe;
decltype (read) *__read = read;
decltype (dup2) *__dup2 = dup2;
decltype (write) *__write = write;
};

std::string sproc::read_all(file_des fd) {
    std::string result;
    while (true) {
        char c[256];
        auto size = c_interface::__read(fd, &c, sizeof (c));
        if(size > 0) {
            result.append(c, size);
        }
        if(size < static_cast<decltype (size)>(sizeof (c))) {
            break;
        }
    }
    return result;
}

sproc::pipe_container::pipe_container(size_t count) {
    pipes = pipe_vector(count);
    for(size_t i = 0; i < count; ++i)
        c_interface::__pipe(pipes[i].data());
}

void sproc::pipe_container::rclose(size_t index) {
    c_interface::__close(rfile(index));
}

void sproc::pipe_container::wclose(size_t index) {
    c_interface::__close(wfile(index));
}

void sproc::pipe_container::rdup2(size_t index, file_des fd) {
    dup2(rfile(index), fd);
}

void sproc::pipe_container::wdup2(size_t index, file_des fd) {
    dup2(wfile(index), fd);
}

void sproc::pipe_container::close() {
    if(!closed) {
        for(size_t i = 0; i < pipes.size(); ++i) {
            rclose(i);
            wclose(i);
        }
        closed = true;
    }
}

ssize_t sproc::write_buffer(sproc::file_des fd, void *data, size_t size) {
    return c_interface::__write(fd, data, size);
}

ssize_t sproc::read_buffer(sproc::file_des fd, void *data, size_t size) {
    return c_interface::__read(fd, data, size);
}
