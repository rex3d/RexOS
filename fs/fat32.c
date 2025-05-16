#include "fat32.h"
#include <string.h>
#include <stdbool.h>

// Zewnętrzna funkcja do odczytu sektora z dysku (musisz mieć samą implementację)
extern int read_sector(uint32_t lba, void* buffer);

// Pomocnicza funkcja porównująca nazwę w formacie 8.3
static bool fat32_match_filename(const struct fat32_dir_entry* entry, const char* filename) {
    char fat_name[13] = {0};
    // Skopiuj nazwę i usuń trailing spacje
    memcpy(fat_name, entry->name, 8);
    for (int i = 7; i >= 0 && fat_name[i] == ' '; --i) fat_name[i] = 0;
    if (entry->ext[0] != ' ') {
        strcat(fat_name, ".");
        strncat(fat_name, entry->ext, 3);
        for (int i = strlen(fat_name) - 1; i >= 0 && fat_name[i] == ' '; --i) fat_name[i] = 0;
    }
    return strcasecmp(fat_name, filename) == 0;
}

// Funkcja pobierająca następny klaster z FAT
static uint32_t fat32_next_cluster(const struct fat32_bpb* bpb, uint32_t cluster, uint8_t* sector) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = bpb->reserved_sectors + (fat_offset / SECTOR_SIZE);
    uint32_t ent_offset = fat_offset % SECTOR_SIZE;
    if (read_sector(fat_sector, sector) != 0) return 0x0FFFFFFF; // error = koniec
    uint32_t next = *(uint32_t*)&sector[ent_offset];
    return next & 0x0FFFFFFF;
}

// Główna funkcja odczytu pliku z FAT32
int fat32_read_file(const char* filename, void* buffer, int max_len) {
    uint8_t sector[SECTOR_SIZE];
    struct fat32_bpb bpb;

    // 1. Wczytaj sektor bootujący
    if (read_sector(0, sector) != 0) return -1;

    // Skopiuj dokładnie BPB (offset 11 do 90 w FAT32, 79 bajtów)
    memcpy(&bpb, sector + 11, sizeof(struct fat32_bpb));

    // 2. Wylicz ważne adresy
    uint32_t first_data_sector = bpb.reserved_sectors + (bpb.fat_count * bpb.sectors_per_fat_32);
    uint32_t root_cluster = bpb.root_cluster;
    uint32_t sectors_per_cluster = bpb.sectors_per_cluster;

    uint32_t cluster = root_cluster;

    while (1) {
        // Iteruj po sektorach klastra katalogu
        for (uint32_t s = 0; s < sectors_per_cluster; ++s) {
            uint32_t lba = first_data_sector + (cluster - 2) * sectors_per_cluster + s;
            if (read_sector(lba, sector) != 0) return -1;
            for (uint32_t i = 0; i < SECTOR_SIZE / sizeof(struct fat32_dir_entry); ++i) {
                struct fat32_dir_entry* entry = (struct fat32_dir_entry*)&sector[i * sizeof(struct fat32_dir_entry)];
                if (entry->name[0] == 0x00) return -1; // koniec katalogu
                if ((entry->attr & 0x0F) == 0x0F) continue; // pomiń LFN

                if (fat32_match_filename(entry, filename)) {
                    // Plik znaleziony
                    uint32_t file_cluster = ((uint32_t)entry->first_cluster_high << 16) | entry->first_cluster_low;
                    uint32_t file_size = entry->file_size;
                    uint32_t bytes_read = 0;

                    while (file_cluster < 0x0FFFFFF8 && bytes_read < file_size && bytes_read < (uint32_t)max_len) {
                        for (uint32_t sc = 0; sc < sectors_per_cluster; ++sc) {
                            uint32_t file_lba = first_data_sector + (file_cluster - 2) * sectors_per_cluster + sc;
                            if (read_sector(file_lba, sector) != 0) return -1;
                            uint32_t to_copy = SECTOR_SIZE;
                            if (bytes_read + to_copy > file_size) to_copy = file_size - bytes_read;
                            if (bytes_read + to_copy > (uint32_t)max_len) to_copy = max_len - bytes_read;
                            memcpy((uint8_t*)buffer + bytes_read, sector, to_copy);
                            bytes_read += to_copy;
                            if (bytes_read >= file_size || bytes_read >= (uint32_t)max_len)
                                return bytes_read;
                        }
                        file_cluster = fat32_next_cluster(&bpb, file_cluster, sector);
                    }
                    return bytes_read;
                }
            }
        }
        // Następny klaster katalogu
        cluster = fat32_next_cluster(&bpb, cluster, sector);
        if (cluster >= 0x0FFFFFF8) break;
    }
    return -1; // Nie znaleziono pliku
}
