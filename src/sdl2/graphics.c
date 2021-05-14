/*
 *  graphics.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-05.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/panic.h"
#include "guikit/primrect.h"
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

const unsigned char blackPattern[] = {0, 0, 0, 0, 0, 0, 0, 0};

static SDL_Window *window;
static SDL_Surface *screen;
static SDL_Surface *surface;
static Uint8 *pixels;
static int pitch;
static Uint32 penColor;
static int setColor;

/* An 8x8 1-bit bitmap, packed 1bpp */
static const unsigned char *currentPattern;

static Uint32 surfaceColor(int color)
{
    const SDL_Color *c;
    c = &mac_pal[color & 0xF];

    return SDL_MapRGBA(surface->format, c->r, c->g, c->b, c->a);
}

int InitGraphics(void)
{
    int ret;

    currentPattern = blackPattern;

    ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "1", SDL_HINT_OVERRIDE);

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
    ret = SDL_FillRect(surface, NULL, surfaceColor(COLOR_WHITE));
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    ret = SDL_BlitSurface(surface, NULL, screen, NULL);
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    ret = SDL_UpdateWindowSurface(window);
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

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
    int ret;

    /* Seems needed on mac for some reason, otherwise window doesn't show up...
     * */
    SDL_Event event;
    SDL_PollEvent(&event);

    ret = SDL_BlitSurface(surface, NULL, screen, NULL);
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }

    ret = SDL_UpdateWindowSurface(window);
    if (ret < 0)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }
}

