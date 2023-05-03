# Project tcp-hash

![](https://github.com/makru86/tcp-hash/actions/workflows/ubuntu.yml/badge.svg)
![](https://img.shields.io/badge/Code%20Coverage-55%25-success?style=flat)

Ubuntu packages used in implementation (see [Dockerfile](Dockerfile)):

- libboost-all-dev mostly for Asio (see [asio-handler-tracking](assets/asio_handler_tracking.png))
- libxxhash-dev

- this project's template [zethon/CCCBTemplate](https://github.com/zethon/CCCBTemplate)


## Building and running

Checkout the code, create Docker container, build CMake project, run tests,
start Docker service listening TCP port 1234:

```
    git clone https://github.com/makru86/tcp-hash.git
    cd tcp-hash
    make up
```

## tcp-hash Prototype: shell script with ncat and xxhsum

See [mock/tcp_hash.sh](mock/tcp_hash.sh).
Start Docker service `prototype`  listening TCP port 1234:
Press Ctrl-C to stop.

```
    make prototype-up &
    ./tools/client.sh
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