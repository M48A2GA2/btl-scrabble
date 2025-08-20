#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include "Board.h"
#include "Player.h"
#include "TileBag.h"
#include "TextRenderer.h"
#include "Dictionary.h"

// Game window dimensions
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

// Board rendering constants
const int CELL_SIZE = 40;
const int BOARD_START_X = 50;
const int BOARD_START_Y = 50;

/**
 * Game state enumeration
 */
enum class GameState
{
    PLAYER_SELECTION,  // Selecting number of players
    PLAYING,           // Game in progress
    GAME_OVER          // Game has ended
};

/**
 * Main game class that controls the Scrabble game flow
 */
class Game
{
public:
    /**
     * Constructor
     */
    Game();

    /**
     * Destructor
     */
    ~Game();

    /**
     * Initialize the game components
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * Run the main game loop
     */
    void run();

    /**
     * Clean up game resources
     */
    void cleanup();

private:
    // SDL components
    SDL_Window *window;           // Main game window
    SDL_Renderer *renderer;       // Renderer for drawing
    TextRenderer textRenderer;    // Text rendering helper

    // Window management
    int currentWindowWidth;       // Current window width
    int currentWindowHeight;      // Current window height

    // Game state
    GameState gameState;          // Current game state
    bool running;                 // Whether the game is running
    int playerCount;              // Number of players (2-4)

    // Game components
    Board board;                  // Game board
    std::vector<Player> players;  // Players in the game
    TileBag tileBag;              // Bag of tiles
    Dictionary dictionary;        // Word dictionary
    int currentPlayer;            // Index of current player

    // Mouse and interaction
    int mouseX, mouseY;           // Current mouse position
    bool isDragging;              // Whether a tile is being dragged
    int selectedTileIndex;        // Index of selected tile in hand

    // Word placement
    bool isPlacingWord;                           // Whether player is placing a word
    bool hasSetDirection;                         // Whether word direction is set
    int placementDirection;                       // 0 = horizontal, 1 = vertical
    std::vector<std::pair<int, int>> pendingPlacements;   // Positions of tiles being placed
    std::vector<int> pendingTileIndices;          // Indices of tiles being placed

    // Blank tile handling
    int pendingBlankTileHandIndex;                // Index of blank tile awaiting letter selection

    // Pass tracking
    int consecutivePasses;                        // Number of consecutive passes

    // Game log
    std::vector<std::string> gameLog;             // Log of game events

    // Event handling
    void handleEvents();
    void handleMouseClick(int x, int y);
    void handleMouseRelease(int x, int y);
    void handleWindowResize(int width, int height);

    // Game logic
    void update();
    bool isValidMove(int row, int col) const;
    void startWordPlacement();
    bool canAddTileToWord(int row, int col);
    void addTileToWord(int row, int col, int tileIndex);
    void commitWord();
    void cancelWordPlacement();
    void passTurn();
    bool validateWordPlacement();
    void initializePlayers();
    void dealInitialTiles();
    int getPlayerCount() const;

    // Word analysis
    std::vector<std::string> getFormedWords() const;
    std::string getMainWordFromPlacements() const;
    std::string getHorizontalWordAt(int row, int col, int tileIndex) const;
    std::string getVerticalWordAt(int row, int col, int tileIndex) const;
    std::string buildWordFromPosition(int row, int col, int deltaRow, int deltaCol, int newTileIndex) const;

    // Positions + scoring helpers
    struct WordWithPositions 
    { 
        std::string word; 
        std::vector<std::pair<int,int>> positions; 
        bool isMain; 
    };
    
    std::vector<WordWithPositions> getFormedWordsWithPositions() const;
    int scoreWordWithPremiums(const WordWithPositions &wordInfo) const;

    // Scoring
    int calculateWordScore(const std::string &word) const;

    // Utility functions
    int getTileIndexAtPosition(int x, int y);
    bool getBoardPosition(int x, int y, int &row, int &col);
    bool isPositionOnBoard(int x, int y);
    bool isPositionInHand(int x, int y);
    SDL_Color getPremiumSquareColor(Board::Premium premium);
    std::string getPremiumSquareText(Board::Premium premium);
    void validateTileIndices(); // Clean up invalid tile indices

    // Rendering
    void render();
    void renderBoard();
    void renderPendingPlacements();
    void renderPlayerHand();
    void renderGameInfo();
    void renderLog();
    void renderDraggedTile();
    void renderPlayerSelection();

    // Logging
    void addLog(const std::string &message);
};