#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>
#include <string>

Game::Game() : window(nullptr), renderer(nullptr), running(false),
               currentPlayer(0), selectedTileIndex(-1), isDragging(false),
               mouseX(0), mouseY(0), placementDirection(0), isPlacingWord(false),
               hasSetDirection(false), gameState(GameState::PLAYER_SELECTION), playerCount(2),
               currentWindowWidth(WINDOW_WIDTH), currentWindowHeight(WINDOW_HEIGHT) {}

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

    if (TTF_Init() == -1)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Scrabble",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

    // if (!textRenderer.initialize(renderer, "", 24)) {
    //     std::cerr << "Failed to initialize text renderer" << std::endl;
    //     return false;
    // }

    if (!textRenderer.initialize(renderer, "assets/fonts/DejaVuSans.ttf", 16))
    {
        addLog("Failed to initialize text renderer");
        return false;
    }

    // Print dictionary statistics
    dictionary.printStats();

    addLog("Use number keys 2-4 to select player count, then press ENTER");

    running = true;
    return true;
}

void Game::initializePlayers()
{
    players.clear();

    for (int i = 0; i < playerCount; i++)
    {
        players.emplace_back("Player " + std::to_string(i + 1));
    }

    addLog("Game started with " + std::to_string(playerCount) + " players");
}

int Game::getPlayerCount() const
{
    return playerCount;
}

void Game::dealInitialTiles()
{
    bool anyBlankTiles = false;
    for (auto &player : players)
    {
        while (player.hasEmptySlots() && !tileBag.isEmpty())
        {
            auto tile = tileBag.drawTile();
            if (tile && tile->letter == ' ')
            {
                anyBlankTiles = true;
            }
            player.addTile(std::move(tile));
        }
    }

    addLog("Initial tiles dealt to all players");
    if (anyBlankTiles)
    {
        addLog("Info: '?' tiles are blank tiles (worth 0 points, can represent any letter)");
    }
    addLog("Duplicates are normal - Scrabble has multiple copies of common letters");
}

void Game::run()
{
    while (running)
    {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // 60 FPS
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
            if (gameState == GameState::PLAYER_SELECTION)
            {
                if (event.key.keysym.sym >= SDLK_2 && event.key.keysym.sym <= SDLK_4)
                {
                    playerCount = event.key.keysym.sym - SDLK_0;
                    addLog("Selected " + std::to_string(playerCount) + " players. Press ENTER to start.");
                }
                else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                {
                    gameState = GameState::PLAYING;
                    addLog("Starting game with " + std::to_string(playerCount) + " players!");
                    initializePlayers();
                    dealInitialTiles();
                }
            }
            else if (gameState == GameState::PLAYING)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if (isPlacingWord)
                    {
                        cancelWordPlacement();
                    }
                    else
                    {
                        running = false;
                    }
                }
                else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                {
                    if (isPlacingWord)
                    {
                        commitWord();
                    }
                }
                else if (event.key.keysym.sym == SDLK_SPACE)
                {
                    if (!isPlacingWord)
                    {
                        // Pass turn
                        passTurn();
                    }
                }
                else if (event.key.keysym.sym == SDLK_p)
                {
                    if (!isPlacingWord)
                    {
                        // Pass turn
                        passTurn();
                    }
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (gameState == GameState::PLAYING && event.button.button == SDL_BUTTON_LEFT)
            {
                handleMouseClick(event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (gameState == GameState::PLAYING && event.button.button == SDL_BUTTON_LEFT)
            {
                handleMouseRelease(event.button.x, event.button.y);
            }
            break;
        case SDL_MOUSEMOTION:
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                handleWindowResize(event.window.data1, event.window.data2);
            }
            break;
        }
    }
}

void Game::handleMouseClick(int x, int y)
{
    int tileIndex = getTileIndexAtPosition(x, y);
    if (tileIndex != -1)
    {
        const auto &hand = players[currentPlayer].getHand();
        if (tileIndex < static_cast<int>(hand.size()))
        {
            selectedTileIndex = tileIndex;
            isDragging = true;
            addLog("Selected tile: " + std::string(1, hand[tileIndex]->letter));
        }
    }
}

