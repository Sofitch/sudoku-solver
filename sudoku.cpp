#include "sudoku.h"

/* =========================================== */
/* ================ AUXILIARY ================ */
/* =========================================== */

template <class T>
void shuffle(std::vector<T>& vec) {
    static std::mt19937 random{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    std::shuffle(vec.begin(), vec.end(), random);
}

int getRandom(int min, int max) {
    static std::mt19937 random{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    std::uniform_int_distribution die{ min, max };
    return die(random);
}

/* =========================================== */
/* =============== CELL CLASS ================ */
/* =========================================== */

Sudoku::Cell::Cell(int val) : _value(val), _type(Type::draft) {};

Sudoku::Cell::Cell(const Cell& cell) : _value(cell._value), _type(cell._type) {};

std::ostream& operator<<(std::ostream& out, const Sudoku::Cell& cell) {
    if (cell._value == 0) return out << '*';
    return out << cell._value;
}

/* =========================================== */
/* ============== SUDOKU CLASS =============== */
/* =========================================== */

/* ============== CONSTRUCTORS =============== */
/* =========================================== */

Sudoku::Sudoku(int difficulty, int size) : _size(size) {
    int root = (int) sqrt(_size);
    if (root * root != _size) throw std::invalid_argument("Sudoku size must have an integer square root.");
    if (difficulty < 1 || difficulty > 4) throw std::invalid_argument("Difficulty must be between 1 and 5.");

    /* Memory allocation */
    _columns = new Cell**[_size];
    for (int x=0; x < _size; x++) {
        _columns[x] = new Cell*[_size];
        for (int y=0; y < _size; y++)
            _columns[x][y] = new Cell(0);
    }
    
    /* Initialize random puzzle */
    fillBoardRecursive(); // Fills the board with a random valid configuration
    
    makeClues(); // Mark the cells as clues to distinguish from future drafts while solving

    makePuzzle(difficulty); // Make the puzzle by clearing some of the cells
}

Sudoku::Sudoku(const Sudoku& sudoku) : _size(sudoku._size) {
    _columns = new Cell**[_size];
    for (int x=0; x < _size; x++) {
        _columns[x] = new Cell*[_size];
        for (int y=0; y < _size; y++)
            _columns[x][y] = new Cell(*sudoku._columns[x][y]);
    }
}

Sudoku::~Sudoku() {
    for (int x=0; x < _size; x++) {
        for (int y=0; y < _size; y++)
            delete _columns[x][y];
        delete _columns[x];
    }
    delete _columns;
}

/* =========== GETTERS AND SETTERS =========== */
/* =========================================== */

int Sudoku::getSize() const { return _size; }

int Sudoku::getNumClues() const {
    int numClues = 0;
    Cell* cell;
    for (int x=0; x < _size; x++) {
        for (int y=0; y < _size; y++) {
            cell = _columns[x][y];
            if (cell->_type == Cell::Type::clue) numClues++;
        }
    }
    return numClues;
}

int Sudoku::getCellValue (const Coords& coords) const {
    if (outOfBounds(coords)) throw std::invalid_argument("Call to getCellValue with invalid coords.");
    return _columns[coords._x][coords._y]->_value;
}

void Sudoku::setCell(const Coords& coords, int value) {
    if (outOfBounds(coords)) throw std::invalid_argument("Call to setCell with invalid coords.");
    Cell* cell = _columns[coords._x][coords._y];
    if (cell->_type == Cell::Type::draft) // clue cells cannot be altered
        cell->_value = value;
}

bool Sudoku::isDraftCell(const Coords& coords) const {
    if (outOfBounds(coords)) throw std::invalid_argument("Call to isDraftCell with invalid coords.");
    return _columns[coords._x][coords._y]->_type == Cell::Type::draft;
}

void Sudoku::makeCellClue(const Coords& coords) {
    if (outOfBounds(coords)) throw std::invalid_argument("Call to makeCellClue with invalid coords.");
    _columns[coords._x][coords._y]->_type = Cell::Type::clue;
}

/* ============= GAME MECHANICS ============== */
/* =========================================== */

Coords Sudoku::getNextCell (const Coords& coords) const {
    int nextX = (coords._x+1) % _size;
    int nextY = coords._y + (coords._x+1)/_size; // change row if necessary
    return Coords{ nextX, nextY };
}

bool Sudoku::outOfBounds(const Coords& coords) const {
    return coords._y >= _size || coords._x >= _size || coords._y < 0 || coords._x < 0; }

bool Sudoku::isValidValue(const Coords& coords, int value) const {
    if (outOfBounds(coords)) throw std::invalid_argument("Call to isValidValue with invalid coords.");
    int x = coords._x, y = coords._y;
    int boxSize = (int) sqrt(_size);

    if (value < 1 || value > _size) return false; // 0 is not valid
    for (int i=0; i < _size; i++) {
        // check row
        if (_columns[i][y]->_value == value && i != x)
            return false;

        // check column
        if (_columns[x][i]->_value == value && i != y)
            return false;
    }
    // check box
    int boxStart_x = (x/boxSize)*boxSize, boxStart_y = (y/boxSize)*boxSize; // start of corresponding box
    for (int i_x = boxStart_x; i_x < boxSize; i_x++)
        for (int i_y = boxStart_y; i_y < boxSize; i_y++)
            if (_columns[i_x][i_y]->_value == value && !(i_x==x && i_y==y))
                return false;

    return true;
}

std::vector<int> Sudoku::getAllValidValues(const Coords& coords) const {
    if (outOfBounds(coords)) throw std::invalid_argument("Call to getAllValidValues with invalid coords.");
    int x = coords._x, y = coords._y;
    int boxSize = (int) sqrt(_size);

    // initialize vector to allow all values (ex. 1-9)
    std::vector<bool> values;
    for (int i = 0; i <= _size; i++) values.push_back(true);
    values[0] = false; // 0 is only here for padding

    // search for invalid values
    for (int i=0; i < _size; i++) {
        // check row
        values[_columns[i][y]->_value] = false; // mark invalid values with false
        // check column
        values[_columns[x][i]->_value] = false;
    }
    // check box
    int boxStart_x = (x/boxSize)*boxSize, boxStart_y = (y/boxSize)*boxSize; // start of corresponding box
    for (int i_x = boxStart_x; i_x < boxSize; i_x++)
        for (int i_y = boxStart_y; i_y < boxSize; i_y++)
            values[_columns[i_x][i_y]->_value] = false;

    // only leave values that were not found (0 is removed as well)
    std::vector<int> validValues;
    for (int i = 0; i <= _size; i++)
        if (values[i] == true) validValues.push_back(i);

    return validValues;
}


/* ============= RANDOMIZE BOARD ============= */
/* =========================================== */

/* =============== FILL BOARD ================ */

/* Fill valid board randomly */
bool Sudoku::fillBoardRecursive(const Coords& currCell) {
    // Check if reached end of board -> all done
    if (outOfBounds(currCell)) return true;

    // Find all valid values
    std::vector<int> valuePool = getAllValidValues(currCell);
    shuffle(valuePool);
    
    while (!valuePool.empty()) {
        setCell(currCell, valuePool.back());

        // Try building board with this value on current cell
        bool isValidBoard = fillBoardRecursive( getNextCell(currCell) );

        // Cannot build valid board with this value -> undo and try next value
        if (!isValidBoard) {
            setCell(currCell, 0);
            valuePool.pop_back();
        }

        // Valid board found -> all done
        else return true;
    }
    // No possible value -> backtrack and reassign previous values
    return false;
}

/* Marks filled cells as clues */
void Sudoku::makeClues() {
    Cell* cell;
    for (int x=0; x < _size; x++) {
        for (int y=0; y < _size; y++) {
            cell = _columns[x][y];
            if (cell->_value != 0) cell->_type = Cell::Type::clue;
        }
    }
}

/* ============ UNIQUENESS CHECK ============= */

/* Returns true is sudoku has exactly one solution */
bool Sudoku::isUnique() const {
    Sudoku copy = Sudoku(*this); // preserve board - we do not want to solve it
    bool hasSolution = false;
    bool hasMultipleSolutions = SudokuSolver::hasMultipleSolutions(copy, Coords(0,0), hasSolution);
    return hasSolution & !hasMultipleSolutions;
}

/* =============== MAKE PUZZLE =============== */

int Sudoku::calculateNumClues(int difficulty) const {
    if (_size == 9) { // custom values for a common sudoku 9x9 
        switch (difficulty) {
            case 1:     return 46;
            case 2:     return 35;
            case 3:     return 30;
            case 4:     return 20;
            default:    return _size*_size; // should never happen
        }
    }
    else { return _size*_size * 3/5; } // just to test
}

/* Clears N cells in the board according to the difficulty */
void Sudoku::makePuzzle(int difficulty) {
    int numTotalCells = _size*_size;
    int toClear = numTotalCells - calculateNumClues(difficulty); // number of cells to clear

    // shuffled vector of all cells
    std::vector<Coords> allCells;
    for (int x = 0; x < _size; x++)
        for (int y = 0; y < _size; y++) allCells.push_back(Coords(x,y));
    shuffle(allCells);

    int x, y, prevValue;
    while (toClear > 0 && !allCells.empty()) {
        // choose random cell to clear
        Coords curr = allCells.back();
        x = curr._x;
        y = curr._y;
        allCells.pop_back();

        // try clearing this cell
        prevValue = _columns[x][y]->_value;

        _columns[x][y]->_value = 0;
        _columns[x][y]->_type = Cell::Type::draft;

        // check if sudoku remains unique
        if (isUnique()) toClear--;
        else { // cannot clear this cell > restore it
            _columns[x][y]->_value = prevValue;
            _columns[x][y]->_type = Cell::Type::clue;
        }
    }
}


/* ================== PRINT ================== */
/* =========================================== */

std::string columnSeparator(int sudokuSize) {
    std::string s = " ";
    int verticalSlash = sqrt(sudokuSize) - 1, slashSize = 2; // dont count the outside
    int cellsPerRow = sudokuSize, cellSize = (sudokuSize < 10) ? 2 : 3;
    for (int i=0; i< verticalSlash * slashSize + cellsPerRow * cellSize + 1; i++) s += "-"; // add 1 for the padding at the end
    s += " \n";
    return s;
}

std::ostream& operator<<(std::ostream& out, const Sudoku& sudoku) {
    int boxSize = (int) sqrt(sudoku._size);
    out << "\n\n";
    for (int y=0; y < sudoku._size; y++) {
        if (y % boxSize == 0) out << columnSeparator(sudoku._size); // horizontal edge of box
        for (int x=0; x < sudoku._size; x++) {
            Sudoku::Cell* cell = sudoku._columns[x][y];
            if (x % boxSize == 0) out << "| "; // vertical edge of box
            if (sudoku._size >= 10 && cell->_value < 10) out << ' ' << *cell << ' ';
            else out << *cell << ' ';
        }
        out << "|\n";
    }
    out << columnSeparator(sudoku._size);
    out << "\n\n";
    return out;
}
    

/* =========================================== */
/* ============== SUDOKU SOLVER ============== */
/* =========================================== */

/* Solves a sudoku puzzle */
bool SudokuSolver::solveRecursive(Sudoku& puzzle, const Coords& currCell) {
    // Check if reached end of board -> all done
    if (puzzle.outOfBounds(currCell)) return true;

    // Skip clue cells
    if (!puzzle.isDraftCell(currCell)) return solveRecursive( puzzle, puzzle.getNextCell(currCell) );
    
    // Go through all possible digits
    int puzzleSize = puzzle.getSize();
    for (int val = 1; val <= puzzleSize; val++) {
        // Skip invalid values due to sudoku rules
        if (!puzzle.isValidValue(currCell, val)) continue; 

        // Try building board with this value on current cell
        puzzle.setCell(currCell, val);
        bool isValidBoard = solveRecursive( puzzle, puzzle.getNextCell(currCell) );

        // Cannot build valid board with this value -> try next value
        if (!isValidBoard) puzzle.setCell(currCell, 0);

        // Valid board found -> all done
        else return true;
    }

    // No possible value -> backtrack and reassign previous values
    return false;
}

void SudokuSolver::solve(Sudoku& puzzle) { 
    solveRecursive(puzzle, Coords(0,0));
}

/* Similar to solveRecursive, but looks for second solution */
bool SudokuSolver::hasMultipleSolutions(Sudoku& puzzle, const Coords& currCell, bool& solutionFound) {
    // Check if reached end of board -> found new solution
    if (puzzle.outOfBounds(currCell)) {
        if (!solutionFound) { solutionFound = true; return false; } // first solution found - keep searching
        else return true; // sudoku is not unique - end function
    }

    // Skip clue cells
    if (!puzzle.isDraftCell(currCell)) return hasMultipleSolutions( puzzle, puzzle.getNextCell(currCell), solutionFound );

    // Go through all possible digits
    int puzzleSize = puzzle.getSize();
    for (int val = 1; val <= puzzleSize; val++) {
        // Skip invalid values due to sudoku rules
        if (!puzzle.isValidValue(currCell, val)) continue; 

        // Try building board with this value on current cell
        puzzle.setCell(currCell, val);
        bool isValidBoard = hasMultipleSolutions( puzzle, puzzle.getNextCell(currCell), solutionFound );

        // Could not find second solution with this value -> undo and try next value
        if (!isValidBoard) puzzle.setCell(currCell, 0);

        // Second solution found -> sudoku is not unique
        else return true;
    }
    // Could not find second solution -> backtrack and reassign previous values
    return false;
}

/* Checks if a solved sudoku (solution) is the solution to a sudoku puzzle (puzzle) */
bool SudokuSolver::isSolution(const Sudoku& puzzle, const Sudoku& solution) {
    Coords coords{0,0};
    int cellVal;
    int size = puzzle.getSize();
    for (coords._x=0; coords._x < size; coords._x++) {
        for (coords._y=0; coords._y < size; coords._y++) {
            cellVal = solution.getCellValue(coords);

            // check that it is the same puzzle (same fixed cells)
            if (!puzzle.isDraftCell(coords) && puzzle.getCellValue(coords) != cellVal)
                return false;

            // check that there are no invalid values
            if (!solution.isValidValue(coords, cellVal))
                return false;
        }
    }
    return true;
}