int SaveScreenShot(const char *path)
{
    int ret;

    ret = SDL_SaveBMP(surface, path);
    if (ret)
    {
        printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
    }

    return ret;
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

static void drawVertLine(int x, int y, int len)
{
    SDL_Rect r;

    r.x = x;
    r.y = y;
    r.w = 1;
    r.h = len;

    SDL_FillRect(surface, &r, penColor);
}

static void drawHorizLine(int x, int y, int len)
{
    SDL_Rect r;

    r.x = x;
    r.y = y;
    r.w = len;
    r.h = 1;

    SDL_FillRect(surface, &r, penColor);
}

void DrawRect(int color, const struct Rect *rect)
{
    int x;
    int y;
    int width;
    int height;

    SetColor(color);

    x = rect->left;
    y = rect->top;
    width = rect->right - rect->left + 1;
    height = rect->bottom - rect->top + 1;

    drawVertLine(x, y, height); /* Left */
    drawHorizLine(x + 1, y, width - 1); /* Top */
    drawVertLine(x + width - 1, y + 1, height - 1); /* Right */
    drawHorizLine(x + 1, y + height - 1, width - 2); /* Bottom */
}

void FillRect(int color, const struct Rect *rect)
{
    SDL_Rect r;

    r.x = rect->left;
    r.y = rect->top;
    r.w = rect->right - rect->left + 1;
    r.h = rect->bottom - rect->top + 1;

    SetColor(color);
    SDL_FillRect(surface, &r, penColor);
}

void FillRectOp(int bg_color, int fg_color, const unsigned char *pattern,
                int op, const struct Rect *rect)
{
    /* TODO */
    (void) bg_color;
    (void) pattern;
    (void) op;
    FillRect(fg_color, rect);
}

void DrawVertLine(int x1, int y1, int len)
{
    drawVertLine(x1, y1, len);
}

void DrawHorizLine(int x1, int y1, int len)
{
    drawHorizLine(x1, y1, len);
}

static void drawPixel(int x, int y)
{
    SDL_Rect r;

    r.x = x;
    r.y = y;
    r.w = 1;
    r.h = 1;

    SDL_FillRect(surface, &r, penColor);
}

static void drawDiagLine(int x1, int y1, int x2, int len)
{
    int x;
    int y;
    int y_start;
    int y_end;
    int leftToRight;

/*
Diagonal lines for testing.
-- (40,0) -> (160,120)
        (40,0)->(160,121)
-- (280,0) -> (160,120)
        (280,0)->(160,121)
-- (160,120) -> (279,239)
        (160,120)->(279,120)
-- (160,120) -> (41,239)
        (160,120)->(41,120)
-- (360,0) -> (480,120)
        (360,0)->(480,121)
-- (600,0) -> (480,120)
        (600,0)->(480,121)
-- (480,120) -> (599,239)
        (480,120)->(599,120)
-- (480,120) -> (361,239)
        (480,120)->(361,120)
-- (40,240) -> (160,360)
        (40,240)->(160,121)
-- (280,240) -> (160,360)
        (280,240)->(160,121)
-- (160,360) -> (279,479)
        (160,360)->(279,120)
-- (160,360) -> (41,479)
        (160,360)->(41,120)
-- (360,240) -> (480,360)
        (360,240)->(480,121)
-- (600,240) -> (480,360)
        (600,240)->(480,121)
-- (480,360) -> (599,479)
        (480,360)->(599,120)
-- (480,360) -> (361,479)
        (480,360)->(361,120)
*/
    printf("\t(%d,%d)->(%d,%d)\n", x1, y1, x2, len);

    /* Right to left or left to right */
    /* Assumes top to bottom */
    y_start = y1;
    y_end = y1 + len - 1;
    x = x1;

    if (x1 < x2)
    {
        leftToRight = 1;
    }
    else
    {
        leftToRight = -1;
    }

    for (y = y_start; y <= y_end; ++y)
    {
        drawPixel(x, y);
        x += leftToRight;
    }
}

void DrawDiagLine(int x1, int y1, int x2, int len)
{
    drawDiagLine(x1, y1, x2, len);
}

/* XXX We are doing run-sliced bressenham. For small lines, standard bressenham
 * should be faster, (but only if we can figure out how to write more than 1
 * pixel at a time from standard bressenham). */
/* Provide standard bressenham here, too, so we can do short lines quickly,
 * without a divide or extra setup. */
/* Algorithm from Michael Abrash's Graphics Programming Black Book, Chapter 37.
 * Variable names and comments kept similar for ease of comparison with the
 * text. */
void DrawLine(int color, int x1, int y1, int x2, int y2)
{
    int AdjUp; /* Error term adjust up on each advance*/
    int AdjDown; /* Error term adjust down when error term turns over*/
    int XAdvance; /* 1 or -1, for direction in which x advances*/
    int YDelta;
    int XDelta;
    int min_run_len;
    int final_run_len;
    int run_len; /* aka step */
    int errorAdj;

    SetColor(color);

    /*printf("(%03d,%03d)->(%03d,%03d)\r", x1, y1, x2, y2);*/

    /* We'll draw top to bottom, to reduce the number of cases we have to
     * handle, and to make lines between the same endpoints always draw the
     * same pixels. */
    /* TODO remove this, as we'll like patterned lines to work well without
     * having to reverse the pattern. */
    if (y1 > y2)
    {
        int tmp;

        /* Swap y1 and y2 */
        tmp = y1;
        y1 = y2;
        y2 = tmp;
        /* Swap x1 and x2 */
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    /* Now the line is top to bottom */

    /* Figure out how far we're going vertically (guaranteed to be positive) */
    YDelta = y2 - y1;

    /* Figure out whether we're going left or right, and how far we're going
     * horizontally. In the process, special-case vertical lines, for speed and
     * to avoid nasty boundary conditions and division by 0 */
    XDelta = x2 - x1;
    /* Do we have a vertical line? */
    if (XDelta == 0)
    {
        /* Yes, so use vertical line drawing */
        drawVertLine(x1, y1, YDelta+1);
        return;
    }
    else if (XDelta < 0)
    {
        /* Right to left */
        XAdvance = -1;
        XDelta = -XDelta; /* |XDelta| -- Make XDelta positive */
    }
    else
    {
        /* Left to right */
        XAdvance = 1;
    }

    /* Special-case horizontal lines */
    if (YDelta == 0)
    {
        if (XAdvance < 0)
        {
            /* Right-to-left */
            drawHorizLine(x2, y1, XDelta+1);
            return;
        }
        else
        {
            /* Left-to-right */
            drawHorizLine(x1, y1, XDelta+1);
            return;
        }
    }
    /* Special-case diagonal lines */
    else if (YDelta == XDelta)
    {
        printf("-- (%d,%d) -> (%d,%d)\n", x1, y1, x2, y2);
        drawDiagLine(x1, y1, x2, XDelta+1);
        return;
    }

    /* Determine whether the line is X or Y major, and handle accordingly. */
    if (XDelta > YDelta)
    {
        /* X-major */
        /* More horizontal than vertical */

        min_run_len = XDelta / YDelta;
        errorAdj = XDelta % YDelta;
        AdjUp = errorAdj + errorAdj;
        AdjDown = YDelta + YDelta;

        /* Initial error term; reflects an initial step of 0.5 along the Y
         * axis */
        errorAdj -= AdjDown;

        /* The initial and last runs are partial, because Y advances only 0.5
         * for these runs, rather than 1. Divide one full run, plus the initial
         * pixel, between the initial and last runs. */
        run_len = min_run_len / 2 + 1;
        final_run_len = run_len;

        /* If there is an odd number of pixels per run, we have one pixel that
         * can't be allocated to either the initial or last partial run, so
         * we'll add 0.5 to the error term so this pixel will be handled by the
         * normal full-run loop. */
        /* Is the run length odd? */
        if (min_run_len & 1)
        {
            /* Odd length, add YDelta to error term (add 0.5 of a pixel to the
             * error term) */
            errorAdj += YDelta;
        }
        else
        {
            /* The basic run length is even */
            /* If there's no fractional advance, we have one pixel that could
             * go to either the initial or last partial run, which we'll
             * arbitrarily allocate to the last run.*/
            if (AdjUp == 0)
            {
                --run_len;
            }
        }

        /* X-Major adjustments done now */

        /* Draw the first partial run of pixels. */
        /* XXX TODO extract out the x2 x1 choice to the outside of the loop. */
        /* Option: make an x variable we set when we choose XAdvance, and then
         * always use XAdvance as "1" when doing x-major
         * Another choice is to have a horizontal line drawing function that
         * takes Xadvance as a param, to pick to go left or right */
        if (XAdvance < 0)
        {
            /* Right to left */
            drawHorizLine(x1-run_len+1, y1++, run_len);
            x1 -= run_len;
        }
        else
        {
            /* Left to right */
            drawHorizLine(x1, y1++, run_len);
            x1 += run_len;
        }

        /* Draw all full runs */
        /* Are there more than 2 scans? so there are some full runs? */
        if (YDelta < 2)
        {
            /* No, no full runs */
            goto xmajor_final;
        }

        while (YDelta-- > 1) {
            run_len = min_run_len; /* Run is at least this long */
            /* Advance the error term and add an extra pixel if the error term
             * so indicates. */
            errorAdj += AdjUp;
            if (errorAdj > 0)
            {
                /* One extra pixel in run */
                ++run_len;
                /* Reset the error term */
                errorAdj -= AdjDown;
            }
            if (XAdvance < 0)
            {
                /* Right to left */
                drawHorizLine(x1-run_len+1, y1++, run_len);
                x1 -= run_len;
            }
            else
            {
                /* Left to right */
                drawHorizLine(x1, y1++, run_len);
                x1 += run_len;
            }
        }
xmajor_final:
        if (XAdvance < 0)
        {
            /* Right to left */
            drawHorizLine(x1-final_run_len+1, y1, final_run_len);
        }
        else
        {
            /* Left to right */
            drawHorizLine(x1, y1, final_run_len);
        }
        return;
    }
    else
    {
        /* Y-major */
        /* More vertical than horizontal */
        min_run_len = YDelta / XDelta;
        errorAdj = YDelta % XDelta;
        AdjUp = errorAdj + errorAdj;
        AdjDown = XDelta + XDelta;

        /* Initial error term; reflects an initial step of 0.5 along the X
         * axis */
        errorAdj -= AdjDown;

        /* The initial and last runs are partial, because X advances only 0.5
         * for these runs, rather than 1. Divide one full run, plus the initial
         * pixel, between the initial and last runs. */
        run_len = min_run_len / 2 + 1;
        final_run_len = run_len;

        /* If there is an odd number of pixels per run, we have one pixel that
         * can't be allocated to either the initial or last partial run, so
         * we'll add 0.5 to the error term so this pixel will be handled by the
         * normal full-run loop. */
        /* Is the run length odd? */
        if (min_run_len & 1)
        {
            /* Odd length, add XDelta to error term (add 0.5 of a pixel to the
             * error term) */
            errorAdj += XDelta;
        }
        else
        {
            /* The basic run length is even */
            /* If there's no fractional advance, we have one pixel that could
             * go to either the initial or last partial run, which we'll
             * arbitrarily allocate to the last run.*/
            if (AdjUp == 0)
            {
                --run_len;
            }
        }

        /* Y-Major adjustments done now */

        /* Draw the first partial run of pixels. */
        /*printf("x1,y1,len: (%d,%d,%d)\r", x1, y1, run_len);*/
        drawVertLine(x1, y1, run_len);
        y1 += run_len;
        x1 += XAdvance;

        /* Draw all full runs */
        /* Are there more than 2 scans? so there are some full runs? */
        if (XDelta < 2)
        {
            /* No, no full runs */
            goto ymajor_final;
        }

        while (XDelta-- > 1) {
            run_len = min_run_len; /* Run is at least this long */
            /* Advance the error term and add an extra pixel if the error term
             * so indicates. */
            errorAdj += AdjUp;
            if (errorAdj > 0)
            {
                /* One extra pixel in run */
                ++run_len;
                /* Reset the error term */
                errorAdj -= AdjDown;
            }
            /*printf("x1,y1,len: (%d,%d,%d)\r", x1, y1, run_len);*/
            drawVertLine(x1, y1, run_len);
            y1 += run_len;
            x1 += XAdvance;
        }
ymajor_final:
        /*printf("x1,y1,len: (%d,%d,%d)\r", x1, y1, run_len);*/
        drawVertLine(x1, y1, final_run_len);
        return;
    }
}

static void circlePoints(int x0, int y0, int x, int y)
{
    drawPixel(x0 + x, y0 + y);
    drawPixel(x0 + y, y0 + x);
    drawPixel(x0 + y, y0 - x);
    drawPixel(x0 + x, y0 - y);
    drawPixel(x0 - x, y0 - y);
    drawPixel(x0 - y, y0 - x);
    drawPixel(x0 - y, y0 + x);
    drawPixel(x0 - x, y0 + y);
}

void DrawCircle(int color, int x0, int y0, int radius)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;

    SetColor(color);

    /* Speed this up by accumulating horizontal, diagonal, and vertical line
     * segments. Draw each line segment reflected around in each octant. */

    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    circlePoints(x0, y0, x, y);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        circlePoints(x0, y0, x, y);
    }
}