void Game::handleMouseRelease(int x, int y)
{
    if (isDragging && selectedTileIndex != -1)
    {
        int row, col;
        if (getBoardPosition(x, y, row, col))
        {
            // Check if clicking on a pending placement to remove it
            if (isPlacingWord)
            {
                for (size_t i = 0; i < pendingPlacements.size(); i++)
                {
                    if (pendingPlacements[i].first == row && pendingPlacements[i].second == col)
                    {
                        // Remove this pending placement
                        addLog("Removed tile from (" + std::to_string(row) + ", " + std::to_string(col) + ")");
                        pendingPlacements.erase(pendingPlacements.begin() + i);
                        pendingTileIndices.erase(pendingTileIndices.begin() + i);

                        // Reset direction if removing all tiles
                        if (pendingPlacements.empty())
                        {
                            cancelWordPlacement();
                        }
                        else if (pendingPlacements.size() == 1)
                        {
                            // Reset direction if only one tile left
                            hasSetDirection = false;
                        }

                        selectedTileIndex = -1;
                        isDragging = false;
                        return;
                    }
                }

                // Add to current word
                if (canAddTileToWord(row, col))
                {
                    addTileToWord(row, col, selectedTileIndex);
                }
                else
                {
                    addLog("Cannot add tile there - violates word placement rules");
                }
            }
            else
            {
                // Start new word
                if (isValidMove(row, col))
                {
                    startWordPlacement();
                    addTileToWord(row, col, selectedTileIndex);
                }
                else
                {
                    if (board.isEmpty())
                    {
                        addLog("First tile must be placed on the center star!");
                    }
                    else
                    {
                        addLog("Tiles must connect to existing words on the board");
                    }
                }
            }
        }
    }

    // Reset dragging state
    selectedTileIndex = -1;
    isDragging = false;
}

void Game::handleWindowResize(int width, int height)
{
    currentWindowWidth = width;
    currentWindowHeight = height;

    // Ensure minimum window size to keep the game playable
    const int minWidth = 800;
    const int minHeight = 600;

    if (width < minWidth || height < minHeight)
    {
        int newWidth = std::max(width, minWidth);
        int newHeight = std::max(height, minHeight);
        SDL_SetWindowSize(window, newWidth, newHeight);
        currentWindowWidth = newWidth;
        currentWindowHeight = newHeight;
    }

    // Update window title to show current dimensions
    std::string title = "Scrabble - " + std::to_string(currentWindowWidth) + "x" + std::to_string(currentWindowHeight);
    SDL_SetWindowTitle(window, title.c_str());

    // Don't log window resize to avoid spam in game log
}

void Game::addLog(const std::string &message)
{
    gameLog.push_back(message);

    // Calculate maximum log entries based on window height
    int availableHeight = currentWindowHeight - (BOARD_START_Y + 250); // Reserve space for other UI
    int lineHeight = 25;                                               // Increased from 20 for better spacing
    int maxLogEntries = std::max(5, availableHeight / lineHeight);     // At least 5 entries

    if (gameLog.size() > static_cast<size_t>(maxLogEntries))
    {
        gameLog.erase(gameLog.begin());
    }
    // Removed terminal logging - log only appears in game window now
}

void Game::validateTileIndices()
{
    const auto &hand = players[currentPlayer].getHand();
    int handSize = static_cast<int>(hand.size());

    // Reset selectedTileIndex if it's invalid
    if (selectedTileIndex >= handSize || selectedTileIndex < 0)
    {
        selectedTileIndex = -1;
        isDragging = false;
    }

    // Clean up pendingTileIndices
    for (int i = static_cast<int>(pendingTileIndices.size()) - 1; i >= 0; i--)
    {
        if (pendingTileIndices[i] >= handSize || pendingTileIndices[i] < 0)
        {
            pendingTileIndices.erase(pendingTileIndices.begin() + i);
            if (i < static_cast<int>(pendingPlacements.size()))
            {
                pendingPlacements.erase(pendingPlacements.begin() + i);
            }
        }
    }

    // Reset word placement if no valid tiles remain
    if (pendingTileIndices.empty())
    {
        isPlacingWord = false;
        hasSetDirection = false;
        placementDirection = 0;
    }
}

