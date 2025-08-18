#include "src/Dictionary.h"
#include <iostream>
#include <vector>

int main() {
    Dictionary dict;
    
    std::cout << "Dictionary loaded with " << dict.size() << " words\n\n";
    
    // Test some common words
    std::vector<std::string> testWords = {
        "cat", "dog", "house", "tree", "scrabble", "quality", "xylem", "quiz",
        "zygote", "a", "aa", "xyz", "supercalifragilisticexpialidocious", 
        "notarealword", "HOUSE", "CaT", "123", "hello!"
    };
    
    std::cout << "Testing word validation:\n";
    for (const auto& word : testWords) {
        bool valid = dict.isValidWord(word);
        std::cout << "'" << word << "' -> " << (valid ? "VALID" : "INVALID") << "\n";
    }
    
    return 0;
}
