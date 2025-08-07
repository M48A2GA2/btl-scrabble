#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <algorithm>

Game::Game() : window(nullptr), renderer(nullptr), running(false),
               currentPlayer(0), selectedTileIndex(-1), isDragging(false),
               mouseX(0), mouseY(0), placementDirection(0), isPlacingWord(false),
               hasSetDirection(false) {}

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

    // if (!textRenderer.initialize(renderer, "", 24)) {
    //     std::cerr << "Failed to initialize text renderer" << std::endl;
    //     return false;
    // }

    if (!textRenderer.initialize(renderer, "assets/fonts/DejaVuSans.ttf", 20))
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
                    // Switch players (temporary)
                    currentPlayer = (currentPlayer + 1) % players.size();
                }
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

// void Game::handleMouseRelease(int x, int y)
// {
//     // if (isDragging && selectedTileIndex != -1)
//     // {
//     //     // isDragging = false;
//     //     int row, col;
//     //     if (getBoardPosition(x, y, row, col))
//     //     {
//     //         auto tile = players[currentPlayer].removeTile(selectedTileIndex);
//     //         if (tile && board.placeTile(row, col, std::move(tile)))
//     //         {
//     //             std::cout << "Placed tile on board: " << tile->letter << std::endl;
//     //             if (!tileBag.isEmpty() && players[currentPlayer].hasEmptySlots())
//     //             {
//     //                 players[currentPlayer].addTile(tileBag.drawTile());
//     //             }
//     //         }
//     //         else
//     //         {
//     //             if (tile)
//     //             {
//     //                 players[currentPlayer].addTile(std::move(tile)); // Return tile to hand if placement failed
//     //                 std::cout << "Failed to place tile on board, returning to hand." << std::endl;
//     //             }
//     //             std::cout << "No tile selected." << std::endl;

