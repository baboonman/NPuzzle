#ifndef PARSER_HPP
# define PARSER_HPP

typedef struct			s_state
{
	uint8_t				*board;
	int					g;
	int					h;
	int					f;
	struct s_state*		predecessor;
}						t_state;

uint8_t		*parse(std::string filename, size_t *size);

#endif
