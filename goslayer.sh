#!/bin/bash

usage()
{
    echo "usage: goslayer [[[-i | --install goslayer ] [-p | --port port to run on]] | [ -u | --uri remote server uri for remote games ] [-h | --help]]"
}

uri="ws://localhost:8080"
port=9002
install=0
mode=""
debug=0

while [ "$1" != "" ]; do
    case $1 in
        -i | --install )        shift
                                install=$1
                                ;;
        -p | --port )           shift
                                port=$1
                                ;;
        -m | --mode )            shift
                                mode=$1
                                ;;
        -u | --uri )            shift
                                uri=$1
                                ;;
        -d | --debug )          shift
                                debug=$1
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done


if [ "$install" = "1" ]; then
    echo "cloning..."
    git clone git@github.com:Kareem-Emad/RoboGo.git
    cd RoboGo
    git checkout integration-communication

    echo "Building GoSlayer ..."
    rm -rf build
    mkdir build
    cd build
    export CC=clang
    export CXX=clang++
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

    echo "Installing GoSlayer..."
    mkdir ~/goslayer
    cp -a ./bin ~/goslayer
    cd ../src/GUI
    cp -a ./client ~/goslayer
    echo "installation complete!!!"
else
    echo "file:///home/$USER/goslayer/client/index.html?port=$port";
    firefox "file:///home/$USER/goslayer/client/index.html?port=$port" & 
    if [ "$mode" -ne "" ]; then
        if [ "$debug" = "1" ]; then
            ~/goslayer/bin/goslayer-executable --mode $mode --port $port --uri $uri
        else
            ~/goslayer/bin/goslayer-executable --mode $mode --port $port --uri $uri &
        fi
    else
        if [ "$debug" = "1" ]; then
            ~/goslayer/bin/goslayer-executable --port $port --uri $uri
        else
            ~/goslayer/bin/goslayer-executable --port $port --uri $uri &
        fi
    fi
fi
