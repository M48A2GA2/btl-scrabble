#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), running(false),
               textRenderer(nullptr), currentPlayer(0),
               selectedTileIndex(-1), isDragging(false), mouseX(0), mouseY(0) {}

Game::~Game()
{
    cleanup();
}

bool Game::initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Scrabble",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!textRenderer.initialize("", 16))
    {
        std::cerr << "Failed to initialize text renderer" << std::endl;
        return false;
    }

    initializePlayers();
    dealInitialTiles();

    running = true;
    return true;
}

void Game::initializePlayers()
{
    players.emplace_back("Player 1");
    players.emplace_back("Player 2");
}

void Game::dealInitialTiles()
{
    for (auto &player : players)
    {
        while (player.hasEmptySlots() && !tileBag.isEmpty())
        {
            player.addTile(tileBag.drawTile());
        }
    }
}

void Game::run()
{
    while (running)
    {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
            else if (event.key.keysym.sym == SDLK_SPACE)
            {
                // Switch players (temporary)
                currentPlayer = (currentPlayer + 1) % players.size();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                handleMouseClick(event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                handleMouseRelease(event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEMOTION:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            break;
        }
    }
}

void Game::handleMouseClick(int x, int y)
{
    int tileIndex = getTileIndexAtPosition(x, y);
    if (tileIndex != -1)
    {
        selectedTileIndex = tileIndex;
        isDragging = true;
        std::cout << "Selected tile: " << players[currentPlayer].getHand()[tileIndex]->letter << std::endl;
    }
}

void Game::handleMouseRelease(int x, int y)
{
    if (isDragging && selectedTileIndex != -1)
    {
        // isDragging = false;
        int row, col;
        if (getBoardPosition(x, y, row, col))
        {
            auto tile = players[currentPlayer].removeTile(selectedTileIndex);
            if (tile && board.placeTile(row, col, std::move(tile)))
            {
                std::cout << "Placed tile on board: " << tile->letter << std::endl;
                if (!tileBag.isEmpty() && players[currentPlayer].hasEmptySlots())
                {
                    players[currentPlayer].addTile(tileBag.drawTile());
                }
            }
            else
            {
                if (tile)
                {
                    players[currentPlayer].addTile(std::move(tile)); // Return tile to hand if placement failed
                    std::cout << "Failed to place tile on board, returning to hand." << std::endl;
                }
                std::cout << "No tile selected." << std::endl;

                // std::cout << "Failed to place tile on board." << std::endl;
            }
        }
        selectedTileIndex = -1; // Reset selection
    }

    selectedTileIndex = -1; // Reset selection
    isDragging = false;
}

int Game::getTileIndexAtPosition(int x, int y)
{
    int handStartY = BOARD_START_Y + Board::BOARD_SIZE * CELL_SIZE + 50;
    if (y < handStartY || y > handStartY + CELL_SIZE)
        return -1; // Not in hand area

    const auto &hand = players[currentPlayer].getHand();
    for (size_t i = 0; i < hand.size(); i++)
    {
        int tileX = BOARD_START_X + i * (CELL_SIZE + 5);
        if (x >= tileX && x < tileX + CELL_SIZE)
        {
            return static_cast<int>(i); // Return index of the tile
        }
    }
    // int index = (x - BOARD_START_X) / (CELL_SIZE + 5);
    // const auto &hand = players[currentPlayer].getHand();
    // if (index < 0 || index >= static_cast<int>(hand.size()))
    //     return -1; // Out of bounds

    return -1;
}

bool Game::getBoardPosition(int x, int y, int &row, int &col)
{
    if (x < BOARD_START_X || x >= BOARD_START_X + Board::BOARD_SIZE * CELL_SIZE ||
        y < BOARD_START_Y || y >= BOARD_START_Y + Board::BOARD_SIZE * CELL_SIZE)
    {
        return false; // Out of bounds
    }

    col = (x - BOARD_START_X) / CELL_SIZE;
    row = (y - BOARD_START_Y) / CELL_SIZE;

    return board.isInBounds(row, col);
}

bool Game::isPositionOnBoard(int x, int y)
{
    int row, col;
    return getBoardPosition(x, y, row, col);
}

bool Game::isPositionInHand(int x, int y)
{
    return getTileIndexAtPosition(x, y) != -1;
}

void Game::update()
{
    // Game logic updates will go here
}

void Game::renderDraggedTile()
{
    if (isDragging && selectedTileIndex != -1)
    {
        const auto &tile = players[currentPlayer].getHand()[selectedTileIndex];

        // Draw tile at mouse position with some offset
        int x = mouseX - CELL_SIZE / 2;
        int y = mouseY - CELL_SIZE / 2;

        SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};

        // Semi-transparent tile background
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 240, 220, 180, 200);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Letter
        char letter = tile->letter;
        if (letter == ' ')
            letter = '?';

        std::string letterStr(1, letter);
        textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});

        // Points
        std::string pointsStr = std::to_string(tile->points);
        textRenderer.renderText(pointsStr, x + CELL_SIZE - 15, y + CELL_SIZE - 15, {0, 0, 0, 255});

        // Highlight valid drop zones
        if (isPositionOnBoard(mouseX, mouseY))
        {
            int row, col;
            if (getBoardPosition(mouseX, mouseY, row, col))
            {
                const auto &square = board.getSquare(row, col);
                if (!square.isOccupied())
                {
                    // Highlight the square where tile would be placed
                    int boardX = BOARD_START_X + col * CELL_SIZE;
                    int boardY = BOARD_START_Y + row * CELL_SIZE;
                    SDL_Rect highlightRect = {boardX + 2, boardY + 2, CELL_SIZE - 4, CELL_SIZE - 4};

                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
                    SDL_RenderFillRect(renderer, &highlightRect);
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                }
            }
        }
    }
}