static void fillCirclePoints(int x0, int y0, int x, int y)
{
    /* NW to NE */
    drawHorizLine(x0 - x, y0 - y, x + x); /* Top */
    drawHorizLine(x0 - y, y0 - x, y + y); /* Upper middle */

    /* SW to SE */
    drawHorizLine(x0 - y, y0 + x, y + y); /* Lower middle */
    drawHorizLine(x0 - x, y0 + y, x + x); /* Bottom */
}

void FillCircle(int color, int x0, int y0, int radius)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;

    SetColor(color);

    /* Speed this up by accumulating horizontal, diagonal, and vertical line
     * segments. Draw each line segment reflected around in each octant. */
    /* Speed this up by not drawing the same pixels multiple times. (Currently,
     * any horizontal lines at the top of the circle are drawn over many times,
     * as a specific example.) */

    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    fillCirclePoints(x0, y0, x, y);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        fillCirclePoints(x0, y0, x, y);
    }
}

static void roundPoints(int x0, int y0, int radius, int x, int y, int width, int height)
{
    /* SE */
    drawPixel(x0 + x + width - radius - 1, y0 + y + height - radius - 1);
    drawPixel(x0 + y + width - radius - 1, y0 + x + height - radius - 1);

    /* NE */
    drawPixel(x0 + y + width - radius - 1, y0 - x + radius);
    drawPixel(x0 + x + width - radius - 1, y0 - y + radius);

    /* NW */
    drawPixel(x0 - x + radius, y0 - y + radius);
    drawPixel(x0 - y + radius, y0 - x + radius);

    /* SW */
    drawPixel(x0 - y + radius, y0 + x + height - radius - 1);
    drawPixel(x0 - x + radius, y0 + y + height - radius - 1);
}

