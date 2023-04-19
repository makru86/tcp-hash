# A Project tcp-hash

This simple project that shows how to use CMake and Conan to create a C++ project that uses the boost framework.

## Building

Checkout the code and create a folder in the repository folder called `build`.

```
git clone https://github.com/makru86/tcp-hash.git
cd tcp-hash
docker-compose build
docker-compose run builder /bin/bash -c "cmake -B build -S project -DCMAKE_BUILD_TYPE=Debug -DBUILD_UNIT_TESTS=ON"
docker-compose run builder /bin/bash -c "cmake --build build"
```