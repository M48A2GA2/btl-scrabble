#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <sys/stat.h>
#include <array>

// Helper function to check if file exists (avoiding std::filesystem for compatibility)
namespace
{
    bool fileExists(const std::string &path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }
} // anonymous namespace

/**
 * Default constructor - attempts to load dictionary from file
 * Falls back to basic word set if file loading fails
 */
Dictionary::Dictionary()
{
    if (!loadFromMultipleSources())
    {
        std::cerr << "Failed to load comprehensive dictionary, falling back to basic words\n";
        loadBasicWords();
    }
}

/**
 * Constructor with specific filename
 *
 * @param filename The path to the dictionary file
 */
Dictionary::Dictionary(const std::string &filename)
{
    if (!loadFromFile(filename))
    {
        std::cerr << "Failed to load dictionary from " << filename << ", trying other sources\n";
        if (!loadFromMultipleSources())
        {
            std::cerr << "Failed to load any dictionary, using basic words\n";
            loadBasicWords();
        }
    }
}

/**
 * Load words from a file
 *
 * @param filename The path to the dictionary file
 * @return true if successfully loaded, false otherwise
 */
bool Dictionary::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open dictionary file: " << filename << '\n';
        return false;
    }

    words.clear();
    words.reserve(400000); // Reserve space for ~370k words + some extra

    std::string word;
    size_t validWords = 0;
    size_t totalLines = 0;

    std::cout << "Loading comprehensive English dictionary...\n";

    while (std::getline(file, word))
    {
        totalLines++;

        // Remove any trailing whitespace
        word.erase(word.find_last_not_of(" \t\r\n") + 1);

        if (!word.empty() && isValidScrabbleWord(word))
        {
            words.insert(normalize(word));
            validWords++;
        }

        // Show progress less frequently for faster loading
        if (totalLines % 100000 == 0)
        {
            std::cout << "Progress: " << totalLines << " lines processed...\n";
        }
    }

    std::cout << "Dictionary loaded: " << validWords << " valid Scrabble words\n";
    return validWords > 0;
}

/**
 * Load a basic set of words as fallback
 * Used when no dictionary file can be loaded
 */
void Dictionary::loadBasicWords()
{
    constexpr std::array<std::string_view, 80> basicWords = {
        "CAT", "DOG", "HOUSE", "TREE", "BOOK", "GAME", "PLAY", "WORD", "TILE",
        "BOARD", "SCORE", "POINT", "LETTER", "PLACE", "MOVE", "TURN", "WIN",
        "LOSE", "DRAW", "HAND", "RACK", "BAG", "BLANK", "TRIPLE", "DOUBLE",
        "START", "END", "FIRST", "LAST", "BEST", "GOOD", "BAD", "NEW", "OLD",
        "BIG", "SMALL", "FAST", "SLOW", "HOT", "COLD", "RED", "BLUE", "GREEN",
        "WHITE", "BLACK", "YELLOW", "ORANGE", "PURPLE", "PINK", "BROWN", "GRAY",
        "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN",
        "GO", "RUN", "WALK", "JUMP", "LOOK", "SEE", "HEAR", "TALK", "SAY",
        "MAKE", "TAKE", "GIVE", "GET", "PUT", "COME", "WORK", "HELP", "FIND", "KNOW"};

    words.clear();
    words.reserve(basicWords.size());

    for (const auto &word : basicWords)
    {
        words.emplace(word);
    }

    std::cout << "Loaded " << words.size() << " basic words\n";
}

/**
 * Check if a word is valid (exists in dictionary)
 *
 * @param word The word to check
 * @return true if word is valid, false otherwise
 */
bool Dictionary::isValidWord(std::string_view word) const
{
    if (word.empty())
        return false;
    return words.find(normalize(word)) != words.end();
}

/**
 * Try to load dictionary from multiple possible sources
 *
 * @return true if successfully loaded from any source, false otherwise
 */
bool Dictionary::loadFromMultipleSources()
{
    // Try multiple possible locations for the dictionary file
    constexpr std::array<std::string_view, 5> possiblePaths = {
        "assets/dictionaries/english_words.txt",
        "./assets/dictionaries/english_words.txt",
        "../assets/dictionaries/english_words.txt",
        "dictionaries/english_words.txt",
        "english_words.txt"};

    std::cout << "Searching for comprehensive dictionary...\n";

    for (const auto &path : possiblePaths)
    {
        if (fileExists(std::string(path)))
        {
            std::cout << "Found dictionary at: " << path << "\n";
            if (loadFromFile(std::string(path)))
            {
                return true;
            }
        }
    }

    std::cerr << "Could not find comprehensive dictionary file in any expected location\n";
    std::cerr << "Searched paths:\n";
    for (const auto &path : possiblePaths)
    {
        std::cerr << "  - " << path << "\n";
    }
    return false;
}

/**
 * Check if a word is valid according to Scrabble rules
 *
 * @param word The word to validate
 * @return true if word is valid, false otherwise
 */
bool Dictionary::isValidScrabbleWord(std::string_view word)
{
    // Check word length (Scrabble words are typically 2-15 letters)
    if (word.length() < 2 || word.length() > 15)
        return false;

    // Check that all characters are letters
    for (char c : word)
    {
        if (!std::isalpha(static_cast<unsigned char>(c)))
            return false;
    }

    return true;
}

/**
 * Normalize a word to uppercase for consistent storage and lookup
 *
 * @param word The word to normalize
 * @return The normalized (uppercase) word
 */
std::string Dictionary::normalize(std::string_view word)
{
    std::string result;
    result.reserve(word.size());

    for (char c : word)
    {
        result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    return result;
}

/**
 * Print statistics about the loaded dictionary
 * Shows word count and distribution by length
 */
void Dictionary::printStats() const
{
    std::cout << "\n=== Dictionary Statistics ===\n";
    std::cout << "Total words loaded: " << words.size() << "\n";

    // Count words by length
    std::vector<size_t> lengthCounts(16, 0); // 0-15 letters
    for (const auto &word : words)
    {
        if (word.length() < lengthCounts.size())
        {
            lengthCounts[word.length()]++;
        }
    }

    std::cout << "Words by length:\n";
    for (size_t i = 2; i <= 15; ++i)
    {
        if (lengthCounts[i] > 0)
        {
            std::cout << "  " << i << " letters: " << lengthCounts[i] << " words\n";
        }
    }
    std::cout << "=============================\n\n";
}