void DrawRoundRect(int color, int x0, int y0, int radius, int width, int height)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;

    /* TODO Fix crash when radius is more curvy than a circle is allowed to be
     * */

    SetColor(color);

    /* Draw rectangle portion, without corners */
    drawVertLine(x0, y0 + radius + 1, height - 2 - 2 * radius); /* Left */
    drawHorizLine(x0 + 1 + radius, y0, width - 2 - 2 * radius); /* Top */
    drawVertLine(x0 + width - 1, y0 + radius + 1,
                 height - 2 - 2 * radius); /* Right */
    drawHorizLine(x0 + 1 + radius, y0 + height - 1,
                  width - 2 - 2 * radius); /* Bottom */

    /* Same as DrawCircle(), but with roundPoints() */
    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    roundPoints(x0, y0, radius, x, y, width, height);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        roundPoints(x0, y0, radius, x, y, width, height);
    }
}

static void fillRoundPoints(int x0, int y0, int radius, int x, int y, int width, int height)
{
    /* NW to NE */
    drawHorizLine(x0 - x + radius, y0 - y + radius,
                  (x0 + x + width - radius) - (x0 - x + radius)); /* Top */
    drawHorizLine(x0 - y + radius, y0 - x + radius,
                  (x0 + y + width - radius) -
                      (x0 - y + radius)); /* Upper middle */

    /* SW to SE */
    drawHorizLine(x0 - y + radius, y0 + x + height - radius - 1,
                  (x0 + y + width - radius) -
                      (x0 - y + radius)); /* Lower middle */
    drawHorizLine(x0 - x + radius, y0 + y + height - radius - 1,
                  (x0 + x + width - radius) - (x0 - x + radius)); /* Bottom */
}

