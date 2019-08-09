#!/bin/bash
#
# Usage: ./run.sh <dockerfile> [docker run options]
#
# Example: ./run.sh Dockerfile.fedora.23 -v /usr/src/glibc:/src:ro
#

START_DIR=$PWD
cd $(dirname $0)
function finish() {
	cd "$START_DIR"
}
trap finish EXIT

if [[ $# -lt 1 ]]; then
	echo "$0: you must specify which Dockerfile to use"
	exit 1
fi

DOCKERFILE=$(basename $1)
IMAGE=$(basename "$PWD")

export SOURCE=$PWD
export BINARY=atfork-malloc
if ! docker build . -t $IMAGE -f "$DOCKERFILE" --build-arg SOURCE --build-arg BINARY; then
	exit 1
fi

if ! docker run --rm -it --name $IMAGE --cap-add=SYS_PTRACE ${@:2} $IMAGE; then
	exit 1
fi
