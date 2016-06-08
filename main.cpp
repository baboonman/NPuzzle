#include "Solver.hpp"
#include "SolverException.hpp"
#include <iostream>
# define FILENAME 1
# define SIZE 2

static int		getHeuristicsChoice(const std::string arg)
{
	if (arg == "manhattan")
		return MANHATTAN;
	if (arg == "hamming")
		return HAMMING;
	if (arg == "misplacedTiles")
		return MISPLACEDTILES;
	return (-1);
	std::cout << "No heuristics function" << std::endl;
}

static int		getType(const std::string arg)
{
	if (arg == "-f")
		return FILENAME;
	if (arg == "-s")
		return SIZE;
	return (0);
}

int		main(int argc, char **argv)
{
	Solver	*solver;
	int		heuristics;
	int		type;

	if (argc < 4
			|| !(type = getType(argv[1]))
			|| (heuristics = getHeuristicsChoice(argv[3])) < 0)
	{
		std::cerr << "./npuzzle [-f filename|-s size] [manhattan|hamming|misplacedTiles]" << std::endl;
		return 0;
	}
	try {
		if (type == FILENAME)
			solver = new Solver(argv[2], heuristics);
		if (type == SIZE)
			solver = new Solver(std::atoi(argv[2]), heuristics);
	} catch (SolverException *e) {
		std::cout << e->what() << std::endl;
		return (1);
	}
	solver->solver();
	delete solver;
	return (0);
}
