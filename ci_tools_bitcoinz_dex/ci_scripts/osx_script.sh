#!/bin/bash

brew update
brew install autoconf \
            automake \
            libtool \
            pkgconfig \
            wget \
            nim \
            ninja \
            gnu-sed \
            coreutils \
            llvm \
            gnu-getopt

pip3 install yq
export CC=clang
export CXX=clang++
export MACOSX_DEPLOYMENT_TARGET=10.14

# get curl
#git clone https://github.com/KomodoPlatform/curl.git
#cd curl
#git checkout curl-7_70_0
#./buildconf
#./configure --disable-shared --enable-static --without-libidn2 --without-ssl --without-nghttp2 --disable-ldap --with-darwinssl
#make -j3 install
#cd ../

git clone https://github.com/KomodoPlatform/libwally-core.git
cd libwally-core
./tools/autogen.sh
./configure --disable-shared
sudo make -j3 install
cd ..

# get SDKs
git clone https://github.com/KomodoPlatform/MacOSX-SDKs $HOME/sdk
