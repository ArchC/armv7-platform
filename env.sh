#!/bin/sh

# ===================================================
# Edit these path's

export SYSTEMC_INSTALL_PATH="/home/max/ArchC/tools/systemc/2.3.1/"
export CROSS_COMPILER_PATH="/home/max/x-tools/arm-newlib-eabi/"

#====================================================

export ARCHC_INSTALL_PATH="${PWD}/acinstall/"
export ARM_SIMULATOR_PATH="${PWD}/arminstall"

export CROSS_COMPILER_TUPLE="arm-newlib-eabi-"

export C_INCLUDE_PATH="$SYSTEMC_INSTALL_PATH/include:$C_INCLUDE_PATH"
export C_INCLUDE_PATH="$ARCHC_INSTALL_PATH/include/archc:$C_INCLUDE_PATH"

export CPLUS_INCLUDE_PATH="$SYSTEMC_INSTALL_PATH/include:$CPLUS_INCLUDE_PATH"
export CPLUS_INCLUDE_PATH="$ARCHC_INSTALL_PATH/include/archc:$CPLUS_INCLUDE_PATH"

export LIBRARY_PATH="$SYSTEMC_INSTALL_PATH/lib:$LIBRARY_PATH"
export LIBRARY_PATH="$ARCHC_INSTALL_PATH/lib:$LIBRARY_PATH"

export LD_LIBRARY_PATH="$SYSTEMC_INSTALL_PATH/lib:$LD_LIBRARY_PATH"

export CROSS_COMPILE="${CROSS_COMPILER_PATH}/bin/${CROSS_COMPILER_TUPLE}"

export PATH="$ARCHC_INSTALL_PATH/bin:$PATH"
export PATH="${ARM_SIMULATOR_PATH}/bin:$PATH"
export PATH="${ARM_SIMULATOR_PATH}/libexec:${PATH}"

