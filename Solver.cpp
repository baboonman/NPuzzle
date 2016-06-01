#include "Solver.hpp"

Solver::Solver(int size)
	: _size(size), _totSize(size * size)
{
	this->_init();
}

void	Solver::_init(void)
{
	this->_solution = new char[this->_totSize]();
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
	this->_init();
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

int		Solver::hamming(char *state)
{
	int		res = 0;

	for (int i = 0; i < this->_totSize; ++i)
	{
		res += state[i] ^ this->_solution[i];
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

int		Solver::manhattan(char *state)
{
	int		pos;
	int		x;
	int		y;
	int		xi;
	int		yi;
	int		res = 0;

	for (int i = 0; i < this->_totSize; ++i)
	{
		pos = this->_findTile(state[i]);
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
			return (i);
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
	std::memcpy(s->board, state->board, this->_totSize * sizeof(char));

	s->board[pos] = state->board[npos];
	s->board[npos] = state->board[pos];

	if (!this->_board_cmp(s, last)) {
		std::cout << "already here" << std::endl;
		return NULL;
	}

	s->g = state->g + 1;
	s->h = state->h;
	s->predecessor = state;

	return s;
}

std::vector<t_state *>			Solver::_getNeighbours(t_state *current, t_state *last)
{
	 int						p, np;
	 std::vector<t_state *>		neighbours;

	t_state*	s; 

	 p = this->_getBlankPos(current);

	if (p % this->_size != 0)
	{
		np = p - 1;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	if ((p + 1) % this->_size != 0)
	{
		np = p + 1;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	if ((p - this->_size) >= 0)
	{
		np = p - this->_size;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	if ((p + this->_size) < this->_totSize)
	{
		np = p + this->_size;
		s = this->_swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	return neighbours;
}

void					Solver::solver()
{
	/*
	bool						success = false;

	std::vector<t_state *>		neighbours;
	t_state *					last;
	t_state	*					current;
	
	this->_openSet.insert(this->_initialState);
	last = this->_initialState;

	while (!this->_openSet.empty() && !success)
	{
		auto it = this->_openSet.begin();
		current = *it;
		this->_openSet.erase(it);

		if (current == solution)
			success == true;
		else
		{
			this->_closeSet.push_back(current);

			neighbours = _getNeighbours(current, last);
			for (auto n : neighbours)
			{
				if (!this->_openSet.find(n) || !this->_closeSet.find(n))
				{
					this->_openSet.insert(n);
				}
				else
				{
		//	WIP			
					// if n is in list we call the list elem old_n
					//  n == old_n <=> n->board == old_n->board
					if ( n->g > old_n->g )
						continue ;
					if ((old_n = getOccurenceList(this->_openSet, n) == NULL); // get ptr on list node
						old_n = n;
					//if (n is in this->_openSet)
					//	this->_openSet[x] == n; // replace old ptr with new one
					else if (n is in this->_closeSet)
					{
						this->_closeSet.extract(old_n);
						this->_openSet.insert(n);
					}

				}
		// WIP
			}

			delete neighbours;
		}
		delete current;
	}
	*/
}
