// kernel64.c - prosty 64-bitowy kernel startowy z hlt w pętli, żeby nie zjadać prądu jak debil
void kernel_main() {
    // Można tu jebnąć jakiś komunikat przez VGA lub serial, ale w wersji hardcore zostawimy minimal
    while(1) {
        // Używamy instrukcji halt, żeby procesor się nie grzał i nie żarł prądu na bezsensownym loopie
        __asm__ volatile ("hlt");
    }
}
