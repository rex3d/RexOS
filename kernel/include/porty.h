#ifndef PORTY_H
#define PORTY_H

#include <stdint.h>

#ifdef __x86_64__
uint8_t inb64(uint16_t port);
void outb64(uint16_t port, uint8_t val);
#define inb(port) inb64(port)
#define outb(port, val) outb64(port, val)
#else
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
#endif

#endif
