#include "D:\project\C++Project\cpp\inc\MCTreeSearch.h"

vector<vector<int>> directions = {
    {-1, 0}, // 上
    {1, 0}, // 下
    {0, -1}, // 左
    {0, 1}, // 右
    {-1, 1}, // 右上
    {1, -1} // 左下
};

BoardState::color allyColor = BoardState::blue;

mutex mtx;

void MCTreeSearch::init(BoardState* board) {
	root = new MCTreeNode();
	root->init(nullptr, board);
}

Coordinate MCTreeSearch::getRes() const{
	MCTreeNode* bestNode = nullptr;
	double bestScore = -DBL_MAX;
	double ucb = 0.0;

	assert(root != nullptr);
	assert(root->children.size() > 0);
	assert(root->numVisits > 0);

	double log_root_numVisits = log(root->numVisits);
	for (const auto& child : root->children) {
		assert(child->numVisits > 0);

		ucb = child->score / (double)child->numVisits 
			+ 1.41 * sqrt(log_root_numVisits / child->numVisits);
		if (ucb > bestScore) {
			bestScore = ucb;
			bestNode = child;
		}
	}

	return bestNode->doneAction;
}


void MCTreeSearch::search() {
	for (int i = 0; i < searchLimitCount; i++) {
		MCTreeNode* node = select(root);
		MCTreeNode* expandedNode = expand(node);
		if(expandedNode->isEndState){
			double result = getScore(expandedNode->board, expandedNode->isEndState);
			backpropagate(expandedNode, result);
			continue;
		}
		double result = rollout(expandedNode->board, expandedNode->player);
		backpropagate(expandedNode, result);
	}
}

MCTreeNode* MCTreeSearch::select(MCTreeNode* node) {
	assert(node != nullptr);
	assert(epsilon >= 0 && epsilon <= 1);
	if(rand() % 100 < epsilon * 100) {//explore, randomly select a child node
		assert(node->isEndState == 0);
		if(!node->isFullyExpanded) return node;
		return *next(node->children.begin(), rand() % node->children.size());
	}
	else{//exploit, select the best child node
		mtx.lock();
		while (!node->isEndState && node->isFullyExpanded) {
			node = getBestChild(node);
		}
		mtx.unlock();
	}
	return node;
}

MCTreeNode* MCTreeSearch::getBestChild(MCTreeNode* node) {
	assert(node != nullptr);
	MCTreeNode* bestNode = nullptr;
	double bestUCB = -DBL_MAX;

	for (const auto& child : node->children) {
		double ucb = child->getUCB();
		if (ucb > bestUCB) {
			bestUCB = ucb;
			bestNode = child;
		}
	}
	assert(bestNode != nullptr);
	return bestNode;
}

MCTreeNode* MCTreeSearch::expand(MCTreeNode* node) {
	if(node->depth >= searchDepth) return node;//limit the depth of the search tree
	if (node->isFullyExpanded || node->isEndState) return node;

	actions& acts = node->UseableActions;
	assert(acts.size() > 0);
	for (auto& act : acts.useableAction) {

		BoardState* newBoard = new BoardState(*node->board);
		newBoard->addCoordinate(act, node->player);
		MCTreeNode* newNode = new MCTreeNode();

		newNode->init(node, newBoard);
		newNode->doneAction = act;
		newNode->isEndState = isEndState(newBoard, allyColor);
		newNode->player = doAction(node->player);
		newNode->depth = node->depth + 1;

		node->children.insert(newNode);
		
		acts.useableAction.erase(act);

		node->isFullyExpanded = (acts.size() == 0);
		return newNode;
	}
	return node;
}

