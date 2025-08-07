#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "Board.h"
#include "Player.h"
#include "TileBag.h"
#include "TextRenderer.h"
#include <vector>
#include <memory>
#include <utility>
#include "Dictionary.h"

class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    Board board;
    std::vector<Player> players;
    TileBag tileBag;
    TextRenderer textRenderer;
    size_t currentPlayer;
    int selectedTileIndex;
    bool isDragging;
    int mouseX, mouseY;

    static const int WINDOW_WIDTH = 1000;
    static const int WINDOW_HEIGHT = 700;
    static const int CELL_SIZE = 35;
    static const int BOARD_START_X = 50;
    static const int BOARD_START_Y = 50;

public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void handleEvents();
    void handleMouseRelease(int x, int y);
    void handleMouseClick(int x, int y);
    void update();
    void render();
    void renderBoard();
    void renderPlayerHand();
    void renderGameInfo();
    void renderDraggedTile();
    void cleanup();

    // Add missing function declarations
    bool validateCompleteWord(const std::vector<std::pair<int, int>> &positions);
    bool validateCompleteMove(const std::vector<std::pair<int, int>> &positions);
    bool tilesFormLine(const std::vector<std::pair<int, int>> &positions);
    int calculateWordScore(const std::vector<std::pair<int, int>> &positions,
                           const std::vector<std::unique_ptr<Tile>> &tiles);

private:
    Dictionary dictionary;
    SDL_Color getPremiumSquareColor(Board::Premium premium);

    void initializePlayers();
    void dealInitialTiles();
    void startWordPlacement();
    void addTileToWord(int row, int col, int tileIndex);
    bool canAddTileToWord(int row, int col) const;
    void commitWord();
    void cancelWordPlacement();
    void renderPendingPlacements();
    bool validateWordPlacement() const;
    std::string getPremiumSquareText(Board::Premium premium);

    std::vector<std::pair<int, int>> pendingPlacements;
    std::vector<int> pendingTileIndices;
    std::vector<std::string> getFormedWords() const;

    int placementDirection;
    int getTileIndexAtPosition(int x, int y);
    bool getBoardPosition(int x, int y, int &row, int &col);
    bool isPositionOnBoard(int x, int y);
    bool isPositionInHand(int x, int y);
    bool isValidMove(int row, int col) const;
    bool validateWordsFormed(int row, int col) const;
    bool isPlacingWord;
    bool hasSetDirection;
};

#endif
