#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

typedef struct			s_state
{
	char				*board;
	int					g;
	int					h;
	struct s_state*		predecessor;
}						t_state;

char		*parse(std::string filename, size_t *size)
{
	std::ifstream	file;
	std::string		line;
	char			*res = nullptr;
	bool			hasSize = false;
	int				curPos = 0;

	*size = 0;
	file.open(filename);
	while (std::getline(file, line))
	{
		size_t pos = line.find('#');
		if (pos != line.npos)
			line.erase(pos);
		if (line.length() <= 0)
			continue ;
		std::istringstream iss(line);
		if (!hasSize) {
			if (!(iss >> *size)) { return nullptr; }
			if (*size < 3)
				return (nullptr);
			hasSize = true;
			res = new char[*size];
		} else {
			for (int i = 0; i < 3; ++i)
			{
				if (!(iss >> (res + curPos))) {
					std::cout << "Unable to get full line" << std::endl;
					return nullptr;
				}
				curPos++;
			}
		}
	}
	std::cout << "Input: " << std::endl;
	for (size_t y = 0; y < *size; ++y)
	{
		for (size_t x = 0; x < *size; ++x)
		{
			std::cout << res[x + y * *size] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "-------" << std::endl;
	return res;
}
