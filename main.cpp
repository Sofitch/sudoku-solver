#include "sudoku.h"
#include <chrono>

/* =========================================== */
/* ================== MAIN =================== */
/* =========================================== */

int main(int argc, char* argv[]) {
    /* 9x9 Sudoku visible test - all difficulties */
    Sudoku* puzzle,* copy;
    for (int difficulty = 1; difficulty < 5; difficulty++) {
        std::cout << "-- Difficulty " << difficulty << " --\n";
        
        puzzle = new Sudoku(difficulty, 9);
        copy = new Sudoku(*puzzle);
        SudokuSolver::solve(*copy);

        if (!puzzle->isUnique()) { std::cout << "Puzzle is not unique.\n"; delete puzzle; delete copy; exit(1); }
        if (!SudokuSolver::isSolution(*puzzle, *copy)) { std::cout << "Solution is not correct.\n"; delete puzzle; delete copy; exit(1); }

        std::cout << "PUZZLE (# clues " << puzzle->getNumClues() << "):" << *puzzle;        
        std::cout << "SOLUTION:" << *copy;
        delete puzzle;
        delete copy;
    }

    /* 16x16 Sudoku visible test */
    std::cout << "-- 16x16 Sudoku --\n";
    
    puzzle = new Sudoku(1, 16);
    copy = new Sudoku(*puzzle);
    SudokuSolver::solve(*copy);

    if (!puzzle->isUnique()) { std::cout << "Puzzle is not unique.\n"; delete puzzle; delete copy; exit(1); }
    if (!SudokuSolver::isSolution(*puzzle, *copy)) { std::cout << "Solution is not correct.\n"; delete puzzle; delete copy; exit(1); }

    std::cout << "PUZZLE (# clues " << puzzle->getNumClues() << "):" << *puzzle;        
    std::cout << "SOLUTION:" << *copy;
    delete puzzle;
    delete copy;

    /* 9x9 Sudoku exhaustive test - all difficulties */
    std::chrono::steady_clock sc;
    int avg_numClues;
    double avg_time;

    for (int difficulty = 1; difficulty < 5; difficulty++) {
        avg_numClues = 0;
        avg_time = 0;
        std::cout << "-- Difficulty " << difficulty << " --\n";
        for (int i = 0; i < 200; i++) {
            auto start = sc.now();

            puzzle = new Sudoku(difficulty, 9);
            avg_numClues += puzzle->getNumClues();
            copy = new Sudoku(*puzzle);            
            SudokuSolver::solve(*copy);

            if (!puzzle->isUnique()) { std::cout << "Puzzle is not unique.\n"; delete puzzle; delete copy; exit(1); }
            if (!SudokuSolver::isSolution(*puzzle, *copy)) { std::cout << "Solution is not correct.\n"; delete puzzle; delete copy; exit(1); }
            
            delete puzzle;
            delete copy;
            auto end = sc.now();
            avg_time += static_cast<std::chrono::duration<double>>(end - start).count();
        }
        std::cout << "Avg number of clues: " << (double)avg_numClues / (double)200 << "\n";
        std::cout << "Avg time running: " << (double)avg_time / (double)200 << "\n";
    }

    return 0;
}