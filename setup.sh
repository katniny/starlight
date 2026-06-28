# download the latest Limine binary release
curl -L https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz | gunzip | tar -xf -

# build "limine" utility
make -C limine-binary

# create a directory which will be our ISO root
mkdir -p iso_root

# create the build dir
mkdir -p build

# copy the relevant files over
mkdir -p iso_root/boot
cp -v bin/starlight iso_root/boot/
mkdir -p iso_root/boot/limine
cp -v limine.conf limine-binary/limine-bios.sys limine-binary/limine-bios-cd.bin \
      limine-binary/limine-uefi-cd.bin iso_root/boot/limine/

# create the EFI boot tree and copy Limine's EFI executables over
mkdir -p iso_root/EFI/BOOT
cp -v limine-binary/BOOTX64.EFI iso_root/EFI/BOOT/
cp -v limine-binary/BOOTIA32.EFI iso_root/EFI/BOOT/

# create the bootable iso
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o build/image.iso

# install Limine stage 1 and 2 for legacy bios boot
./limine-binary/limine bios-install build/image.iso