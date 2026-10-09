#!/bin/sh
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
exec "$HERE/runtime/tools/docker_build.sh" "$@"
