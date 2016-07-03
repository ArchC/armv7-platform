Full-system ARMv7 Platform Simulator
=====================================

## Background

ArchC is a framework to create single process virtual machines by describing the modeled architecture using a high level description language. Number and size of registers, instruction encoding, memory devices and several other architectural details can be described using ArchC.

Nevertheless, ArchC was limited to describe the processor core and a few other things, and wasn't capable of generating more complex platform virtual machines, capable of executing a full operating system, for example.

In this project, we used an ARMv7 core modeled in ArchC and implemented several other peripherals and SoC components required to fully simulate a Freescale iMX53 Quick Start Board. These include functional modules such as Memory Management Units, UARTs, Storage devices, and buses and also some modules to improve the simulator performance, such as simulated TLBs and cache for instructions already decoded. ***Our goal was to boot a full GNU/Linux under the simulated environment.***

The simulator is used since 2013 to teach computer architecture and Assembly language for undergraduate students at UNICAMP. In this course, students are required to implement several parts of the operating system, such as device drivers, schedulers, syscall handlers, and some userland programs in ARM Assembly. All their code is run on the simulator, allowing them to collect information and data about their programs that could not be available otherwise.

The development of this first platform simulator based on ArchC allowed us to identify some deficiencies in ArchC in describing complex models. As a result several modules where included in the ArchC framework, such as a new Decoder Unit and a cache for decoded instructions with support for self-modifying code.

All of the simulator code, the ROM code to perform bootstrapping and the small operating System we created for use in the classroom, are available under a GPLv3 license at my [Gitweb page](http://git.krisman.be/).

## ARMv7

The ARMv7 Simulator is written in ArchC and C++, and models several devices other than the core, itself. Currently, the following devices are modeled:

* ARMv7 Cortex A8 Core
* Coprocessors
* Buses
* Memory Management UNit
* SD controllers
* UARTs
* General Purpose Timers
* Memories
* …

The Figure below shows an schematics of the implemented modules.

![Alt tag](http://krisman.be/img/armv7_model_architectural_overview.png)

## Environment

First of all, open the ``env.sh`` file and edit the variables:

```bash
SYSTEMC_INSTALL_PATH = point to the SystemC installation folder.
CROSS_COMPILER_PATH  = point to ARM cross-compiler (toolchain) folder
```

See the ``env.sh`` for examples. Look in the **Dumboot** section to download the cross-compiler. 
After that, do it:

```bash
source env.sh
```

Every time that you change the `env.sh` file, you must run the command above. 

## Quick Start

This project has a global Makefile to guide the compilation process without manual intervention.
Besides compiling, the `run` role loads the images and runs the simulation. 
So, it's a good way to start:

```bash
make
make run
```

If you want to understand all the steps, see the [Krisman page](http://krisman.be/armv7.html). The sections below 
were written using this page as a guide, adding some kind information.

## Detailed Installation 

This repository has all the subprojects needed: archc, armv7, dumboot and dummyos. But here we show you how to 
download these subprojects from the Krisman repository. So, **you can skip the git clone process if you use
the subprojects contained herein.**
 
#### ArchC

```bash 
git clone git://git.krisman.be/archc.git
```

Just do

```bash
./configure --prefix=${ARCHC_INSTALL_PATH}     \
        --with-systemc=${SYSTEMC_INSTALL_PATH} \
        --with-tlm=${SYSTEMC_INSTALL_PATH}
make
make install
```


#### Simulator and some utilities.

```bash 
git clone git://git.krisman.be/armv7.git
```

Installation steps:

```bash
./configure --prefix=${ARM_SIMULATOR_PATH}
make
make install
```

#### Dumboot

This is loaded to ROM during the simulator start up and is responsible for detecting the boot device and loading the next step of the boot chain. It follows the specification for iMX53, and can boot any compatible image, such as U-boot bootloader with some operating system payload.

```bash
git clone git://git.krisman.be/dumboot.git
```

Obviously, you are going to need a ARM cross-compiler to build this. Get the ARM cross-compiler [here](http://archc.lsc.ic.unicamp.br/downloads/Tools/arm/archc_arm_toolchain_20150102_64bit.tar.bz2). Remember to set the cross-compiler path in the CROSS_COMPILER_PATH variable into `env.sh`. Now, you can compile the bootloader:

```bash
make 
```

#### DummyOS

Here we are download a small Operating System (OS) to load after the Dumboot. 

```bash
git://git.krisman.be/dummyos.git
```

If the Dumboot was compiled with sucess, you'll compile the DummyOS easily:

```bash
make
```

## Generating the images

In the simulator repository there are some tools to generate compatible images. Assuming you want to generate images for your Dumboot and your own small OS, one can simply do:

```bash
mksd.sh --os dumboot/dumboot.elf
mv disk.img dumboot.img
mksd.sh --os dummyos/knrl 
mv disk.img card.img
```
Two images should be generated. 

## Running the simulator

#### Boot from a SD card

```bash
armsim --rom=dumboot.img --sd=card.img
```

#### Make simulator wait for a GDB connection

```bash
armsim --rom=dumboot.img --sd=card.img --gdb --gdb-port 5000
```

#### Enable debug information for some units.

```bash
armsim --rom=dumboot.img --sd=card.img --debug=core,mmu,uart
```

For further documentation, check the manual in the source or invoke armsim --help

## Contribute

If you think you have found a bug or you want to contribute with this project, you can send your comments and patches to <archc@googlegroups.com>. If your patch is related to the ARM model, ArchC platform stuff or to Dumboot/DummyOS, please Cc: me directly, as well.

## Boot Sequence

![Alt tag](http://krisman.be/img/bootprocess.png)

## Publications

BERTAZI, G. ; AULER, R. ; BORIN, E. . Uma plataforma para o ensino de organização de computadores e linguagem de montagem. International Journal of Computer Architecture Education , v. 3, p. 13, 2014.
[Paper](http://krisman.be/publications/bertazi-weac-2014.pdf)
[Presentation](http://krisman.be/publications/bertazi-weac-2014-presentation.pdf)

## Footnotes:

Author: Gabriel Krisman Bertazi <br />
Created: 2015-08-23 Sun 16:09   <br />
Emacs 24.5.1 (Org mode 8.2.10)  <br />
http://krisman.be/armv7.html

