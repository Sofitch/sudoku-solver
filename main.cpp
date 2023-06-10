#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <ctime>
#include <random>

// TODO put type outside cell, join recursive functions (generator can just reset the board, call the solver and then set all to final)
// TODO improve squares
// TODO solver::getAllSolutions, makePuzzle
// (maybe) TODO class boardcreator? operator[] that receives coords on class sudoku?

/* =========================================== */
/* ================ AUXILIARY ================ */
/* =========================================== */

constexpr int g_size = 9;
constexpr std::array<int, g_size+1> g_values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // 0 is here for padding

struct Coords {
    int _x;
    int _y;
    Coords(int x, int y) : _x(x), _y(y) {};
};

void shuffle(std::vector<int>& vec) {
    static std::mt19937 random{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    std::shuffle(vec.begin(), vec.end(), random);
}

static std::string columnSeparator() {
    std::string s = " ";
    for (int i=0; i<(2*g_size+3*2-1); i++) s += "-";
    s += " \n";
    return s;
}

/* =========================================== */
/* ============== SUDOKU CLASS =============== */
/* =========================================== */

class Sudoku {

    /* ================ CELL ================= */

    struct Cell {
        enum Type {final, draft};

        int _value;
        Type _type;

        Cell(int val) : _value(val), _type(Type::draft) {};
        friend std::ostream& operator<<(std::ostream& out, const Cell& cell) {
            return out << cell._value;
        }
    };

    /* ======================================= */

private:
    Cell* _columns[g_size][g_size];

public:

    /* ============== CONSTRUCTORS =============== */

    Sudoku() {
        for (int x=0; x < g_size; x++) {
            for (int y=0; y < g_size; y++) {
                Cell* cell = new Cell(0);
                _columns[x][y] = cell;
            }
        }
        randomizeBoardRecursive();
        makePuzzle();
    }

    Sudoku(const Sudoku& sudoku) {
        Cell* copy;
        for (int x=0; x < g_size; x++)
            for (int y=0; y < g_size; y++) {
                copy = sudoku._columns[x][y];
                Cell* cell = new Cell(copy->_value);
                cell->_type = copy->_type;
                _columns[x][y] = cell;
            }
    }

    ~Sudoku() {
        for (int x=0; x < g_size; x++)
            for (int y=0; y < g_size; y++)
                delete _columns[x][y];
    }

    /* ============== CELL HANDLING ============== */
    
    int getCellValue (const Coords& coords) const {
        return _columns[coords._x][coords._y]->_value;
    }

    static Coords getNextCell (const Coords& coords) {
        int nextX = (coords._x+1) % 9;
        int nextY = coords._y + (coords._x+1)/9; // change row if necessary
        return Coords{ nextX, nextY };
    }

    bool outOfBounds(const Coords& coords) const { return coords._y >= g_size; }

    bool isDraftCell(const Coords& coords) const {
        return _columns[coords._x][coords._y]->_type == Cell::Type::draft;
    }

    void setDraftCell(const Coords& coords, int value) {
        Cell* cell = _columns[coords._x][coords._y];
        if (cell->_type == Cell::Type::draft) // final cells cannot be altered
            cell->_value = value;
    }

    void setFinalCell(const Coords& coords) {
        _columns[coords._x][coords._y]->_type = Cell::Type::final;
    }

    /* ============= RANDOMIZE BOARD ============= */

    bool isPossibleValue(const Coords& coords, int value) const {
        int x = coords._x, y = coords._y;
        int square_x = (x/3)*3;
        int square_y = (y/3)*3;

        if (value < 1 || value > g_size) return false; // 0 is not valid
        for (int i=0; i < g_size; i++) {
            // check row
            if (_columns[i][y]->_value == value && i != x)
                return false;

            // check column
            if (_columns[x][i]->_value == value && i != y)
                return false;

            // check square
            if (_columns[square_x+(i%3)][square_y+(i/3)]->_value == value
                && !(square_x+(i%3)==x && square_y+(i/3)==y))
                return false;
        }
        return true;
    }

    std::vector<int> getPossibleValues(const Coords& coords) const {
        int x = coords._x, y = coords._y;
        std::vector<int> values = {g_values.begin(), g_values.end()}; // 0 is here for padding
        int square_x = (x/3)*3, square_y = (y/3)*3; // corresponding 3x3 square
        
        // search for invalid values
        for (int i=0; i < g_size; i++) {
            // check row
            values[_columns[i][y]->_value] = 0;
            // check column
            values[_columns[x][i]->_value] = 0;
            // check square
            values[_columns[square_x+(i%3)][square_y+(i/3)]->_value] = 0;
        }

        // only leave values that were not found (0 is removed as well)
        for (auto it = values.begin(); it != values.end(); it++)
            if (*it == 0) values.erase(it--);

        return values;
    }

    bool randomizeBoardRecursive(const Coords& currCell = Coords{0,0}) {
        int x = currCell._x, y = currCell._y;

        // Check if reached end of board -> all done
        if (outOfBounds(currCell)) return true;

        // Find possible values
        std::vector<int> valuePool = getPossibleValues(currCell);
        shuffle(valuePool);
        
        while (!valuePool.empty()) {
            int val = valuePool.back();
            setDraftCell(currCell, val);

            // Try building board with this value set
            bool isValidBoard = randomizeBoardRecursive( getNextCell(currCell) );

            // Cannot build valid board with this value -> try next value
            if (!isValidBoard) {
                setDraftCell(currCell, 0);
                valuePool.pop_back();
            }

            // Valid board found -> all done
            else {
                setFinalCell(currCell);
                return true;
            }
        }

        // No possible value -> backtrack and reassign previous values
        return false;
    }

    /* =============== MAKE PUZZLE ================ */

    // TODO remove some values, maybe option to choose difficulty
    void makePuzzle() {
        _columns[0][0]->_value = 0;
        _columns[0][0]->_type = Cell::Type::draft;
        /*for (int x=0; x < g_size; x++) {
            for (int y=0; y < g_size; y++) {
                _columns[x][y] = 0;
            }
        }*/
    }

    /* ================== PRINT ================== */

    friend std::ostream& operator<<(std::ostream& out, const Sudoku& sudoku) {
        out << "\n\n";
        for (int y=0; y < g_size; y++) {
            if (y%3 == 0) out << columnSeparator();
            for (int x=0; x < g_size; x++) {
                Cell* cell = sudoku._columns[x][y];
                if (x%3 == 0) out << "| ";
                out << *cell << ' ';
            }
            out << "|\n";
        }
        out << columnSeparator();
        out << "\n\n";
        return out;
    }
    
};

/* ================= SOLVER ================== */

namespace SudokuSolver {

    /* Similiar to Sudoku::getNextCell, but looks for next cell to fill */
    Coords getNextDraftCell(Sudoku& puzzle, const Coords& coords) {
        Coords nextCoords{coords._x, coords._y};
        
        do {
            nextCoords._y += (nextCoords._x+1)/9;
            nextCoords._x = (nextCoords._x+1) % 9;
            
            if (puzzle.outOfBounds(nextCoords)) return nextCoords; // check if reached end of board
        } while (!puzzle.isDraftCell(nextCoords));
        
        return nextCoords;
    }

    /* Similiar to Sudoku::randomizeBoardRecursive, but only fills draft cells */
    bool solveRecursive(Sudoku& puzzle, const Coords& currCell = Coords{0,0}) {
        int x = currCell._x, y = currCell._y;

        // Check if reached end of board -> all done
        if (puzzle.outOfBounds(currCell)) return true;

        // Find possible values
        std::vector<int> valuePool = puzzle.getPossibleValues(currCell);
        shuffle(valuePool); // probably more efficient on average
        
        while (!valuePool.empty()) {
            int val = valuePool.back();
            puzzle.setDraftCell(currCell, val);

            // Try building board with this value set
            bool isValidBoard = solveRecursive( puzzle, getNextDraftCell(puzzle, currCell) );

            // Cannot build valid board with this value -> try next value
            if (!isValidBoard) {
                puzzle.setDraftCell(currCell, 0);
                valuePool.pop_back();
            }

            // Valid board found -> all done
            else return true;
        }

        // No possible value -> backtrack and reassign previous values
        return false;
    }

    void solve(Sudoku& puzzle) { 
        //puzzle.setDraftCell(Coords{0,0}, 1);
        Coords firstCell = getNextDraftCell(puzzle, Coords{-1, 0}); // find first empty cell
        solveRecursive(puzzle, firstCell);
    }

    bool isSolution(const Sudoku& puzzle, const Sudoku& solution) {
        Coords coords{0,0};
        int cellVal;
        for (coords._x=0; coords._x < g_size; coords._x++) {
            for (coords._y=0; coords._y < g_size; coords._y++) {
                cellVal = solution.getCellValue(coords);

                // check that it is the same puzzle (same fixed cells)
                if (!puzzle.isDraftCell(coords) && puzzle.getCellValue(coords) != cellVal)
                    return false;

                // check that there are no invalid values
                if (!solution.isPossibleValue(coords, cellVal))
                    return false;
            }
        }
        return true;
    }
}

/* =========================================== */
/* ================== MAIN =================== */
/* =========================================== */

int main() {
    Sudoku puzzle = Sudoku();

    Sudoku solved = Sudoku(puzzle); // make a copy to keep the original puzzle
    SudokuSolver::solve(solved);
    std::cout << puzzle;
    std::cout << solved;

    if (SudokuSolver::isSolution(puzzle, solved)) std::cout << "SOLVED!\n";
    else std::cout << "FAILED :(\n";

    return 0;
}