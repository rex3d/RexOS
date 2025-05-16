#include "utils.h"

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void memset(void* dest, uint8_t val, uint32_t len) {
    uint8_t* ptr = (uint8_t*)dest;
    for (uint32_t i = 0; i < len; i++) {
        ptr[i] = val;
    }
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, int n) {
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == 0 || s2[i] == 0)
            return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}
int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}
int strnlen(const char* str, int n) {
    int len = 0;
    while (len < n && str[len] != '\0') {
        len++;
    }
    return len;
}
int strncat(char* dest, const char* src, int n) {
    int dest_len = strlen(dest);
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';
    return dest_len + i;
}
int strncopy(char* dest, const char* src, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return i;
}

