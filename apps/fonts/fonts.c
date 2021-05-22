/*
 *  fonts.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-05-22.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/font.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include "guikit/panic.h"
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Lorem ipsum with hipster words. Word list from https://hipsum.co/ */
static const char* loremHipsumWords[] = {
    "3 wolf moon",
    "+1",
    "8-bit",
    "90's",
    "activated charcoal",
    "actually",
    "adaptogen",
    "aesthetic",
    "af",
    "affogato",
    "air plant",
    "art party",
    "artisan",
    "asymmetrical",
    "austin",
    "authentic",
    "banh mi",
    "banjo",
    "small batch",
    "beard",
    "before they sold out",
    "bespoke",
    "bicycle rights",
    "biodiesel",
    "put a bird on it",
    "bitters",
    "blog",
    "blue bottle",
    "man braid",
    "man bun",
    "brooklyn",
    "brunch",
    "bulb",
    "bushwick",
    "butcher",
    "cardigan",
    "celiac",
    "chambray",
    "chartreuse",
    "chia",
    "chicharrones",
    "chicken",
    "chillwave",
    "church-key",
    "master cleanse",
    "cliche",
    "cloud bread",
    "cold-pressed",
    "coloring book",
    "copper mug",
    "cornhole",
    "craft beer",
    "cray",
    "cred",
    "cronut",
    "crucifix",
    "DIY",
    "deep v",
    "direct trade",
    "disrupt",
    "distillery",
    "dreamcatcher",
    "drinking vinegar",
    "echo park",
    "edison bulb",
    "enamel pin",
    "ennui",
    "ethical",
    "etsy",
    "everyday carry",
    "fam",
    "fanny pack",
    "farm-to-table",
    "fashion axe",
    "fingerstache",
    "fixie",
    "flannel",
    "flexitarian",
    "forage",
    "franzen",
    "freegan",
    "gastropub",
    "gentrify",
    "glossier",
    "gluten-free",
    "gochujang",
    "godard",
    "health goth",
    "green juice",
    "hammock",
    "hashtag",
    "you probably haven't heard of them",
    "heirloom",
    "hell of",
    "hella",
    "helvetica",
    "hexagon",
    "hoodie",
    "hot chicken",
    "humblebrag",
    "iPhone",
    "iceland",
    "intelligentsia",
    "irony",
    "jean shorts",
    "jianbing",
    "kale chips",
    "keffiyeh",
    "keytar",
    "kickstarter",
    "kinfolk",
    "kitsch",
    "knausgaard",
    "kogi",
    "kombucha",
    "la croix",
    "leggings",
    "letterpress",
    "listicle",
    "literally",
    "live-edge",
    "lo-fi",
    "locavore",
    "lomo",
    "lumbersexual",
    "lyft",
    "marfa",
    "master",
    "meditation",
    "meggings",
    "meh",
    "messenger bag",
    "microdosing",
    "migas",
    "mixtape",
    "mlkshk",
    "mumblecore",
    "mustache",
    "narwhal",
    "neutra",
    "next level",
    "normcore",
    "occupy",
    "offal",
    "organic",
    "out",
    "PBR&B",
    "pabst",
    "paleo",
    "palo santo",
    "party",
    "photo booth",
    "pickled",
    "pinterest",
    "pitchfork",
    "plaid",
    "plant",
    "pok pok",
    "poke",
    "polaroid",
    "pop-up",
    "pork belly",
    "portland",
    "post-ironic",
    "pour-over",
    "poutine",
    "prism",
    "pug",
    "quinoa",
    "raclette",
    "ramps",
    "raw denim",
    "readymade",
    "retro",
    "rights",
    "roof party",
    "salvia",
    "sartorial",
    "scenester",
    "schlitz",
    "seitan",
    "selfies",
    "selvage",
    "semiotics",
    "shabby chic",
    "shaman",
    "shoreditch",
    "single-origin coffee",
    "skateboard",
    "slow-carb",
    "snackwave",
    "sold",
    "squid",
    "sriracha",
    "street art",
    "stumptown",
    "subway tile",
    "succulents",
    "sustainable",
    "swag",
    "synth",
    "tacos",
    "taiyaki",
    "tattooed",
    "taxidermy",
    "tbh",
    "them",
    "they",
    "thundercats",
    "tilde",
    "four dollar toast",
    "four loko",
    "tofu",
    "tote bag",
    "tousled",
    "food truck",
    "truffaut",
    "trust fund",
    "try-hard",
    "tumblr",
    "tumeric",
    "twee",
    "typewriter",
    "ugh",
    "umami",
    "unicorn",
    "VHS",
    "vape",
    "vaporware",
    "vegan",
    "venmo",
    "vexillologist",
    "vice",
    "vinyl",
    "viral",
    "waistcoat",
    "wayfarers",
    "whatever",
    "williamsburg",
    "woke",
    "XOXO",
    "YOLO",
    "yr",
    "yuccie"
};

