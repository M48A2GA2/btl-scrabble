# Scrabble Game - Codebase Documentation

This document provides a comprehensive overview of the Scrabble game implementation in C++ with SDL2.

## Table of Contents
1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Core Classes](#core-classes)
   - [Game](#game)
   - [Board](#board)
   - [Player](#player)
   - [Tile](#tile)
   - [TileBag](#tilebag)
   - [Dictionary](#dictionary)
   - [TextRenderer](#textrenderer)
4. [Build System](#build-system)
5. [Game Flow](#game-flow)
6. [Testing](#testing)

## Project Overview

This is a fully functional Scrabble game implemented in C++ with a graphical user interface using SDL2. The game supports:
- Standard Scrabble board (15x15 grid)
- Premium squares (Double/Triple Letter/Word scores)
- Two-player gameplay
- Dictionary validation for words
- Drag-and-drop tile placement
- Score calculation according to Scrabble rules

## Architecture

The codebase follows an object-oriented design with the following key components:

```
scrabble/
├── src/
│   ├── Game.cpp/.h         # Main game controller
│   ├── Board.cpp/.h        # Game board logic
│   ├── Player.cpp/.h       # Player management
│   ├── Tile.cpp/.h         # Tile representation
│   ├── TileBag.cpp/.h      # Tile distribution
│   ├── Dictionary.cpp/.h   # Word validation
│   ├── TextRenderer.cpp/.h # Text rendering with SDL2_ttf
│   └── main.cpp            # Entry point
├── assets/
│   ├── dictionaries/       # Word list files
│   └── fonts/              # Font files
├── Makefile               # Build configuration
└── README.md              # User documentation
```

## Core Classes

### Game

The `Game` class is the main controller that manages the entire game lifecycle.

**Key Responsibilities:**
- Initialize SDL2 components (window, renderer)
- Manage game state (PLAYER_SELECTION, PLAYING, GAME_OVER)
- Handle user input (mouse events, keyboard)
- Coordinate game components (board, players, tile bag)
- Render the game interface
- Implement game rules and logic

**Key Features:**
- Window management and resizing
- Mouse interaction for tile dragging
- Word placement validation
- Score calculation
- Game log display
- Player turn management

### Board

The `Board` class represents the 15x15 Scrabble board.

**Key Responsibilities:**
- Maintain board state (occupied squares, premium squares)
- Manage tile placement
- Validate move legality
- Calculate word scores
- Identify formed words

**Premium Squares:**
- `NONE`: Regular square
- `DOUBLE_LETTER`: Double letter score
- `TRIPLE_LETTER`: Triple letter score
- `DOUBLE_WORD`: Double word score
- `TRIPLE_WORD`: Triple word score
- `START`: Center star square (first move must touch this)

**Key Methods:**
- `placeTile()`: Place a tile on the board
- `canPlaceTile()`: Check if a tile can be placed at a position
- `getWordsFormedByMove()`: Identify words formed by a move
- `calculateWordScore()`: Calculate score for a word

### Player

The `Player` class manages player-specific data and actions.

**Key Responsibilities:**
- Manage player's tile rack (7 tiles)
- Track player score
- Handle tile addition/removal from rack

**Key Features:**
- Hand management (add/remove tiles)
- Score tracking
- Tile search capabilities

### Tile

The `Tile` class represents individual letter tiles.

**Attributes:**
- `letter`: The letter on the tile (' ' for blank tiles)
- `points`: Point value of the tile
- `isBlank`: Whether this is a blank tile
- `usedAs`: What letter a blank tile represents

**Scrabble Tile Distribution:**
- A-1, B-3, C-3, D-2, E-1, F-4, G-2, H-4, I-1, J-8, K-5, L-1, M-3, N-1, O-1, P-3, Q-10, R-1, S-1, T-1, U-1, V-4, W-4, X-8, Y-4, Z-10

### TileBag

The `TileBag` class manages the pool of available tiles.

**Key Responsibilities:**
- Initialize with all 100 Scrabble tiles
- Provide random tile drawing
- Accept returned tiles
- Shuffle tiles

### Dictionary

The `Dictionary` class handles word validation.

**Key Responsibilities:**
- Load words from file or use basic word set
- Validate if a word is a legitimate Scrabble word
- Normalize words (uppercase conversion)
- Filter invalid words (non-alphabetic, wrong length)

**Features:**
- Loads from multiple possible file locations
- Falls back to basic word set if no dictionary file found
- Case-insensitive word validation
- Scrabble-specific word validation (2-15 letters, alphabetic only)

### TextRenderer

The `TextRenderer` class handles text rendering using SDL2_ttf.

**Key Responsibilities:**
- Load fonts
- Render text to SDL surfaces
- Manage text colors and positioning

## Build System

The project uses a simple Makefile for building:

```makefile
CXX = g++
CXXFLAGS = -g -Wall -std=c++17
LIBS = -lSDL2 -lSDL2_ttf

TARGET = scrabble
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
```

**Dependencies:**
- C++17 compatible compiler
- SDL2 library
- SDL2_ttf library

**Build Commands:**
```bash
# Build the game
make

# Clean build artifacts
make clean
```

## Game Flow

1. **Initialization:**
   - SDL2 components are initialized
   - Game window is created
   - Dictionary is loaded
   - Tile bag is populated with 100 tiles

2. **Player Selection:**
   - Players enter their names
   - Initial tiles are dealt (7 per player)

3. **Gameplay Loop:**
   - Current player makes a move by dragging tiles to board
   - Words formed are validated against dictionary
   - Scores are calculated based on tile values and premium squares
   - New tiles are drawn to replenish player's rack
   - Turn passes to next player

4. **Move Validation:**
   - First move must touch center star
   - Subsequent moves must connect to existing tiles
   - All formed words must be in dictionary
   - Tile placement must form valid words horizontally/vertically

5. **Scoring:**
   - Each letter has a point value
   - Premium squares multiply letter or word scores
   - "Bingo" bonus of 50 points for using all 7 tiles

6. **Game End:**
   - Game ends when tile bag is empty and one player uses all tiles
   - Player with highest score wins
   - Remaining tiles in opponents' racks are subtracted from their scores

## Testing

A simple test program (`test_dictionary.cpp`) is included to verify dictionary functionality:

```bash
# Compile test program
g++ -std=c++17 test_dictionary.cpp src/Dictionary.cpp -o test_dict

# Run tests
./test_dict
```

The test validates various words to ensure the dictionary is working correctly.

## License

This project is for educational purposes only.