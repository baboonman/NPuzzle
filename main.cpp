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
	try {
		solver = new Solver(argv[1]);
	} catch (std::exception *e) {
		return (1);
	}
	solver->solver();
	delete solver;
	return (0);
}
