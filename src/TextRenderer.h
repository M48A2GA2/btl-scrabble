#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>

class TextRenderer
{
private:
    TTF_Font *font;                                    // Font used for rendering text
    SDL_Renderer *renderer;                            // Renderer used for creating textures
    std::unordered_map<std::string, SDL_Texture *> textCache;  // Cache for rendered text textures

public:
    /**
     * Default constructor
     */
    TextRenderer();

    /**
     * Destructor - cleans up font and cached textures
     */
    ~TextRenderer();

    /**
     * Initialize the text renderer with a font
     * @param sdlRenderer The SDL renderer to use
     * @param fontPath Path to the font file
     * @param fontSize Size of the font
     * @return true if initialization successful, false otherwise
     */
    bool initialize(SDL_Renderer *sdlRenderer, const std::string &fontPath, int fontSize);

    /**
     * Render text at a specific position
     * @param text The text to render
     * @param x X coordinate for rendering
     * @param y Y coordinate for rendering
     * @param color Color of the text (default: black)
     */
    void renderText(const std::string &text, int x, int y, SDL_Color color = {0, 0, 0, 255});

    /**
     * Render text centered within a rectangle
     * @param text The text to render
     * @param x X coordinate of the rectangle
     * @param y Y coordinate of the rectangle
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @param color Color of the text (default: black)
     */
    void renderCenteredText(const std::string &text, int x, int y, int width, int height, SDL_Color color = {0, 0, 0, 255});

    /**
     * Clear all cached text textures
     * Should be called when font or renderer changes
     */
    void clearCache();
};

#endif