/* Standard lorem ipsum words */
static const char* loremIpsumWords[] = {
    "a",
    "ac",
    "accumsan",
    "ad",
    "adipiscing",
    "aenean",
    "aliqua",
    "aliquam",
    "aliquet",
    "aliquip",
    "adipisicing",
    "met",
    "anim",
    "ante",
    "aptent",
    "arcu",
    "at",
    "auctor",
    "augue",
    "aute",
    "bibendum",
    "blandit",
    "cillum",
    "class",
    "commodo",
    "condimentum",
    "congue",
    "consectetur",
    "consequat",
    "conubia",
    "convallis",
    "cras",
    "cubilia",
    "culpa",
    "cupidatat",
    "curabitur",
    "curae",
    "cursus",
    "dapibus",
    "deserunt",
    "diam",
    "dictum",
    "dictumst",
    "dignissim",
    "dis",
    "do",
    "dolor",
    "dolore",
    "donec",
    "dui",
    "duis",
    "ea",
    "efficitur",
    "egestas",
    "eget",
    "eiusmod",
    "eleifend",
    "elementum",
    "elit",
    "enim",
    "erat",
    "eros",
    "esse",
    "est",
    "et",
    "etiam",
    "eu",
    "euismod",
    "ex",
    "excepteur",
    "exercitation",
    "facilisi",
    "facilisis",
    "fames",
    "faucibus",
    "felis",
    "fermentum",
    "feugiat",
    "finibus",
    "fringilla",
    "fugiat",
    "fusce",
    "gravida",
    "habitant",
    "habitasse",
    "hac",
    "hendrerit",
    "himenaeos",
    "iaculis",
    "id",
    "imperdiet",
    "in",
    "inceptos",
    "incididunt",
    "integer",
    "interdum",
    "ipsum",
    "irure",
    "justo",
    "labore",
    "laboris",
    "laborum",
    "lacinia",
    "lacus",
    "laoreet",
    "lectus",
    "leo",
    "libero",
    "ligula",
    "litora",
    "lobortis",
    "lorem",
    "luctus",
    "maecenas",
    "magna",
    "magnis",
    "malesuada",
    "massa",
    "mattis",
    "mauris",
    "maximus",
    "metus",
    "mi",
    "minim",
    "molestie",
    "mollis",
    "mollit",
    "montes",
    "morbi",
    "mus",
    "nam",
    "nascetur",
    "natoque",
    "nec",
    "neque",
    "netus",
    "nibh",
    "nisi",
    "nisl",
    "non",
    "nostra",
    "nostrud",
    "nulla",
    "nullam",
    "nunc",
    "occaecat",
    "odio",
    "officia",
    "orci",
    "ornare",
    "pariatur",
    "parturient",
    "pellentesque",
    "penatibus",
    "per",
    "pharetra",
    "phasellus",
    "placerat",
    "platea",
    "porta",
    "porttitor",
    "posuere",
    "potenti",
    "praesent",
    "pretium",
    "primis",
    "proident",
    "proin",
    "pulvinar",
    "purus",
    "quam",
    "qui",
    "quis",
    "quisque",
    "reprehenderit",
    "rhoncus",
    "ridiculus",
    "risus",
    "rutrum",
    "sagittis",
    "sapien",
    "scelerisque",
    "sed",
    "sem",
    "semper",
    "senectus",
    "sint",
    "sit",
    "sociosqu",
    "sodales",
    "sollicitudin",
    "sunt",
    "suscipit",
    "suspendisse",
    "taciti",
    "tellus",
    "tempor",
    "tempus",
    "tincidunt",
    "torquent",
    "tortor",
    "tristique",
    "turpis",
    "ullamco",
    "ullamcorper",
    "ultrices",
    "ultricies",
    "urna",
    "ut",
    "varius",
    "vehicula",
    "vel",
    "velit",
    "venenatis",
    "veniam",
    "vestibulum",
    "vitae",
    "vivamus",
    "viverra",
    "volutpat",
    "vulputate"
};

