#! /bin/sh

# tcp-hash.sh - a TCP port 1234 listener that processes
#               incoming data with handler.sh.
#
# Usage:  ./prototype/tcp-hash.sh 1234
# Press Ctrl-C to exit.

THIS_DIR=$(dirname "$0")
LISTEN_PORT=${1:-1234}

echo "Listening on port $LISTEN_PORT..."
ncat -l $LISTEN_PORT -v \
    --keep-open --max-conns 10000 \
    --sh-exec "xargs -n1 $THIS_DIR/handler.sh"
