# sproc
sproc (Sexy Process) is the library for running child processes on linux.


## adding sproc to your c++ project

1. `git submodule add https://github.com/burbokop/sproc.git`</br>
2. in `CMakeLists.txt`: 
```
include(${CMAKE_CURRENT_LIST_DIR}/sproc/sproc.cmake)
target_link_libraries(your_executable_or_lib
    sproc
)

```


## example

This example shows using `sproc::environment::auto_system` function which is same as `std::system` but if cmd not found on mashine it will be installed authomatically.</br>

Function `auto_system_example` creates `result.pdf` file in working directory.

```
#include <sproc/src/environment.h>
#include <fstream>

void auto_system_example(const std::list<std::pair<int, int>> &rl, const std::string &path) {
    const auto compile_line = "dot -Tpdf " + path + ".gv -o " + path + ".pdf";
    std::ofstream stream(path + ".gv", std::ios::out);
    stream << "digraph G {\n";
    for(const auto& relation : rl) {
        stream
                << "\tv"
                << relation.second
                << " -> v"
                << relation.first
                << ";\n";
    }
    stream << "}\n";
    stream.close();
    std::cout << sproc::home.auto_system(compile_line);
}


int main() {
    auto_system_example({
                            { 0, 1 },
                            { 0, 2 },
                            { 0, 3 },
                            { 2, 3 }
                        }, "result");
    return 0;
}
```
