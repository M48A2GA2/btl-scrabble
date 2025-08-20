#include "Game.h"
#include <iostream>
#include <cstdlib>

/**
 * Main entry point for the Scrabble game
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Exit code (0 for success, non-zero for error)
 */
int main(int argc, char *argv[])
{
    // Create and initialize the game
    Game game;

    if (!game.initialize())
    {
        std::cerr << "Failed to initialize game!" << std::endl;
        return EXIT_FAILURE;
    }

    // Run the main game loop
    game.run();

    return EXIT_SUCCESS;
}
