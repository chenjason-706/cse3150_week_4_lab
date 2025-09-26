#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    // TODO: implement print_board here
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board) {
	vector<pair<int, int>> empty_positions;

    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            if (board[r][c]==0) { 
		    empty_positions.emplace_back(r, c); 
    	    }
	}
    }
    if (empty_positions.empty()) return; // No space to spawn

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> pos_dist(0, empty_positions.size() - 1);
    std::pair<int, int> chosen = empty_positions[pos_dist(gen)];

    std::uniform_int_distribution<> val_dist(1, 10);
    int value = (val_dist(gen) <= 9) ? 2 : 4; // 90% chance for 2

    board[chosen.first][chosen.second] = value;
}


// TODO: Compress a row, remove zeroes, and then pad with zeroes at the end
std::vector<int> compress_row(const std::vector<int>& row) {
	vector<int> compressed;
	copy_if(row.begin(), row.end(), back_inserter(compressed), [](int i){return i!= 0;});
	if (compressed.size() < 4) {
		while (compressed.size() < 4) {
			compressed.push_back(0);
		}
	}
	return compressed;
}

// TODO: Merge a row (assumes the row is already compressed)
vector<int> merge_row(vector<int> row) {
	for (auto it = row.begin(); it != row.end() - 1; it++) {
		if (*it == 0) {
			break;
		}
		auto next_it = it + 1;
		if (*it == *next_it) {
			*it = *it * 2;
			*next_it = 0;
			row = compress_row(row);
		}
	}
	return row;
}

// TODO: Use copy_if and iterators to move tiles left
bool move_left(vector<vector<int>>& board) {
	bool change = false;
	for (auto& row : board) {
		vector<int> original = compress_row(row);
		row = merge_row(row);

		if (row != original) {
			change = true;
		}
		else {
			copy_if(original.begin(), original.end(), row.begin(), [](int x){return true;});
		}
	}
    	return change;
}

// TODO: Use reverse iterators to move tiles right
bool move_right(vector<vector<int>>& board) {
	bool change = false;
	for (auto& row : board) {
		vector<int> original(row);
    		reverse(row.begin(), row.end());
    		row = compress_row(row);
    		row = merge_row(row);
    		row = compress_row(row);

    		if (row != original) {
        		change = true;
        		reverse(row.begin(), row.end());	
    		} 
		else {

        		copy_if(original.rbegin(), original.rend(), row.begin(),[](int x) { return true; });
		}
	}
	return change;
}

// TODO: Use column traversal to move tiles up
bool move_up(vector<vector<int>>& board) {
	bool change = false;

	for (int i = 0; i < 4; i++) {
		vector<int> column;
		
		for (int j = 0; j < 4; j++) {
			column.push_back(board[j][i]);
		}
		vector<int> original = compress_row(column);
                column = merge_row(column);
                if (column != original) {
			change = true;
                        for (int k : column)
				board[k][i] = column[k];
		}
	}
    	return change;
}

// TODO: Use column traversal with reverse to move tiles down
bool move_down(vector<vector<int>>& board) {
	bool change = false;

        for (int i = 0; i < 4 ; i++) {
                vector<int> column;

                for (int j = 0; j < 4; j++) {
                        column.push_back(board[j][i]);
                }
		reverse(column.begin(), column.end());
                vector<int> original =	column;
		column = compress_row(column);
                column = merge_row(column);
                if (column != original) {
                        change = true;
			reverse(column.begin(), column.end());
                        for (int k = 0; k < 4; k++) {
                                board[k][i] = column[k];
			}
		}
        }
        return change;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            // TODO: get the history and print the board and continue
	    if (!history.empty()) {
	    	board = history.top();
	    	history.pop();
	    print_board(board, first);
	    }
	    continue;
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            // TODO: Store the previous state here!
	    history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
