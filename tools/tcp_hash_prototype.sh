#! /bin/sh

if [ $# -gt 1 ]; then
  echo "Usage: $1 [ <port> ]"
  echo "    Default port is 1234."
  echo "    Press Ctrl-C to exit."
  exit 1
fi

LISTEN_PORT=${1:-1234}
VERBOSITY="-vv"
MAX_CONNS="--max-conns 20000"
THIS_DIR=$(dirname "$0")

echo "Options:"
echo " LISTEN_PORT=$LISTEN_PORT"
echo " VERBOSITY=$VERBOSITY"
echo " MAX_CONNS=$MAX_CONNS"
echo " THIS_DIR=$THIS_DIR"

ls $THIS_DIR/prototype_handler.sh || exit 1
ncat -l $LISTEN_PORT $VERBOSITY \
    --keep-open --nodns $MAX_CONNS \
    --sh-exec "xargs --delimiter='\n' -n1 $THIS_DIR/prototype_handler.sh"
