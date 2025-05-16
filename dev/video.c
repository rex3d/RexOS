#include "video.h"
#include "utils.h"  // bo potrzebujesz outb()

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* const VGA_BUFFER = (uint16_t*)VGA_ADDRESS;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void move_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void video_clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = (uint16_t)(' ' | (0x07 << 8));
    }
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

void video_clear() {
    video_clear_screen();
}

void video_print_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\t') {
        int spaces = 4 - (cursor_x % 4);
        for (int i = 0; i < spaces; i++) video_print_char(' ');
    } else {
        VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)(c | (0x07 << 8));
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    if (cursor_y >= VGA_HEIGHT) {
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            VGA_BUFFER[i] = VGA_BUFFER[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            VGA_BUFFER[i] = (uint16_t)(' ' | (0x07 << 8));
        }
        cursor_y = VGA_HEIGHT - 1;
    }
    move_cursor();
}

void video_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        video_print_char(str[i]);
    }
}
