#ifndef FORK_H
#define FORK_H

#include <string>
#include <functional>

namespace sproc {
struct process_result {
    std::string out;
    std::string err;
    int code;
    bool valid = false;
};

std::ostream &operator<<(std::ostream& stream, const process_result& r);
process_result fork(const std::function<int()> &callback);

}


#endif // FORK_H
