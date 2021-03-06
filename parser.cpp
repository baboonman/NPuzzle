#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "parser.hpp"


int		*parse(std::string filename, size_t *size)
{
	std::ifstream	file;
	std::string		line;
	int			*res = nullptr;
	bool			hasSize = false;
	size_t			curPos = 0;
	int				tmp;

	*size = 0;
	file.open(filename);
	while (std::getline(file, line) && (!hasSize || curPos < *size * *size))
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
			res = new int[*size * *size];
		} else {
			for (size_t i = 0; i < *size; ++i)
			{
				if (!(iss >> tmp)) {
					std::cout << "Unable to get full line" << std::endl;

					return nullptr;
				}
				res[curPos] = tmp;
				curPos++;
			}
		}
	}
	file.close();
	return (res);
}
