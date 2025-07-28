#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_set>
#include <string>

class Dictionary
{
private:
    std::unordered_set<std::string> words;

public:
    Dictionary();
    bool loadFromFile(const std::string &filename);
    void loadBasicWords(); // Fallback if no dictionary file
    bool isValidWord(const std::string &word) const;
    size_t size() const { return words.size(); }
};

#endif
