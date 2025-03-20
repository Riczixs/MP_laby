#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CITY_NUMBER 15
#define CITY_SUBSET 4

typedef struct City {
	int id;
    char name[20];
	int population;
	float x;
	float y;
}City;

typedef struct Indexes{
  int counter; //licznik permutacji
  int perm_index; //określa która to permutacja
  int isUsed; //okresla czy miasto bylo wykorzystane w permutacji
}Indexes;

typedef struct Permutation{
    float min_path;
    int* min_permutation; //przechowyuje minimalną permutację
    int* citizens_permutation; //przechowuje ścieżkę, dla której liczba mieszkancow jest najblizsza 50% wszystkich
    int citizens; //liczba mieszkancow N miast
    int closest_value; //najblizsza liczba (CITY_SUBSET) miast do 50% citizens
    int best_difference; //najbliższa znaleziona różnica do 50%
}Permutation;

//1 2 3 4 - indexy miast
float count_distance(City** cities, int* path){
    float d = 0;
    float l = 0;
    for(int i=1; i<CITY_SUBSET; i++){
        float Xa = cities[path[i-1]-1]->x;
        float Ya = cities[path[i-1]-1]->y;
        float Xb = cities[path[i]-1]->x;
        float Yb = cities[path[i]-1]->y;
        l = sqrtf(((Xb-Xa)*(Xb-Xa)) + (Yb-Ya)*(Yb-Ya));
        d += l;
    }
    //oddzielnie dodanie odleglosci z ostatniego miasta w permutacji do pierwszego(zamkniecie cyklu)
    float Xa = cities[path[CITY_SUBSET-1]-1]->x;
    float Ya = cities[path[CITY_SUBSET-1]-1]->y;
    float Xb = cities[path[0]-1]->x;
    float Yb = cities[path[0]-1]->y;
    l = sqrtf(((Xb-Xa)*(Xb-Xa)) + (Yb-Ya)*(Yb-Ya));
    d+=l;
    return d;
}

void check_population(City** cities, int* path, Permutation* perm) { //sprawdza liczbe mieszkancow w permutacji
    int count_people = 0;
    for(int i = 0; i < CITY_SUBSET; i++){
        count_people+=cities[path[i]-1]->population;
    }
    if((abs(perm->citizens - count_people))< perm->best_difference){
        perm->closest_value = count_people;
        perm->best_difference = abs(perm->citizens - count_people);
        for(int i = 0; i<CITY_SUBSET; i++){
            perm->citizens_permutation[i]=path[i];
        }
    }
}

void free_memory(City** cities, Permutation* perm, int* path, int* used) {
    for (int i = 0; i < CITY_NUMBER; i++) {
        if (cities[i] != NULL) {
            free(cities[i]);
        }
    }
    free(perm->min_permutation);
    free(perm->citizens_permutation);
    free(path);
    free(used);
}
//path-określa obecną permutację, used - przechowuje miasta juz wykorzystane w danej gałęzi, struktury pomocnicze
void wariations(int* path, int* used, Indexes* indexes, City** cities, Permutation* perm) {
    if (indexes->perm_index == CITY_SUBSET) {
        printf("%d. ",indexes->counter);
        for(int i=0;i < CITY_SUBSET; i++){
            printf("%d ",path[i]);
        }
        indexes->counter++;
        printf("\n");
        check_population(cities, path, perm); //sprawdzamy, czy obecna pełna ścieżka, jest bliższa 50% od obecnej minimalnej
        //TODO:SPRAWDZIC CZY MIN_PATH JEST MNIEJSZA OD POPRZEDNIEJ I W RAZIE W ZMIENIC PRZECHOWYWANĄ PERMUTACJE
        float check_path = count_distance(cities, path);
        if (perm->min_path > check_path) {
            for (int j = 0; j < CITY_SUBSET; j++) {
                perm->min_permutation[j] = path[j]; //ustawiamy bieżącą permutację, jako najmniejszą
            }
            perm->min_path = check_path; //minimalna ścieżka
        }
        indexes->counter++;
        return;
    }

    for (int i = 0; i < CITY_NUMBER; i++) {
        for (int j = 0; j < CITY_SUBSET; j++) {
            if ((i + 1 == used[j]) && (used[j] != 0)) indexes->isUsed = 1;
        }
        if (indexes->isUsed == 0) {
            used[indexes->perm_index] = i + 1;
            path[indexes->perm_index] = i + 1;
            (indexes->perm_index)++;
            wariations(path, used, indexes, cities, perm);
            if (indexes->perm_index > 0) {
                used[indexes->perm_index] = 0;
                used[indexes->perm_index] = 0;
                (indexes->perm_index)--;
            }
        } else {
            indexes->isUsed = 0;
        }
    }
}