bool Game::isValidMove(int row, int col) const
{
    // Must be an empty square
    if (!board.canPlaceTile(row, col))
        return false;

    // First move must go through center (7,7)
    if (board.isEmpty())
    {
        return (row == 7 && col == 7);
    }

    // Subsequent moves must connect to existing tiles
    return board.isAdjacentToExistingTile(row, col);
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
        const auto &hand = players[currentPlayer].getHand();
        if (selectedTileIndex >= static_cast<int>(hand.size()))
        {
            // Invalid index, reset dragging state
            isDragging = false;
            selectedTileIndex = -1;
            return;
        }

        const auto &tile = hand[selectedTileIndex];

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
    // Dark green background for better visibility
    SDL_SetRenderDrawColor(renderer, 34, 80, 34, 255);
    SDL_RenderClear(renderer);

    if (gameState == GameState::PLAYER_SELECTION)
    {
        renderPlayerSelection();
    }
    else if (gameState == GameState::PLAYING)
    {
        renderBoard();
        renderPendingPlacements();
        renderPlayerHand();
        renderGameInfo();
        renderDraggedTile();
    }

    renderLog();
    SDL_RenderPresent(renderer);
}

void Game::renderBoard()
{
    for (int row = 0; row < Board::BOARD_SIZE; row++)
    {
        for (int col = 0; col < Board::BOARD_SIZE; col++)
        {
            int x = BOARD_START_X + col * CELL_SIZE;
            int y = BOARD_START_Y + row * CELL_SIZE;

            SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};

            const auto &square = board.getSquare(row, col);
            SDL_Color color = getPremiumSquareColor(square.premium);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            if (square.isOccupied())
            {
                // Render placed tiles with light tile background
                SDL_SetRenderDrawColor(renderer, 240, 220, 180, 255); // Tile color
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &rect);

                char letter = square.tile->letter;
                if (letter == ' ')
                    letter = '?';

                std::string letterStr(1, letter);
                textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255}); // Black text for visibility

                std::string pointsStr = std::to_string(square.tile->points);
                textRenderer.renderText(pointsStr, x + CELL_SIZE - 15, y + CELL_SIZE - 15, {0, 0, 0, 255});
            }
            else
            {
                // Render premium square labels ONLY for empty squares
                std::string premiumText = getPremiumSquareText(square.premium);
                if (!premiumText.empty())
                {
                    textRenderer.renderCenteredText(premiumText, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});
                }
            }
        }
    }
}

void Game::renderPendingPlacements()
{
    if (!isPlacingWord || pendingPlacements.empty())
        return;

    // Highlight pending tile positions
    for (size_t i = 0; i < pendingPlacements.size(); i++)
    {
        int row = pendingPlacements[i].first;
        int col = pendingPlacements[i].second;
        int tileIndex = pendingTileIndices[i];

        int x = BOARD_START_X + col * CELL_SIZE;
        int y = BOARD_START_Y + row * CELL_SIZE;

        // Semi-transparent tile preview
        SDL_Rect rect = {x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4};
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 150); // Yellow highlight
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Show tile letter
        const auto &hand = players[currentPlayer].getHand();
        if (tileIndex >= 0 && tileIndex < static_cast<int>(hand.size()))
        {
            char letter = hand[tileIndex]->letter;
            if (letter == ' ')
                letter = '?';

            std::string letterStr(1, letter);
            textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});
        }
    }
}

void Game::renderPlayerHand()
{
    int handStartY = BOARD_START_Y + Board::BOARD_SIZE * CELL_SIZE + 50;

    // Current player indicator
    std::string playerText = players[currentPlayer].getName() + "'s Turn (Click to place tile)";
    textRenderer.renderText(playerText, BOARD_START_X, handStartY - 30, {220, 220, 220, 255}); // Light gray text

    // Render hand tiles
    const auto &hand = players[currentPlayer].getHand();
    for (size_t i = 0; i < hand.size(); i++)
    {
        int x = BOARD_START_X + i * (CELL_SIZE + 5);
        int y = handStartY;

        SDL_Rect rect = {x, y, CELL_SIZE, CELL_SIZE};

        // highlight selected tile
        bool isPendingPlacement = false;
        if (isPlacingWord)
        {
            // Check if this tile is part of a pending placement
            for (size_t j = 0; j < pendingTileIndices.size(); j++)
            {
                if (pendingTileIndices[j] == static_cast<int>(i))
                {
                    isPendingPlacement = true;
                    break;
                }
            }
        }

        if (static_cast<int>(i) == selectedTileIndex && isDragging)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255); // Yellow highlight
        }
        else if (isPendingPlacement)
        {
            SDL_SetRenderDrawColor(renderer, 150, 255, 150, 255); // Green tint for pending tiles
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 240, 220, 180, 255); // Normal color
        }

        // Tile background
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // Always render letters, don't skip pending tiles
        // Removed the skip logic for dragged tiles so they remain visible

        // Letter
        char letter = hand[i]->letter;
        if (letter == ' ')
            letter = '?';

        std::string letterStr(1, letter);
        textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});

        // Points
        std::string pointsStr = std::to_string(hand[i]->points);
        textRenderer.renderText(pointsStr, x + CELL_SIZE - 15, y + CELL_SIZE - 15, {0, 0, 0, 255});

        // i++;

        // Current player
        // std::string statusText = players[currentPlayer].getName() + "'s Turn";
        // if (isPlacingWord)
        // {
        //     statusText += " - Placing word (ENTER to commit, ESC to cancel)";
        // }
        // else
        // {
        //     statusText += " - Click tiles to place them";
        // }
        // textRenderer.renderText(statusText, BOARD_START_X, handStartY - 30, {255, 255, 255, 255});
    }
}

