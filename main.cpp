#include "Solver.hpp"
#include <iostream>

int		main(int argc, char **argv)
{
	Solver	*solver;

	if (argc < 2)
	{
		std::cerr << "./npuzzle filename" << std::endl;
		return 0;
	}
	solver = new Solver(argv[1]);
	solver->solver();
	delete solver;
	return (0);
}
