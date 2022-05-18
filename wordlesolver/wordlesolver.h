#ifndef WORDLE_HEADER
#define WORDLE_HEADER

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <math.h> 
#include <thread>
#include <iterator>
#include <sstream>

enum states {
	grey,
	yellow,
	green
};

// calculate entropy of a word
double entropy(std::string word);

// find all the states possible 
void recursion(std::vector<double> &results, int level, std::vector<states> currStates, std::string word);

// calculates the probability of a result for a given word
double calcProb(std::vector<states> currStates, std::string guess);

// finds the optimal word from the current wordbank
std::string optimalWord();

// splits strings 
template <typename Out>
void split(const std::string& s, char delim, Out result);
std::vector<std::string> split(const std::string& s, char delim);

// given a set of states and a word, filters the current word bank 
std::vector<std::string> filterWords(std::vector<states> result, std::string word);

// calculates the bestOpener for a given word set (MAY TAKE FOREVER)
std::string bestOpenerCalc();
#endif