void FillRoundRect(int color, int x0, int y0, int radius, int width,
                   int height)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;
    struct Rect rect;

    SetColor(color);

    /* Use FillRect() to set mode for us, and avoid extra register setting */
    rect.left = x0;
    rect.top = y0 + radius + 1;
    rect.right = rect.left + width - 1;
    rect.bottom = rect.top + height - radius - radius - 2 - 1;
    FillRect(color, &rect);

    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    fillRoundPoints(x0, y0, radius, x, y, width, height);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        fillRoundPoints(x0, y0, radius, x, y, width, height);
    }
}

int BlitWithMask(const unsigned char *img, const unsigned char *mask,
                 const struct Rect *dst, const struct Rect *src, int span)
{
    SDL_Surface *masked;
    int x;
    int y;
    int spanBytes;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    Uint8 *bitmapPixels;

    spanBytes = (span + 7) / 8; /* Convert to bytes */


    srcRect.x = src->left;
    srcRect.y = src->top;
    srcRect.w = src->right - src->left + 1;
    srcRect.h = src->bottom - src->top + 1;

    dstRect.x = dst->left;
    dstRect.y = dst->top;
    dstRect.w = dst->right - dst->left + 1;
    dstRect.h = dst->bottom - dst->top + 1;

    if (srcRect.w != dstRect.w)
    {
        panic("Blit width mismatch. Not supported.");
    }
    if (srcRect.h != dstRect.h)
    {
        panic("Blit height mismatch. Not supported.");
    }

    masked = SDL_CreateRGBSurfaceWithFormat(0, span, dstRect.h, 8,
                                            SDL_PIXELFORMAT_INDEX8);
    if (!masked)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }
    SDL_SetPaletteColors(masked->format->palette, mac_pal, 0,
                         ARRAY_SIZE(mac_pal));
    SDL_SetColorKey(masked, SDL_TRUE, COLOR_TRANSPARENT);

    /* Blit */
    SDL_LockSurface(masked);
    bitmapPixels = masked->pixels;
    for (y = 0; y < dstRect.h; ++y)
    {
        for (x = 0; x < spanBytes; ++x)
        {
            Uint8 maskByte;
            Uint8 imgByte;
            int i;

            i = y * masked->pitch + x * 8;

            maskByte = mask[y * spanBytes + x];
            imgByte = img[y * spanBytes + x];

            /* Mode 0 */
            /* Copies the white as color. black as transparent. */
            bitmapPixels[i] = imgByte & maskByte ? COLOR_TRANSPARENT : setColor;
            /* Mode 3 */
            /* Like a stencil. Whereever is black is goes through to the color
             * */
            bitmapPixels[i] = imgByte & maskByte ? setColor : COLOR_TRANSPARENT;
        }
    }
    SDL_UnlockSurface(masked);

    SDL_BlitSurface(masked, &srcRect, surface, &dstRect);

    return 0;
}

