#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
cmake -H. -Bbuild
cmake --build build -- -j4
exit 0