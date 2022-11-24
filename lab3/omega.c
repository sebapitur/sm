#include <stdio.h>
#include <stdlib.h>
#define DISPLAY_STRING "level = %d, tip conexiune = %s, pozitie curenta %c%c%c, shuffle = %c%c%c, block number = %d\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

struct pair
{
    int s, d;
};

unsigned char get_last_bit(int val, int bits) {
    return (val & (1 << (bits - 1))) ? 1 : 0; 
}

int shuffle(int val, int bits) {
    unsigned char last_bit = get_last_bit(val, bits);
    val <<= 1;
    val = last_bit ? (val | 1) : val;
    val &= ~(1 << (bits));


    return val;
}

void compute_path(int source, int destination, int levels) {
    int aux_source = shuffle(source, levels);

    for (int i = 0; i < levels; i++) {
        int index = levels - i - 1;
        int parity = (source & (1 << index)) == (destination & (1 << index));
        char *connection_type = parity ? "directa" : "inversa";
        int aux_dest;

        if (!parity) {
            if (aux_source % 2 == 1) {
                aux_source--;
            } else {
                aux_source++;
            }
        }

        if (i < levels - 1) {
            aux_dest = shuffle(aux_source, levels);
            int block_number = aux_source / 2;
            // printf(DISPLAY_STRING, i, connection_type, BYTE_TO_BINARY(aux_source), BYTE_TO_BINARY(aux_dest), block_number);
            printf("level = %d, tip conexiune = %s, pozitie curenta %d, shuffle %d, block number = %d\n", i, connection_type, aux_source, aux_dest, block_number);
        } else {
            int block_number = aux_source / 2;
            // printf("level = %d, tip conexiune = %s, pozitie curenta %c%c%c, block number = %d\n", i, connection_type, BYTE_TO_BINARY(aux_source), block_number);
            printf("level = %d, tip conexiune = %s, pozitie curenta %d, block number = %d\n", i, connection_type, aux_source, block_number);
        }
        
        aux_source = aux_dest;
    }

    printf("\n");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Please specify k value as: './prog [k] [m]'");
        return 0;
    }

    int k = atoi(argv[1]);
    int m = atoi(argv[2]);
    struct pair pairs[m];
    
    // printf("k=%d m=%d\n", k, m);

    for (int i = 0; i < m; i++)
    {
        scanf("%d", &pairs[i].s);
        scanf("%d", &pairs[i].d);
    }

    for (int i = 0; i < m; i++) {
        printf("sursa = %d destinatie = %d\n", pairs[i].s, pairs[i].d);
        compute_path(pairs[i].s, pairs[i].d, k);
    }

    return 0;
}