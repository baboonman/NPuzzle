#ifndef SOLVER_HPP
# define SOLVER_HPP

# include <numeric>
# include <cstring>
# include <iostream>
# include <vector>
# include <set>
# include <unordered_map>
# include <random>
# include <algorithm>
# include <iomanip>
# include <sys/time.h>
# include "SolverException.hpp"
# include "parser.hpp"
# define NORTH			0
# define WEST			1
# define SOUTH			2
# define EAST			3

# define HAMMING		0
# define MANHATTAN		1
# define MISPLACEDTILES	2
# define LNMANHATTAN	3
# define CRAZYMAN		4

typedef struct			s_state_cmp
{
	bool operator() (const t_state *lhs, const t_state *rhs)
	{
		return lhs->f <= rhs->f; 
	}
}						t_state_cmp;

typedef std::set<t_state *, t_state_cmp>::iterator	t_openSetIt;

class	Solver
{
	public:
		Solver(int size, int heuristics, bool greedy);
		Solver(std::string filename, int heuristics, bool greedy);
		virtual					~Solver();
		int						hamming(int *state);
		int						manhattan(int *state);
		int						lnManhattan(int *board);
		int						misplacedTiles(int *state);
		int						crazyMan(int *board);
		int						getHeuristics(int *state);
		void					solver();

	private:
		Solver();
		Solver(const Solver & rhs);
		Solver&					operator=(const Solver & rhs);
		void					_generator(void);
		int						_findTile(int id);
		int						_findTile(int *board, int tile);
		void					_genSol(void);
		int						_getBlankPos(t_state * current);
		bool					_board_cmp(t_state *s1, t_state * s2);
		t_state*				_swapTile(int pos, int npos, t_state *state, t_state *last);
		std::vector<t_state *>	*_getNeighbours(t_state *current, t_state *last);
		void					_init(void);

		template <class I>
		bool					_cmpBoard(t_state *s, I it);
		template <class T>
		auto					_findState(const T &set, t_state *s);

		void					_printState(t_state *s);
		void					_printPred(t_state *finalState);
		std::string				_getHash(t_state *state);
		void					_insertInClose(t_state *state, const std::string &hash);

		int						_inversions(int *board);
		bool					_isSolvable(t_state *initialState, int *goalState);
		void					_deleteState(t_state *state);
		void					_printBoard(int *board);
		void					_printInfo(t_state *s);

		int						_time;
		int						*(_heuristicsFn)(int *);
		int						_whichHeuristics;
		t_state					*_initialState;
		int					*_solution;
		int						_size;
		int						_totSize;
		std::string				_srcFile;
		int						_maxStateInMemory = 0;
		int						_totalOpenedStateEver = 0;
		
		std::set<t_state*, t_state_cmp>					_openSet;
		std::unordered_map<std::string, t_state *>		_closeSetHash;
		std::unordered_map<std::string, t_openSetIt>	_openSetHash;
		bool											_greedy;
};

template<class I>
bool					Solver::_cmpBoard(t_state *s, I it)
{
	for (int i = 0 ; i < this->_totSize ; i++)
	{
		if (s->board[i] != (*it)->board[i])
			return false;
	}
	return true;
}

template<class T>
auto					Solver::_findState(const T &set, t_state *s)
{
	for (auto it = set.begin() ; it != set.end() ; it++)
	{
		if (this->_cmpBoard(s, it))
			return it;
	}
	return set.end();
}

#endif
