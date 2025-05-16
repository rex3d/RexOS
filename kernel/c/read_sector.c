#include <stdint.h>

extern int read_sector_16(uint16_t es, uint16_t bx, uint8_t dl, uint16_t cx, uint8_t dh, uint8_t al);

int read_sector_16bit(uint32_t lba, void* buffer) {
    uint16_t segment = ((uint32_t)buffer) >> 4;
    uint16_t offset = ((uint32_t)buffer) & 0xF;

    uint16_t cylinder = (lba /  (  36)) /  18;
    uint8_t head = (lba / 18) % 2;
    uint8_t sector = (lba % 18) + 1;

    uint16_t cx = (cylinder << 8) | sector;
    uint8_t dh = head;

    int status = read_sector_16(segment, offset, 0x80, cx, dh, 1);
    return status;
}
