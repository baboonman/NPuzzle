#include "Solver.hpp"

Solver::Solver(int size, int heuristics, bool greedy)
	: _whichHeuristics(heuristics), _size(size), _totSize(size * size), _greedy(greedy)
{
	if (size < 3)
		throw new SolverException("Minimum grid size is 3");
	this->_generator();
	this->_init();
}

void	Solver::_init(void)
{
	this->_openSetHash.reserve(5000000);
	this->_closeSetHash.reserve(5000000);
	this->_solution = new int[this->_totSize]();
	this->_genSol();
	if (!this->_isSolvable(this->_initialState, this->_solution))
	{
		throw new SolverException("This puzzle has no solution");
	}
	this->_initialState->h = this->getHeuristics(this->_initialState->board);
	this->_initialState->f = this->_initialState->h;
	std::cout << "Puzzle to solve: " << std::endl;
	this->_printState(this->_initialState);
	std::cout << std::endl << "Goal: " << std::endl;
	this->_printBoard(this->_solution);
	std::cout << std::endl;
	srand(time(NULL));
}

void	Solver::_generator(void)
{
	std::vector<int>	val(this->_totSize);
	int					output;

	srand(time(0));
	this->_initialState = new t_state();
	this->_initialState->board = new int[this->_totSize]();
	std::iota(val.begin(), val.end(), 0);
	for (int i = 0; i < this->_totSize; ++i)
	{
		output = rand() % val.size();
		this->_initialState->board[i] = val[output];
		val.erase(val.begin() + output);
	}
}

Solver::Solver(std::string filename, int heuristics, bool greedy)
	: _whichHeuristics(heuristics), _greedy(greedy)
{
	size_t	size = 0;

	this->_initialState = new t_state();
	if (!(this->_initialState->board = parse(filename, &size)))
		throw new SolverException("Unable to parse the file (" + filename + ")");
	this->_size = size;
	this->_totSize = size * size;
	this->_init();
}

Solver::~Solver()
{
	delete this->_solution;
	for (auto s : this->_openSet)
	{
		this->_deleteState(s);
	}
	for (auto s : this->_closeSetHash)
	{
		this->_deleteState(s.second);
	}
}

void	Solver::_genSol(void)
{
	bool	notFinish = true;
	int		maxX = this->_size - 1;
	int		maxY = this->_size - 1;
	int		minX = 0;
	int		minY = 0;
	int		curId = 1;
	bool	border[4] = {true, false, false, false};
	int		curPos = minX;

	while (notFinish)
	{
		if (border[NORTH])
		{
			this->_solution[curPos++] = curId++;
			if (curPos > maxX + minY * this->_size)
			{
				minY++;
				border[NORTH] = false;
				border[EAST] = true;
				curPos = minY * this->_size + maxX;
			}
		}
		else if (border[EAST])
		{
			this->_solution[curPos] = curId++;
			curPos += this->_size;
			if (curPos / this->_size > maxY)
			{
				maxX--;
				border[EAST] = false;
				border[SOUTH] = true;
				curPos = maxY * this->_size + maxX;
			}
		}
		else if (border[SOUTH])
		{
			this->_solution[curPos--] = curId++;
			if (curPos < maxY * this->_size + minX)
			{
				maxY--;
				border[SOUTH] = false;
				border[WEST] = true;
				curPos = maxY * this->_size + minX;
			}
		}
		else if (border[WEST])
		{
			this->_solution[curPos] = curId++;
			curPos -= this->_size;
			if (curPos < minY * this->_size + minX)
			{
				minX++;
				border[WEST] = false;
				border[NORTH] = true;
				curPos = minY * this->_size + minX;
			}
		}
		if (maxX == minX && maxY == minY)
		{
			notFinish = false;
		}
	}
}

int		Solver::getHeuristics(int *state)
{
	if (this->_whichHeuristics == HAMMING)
	{
		return this->hamming(state);
	}
	if (this->_whichHeuristics == MANHATTAN)
	{
		return this->manhattan(state);
	}
	if (this->_whichHeuristics == MISPLACEDTILES)
	{
		return this->misplacedTiles(state);
	}
	if (this->_whichHeuristics == LNMANHATTAN)
	{
		return this->lnManhattan(state);
	}
	if (this->_whichHeuristics == CRAZYMAN)
	{
		return this->crazyMan(state);
	}
	return (-1);
}

int		Solver::_findTile(int *board, int tile)
{
	for (int i = 0 ; i < this->_totSize ; i++)
	{
		if (board[i] == tile)
			return (i);
	}
	return (-1);
}

int		Solver::_findTile(int id)
{
	for (int i = 0; i < this->_totSize; ++i)
	{
		if (this->_solution[i] == id)
			return (i);
	}
	return (-1);
}

int		Solver::hamming(int *board)
{
	int		res = 0;

	for (int i = 0; i < this->_totSize; ++i)
	{
		if (board[i] ^ this->_solution[i])
			res++;
	}
	return (res);
}

