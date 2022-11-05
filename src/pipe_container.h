#ifndef PIPE_CONTAINER_H
#define PIPE_CONTAINER_H

#include <vector>
#include <string>
#include <array>

namespace sproc {

typedef int file_des;
typedef std::array<file_des, 2> pipe;
typedef std::vector<pipe> pipe_vector;

std::string read_all(file_des fd);
std::size_t write_buffer(file_des fd, void *data, size_t size);
std::size_t read_buffer(file_des fd, void *data, size_t size);

/**
 * @brief The pipe_container class provides simple interface for managing c pipes
 * @note The pipe_container class has form of vector
 * @note The pipe_container class authomatically close all pipes in destructor
 */
class pipe_container {
    pipe_vector pipes;
    bool closed = false;
public:
    pipe_container(size_t count);
    inline file_des &rfile(size_t index) { return pipes[index][0]; }
    inline file_des &wfile(size_t index) { return pipes[index][1]; }
    inline file_des rfile(size_t index) const { return pipes[index][0]; }
    inline file_des wfile(size_t index) const { return pipes[index][1]; }
    inline size_t size() const { return pipes.size(); }
    inline std::string read_all(size_t index) { return sproc::read_all(rfile(index)); }
    inline std::string read_all_reversed(size_t index) { return sproc::read_all(wfile(index)); }

    template<typename T>
    inline auto write(size_t index, T&& value) { return write_buffer(wfile(index), &value, sizeof (T)); }
    template<typename T>
    inline auto write_reversed(size_t index, T&& value) { return write_buffer(rfile(index), &value, sizeof (T)); }

    template<typename T>
    inline T read(size_t index) {
        T result;
        read_buffer(rfile(index), &result, sizeof (T));
        return result;
    }
    template<typename T>
    inline T read_reversed(size_t index) {
        T result;
        read_buffer(wfile(index), &result, sizeof (T));
        return result;
    }

    void rclose(size_t index);
    void wclose(size_t index);
    void rdup2(size_t index, file_des fd);
    void wdup2(size_t index, file_des fd);
    void close();
};


}

#endif // PIPE_CONTAINER_H
