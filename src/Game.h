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

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int CELL_SIZE = 40;
const int BOARD_START_X = 50;
const int BOARD_START_Y = 50;

enum GameState
{
    PLAYER_SELECTION,
    PLAYING,
    GAME_OVER
};

class Game
{
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void cleanup();

private:
    // SDL components
    SDL_Window *window;
    SDL_Renderer *renderer;
    TextRenderer textRenderer;

    // Window management
    int currentWindowWidth;
    int currentWindowHeight;

    // Game state
    GameState gameState;
    bool running;
    int playerCount;

    // Game components
    Board board;
    std::vector<Player> players;
    TileBag tileBag;
    Dictionary dictionary;
    int currentPlayer;

    // Mouse and interaction
    int mouseX, mouseY;
    bool isDragging;
    int selectedTileIndex;

    // Word placement
    bool isPlacingWord;
    bool hasSetDirection;
    int placementDirection; // 0 = horizontal, 1 = vertical
    std::vector<std::pair<int, int>> pendingPlacements;
    std::vector<int> pendingTileIndices;

    // Game log
    std::vector<std::string> gameLog;

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