double MCTreeSearch::rollout(BoardState* board, BoardState::color player) {
	BoardState* tempBoard = new BoardState(*board);
	BoardState::color tempPlayer = player;
	int depth = 0;
	int isWin = isEndState(tempBoard, allyColor);
	while (!isWin) {
		//limit the depth of the rollout
		//when didnt reach the end state, use the score to evaluate the board
		if (depth++ > maxRolloutDepth) return evaluate(tempBoard);

		int x = rand() % SIZE;//TODO: change to random action
		int y = rand() % SIZE;//such as select the surrounding empty cell
		if (tempBoard->getCoordinate(x, y) == BoardState::empty) {
			tempBoard->addCoordinate(x, y, tempPlayer);
			tempPlayer = doAction(tempPlayer);
		}
		isWin = isEndState(tempBoard, allyColor);
	}
	//when reach the end state, return the score of the board
	return getScore(tempBoard, isWin);
}

void MCTreeSearch::backpropagate(MCTreeNode* node, double result) {
	while (node != nullptr) {
		node->numVisits++;
		node->score += result;
		node = node->parent;
	}
}

//TODO: evaluate the board when the rollout didnt reach the end state
double MCTreeSearch::evaluate(BoardState* board) {
	return 0.5;
}

BoardState::color doAction(BoardState::color player) {
	return player == BoardState::color::red ? BoardState::color::blue : BoardState::color::red;
}

//TODO: check
int isEndState(BoardState* oneBoardState, BoardState::color player) {
    // 获取棋盘大小
    int size = oneBoardState->boardSize;

    // 创建一个标记数组来跟踪已访问的单元格
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

    // 对于红色玩家，我们检查从顶部到底部的路径
    // 对于蓝色玩家，我们检查从左到右的路径
    if (player == BoardState::color::red) {
        for (int i = 0; i < size; i++) {
            if ((*oneBoardState)[i][0] == player && dfs(oneBoardState, i, 0, visited, player)) {
                return allyColor == player ? 1 : -1;
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            if ((*oneBoardState)[0][i] == player && dfs(oneBoardState, 0, i, visited, player)) {
                return allyColor == player ? 1 : -1;
            }
        }
    }

    // 如果没有找到路径，那么游戏还没有结束
    return 0;
}

bool dfs(BoardState* oneBoardState, int x, int y, std::vector<std::vector<bool>>& visited, BoardState::color player) {
    // 检查是否到达了边界
    if (player == BoardState::color::red && y == oneBoardState->boardSize - 1) {
        return true;
    }
    if (player == BoardState::color::blue && x == oneBoardState->boardSize - 1) {
        return true;
    }

    // 标记当前单元格为已访问
    visited[x][y] = true;

    // 检查所有相邻的单元格
    for (auto& dir : directions) {
        int newX = x + dir[0];
        int newY = y + dir[1];
        if (isValid(newX, newY, oneBoardState->boardSize) && !visited[newX][newY] && (*oneBoardState)[newX][newY] == player) {
            if (dfs(oneBoardState, newX, newY, visited, player)) {
                return true;
            }
        }
    }

    return false;
}

bool isValid(int x, int y, int size) {
    return x >= 0 && y >= 0 && x < size && y < size;
}

//TODO: calculate the score of the board when the rollout reach the end state
double getScore(BoardState* board, int isWin)
{
	return (double)isWin * 100;
}

void actions::init(BoardState* board) {
	for (int i = 0; i < SIZE; i++) 
		for (int j = 0; j < SIZE; j++) 
			if (board->getCoordinate(i, j) == BoardState::empty)
				useableAction.insert(Coordinate(i, j));
}

int actions::size() const {
	return (int)useableAction.size();
}

void MCTreeNode::init(MCTreeNode* parent, BoardState* board) {
	this->parent = parent;
	this->board = board;

	if (parent != nullptr)
		this->player = doAction(parent->player);
	else 
		this->player = BoardState::color::red;

	UseableActions.init(board);
}

double MCTreeNode::getUCB() {
	double ucb = 0.0;
	if (numVisits == 0) return DBL_MAX;
	else return score / (double)numVisits + 1.41 * sqrt(log(parent->numVisits) / numVisits);
}
