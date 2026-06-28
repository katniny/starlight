qemu-system-x86_64 \
    -m 4096 \
    -smp 4 \
    -cpu max \
    -accel tcg \
    -boot d \
    -cdrom build/image.iso \
    -vga virtio \
    -display default