void Game::render()
{
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    renderBoard();
    renderPlayerHand();
    renderGameInfo();
    renderDraggedTile();

    SDL_RenderPresent(renderer);
}

void Game::renderBoard()
{
    // Draw board squares
    for (int row = 0; row < Board::BOARD_SIZE; row++)
    {
        for (int col = 0; col < Board::BOARD_SIZE; col++)
        {
            int x = BOARD_START_X + col * CELL_SIZE;
            int y = BOARD_START_Y + row * CELL_SIZE;

            SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};

            // Fill square with premium color
            const auto &square = board.getSquare(row, col);
            SDL_Color color = getPremiumSquareColor(square.premium);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);

            // Draw border
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            // Render tile if present
            if (square.isOccupied())
            {
                char letter = square.tile->letter;
                if (letter == ' ')
                    letter = '?'; // Show blank tiles as ?

                std::string letterStr(1, letter);
                textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});

                // Show points in corner
                std::string pointsStr = std::to_string(square.tile->points);
                textRenderer.renderText(pointsStr, x + CELL_SIZE - 15, y + CELL_SIZE - 15, {0, 0, 0, 255});
            }
        }
    }
}

void Game::renderPlayerHand()
{
    int handStartY = BOARD_START_Y + Board::BOARD_SIZE * CELL_SIZE + 50;

    // Current player indicator
    std::string playerText = players[currentPlayer].getName() + "'s Turn (Click to place tile)";
    textRenderer.renderText(playerText, BOARD_START_X, handStartY - 30, {255, 255, 255, 255});

    // Render hand tiles
    const auto &hand = players[currentPlayer].getHand();
    for (size_t i = 0; i < hand.size(); i++)
    {
        int x = BOARD_START_X + i * (CELL_SIZE + 5);
        int y = handStartY;

        SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};

        // highlight selected tile
        if (static_cast<int>(i) == selectedTileIndex && isDragging)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255); // Yellow highlight
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 240, 220, 180, 255); // Normal color
        }

        // Tile background
        // SDL_SetRenderDrawColor(renderer, 240, 220, 180, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // skip rendering the dragged tile
        if (static_cast<int>(i) == selectedTileIndex && isDragging)
        {
            continue;
        }

        // Letter
        char letter = hand[i]->letter;
        if (letter == ' ')
            letter = '?';

        std::string letterStr(1, letter);
        textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});

        // Points
        std::string pointsStr = std::to_string(hand[i]->points);
        textRenderer.renderText(pointsStr, x + CELL_SIZE - 15, y + CELL_SIZE - 15, {0, 0, 0, 255});

        i++;
    }
}

void Game::renderGameInfo()
{
    int infoX = BOARD_START_X + Board::BOARD_SIZE * CELL_SIZE + 50;
    int infoY = BOARD_START_Y;

    // Scores
    for (size_t i = 0; i < players.size(); i++)
    {
        std::string scoreText = players[i].getName() + ": " + std::to_string(players[i].getScore());
        SDL_Color color = (i == currentPlayer) ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 255, 255, 255};
        textRenderer.renderText(scoreText, infoX, infoY + i * 30, color);
    }

    // Tiles remaining
    std::string tilesText = "Tiles remaining: " + std::to_string(tileBag.remainingTiles());
    textRenderer.renderText(tilesText, infoX, infoY + 100, {255, 255, 255, 255});
}

SDL_Color Game::getPremiumSquareColor(Board::Premium premium)
{
    switch (premium)
    {
    case Board::START:
        return {255, 0, 0, 255}; // Red
    case Board::TRIPLE_WORD:
        return {255, 100, 100, 255}; // Light red
    case Board::DOUBLE_WORD:
        return {255, 200, 200, 255}; // Pink
    case Board::TRIPLE_LETTER:
        return {100, 100, 255, 255}; // Blue
    case Board::DOUBLE_LETTER:
        return {200, 200, 255, 255}; // Light blue
    default:
        return {240, 240, 240, 255}; // Light gray
    }
}

void Game::cleanup()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}
