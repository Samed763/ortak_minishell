#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("olmayan_dosya.txt", "r");
    if (!f) {
        perror("Dosya açma hatası");
        exit(EXIT_FAILURE);
    }
    fclose(f);
    return 0;
}
