#include <vector>
#include <iostream>

#define N				3

typedef struct			s_state
{
	char				board[N*N];
	int					g;
	int					h;
	struct s_state*		predecessor;
}						t_state;

void							printS(t_state *s)
{
	int					j = 0;

	std::cout << "";
	for (int i = 0; i < N*N ; i++)
	{
		std::cout << static_cast<int>(s->board[i]);
		if (i < (N*N -1))
			std::cout << ", ";
		j = (j + 1) % N;
		if (!j)
			std::cout << std::endl;
	}
	std::cout << "\tg: " << s->g << "\th: " << s->h << std::endl;
}


int								getBlankPos(t_state *current)
{
	for (int i = 0 ; i < N*N ; i++)
	{
		if (current->board[i] == 0)
			return (i);
	}
	return (-1);
}

bool							board_cmp(t_state *s1, t_state * s2)
{
	for (int i = 0 ; i < N*N ; i++)
	{
		if (s1->board[i] != s2->board[i])
			return (true);
	}
	return false;
}

t_state*						swapTile(int pos, int npos, t_state *state, t_state *last)
{
	t_state						*s;

	s = new t_state;
	std::memcpy(s->board, state->board, N*N * sizeof(char));

	s->board[pos] = state->board[npos];
	s->board[npos] = state->board[pos];

	if (!board_cmp(s, last)) {
		std::cout << "already here" << std::endl;
		return NULL;
	}

	s->g = state->g + 1;
	s->h = state->h;
	s->predecessor = state;

	return s;
}

std::vector<t_state *>			getNeighbours(t_state *current, t_state *last)
{
	 int						p, np;
	 std::vector<t_state *>		neighbours;

	t_state*	s; 

	 p = getBlankPos(current);

	if (p % N != 0)
	{
		np = p - 1;
//		neighbours.push_back(swapTile(p, np, current, last));

		s = swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	if ((p + 1) % N != 0)
	{
		np = p + 1;
//		neighbours.push_back(swapTile(p, np, current, last));

		s = swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	if ((p - N) >= 0)
	{
		np = p - N;
//		neighbours.push_back(swapTile(p, np, current, last));

		s = swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	if ((p + N) < N*N)
	{
		np = p + N;
//		neighbours.push_back(swapTile(p, np, current, last));

		s = swapTile(p, np, current, last);
		if (s)
			neighbours.push_back(s);
	}
	return neighbours;
}


void							printN(std::vector<t_state *> n)
{
	for (auto i : n)
	{
		printS(i);
	}
}

int								main()
{
	char						b[9] = {4, 1, 2, 3, 0, 5, 6, 7, 8};
	char						c[9] = {4, 1, 2, 0, 3, 5, 6, 7, 8};
	t_state						s;
	t_state						t;
	std::vector<t_state *>		n;

	std::memcpy(s.board, b, N*N * sizeof(char));
	s.g = 0;
	s.h = 10;

	std::memcpy(t.board, c, N*N * sizeof(char));
	t.g = 0;
	t.h = 10;

	printS(&s);
	std::cout << "neighbours: " << std::endl;
	n = getNeighbours(&s, &t);
	printN(n);

	return (0);
}
