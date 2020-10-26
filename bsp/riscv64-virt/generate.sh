./createimage --extended bootblock rtthread.elf
dd if=image of=image_padded bs=1M conv=sync
mv image_padded image
