// kernel.c
#include "video.h"
#include "keyboard.h"
#include "shell.h"
#include "loader_screen.h"

// Prosty kernel start, wyświetla komunikat, inicjuje klawiaturę i uruchamia powłokę
void kernel_main() {
    video_clear_screen();
    show_loading_screen();  // ekran ładowania
    shell_start();          // potem normalny shell
    video_print("Kernel startuje! Wersja 0.1.\n");

    keyboard_init();

    // Startujemy shell - kontrola przeklętej maszyny w Twoich łapach
    shell_run();

    // Wkurwiający nieskończony loop, ale bardziej elegancko
    while(1) {
        // Można dać asm volatile("hlt") - halt CPU do momentu przerwania, żeby nie żreć prądu bez sensu
        __asm__ volatile ("hlt");
    }
}
