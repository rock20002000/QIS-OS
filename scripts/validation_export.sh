#!/bin/bash
set -e

if [ -z $1 ];then
    echo "Please set a output directory!"
    exit 1
elif [ -z $2 ];then
    echo "Please set a valid build number!"
    exit 1
else
outdir=$1
fi

if [ -e $outdir ];then
rm -rf $outdir
fi


make clean
# Create work dirs
mkdir -p $outdir/prebuilt/arm $outdir/prebuilt/x86_64 $outdir/sdk/build
cp tools/mkimg $outdir/prebuilt
cp host_settings.mk $outdir/
cp -r include_board/ $outdir/

# Copy useful SDK files
cp -r sdk/includes $outdir/sdk
cp -r sdk/lib $outdir/sdk
cp -r sdk/prebuilt $outdir/sdk
find $outdir/sdk -name "*.c" | xargs rm -rf

# Build up user project
cp -r userspace $outdir/
rm -r $outdir/userspace/service/proc
rm -rf $outdir/userspace/.git


# Build & Copy arch-x86_64 files
make BUILD_NO=$2
cp -r sdk/build/kirk $outdir/sdk/build
cp bsp/build/bsp.elf $outdir/prebuilt/x86_64/
cp kernel/build/kernel.elf $outdir/prebuilt/x86_64/
cp tools/origin_bootable_hdd_kirkipl.img $outdir/prebuilt/x86_64/
cp userspace/build/kirk/x86_64/debug/proc.elf $outdir/prebuilt/x86_64/

# Build & Copy arch-arm files
make BUILD_NO=$2 ARCH=arm
cp -r sdk/build/kirk $outdir/sdk/build
cp bsp/build/extraboot.bin $outdir/prebuilt/arm/
cp bsp/build/bsp.elf $outdir/prebuilt/arm/
cp kernel/build/kernel.elf $outdir/prebuilt/arm/
cp tools/u-boot $outdir/prebuilt/arm/
cp userspace/build/kirk/arm/debug/proc.elf $outdir/prebuilt/arm/


make clean

# Construct makefile for validation project
cp scripts/validation_makefile $outdir/Makefile

sed -i 's/-- Should disable in release version/set_default(false)/g' $outdir/userspace/build_scripts/targets.lua

git rev-parse --short HEAD  >> $outdir/version.log
echo "BUILD_NO=$2" >> $outdir/version.log
