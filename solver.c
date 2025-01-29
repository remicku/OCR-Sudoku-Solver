#include "solver.h"
#include <stdio.h>
#include <stdlib.h>

#define SIZE 9

// Fonction pour valider la grille
int isValid(SudokuGrid *grid, int row, int col, int num) {
    // Vérifie si le chiffre est présent dans la ligne ou la colonne
    for (int i = 0; i < SIZE; i++) {
        if (grid->cells[row][i].value == num || grid->cells[i][col].value == num) {
            return 0;
        }
    }

    // Vérifie si le chiffre est présent dans le bloc 3x3
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid->cells[startRow + i][startCol + j].value == num) {
                return 0;
            }
        }
    }

    return 1; // Le chiffre est valide
}

// Fonction de résolution avec l'algorithme de backtracking
int solve(SudokuGrid *grid) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            // Si la case est vide, essaye les chiffres de 1 à 9
            if (grid->cells[row][col].value == 0) {
                for (int num = 1; num <= SIZE; num++) {
                    if (isValid(grid, row, col, num)) {
                        grid->cells[row][col].value = num;

                        if (solve(grid)) {
                            return 1; // Solution trouvée
                        }

                        grid->cells[row][col].value = 0; // Backtrack
                    }
                }

                return 0; // Aucune solution possible à partir de cette configuration
            }
        }
    }

    return 1; // La grille est résolue
}
/*
// Fonction principale
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Lecture du fichier
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    SudokuGrid grid;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char c;
            fscanf(file, " %c", &c);
            if (c == '.') {
                grid.cells[i][j].value = 0; // Case vide
            } else {
                grid.cells[i][j].value = c - '0'; // Convertit le caractère en entier
                grid.cells[i][j].fixed = 1; // Marque les chiffres fixes
            }
        }
        fscanf(file, "\n"); // Ajout du saut de ligne
    }

    fclose(file);

    // Résolution de la grille
    if (solve(&grid)) {

        // Écriture du résultat dans un nouveau fichier
        char outputFileName[50];
        snprintf(outputFileName, sizeof(outputFileName), "%s.result", argv[1]);
        FILE *outputFile = fopen(outputFileName, "w");
        if (!outputFile) {
            perror("Error creating output file");
            return 1;
        }

        for (int i = 0; i < SIZE; i++) {
            if (i % 3 == 0 && i != 0) {
                fprintf(outputFile, "\n");
            }
            for (int j = 0; j < SIZE; j++) {
                if (j % 3 == 0 && j != 0) {
                fprintf(outputFile, " ");
                }

                fprintf(outputFile, "%d", grid.cells[i][j].value);
            }
            if (i != 8) {
                fprintf(outputFile, "\n");
            }
        }

        fclose(outputFile);

    } else {
        printf("\nAucune solution trouvée.\n");
    }

    return 0;
}*/
