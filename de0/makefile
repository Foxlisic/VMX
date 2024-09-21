VLIB=/usr/share/verilator/include

all: app
	./vmx > vmx.log

app: syn
	g++ -Ofast -o vmx -I../app/roms -I$(VLIB) \
		-Wno-unused-result \
		vmx.cc \
		$(VLIB)/verilated_threads.cpp \
		$(VLIB)/verilated.cpp \
		obj_dir/Vz80__ALL.a \
		-lSDL2
syn:
	verilator -cc z80.v > /dev/null
	cd obj_dir && make -f Vz80.mk > /dev/null
clean:
	rm -r obj_dir tb
