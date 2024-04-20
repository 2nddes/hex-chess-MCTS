// delete the following definition to use multi threads for MCTS search
#define NO_USING_THREADS
//TODO: implement multi-threading for MCTS search

#include"../inc/BoardState.h"
#include"../inc/MCTreeSearch.h"
#include<thread>
using namespace std;

extern BoardState::color allyColor;

/*
// adjustable parameters:
// MCTS params, in MCTreeSearch.h, class MCTreeSearch
// the size of board, in BoardState.h, class BoardState
*/

int main(void) {
	BoardState board = *(new BoardState());

	cin >> board.round;
	int& n = board.round;
	int x, y;
	for (int i = 0; i < n - 1; i++) {
		cin >> x >> y; 
		if (x != -1) board[x][y] = BoardState::red;
		else allyColor = BoardState::red;

		cin >> x >> y; 
		board[x][y] = BoardState::blue;
	}
	cin >> x >> y;
	if (x != -1) board[x][y] = BoardState::red;
	else allyColor = BoardState::red;

	if(allyColor == BoardState::red && x == -1)
		{cout << "1 2" << endl;return 0;}

	MCTreeSearch mcts = *(new MCTreeSearch());
	mcts.init(&board);

#ifndef NO_USING_THREADS
	thread t[mcts.threadNum];
	for (int i = 0; i < mcts.threadNum; i++) {
		t[i] = thread(&MCTreeSearch::search, &mcts);
	}
	for (int i = 0; i < mcts.threadNum; i++) {
		t[i].join();
	}
#else
	mcts.search();
#endif

	Coordinate action = mcts.getRes();
	
	cout << action.indexX << ' ' << action.indexY << endl;
	return 0;
}