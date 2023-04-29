#! /bin/sh

# prototype_handler.sh
#   pass first argument to
#   xxHash and return the hash value in HEX format.
#
# Example:
#   $0 "Hello, world!"
# > 0x2c6b514f4f9e3e3c

echo $1 | xxhsum | awk '{print "0x"$1}'
