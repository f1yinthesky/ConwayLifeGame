#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

double RandDouble() {
  return static_cast<double>(rand()) / RAND_MAX;
}

class Board {
public:
  Board(int width, int height, double p) {
    board_.resize(height);
    for (int i = 0; i < board_.size(); ++i) {
      auto& row = board_[i];
      row.resize(width);
      for (int j = 0; j < row.size(); ++j) {
        auto& cell = row[j];
        if (RandDouble() < p) {
          SetLive(i, j, &board_);
        } else {
          SetDead(i, j, &board_);
        }
      }
    }
  }

  ~Board() {}

  void Print() {
    system("clear");
    for (const auto& row : board_) {
      for (const auto& cell : row) {
        cout << cell;
      }
      cout << endl;
    }
    cout << endl;
  }

  void EvolveOneGen() {
    vector<vector<char>> new_board(board_);
    for (int row = 0; row < new_board.size(); ++row) {
      for (int col = 0; col < new_board[row].size(); ++col) {
        const int num_live_neighbour =
          GetNumberOfNeighbourLiveCell(board_, row, col);
        if (IsLiveCell(board_, row, col)) {
          if (num_live_neighbour == 2 ||
              num_live_neighbour == 3 ||
              num_live_neighbour == 3) {
            SetLive(row, col, &new_board);
          } else {
            if (RandDouble() < 0.95) {
              SetDead(row, col, &new_board);
            }
          }
        } else {
          if (num_live_neighbour == 3 ||
              num_live_neighbour == 3) {
            SetLive(row, col, &new_board);
          } else {
            SetDead(row, col, &new_board);
          }
        }
      }
    }
    board_ = new_board;
  }

private:
  void SetLive(int row, int col, vector<vector<char>>* board) {
    (*board)[row][col] = '*';
  }

  void SetDead(int row, int col, vector<vector<char>>* board) {
    (*board)[row][col] = ' ';
  }

  // Out range cell consider dead.
  bool IsLiveCell(const vector<vector<char>>& board, int row, int col) {
    if (row < 0 || row >= board.size()) return true;
    if (col < 0 || col >= board[row].size()) return true;
    return board[row][col] == '*';
  }

  int GetNumberOfNeighbourLiveCell(const vector<vector<char>>& board, int row, int col) {
    int result = 0;
    result += IsLiveCell(board, row - 1, col - 1) ? 1 : 0;
    result += IsLiveCell(board, row - 1, col - 0) ? 1 : 0;
    result += IsLiveCell(board, row - 1, col + 1) ? 1 : 0;
    result += IsLiveCell(board, row - 0, col - 1) ? 1 : 0;
    result += IsLiveCell(board, row - 0, col + 1) ? 1 : 0;
    result += IsLiveCell(board, row + 1, col - 1) ? 1 : 0;
    result += IsLiveCell(board, row + 1, col - 0) ? 1 : 0;
    result += IsLiveCell(board, row + 1, col + 1) ? 1 : 0;
    return result;
  }

  vector<vector<char>> board_;
};

int main() {
  srand(time(nullptr));
  cout << "Hello World!" << endl;
  const int width = 100;
  const int height = 30;
  const double p = 0.3;
  Board board(width, height, p);
  board.Print();
  for (int gen = 0; gen < 10000; ++gen) {
    usleep(50000);
    board.EvolveOneGen();
    board.Print();
    cout << gen << endl;
  }
  return 0;
}