void Game::renderGameInfo()
{
    int infoX = BOARD_START_X + Board::BOARD_SIZE * CELL_SIZE + 50;
    int infoY = BOARD_START_Y;

    // Ensure info panel is visible even in smaller windows
    if (infoX >= currentWindowWidth - 200)
    {
        infoX = currentWindowWidth - 250; // Fallback position
    }

    // Scores
    for (size_t i = 0; i < players.size(); i++)
    {
        std::string scoreText = players[i].getName() + ": " + std::to_string(players[i].getScore());
        SDL_Color color = (i == currentPlayer) ? SDL_Color{255, 220, 100, 255} : SDL_Color{200, 200, 200, 255};
        textRenderer.renderText(scoreText, infoX, infoY + i * 30, color);
    }

    // Tiles remaining
    std::string tilesText = "Tiles remaining: " + std::to_string(tileBag.remainingTiles());
    textRenderer.renderText(tilesText, infoX, infoY + 100, {200, 200, 200, 255}); // Light gray

    // Controls
    int controlsY = infoY + 130;
    textRenderer.renderText("Controls:", infoX, controlsY, {255, 220, 100, 255});
    textRenderer.renderText("ENTER - Commit word", infoX, controlsY + 20, {180, 180, 180, 255});
    textRenderer.renderText("ESC - Cancel/Quit", infoX, controlsY + 40, {180, 180, 180, 255});
    textRenderer.renderText("SPACE/P - Pass turn", infoX, controlsY + 60, {180, 180, 180, 255});
}

void Game::renderLog()
{
    // Position log in the right side of the window with responsive positioning
    int logX = BOARD_START_X + Board::BOARD_SIZE * CELL_SIZE + 50;
    int logY = BOARD_START_Y + 200;
    int logWidth = currentWindowWidth - logX - 20; // Leave 20px margin from right edge

    // Ensure log area is visible even in smaller windows
    if (logX >= currentWindowWidth - 200)
    {
        logX = currentWindowWidth - 250; // Fallback position
        logWidth = 230;
    }

    // Header with better styling
    textRenderer.renderText("Game Log:", logX, logY, {255, 220, 100, 255}); // Gold header

    // Draw a subtle separator line under the header
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_Rect separatorRect = {logX, logY + 20, logWidth, 1};
    SDL_RenderFillRect(renderer, &separatorRect);

    // Render log entries with improved spacing
    int lineHeight = 25;    // Increased spacing between lines
    int startY = logY + 30; // Start below header and separator

    for (size_t i = 0; i < gameLog.size(); ++i)
    {
        int currentY = startY + i * lineHeight;

        // Only render if the line is visible within the window
        if (currentY + lineHeight <= currentWindowHeight - 20)
        {
            // Add subtle bullet points for better readability
            textRenderer.renderText("• " + gameLog[i], logX, currentY, {220, 220, 220, 255}); // Light gray log text
        }
    }
}

void Game::startWordPlacement()
{
    isPlacingWord = true;
    hasSetDirection = false;
    pendingPlacements.clear();
    pendingTileIndices.clear();
    addLog("Started word placement mode");
}

