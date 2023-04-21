#! /bin/sh

# tcp-hash.sh - a TCP port 1234 listener that processes
#               incoming data with handler.sh.
#
# Usage:  ./tcp-hash.sh
# Press Ctrl-C to exit.
#
# Examples:
# With netcat, press Ctrl-C to exit:
#          echo "Hello, world!" | netcat localhost 1234
#          0x2c6b514f4f9e3e3c
#
# With netcat, keep the connection open:
#          netcat localhost 1234
#          Hello, world!
#          0x2c6b514f4f9e3e3c
#
# With netcat-openbsd (nc), single request:
#          echo "Hello, world!" | nc -W1 localhost 1234
#          0x2c6b514f4f9e3e3c

THIS_DIR=$(dirname "$0")

echo "Listening on port 1234..."
ncat -l 1234 -v \
    --keep-open --max-conns 10000 \
    --sh-exec "xargs -n1 $THIS_DIR/handler.sh"
