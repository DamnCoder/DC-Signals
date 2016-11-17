#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
cmake -G"Eclipse CDT4 - Unix Makefiles"