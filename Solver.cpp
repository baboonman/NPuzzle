#include "Solver.hpp"

Solver::Solver(int x, int y)
	: _x(x), _y(y), _totSize(x * y)
{
	this->_solution = new char[9]();
	this->_genSol();
}

Solver::~Solver()
{

}

void	Solver::_genSol(void)
{
	bool	notFinish = true;
	int		maxX = this->_x - 1;
	int		maxY = this->_y - 1;
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
			if (curPos > maxX + minY * this->_x)
			{
				minY++;
				border[NORTH] = false;
				border[EAST] = true;
				curPos = minY * this->_x + maxX;
			}
		}
		else if (border[EAST])
		{
			this->_solution[curPos] = curId++;
			curPos += this->_x;
			if (curPos / this->_x > maxY)
			{
				maxX--;
				border[EAST] = false;
				border[SOUTH] = true;
				curPos = maxY * this->_x + maxX;
			}
		}
		else if (border[SOUTH])
		{
			this->_solution[curPos--] = curId++;
			if (curPos < maxY * this->_x + minX)
			{
				maxY--;
				border[SOUTH] = false;
				border[WEST] = true;
				curPos = maxY * this->_x + minX;
			}
		}
		else if (border[WEST])
		{
			this->_solution[curPos] = curId++;
			curPos -= this->_x;
			if (curPos < minY * this->_x + minX)
			{
				minX++;
				border[WEST] = false;
				border[NORTH] = true;
				curPos = minY * this->_x + minX;
				std::cout << " cur pos: " << curPos;
			}
		}
		if (maxX == minX && maxY == minY)
		{
			notFinish = false;
		}
	}
	for (int y = 0; y < this->_y; ++y)
	{
		for (int x = 0; x < this->_x; ++x)
		{
			std::cout << static_cast<int>(this->_solution[x + y * this->_x]) << " ";
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
		x = pos % this->_x;
		y = pos / this->_y;
		xi = i % this->_x;
		yi = i / this->_y;
		res += std::abs(x - xi) + std::abs(y - yi);
	}
	return (res);
}
