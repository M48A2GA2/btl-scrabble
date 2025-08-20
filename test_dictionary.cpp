#include "src/Dictionary.h"
#include <iostream>
#include <vector>
#include <cstdlib>

/**
 * Test program for the Dictionary class
 * 
 * This program tests the Dictionary class functionality by:
 * 1. Loading the dictionary
 * 2. Testing various words for validity
 * 3. Reporting results
 * 
 * @return Exit code (0 for success, non-zero for error)
 */
int main() {
    // Create and load dictionary
    Dictionary dict;
    
    std::cout << "Dictionary loaded with " << dict.size() << " words\n\n";
    
    // Test some common words
    const std::vector<std::string> testWords = {
        "cat", "dog", "house", "tree", "scrabble", "quality", "xylem", "quiz",
        "zygote", "a", "aa", "xyz", "supercalifragilisticexpialidocious", 
        "notarealword", "HOUSE", "CaT", "123", "hello!"
    };
    
    std::cout << "Testing word validation:\n";
    for (const auto& word : testWords) {
        bool valid = dict.isValidWord(word);
        std::cout << "'" << word << "' -> " << (valid ? "VALID" : "INVALID") << "\n";
    }
    
    return EXIT_SUCCESS;
}
