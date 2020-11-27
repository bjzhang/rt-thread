qemu-system-riscv64 -M virt -m 512M -smp 2     \
        -bios /home/bamvor/works/source2/opensbi/build/platform/generic/firmware/fw_jump.bin	\
        -kernel /home/bamvor/works/source2/linux/linux/arch/riscv/boot/Image			\
        -initrd /home/bamvor/works/source2/rt-thread/bsp/riscv64-virt/rtthread.bin		\
        -append "rootwait root=/dev/vda ro maxcpus=1"						\
	-drive file=/home/bamvor/works/source2/buildroot/output/images/rootfs.ext2,format=raw,id=hd0 \
	-device virtio-blk-device,drive=hd0							\
	-drive file=/home/bamvor/works/source2/software/fedora/Fedora-Developer-Rawhide-20200108.n.0-sda.qcow2,format=qcow2,id=hd1 -device virtio-blk-device,drive=hd1					\
	-netdev user,id=net0 -device virtio-net-device,netdev=net0 -nographic $*
