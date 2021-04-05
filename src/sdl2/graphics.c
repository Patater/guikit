/*
 *  graphics.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-05.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/panic.h"
#include <SDL.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define COLOR_TRANSPARENT 16

static const struct SDL_Color mac_pal[] = {
    {0x00, 0x00, 0x00, 0xFF}, /* Black */
    {0x20, 0x20, 0x20, 0xFF}, /* Dark Gray */
    {0x80, 0x80, 0x80, 0xFF}, /* Gray */
    {0xC0, 0xC0, 0xC0, 0xFF}, /* Light Gray */
    {0x90, 0x71, 0x3A, 0xFF}, /* Light Brown */
    {0x56, 0x2C, 0x05, 0xFF}, /* Brown */
    {0x00, 0x64, 0x11, 0xFF}, /* Dark Green */
    {0x1F, 0xB7, 0x14, 0xFF}, /* Green */
    {0x02, 0xAB, 0xEA, 0xFF}, /* Light Blue */
    {0x00, 0x00, 0xD4, 0xFF}, /* Blue */
    {0x46, 0x00, 0xA5, 0xFF}, /* Purple */
    {0xF2, 0x08, 0x84, 0xFF}, /* Pink */
    {0xDD, 0x08, 0x06, 0xFF}, /* Red */
    {0xFF, 0x64, 0x02, 0xFF}, /* Orange */
    {0xFC, 0xF3, 0x05, 0xFF}, /* Yellow */
    {0xFF, 0xFF, 0xFF, 0xFF}, /* White */
    {0xFF, 0x00, 0xFF, 0x00}  /* Transparent */
};

static SDL_Window *window;
static SDL_Surface *screen;
static SDL_Surface *surface;
static Uint8 *pixels;
static int pitch;
static Uint32 penColor;
static int setColor;

static Uint32 surfaceColor(int color)
{
    const SDL_Color *c;
    c = &mac_pal[color & 0xF];

    return SDL_MapRGBA(surface->format, c->r, c->g, c->b, c->a);
}

int InitGraphics(void)
{
    int ret;
    ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("Patater GUI Kit", SDL_WINDOWPOS_UNDEFINED,
                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                  0 /* | SDL_WINDOW_FULLSCREEN_DESKTOP*/);
    if (window == NULL)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    /* We might use a bit-depth of 4, but SDL has a bug that means this doesn't
     * work. We could use a depth of 8 as a workaround. However, we've found a
     * need to actually use 8 because we want to use a color key for
     * transparency (without losing one of our 16 colors). There doesn't seem
     * to be another way to SDL_BlitSurface with some areas of the dst rect not
     * getting updated without either a color key (fast) or use of alpha
     * blending (slow). */
    surface =
        SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 8, 0, 0, 0, 0);
    if (surface == NULL)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    SDL_SetPaletteColors(surface->format->palette, mac_pal, 0,
                         ARRAY_SIZE(mac_pal));
    SDL_SetColorKey(surface, SDL_TRUE, COLOR_TRANSPARENT); /* Needed? */
    pixels = surface->pixels;
    pitch = surface->pitch;

    /* Update the screen with the new surface. */
    SDL_FillRect(surface, NULL, surfaceColor(COLOR_WHITE));
    SDL_BlitSurface(surface, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);

    return 0;
}

void FreeGraphics(void)
{
    SDL_FreeSurface(screen);
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ShowGraphics(void)
{
    SDL_BlitSurface(surface, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);
}

void SetColor(int color)
{
    setColor = color;
    penColor = surfaceColor(color);
}

void FillScreen(int color)
{
    SDL_FillRect(surface, NULL, surfaceColor(color));
}