bool Game::canAddTileToWord(int row, int col)
{
    // Must be empty square
    if (board.getSquare(row, col).isOccupied())
    {
        addLog("Cannot place tile: Square is occupied");
        return false;
    }

    // First tile can go anywhere valid
    if (pendingPlacements.empty())
    {
        bool valid = isValidMove(row, col);
        if (!valid)
        {
            if (board.isEmpty())
            {
                addLog("First tile must be placed on center star (7,7)");
            }
            else
            {
                addLog("Tile must connect to existing words");
            }
        }
        return valid;
    }

    // Subsequent tiles must be in line with first tile
    int firstRow = pendingPlacements[0].first;
    int firstCol = pendingPlacements[0].second;

    if (!hasSetDirection)
    {
        // Second tile determines direction - must be adjacent in a line
        bool sameRow = (row == firstRow);
        bool sameCol = (col == firstCol);
        bool validDirection = (sameRow || sameCol) && !(sameRow && sameCol);

        if (!validDirection)
        {
            addLog("Second tile must be in same row or column as first tile");
        }

        return validDirection;
    }

    // Must continue in the established direction
    if (placementDirection == 0)
    { // Horizontal
        bool valid = (row == firstRow);
        if (!valid)
        {
            addLog("Must continue placing horizontally in row " + std::to_string(firstRow));
        }
        return valid;
    }
    else
    { // Vertical
        bool valid = (col == firstCol);
        if (!valid)
        {
            addLog("Must continue placing vertically in column " + std::to_string(firstCol));
        }
        return valid;
    }
}

void Game::addTileToWord(int row, int col, int tileIndex)
{
    // Check if this tile index is already being used in pending placements
    for (size_t i = 0; i < pendingTileIndices.size(); i++)
    {
        if (pendingTileIndices[i] == tileIndex)
        {
            addLog("Cannot use the same tile twice in one word");
            return;
        }
    }

    pendingPlacements.push_back({row, col});
    pendingTileIndices.push_back(tileIndex);

    // Set direction after second tile
    if (pendingPlacements.size() == 2 && !hasSetDirection)
    {
        int firstRow = pendingPlacements[0].first;
        // int firstCol = pendingPlacements[0].second;

        placementDirection = (row == firstRow) ? 0 : 1; // 0 = horizontal, 1 = vertical
        hasSetDirection = true;

        addLog("Direction set to: " + std::string(placementDirection == 0 ? "horizontal" : "vertical"));
    }

    addLog("Added tile to word at (" + std::to_string(row) + ", " + std::to_string(col) + ")");
}

void Game::commitWord()
{
    if (!validateWordPlacement())
    {
        cancelWordPlacement();
        return;
    }

    // Calculate formed words BEFORE placing tiles (while pendingTileIndices is still valid)
    auto words = getFormedWords();
    int totalScore = 0;
    for (const auto &word : words)
    {
        addLog("Formed word: " + word);
        int wordScore = calculateWordScore(word);
        totalScore += wordScore;
        addLog("Word '" + word + "' scored: " + std::to_string(wordScore));
    }

    // Bonus for using all 7 tiles (50 points)
    if (pendingPlacements.size() == 7)
    {
        totalScore += 50;
        addLog("Bonus for using all tiles: +50");
    }

    // Create a list of indices to remove, sorted in descending order
    // This prevents index shifting when removing tiles
    std::vector<std::pair<int, std::pair<int, int>>> indexedPlacements;
    for (size_t i = 0; i < pendingPlacements.size(); i++)
    {
        indexedPlacements.push_back({pendingTileIndices[i], pendingPlacements[i]});
    }

    // Sort by tile index in descending order
    std::sort(indexedPlacements.begin(), indexedPlacements.end(),
              [](const auto &a, const auto &b)
              { return a.first > b.first; });

    // Remove tiles from hand (in descending index order) and place on board
    for (const auto &indexedPlacement : indexedPlacements)
    {
        int tileIndex = indexedPlacement.first;
        int row = indexedPlacement.second.first;
        int col = indexedPlacement.second.second;

        auto tile = players[currentPlayer].removeTile(tileIndex);
        if (tile)
        {
            board.placeTile(row, col, std::move(tile));
            addLog("Placed tile at (" + std::to_string(row) + ", " + std::to_string(col) + ")");
        }
        else
        {
            addLog("Error: Could not remove tile at index " + std::to_string(tileIndex));
        }
    }

    // Validate tile indices after removing all tiles
    validateTileIndices();

    players[currentPlayer].addScore(totalScore);
    addLog("Total score added: " + std::to_string(totalScore));

    // Draw new tiles
    int tilesDrawn = 0;
    int blankTilesDrawn = 0;
    while (players[currentPlayer].hasEmptySlots() && !tileBag.isEmpty())
    {
        auto newTile = tileBag.drawTile();
        if (newTile)
        {
            if (newTile->letter == ' ')
            {
                blankTilesDrawn++;
            }
            players[currentPlayer].addTile(std::move(newTile));
            tilesDrawn++;
        }
    }

    if (tilesDrawn > 0)
    {
        addLog("Drew " + std::to_string(tilesDrawn) + " new tile(s)");
        if (blankTilesDrawn > 0)
        {
            addLog("Note: '?' tiles are blank tiles (can be any letter)");
        }
    }

    // End turn
    currentPlayer = (currentPlayer + 1) % players.size();
    isPlacingWord = false;
    pendingPlacements.clear();
    pendingTileIndices.clear();
}

