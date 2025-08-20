# Scrabble Game User Guide

## Getting Started

1. **Launch the Game**
   - Double-click the scrabble executable or run `./scrabble` from terminal
   - Enter names for Player 1 and Player 2 when prompted
   - The game board will appear with each player's rack at the bottom

2. **Understanding the Interface**
   - **Board**: 15x15 grid with premium squares colored differently
   - **Player Rack**: 7 tiles at the bottom of the screen
   - **Game Info**: Current player, score, and tiles remaining
   - **Game Log**: History of moves and scores on the right side

## Playing the Game

### Making a Move

1. **Select a Tile**
   - Click on a tile in your rack to select it
   - The tile will highlight to indicate selection

2. **Place a Tile**
   - Drag the selected tile to a position on the board
   - Release the mouse button to place the tile
   - You can reposition tiles before confirming your move

3. **Continue Building Your Word**
   - Place additional tiles to form a complete word
   - Tiles must be placed in a straight line (horizontally or vertically)

4. **Confirm Your Move**
   - Press **ENTER** to confirm your word placement
   - The game will validate your word against the dictionary
   - If valid, your score will update and new tiles will be drawn
   - If invalid, you'll need to correct your word

### Special Controls

- **ESC**: Cancel your current word placement and return tiles to rack
- **SPACE**: Pass your turn (use when you cannot make a valid move)

### Handling Blank Tiles

- Blank tiles can represent any letter
- When placing a blank tile, you'll be prompted to select which letter it represents
- Once set, the blank tile acts as that letter for the rest of the game

## Game Rules

### Starting the Game
- Player 1 goes first
- The first word must cover the center star square (H8)
- The first word must be at least 2 letters long

### Forming Words
- All tiles placed in a turn must form a single word in one direction
- Any additional words formed by connecting to existing tiles are also scored
- All words formed must be in the dictionary

### Premium Squares
- **Light Blue (2L)**: Double letter score
- **Dark Blue (3L)**: Triple letter score
- **Light Red (2W)**: Double word score
- **Dark Red (3W)**: Triple word score
- Premium squares are only used once (they turn white after first use)

### Scoring
- Each letter has a point value:
  - A, E, I, O, U, L, N, S, T, R: 1 point
  - D, G: 2 points
  - B, C, M, P: 3 points
  - F, H, V, W, Y: 4 points
  - K: 5 points
  - J, X: 8 points
  - Q, Z: 10 points
- Premium squares multiply letter or word scores
- **Bingo Bonus**: 50 points for using all 7 tiles in one turn

### Ending the Game
- The game ends when:
  1. The tile bag is empty and one player uses all their tiles, or
  2. Both players pass consecutively
- Points from remaining tiles in players' racks are subtracted from their scores
- Player with the highest score wins

## Troubleshooting

### Common Issues

**Game won't start**
- Ensure SDL2 libraries are properly installed
- Check that assets/dictionaries/english_words.txt exists

**Tiles won't drag**
- Make sure you're clicking directly on a tile in your rack
- Tiles cannot be moved once confirmed

**Word not accepted**
- Check that all letters are connected in a straight line
- Verify that all formed words are valid dictionary words
- Ensure the first word touches the center star

### Getting Help
For technical issues, please check the documentation or contact the development team.