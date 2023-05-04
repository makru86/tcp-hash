#! /bin/sh

if [ $# -gt 1 ]; then
  echo "Usage: $1 [ <port> ]"
  echo "    Default port is 1234."
  echo "    Press Ctrl-C to exit."
  exit 1
fi

RUN_TIME="1"
DATA_SIZE="512"
CONNECT_PORT=${1:-1234}
THIS_DIR=$(dirname "$0")

echo "Options:"
echo " RUN_TIME=${RUN_TIME}s"
echo " CONNECT_PORT=$CONNECT_PORT"


# function to run ncat
function client_session() {
  ( \
  head -c${DATA_SIZE} /dev/random; \
  sleep $RUN_TIME \
) | \
  ncat "0.0.0.0" $CONNECT_PORT $VERBOSITY --nodns | \
  wc
}


# spawn multiple clients repeatedly calling ncat,
# then sleeping 500 ms, until RUN_TIME is over.

for i in $(seq 1 2); do
  { client_session; } &
done;

wait
