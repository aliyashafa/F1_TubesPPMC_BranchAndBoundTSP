/* EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
* Modul            : Tugas Besar - Travelling Salesmen Problem (TSP)
* Hari dan Tanggal : Senin, 20 Mei 2024
* Nama (NIM)       : Aliya Shafa I. (18321009)
* Asisten (NIM)    : Emmanuella Pramudita Rumanti (13220031)
* Nama File        : main.c
* Deskripsi        : Program yang memecahkan masalah TSP dengan algoritma Branch and Bound
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <ctype.h>

#define MAX_CITIES 100

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

City cities[MAX_CITIES];
int numCities = 0;
double distMatrix[MAX_CITIES][MAX_CITIES];
double bestRoute[MAX_CITIES + 1];
double minDist = DBL_MAX;
int startIndex = -1;

// Function to calculate distance between two cities using Haversine formula
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dlat = (lat2 - lat1) * M_PI / 180.0;
    double dlon = (lon1 - lon2) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double R = 6371.0; // Earth radius in kilometers
    return R * c;
}

// Function to trim leading and trailing whitespace and quotes from a string
char* trimQuotes(char *str) {
    char *end;

    // Trim leading space and quotes
    while (isspace((unsigned char)*str) || *str == '"') str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space and quotes
    end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == '"')) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

// Function to read cities from CSV file
int readCities(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Remove the newline character if it exists
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ",\t");
        if (token != NULL) {
            strcpy(cities[numCities].name, trimQuotes(token));
        }

        token = strtok(NULL, ",\t");
        if (token != NULL) {
            cities[numCities].latitude = atof(trimQuotes(token));
        }

        token = strtok(NULL, ",\t");
        if (token != NULL) {
            cities[numCities].longitude = atof(trimQuotes(token));
        }

        numCities++;
    }
    fclose(file);
    return 1;
}

// Function to print the best route
void printBestRoute() {
    printf("Best route found:\n");
    for (int i = 0; i <= numCities; i++) {
        printf("%s", cities[(int)bestRoute[i]].name);
        if (i < numCities) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.5f km\n", minDist);
}

// Recursive function to solve TSP using branch and bound
void tsp(int level, double currDist, int visited[], double currRoute[]) {
    if (level == numCities) {
        currDist += distMatrix[(int)currRoute[level - 1]][startIndex];
        if (currDist < minDist) {
            minDist = currDist;
            memcpy(bestRoute, currRoute, (numCities + 1) * sizeof(double));
            bestRoute[numCities] = startIndex;
        }
        return;
    }

    for (int i = 0; i < numCities; i++) {
        if (!visited[i]) {
            visited[i] = 1;
            currRoute[level] = i;
            tsp(level + 1, currDist + distMatrix[(int)currRoute[level - 1]][i], visited, currRoute);
            visited[i] = 0;
        }
    }
}

// Main function
int main() {
    char filename[50];
    char startCity[50];

    printf("Enter list of cities file name: ");
    scanf("%s", filename);
    if (!readCities(filename)) {
        return 1;
    }

    printf("Enter starting point: ");
    scanf("%s", startCity);

    // Trim and clean the start city name
    char cleanStartCity[50];
    strcpy(cleanStartCity, trimQuotes(startCity));

    // Find the index of the starting city
    for (int i = 0; i < numCities; i++) {
        if (strcasecmp(cities[i].name, cleanStartCity) == 0) {
            startIndex = i;
            break;
        }
    }

    if (startIndex == -1) {
        printf("Starting city not found in the list.\n");
        return 1;
    }

    // Build distance matrix
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            distMatrix[i][j] = haversine(cities[i].latitude, cities[i].longitude, cities[j].latitude, cities[j].longitude);
        }
    }

    int visited[MAX_CITIES] = {0};
    double currRoute[MAX_CITIES + 1] = {0};
    currRoute[0] = startIndex;
    visited[startIndex] = 1;

    clock_t start = clock();
    tsp(1, 0, visited, currRoute);
    clock_t end = clock();

    printBestRoute();
    double timeElapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.10f s\n", timeElapsed);

    return 0;
}
