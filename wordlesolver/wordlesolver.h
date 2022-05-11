#ifndef WORDLE_HEADER
#define WORDLE_HEADER

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <math.h> 

enum states {
	grey,
	yellow,
	green
};

double entropy(std::string word);
void recursion(std::vector<double> &results, int level, std::vector<states> currStates, std::string word);
double calcProb(std::vector<states> currStates, std::string guess);
std::string optimalWord();
#endif