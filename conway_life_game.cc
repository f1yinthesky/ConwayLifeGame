#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <unistd.h>
#include <vector>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::vector;

double RandDouble() {
  return static_cast<double>(rand()) / RAND_MAX;
}

class Board {
public:
  Board(int width, int height, double p) {
    boards_.resize(2);
    cur_ = 0;
    old_ = 1;
    auto& board = boards_[cur_];
    board.resize(height);
    for (int i = 0; i < board.size(); ++i) {
      auto& row = board[i];
      row.resize(width);
      for (int j = 0; j < row.size(); ++j) {
        auto& cell = row[j];
        if (RandDouble() < p) {
          SetLive(boards_[old_], i, j, &board);
        } else {
          SetDead(i, j, &board);
        }
      }
    }
    boards_[old_] = boards_[cur_];
  }

  ~Board() {}

  void Print() {
    system("clear");
    cout << endl;
    for (const auto& row : boards_[cur_]) {
      for (const auto& cell : row) {
        cout << cell;
      }
      cout << endl;
    }
    cout << endl;
  }

  void EvolveOneGen() {
    old_ = old_ ^ cur_;
    cur_ = old_ ^ cur_;
    old_ = old_ ^ cur_;
    for (int row = 0; row < boards_[cur_].size(); ++row) {
      for (int col = 0; col < boards_[cur_][row].size(); ++col) {
        const int num_live_neighbour =
          GetNumberOfNeighbourLiveCell(boards_[old_], row, col);
        if (IsLiveCell(boards_[old_], row, col)) {
          if (num_live_neighbour == 2 ||
              num_live_neighbour == 3) {
            SetLive(boards_[old_], row, col, &boards_[cur_]);
          } else {
            SetDead(row, col, &boards_[cur_]);
          }
        } else {
          if (num_live_neighbour == 3) {
            SetLive(boards_[old_], row, col, &boards_[cur_]);
          } else {
            SetDead(row, col, &boards_[cur_]);
          }
        }
      }
    }
  }

private:
  void IncreaseValue(const char& key, map<char, int>* a_map) {
    if (a_map->find(key) == a_map->end()) {
      (*a_map)[key] = 1;
    } else {
      (*a_map)[key] += 1;
    }
  }

  void GetTeamToNum(const vector<vector<char>>& board,
                    int row, int col,
                    map<char, int>* team_to_num) {
    team_to_num->clear();
    IncreaseValue(GetCell(board, row - 1, col - 1), team_to_num);
    IncreaseValue(GetCell(board, row - 1, col - 0), team_to_num);
    IncreaseValue(GetCell(board, row - 1, col + 1), team_to_num);
    IncreaseValue(GetCell(board, row - 0, col - 1), team_to_num);
    IncreaseValue(GetCell(board, row - 0, col + 1), team_to_num);
    IncreaseValue(GetCell(board, row - 1, col - 1), team_to_num);
    IncreaseValue(GetCell(board, row - 0, col - 0), team_to_num);
    IncreaseValue(GetCell(board, row + 1, col + 1), team_to_num);
  }

  char SetTeam(const map<char, int>& team_to_num) {
    vector<pair<int, char>> ladder;
    int total_num = 0;
    for (const auto& kv : team_to_num) {
      if (kv.first == ' ') continue;
      total_num += kv.second;
      ladder.emplace_back(total_num, kv.first);
    }

    if (total_num == 0) {
      ladder = {{1, 'O'},
                {2, '-'},
                {3, '|'},
                {4, '^'}};
      total_num = ladder.size();
    }

    int rand_number = RandDouble() * total_num;
    for (const auto& a_pair : ladder) {
      if (rand_number < a_pair.first) return a_pair.second;
    }
    return ladder.back().second;
  }

  void SetLive(const vector<vector<char>>& old_board,
               int row, int col, vector<vector<char>>* board) {
    map<char, int> team_to_num;
    GetTeamToNum(old_board, row, col, &team_to_num);  
    (*board)[row][col] = SetTeam(team_to_num);
  }

  void SetDead(int row, int col, vector<vector<char>>* board) {
    (*board)[row][col] = ' ';
  }

  const char GetCell(const vector<vector<char>>& board,
                     int row,
                     int col) {
    if (row < 0 || row >= board.size()) return ' ';
    if (col < 0 || col >= board[row].size()) return ' ';
    return board[row][col];
  }

  bool IsCellContains(const vector<vector<char>>& board,
                      int row,
                      int col,
                      char cell) {
    return GetCell(board, row, col) == cell;
  }

  // Out range cell consider dead.
  bool IsLiveCell(const vector<vector<char>>& board, int row, int col) {
    return !IsCellContains(board, row, col, ' ');
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

  vector<vector<vector<char>>> boards_;
  int cur_ = 0;
  int old_ = 1;
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
    usleep(100000);
    board.EvolveOneGen();
    board.Print();
    cout << gen << endl;
  }
  return 0;
}
