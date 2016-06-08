#ifndef __SOLVER_EXCEPTION__HPP__
# define __SOLVER_EXCEPTION__HPP__

# include <iostream>
# include <string>

class SolverException : public std::exception {
	public:
		SolverException (std::string message);
		virtual const char* what() const throw();

	private:
		std::string		_msg;
};

#endif

