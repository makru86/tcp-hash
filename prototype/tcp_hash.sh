#! /bin/sh

# tcp-hash.sh - a TCP port 1234 listener that processes
#               incoming data with handler.sh.
#
# Usage:  ./prototype/tcp-hash.sh
# Press Ctrl-C to exit.

THIS_DIR=$(dirname "$0")

echo "Listening on port 1234..."
ncat -l 1234 -v \
    --keep-open --max-conns 10000 \
    --sh-exec "xargs -n1 $THIS_DIR/handler.sh"
