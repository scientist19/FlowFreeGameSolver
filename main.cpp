#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <string>
#include <cstdlib>
#include <optional>

using namespace std;

typedef vector<vector<int>> Field;
typedef pair<int, int> ii;

const int VISITED = -1e6;
const int WALL = -1e3;
const char WALL_CHAR = '#';

// debug
void printField(const Field& field){
	int m = field[0].size();
	int n = field.size();
	cout << "Field:\n";
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++)
			cout << field[i][j] << " ";
		cout << "\n";
	}
	cout << "\n";
}

void calcBalls(Field& field, const vector<pair<ii, ii>>& balls, vector<int>& balls_count, int& balls_numb, int x, int y){
	int m = field[0].size();
	int n = field.size();
	
	//cout << "calcBalls " << x << " " << y << "\n";
	
	if (x < 0 || x >= n || y < 0 || y >= m)
		return;
		
	//cout << "calcBalls " << x << " " << y << " " << field[x][y] << "\n";
	
	int value = field[x][y];	
	
	if (value < 0)
		return;
		
	if (value == 0){
		field[x][y] = VISITED;
	}	
		
	if (value > 0){
		//cout << "Found BALL\n";
		balls_numb++;
		int color = value-1;
		balls_count[color]++;
		field[x][y] = -value;
		return;
	}
	
	calcBalls(field, balls, balls_count, balls_numb, x+1, y);
	calcBalls(field, balls, balls_count, balls_numb, x-1, y);		
	calcBalls(field, balls, balls_count, balls_numb, x, y+1);		
	calcBalls(field, balls, balls_count, balls_numb, x, y-1);
}


void restoreBalls(Field& field, const vector<pair<ii, ii>>& balls){
	
	for (int i = 0; i < balls.size(); i++){
		{
			auto ball = balls[i].first;
			field[ball.first][ball.second] = i+1;
		}
		{
			auto ball = balls[i].second;
			field[ball.first][ball.second] = i+1;
		}		
	}
}

bool checkBlankCells(const Field& field){
	int m = field[0].size();
	int n = field.size();
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			if (field[i][j] == 0)
				return true;
	return false;
}

bool checkSolution(const Field& field, const vector<pair<ii, ii>>& balls, int color){
	int m = field[0].size();
	int n = field.size();
	int bn = balls.size();
		
	Field field_copy = field;	
	vector<bool> connected(bn);
	vector<bool> disconnected(bn);
		
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++){
			if (field_copy[i][j] == 0){
				vector<int> balls_count(bn);
				int numb = 0;
				calcBalls(field_copy, balls, balls_count, numb, i, j);
				if (numb == 0)
					return false;
				for (int z = 0; z < bn; z++){
					if (balls_count[z] == 2)
						connected[z] = true;
					if (balls_count[z] == 1)
						disconnected[z] = true;
				}
				
				//printField(field_copy);
				
				restoreBalls(field_copy, balls);	
			}
		}
	
	for (int i = color+1; i < bn; i++)
		if (!connected[i] && disconnected[i]){
			//cout << "cehckSolution false because of " << i << "\n";
			return false;	
		}

	
	//cout << "cehckSolution true\n";
	return true;
}

bool solve(Field& field, int color, int x, int y, const vector<pair<ii, ii>>& balls){
	int m = field[0].size();
	int n = field.size();
	int bn = balls.size();
	
	//cout << "Solve " << color << " " << x << " " << y << "\n";

	if (x < 0 || x >= n || y < 0 || y >= m)
		return false;
		
	auto target = balls[color].second;
	if (x == target.first && y == target.second){
		if (color+1 == bn){
			
			if (checkBlankCells(field))
				return false;
			
			cout << "Found path for last!\n";
			return true;
		}
		
		int next_x = balls[color+1].first.first;
		int	next_y = balls[color+1].first.second;
		
		return solve(field, color+1, next_x, next_y, balls);
	}
	
		
	auto is_start_cell = [&](){
		auto start = balls[color].first;
		return (x == start.first && y == start.second);
	};
		
	if (!is_start_cell()){
		if ((field[x][y] != 0))
			return false;	
			
		field[x][y] = (color+1)*(-1);
		
		
		if (!checkSolution(field, balls, color)){
			field[x][y] = 0;
			return false;
		}		
	}

	
	int next_x = balls[color+1].first.first;
	int	next_y = balls[color+1].first.second;
	
	Field field_copy = field;
	if (solve(field_copy, color, x+1, y, balls)){
		field = field_copy;
		return true;
	}

	field_copy = field;
	if (solve(field_copy, color, x, y+1, balls)){
		field = field_copy;
		return true;		
	}

	field_copy = field;		
	if (solve(field_copy, color, x-1, y, balls)){
		field = field_copy;
		return true;		
	}

	field_copy = field;			
	if (solve(field_copy, color, x, y-1, balls)){
		field = field_copy;
		return true;		
	}
		
	if (!is_start_cell())
		field[x][y] = 0;
	return false;
}


int main(){
	
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	
	ofstream fout("output.txt");
	ifstream fin("input.txt");
	
	int n, m, bn;
	fin >> n >> m >> bn;
	
	Field f(n);
	
	//for (int i = 0; i < n; i++){
	//	f[i].resize(m);
	//	for (int j = 0; j < m; j++)
	//		fin >> f[i][j];
	//}
	
	
	for (int i = 0; i < n; i++){
		string s;
		fin >> s;
		
		f[i].resize(m);
		
		for (int j = 0; j < m; j++)
			if (s[j] == WALL_CHAR)
				f[i][j] = WALL;
			else
				f[i][j] = stoi(s.substr(j, 1), nullptr, 16); //s[j] - (int)'0';
	}
	
	printField(f);
	
	vector<int> cnt(bn);
	vector<pair<ii, ii>> balls(bn);
	
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++){
			if (f[i][j] > 0){
				int color = f[i][j] - 1;
				cnt[color]++;
				int c = cnt[color];
				if (c > 2){
					fout << "Balls with color " << color << " are more than 2\n";
					fout.close();
					return 0;
				}
				if (c == 1)
					balls[color].first = {i,j};
				if (c == 2)
					balls[color].second = {i,j};
			}
		}
		
	//for (int i = 0; i < n; i++){
	//	for (int j = 0; j < m; j++)
	//		cout << f[i][j];
	//cout << "\n";
	
	
	//cout << "\n\n";
	
	for (int i = 0; i < bn; i++){
		auto start = balls[i].first;
		auto target = balls[i].second;
		//cout << "(" << start.first << ", " << start.second << ") -> (" << target.first << ", " << target.second << ")\n";
	}
	
		
	int start_x = balls[0].first.first;
	int start_y = balls[0].first.second;
	bool succeed = solve(f, 0, start_x, start_y, balls);
	
	if (succeed){
		fout << "Succeed!\n";
		for (int i = 0; i < n; i++){
			for (int j = 0; j < m; j++)
				if (f[i][j] != WALL)
					fout << abs(f[i][j]);
				else
					fout << WALL_CHAR;
			fout << "\n";
		}
	}
	else{
		fout << "Solution does not exist :(\n";
	}
	
	fout.close();
	
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	std::cout << "Elapsed time = " 
			  << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 << "[s]" << std::endl;
}

