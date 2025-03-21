#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_VALUE ((unsigned long long)1<<32) //2^31
#define N 100000
typedef struct Generator {
    int a; //mnożnik
    int c; //przyrost
    long long seed;
}Generator;

typedef struct LFSR {
    uint32_t seed;
    uint32_t p;
    uint32_t q;
}LFSR;


int match_range(uint32_t number) {
    int range = MAX_VALUE / 10;

    if (number >= 0 && number <= range) return 0;
    else if (number > range && number <= 2 * range) return 1;
    else if (number > 2 * range && number <= 3 * range) return 2;
    else if (number > 3 * range && number <= 4 * range) return 3;
    else if (number > 4 * range && number <= 5 * range) return 4;
    else if (number > 5 * range && number <= 6 * range) return 5;
    else if (number > 6 * range && number <= 7 * range) return 6;
    else if (number > 7 * range && number <= 8 * range) return 7;
    else if (number > 8 * range && number <= 9 * range) return 8;
    else if (number > 9 * range && number <= MAX_VALUE) return 9;

    return -1;  // Jeśli liczba jest poza zakresem
}

void linear_generator(Generator* gen, long long* buckets) {
    int index = 0;
    for (int i = 0; i<N; i++) {
        index = match_range(gen->seed);
       if (index!=-1) {
            buckets[index]++;
            gen->seed = ((unsigned long long)gen->a*gen->seed + gen->c)%MAX_VALUE;
        }else {
            printf("Wystapil blad indexowania!\n");
            return;
        }
    }
    int counter=0;
    printf("Rozklad liczb dla Generatora Liniowego:\n");
    for (int i = 0; i<10; i++) {
        printf("%d ", buckets[i]);
        counter+=buckets[i];
    }
    printf("\n%d\n",counter);
}

void LFSR_generator(LFSR* gen, long long* buckets) {
    uint32_t curr_seed = gen->seed;
    uint32_t bit_p=0;
    uint32_t bit_q=0;
    uint32_t new_shape = 0;
    for (int i = 0; i<31; i++) {
        uint8_t temp = 0;
        bit_p = curr_seed & (1<<(gen->p)); //getting p,q bits from current seed
        bit_q = curr_seed & (1<<(gen->q));
        bit_p = bit_p >> gen->p;
        bit_q = bit_q >> gen->q;
        curr_seed = gen->seed >> 1; //shifting curr_seed
    //dodawanie wyrzuconego bitu do outputowej liczby
        temp = gen->seed & 1; //zostawianie tylko najmlodszego bitu
        new_shape = new_shape | temp; //umieszczanie nowego bitu k-bitowej liczby na koncu
        new_shape = new_shape << 1; //robimy miejsce
    //umieszczanie xor bitu w seedzie
        curr_seed = curr_seed | ((bit_p^bit_q)<<31);
    }
    gen->seed = curr_seed;
    //printf("%u \n", new_shape);
    int index = match_range(gen->seed);
    if (index!=-1) {
        buckets[index]++;
    }
}

int main(void) {
    Generator lin_gen;
    lin_gen.a = 69069;
    lin_gen.c = 2;
    lin_gen.seed = 15;
    unsigned long long buckets[10]={0};
    linear_generator(&lin_gen,buckets);
    LFSR lfsr_gen;
    lfsr_gen.seed = 0xDEADBEEF;
    lfsr_gen.q = 28;
    lfsr_gen.p = 31;
    uint8_t test=15; //0001111

    unsigned long long buckets2[10] = {0};
    int counter = 0;
    for (int i=0; i<N; i++) {
        LFSR_generator(&lfsr_gen, buckets2);
    }
    printf("Rozklad liczb dla Generatora opartego na rejestrach przesuwnych:\n");
    for (int i = 0; i < 10; i++) {
        printf("%llu ", buckets2[i]);
        counter+=buckets2[i];
    }
    printf("\n%d",counter);
    return 0;
}