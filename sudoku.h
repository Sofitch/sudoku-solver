#ifndef SUDOKU_H
#define SUDOKU_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>

#include "coords.h"

/* =========================================== */
/* ================ AUXILIARY ================ */
/* =========================================== */

template <class T>
void shuffle(std::vector<int>& vec);

int getRandom(int min, int max);

/* =========================================== */
/* ============== SUDOKU CLASS =============== */
/* =========================================== */

class Sudoku {

public:
    /* ================ CELL ================= */

    struct Cell {
        enum Type {clue, draft};

        int _value;
        Type _type;

        Cell(int val);
        Cell(const Cell& cell);
        friend std::ostream& operator<<(std::ostream& out, const Cell& cell);
    };

    /* ======================================= */

private:
    Cell*** _columns;
    int _size;

public:

    /* ============== CONSTRUCTORS =============== */

    Sudoku(int difficulty = 1, int size = 9);

    Sudoku(const Sudoku& sudoku);

    ~Sudoku();

    /* =========== GETTERS AND SETTERS =========== */
    
    int getSize() const;
    
    int getNumClues() const;

    int getCellValue (const Coords& coords) const;

    void setCell(const Coords& coords, int value);

    bool isDraftCell(const Coords& coords) const;

private:
    void makeCellClue(const Coords& coords);

    /* ============= GAME MECHANICS ============== */

public:
    Coords getNextCell (const Coords& coords) const;

    bool outOfBounds(const Coords& coords) const;

    bool isValidValue(const Coords& coords, int value) const;

    std::vector<int> getAllValidValues(const Coords& coords) const;

    /* ============= RANDOMIZE BOARD ============= */

private:
    bool fillBoardRecursive(const Coords& currCell = Coords(0,0));
    
    void makeClues();

    int calculateNumClues(int difficulty) const;

    void makePuzzle(int difficulty);

public:
    bool isUnique() const;

    /* ================== PRINT ================== */

    friend std::string columnSeparator(int sudokuSize);

    friend std::ostream& operator<<(std::ostream& out, const Sudoku& sudoku);
    
};

/* =========================================== */
/* ============== SUDOKU SOLVER ============== */
/* =========================================== */

namespace SudokuSolver {
    bool solveRecursive(Sudoku& puzzle, const Coords& currCell = Coords{0,0});

    void solve(Sudoku& puzzle);

    bool hasMultipleSolutions(Sudoku& puzzle, const Coords& currCell, bool& solutionFound);

    bool isSolution(const Sudoku& puzzle, const Sudoku& solution);
}

#endif
