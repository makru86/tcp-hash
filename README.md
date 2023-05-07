# Project tcp-hash

![](https://github.com/makru86/tcp-hash/actions/workflows/build.yml/badge.svg)
![](https://img.shields.io/badge/Code%20Coverage-55%25-success?style=flat)

Hash server:

- 3 background threads to calculate hashes
- main thread accepting connections and passing sockets to background threads
- per socket server uses 2 x 1024 bytes for a buffer


Implemented on top of Boost Asio and xxHash (see [Dockerfile](Dockerfile)):
- see [asio-handler-tracking](assets/asio_handler_tracking.png))
- libxxhash-dev

- GitHub template [zethon/CCCBTemplate](https://github.com/zethon/CCCBTemplate)


TODO:
- very slow Integer to Hex string conversion (std::stream, setfill) - 50% of
  server time
- segfaults in xxHash on big input

## Building and running

Checkout the code, create Docker container, start Docker service listening TCP port 1234:

```
    git clone https://github.com/makru86/tcp-hash.git
    cd tcp-hash
    make docker-serve # or make serve
```

## tcp-hash Prototype: shell script with ncat and xxhsum

See [tools/tcp_hash_prototype.sh](tools/tcp_hash_prototype.sh).
See [tools/client.sh](tools/client.sh).

## netcat example

Keep connection open, exit by Ctrl-C:

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

```
    make build  # or make docker-build
    make test   # or make docker-test
```

### Formatting code, clang-tidy

```
    make formatted
    make tidy
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