static unsigned char get_op(int op, const unsigned char src,
                            const unsigned char *pat, size_t row)
{
    unsigned char p = pat[row % 8];
    unsigned char byte = src & 0xFF;
    switch (op)
    {
    case OP_SRC_INV:
        byte = ~byte;
        break;
    case OP_SRC_INV_PAT_XOR:
        byte = ~byte ^ p;
        break;
    case OP_PAT_AND:
        byte &= p;
        break;
    case OP_SRC_INV_PAT_AND:
        byte = ~byte & p;
        break;
    case OP_PAT_NOT:
        byte &= ~p;
        break;
    case OP_PAT_OR:
        byte |= p;
        break;
    case OP_SRC_INV_PAT_OR:
        byte = ~byte | p;
        break;
    case OP_PAT_XOR:
        byte ^= p;
        break;
    case OP_NONE:
    default:
        break;
    }

    return byte;
}

int Blit(const unsigned char *img, const struct Rect *dst0,
         const struct Rect *src0, int span)
{
    return BlitOp(img, OP_NONE, dst0, src0, span);
}

int BlitOp(const unsigned char *img, int op, const struct Rect *dst0,
           const struct Rect *src0, int span)
{
    SDL_Surface *colored;
    int x;
    int y;
    int srcWidth;
    int srcHeight;
    int dstWidth;
    int dstHeight;
    int spanBytes;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    Uint8 *bitmapPixels;
    int colInByte;
    size_t row;

    spanBytes = (span + 7) / 8; /* Convert to bytes */

    srcWidth = src0->right - src0->left + 1;
    srcHeight = src0->bottom - src0->top + 1;
    dstWidth = dst0->right - dst0->left + 1;
    dstHeight = dst0->bottom - dst0->top + 1;
    if (srcWidth != dstWidth)
    {
        panic("Blit width mismatch. Not supported.");
    }
    if (srcHeight != dstHeight)
    {
        panic("Blit height mismatch. Not supported.");
    }

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = srcWidth;
    srcRect.h = srcHeight;

    dstRect.x = dst0->left;
    dstRect.y = dst0->top;
    dstRect.w = dstWidth;
    dstRect.h = dstHeight;

    colored = SDL_CreateRGBSurfaceWithFormat(0, span, dstHeight, 8,
                                             SDL_PIXELFORMAT_INDEX8);
    if (!colored)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }
    SDL_SetPaletteColors(colored->format->palette, mac_pal, 0,
                         ARRAY_SIZE(mac_pal));
    SDL_SetColorKey(colored, SDL_TRUE, COLOR_TRANSPARENT);

    /* Blit */
    /* Make a new texture from the 1-bit bitmap, colorizing it along the way.
     * */
    SDL_LockSurface(colored);
    bitmapPixels = colored->pixels;
    colInByte = src0->left % 8; /* keep only the column in-byte address */

    /* If x starts at 3 pixels in, we want pixel numbers 2 3 4 5 6 7 0 1 in
     * that order, where 0 and 1 are from the next byte. */
    row = 0;
    for (y = 0; y < srcHeight; ++y)
    {
        int curColInByte;
        const Uint8 *imgByte;
        Uint8 byte;

        curColInByte = colInByte;
        imgByte = &img[(y + src0->top) * spanBytes + src0->left / 8];

        for (x = 0; x < srcWidth; ++x)
        {
            int i;
            int mask;
            int shift;
            unsigned char src;

            i = y * colored->pitch + x;
            src = *imgByte;
            byte = get_op(op, src, currentPattern, row);
            shift = 7 - curColInByte;
            mask = 1 << shift;

            /* Mode 0 */
            /* Copies the white as color. black as transparent. */
            bitmapPixels[i] = byte & mask ? COLOR_TRANSPARENT : setColor;
            /* Mode 3 */
            /* Like a stencil. Whereever is black is goes through to the color
             * */
            bitmapPixels[i] = byte & mask ? setColor : COLOR_TRANSPARENT;

            ++curColInByte;
            if (curColInByte == 8)
            {
                curColInByte = 0;
                ++imgByte;
            }
        }
        ++row;
    }
    SDL_UnlockSurface(colored);

    /* Copy from the new texture onto our main graphics surface. */
    SDL_BlitSurface(colored, &srcRect, surface, &dstRect);

    return 0;
}