int		Solver::manhattan(int *board)
{
	int		pos;
	int		x;
	int		y;
	int		xi;
	int		yi;
	int		res = 0;

	for (int i = 0; i < this->_totSize; ++i)
	{
		pos = this->_findTile(board[i]);
		x = pos % this->_size;
		y = pos / this->_size;
		xi = i % this->_size;
		yi = i / this->_size;
		res += std::abs(x - xi) + std::abs(y - yi);
	}
	return (res);
}

int		Solver::lnManhattan(int *board)
{
	int		xi, yi, xj, yj;
	int		xiDest, yiDest, xjDest, yjDest;
	int		iDest, jDest;
	int		cost = 0;
	std::vector<bool> processed(_totSize, false);

	for (int i = 0; i < this->_totSize; ++i)
	{
		if (processed[i])
			continue ;
		xi = i % this->_size;
		yi = i / this->_size;
		iDest = this->_findTile(board[i]);
		xiDest = iDest % this->_size;
		yiDest = iDest / this->_size;
		for (int j = 0; j < this->_totSize; ++j)
		{
			if (j == i)
				continue ;
			jDest = this->_findTile(board[j]);
			xjDest = jDest % this->_size;
			yjDest = jDest / this->_size;
			xj = j % this->_size;
			yj = j / this->_size;
			if (xj == xi && xjDest == xiDest && xj == xjDest
					&& yi > yj && yiDest < yjDest)
			{
				cost += 2;
				processed[j] = true;;
			}
		}
	}
	return (cost + this->manhattan(board));
}

int		Solver::misplacedTiles(int *board)
{
	int	row	= 0;
	int	col	= 0;
	int	pos;

	for (int i = 0 ; i < this->_totSize ; i++)
	{
		pos = this->_findTile(board[i]);
		if ( (pos / this->_size) != (i / this->_size) )
			row++;
		if ( (pos % this->_size) != (i % this->_size) )
			col++;
	}
	return col + row;
}

int		Solver::crazyMan(int *board)
{
	int		res = 0;

	for (int i = 0; i < this->_totSize; ++i)
	{
		if (board[i] ^ this->_solution[i])
			res++;
	}
	if (!res) 
		res = rand() % 9 + 1;
	return (res);
}

int								Solver::_getBlankPos(t_state *current)
{
	for (int i = 0 ; i < this->_totSize ; i++)
	{
		if (current->board[i] == 0)
		{
			return (i);
		}
	}
	return (-1);
}

bool							Solver::_board_cmp(t_state *s1, t_state * s2)
{
	for (int i = 0 ; i < this->_totSize ; i++)
	{
		if (s1->board[i] != s2->board[i])
			return (true);
	}
	return false;
}

t_state*						Solver::_swapTile(int pos, int npos, t_state *state, t_state *last)
{
	t_state						*s;

	s = new t_state;
	s->board = new int[this->_totSize];
	std::memcpy(s->board, state->board, this->_totSize * sizeof(int));

	s->board[pos] = state->board[npos];
	s->board[npos] = state->board[pos];

	if (!this->_board_cmp(s, last)) {
		this->_deleteState(s);
		return NULL;
	}

	s->g = state->g + 1;
	s->h = this->getHeuristics(s->board);
	if (this->_greedy)
		s->f = s->h;
	else
		s->f = s->g + s->h;
	s->predecessor = state;

	return s;
}

std::vector<t_state *>			*Solver::_getNeighbours(t_state *current, t_state *last)
{
	 int						p, np;
	 std::vector<t_state *>		*neighbours;

	neighbours = new std::vector<t_state *>;
	t_state*	s; 

	 p = this->_getBlankPos(current);

	if (p % this->_size != 0)
	{
		np = p - 1;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours->push_back(s);
	}
	if ((p + 1) % this->_size != 0)
	{
		np = p + 1;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours->push_back(s);
	}
	if ((p - this->_size) >= 0)
	{
		np = p - this->_size;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours->push_back(s);
	}
	if ((p + this->_size) < this->_totSize)
	{
		np = p + this->_size;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours->push_back(s);
	}
	return neighbours;
}

void					Solver::_printBoard(int *board)
{
	for (int i = 0 ; i < this->_totSize ; i++)
	{
		if (i != 0 && i % this->_size == 0)
			std::cout << std::endl;
		std::cout << std::setw(5) << static_cast<int>(board[i]) << " ";
	}
	std::cout << std::endl;
}

void					Solver::_printState(t_state *s)
{
	std::cout << "Cost: " << s->g << ", Heuristic: " << s->h << std::endl;
	this->_printBoard(s->board);
}

