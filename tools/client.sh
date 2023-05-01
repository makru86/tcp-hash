#! /bin/sh

if [ $# -gt 1 ]; then
  echo "Usage: $1 [ <port> ]"
  echo "    Default port is 1234."
  echo "    Press Ctrl-C to exit."
  exit 1
fi

RUN_TIME="10"
DATA_SIZE="4M"
CONNECT_PORT=${1:-1234}
THIS_DIR=$(dirname "$0")

echo "Options:"
echo " RUN_TIME=${RUN_TIME}s"
echo " CONNECT_PORT=$CONNECT_PORT"

( \
  head -c${DATA_SIZE} /dev/random; \
  sleep $RUN_TIME \
) | \
  ncat "0.0.0.0" $CONNECT_PORT $VERBOSITY --nodns | \
  od -c -A doxn