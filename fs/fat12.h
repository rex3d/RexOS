#pragma once
#include <stdint.h>

#define SECTOR_SIZE 512

// BIOS Parameter Block (BPB) dla FAT12 (i FAT16)
struct fat12_bpb {
    uint8_t  jmp[3];              // Skok do kodu startowego
    char     oem[8];              // OEM Name
    uint16_t bytes_per_sector;    // Bajtów na sektor (zazwyczaj 512)
    uint8_t  sectors_per_cluster; // Sektorów na klaster
    uint16_t reserved_sectors;    // Sektorów zarezerwowanych przed FAT
    uint8_t  fat_count;           // Ilość FAT (zazwyczaj 2)
    uint16_t root_entries;        // Ilość wpisów w katalogu głównym
    uint16_t total_sectors_short; // Całkowita liczba sektorów jeśli < 65535
    uint8_t  media_descriptor;    // Typ nośnika
    uint16_t sectors_per_fat;     // Liczba sektorów na jedną FAT
    uint16_t sectors_per_track;   // Liczba sektorów na ścieżkę (dla dysków fizycznych)
    uint16_t head_count;          // Liczba głowic
    uint32_t hidden_sectors;      // Ukryte sektory przed partycją
    uint32_t total_sectors_long;  // Całkowita liczba sektorów jeśli > 65535
} __attribute__((packed));

// Wpis katalogowy FAT12 (8.3 format)
struct fat12_dir_entry {
    char     name[8];        // Nazwa pliku (bez rozszerzenia), spacje na końcu
    char     ext[3];         // Rozszerzenie pliku, spacje na końcu
    uint8_t  attr;           // Atrybuty pliku (readonly, hidden, system itd.)
    uint8_t  reserved[10];   // Zarezerwowane / metadata
    uint16_t time;           // Czas modyfikacji (bitowe pola)
    uint16_t date;           // Data modyfikacji (bitowe pola)
    uint16_t first_cluster;  // Pierwszy klaster pliku (0 = pusty)
    uint32_t file_size;      // Rozmiar pliku w bajtach
} __attribute__((packed));

// Funkcja odczytu pliku FAT12
int fat12_read_file(const char* filename, void* buffer, int max_len);
