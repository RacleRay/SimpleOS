GPPPRAMS = -Iinclude -m32 -fno-use-cxa-atexit -fleading-underscore -fno-exceptions -fno-builtin -nostdlib -fno-rtti -fno-pie
ASPARAMS = --32
LDPARAMS = -melf_i386 -no-pie

objs = obj/multitasks.o \
	   obj/pci.o \
	   obj/driver.o \
	   obj/handlers.o \
	   obj/keyboard.o \
	   obj/mouse.o \
       obj/print.o \
	   obj/loader.o \
	   obj/gdt.o \
	   obj/port.o \
	   obj/interrupts.o \
	   obj/interruptstubs.o \
	   obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GPPPRAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

kernel.bin: linker.ld ${objs}
	ld ${LDPARAMS} -T $< -o $@ ${objs}

myos.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "my os" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

.PHONY: clean
clean:
	rm -rf myos.iso kernel.bin obj