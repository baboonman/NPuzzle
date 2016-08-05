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
	if (arg == "lnManhattan")
		return LNMANHATTAN;
	if (arg == "crazyMan")
		return CRAZYMAN;
	std::cerr << "Heuristics " << arg << " does not exist" << std::endl;
	return (-1);
}

static int		getType(const std::string arg)
{
	if (arg == "-f")
		return FILENAME;
	if (arg == "-s")
		return SIZE;
	return (0);
}

static bool		getGreedy(int argc, const char **arg)
{
	if (argc < 5)
		return false;
	if (std::string(arg[4]) == "--greedy")
		return true;
	return false;
}

int		main(int argc, const char **argv)
{
	Solver	*solver;
	int		heuristics;
	bool	isGreedy;
	int		type;

	if (argc < 4
			|| !(type = getType(argv[1]))
			|| (heuristics = getHeuristicsChoice(argv[3])) < 0
			|| (argc == 5 && !(isGreedy = getGreedy(argc, argv))))
	{
		std::cerr << "./npuzzle [-f filename|-s size] [manhattan|hamming|misplacedTiles|lnManhattan] [--greedy]" << std::endl;
		return 0;
	}
	try {
		if (type == FILENAME)
			solver = new Solver(argv[2], heuristics, isGreedy);
		if (type == SIZE) {
			if (strlen(argv[2]) > 2)
				throw new SolverException("Maximum grid size is 99");
			solver = new Solver(std::atoi(argv[2]), heuristics, isGreedy);
		}
	} catch (SolverException *e) {
		std::cerr << e->what() << std::endl;
		return (1);
	}
	solver->solver();
	delete solver;
	return (0);
}
