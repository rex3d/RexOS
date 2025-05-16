#include <ctype.h>

// Funkcja rozbija name[11] na 8 znaków nazwy i 3 rozszerzenia, usuwa spacje
static void fat32_format_filename(const char* fatname, char* out_name) {
    int i, j;

    // Nazwa 8 znaków bez spacji
    for (i = 0; i < 8 && fatname[i] != ' '; ++i) {
        out_name[i] = fatname[i];
    }

    // Dodajemy kropkę, jeśli jest rozszerzenie
    if (fatname[8] != ' ') {
        out_name[i++] = '.';
        // rozszerzenie 3 znaki bez spacji
        for (j = 8; j < 11 && fatname[j] != ' '; ++j) {
            out_name[i++] = fatname[j];
        }
    }
    out_name[i] = '\0';

    // Zamieniamy na duże litery dla spójności (FAT32 trzyma uppercase)
    for (int k = 0; out_name[k]; ++k) {
        out_name[k] = toupper((unsigned char)out_name[k]);
    }
}

// Porównanie nazwy pliku (ignorujemy wielkość liter)
static bool fat32_match_filename(const struct fat32_dir_entry* entry, const char* filename) {
    char fat_name[13];
    fat32_format_filename(entry->name, fat_name);

    // Porównaj wielkość liter ignorując case
    char filename_upper[13];
    int i = 0;
    for (; i < 12 && filename[i]; ++i) {
        filename_upper[i] = toupper((unsigned char)filename[i]);
    }
    filename_upper[i] = 0;

    return strcmp(fat_name, filename_upper) == 0;
}