static char capitalize(char c)
{
    /* Clear the bit that is the difference between a and A. This is so that
     * even if we get a capital letter (e.g. proper nouns, abbreviations), we
     * won't change it to something trashy. */
    return c & ~0x20;
}

static void sentence(char *str, size_t size, int words, const char **list,
                     size_t len, const char *firstWords)
{
    int minWords;
    int maxWords;
    int numWords;

    (void)size; /* XXX use strncat() */
    minWords = words - words / 3;
    maxWords = words + words / 3;

    /* Ensure we output 0 or more words. */
    minWords = minWords > 0 ? minWords : 0;

    /* Output approximately however many words were requested. */
    numWords = RandRange(minWords, maxWords);

    str[0] = '\0'; /* Start with an empty string */

    /* About 1/5 the time, start with the first words (like "Lorem ipsum") */
    if (RandRange(0, 4) == 0)
    {
        strcat(str, firstWords);
        numWords -= 2;
    }

    /* Print some random words from the word list. */
    while (numWords-- > 0)
    {
        size_t which;

        which = RandRange(0, len - 1);
        strcat(str, list[which]);
        strcat(str, " ");
    }

    /* Ensure the sentence starts with a capital letter. */
    str[0] = capitalize(str[0]);

    /* End the sentence with a full stop. */
    str[strlen(str) - 1] = '.';
}

static void hipsumSentence(char *str, size_t size, int words)
{
    sentence(str, size, words, loremHipsumWords, ARRAY_SIZE(loremHipsumWords),
             "I'm baby ");
}

/* Output up to size characters into the provided string. */
static void loremSentence(char *str, size_t size, int words)
{
    sentence(str, size, words, loremIpsumWords, ARRAY_SIZE(loremIpsumWords),
             "Lorem ipsum ");
}

void RandomText(int num)
{
    int fgColor;
    int bgColor;
    struct Rect dst;
    int i;
    const struct Font *font;
    int textWidth;
    int textHeight;
    int x;
    int y;
    const char *fonts[] = {"Windy12", "Orange", "Plain9"};
    static char text[300];

    for (i = 0; i < num; ++i)
    {
        int fontNum;
        /* Make up a string */
        loremSentence(text, sizeof(text), 7);
        hipsumSentence(text, sizeof(text), 7);

        /* Pick a font. */
        fontNum = RandRange(0, ARRAY_SIZE(fonts) - 1);
        font = GetFont(fonts[fontNum]);
        if (!font)
        {
            return;
        }

        MeasureString(font, text, &textWidth, &textHeight);

        /* Pick a random color for the background. */
        bgColor = RandRange(0, NUM_COLORS - 1);
        do
        {
            /* Pick a different random color for the foreground, retrying by
             * random until we get a different color. */
            fgColor = RandRange(0, NUM_COLORS - 1);
        } while (fgColor == bgColor);

        x = RandRange(-textWidth / 2, SCREEN_WIDTH - 1);
        y = RandRange(-textHeight / 2, SCREEN_HEIGHT - 1);
        dst.left = x;
        dst.top = y;
        dst.right = dst.left + textWidth - 1;
        dst.bottom = dst.top + textHeight - 1;

        FillRect(bgColor, &dst);
        DrawString(font, text, fgColor, x, y);

        ShowGraphics();
    }
}

int main()
{
    int ret;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing 10000 strings...\n");
    FillScreen(COLOR_WHITE);
    ShowGraphics();
    RandomText(10000);

    SaveScreenShot("fonts.bmp");

    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
