# Starlight
Starlight is an x86_64 (64-bit) operating system created from scratch in C.

This is a learning project, and not built for daily use.

## Build
> [!CAUTION]
> Building should work on all UNIX or UNIX-like OSes, but it is not guaranteed. I build on arm64 macOS, so there is no guarantee it'll compile on x86_64 Linux, for example.
>
> It may work on Windows with WSL2 or Cygwin, but please note that again, it is not guaranteed.

Building is pretty easy. You'll want a couple dependencies.
- macOS: `brew install xorriso qemu` (You will need [brew](https://brew.sh/) installed)
- Debian/Ubuntu: `apt install xorriso qemu-system qemu-utils`
- Arch Linux: `pacman -S qemu`

Please see your OSes package manager for the correct package names if not listed above.