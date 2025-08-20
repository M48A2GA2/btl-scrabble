#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>

class Dictionary
{
private:
    std::unordered_set<std::string> words;  // Set of valid words for fast lookup

    /**
     * Load a basic set of words as fallback
     * Used when no dictionary file can be loaded
     */
    void loadBasicWords();

    /**
     * Try to load dictionary from multiple possible sources
     * @return true if successfully loaded from any source, false otherwise
     */
    bool loadFromMultipleSources();

    /**
     * Check if a word is valid according to Scrabble rules
     * @param word The word to validate
     * @return true if word is valid, false otherwise
     */
    static bool isValidScrabbleWord(std::string_view word);

    /**
     * Normalize a word to uppercase for consistent storage and lookup
     * @param word The word to normalize
     * @return The normalized (uppercase) word
     */
    static std::string normalize(std::string_view word);

public:
    /**
     * Default constructor - attempts to load dictionary from file
     * Falls back to basic word set if file loading fails
     */
    Dictionary();

    /**
     * Constructor with specific filename
     * @param filename The path to the dictionary file
     */
    explicit Dictionary(const std::string &filename);

    /**
     * Load words from a file
     * @param filename The path to the dictionary file
     * @return true if successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string &filename);

    /**
     * Check if a word is valid (exists in dictionary)
     * @param word The word to check
     * @return true if word is valid, false otherwise
     */
    bool isValidWord(std::string_view word) const;

    /**
     * Get the number of words in the dictionary
     * @return The number of words
     */
    size_t size() const noexcept { return words.size(); }

    /**
     * Check if the dictionary has been successfully loaded
     * @return true if dictionary contains words, false otherwise
     */
    bool isLoaded() const noexcept { return !words.empty(); }

    /**
     * Print statistics about the loaded dictionary
     * Shows word count and distribution by length
     */
    void printStats() const;
};

#endif