int DrawBitmap(const struct Rect *dst, int span, const unsigned char *img,
               const unsigned char *mask, int color)
{
    SDL_Surface *masked;
    int x;
    int y;
    int width;
    int height;
    int spanBytes;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    Uint8 *bitmapPixels;

    spanBytes = (span + 7) / 8; /* Convert to bytes */

    width = dst->right - dst->left + 1;
    height = dst->bottom - dst->top + 1;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = width;
    srcRect.h = height;

    dstRect.x = dst->left;
    dstRect.y = dst->top;
    dstRect.w = width;
    dstRect.h = height;

    masked = SDL_CreateRGBSurfaceWithFormat(0, span, height, 8,
                                            SDL_PIXELFORMAT_INDEX8);
    if (!masked)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }
    SDL_SetPaletteColors(masked->format->palette, mac_pal, 0,
                         ARRAY_SIZE(mac_pal));
    SDL_SetColorKey(masked, SDL_TRUE, COLOR_TRANSPARENT);

    /* Blit */
    SDL_LockSurface(masked);
    bitmapPixels = masked->pixels;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < spanBytes; ++x)
        {
            Uint8 maskByte;
            Uint8 imgByte;
            int i;

            i = y * masked->pitch + x * 8;

            maskByte = mask[y * spanBytes + x];
            imgByte = img[y * spanBytes + x];

            bitmapPixels[i + 0] =
                maskByte & (0x80 >> 0)
                    ? imgByte & (0x80 >> 0) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 1] =
                maskByte & (0x80 >> 1)
                    ? imgByte & (0x80 >> 1) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 2] =
                maskByte & (0x80 >> 2)
                    ? imgByte & (0x80 >> 2) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 3] =
                maskByte & (0x80 >> 3)
                    ? imgByte & (0x80 >> 3) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 4] =
                maskByte & (0x80 >> 4)
                    ? imgByte & (0x80 >> 4) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 5] =
                maskByte & (0x80 >> 5)
                    ? imgByte & (0x80 >> 5) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 6] =
                maskByte & (0x80 >> 6)
                    ? imgByte & (0x80 >> 6) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 7] =
                maskByte & (0x80 >> 7)
                    ? imgByte & (0x80 >> 7) ? COLOR_WHITE : color
                    : COLOR_TRANSPARENT;
        }
    }
    SDL_UnlockSurface(masked);

    SDL_BlitSurface(masked, &srcRect, surface, &dstRect);

    return 0;
}

