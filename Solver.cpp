#include "Solver.hpp"

Solver::Solver(int size, int heuristics)
	: _whichHeuristics(heuristics), _size(size), _totSize(size * size)
{
	if (size < 3)
		throw new SolverException("Minimum grid size is 3");
	this->_generator();
	this->_init();
}

void	Solver::_init(void)
{
	this->_solution = new uint8_t[this->_totSize]();
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

}

void	Solver::_generator(void)
{
	std::vector<int>	val(this->_totSize);
	int					output;

	srand(time(0));
	this->_initialState = new t_state();
	this->_initialState->board = new uint8_t[this->_totSize]();
	std::iota(val.begin(), val.end(), 0);
	for (int i = 0; i < this->_totSize; ++i)
	{
		output = rand() % val.size();
		this->_initialState->board[i] = val[output];
		val.erase(val.begin() + output);
	}
}

Solver::Solver(std::string filename, int heuristics)
	: _whichHeuristics(heuristics)
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

int		Solver::getHeuristics(uint8_t *state)
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
		return this->heuristic3(state);
	}
	return (-1);
}

int		Solver::hamming(uint8_t *board)
{
	int		res = 0;

	for (int i = 0; i < this->_totSize; ++i)
	{
		if (board[i] ^ this->_solution[i])
			res++;
	}
	return (res);
}

int		Solver::_findTile(uint8_t *board, int tile)
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

int		Solver::manhattan(uint8_t *board)
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

int		Solver::heuristic3(uint8_t *board)
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
	s->board = new uint8_t[this->_totSize];
	std::memcpy(s->board, state->board, this->_totSize * sizeof(uint8_t));

	s->board[pos] = state->board[npos];
	s->board[npos] = state->board[pos];

	if (!this->_board_cmp(s, last)) {
		this->_deleteState(s);
		return NULL;
	}

	s->g = state->g + 1;
	s->h = this->getHeuristics(s->board);
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

void					Solver::_printBoard(uint8_t *board)
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
	//	std::cout << "current: " << current->f << std::endl;

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
	if (!success)
		std::cout << "Puzzle not solved fucking biatch" << std::endl;
	else
	{
		this->_printInfo(current);
		this->_deleteState(current);
	}
}

int			Solver::_inversions(uint8_t *board)
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

bool		Solver::_isSolvable(t_state *initialState, uint8_t *goalState)
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