void combinations(int* path, int* used, Indexes* indexes) {
    if (indexes->perm_index == CITY_NUMBER) {
        //checking number of citizens
        printf("%d. ",indexes->counter);
        for(int i=0;i < CITY_NUMBER; i++){
            printf("%d ",path[i]);
        }
        indexes->counter++;
        printf("\n");
        return;
    }
    for (int i = 0; i < CITY_NUMBER; i++) {
        for (int j = 0; j < CITY_SUBSET; j++) {
            if ((i + 1 == used[j]) && (used[j] != 0)) indexes->isUsed = 1;
        }
        if (indexes->isUsed == 0) {
            used[indexes->perm_index] = i + 1;
            path[indexes->perm_index] = i + 1;
            (indexes->perm_index)++;
            combinations(path, used, indexes);
            if (indexes->perm_index > 0) {
                used[indexes->perm_index] = 0;
                used[indexes->perm_index] = 0;
                (indexes->perm_index)--;
            }
        } else {
            indexes->isUsed = 0;
        }
    }
}


int main() {
	FILE* france;
	france = fopen("C:\\Users\\rysie\\CLionProjects\\MP_lab1\\france.txt", "r");
	if (france == NULL) {
		printf("Nie udalo sie odczytac pliku\n");
	}
	else {
		printf("Udalo sie otworzyc plik!\n");
	}


	City* cities[CITY_NUMBER];

	//ALOCATING SPACE FOR EACH CITY
	for (int i = 0; i < CITY_NUMBER; i++) {
		cities[i] = (City*)malloc(sizeof(City));
        cities[i]->id=0;
        for(int j=0; j<20; j++){
            cities[i]->name[j]='0';
        }
        cities[i]->population=0;
        cities[i]->x=0;
        cities[i]->y=0;
	}
	fscanf(france, "%*[^\n]");
    Permutation perm;
    perm.citizens=0;
    for (int i = 0; i < CITY_NUMBER; i++) {
        int result = fscanf(france, "%d %*s %d %f %f",
                            &cities[i]->id,
                            &cities[i]->population,
                            &cities[i]->x,
                            &cities[i]->y);
        if (result != 4) {
            printf("Blad przy czytaniu linii numer %d\n", i + 1);
            break;
        }
        perm.citizens += cities[i]->population;
    }
    perm.citizens /= 2; //50% z liczby wszystkich mieszkańców miast

    fclose(france);

    Indexes index;
    index.perm_index=0;
    index.counter=1;
    index.isUsed=0;
    perm.min_permutation=malloc(CITY_SUBSET*sizeof(int));
    perm.citizens_permutation=malloc(CITY_SUBSET*sizeof(int));
    perm.best_difference = INT_MAX;
    for(int i=0; i<CITY_SUBSET; i++){
        perm.min_permutation[i]=0;
        perm.citizens_permutation[i]=0;
    }
    perm.min_path=INT_MAX;
    perm.closest_value = INT_MAX;
    int* path = malloc(CITY_NUMBER*sizeof(int));
    int* used = malloc(CITY_NUMBER*sizeof(int));

    if(!perm.min_permutation || !path || !used){
        perror("blad malloc dla perm/path/used");
        free_memory(cities, &perm, path, used);
        return 1;
    }

    for(int i=0; i<CITY_SUBSET; i++){
        path[i]=0;
        used[i]=0;
    }
//////WYSWIETLANIE NAJBLIZSZEJ PERMUTACJI DO 50%
    wariations(path, used, &index, cities, &perm);
    //combinations(path,used,&index);
    printf("\n50% wszystkich mieszkancow: %d Najblizsza wartosc do 50%: %d\n",perm.citizens,perm.closest_value);
    for(int i = 0; i < CITY_SUBSET; i++){
        printf("%d ", perm.citizens_permutation[i]);
    }
    printf("\n");


//////WYSWIETLANIE MINIMALNEJ DROGI KOMIWOJAZERA WRAZ Z DYSTANSEM
    printf("\nMinimalny dystans: %f\n", perm.min_path);
    for(int i = 0; i < CITY_SUBSET; i++){
        printf("%d ", cities[perm.min_permutation[i]-1]->id);
    }

    // Zwalnianie pamięci
    free_memory(cities, &perm, path, used);
    return 0;
}