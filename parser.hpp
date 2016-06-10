#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>

typedef struct			s_state
{
	int				*board;
	int					g;
	int					h;
	int					f;
	struct s_state*		predecessor;
}						t_state;

int		*parse(std::string filename, size_t *size);

#endif
