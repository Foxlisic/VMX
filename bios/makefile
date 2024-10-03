all:
	sjasm bios.asm bios.rom
	php ../utils/bin2mif.php 32 bios.rom ../de0/rom32.mif
	../app/vmx -2 -r0 bios.rom -D
test:
	dd if=../scr/firecast.scr of=sd.img seek=1 bs=512 conv=notrunc
mnt:
	sudo mount sd.img -t vfat -o loop,rw,uid="`whoami`",sync,offset=$[1048576] disk/
unmnt:
	sudo umount -l disk/