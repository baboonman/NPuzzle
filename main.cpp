#include "Solver.hpp"
#include <iostream>

static int		getHeuristicsChoice(const std::string arg)
{
	if (arg == "manhattan")
		return MANHATTAN;
	if (arg == "hamming")
		return HAMMING;
	return (-1);
	std::cout << "No heuristics function" << std::endl;
}

int		main(int argc, char **argv)
{
	Solver	*solver;
	int		heuristics;

	if (argc < 3 || (heuristics = getHeuristicsChoice(argv[2])) < 0)
	{
		std::cerr << "./npuzzle filename [manhattan|hamming]" << std::endl;
		return 0;
	}
	try {
		solver = new Solver(argv[1], heuristics);
	} catch (std::exception *e) {
		return (1);
	}
	solver->solver();
	delete solver;
	return (0);
}