void Game::handleMouseRelease(int x, int y)
{
    if (isDragging && selectedTileIndex != -1)
    {
        int row, col;
        if (getBoardPosition(x, y, row, col))
        {
            if (isPlacingWord)
            {
                // Add to current word
                if (canAddTileToWord(row, col))
                {
                    addTileToWord(row, col, selectedTileIndex);
                }
                else
                {
                    std::cout << "Cannot add tile there - violates word placement rules" << std::endl;
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
            }
        }
    }

    // Reset dragging state
    selectedTileIndex = -1;
    isDragging = false;
}

bool Game::isValidMove(int row, int col) const
{
    return board.canPlaceTile(row, col);
}

bool Game::validateWordsFormed(int row, int col) const
{
    // Create a single position vector for this tile
    std::vector<std::pair<int, int>> positions = {{row, col}};
    auto words = board.getWordsFormedByMove(positions);

    // Single tiles don't form words unless it's the first move
    if (words.empty() && board.isFirstMovePlayed())
    {
        return false; // Must form at least one word after first move
    }

    // Check all formed words against dictionary
    for (const auto &wordInfo : words)
    {
        if (!dictionary.isValidWord(wordInfo.word))
        {
            std::cout << "Invalid word: " << wordInfo.word << std::endl;
            return false;
        }
        else
        {
            std::cout << "Valid word: " << wordInfo.word << std::endl;
        }
    }

    return true;
}

bool Game::validateCompleteMove(const std::vector<std::pair<int, int>> &positions)
{
    if (positions.empty())
        return false;

    // Check if tiles form a continuous line
    if (!tilesFormLine(positions))
    {
        std::cout << "Tiles must form a continuous line\n";
        return false;
    }

    // Check adjacency to existing tiles (except first move)
    if (board.isFirstMovePlayed())
    {
        bool hasAdjacency = false;
        for (const auto &pos : positions)
        {
            if (board.isAdjacentToExistingTile(pos.first, pos.second))
            {
                hasAdjacency = true;
                break;
            }
        }
        if (!hasAdjacency)
        {
            std::cout << "New tiles must connect to existing tiles\n";
            return false;
        }
    }
    else
    {
        // First move must include center square
        bool includesCenter = false;
        for (const auto &pos : positions)
        {
            if (pos.first == 7 && pos.second == 7)
            {
                includesCenter = true;
                break;
            }
        }
        if (!includesCenter)
        {
            std::cout << "First move must include center square\n";
            return false;
        }
    }

    // Validate all formed words
    auto words = board.getWordsFormedByMove(positions);
    if (words.empty() && board.isFirstMovePlayed())
    {
        std::cout << "Move must form at least one word\n";
        return false;
    }

    for (const auto &wordInfo : words)
    {
        if (wordInfo.word.length() > 1 && !dictionary.isValidWord(wordInfo.word))
        {
            std::cout << "Invalid word formed: " << wordInfo.word << '\n';
            return false;
        }
    }

    return true;
}

bool Game::tilesFormLine(const std::vector<std::pair<int, int>> &positions)
{
    if (positions.size() <= 1)
        return true;

    // Sort positions
    auto sortedPos = positions;
    std::sort(sortedPos.begin(), sortedPos.end());

    // Check if all in same row
    bool sameRow = true;
    for (size_t i = 1; i < sortedPos.size(); ++i)
    {
        if (sortedPos[i].first != sortedPos[0].first)
        {
            sameRow = false;
            break;
        }
    }

    // Check if all in same column
    bool sameCol = true;
    for (size_t i = 1; i < sortedPos.size(); ++i)
    {
        if (sortedPos[i].second != sortedPos[0].second)
        {
            sameCol = false;
            break;
        }
    }

    if (!sameRow && !sameCol)
        return false;

    // Check for gaps (considering existing tiles)
    if (sameRow)
    {
        for (int col = sortedPos[0].second; col <= sortedPos.back().second; ++col)
        {
            int row = sortedPos[0].first;
            if (!board.getSquare(row, col).isOccupied())
            {
                // Check if this position is in our placement list
                bool inPlacement = std::find(positions.begin(), positions.end(),
                                             std::make_pair(row, col)) != positions.end();
                if (!inPlacement)
                {
                    return false; // Gap found
                }
            }
        }
    }
    else
    {
        for (int row = sortedPos[0].first; row <= sortedPos.back().first; ++row)
        {
            int col = sortedPos[0].second;
            if (!board.getSquare(row, col).isOccupied())
            {
                bool inPlacement = std::find(positions.begin(), positions.end(),
                                             std::make_pair(row, col)) != positions.end();
                if (!inPlacement)
                {
                    return false; // Gap found
                }
            }
        }
    }

    return true;
}

int Game::calculateWordScore(const std::vector<std::pair<int, int>> &positions,
                             const std::vector<std::unique_ptr<Tile>> &tiles)
{
    int score = 0;
    int wordMultiplier = 1;

    for (size_t i = 0; i < positions.size(); i++)
    {
        int row = positions[i].first;
        int col = positions[i].second;
        auto &square = board.getSquare(row, col);

        int letterScore = tiles[i]->points;

        // Apply premium square multipliers
        switch (square.premium)
        {
        case Board::DOUBLE_LETTER:
            letterScore *= 2;
            break;
        case Board::TRIPLE_LETTER:
            letterScore *= 3;
            break;
        case Board::DOUBLE_WORD:
            wordMultiplier *= 2;
            break;
        case Board::TRIPLE_WORD:
            wordMultiplier *= 3;
            break;
        case Board::START:
            // START square acts as a regular square for scoring
            break;
        case Board::NONE:
            // No premium bonus
            break;
        }

        score += letterScore;
    }

    return score * wordMultiplier;
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    renderBoard();
    renderPendingPlacements(); // Add this line
    renderPlayerHand();
    renderGameInfo();
    renderDraggedTile();

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
                // Render placed tiles
                char letter = square.tile->letter;
                if (letter == ' ')
                    letter = '?';

                std::string letterStr(1, letter);
                textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255}); // WHITE text for visibility

                std::string pointsStr = std::to_string(square.tile->points);
                textRenderer.renderText(pointsStr, x + CELL_SIZE - 15, y + CELL_SIZE - 15, {255, 255, 255, 255});
            }
            else
            {
                // ADD THIS: Render premium square labels for empty squares
                std::string premiumText = getPremiumSquareText(square.premium);
                if (!premiumText.empty())
                {
                    textRenderer.renderCenteredText(premiumText, x, y, CELL_SIZE, CELL_SIZE, {255, 255, 255, 255});
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
        char letter = players[currentPlayer].getHand()[tileIndex]->letter;
        if (letter == ' ')
            letter = '?';

        std::string letterStr(1, letter);
        textRenderer.renderCenteredText(letterStr, x, y, CELL_SIZE, CELL_SIZE, {0, 0, 0, 255});
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

void Game::startWordPlacement()
{
    isPlacingWord = true;
    hasSetDirection = false;
    pendingPlacements.clear();
    pendingTileIndices.clear();
    std::cout << "Started word placement mode" << std::endl;
}

bool Game::canAddTileToWord(int row, int col) const
{
    // Must be empty square
    if (board.getSquare(row, col).isOccupied())
    {
        return false;
    }

    // First tile can go anywhere valid
    if (pendingPlacements.empty())
    {
        return board.canPlaceTile(row, col);
    }

    // Subsequent tiles must be in line with first tile
    int firstRow = pendingPlacements[0].first;
    int firstCol = pendingPlacements[0].second;

    if (!hasSetDirection)
    {
        // Second tile determines direction
        return (row == firstRow || col == firstCol);
    }

    // Must continue in the established direction
    if (placementDirection == 0)
    { // Horizontal
        return row == firstRow;
    }
    else
    { // Vertical
        return col == firstCol;
    }
}

void Game::addTileToWord(int row, int col, int tileIndex)
{
    pendingPlacements.push_back({row, col});
    pendingTileIndices.push_back(tileIndex);

    // Set direction after second tile
    if (pendingPlacements.size() == 2 && !hasSetDirection)
    {
        int firstRow = pendingPlacements[0].first;
        // int firstCol = pendingPlacements[0].second;

        placementDirection = (row == firstRow) ? 0 : 1; // 0 = horizontal, 1 = vertical
        hasSetDirection = true;

        std::cout << "Direction set to: " << (placementDirection == 0 ? "horizontal" : "vertical") << std::endl;
    }

    std::cout << "Added tile to word at (" << row << ", " << col << ")" << std::endl;
}

void Game::commitWord()
{
    if (!validateWordPlacement())
    {
        cancelWordPlacement();
        return;
    }

    // Place all tiles on board
    std::vector<std::unique_ptr<Tile>> placedTiles;
    for (int i = pendingTileIndices.size() - 1; i >= 0; i--)
    {
        auto tile = players[currentPlayer].removeTile(pendingTileIndices[i]);
        placedTiles.push_back(std::move(tile));
    }

    // Place tiles in reverse order (since we removed them backwards)
    for (int i = placedTiles.size() - 1; i >= 0; i--)
    {
        int row = pendingPlacements[i].first;
        int col = pendingPlacements[i].second;
        board.placeTile(row, col, std::move(placedTiles[placedTiles.size() - 1 - i]));
    }

    // Calculate and add score
    auto words = getFormedWords();
    int totalScore = 0;
    for (const auto &word : words)
    {
        std::cout << "Formed word: " << word << std::endl;
        // Add scoring logic here
        totalScore += word.length() * 10; // Simple scoring for now
    }

    players[currentPlayer].addScore(totalScore);
    std::cout << "Score added: " << totalScore << std::endl;

    // Draw new tiles
    while (players[currentPlayer].hasEmptySlots() && !tileBag.isEmpty())
    {
        players[currentPlayer].addTile(tileBag.drawTile());
    }

    // End turn
    currentPlayer = (currentPlayer + 1) % players.size();
    isPlacingWord = false;
    pendingPlacements.clear();
    pendingTileIndices.clear();
}

void Game::cancelWordPlacement()
{
    std::cout << "Cancelled word placement" << std::endl;
    isPlacingWord = false;
    pendingPlacements.clear();
    pendingTileIndices.clear();
    hasSetDirection = false;
}

bool Game::validateWordPlacement() const
{
    if (pendingPlacements.empty())
        return false;

    // Get all formed words and validate
    auto words = getFormedWords();
    for (const auto &word : words)
    {
        if (word.length() < 2)
            continue; // Single letters don't need validation
        if (!dictionary.isValidWord(word))
        {
            std::cout << "Invalid word: " << word << std::endl;
            return false;
        }
    }

    return true;
}

std::vector<std::string> Game::getFormedWords() const
{
    std::vector<std::string> words;

    if (pendingPlacements.empty())
        return words;

    // Get the main word being placed
    std::string mainWord = getMainWordFromPlacements();
    if (mainWord.length() > 1)
    {
        words.push_back(mainWord);
    }

    // Get cross-words formed by each new tile
    for (size_t i = 0; i < pendingPlacements.size(); i++)
    {
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
                letter = players[currentPlayer].getHand()[sortedIndices[index]]->getDisplayLetter();
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
                letter = players[currentPlayer].getHand()[sortedIndices[index]]->getDisplayLetter();
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
            letter = players[currentPlayer].getHand()[newTileIndex]->getDisplayLetter();
        }
        // Check if there's a pending tile here
        else
        {
            auto pendingIt = std::find(pendingPlacements.begin(), pendingPlacements.end(),
                                       std::make_pair(currentRow, currentCol));
            if (pendingIt != pendingPlacements.end())
            {
                int index = std::distance(pendingPlacements.begin(), pendingIt);
                letter = players[currentPlayer].getHand()[pendingTileIndices[index]]->getDisplayLetter();
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

bool Game::hasGapsInWordPlacement() const
{
    if (pendingPlacements.size() <= 1)
        return false;

    auto sortedPos = pendingPlacements;
    std::sort(sortedPos.begin(), sortedPos.end());

    if (placementDirection == 0)
    { // Horizontal
        int row = sortedPos[0].first;
        for (int col = sortedPos[0].second; col <= sortedPos.back().second; ++col)
        {
            // Check if position has a tile (either placed or pending)
            bool hasPlacedTile = board.getSquare(row, col).isOccupied();
            bool hasPendingTile = std::find(pendingPlacements.begin(), pendingPlacements.end(),
                                            std::make_pair(row, col)) != pendingPlacements.end();

            if (!hasPlacedTile && !hasPendingTile)
            {
                std::cout << "Gap found at position (" << row << ", " << col << ")" << std::endl;
                return true; // Gap found
            }
        }
    }
    else
    { // Vertical
        int col = sortedPos[0].second;
        for (int row = sortedPos[0].first; row <= sortedPos.back().first; ++row)
        {
            bool hasPlacedTile = board.getSquare(row, col).isOccupied();
            bool hasPendingTile = std::find(pendingPlacements.begin(), pendingPlacements.end(),
                                            std::make_pair(row, col)) != pendingPlacements.end();

            if (!hasPlacedTile && !hasPendingTile)
            {
                std::cout << "Gap found at position (" << row << ", " << col << ")" << std::endl;
                return true; // Gap found
            }
        }
    }

    return false;
}

int Game::calculateCompleteWordScore(const std::vector<std::pair<int, int>> &positions)
{
    if (positions.empty())
        return 0;

    int totalScore = 0;

    // Calculate main word score
    int mainWordScore = 0;
    int wordMultiplier = 1;

    // Get the complete main word
    std::string mainWord = getMainWordFromPlacements();

    // Calculate score for each position in the main word
    for (const auto &pos : positions)
    {
        int row = pos.first;
        int col = pos.second;
        auto &square = board.getSquare(row, col);

        // Get tile points
        int letterScore = 0;
        auto pendingIt = std::find(pendingPlacements.begin(), pendingPlacements.end(), pos);
        if (pendingIt != pendingPlacements.end())
        {
            int index = std::distance(pendingPlacements.begin(), pendingIt);
            letterScore = players[currentPlayer].getHand()[pendingTileIndices[index]]->points;

            // Apply premium squares only for newly placed tiles
            switch (square.premium)
            {
            case Board::DOUBLE_LETTER:
                letterScore *= 2;
                std::cout << "Double letter score applied at (" << row << ", " << col << ")" << std::endl;
                break;
            case Board::TRIPLE_LETTER:
                letterScore *= 3;
                std::cout << "Triple letter score applied at (" << row << ", " << col << ")" << std::endl;
                break;
            case Board::DOUBLE_WORD:
                wordMultiplier *= 2;
                std::cout << "Double word score applied at (" << row << ", " << col << ")" << std::endl;
                break;
            case Board::TRIPLE_WORD:
                wordMultiplier *= 3;
                std::cout << "Triple word score applied at (" << row << ", " << col << ")" << std::endl;
                break;
            default:
                break;
            }
        }

        mainWordScore += letterScore;
    }

    // Apply word multiplier to main word
    mainWordScore *= wordMultiplier;
    totalScore += mainWordScore;

    std::cout << "Main word '" << mainWord << "' scores " << mainWordScore << " points" << std::endl;

    // Add cross-word scores
    int crossWordScore = calculateCrossWordScores();
    totalScore += crossWordScore;

    // Bingo bonus: +50 points for using all 7 tiles
    if (pendingPlacements.size() == 7)
    {
        totalScore += 50;
        std::cout << "BINGO! +50 bonus points for using all tiles!" << std::endl;
    }

    return totalScore;
}

int Game::calculateCrossWordScores() const
{
    int totalCrossScore = 0;

    for (size_t i = 0; i < pendingPlacements.size(); i++)
    {
        int row = pendingPlacements[i].first;
        int col = pendingPlacements[i].second;

        std::string crossWord;
        int crossScore = 0;
        int crossWordMultiplier = 1;

        // Get cross-word in perpendicular direction
        if (placementDirection == 0)
        { // Main word is horizontal, check vertical
            crossWord = getVerticalWordAt(row, col, pendingTileIndices[i]);
        }
        else
        { // Main word is vertical, check horizontal
            crossWord = getHorizontalWordAt(row, col, pendingTileIndices[i]);
        }

        // Only score cross-words that are longer than 1 letter
        if (crossWord.length() > 1)
        {
            // Calculate cross-word score
            std::vector<std::pair<int, int>> crossPositions;

            // This is a simplified version - in a complete implementation,
            // you'd need to get all positions of the cross-word
            auto &square = board.getSquare(row, col);
            int letterScore = players[currentPlayer].getHand()[pendingTileIndices[i]]->points;

            // Apply premium squares
            switch (square.premium)
            {
            case Board::DOUBLE_LETTER:
                letterScore *= 2;
                break;
            case Board::TRIPLE_LETTER:
                letterScore *= 3;
                break;
            case Board::DOUBLE_WORD:
                crossWordMultiplier *= 2;
                break;
            case Board::TRIPLE_WORD:
                crossWordMultiplier *= 3;
                break;
            default:
                break;
            }

            // For simplicity, just add the new tile's contribution
            // In a complete implementation, you'd calculate the full cross-word score
            crossScore = letterScore * crossWordMultiplier;
            totalCrossScore += crossScore;

            std::cout << "Cross-word '" << crossWord << "' scores " << crossScore << " points" << std::endl;
        }
    }

    return totalCrossScore;
}

int Game::getTilePoints(int row, int col) const
{
    if (!board.isInBounds(row, col))
        return 0;

    auto &square = board.getSquare(row, col);
    if (square.isOccupied())
    {
        return square.tile->points;
    }

    return 0;
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

std::string Game::getPremiumSquareText(Board::Premium premium)
{
    switch (premium)
    {
    case Board::START:
        return "★";
    case Board::TRIPLE_WORD:
        return "3W";
    case Board::DOUBLE_WORD:
        return "2W";
    case Board::TRIPLE_LETTER:
        return "3L";
    case Board::DOUBLE_LETTER:
        return "2L";
    default:
        return "";
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
