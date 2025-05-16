#include "video.h"
#include "loader_screen.h"

void delay(int count) {
    for (volatile int i = 0; i < count * 100000; i++);
}

void draw_progress_bar(int percent) {
    int blocks = percent / 5; // 5% = 1 blok, max 20 bloków
    print_string("[", WHITE, BLACK);
    for (int i = 0; i < 20; i++) {
        if (i < blocks)
            print_char(219, LIGHT_GREEN, BLACK); // █
        else
            print_char(' ', DARK_GREY, BLACK);
    }
    print_string("] ", WHITE, BLACK);

    // Wyświetl % w formacie " 45%"
    char percent_str[5];
    if (percent < 10)
        print_string("  ", LIGHT_CYAN, BLACK);
    else if (percent < 100)
        print_string(" ", LIGHT_CYAN, BLACK);

    itoa(percent, percent_str, 10);
    print_string(percent_str, LIGHT_CYAN, BLACK);
    print_string("%\n", LIGHT_CYAN, BLACK);
}

void show_loading_screen() {
    clear_screen();

    print_string(">> [REXOS] SYSTEM BOOTLOADER v0.1\n", LIGHT_CYAN, BLACK);
    print_string(">> Rozpoczynanie inicjalizacji systemu...\n\n", WHITE, BLACK);

    const char* steps[] = {
        "Inicjalizacja GDT...",
        "Inicjalizacja IDT...",
        "Ustawianie trybu długiego...",
        "Konfiguracja paging'u...",
        "Ładowanie sterowników...",
        "Inicjalizacja klawiatury...",
        "Inicjalizacja systemu plików...",
        "Ładowanie SHELL'a...",
        "Finalizacja bootowania..."
    };

    int step_count = sizeof(steps) / sizeof(steps[0]);

    for (int i = 0; i < step_count; i++) {
        print_string(">> ", LIGHT_BLUE, BLACK);
        print_string(steps[i], WHITE, BLACK);
        delay(3);  // Krótkie opóźnienie
        print_string(" OK\n", LIGHT_GREEN, BLACK);

        draw_progress_bar(((i + 1) * 100) / step_count);
        delay(5); // Dodatkowy delay żeby to nie było instant
    }

    print_string("\n>> SYSTEM ZAŁADOWANY POMYŚLNIE\n", LIGHT_GREEN, BLACK);
    print_string(">> WITAJ, ADMINISTRATORZE\n\n", LIGHT_CYAN, BLACK);
}
