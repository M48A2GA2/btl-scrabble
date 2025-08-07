#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

Dictionary::Dictionary()
{
    loadBasicWords();
}

Dictionary::Dictionary(const std::string &filename)
{
    if (!loadFromFile(filename))
    {
        std::cerr << "Failed to load dictionary from file, using basic words\n";
        loadBasicWords();
    }
}

bool Dictionary::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open dictionary file: " << filename << '\n';
        return false;
    }

    words.clear();
    words.reserve(100000); // Reserve space for better performance

    std::string word;
    while (std::getline(file, word))
    {
        if (!word.empty())
        {
            // Remove any trailing whitespace
            word.erase(word.find_last_not_of(" \t\r\n") + 1);
            if (!word.empty())
            {
                words.insert(toUpperCase(word));
            }
        }
    }

    std::cout << "Loaded " << words.size() << " words from dictionary\n";
    return true;
}

void Dictionary::loadBasicWords()
{
    constexpr std::string_view basicWords[] = {
        "CAT", "DOG", "HOUSE", "TREE", "BOOK", "GAME", "PLAY", "WORD", "TILE",
        "BOARD", "SCORE", "POINT", "LETTER", "PLACE", "MOVE", "TURN", "WIN",
        "LOSE", "DRAW", "HAND", "RACK", "BAG", "BLANK", "TRIPLE", "DOUBLE",
        "START", "END", "FIRST", "LAST", "BEST", "GOOD", "BAD", "NEW", "OLD",
        "BIG", "SMALL", "FAST", "SLOW", "HOT", "COLD", "RED", "BLUE", "GREEN",
        "WHITE", "BLACK", "YELLOW", "ORANGE", "PURPLE", "PINK", "BROWN", "GRAY",
        "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN",
        "GO", "RUN", "WALK", "JUMP", "STOP", "LOOK", "SEE", "HEAR", "TALK", "SAY",
        "MAKE", "TAKE", "GIVE", "GET", "PUT", "COME", "WORK", "HELP", "FIND", "KNOW"};

    words.clear();
    words.reserve(std::size(basicWords));

    for (const auto &word : basicWords)
    {
        words.emplace(word);
    }

    std::cout << "Loaded " << words.size() << " basic words\n";
}

bool Dictionary::isValidWord(std::string_view word) const
{
    if (word.empty())
        return false;
    return words.find(toUpperCase(word)) != words.end();
}

std::string Dictionary::toUpperCase(std::string_view word)
{
    std::string result;
    result.reserve(word.size());

    for (char c : word)
    {
        result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    return result;
}