void Game::cancelWordPlacement()
{
    addLog("Cancelled word placement");
    isPlacingWord = false;
    pendingPlacements.clear();
    pendingTileIndices.clear();
    hasSetDirection = false;
}

void Game::passTurn()
{
    addLog(players[currentPlayer].getName() + " passes their turn");
    currentPlayer = (currentPlayer + 1) % players.size();
    addLog("Now " + players[currentPlayer].getName() + "'s turn");
}

bool Game::validateWordPlacement()
{
    if (pendingPlacements.empty())
    {
        addLog("No tiles placed!");
        return false;
    }

    // Check if first word goes through center
    if (board.isEmpty())
    {
        bool goesThoughCenter = false;
        for (const auto &placement : pendingPlacements)
        {
            if (placement.first == 7 && placement.second == 7)
            {
                goesThoughCenter = true;
                break;
            }
        }
        if (!goesThoughCenter)
        {
            addLog("First word must go through the center star!");
            return false;
        }
    }

    // Get all formed words and validate
    auto words = getFormedWords();
    if (words.empty())
    {
        addLog("No valid words formed!");
        return false;
    }

    for (const auto &word : words)
    {
        if (word.length() < 2)
            continue; // Single letters don't need validation
        if (!dictionary.isValidWord(word))
        {
            addLog("Invalid word: " + word);
            return false;
        }
    }

    return true;
}

std::vector<std::string> Game::getFormedWords() const
{
    std::vector<std::string> words;

    if (pendingPlacements.empty() || pendingTileIndices.empty())
        return words;

    // Safety check: ensure indices are valid
    const auto &hand = players[currentPlayer].getHand();
    for (int index : pendingTileIndices)
    {
        if (index < 0 || index >= static_cast<int>(hand.size()))
        {
            // Invalid index found, return empty to prevent crash
            return words;
        }
    }

    // Get the main word being placed
    std::string mainWord = getMainWordFromPlacements();
    if (mainWord.length() > 1)
    {
        words.push_back(mainWord);
    }

    // Get cross-words formed by each new tile
    for (size_t i = 0; i < pendingPlacements.size(); i++)
    {
        if (i >= pendingTileIndices.size())
            break; // Safety check

        int row = pendingPlacements[i].first;
        int col = pendingPlacements[i].second;

        // Check perpendicular direction for cross-words
        if (placementDirection == 0)
        { // Horizontal main word
            std::string crossWord = getVerticalWordAt(row, col, pendingTileIndices[i]);
            if (crossWord.length() > 1)
            {
                words.push_back(crossWord);
            }
        }
        else
        { // Vertical main word
            std::string crossWord = getHorizontalWordAt(row, col, pendingTileIndices[i]);
            if (crossWord.length() > 1)
            {
                words.push_back(crossWord);
            }
        }
    }

    return words;
}

