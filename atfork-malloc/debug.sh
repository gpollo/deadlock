#!/bin/bash
#
# Usage: ./debug.sh
#
# If glibc sources has been mounted in /src, gdb will use them.
#

START_DIR=$PWD
cd $(dirname $0)
function finish() {
	cd "$START_DIR"
}
trap finish EXIT

IMAGE=$(basename "$PWD")
docker exec -it "$IMAGE" /bin/bash -c 'reset; cgdb -p $(pidof $BINARY) -ex "dir /src/"'

