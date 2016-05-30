#ifndef SOLVER_HPP
# define SOLVER_HPP

# include <cstring>
# include <iostream>
# define NORTH 0
# define WEST 1
# define SOUTH 2
# define EAST 3

class	Solver
{
	public:
		Solver(int x, int y);
		virtual ~Solver();
		int		hamming(char *state);
		int		manhattan(char *state);
	private:
		int		_findTile(int id);
		void	_genSol(void);

		char	*_solution;
		int		_x;
		int		_y;
		int		_totSize;
};



#endif
