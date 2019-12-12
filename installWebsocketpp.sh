#!/bin/bash

sudo apt-get install libboost-all-dev
git clone https://github.com/zaphoyd/websocketpp.git
cd websocketpp
cmake .
sudo make install
cd ..
