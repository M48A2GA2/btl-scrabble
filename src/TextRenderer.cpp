#include "TextRenderer.h"
#include <iostream>
#include <vector>
#include <array>

/**
 * Default constructor
 */
TextRenderer::TextRenderer() : font(nullptr), renderer(nullptr) {}

/**
 * Destructor - cleans up font and cached textures
 */
TextRenderer::~TextRenderer()
{
    clearCache();
    if (font)
    {
        TTF_CloseFont(font);
    }
}

/**
 * Initialize the text renderer with a font
 * 
 * @param sdlRenderer The SDL renderer to use
 * @param fontPath Path to the font file
 * @param fontSize Size of the font
 * @return true if initialization successful, false otherwise
 */
bool TextRenderer::initialize(SDL_Renderer *sdlRenderer, const std::string &fontPath, int fontSize)
{
    if (!sdlRenderer)
    {
        std::cerr << "ERROR: Renderer is null in TextRenderer::initialize!\n";
        return false;
    }

    renderer = sdlRenderer;

    // Try multiple font paths with better error handling
    constexpr std::array<std::string_view, 6> fontPaths = {
        "",
        "assets/fonts/DejaVuSans.ttf",
        "./assets/fonts/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf", // Common Linux path
        "/System/Library/Fonts/Arial.ttf",        // macOS
        "C:/Windows/Fonts/arial.ttf"              // Windows
    };

    // Try the provided path first
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (font)
    {
        std::cout << "Successfully loaded font: " << fontPath << '\n';
        return true;
    }
    else
    {
        std::cout << "Failed to load font: " << fontPath << " - " << TTF_GetError() << '\n';
    }

    // Try fallback paths
    for (const auto &path : fontPaths)
    {
        if (path.empty())
            continue;

        font = TTF_OpenFont(std::string(path).c_str(), fontSize);
        if (font)
        {
            std::cout << "Successfully loaded font: " << path << '\n';
            return true;
        }
        else
        {
            std::cout << "Failed to load font: " << path << " - " << TTF_GetError() << '\n';
        }
    }

    std::cerr << "Could not load any font. TTF_GetError: " << TTF_GetError() << '\n';
    return false;
}

/**
 * Render text at a specific position
 * 
 * @param text The text to render
 * @param x X coordinate for rendering
 * @param y Y coordinate for rendering
 * @param color Color of the text (default: black)
 */
void TextRenderer::renderText(const std::string &text, int x, int y, SDL_Color color)
{
    if (!font || !renderer || text.empty())
    {
        if (!font)
            std::cerr << "ERROR: Font is null!\n";
        if (!renderer)
            std::cerr << "ERROR: Renderer is null!\n";
        return;
    }

    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!textSurface)
    {
        std::cerr << "ERROR: Failed to create text surface: " << TTF_GetError() << '\n';
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int width = textSurface->w;
    int height = textSurface->h;
    SDL_FreeSurface(textSurface);

    if (!textTexture)
    {
        std::cerr << "ERROR: Failed to create text texture: " << SDL_GetError() << '\n';
        return;
    }

    SDL_Rect destRect = {x, y, width, height};
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    SDL_DestroyTexture(textTexture);
}

/**
 * Render text centered within a rectangle
 * 
 * @param text The text to render
 * @param x X coordinate of the rectangle
 * @param y Y coordinate of the rectangle
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 * @param color Color of the text (default: black)
 */
void TextRenderer::renderCenteredText(const std::string &text, int x, int y, int width, int height, SDL_Color color)
{
    if (!font || !renderer || text.empty())
    {
        if (!font)
            std::cerr << "ERROR: Font is null!\n";
        if (!renderer)
            std::cerr << "ERROR: Renderer is null!\n";
        return;
    }

    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!textSurface)
    {
        std::cerr << "ERROR: Failed to create text surface: " << TTF_GetError() << '\n';
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);

    if (!textTexture)
    {
        std::cerr << "ERROR: Failed to create text texture: " << SDL_GetError() << '\n';
        return;
    }

    // Calculate centered position
    int centeredX = x + (width - textWidth) / 2;
    int centeredY = y + (height - textHeight) / 2;

    SDL_Rect destRect = {centeredX, centeredY, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    SDL_DestroyTexture(textTexture);
}

/**
 * Clear all cached text textures
 * Should be called when font or renderer changes
 */
void TextRenderer::clearCache()
{
    for (auto &[key, texture] : textCache)
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
        }
    }
    textCache.clear();
}
