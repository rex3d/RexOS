// filesystem.c
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "fat32.h"
#include "fat12.h"

#define SECTOR_SIZE 512

extern int read_sector(uint32_t lba, void* buffer);

// Pomocnicza funkcja formatowania nazwy FAT 8.3 do stringa
static void fat_format_filename(const char* fatname, char* out_name) {
    int i, j;

    for (i = 0; i < 8 && fatname[i] != ' '; ++i) {
        out_name[i] = fatname[i];
    }

    if (fatname[8] != ' ') {
        out_name[i++] = '.';
        for (j = 8; j < 11 && fatname[j] != ' '; ++j) {
            out_name[i++] = fatname[j];
        }
    }

    out_name[i] = '\0';

    for (int k = 0; out_name[k]; ++k) {
        out_name[k] = toupper((unsigned char)out_name[k]);
    }
}

// Porównaj nazwę pliku (case insensitive)
static bool fat_match_filename(const char* a, const char* b) {
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

// Odczyt FAT12 - prosty, root dir na stałym miejscu (np sektor 19), cluster size = 1 sector
static int fs_read_file_fat12(const char* filename, void* buffer, int max_len, const struct fat12_bpb* bpb) {
    uint8_t sector[SECTOR_SIZE];
    char name_formatted[13];
    int root_dir_sectors = ((bpb->root_entries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    int root_dir_start = bpb->reserved_sectors + bpb->fat_count * bpb->sectors_per_fat;

    for (int sector_i = 0; sector_i < root_dir_sectors; ++sector_i) {
        if (read_sector(root_dir_start + sector_i, sector) != 0) return -1;

        for (int offset = 0; offset < SECTOR_SIZE; offset += 32) {
            const struct fat12_dir_entry* entry = (const struct fat12_dir_entry*)(sector + offset);
            if (entry->name[0] == 0x00) return -1; // koniec katalogu
            if ((uint8_t)entry->name[0] == 0xE5) continue; // usuniety plik
            if (entry->attr == 0x0F) continue; // long file name - pomijamy

            fat32_format_filename(entry->name, name_formatted);

            if (fat_match_filename(name_formatted, filename)) {
                // Znaleziono plik, czytamy jego klastry
                uint16_t cluster = entry->first_cluster;
                uint32_t size = entry->file_size;
                uint32_t bytes_read = 0;
                uint32_t data_start = root_dir_start + root_dir_sectors + bpb->reserved_sectors; // mniej więcej - uproszczenie

                while (cluster >= 2 && cluster < 0xFF8 && bytes_read < size && bytes_read < (uint32_t)max_len) {
                    uint32_t sector_lba = data_start + (cluster - 2);
                    if (read_sector(sector_lba, sector) != 0) return -1;
                    uint32_t to_copy = SECTOR_SIZE;
                    if (bytes_read + to_copy > size) to_copy = size - bytes_read;
                    if (bytes_read + to_copy > (uint32_t)max_len) to_copy = max_len - bytes_read;
                    memcpy((uint8_t*)buffer + bytes_read, sector, to_copy);
                    bytes_read += to_copy;

                    // odczytaj następny klaster z FAT
                    // TODO: trzeba czytać FAT i pobrać następny klaster
                    // tutaj dla prostoty zakładam brak fragmentacji i cluster++
                    cluster++;
                }
                return bytes_read;
            }
        }
    }

    return -1; // nie znaleziono pliku
}

// Odczyt FAT32 - root katalog zaczyna się od klastra root_cluster
static uint32_t fat32_next_cluster(const struct fat32_bpb* bpb, uint32_t cluster, uint8_t* fat_sector) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector_num = bpb->reserved_sectors + (fat_offset / SECTOR_SIZE);
    uint32_t ent_offset = fat_offset % SECTOR_SIZE;

    if (read_sector(fat_sector_num, fat_sector) != 0) return 0x0FFFFFFF; // koniec

    uint32_t next = *(uint32_t*)&fat_sector[ent_offset];
    return next & 0x0FFFFFFF;
}

static int fs_read_file_fat32(const char* filename, void* buffer, int max_len, const struct fat32_bpb* bpb) {
    uint8_t sector[SECTOR_SIZE];
    char name_formatted[13];

    uint32_t cluster = bpb->root_cluster;
    uint32_t sectors_per_cluster = bpb->sectors_per_cluster;
    uint32_t first_data_sector = bpb->reserved_sectors + (bpb->fat_count * bpb->fat_size_32);

    while (cluster < 0x0FFFFFF8) {
        for (uint32_t s = 0; s < sectors_per_cluster; ++s) {
            uint32_t lba = first_data_sector + (cluster - 2) * sectors_per_cluster + s;
            if (read_sector(lba, sector) != 0) return -1;

            for (int i = 0; i < SECTOR_SIZE / sizeof(struct fat32_dir_entry); ++i) {
                struct fat32_dir_entry* entry = (struct fat32_dir_entry*)(sector + i * sizeof(struct fat32_dir_entry));
                if (entry->name[0] == 0x00) return -1;
                if ((entry->attr & 0x0F) == 0x0F) continue;

                fat32_format_filename(entry->name, name_formatted);
                if (fat_match_filename(name_formatted, filename)) {
                    uint32_t file_cluster = ((uint32_t)entry->first_cluster_high << 16) | entry->first_cluster_low;
                    uint32_t size = entry->file_size;
                    uint32_t bytes_read = 0;
                    uint32_t current_cluster = file_cluster;

                    while (current_cluster < 0x0FFFFFF8 && bytes_read < size && bytes_read < (uint32_t)max_len) {
                        for (uint32_t sc = 0; sc < sectors_per_cluster; ++sc) {
                            uint32_t file_lba = first_data_sector + (current_cluster - 2) * sectors_per_cluster + sc;
                            if (read_sector(file_lba, sector) != 0) return -1;

                            uint32_t to_copy = SECTOR_SIZE;
                            if (bytes_read + to_copy > size) to_copy = size - bytes_read;
                            if (bytes_read + to_copy > (uint32_t)max_len) to_copy = max_len - bytes_read;

                            memcpy((uint8_t*)buffer + bytes_read, sector, to_copy);
                            bytes_read += to_copy;

                            if (bytes_read >= size || bytes_read >= (uint32_t)max_len)
                                return bytes_read;
                        }
                        current_cluster = fat32_next_cluster(bpb, current_cluster, sector);
                    }
                    return bytes_read;
                }
            }
        }
        cluster = fat32_next_cluster(bpb, cluster, sector);
    }
    return -1;
}

// Główna funkcja odczytu pliku (wybiera FAT12 lub FAT32)
int fs_read_file(const char* filename, void* buffer, int max_len) {
    uint8_t sector[SECTOR_SIZE];
    if (read_sector(0, sector) != 0) return -1;

    struct fat32_bpb* bpb32 = (struct fat32_bpb*)sector;
    struct fat12_bpb* bpb12 = (struct fat12_bpb*)sector;

    if (bpb32->fat_size_32 != 0) {
        // FAT32
        return fs_read_file_fat32(filename, buffer, max_len, bpb32);
    } else {
        // FAT12
        return fs_read_file_fat12(filename, buffer, max_len, bpb12);
    }
}
