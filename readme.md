sudo qemu-system-x86_64 -smp 3 --enable-kvm -bios /usr/share/edk2-ovmf/OVMF.fd -hda fat:ro:boot/ -m 256m -net none
