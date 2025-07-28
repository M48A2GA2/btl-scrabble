#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

Dictionary::Dictionary()
{
    loadBasicWords(); // Load basic words as fallback
}

bool Dictionary::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open dictionary file: " << filename << std::endl;
        return false;
    }

    words.clear();
    std::string word;
    while (std::getline(file, word))
    {
        // Convert to uppercase and add to set
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);
        if (!word.empty())
        {
            words.insert(word);
        }
    }

    std::cout << "Loaded " << words.size() << " words from dictionary" << std::endl;
    return true;
}

void Dictionary::loadBasicWords()
{
    // Basic word list for testing - in a real game you'd load from a comprehensive dictionary file
    std::vector<std::string> basicWords = {
        "CAT", "DOG", "HOUSE", "TREE", "BOOK", "GAME", "PLAY", "WORD", "TILE",
        "BOARD", "SCORE", "POINT", "LETTER", "PLACE", "MOVE", "TURN", "WIN",
        "LOSE", "DRAW", "HAND", "RACK", "BAG", "BLANK", "TRIPLE", "DOUBLE",
        "START", "END", "FIRST", "LAST", "BEST", "GOOD", "BAD", "NEW", "OLD",
        "BIG", "SMALL", "FAST", "SLOW", "HOT", "COLD", "RED", "BLUE", "GREEN",
        "WHITE", "BLACK", "YELLOW", "ORANGE", "PURPLE", "PINK", "BROWN", "GRAY",
        "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN",
        "GO", "RUN", "WALK", "JUMP", "STOP", "LOOK", "SEE", "HEAR", "TALK", "SAY",
        "MAKE", "TAKE", "GIVE", "GET", "PUT", "COME", "WORK", "HELP", "FIND", "KNOW"};

    for (const auto &word : basicWords)
    {
        words.insert(word);
    }

    std::cout << "Loaded " << words.size() << " basic words" << std::endl;
}

bool Dictionary::isValidWord(const std::string &word) const
{
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
    return words.find(upperWord) != words.end();
}
