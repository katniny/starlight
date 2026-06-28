// thank you to the osdev wiki for this code:
// https://wiki.osdev.org/Limine_Bare_Bones
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

// set the base revision to 6, this is recommended as this is the latest
// base revision described by the Limine boot protocol spec.
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

// the Limine requests can be placed anywhere, but it is important that
// the compiler does not optimize them away, so, usually, they should
// be made volatile or equivalent, and they should be accessed at least
// once or marked as used with the "used" attribute as done here.
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

// finally, define the start and end markers for the Limine requests.
// these can also be moved anywhere, to any .c file, as seen fit
__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section("limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// gcc and clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// implement them as the C spec mandiates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// they CAN be moved to a different .c file
void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = dest;
    const uint8_t *restrict psrc = src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = dest;
    const uint8_t *psrc = src;

    if ((uintptr_t)src > (uintptr_t)dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if ((uintptr_t)src > (uintptr_t)dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = s1;
    const uint8_t *p2 = s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// halt and catch fire function
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// the following will be our kernel's entry point.
void starmain(void) {
    // ensure the bootloader actually understands our base revision
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    // ensure we got a framebuffer
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // fetch the first framebuffer
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // print a nice pattern to screen as an example
    // note: we assume the framebuffer model is RGB with 32-bit pixels.
    volatile uint32_t *fb_ptr = framebuffer->address;
    for (size_t y = 0; y < framebuffer->height; y++) {
        for (size_t x = 0; x < framebuffer->width; x++) {
            uint32_t nX = x * 255 / framebuffer->width;
            uint32_t nY = y * 255 / framebuffer->height;
            fb_ptr[y * (framebuffer->pitch / 4) + x] = (nY << 8) | nX;
        }
    }

    // we're done, just hang...
    hcf();
}