void				Solver::_printInfo(t_state *s)
{
	char				resp;

	std::cout << "Display the solution? (y/n): ";
	std::cin >> resp;
	if (resp == 'y')
	{
		this->_printPred(s);
	}
	std::cout << "Total move: " << s->g << std::endl;
	std::cout << "Total number of states ever selected in the \"opened\" set: " << this->_totalOpenedStateEver << std::endl;
	std::cout << "Maximum number of states ever represented in memory at the same time during the search: " << this->_maxStateInMemory << std::endl;
}

void					Solver::_printPred(t_state *finalState)
{
	t_state				*c;

	c = finalState;
	while (c != nullptr)
	{
		this->_printState(c);
		c = c->predecessor;
	}
}

std::string				Solver::_getHash(t_state *state)
{
	std::string	res;
	for (int i = 0; i < this->_totSize; i++)
	{
		res += std::to_string(state->board[i]);
		res += " ";
	}
	return res;
}

void					Solver::_insertInClose(t_state *state, const std::string &hash)
{
	this->_closeSetHash[hash] = state;
}

void					Solver::_deleteState(t_state *state)
{
	delete state->board;
	delete state;
}

void					Solver::solver()
{
	bool						success = false;

	std::vector<t_state *>		*neighbours;
	t_state *					last;
	t_state	*					current;
	std::string					neighbourHash;
	std::string					currentHash;
	struct timeval				tStart;
	struct timeval				tEnd;
	
	gettimeofday(&tStart, NULL);
	this->_openSet.insert(this->_initialState);
	last = this->_initialState;

	while (!this->_openSet.empty() && !success)
	{
		auto it = this->_openSet.begin();
		current = *it;
		currentHash = this->_getHash(current);
		this->_openSet.erase(it);
		this->_openSetHash.erase(currentHash);
		this->_totalOpenedStateEver++;

		if (!this->hamming(current->board))
		{
			success = true;
			std::cout << "Puzzle solved biatch" << std::endl;
		}
		else
		{
			this->_insertInClose(current, currentHash);

			neighbours = this->_getNeighbours(current, last);
			for (auto n : *neighbours)
			{
				neighbourHash = this->_getHash(n);
				auto openHashIt = this->_openSetHash.find(neighbourHash);
				auto closeIt = this->_closeSetHash.find(neighbourHash);
				bool isInOpen = (openHashIt != this->_openSetHash.end());
				bool isInClose = (closeIt != this->_closeSetHash.end());

				if (!isInOpen && !isInClose)
				{
					auto openIt = this->_openSet.insert(n).first;
					this->_openSetHash[neighbourHash] = openIt;
				}
				else
				{
					if (isInOpen && n->f < (*(openHashIt->second))->f)
					{
						auto openIt	= openHashIt->second;
						this->_deleteState(*openIt);
						this->_openSet.erase(openIt);
						openIt = this->_openSet.insert(n).first;
						this->_openSetHash[neighbourHash] = openIt;
					}
					else if (isInClose && n->f < closeIt->second->f)
					{
						this->_deleteState(closeIt->second);
						this->_closeSetHash.erase(closeIt);
						auto openIt = this->_openSet.insert(n).first;
						this->_openSetHash[neighbourHash] = openIt;
					}
					else
					{
						this->_deleteState(n);
					}
				}
			}
			delete neighbours;
		}
		this->_maxStateInMemory = std::max<int>(this->_openSet.size() + this->_closeSetHash.size(), this->_maxStateInMemory);
		last = current;
	}
	gettimeofday(&tEnd, NULL);
	int64_t totTime = ((static_cast<int64_t>(tEnd.tv_sec) * static_cast<int64_t>(1'000'000)
								+ static_cast<int64_t>(tEnd.tv_usec))
			- (static_cast<int64_t>(tStart.tv_sec) * static_cast<int64_t>(1'000'000)
								+ static_cast<int64_t>(tStart.tv_usec))
				);
	std::cout << "Total algorithm execution time: " 
		<< (totTime / 1'000'000'000) << "m "
		<< (totTime / 1'000'000) % 60 << "s "
		<< (totTime / 1'000) % 1'000 << "ms "
		<< totTime % 1'000 << "us "
		<< std::endl;
	if (!success)
		std::cout << "Puzzle not solved fucking biatch" << std::endl;
	else
	{
		this->_printInfo(current);
		this->_deleteState(current);
	}
}

int			Solver::_inversions(int *board)
{
	int		inv = 0;

	for (int i = 0 ; i < this->_totSize - 1 ; i++)
	{
		for (int j = i + 1 ; j < this->_totSize ; j++)
		{
			if (board[i] && board[j] && board[i] > board[j])
				inv++;
		}
	}
	return inv;
}

bool		Solver::_isSolvable(t_state *initialState, int *goalState)
{
	int		startInv = this->_inversions(initialState->board);
	int		goalInv = this->_inversions(goalState);

	if (this->_size % 2 == 0)
	{
		startInv += this->_getBlankPos(initialState) / this->_size;
		goalInv +=  this->_findTile(goalState, 0) / this->_size;
	}

	return (startInv % 2 == goalInv % 2);
}
