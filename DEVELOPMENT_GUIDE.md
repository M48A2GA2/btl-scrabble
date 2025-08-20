# Scrabble Development Guide

This guide is for developers who want to contribute to or extend the Scrabble game.

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
├── test/                  # Test programs
│   └── test_dictionary.cpp# Dictionary testing
├── Makefile              # Build configuration
├── README.md             # User documentation
├── USER_GUIDE.md         # Player instructions
└── DOCUMENTATION.md      # Technical documentation
```

## Code Standards

### C++ Standard
- Code uses C++17 features
- Follow modern C++ practices (RAII, smart pointers, etc.)

### Naming Conventions
- Classes: PascalCase (e.g., `Game`, `TileBag`)
- Functions: camelCase (e.g., `isValidWord`, `drawTile`)
- Variables: camelCase (e.g., `currentPlayer`, `tileBag`)
- Constants: UPPER_SNAKE_CASE (e.g., `BOARD_SIZE`, `WINDOW_WIDTH`)

### Memory Management
- Use smart pointers (`std::unique_ptr`) for automatic memory management
- Avoid raw `new`/`delete` operations
- Follow rule of five for classes managing resources

### Error Handling
- Use exceptions for error conditions
- Validate inputs at function boundaries
- Provide meaningful error messages

## Key Classes

### Game Class
The central coordinator of the game:
- Manages SDL2 initialization and rendering
- Handles user input and events
- Controls game state transitions
- Coordinates interactions between other components

**When modifying:**
- Be careful with SDL2 resource management
- Ensure all event handlers are properly updated
- Maintain consistent game state

### Board Class
Represents the game board:
- Manages tile placement and validation
- Calculates scores based on premium squares
- Identifies words formed by moves

**When modifying:**
- Ensure move validation rules are preserved
- Maintain consistency with Scrabble rules
- Update premium square logic if changed

### Dictionary Class
Handles word validation:
- Loads words from file or fallback list
- Provides efficient word lookup
- Normalizes words for comparison

**When modifying:**
- Ensure backward compatibility with word validation
- Test with various word inputs
- Consider performance implications of changes

## Build System

### Makefile
Simple make-based build system:
```makefile
# Build the game
make

# Clean build artifacts
make clean
```

### Adding New Source Files
1. Create the .cpp/.h files in src/
2. The Makefile automatically includes all .cpp files in src/
3. Include headers in source files as needed

### Dependencies
- SDL2 for graphics and input
- SDL2_ttf for text rendering
- Standard C++17 library

## Testing

### Dictionary Testing
A simple test program exists to validate dictionary functionality:

```bash
# Compile and run dictionary tests
g++ -std=c++17 test_dictionary.cpp src/Dictionary.cpp -o test_dict
./test_dict
```

### Adding New Tests
1. Create a new test file in the test/ directory
2. Include necessary headers
3. Write test cases that validate functionality
4. Document what each test is verifying

## Extending Functionality

### Adding New Game Features
1. Determine which class needs modification
2. Follow existing code patterns and conventions
3. Add new methods or modify existing ones
4. Update related classes as needed
5. Test thoroughly

### Adding New UI Elements
1. Modify Game class rendering methods
2. Add new SDL2 rendering code
3. Handle any new user input
4. Update game state as needed

### Modifying Game Rules
1. Identify the classes that implement the rules
2. Modify the relevant methods
3. Ensure all related functionality is updated
4. Test with various scenarios

## Debugging

### Common Debugging Approaches
1. Use `std::cout` statements for simple debugging
2. Use GDB for complex runtime issues
3. Check SDL2 error messages with `SDL_GetError()`
4. Validate file paths and resource loading

### Logging
The Game class includes a logging system:
- Use `addLog()` to add messages to the game log
- Displayed in the UI for debugging gameplay issues

## Performance Considerations

### Rendering
- Minimize redraws by only updating changed areas
- Use SDL2's double buffering
- Batch rendering operations when possible

### Memory
- Use object pooling for frequently created/destroyed objects
- Avoid memory leaks with smart pointers
- Profile memory usage for large data structures

### Computation
- Cache expensive calculations
- Use efficient data structures for lookups
- Profile performance bottlenecks

## Contributing

### Pull Request Process
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests if applicable
5. Update documentation
6. Submit a pull request

### Code Review
All pull requests must be reviewed by maintainers before merging:
- Code must follow project standards
- New functionality must be tested
- Documentation must be updated
- No breaking changes without discussion

## Future Improvements

Some ideas for future enhancements:
- AI player implementation
- Network multiplayer support
- Save/load game state
- Custom board layouts
- Tournament mode with multiple games
- Enhanced UI with animations
- Mobile port using SDL2 ports