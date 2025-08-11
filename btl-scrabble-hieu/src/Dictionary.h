#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_set>
#include <string>
#include <string_view>

class Dictionary
{
private:
    std::unordered_set<std::string> words;

    static std::string toUpperCase(std::string_view word);
    void loadBasicWords();

public:
    Dictionary();
    explicit Dictionary(const std::string &filename);

    bool loadFromFile(const std::string &filename);
    bool isValidWord(std::string_view word) const;
    size_t size() const noexcept { return words.size(); }

    // Check loaded
    bool isLoaded() const noexcept { return !words.empty(); }
};

#endif
