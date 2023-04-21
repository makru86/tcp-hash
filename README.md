# Project tcp-hash


![Github Actions Badge.](https://github.com/makru86/tcp-hash/actions/workflows/ubuntu.yml/badge.svg)

## Building

Checkout the code, create Docker container, and build CMake project.

```
    git clone https://github.com/makru86/tcp-hash.git
    cd tcp-hash
    docker-compose up
```

Start Docker service `mock`  listening TCP port 1234:

```
    docker-compose \
    -f docker-compose.yml \
    -f docker-compose.mock.yml \
    up mock
```

## Using

netcat example, keep connection open, exit by Ctrl-C:

```
    netcat localhost 1234
    Hello, world!
>>> 0x2c6b514f4f9e3e3c
    dd
>>> 0x3fa6d80d3a0da668
```

netcat-openbsd (nc) example, closing connection after receiving response.
Option `-W recvlimit`: Terminate after receiving recvlimit packets from the network.

```
    echo "Hello, world!" | nc -W1 localhost 1234
>>> 0x2c6b514f4f9e3e3c
```
