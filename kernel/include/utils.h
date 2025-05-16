#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void memset(void* dest, uint8_t val, uint32_t len);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);

#endif // UTILS_H
