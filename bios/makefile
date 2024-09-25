all:
	sjasm bios.asm bios.rom
	php ../utils/bin2mif.php 32 bios.rom ../de0/rom32.mif
	../app/vmx -2 -r0 bios.rom -D