std::string Game::getMainWordFromPlacements() const
{
    if (pendingPlacements.empty())
        return "";

    // Sort placements to get word in order
    auto sortedPlacements = pendingPlacements;
    auto sortedIndices = pendingTileIndices;

    // Sort both vectors together
    for (size_t i = 0; i < sortedPlacements.size(); i++)
    {
        for (size_t j = i + 1; j < sortedPlacements.size(); j++)
        {
            bool shouldSwap = false;
            if (placementDirection == 0)
            { // Horizontal
                shouldSwap = sortedPlacements[i].second > sortedPlacements[j].second;
            }
            else
            { // Vertical
                shouldSwap = sortedPlacements[i].first > sortedPlacements[j].first;
            }

            if (shouldSwap)
            {
                std::swap(sortedPlacements[i], sortedPlacements[j]);
                std::swap(sortedIndices[i], sortedIndices[j]);
            }
        }
    }

    // Build complete word including existing tiles
    std::string word;
    if (placementDirection == 0)
    { // Horizontal
        int row = sortedPlacements[0].first;
        int startCol = sortedPlacements[0].second;
        int endCol = sortedPlacements.back().second;

        // Find actual start of word (include existing tiles to the left)
        while (startCol > 0 && board.getSquare(row, startCol - 1).isOccupied())
        {
            startCol--;
        }

        // Find actual end of word (include existing tiles to the right)
        while (endCol < Board::BOARD_SIZE - 1 && board.getSquare(row, endCol + 1).isOccupied())
        {
            endCol++;
        }

        // Build the complete word
        for (int col = startCol; col <= endCol; col++)
        {
            char letter = ' ';

            // Check if this position has a pending tile
            auto pendingIt = std::find(sortedPlacements.begin(), sortedPlacements.end(),
                                       std::make_pair(row, col));
            if (pendingIt != sortedPlacements.end())
            {
                int index = std::distance(sortedPlacements.begin(), pendingIt);
                const auto &hand = players[currentPlayer].getHand();
                if (index >= 0 && index < static_cast<int>(sortedIndices.size()) &&
                    sortedIndices[index] >= 0 && sortedIndices[index] < static_cast<int>(hand.size()))
                {
                    letter = hand[sortedIndices[index]]->getDisplayLetter();
                }
            }
            else if (board.getSquare(row, col).isOccupied())
            {
                letter = board.getSquare(row, col).tile->getDisplayLetter();
            }

            if (letter != ' ')
            {
                word += letter;
            }
        }
    }
    else
    { // Vertical - similar logic
        int col = sortedPlacements[0].second;
        int startRow = sortedPlacements[0].first;
        int endRow = sortedPlacements.back().first;

        // Find actual start of word
        while (startRow > 0 && board.getSquare(startRow - 1, col).isOccupied())
        {
            startRow--;
        }

        // Find actual end of word
        while (endRow < Board::BOARD_SIZE - 1 && board.getSquare(endRow + 1, col).isOccupied())
        {
            endRow++;
        }

        // Build the complete word
        for (int row = startRow; row <= endRow; row++)
        {
            char letter = ' ';

            auto pendingIt = std::find(sortedPlacements.begin(), sortedPlacements.end(),
                                       std::make_pair(row, col));
            if (pendingIt != sortedPlacements.end())
            {
                int index = std::distance(sortedPlacements.begin(), pendingIt);
                const auto &hand = players[currentPlayer].getHand();
                if (index >= 0 && index < static_cast<int>(sortedIndices.size()) &&
                    sortedIndices[index] >= 0 && sortedIndices[index] < static_cast<int>(hand.size()))
                {
                    letter = hand[sortedIndices[index]]->getDisplayLetter();
                }
            }
            else if (board.getSquare(row, col).isOccupied())
            {
                letter = board.getSquare(row, col).tile->getDisplayLetter();
            }

            if (letter != ' ')
            {
                word += letter;
            }
        }
    }

    return word;
}

std::string Game::getHorizontalWordAt(int row, int col, int tileIndex) const
{
    return buildWordFromPosition(row, col, 0, 1, tileIndex);
}

std::string Game::getVerticalWordAt(int row, int col, int tileIndex) const
{
    return buildWordFromPosition(row, col, 1, 0, tileIndex);
}

std::string Game::buildWordFromPosition(int row, int col, int deltaRow, int deltaCol, int newTileIndex) const
{
    // Find start of word
    int startRow = row, startCol = col;
    while (startRow - deltaRow >= 0 && startCol - deltaCol >= 0 &&
           startRow - deltaRow < Board::BOARD_SIZE && startCol - deltaCol < Board::BOARD_SIZE)
    {

        bool hasExistingTile = board.getSquare(startRow - deltaRow, startCol - deltaCol).isOccupied();
        bool hasPendingTile = std::find(pendingPlacements.begin(), pendingPlacements.end(),
                                        std::make_pair(startRow - deltaRow, startCol - deltaCol)) != pendingPlacements.end();

        if (!hasExistingTile && !hasPendingTile)
            break;

        startRow -= deltaRow;
        startCol -= deltaCol;
    }

    // Build word from start
    std::string word;
    int currentRow = startRow, currentCol = startCol;

    while (currentRow >= 0 && currentCol >= 0 &&
           currentRow < Board::BOARD_SIZE && currentCol < Board::BOARD_SIZE)
    {

        char letter = ' ';

        // Check if this is the new tile position
        if (currentRow == row && currentCol == col && newTileIndex >= 0)
        {
            const auto &hand = players[currentPlayer].getHand();
            if (newTileIndex < static_cast<int>(hand.size()))
            {
                letter = hand[newTileIndex]->getDisplayLetter();
            }
        }
        // Check if there's a pending tile here
        else
        {
            auto pendingIt = std::find(pendingPlacements.begin(), pendingPlacements.end(),
                                       std::make_pair(currentRow, currentCol));
            if (pendingIt != pendingPlacements.end())
            {
                int index = std::distance(pendingPlacements.begin(), pendingIt);
                const auto &hand = players[currentPlayer].getHand();
                if (index >= 0 && index < static_cast<int>(pendingTileIndices.size()) &&
                    pendingTileIndices[index] >= 0 && pendingTileIndices[index] < static_cast<int>(hand.size()))
                {
                    letter = hand[pendingTileIndices[index]]->getDisplayLetter();
                }
            }
            // Check if there's an existing tile
            else if (board.getSquare(currentRow, currentCol).isOccupied())
            {
                letter = board.getSquare(currentRow, currentCol).tile->getDisplayLetter();
            }
        }

        if (letter == ' ')
            break;

        word += letter;
        currentRow += deltaRow;
        currentCol += deltaCol;
    }

    return word;
}

