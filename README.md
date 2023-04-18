# A C++, CMake, Conan, Boost Template

This simple project that shows how to use CMake and Conan to create a C++ project that uses the boost framework.

## Building

Checkout the code and create a folder in the repository folder called `build`.

```
git clone https://github.com/zethon/CCCBTemplate.git
cd CCCBTemplate
docker-compose build
docker-compose up
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```
