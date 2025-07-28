#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), running(false),
               textRenderer(nullptr), currentPlayer(0) {}

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
        }
    }
}

void Game::update()
{
    // Game logic updates will go here
}

void Game::render()
{
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    renderBoard();
    renderPlayerHand();
    renderGameInfo();

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
    std::string playerText = players[currentPlayer].getName() + "'s Turn (Press SPACE to switch)";
    textRenderer.renderText(playerText, BOARD_START_X, handStartY - 30, {255, 255, 255, 255});

    // Render hand tiles
    const auto &hand = players[currentPlayer].getHand();
    for (size_t i = 0; i < hand.size(); i++)
    {
        int x = BOARD_START_X + i * (CELL_SIZE + 5);
        int y = handStartY;

        SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};

        // Tile background
        SDL_SetRenderDrawColor(renderer, 240, 220, 180, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

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
