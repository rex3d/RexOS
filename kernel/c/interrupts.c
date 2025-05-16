// interrupts.c
#include "interrupts.h"
#include "utils.h"
#include "video.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

extern void isr_stub();  // Deklaracja assemblerowego stubu ISR (CPU exceptions)
extern void irq_stub();  // Deklaracja assemblerowego stubu IRQ (sprzętowe)

static void remap_pic() {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20);  // Offset przerwań dla master PIC (IRQ0-7 -> 32-39)
    outb(PIC2_DATA, 0x28);  // Offset dla slave PIC (IRQ8-15 -> 40-47)

    outb(PIC1_DATA, 0x04);  // Master PIC mówi, że slave jest pod IRQ2
    outb(PIC2_DATA, 0x02);  // Slave PIC identyfikuje się jako połączony z IRQ2

    outb(PIC1_DATA, 0x01);  // Tryb 8086
    outb(PIC2_DATA, 0x01);  // Tryb 8086

    // Odmaskuj wszystko na razie
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

// Musisz mieć implementację idt_set_gate gdzieś u siebie!
// Przykład: ustawienie bramki w IDT pod nr 'n' na handler o adresie 'handler_addr'
extern void idt_set_gate(int n, uint32_t handler_addr);

void interrupts_init() {
    remap_pic();

    // Ustawiamy stuby ISR i IRQ w IDT, żeby CPU wiedziało gdzie skakać
    idt_set_gate(0x20, (uint32_t)isr_stub); // IRQ0 - Timer
    idt_set_gate(0x21, (uint32_t)irq_stub); // IRQ1 - Keyboard

    video_print("Interrupts initialized\n");
}

// Obsługa CPU Exception (przerwanie ISR)
void isr_handler() {
    video_print("CPU Exception (ISR) caught!\n");
    // Tu możesz wypierdolić więcej info, numer wyjątku itd.
}

// Obsługa przerwań sprzętowych (IRQ)
void irq_handler(int irq_num) {
    if (irq_num >= 8) {
        outb(PIC2_COMMAND, 0x20);  // EOI do slave PIC
    }
    outb(PIC1_COMMAND, 0x20);      // EOI do master PIC

    char buf[64];
    snprintf(buf, sizeof(buf), "IRQ %d caught\n", irq_num);
    video_print(buf);

    // Tutaj możesz dodać własną obsługę IRQ, np. klawiatury, timera itd.
}
