// wordlesolver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "wordlesolver.h"

std::vector<std::string> currList;

int main()
{
    // setup
    int tries = 0;

    std::ifstream wordFile;
    std::string word;
    wordFile.open("words.txt");
    while (std::getline(wordFile, word)) {
        currList.push_back(word);
    }

    std::cout << "Tries allowed: ";
    std::cin >> tries;

    // let 0 be black, 1 be yellow, 2 be green

    for (int i = 0; i < tries; i++) {
        std::string input;
        std::cout << optimalWord();
        std::cout << "Input result\n";
        std::cin >> input;
    }
}

std::string optimalWord() {

    double maxEntropy = 0;
    std::string returnWord = currList[0];

    for (std::string word : currList) {

        double val = entropy(word);
        if (val > maxEntropy) {
            std::cout << val;
            returnWord = word;
            maxEntropy = val;
        }

    }

    return returnWord;

}
// find entropy of a given word (and available word list)
double entropy(std::string word) {

    // find chances of each 3^5 wordle input possibilities 
    
    // 1 green 2 yellow 3 grey 
    std::vector<double> results; // probability of each "outcome"
    std::vector<states> currstates;
    double ret = 0;
    recursion(results, 0, currstates, word);
    // calculated entropy for each of those (entropy would be -(probofchanceoccuring)log_2(probofchanceoccuring))

    for (double i : results) {
        if (i == 0) {
            ret += 0;
        }
        else {
            ret += -i * log(i);
        }
    }

    std::cout << ret << std::endl;

    return ret;
}

// find chances of each 3^5 wordle input possibilities 

void recursion(std::vector<double> &results, int level, std::vector<states> currStates, std::string word) {

    if (level > 5) return;
    if (level != 0) {

        results.push_back(calcProb(currStates, word));

    }

    int n = currStates.size();
    currStates.push_back(green);
    recursion(results, level + 1, currStates, word);
    currStates[n] = yellow;
    recursion(results, level + 1, currStates, word);
    currStates[n] = grey;
    recursion(results, level + 1, currStates, word);
    

}

double calcProb(std::vector<states> currStates, std::string word) {

    std::unordered_map<char, int> incLetters;
    std::vector<char> exclLetters;
    std::vector<std::pair<char, int>> greenLetters; // second value is position of letter in word
    std::vector<std::string> wordPool = currList;

    while (currStates.size() < 5) {
        currStates.push_back(grey);
    }

    // go through - find how many of each letter required in word 
    for (int i = 0; i < currStates.size(); i++) {

        switch(currStates[i]) {
        case green:
            greenLetters.push_back(std::make_pair(word.at(i), i));
            if (!incLetters[word.at(i)]) {
                incLetters[word.at(i)] = 1;
            }
            else {
                incLetters[word.at(i)]++;
            }
            break;
        case yellow:
            if (!incLetters[word.at(i)]) {
                incLetters[word.at(i)] = 1;
            }
            else {
                incLetters[word.at(i)]++;
            }
            break;
        default: // grey 
            exclLetters.push_back(word.at(i));
        }
    }

    // find viable words from states 
    
    for (auto i : incLetters) {

        std::vector<std::string> newList;

        for (int j = 0; j < wordPool.size(); j++) {

            int count = 0;
            for (int k = 0; k < 5; k++) {
                if (wordPool[j].at(k) == i.first) count++;
            }
            if (count >= i.second) newList.push_back(wordPool[j]);

        }

        wordPool = newList;

    }

    // account for green

    for (auto i : greenLetters) {

        std::vector<std::string> newList;

        for (int j = 0; j < wordPool.size(); j++) {
            if (wordPool[j].at(i.second) == i.first) newList.push_back(wordPool[j]);
        }

        wordPool = newList;

    }

    // account for grey 

    for (auto i : exclLetters) {

        std::vector<std::string> newList;

        for (int j = 0; j < wordPool.size(); j++) {

            bool notFound = true;

            for (int k = 0; k < 5; k++) {
                // grey letter found in word, thus is not valid word
                if (wordPool[j].at(k) == i) notFound = false;
            }

            if (notFound) newList.push_back(wordPool[j]);

        }

        wordPool = newList;

    }

    return (double)wordPool.size() / (double)currList.size();

}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
