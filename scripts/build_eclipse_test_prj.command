#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
cd ..

build_folder=build_eclipse_test
if [ ! -d ./$build_folder ]; then
	mkdir $build_folder
fi

cd $build_folder

cmake ../test -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.2