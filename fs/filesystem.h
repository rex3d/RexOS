// filesystem.c
#include "filesystem.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "fat32.h"
#include "fat12.h"

#define SECTOR_SIZE 512

extern int read_sector(uint32_t lba, void* buffer);

#define MAX_FILES 64
#define MAX_FILENAME_DISPLAY 13

typedef struct {
    char name[MAX_FILENAME_DISPLAY];
    uint32_t start_cluster;
    uint32_t size;
} FileEntry;

static FileEntry files[MAX_FILES];
static int file_count = 0;

// Formatowanie nazwy z FAT 8.3 na string
static void fat_format_filename(const char* fatname, char* out_name) {
    int i, j;
    for (i = 0; i < 8 && fatname[i] != ' '; ++i) out_name[i] = fatname[i];
    if (fatname[8] != ' ') {
        out_name[i++] = '.';
        for (j = 8; j < 11 && fatname[j] != ' '; ++j) out_name[i++] = fatname[j];
    }
    out_name[i] = '\0';
    for (int k = 0; out_name[k]; ++k) {
        out_name[k] = toupper((unsigned char)out_name[k]);
    }
}

// Porównanie nazw bez względu na wielkość liter
static bool fat_match_filename(const char* a, const char* b) {
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

// Wczytaj katalog root i załaduj pliki (FAT12 uproszczone)
static void load_fat12_root(const struct fat12_bpb* bpb) {
    uint8_t sector[SECTOR_SIZE];
    int root_dir_sectors = ((bpb->root_entries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;
    int root_dir_start = bpb->reserved_sectors + bpb->fat_count * bpb->sectors_per_fat;

    file_count = 0;

    for (int sector_i = 0; sector_i < root_dir_sectors && file_count < MAX_FILES; ++sector_i) {
        if (read_sector(root_dir_start + sector_i, sector) != 0) break;

        for (int offset = 0; offset < SECTOR_SIZE && file_count < MAX_FILES; offset += 32) {
            const struct fat12_dir_entry* entry = (const struct fat12_dir_entry*)(sector + offset);
            if (entry->name[0] == 0x00) return; // koniec katalogu
            if ((uint8_t)entry->name[0] == 0xE5) continue; // usunięty
            if (entry->attr == 0x0F) continue; // LFN pomijamy

            fat_format_filename(entry->name, files[file_count].name);
            files[file_count].start_cluster = entry->first_cluster;
            files[file_count].size = entry->file_size;
            file_count++;
        }
    }
}

// Wczytaj katalog root FAT32 (prosty)
static void load_fat32_root(const struct fat32_bpb* bpb) {
    uint8_t sector[SECTOR_SIZE];
    uint32_t cluster = bpb->root_cluster;
    uint32_t sectors_per_cluster = bpb->sectors_per_cluster;
    uint32_t first_data_sector = bpb->reserved_sectors + (bpb->fat_count * bpb->fat_size_32);

    file_count = 0;

    while (cluster < 0x0FFFFFF8 && file_count < MAX_FILES) {
        for (uint32_t s = 0; s < sectors_per_cluster; ++s) {
            uint32_t lba = first_data_sector + (cluster - 2) * sectors_per_cluster + s;
            if (read_sector(lba, sector) != 0) return;

            for (int i = 0; i < SECTOR_SIZE / sizeof(struct fat32_dir_entry) && file_count < MAX_FILES; ++i) {
                struct fat32_dir_entry* entry = (struct fat32_dir_entry*)(sector + i * sizeof(struct fat32_dir_entry));
                if (entry->name[0] == 0x00) return;
                if ((entry->attr & 0x0F) == 0x0F) continue;

                fat_format_filename(entry->name, files[file_count].name);
                files[file_count].start_cluster = ((uint32_t)entry->first_cluster_high << 16) | entry->first_cluster_low;
                files[file_count].size = entry->file_size;
                file_count++;
            }
        }
        // TODO: wczytać następny klaster z FAT32 - dla uproszczenia zakładam, że katalog root jest jeden klaster
        break;
    }
}

void fs_init() {
    uint8_t sector[SECTOR_SIZE];
    if (read_sector(0, sector) != 0) return;

    struct fat32_bpb* bpb32 = (struct fat32_bpb*)sector;
    struct fat12_bpb* bpb12 = (struct fat12_bpb*)sector;

    if (bpb32->fat_size_32 != 0) {
        // FAT32
        load_fat32_root(bpb32);
    } else {
        // FAT12
        load_fat12_root(bpb12);
    }
}

void fs_ls() {
    for (int i = 0; i < file_count; ++i) {
        // Wyświetl pliki, np. print(files[i].name);
        // print musisz mieć zdefiniowane u siebie
        print(files[i].name);
    }
}

const char* fs_cat(const char* filename) {
    for (int i = 0; i < file_count; ++i) {
        if (fat_match_filename(files[i].name, filename)) {
            // Tutaj normalnie czytaj plik z klastra files[i].start_cluster i rozmiaru files[i].size
            // Dla uproszczenia zwracam szablonowy string
            return "Zawartosc pliku (mock)";
        }
    }
    return "Plik nie znaleziony";
}
