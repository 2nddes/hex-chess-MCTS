#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include<bits/stdc++.h>

using namespace std;

const int SIZE = 11;

struct Coordinate {
	int indexX;
	int indexY;

	Coordinate() : indexX(-1), indexY(-1) {}

	Coordinate(int _indexX, int _indexY) : indexX(_indexX), indexY(_indexY) {}

	Coordinate(const Coordinate& oneCoordinate) : indexX(oneCoordinate.indexX), indexY(oneCoordinate.indexY) {}

	bool operator== (const Coordinate& other) const {
		return this->indexX == other.indexX && this->indexY == other.indexY;
	}

	static bool isValidCoordinate(Coordinate& oneCoordinate) {
		return isValidCoordinate(oneCoordinate.indexX, oneCoordinate.indexY);
	}

	static bool isValidCoordinate(int _indexX, int _indexY) {
		return _indexX >= 0 && _indexX < SIZE && _indexY >= 0 && _indexY < SIZE;
	}

};

class BoardState {
public:
	enum color {
		empty = 0,
		red = 1,
		blue = -1
	};

	int round = 0;
	const int boardSize = 11;
	vector<vector<color>> board;

	BoardState() : board(boardSize, vector<color>(boardSize, empty)) {}
	~BoardState() = default;
	BoardState(const BoardState& oneBoardState) : board(oneBoardState.board), round(oneBoardState.round) {}

	vector<color>& operator[](int indexX) {
		return board[indexX];
	}

	bool addCoordinate(int indexX, int indexY, color colorValue) {
		if (!Coordinate::isValidCoordinate(indexX, indexY) || colorValue != empty) {
			return false;
		}

		this->board[indexX][indexY] = colorValue;
		return true;
	}

	bool addCoordinate(Coordinate coordinate, color colorValue) {
		return addCoordinate(coordinate.indexX, coordinate.indexY, colorValue);
	}

	bool deleteCoordinate(int indexX, int indexY, color colorValue) {
		if (!Coordinate::isValidCoordinate(indexX, indexY) || colorValue != this->board[indexX][indexY]) {
			return false;
		}

		this->board[indexX][indexY] = empty;
		return true;
	}

	bool deleteCoordinate(Coordinate coordinate, color colorValue) {
		return deleteCoordinate(coordinate.indexX, coordinate.indexY, colorValue);
	}

	color getCoordinate(int indexX, int indexY) const {
		if (!Coordinate::isValidCoordinate(indexX, indexY)) {
			return empty;
		}

		return this->board[indexX][indexY];
	}
};


#endif // !BOARDSTATE_H