#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>

// TODO remove aux functions from inside class, refacor functions, refactor expressions, delete unsused functions
// TODO remove numbers
// TODO solver

constexpr int g_size = 9;

class Sudoku {
    struct Num {
        int _value;
        int _x;
        int _y;
        int _square;

        Num(int val, int x, int y) : _value(val), _x(x), _y(y), _square((y/3)*3+(x/3)) {};
        friend std::ostream& operator<<(std::ostream& out, const Num& num) {
            return out << num._value;
        }
    };

private:
    Num* _columns[g_size][g_size];

public:
    Sudoku() {
        for (int x=0; x < g_size; x++) {
            for (int y=0; y < g_size; y++) {
                Num* num = new Num(0, x, y);
                _columns[x][y] = num;
            }
        }
        randomizeBoardRecursive();
    }

    ~Sudoku() {
        for (int x=0; x < g_size; x++)
            for (int y=0; y < g_size; y++)
                delete _columns[x][y];
    }

    std::vector<int> generateNumPool() {
        static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
        std::vector<int> numPool = {1, 2, 3, 4, 5, 6, 7, 8, 9}; // FIXME should it be a vector?
        std::shuffle(numPool.begin(), numPool.end(), mt);
        return numPool;
    }

    bool inRow(int row, int value) {
        for (int i=0; i < g_size; i++)
            if (_columns[i][row]->_value == value) return false;
        return true;
    }

    bool inColumn(int column, int value) {
        for (int i=0; i < g_size; i++)
            if (_columns[column][i]->_value == value) return false;
        return true;
    }

    bool inSquare(int square, int value) {
        for (int i=0; i < g_size; i++)
            if (_columns[(square%3)*3+(i%3)][(square/3)*3+(i/3)]->_value == value) return false;
        return true;
    }

    bool isPossibleValue(int x, int y, int value) {
        int square_x = (x/3)*3, square_y = (y/3)*3;
        for (int i=0; i < g_size; i++) {
            // check row
            if (_columns[i][y]->_value == value) return false;
            // check column
            if (_columns[x][i]->_value == value) return false;
            // check square
            if (_columns[square_x+(i%3)][square_y+(i/3)]->_value == value) return false;
        }
        return true;
    }

    std::vector<int> getPossibleValues(int x, int y) {
        std::vector<int> values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // 0 is here for padding
        int square_x = (x/3)*3, square_y = (y/3)*3; // corresponding 3x3 square
        
        // search for invalid numbers
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

    void shuffle(std::vector<int>& vec) {
        static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
        std::shuffle(vec.begin(), vec.end(), mt);
    }

    bool randomizeBoardRecursive(int x = 0, int y = 0) {
        // Find possible values
        std::vector<int> valuePool = getPossibleValues(x, y);
        shuffle(valuePool);
        
        while (!valuePool.empty()) { //FIXME
            // Try value
            int val = valuePool.back();
            _columns[x][y]->_value = val;

            // If this was the last position -> all done
            if (y == 8 && x == 8) return true; 

            // If not, try building board from here
            int nextX = (x+1) % 9, nextY = y + (x+1)/9; // move on to next position, change row if necessary
            bool isValidBoard = randomizeBoardRecursive(nextX, nextY); 

            // Cannot build valid board with this value -> try next valuw
            if (!isValidBoard) {
                _columns[x][y]->_value = 0;
                valuePool.pop_back();
            }

            // Valid board found -> all done
            else return true;
        }

        // No possible value -> backtrack and reassign previous values
        return false;
    }

    static std::string columnSeparator() {
        std::string s = " ";
        for (int i=0; i<(2*g_size+3*2-1); i++) s += "-";
        s += " \n";
        return s;
    }

    friend std::ostream& operator<<(std::ostream& out, const Sudoku& sudoku) {
        out << "\n\n";
        for (int y=0; y < g_size; y++) {
            if (y%3 == 0) out << Sudoku::columnSeparator();
            for (int x=0; x < g_size; x++) {
                Num* num = sudoku._columns[x][y];
                if (x%3 == 0) out << "| ";
                out << *num << ' ';
            }
            out << "|\n";
        }
        out << Sudoku::columnSeparator();
        out << "\n\n";
        return out;
    }
};

int main() {
    Sudoku* sudoku = new Sudoku;
    std::cout << *sudoku;
    return 0;
}