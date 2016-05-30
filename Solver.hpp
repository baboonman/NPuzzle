#ifndef SOLVER_HPP
# define SOLVER_HPP

# include <cstring>
# include <iostream>
# include <vector>
# include <set>
# define NORTH 0
# define WEST 1
# define SOUTH 2
# define EAST 3

typedef struct			s_state
{
	char				*board;
	int					g;
	int					h;
	struct s_state*		predecessor;
}						t_state;

typedef struct			s_state_cmp
{
	bool operator() (const t_state *lhs, const t_state *rhs)
	{
		return lhs->h < rhs->h; 
	}
}						t_state_cmp;

class	Solver
{
	public:
		Solver(int x, int y);
		virtual ~Solver();
		int		hamming(char *state);
		int		manhattan(char *state);

		void			solver();

	private:
		int		_findTile(int id);
		void	_genSol(void);
		int		_getBlankPos(t_state * current);
		bool							_board_cmp(t_state *s1, t_state * s2);
		t_state*						_swapTile(int pos, int npos, t_state *state, t_state *last);
		std::vector<t_state *>			_getNeighbours(t_state *current, t_state *last);

		t_state	*_initialState;
		char	*_solution;
		int		_x;
		int		_y;
		int		_totSize;
		
		std::set<t_state*, t_state_cmp>		_openSet;
		std::vector<t_state *>				_closeSet;
};



#endif
