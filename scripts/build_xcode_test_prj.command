#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
cd ..

build_folder=build_xcode_test
if [ ! -d ./$build_folder ]; then
	mkdir $build_folder
fi

cd $build_folder

cmake ../test -G "Xcode"

exit 0