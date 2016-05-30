
typedef struct			s_state
{
	int					board[9];
	int					g;
	int					h;
	struct s_state*		predecessor;
}						t_state;

list<t_state>			getNeighbours(t_state* current, t_state* last)
{
	list<t_state *>		allNeighbours;
	list<t_state *>		neighbours;
	
	for (auto n : allNeighbours)
	{
		if (n == last)
			continue ;
		n->g = current->g + C;
		n->h = heuristicfn();
		n->predecessor = current;
		neighbours.insert(n);
	}
	return (neighbours);
}

void					solver(t_state* initial, t_state* solution)
{
	list<t_state *>		open_list;
	list<t_state *>		close_list;
	bool				succes = false;

	list<t_state *>		neighbours();
	t_state *			last;
	t_state	*			current;
	
	open_list.insert(initial);
	last = initial;

	while (open_list != null && !success)
	{
		current = open_list.getMin();

		if (current == solution)
			success == true;
		else
		{
			open_list.pop();
			close_list.insert(current);

			neighbours = getNeighbours(current, last);
			for (auto n : neighbours)
			{
				if (n is not in open_list or close_list)
				{
					open_list.insert(n);
				}
				else
				{
					// if n is in list we call the list elem old_n
					//  n == old_n <=> n->board == old_n->board
					if ( n->g > old_n->g )
						continue ;
					if ((old_n = getOccurenceList(open_list, n) == NULL); // get ptr on list node
						old_n = n;
					//if (n is in open_list)
					//	open_list[x] == n; // replace old ptr with new one
					else if (n is in close_list)
					{
						close_list.extract(old_n);
						open_list.insert(n);
					}

				}
			}

			delete neighbours;
		}
		delete current;
	}
}
