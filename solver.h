#ifndef SOLVER_H
#define SOLVER_H

#define SIZE 9

// Structure pour représenter une case de la grille
typedef struct {
    int value;
    int fixed; // pour marquer les chiffres déjà présents dans la grille d'origine
} Cell;

// Structure pour représenter la grille
typedef struct {
    Cell cells[SIZE][SIZE];
} SudokuGrid;

// Prototypes des fonctions
int isValid(SudokuGrid *grid, int row, int col, int num);
int solve(SudokuGrid *grid);

#endif

