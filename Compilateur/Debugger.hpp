#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <vector>
#include "ByteCode.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

class Debugger {
public:
	Debugger(ByteCode b) : bc(std::move(b)) {};
	void debug();
private:
	ByteCode bc;
	// std::vector<
	void executeBC();
};

#endif