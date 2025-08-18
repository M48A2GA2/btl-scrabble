#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>

class Dictionary
{
private:
    std::unordered_set<std::string> words;

    static std::string toUpperCase(std::string_view word);
    void loadBasicWords();
    bool loadFromMultipleSources();

    // Helper functions for better word validation
    static bool isValidScrabbleWord(std::string_view word);
    static std::string normalize(std::string_view word);

public:
    Dictionary();
    explicit Dictionary(const std::string &filename);

    bool loadFromFile(const std::string &filename);
    bool isValidWord(std::string_view word) const;
    size_t size() const noexcept { return words.size(); }

    // Check loaded
    bool isLoaded() const noexcept { return !words.empty(); }

    // Get statistics about loaded dictionary
    void printStats() const;
};

#endif
