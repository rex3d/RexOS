// shell.c
#include "video.h"
#include "keyboard.h"
#include <string.h>  // do strcmp, strncmp

#define CMD_BUFFER_SIZE 128

static char command_buffer[CMD_BUFFER_SIZE];
static int command_index = 0;

void shell_run() {
    video_print("SimpleShell v0.1\n> ");
    while (1) {
        char c = 0;
        while (!(c = keyboard_get_char()));
        
        if (c == '\b') {
            if (command_index > 0) {
                command_index--;
                video_print("\b \b");  // usuń znak z ekranu
            }
        } else if (c == '\n' || c == '\r') {  // obsłuż też enter CR
            command_buffer[command_index] = 0;
            video_print("\n");
            
            if (command_index == 0) {
                video_print("> ");
                continue;
            }
            
            if (!strcmp(command_buffer, "help")) {
                video_print("Help: commands: help, clear, echo\n");
            } else if (!strcmp(command_buffer, "clear")) {
                video_clear();
            } else if (!strncmp(command_buffer, "echo ", 5)) {
                video_print(command_buffer + 5);
                video_print("\n");
            } else {
                video_print("Unknown command\n");
            }
            
            command_index = 0;
            video_print("> ");
        } else if (c >= 32 && c <= 126) {  // drukowalne znaki tylko
            if (command_index < CMD_BUFFER_SIZE - 1) {
                command_buffer[command_index++] = c;
                char s[2] = {c, 0};
                video_print(s);
            }
        }
        // inne znaki ignorujemy
    }
}
