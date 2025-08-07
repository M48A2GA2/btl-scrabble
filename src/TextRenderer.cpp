#include "TextRenderer.h"
#include <iostream>
#include <vector>

TextRenderer::TextRenderer() : font(nullptr), renderer(nullptr) {}

TextRenderer::~TextRenderer()
{
    clearCache();
    if (font)
    {
        TTF_CloseFont(font);
    }
}

bool TextRenderer::initialize(SDL_Renderer *sdlRenderer, const std::string &fontPath, int fontSize)
{
    if (!sdlRenderer)
    {
        std::cerr << "ERROR: Renderer is null in TextRenderer::initialize!\n";
        return false;
    }

    renderer = sdlRenderer;

    // Try multiple font paths with better error handling
    const std::vector<std::string> fontPaths = {
        fontPath,
        "assets/fonts/DejaVuSans.ttf",
        "./assets/fonts/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf", // Common Linux path
        "/System/Library/Fonts/Arial.ttf",        // macOS
        "C:/Windows/Fonts/arial.ttf"              // Windows
    };

    for (const auto &path : fontPaths)
    {
        if (path.empty())
            continue;

        font = TTF_OpenFont(path.c_str(), fontSize);
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

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
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

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
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
