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

// framebuffer drawing
static void putpixel(struct limine_framebuffer *fb_info, int x, int y, uint32_t color) {
    uint32_t *fb = fb_info->address;
    uint64_t pitch = fb_info->pitch / 4;

    fb[y * pitch + x] = color;
}

// 8x8 font
static const uint8_t font8x8[128][10] = {
    ['H'] = {0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0x00},
    ['E'] = {0x7E,0x40,0x7C,0x40,0x40,0x40,0x7E,0x00},
    ['L'] = {0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00},
    ['O'] = {0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00},
    ['A'] = {0x18,0x24,0x42,0x7E,0x42,0x42,0x42,0x00},
    ['T'] = {0x7E,0x10,0x10,0x10,0x10,0x10,0x10,0x00},
    ['S'] = {0x3C,0x42,0x30,0x0C,0x42,0x42,0x3C,0x00},
    ['R'] = {0x7C,0x42,0x42,0x7C,0x48,0x44,0x42,0x00},
    ['I'] = {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00},
    ['G'] = {0x3C,0x42,0x40,0x4E,0x42,0x42,0x3C,0x00},
};

static void draw_char(struct limine_framebuffer *fb_info, int x, int y, char c, uint32_t color) {
    for (int row = 0; row < 8; row++) {
        uint8_t bits = font8x8[(uint8_t)c][row];

        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                putpixel(fb_info, x + col, y + row, color);
            }
        }
    }
}

static void draw_string(struct limine_framebuffer *fb_info, int x, int y, const char *str, uint32_t color) {
    int cx = x;

    for (int i = 0; str[i]; i++) {
        draw_char(fb_info, cx, y, str[i], color);
        cx += 8;
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

    draw_string(framebuffer, 50, 50, "HELLO STARLIGHT", 0xFFFFFFFF);

    // we're done, just hang...
    hcf();
}