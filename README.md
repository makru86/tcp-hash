# Project tcp-hash

![](https://github.com/makru86/tcp-hash/actions/workflows/ubuntu.yml/badge.svg)
![](https://img.shields.io/badge/Code%20Coverage-55%25-success?style=flat)

Ubuntu packages used in implementation (see [Dockerfile](Dockerfile)):

- libboost-all-dev
- libxxhash-dev

## Building and running

Checkout the code, create Docker container, build CMake project, run tests,
start Docker service listening TCP port 1234:

```
    git clone https://github.com/makru86/tcp-hash.git
    cd tcp-hash
    make up
```

## Running mock

Mock server implemented as a shell script using utilities `ncat` server and `xxhsum`
(see [mock/tcp_hash.sh](mock/tcp_hash.sh)). Start Docker service `mock`  listening TCP port 1234:

```
    docker-compose                 \
        -f docker-compose.yml      \
        -f docker-compose.mock.yml \
        up mock
```

## Using with netcat

netcat example, keep connection open, exit by Ctrl-C:

```
    netcat localhost 1234
    Hello, world!
>>> 0x2c6b514f4f9e3e3c
    dd
>>> 0x3fa6d80d3a0da668
```

## Using with netcat-openbsd

netcat-openbsd (nc) example, closing connection after receiving response:

```
    echo "Hello, world!" | nc -W 1 localhost 1234
>>> 0x2c6b514f4f9e3e3c
```

`nc` option `-W recvlimit`:
> Terminate after receiving *recvlimit* packets from the network.

## Development

### Formatting code, clang-tidy

```
    make format-code
    make tidy-code
```

### Benchmarking performance

```
    make loadtest
```

### Code coverage report

```
    env CODE_COVERAGE=ON make up
    make coverage-report
>>> Generating coverage report...
>>> lines: 54.9% (79 out of 144)
>>> branches: 36.6% (232 out of 634)
>>> build/coverage.xml
```