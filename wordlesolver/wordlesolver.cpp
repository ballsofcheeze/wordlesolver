// wordlesolver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "wordlesolver.h"

std::vector<std::string> currList;
// vector used for threading
std::vector<std::pair<std::string, double>> threadList;

std::string file = "words1.txt"; // TODO add option to change this 

int main()
{
    // setup

    std::ifstream wordFile;
    std::string line;
    wordFile.open("wordBanks/" + file);
    while (std::getline(wordFile, line)) {
        std::vector<std::string> newLine = split(line, ' ');
        currList.push_back(newLine[0]);
    }

    std::string suggestion = bestOpenerCalc();

    int tries = 0;
    std::cout << "Tries allowed: ";
    std::cin >> tries;

    // let 0 be black, 1 be yellow, 2 be green

    for (int i = 0; i < tries; i++) {

        std::string result;
        std::string input;

        std::cout << "Suggested Guess: " << suggestion << std::endl;
        std::cout << "What was your guess?" << std::endl;
        std::cin >> input;
        std::cout << "Input result (0 = grey, 1 = yellow, 2 = green) MUST BE LENGTH 5" << std::endl;
        std::cin >> result;

        // split string into states vector 

        std::vector<states> resultStates;

        for (char c : result) {

            switch (c) {
            case '0':
                resultStates.push_back(grey);
                break;
            case '1':
                resultStates.push_back(yellow);
                break;
            default: // grey 
                resultStates.push_back(green);
                break;
            }
        }

        currList = filterWords(resultStates, input);

        suggestion = optimalWord();

    }
}

std::vector<std::string> filterWords(std::vector<states> result, std::string word) {

    std::unordered_map<char, int> incLetters;
    std::vector<char> exclLetters;
    std::vector<std::pair<char, int>> greenLetters; // second value is position of letter in word
    std::vector<std::string> wordPool = currList;

    if (result.size() != 5) {
        exit(0);
    }

    // go through - find how many of each letter required in word 
    for (int i = 0; i < result.size(); i++) {
        switch (result[i]) {
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

    // account for green
    if (greenLetters.size() != 0) {
        for (auto i : greenLetters) {
            std::vector<std::string> newList;
            for (int j = 0; j < wordPool.size(); j++) {
                if (wordPool[j].at(i.second) == i.first) newList.push_back(wordPool[j]);
            }
            wordPool = newList;
        }
    }

    // find viable words from states 
    if (incLetters.size() != 0) {
        for (auto i : incLetters) {
            std::vector<std::string> newList;
            for (int j = 0; j < wordPool.size(); j++) {
                int count = 0;
                for (int k = 0; k < 5; k++) {
                    if (wordPool[j].at(k) == i.first) count++;
                }
                if (count >= i.second)
                    newList.push_back(wordPool[j]);
            }
            wordPool = newList;
        }
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

    // get rid of word already searched 
    for (int i = 0; i < wordPool.size(); i++) {
        if (wordPool[i] == word) {
            wordPool.erase(wordPool.begin() + i);
        }
    }

    return wordPool;
}

std::string bestOpenerCalc() {

    // copy contents from beginningWeights directory to wordBanks directory 

    std::string line;


    std::ofstream copyTo("wordBanks/" + file);
    std::ifstream copyFrom;
    copyFrom.open("beginningWeights/" + file);

    int lineNo = 0;

    while (std::getline(copyFrom, line)) {
        copyTo << line << std::endl;
        lineNo++;
    }

    for (int i = lineNo; i < currList.size(); i++) {
        copyTo << currList[i] << std::endl;
    }

    // open file with associated weights
    std::ifstream wordFile;
    std::ofstream outFile("beginningWeights/" + file);

    double maxVal = 0.0;
    std::string ret = "";

    wordFile.open("wordBanks/" + file);
    while (std::getline(wordFile, line)) {
        std::vector <std::string> newLine = split(line, ' ');
        if (newLine.size() == 1) {
            double val = entropy(newLine[0]);

            if (val > maxVal) {
                maxVal = val;
                ret = newLine[0];
            }

            outFile << newLine[0] + " " + std::to_string(val) << std::endl;
        }
        else {

            double val = std::stod(newLine[1]);
            if (val > maxVal) {
                maxVal = val;
                ret = newLine[0];
            }

            outFile << line << std::endl;

        }
    }

    return ret;

}

// GET RID OF THREADING COMPLETELY!
std::string optimalWord() {

    if (currList.size() == 0) {
        return "";
    }

    double maxEntropy = 0;
    std::string finalWord = currList[0];

    for (std::string word : currList) {

        double val = entropy(word);
        if (val > maxEntropy) {
            finalWord = word;
            maxEntropy = val;
        }

    }

    return finalWord;


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

    return ret;
}

// find chances of each 3^5 wordle input possibilities for a single word

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

    while (currStates.size() < 5) {
        currStates.push_back(grey);
    }

    std::vector<std::string> wordPool = filterWords(currStates, word);

    return (double)wordPool.size() / (double)currList.size();

}

template <typename Out>
void split(const std::string& s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
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
