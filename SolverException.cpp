#include "SolverException.hpp"

SolverException::SolverException(std::string message)
	: _msg(message)
{
	std::exception::exception();
}

const char	*SolverException::what() const throw()
{
	return (this->_msg.c_str());
}
