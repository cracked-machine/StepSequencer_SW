FROM ubuntu:22.04

ARG USER=builder

RUN apt update -y && \
    DEBIAN_FRONTEND=noninteractive apt install -y \
    wget \      
    atool \
    sudo \
    git \
    ninja-build \
    pkg-config \
    catch2

RUN useradd -m $USER && echo "$USER:$USER" | chpasswd && adduser $USER sudo

##################### 
# ARM Cross toolchain
#####################

# get 10.2.2021.10
ARG ARM_URL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10
ARG ARM_INSTALLER=gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
RUN wget --directory-prefix /tmp $ARM_URL/$ARM_INSTALLER \
    && aunpack -vf /tmp/$ARM_INSTALLER -X /opt \
    && rm -rf /tmp/$ARM_INSTALLER

# configure cmake kits
RUN mkdir -p -m 777 /home/$USER/.local/share/CMakeTools && \
    echo '\
    [\n\
    {\n\
    "name": "GCC 10.3.1 arm-none-eabi",\n\
    "compilers": {\n\
    "C": "/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc",\n\
    "CXX": "/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++"\n\
    }\n\
    }\n\
    ]\n\
    ' > /home/$USER/.local/share/CMakeTools/cmake-tools-kits.json

# # get 11.3.rel1
# ARG ARM_URL=https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel
# ARG ARM_INSTALLER=arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz
# RUN wget --directory-prefix /tmp $ARM_URL/$ARM_INSTALLER \
#     && aunpack -vf /tmp/$ARM_INSTALLER -X /opt \
#     && rm /tmp/$ARM_INSTALLER

##################### 
# CMake
####################
ARG CMAKE_VERSION=cmake-3.24.2-linux-x86_64
ARG CMAKE_INSTALLER=${CMAKE_VERSION}.tar.gz
ARG CMAKE_INSTALL_PATH=/opt/${CMAKE_VERSION}/bin
RUN wget --directory-prefix /tmp https://github.com/Kitware/CMake/releases/download/v3.24.2/$CMAKE_INSTALLER
RUN aunpack -vf /tmp/$CMAKE_INSTALLER -X /opt


##################### 
# JLink tools
#####################
ARG JLINK_INSTALLER=JLink_Linux_x86_64.tgz
ARG JLINK_DIR=JLink_Linux_V782_x86_64
RUN wget -P /tmp --post-data='accept_license_agreement=accepted' https://www.segger.com/downloads/jlink/${JLINK_INSTALLER}
RUN aunpack -vf /tmp/${JLINK_INSTALLER} -X /opt
# runtime deps - note these are all X-related libs
RUN apt update -y && \
    DEBIAN_FRONTEND=noninteractive apt install -y \
    libfreetype6 \
    libsm6 \
    libxrender1 \
    libxrandr2 \
    libxfixes3 \
    libxcursor1 \
    libfontconfig1

######################
# Catch2
######################
# RUN git clone https://github.com/catchorg/Catch2.git \
#     && cd Catch2 \
#     && ${CMAKE_INSTALL_PATH}/cmake -D"CMAKE_MAKE_PROGRAM:PATH=/usr/bin/ninja" -Bbuild -H. -DBUILD_TESTING=OFF \
#     && ${CMAKE_INSTALL_PATH}/cmake -D"CMAKE_MAKE_PROGRAM:PATH=/usr/bin/ninja" --build build/ --target install

# ##################### 
# # VSCode
# #####################
# # fetch vscode

# ##################### 
# # STM32CubeIDE
# #####################
# # fetch STM32CubeIDE

# Setup PATH to use the installed tools
RUN echo export PATH=/opt/${ARM_INSTALLER%.tar.xz}/bin:${CMAKE_INSTALL_PATH}:/opt/${JLINK_DIR}:$PATH >> /home/$USER/.bashrc

USER $USER
# RUN git clone https://github.com/cracked-machine/BassStationSequencerSW.git --recursive ~/project
WORKDIR /home/$USER/
CMD /bin/bash

ENV CC=/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc \
    CXX=/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++ \
    COV=/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcov \
    GDB=/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gdb \
    SIZE=/opt/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-size