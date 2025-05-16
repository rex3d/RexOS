#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init();
char keyboard_get_char();
void keyboard_handle_interrupt();  // obsługa przerwania klawiatury

#endif
