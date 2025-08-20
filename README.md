# Scrabble Game

A fully functional Scrabble game implemented in C++ with SDL2 graphics.

## Features

- Complete Scrabble board with premium squares (Double/Triple Letter/Word)
- Two to four player turn-based gameplay
- Dictionary word validation
- Drag-and-drop tile placement
- Score calculation according to official Scrabble rules
- Visual feedback for valid moves and premium squares
- Game log to track moves and scores

## Requirements

- C++17 compatible compiler (g++ or clang++)
- SDL2 library
- SDL2_ttf library

## Installation

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev
```

### macOS (with Homebrew)
```bash
brew install sdl2 sdl2_ttf
```

### Windows
1. Install Visual Studio Community
2. Download SDL2 development libraries from https://www.libsdl.org/
3. Download SDL2_ttf from https://www.libsdl.org/projects/SDL_ttf/

## Building

```bash
# Clone the repository
git clone <repository-url>
cd scrabble

# Build the game
make

# Run the game
./scrabble

# Build and run tests
make test
./test_dict
```

## How to Play

1. **Starting the Game**
   - Run the executable
   - Select number of players (2-4) using number keys, then press ENTER
   - Player 1 goes first

2. **Playing a Turn**
   - Click and drag tiles from your rack to the board
   - Tiles must form valid words horizontally or vertically
   - The first word must touch the center star
   - Subsequent words must connect to existing tiles

3. **Controls**
   - **Click + Drag**: Move tiles from rack to board
   - **ENTER**: Confirm your word placement
   - **ESC**: Cancel current word placement
   - **SPACE**: Pass your turn

4. **Scoring**
   - Each letter has a point value
   - Premium squares multiply letter or word scores:
     - **2L**: Double letter score
     - **3L**: Triple letter score
     - **2W**: Double word score
     - **3W**: Triple word score
   - **Bingo Bonus**: 50 points for using all 7 tiles in one turn

5. **Game End**
   - Game ends when the tile bag is empty and a player uses all their tiles
   - Points from remaining tiles in opponents' racks are subtracted from their scores
   - Player with the highest score wins

## Project Structure

```
scrabble/
├── src/                    # Source code
│   ├── Game.cpp/.h        # Main game controller
│   ├── Board.cpp/.h       # Game board logic
│   ├── Player.cpp/.h      # Player management
│   ├── Tile.cpp/.h        # Tile representation
│   ├── TileBag.cpp/.h     # Tile distribution
│   ├── Dictionary.cpp/.h  # Word validation
│   ├── TextRenderer.cpp/.h# Text rendering
│   └── main.cpp           # Entry point
├── assets/                # Game assets
│   ├── dictionaries/      # Word list files
│   └── fonts/             # Font files
├── Makefile              # Build configuration
└── DOCUMENTATION.md      # Technical documentation
```

## Technical Documentation

See [DOCUMENTATION.md](DOCUMENTATION.md) for detailed information about:
- Class implementations
- Game architecture
- Build system
- Game flow and rules

## License

This project is for educational purposes only.