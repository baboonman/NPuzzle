#include "Solver.hpp"

Solver::Solver(int size)
	: _size(size), _totSize(size * size)
{
	this->_init();
}

void	Solver::_init(void)
{
	this->_solution = new uint8_t[this->_totSize]();
	this->_genSol();
}

Solver::Solver(std::string filename)
{
	size_t	size = 0;

	this->_initialState = new t_state();
	if (!(this->_initialState->board = parse(filename, &size)))
	{
		std::cerr << "Unable to parse file" << std::endl;
		throw new std::exception;
	}
	this->_size = size;
	this->_totSize = size * size;
	std::cout << "size: " << size << " totsize: " << this->_totSize << std::endl;
	this->_init();
	if (this->_whichHeuristics == HAMMING)
	{
//		this->_initialState->h = this->hamming(this->_initialState->board);
		this->_initialState->h = this->manhattan(this->_initialState->board);
		this->_initialState->f = this->_initialState->h;
	}
}

Solver::~Solver()
{
	delete this->_solution;
	delete this->_initialState;
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
	for (int y = 0; y < this->_size; ++y)
	{
		for (int x = 0; x < this->_size; ++x)
		{
			std::cout << static_cast<int>(this->_solution[x + y * this->_size]) << " ";
		}
		std::cout << std::endl;
	}
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
		return NULL;
	}

	s->g = state->g + 1;
//	s->h = this->hamming(s->board);
	s->h = this->manhattan(s->board);
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

void					Solver::_printState(t_state *s)
{
	std::cout << "cost: " << s->g << " heuristic: " << s->h << " f: " << s->f << std::endl;
	for (int i = 0 ; i < this->_totSize ; i++)
	{
		if (i != 0 && i % this->_size == 0)
			std::cout << std::endl;
		std::cout << static_cast<int>(s->board[i]) << " ";
	}
	std::cout << std::endl;
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
	std::cout << "Total move: " << finalState->g << std::endl;
}

std::string				Solver::_getHash(t_state *state)
{
	std::string	res;
	for (int i = 0; i < this->_totSize; i++)
	{
		res += state->board[i];
		res += " ";
	}
	return res;
}

void					Solver::_insertInClose(t_state *state, const std::string &hash)
{
	std::pair<const std::string, t_state *>	pair(hash, state);

	this->_closeSetHash.insert(pair);
}

void					Solver::solver()
{
	bool						success = false;

	std::vector<t_state *>		*neighbours;
	t_state *					last;
	t_state	*					current;
	int							i = 0;
	std::string					neighbourHash;
	std::string					currentHash;
	
	this->_openSet.insert(this->_initialState);
	last = this->_initialState;

	while (!this->_openSet.empty() && !success)
	{
		i++;
		auto it = this->_openSet.begin();
		current = *it;
		currentHash = this->_getHash(current);
		this->_openSet.erase(it);
		this->_openSetHash.erase(currentHash);
	//	std::cout << "current: " << current->f << std::endl;

		if (!this->hamming(current->board))
		{
			success = true;
			std::cout << "Puzzle solved biatch" << std::endl;
		}
		else
		{
			this->_insertInClose(current, this->_getHash(current));

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
				else if (isInOpen && isInClose)
					std::cout << "FAIL" << std::endl;
				else
				{
					if (isInOpen && n->f < (*(openHashIt->second))->f)
					{
						auto openIt	= openHashIt->second;
						this->_openSet.erase(openIt);
						openIt = this->_openSet.insert(n).first;
						this->_openSetHash[neighbourHash] = openIt;
					}
					else if (isInClose && n->f < closeIt->second->f)
					{
						this->_closeSetHash.erase(closeIt);
						auto openIt = this->_openSet.insert(n).first;
						this->_openSetHash[neighbourHash] = openIt;
					}
				}
			}
			delete neighbours;
		}
		//delete current;
		last = current;
	}
	if (!success)
		std::cout << "Puzzle not solved fucking biatch" << std::endl;
	else
		this->_printPred(current);
	std::cout << "nb loop turn: " << i << std::endl;
}
