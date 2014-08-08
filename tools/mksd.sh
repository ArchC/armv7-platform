#!/bin/bash

########## In program parameters. ##########
DISK="disk.img"
IMG_SIZE=0
IMG_BEGIN=0x77800000

########## Tools. ##########
readelf=${CROSS_COMPILE}readelf
objcopy=${CROSS_COMPILE}objcopy

function error () {
    echo "$0: $1"
    exit 1
}

function usage () {
    cat <<EOF
$(basename $0) -- Create an SD card bootable image

Arguments:

        --so <PATH>: Operating System ELF binary Path
        --user <PATH>: User code ELF binary path.
        --help: Display this message.

Report bugs to gabriel@krisman.be
EOF

exit 1
}

function add_program () {
    ### $1 - Program name

    function add_section () {
        ### $1 - Program name
        ### $2 - section name
        ### $4 - Start Address
        ### $6 - Size
        section_offset=$(($((0x$4)) - IMG_BEGIN))
        section_raw=$(basename $1)$2
        end_address=$(($((0x$4))+$((0x$6))))

        echo "--Adding section from: $1 name: $2 address: 0x$4 size: 0x$6"
        ${objcopy} -O binary -j $2 $1 ${section_raw}
        dd if=${section_raw} of=${DISK} seek=${section_offset} obs=1 2>/dev/null conv=notrunc
        rm ${section_raw}
    }

    echo ------------
    echo "-Adding program $1"

    ${readelf} -S $1 \
        | grep  -G '[WXMSILGTExOop]*A[WXMSILGTExOop]* [0-9 ]*$' \
        | cut --complement -b 1-6 \
        | while read line ;
    do
        add_section $1 $line ;
    done
}

function get_elf_entry_point () {
    ${readelf} -h $1 | grep "Entry point address:" | cut -b 38-47
}

#########

# Argument parsing.

test -n "$1" || usage

while test -n "$1" ; do
    case "$1" in
        "--so")
            test -n "$2" || error "You must provide an operating system."
            SO_PATH=$2
            shift 2
            ;;

        "--user")
            test -n "$2" || error "You must provide an user code."
            USER_PATH=$2
            shift 2
            ;;

        "--help")
            usage
            exit 0
            ;;
        *)
            error "Unknown option $1. See --help for command line usage."
            exit 0
            ;;
    esac
done

test -n "$SO_PATH" || error "You must provide an operating system."
#test -n "$USER_PATH" || error "You must provide an user code."

######### Do the magic.

# Create new disk.
dd if=/dev/zero of=${DISK} bs=1k count=512 2> /dev/null

## Add SO to image.
SO_ENTRY=$(get_elf_entry_point ${SO_PATH})
add_program ${SO_PATH}

## Add user code.
#add_program ${USER_PATH}
echo "Generating IVT... Entry is ${SO_ENTRY}"
ivtgen ${IMG_BEGIN} 1ffff ${SO_ENTRY} \
    | dd of=${DISK} obs=1 seek=$((0x400)) conv=notrunc 2>/dev/null

echo "Disk Ready."