int Game::calculateWordScore(const std::string &word) const
{
    // Basic tile values (official Scrabble scoring)
    const int tileValues[26] = {
        1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10
        // A, B, C, D, E, F, G, H, I, J, K, L, M,  N, O, P, Q,  R, S, T, U, V, W, X, Y, Z
    };

    int score = 0;
    for (char c : word)
    {
        if (c >= 'A' && c <= 'Z')
        {
            score += tileValues[c - 'A'];
        }
        else if (c >= 'a' && c <= 'z')
        {
            score += tileValues[c - 'a'];
        }
    }

    // For this simplified version, we return base letter values
    // In a full implementation, premium squares would be calculated
    // based on which tiles were just placed and their board positions
    return score;
}

SDL_Color Game::getPremiumSquareColor(Board::Premium premium)
{
    switch (premium)
    {
    case Board::Premium::START:
        return {255, 0, 0, 255}; // Red
    case Board::Premium::TRIPLE_WORD:
        return {255, 100, 100, 255}; // Light red
    case Board::Premium::DOUBLE_WORD:
        return {255, 200, 200, 255}; // Pink
    case Board::Premium::TRIPLE_LETTER:
        return {100, 100, 255, 255}; // Blue
    case Board::Premium::DOUBLE_LETTER:
        return {200, 200, 255, 255}; // Light blue
    default:
        return {240, 240, 240, 255}; // Light gray
    }
}

std::string Game::getPremiumSquareText(Board::Premium premium)
{
    switch (premium)
    {
    case Board::Premium::START:
        return "★";
    case Board::Premium::TRIPLE_WORD:
        return "3W";
    case Board::Premium::DOUBLE_WORD:
        return "2W";
    case Board::Premium::TRIPLE_LETTER:
        return "3L";
    case Board::Premium::DOUBLE_LETTER:
        return "2L";
    default:
        return "";
    }
}

void Game::renderPlayerSelection()
{
    int centerX = currentWindowWidth / 2;
    int centerY = currentWindowHeight / 2;

    // Title
    std::string title = "SCRABBLE";
    textRenderer.renderCenteredText(title, centerX - 100, centerY - 100, 200, 50, {255, 220, 100, 255});

    // Instructions
    std::string instruction1 = "Select Number of Players (2-4)";
    textRenderer.renderCenteredText(instruction1, centerX - 150, centerY - 40, 300, 30, {220, 220, 220, 255});

    std::string instruction2 = "Press 2, 3, or 4, then ENTER";
    textRenderer.renderCenteredText(instruction2, centerX - 120, centerY - 10, 240, 25, {200, 200, 200, 255});

    // Current selection
    std::string selection = "Current: " + std::to_string(playerCount) + " players";
    textRenderer.renderCenteredText(selection, centerX - 80, centerY + 30, 160, 25, {255, 220, 100, 255});

    // Player options
    for (int i = 2; i <= 4; i++)
    {
        std::string option = std::to_string(i) + " Players";
        SDL_Color color = (i == playerCount) ? SDL_Color{255, 255, 100, 255} : SDL_Color{180, 180, 180, 255};
        textRenderer.renderCenteredText(option, centerX - 40, centerY + 60 + (i - 2) * 30, 80, 25, color);
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
