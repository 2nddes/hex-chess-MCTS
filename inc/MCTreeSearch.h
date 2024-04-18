#ifndef MCTRREESEARCH_H
#define MCTRREESEARCH_H

#include<bits/stdc++.h>
#include<unordered_map>
#include<unordered_set>
#include"BoardState.h"

int    isEndState(BoardState* board, BoardState::color player);
bool   dfs(BoardState* oneBoardState, int x, int y, std::vector<std::vector<bool>>& visited, BoardState::color player);
bool   isValid(int x, int y, int size);
double getScore(BoardState* board, int isWin);
BoardState::color doAction(BoardState::color player);

//hash function for Coordinate
struct hash_pair {
	size_t operator()(Coordinate const& coordinate) const {
		size_t h1 = std::hash<int>()(coordinate.indexX);
		size_t h2 = std::hash<int>()(coordinate.indexY);
		return h1 ^ h2; // or use boost::hash_combine
	}
};

//actions class, store all the useable actions and isActionDone
struct actions {
	unordered_set<Coordinate, hash_pair>       useableAction;

	void init(BoardState* board);
	int  size() const;//return the number of useable actions
};

//MCTreeNode class, store the tree structure and corresponding board state
struct MCTreeNode
{
	//tree structure
	MCTreeNode*      parent = nullptr;
	set<MCTreeNode*> children;

	//board state
	BoardState*       board = nullptr;
	BoardState::color player;//the player who will make next action
	
	//tree node properties
	int        numVisits = 0;
	int        depth = 0;
	double     score = 0.0;
	bool       isFullyExpanded = false;
	int        isEndState = 0;//0: not end state, 1: win, -1: lose
	Coordinate doneAction;
	actions    UseableActions;

	void   init(MCTreeNode* parent, BoardState* board);
	double getUCB();
};

//MCTreeSearch class, store the root node of the tree and implement the MCTS algorithm
class MCTreeSearch
{
public:

	void init(BoardState* board);

	void search();

	Coordinate getRes() const;

	//limit the number of (select, expand, rollout and backpropagate)
	int searchLimitCount = 1000;

	//limit the depth of the search tree
	int searchDepth      = 5;

	//limit the depth of the rollout, when didnt reach the end state, use the score to evaluate the board
	int maxRolloutDepth  = 10;

	//number of threads for parallel search
	int threadNum        = 20;

	//balance the exploration and exploitation, range from 0 to 1, 0 means pure exploitation, 1 means pure exploration
	double epsilon       = 0.5;

private:
	MCTreeNode* root = nullptr;


	MCTreeNode* select(MCTreeNode* node);
	MCTreeNode* getBestChild(MCTreeNode* node);
	MCTreeNode* expand(MCTreeNode* node);
	double      rollout(BoardState* board, BoardState::color player);
	void        backpropagate(MCTreeNode* node, double result);

	double      evaluate(BoardState* board);
};

#endif // !MCTRREESEARCH_H