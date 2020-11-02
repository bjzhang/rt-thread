#!/bin/bash

echo "temporary script, will replace by scons"

OPENAMP_BASE=bsp/riscv64-virt/board/ports/OpenAMP
METAL_INCLUDE=$OPENAMP_BASE/libmetal/lib/include/metal
mkdir -p $METAL_INCLUDE
cp -a $OPENAMP_BASE/libmetal/lib/*.h $METAL_INCLUDE/
for header in `ls $METAL_INCLUDE/*.h`; do
	sed -i "s/@PROJECT_SYSTEM@/rtthread/g" $header
done