int DrawColorBitmap(const struct Rect *dst, int span, const unsigned char *img,
                    const unsigned char *mask)
{
    SDL_Surface *masked;
    int x;
    int y;
    int width;
    int height;
    int spanBytes;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    Uint8 *bitmapPixels;

    spanBytes = (span + 7) / 8; /* Convert to bytes */

    width = dst->right - dst->left + 1;
    height = dst->bottom - dst->top + 1;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = width;
    srcRect.h = height;

    dstRect.x = dst->left;
    dstRect.y = dst->top;
    dstRect.w = width;
    dstRect.h = height;

    masked = SDL_CreateRGBSurfaceWithFormat(0, span, height, 8,
                                            SDL_PIXELFORMAT_INDEX8);
    if (!masked)
    {
        panic("SDL Error: %s\n", SDL_GetError());
    }
    SDL_SetPaletteColors(masked->format->palette, mac_pal, 0,
                         ARRAY_SIZE(mac_pal));
    SDL_SetColorKey(masked, SDL_TRUE, COLOR_TRANSPARENT);

    /* Color Blit */
    SDL_LockSurface(masked);
    bitmapPixels = masked->pixels;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < spanBytes; ++x)
        {
            Uint8 maskByte;
            Uint8 imgBytes[4];
            Uint8 color[16];
            int i;

            i = y * masked->pitch + x*8;

            maskByte = mask[y * spanBytes + x];

            imgBytes[0] = img[0 * spanBytes * height + y * spanBytes + x];
            imgBytes[1] = img[1 * spanBytes * height + y * spanBytes + x];
            imgBytes[2] = img[2 * spanBytes * height + y * spanBytes + x];
            imgBytes[3] = img[3 * spanBytes * height + y * spanBytes + x];

            color[0] = (((imgBytes[0] & 0x80) >> 7)) << 0 |
                       (((imgBytes[1] & 0x80) >> 7)) << 1 |
                       (((imgBytes[2] & 0x80) >> 7)) << 2 |
                       (((imgBytes[3] & 0x80) >> 7)) << 3 |
                       0;
            color[1] = (((imgBytes[0] & 0x40) >> 6)) << 0 |
                       (((imgBytes[1] & 0x40) >> 6)) << 1 |
                       (((imgBytes[2] & 0x40) >> 6)) << 2 |
                       (((imgBytes[3] & 0x40) >> 6)) << 3 |
                       0;
            color[2] = (((imgBytes[0] & 0x20) >> 5)) << 0 |
                       (((imgBytes[1] & 0x20) >> 5)) << 1 |
                       (((imgBytes[2] & 0x20) >> 5)) << 2 |
                       (((imgBytes[3] & 0x20) >> 5)) << 3 |
                       0;
            color[3] = (((imgBytes[0] & 0x10) >> 4)) << 0 |
                       (((imgBytes[1] & 0x10) >> 4)) << 1 |
                       (((imgBytes[2] & 0x10) >> 4)) << 2 |
                       (((imgBytes[3] & 0x10) >> 4)) << 3 |
                       0;
            color[4] = (((imgBytes[0] & 0x08) >> 3)) << 0 |
                       (((imgBytes[1] & 0x08) >> 3)) << 1 |
                       (((imgBytes[2] & 0x08) >> 3)) << 2 |
                       (((imgBytes[3] & 0x08) >> 3)) << 3 |
                       0;
            color[5] = (((imgBytes[0] & 0x04) >> 2)) << 0 |
                       (((imgBytes[1] & 0x04) >> 2)) << 1 |
                       (((imgBytes[2] & 0x04) >> 2)) << 2 |
                       (((imgBytes[3] & 0x04) >> 2)) << 3 |
                       0;
            color[6] = (((imgBytes[0] & 0x02) >> 1)) << 0 |
                       (((imgBytes[1] & 0x02) >> 1)) << 1 |
                       (((imgBytes[2] & 0x02) >> 1)) << 2 |
                       (((imgBytes[3] & 0x02) >> 1)) << 3 |
                       0;
            color[7] = (((imgBytes[0] & 0x01) >> 0)) << 0 |
                       (((imgBytes[1] & 0x01) >> 0)) << 1 |
                       (((imgBytes[2] & 0x01) >> 0)) << 2 |
                       (((imgBytes[3] & 0x01) >> 0)) << 3 |
                       0;

            bitmapPixels[i + 0] =
                maskByte & (0x80 >> 0)
                    ? color[0]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 1] =
                maskByte & (0x80 >> 1)
                    ? color[1]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 2] =
                maskByte & (0x80 >> 2)
                    ? color[2]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 3] =
                maskByte & (0x80 >> 3)
                    ? color[3]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 4] =
                maskByte & (0x80 >> 4)
                    ? color[4]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 5] =
                maskByte & (0x80 >> 5)
                    ? color[5]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 6] =
                maskByte & (0x80 >> 6)
                    ? color[6]
                    : COLOR_TRANSPARENT;
            bitmapPixels[i + 7] =
                maskByte & (0x80 >> 7)
                    ? color[7]
                    : COLOR_TRANSPARENT;
        }
    }
    SDL_UnlockSurface(masked);

    SDL_BlitSurface(masked, &srcRect, surface, &dstRect);

    return 0;
}
