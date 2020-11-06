#!/bin/bash
QEMU_DIR=/home/bamvor/works/source2/risc-v-userapp/rv_uboot/qemu-4.1.0/qemu-4.1.0
sudo $QEMU_DIR/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -m 256M -kernel /home/bamvor/works/source2/risc-v-userapp/rv_uboot/u-boot/u-boot/u-boot  -drive if=none,format=raw,id=image,file=image -device virtio-blk-device,drive=image -netdev tap,id=mytap,ifname=tap0,script=$QEMU_DIR/etc/qemu-ifup,downscript=$QEMU_DIR/etc/qemu-ifdown -device e1000,netdev=mytap
