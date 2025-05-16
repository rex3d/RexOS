#include "fat12.h"
#include <string.h>
#include <stdbool.h>
#include <strings.h>  // strcasecmp

// Zewnętrzna funkcja do odczytu sektora z dysku
extern int read_sector(uint32_t lba, void* buffer);

// Pomocnicza funkcja do porównania nazw plików FAT12 (8.3)
static bool fat12_match_filename(const struct fat12_dir_entry* entry, const char* filename) {
    char fat_name[13] = {0};
    memcpy(fat_name, entry->name, 8);
    
    // Usuń spacje na końcu nazwy
    for (int i = 7; i >= 0 && fat_name[i] == ' '; --i)
        fat_name[i] = 0;

    if (entry->ext[0] != ' ') {
        strcat(fat_name, ".");
        strncat(fat_name, entry->ext, 3);
    }

    return strcasecmp(fat_name, filename) == 0;
}

// Zwraca następny klaster z FAT12
static uint16_t fat12_next_cluster(const struct fat12_bpb* bpb, uint16_t cluster, uint8_t* sector) {
    uint32_t fat_offset = cluster + (cluster / 2);
    uint32_t fat_sector = bpb->reserved_sectors + (fat_offset / SECTOR_SIZE);
    uint32_t fat_index = fat_offset % SECTOR_SIZE;

    if (read_sector(fat_sector, sector) != 0)
        return 0xFFF; // Błąd – uznaj za koniec

    uint16_t next = *(uint16_t*)&sector[fat_index];
    return (cluster & 1) ? (next >> 4) : (next & 0x0FFF);
}

// Główna funkcja: odczytaj plik z systemu FAT12
int fat12_read_file(const char* filename, void* buffer, int max_len) {
    uint8_t sector[SECTOR_SIZE];
    struct fat12_bpb bpb;

    // 1. Odczytaj sektor rozruchowy (boot sector)
    if (read_sector(0, sector) != 0) return -1;
    memcpy(&bpb, sector, sizeof(bpb));

    // 2. Oblicz lokalizację katalogu głównego
    uint32_t root_dir_sectors = ((bpb.root_entries * 32) + (bpb.bytes_per_sector - 1)) / bpb.bytes_per_sector;
    uint32_t fat_size = bpb.sectors_per_fat * bpb.fat_count;
    uint32_t root_dir_lba = bpb.reserved_sectors + fat_size;

    // 3. Przeszukaj katalog główny w poszukiwaniu pliku
    for (uint32_t i = 0; i < root_dir_sectors; ++i) {
        if (read_sector(root_dir_lba + i, sector) != 0) return -1;

        for (uint32_t j = 0; j < SECTOR_SIZE / sizeof(struct fat12_dir_entry); ++j) {
            struct fat12_dir_entry* entry = (struct fat12_dir_entry*)&sector[j * sizeof(struct fat12_dir_entry)];

            if (entry->name[0] == 0x00)
                return -1; // Koniec katalogu – nie ma takiego pliku

            if ((entry->attr & 0x0F) == 0x0F)
                continue; // Pomiń długie nazwy (LFN)

            if (fat12_match_filename(entry, filename)) {
                // 4. Odczyt danych pliku z klastra
                uint16_t cluster = entry->first_cluster;
                uint32_t bytes_read = 0;

                while (cluster < 0xFF8 && bytes_read < entry->file_size && bytes_read < (uint32_t)max_len) {
                    uint32_t data_lba = root_dir_lba + root_dir_sectors + (cluster - 2) * bpb.sectors_per_cluster;

                    for (uint8_t s = 0; s < bpb.sectors_per_cluster; ++s) {
                        if (read_sector(data_lba + s, sector) != 0) return -1;

                        uint32_t to_copy = SECTOR_SIZE;
                        if (bytes_read + to_copy > entry->file_size)
                            to_copy = entry->file_size - bytes_read;
                        if (bytes_read + to_copy > (uint32_t)max_len)
                            to_copy = max_len - bytes_read;

                        memcpy((uint8_t*)buffer + bytes_read, sector, to_copy);
                        bytes_read += to_copy;

                        if (bytes_read >= entry->file_size || bytes_read >= (uint32_t)max_len)
                            return bytes_read;
                    }

                    cluster = fat12_next_cluster(&bpb, cluster, sector);
                }

                return bytes_read;
            }
        }
    }

    return -1; // Plik nie znaleziony
}
