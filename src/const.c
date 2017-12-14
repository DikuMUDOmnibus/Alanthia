/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


/* item type list */
const struct item_type item_table[] = {
    {ITEM_LIGHT, "light"},
    {ITEM_SCROLL, "scroll"},
    {ITEM_WAND, "wand"},
    {ITEM_STAFF, "staff"},
    {ITEM_WEAPON, "weapon"},
    {ITEM_TREASURE, "treasure"},
    {ITEM_ARMOR, "armor"},
    {ITEM_POTION, "potion"},
    {ITEM_CLOTHING, "clothing"},
    {ITEM_FURNITURE, "furniture"},
    {ITEM_TRASH, "trash"},
    {ITEM_BLOOD_POOL, "blood_pool"},
    {ITEM_CONTAINER, "container"},
    {ITEM_DRINK_CON, "drink"},
    {ITEM_BLOOD_CON, "flask"},
    {ITEM_KEY, "key"},
    {ITEM_FOOD, "food"},
    {ITEM_MONEY, "money"},
    {ITEM_BOAT, "boat"},
    {ITEM_CORPSE_NPC, "npc_corpse"},
    {ITEM_CORPSE_PC, "pc_corpse"},
    {ITEM_FOUNTAIN, "fountain"},
    {ITEM_PILL, "pill"},
    {ITEM_PROTECT, "protect"},
    {ITEM_MAP, "map"},
    {ITEM_PORTAL, "portal"},
    {ITEM_WARP_STONE, "warp_stone"},
    {ITEM_ROOM_KEY, "room_key"},
    {ITEM_GEM, "gem"},
    {ITEM_JEWELRY, "jewelry"},
    {ITEM_JUKEBOX, "jukebox"},
    {0, NULL}
};


/* weapon selection table */
const struct weapon_type weapon_table[] = {
    {"sword", OBJ_VNUM_SCHOOL_SWORD, WEAPON_SWORD, &gsn_sword},
    {"mace", OBJ_VNUM_SCHOOL_MACE, WEAPON_MACE, &gsn_mace},
    {"dagger", OBJ_VNUM_SCHOOL_DAGGER, WEAPON_DAGGER, &gsn_dagger},
    {"axe", OBJ_VNUM_SCHOOL_AXE, WEAPON_AXE, &gsn_axe},
    {"staff", OBJ_VNUM_SCHOOL_STAFF, WEAPON_SPEAR, &gsn_spear},
    {"flail", OBJ_VNUM_SCHOOL_FLAIL, WEAPON_FLAIL, &gsn_flail},
    {"whip", OBJ_VNUM_SCHOOL_WHIP, WEAPON_WHIP, &gsn_whip},
    {"polearm", OBJ_VNUM_SCHOOL_POLEARM, WEAPON_POLEARM, &gsn_polearm},
    {NULL, 0, 0, NULL}
};



/* wiznet table and prototype for future flag setting */
const struct wiznet_type wiznet_table[] = {
    {"on", WIZ_ON, L1},
    {"ticks", WIZ_TICKS, L1},
    {"logins", WIZ_LOGINS, L1},
    {"sites", WIZ_SITES, L1},
    {"links", WIZ_LINKS, L1},
    {"deaths", WIZ_DEATHS, L1},
    {"resets", WIZ_RESETS, L1},
    {"mobdeaths", WIZ_MOBDEATHS, L1},
    {"flags", WIZ_FLAGS, L1},
    {"penalties", WIZ_PENALTIES, L1},
    {"saccing", WIZ_SACCING, L1},
    {"levels", WIZ_LEVELS, L1},
    {"secure", WIZ_SECURE, L1},
    {"switches", WIZ_SWITCHES, L1},
    {"snoops", WIZ_SNOOPS, IM},
    {"restore", WIZ_RESTORE, L1},
    {"load", WIZ_LOAD, L1},
    {"newbie", WIZ_NEWBIE, L1},
    {"prefix", WIZ_PREFIX, L1},
    {"spam", WIZ_SPAM, L1},
    {"notes", WIZ_NOTES, L1},
    {NULL, 0, 0}
};

/*
 * Attribute bonus tables.
 */
const struct str_app_type str_app[56] = {
	{-5, -4, 0, 0},		// 0  //
    {-5, -4, 5, 1},		// 1  //
    {-3, -2, 10, 2},
    {-3, -1, 15, 3},		// 3  //
    {-2, -1, 20, 4},
    {-2, -1, 25, 5},		// 5  //
    {-1, 0, 30, 6},
    {-1, 0, 35, 7},
    {0, 0, 40, 8},
    {0, 0, 45, 9},
    {0, 0, 50, 10},		// 10  //
    {0, 0, 55, 11},
    {0, 0, 60, 12},
    {0, 0, 65, 13},		// 13  //
    {0, 1, 70, 14},
    {1, 1, 75, 15},		// 15  //
    {1, 2, 80, 16},
    {2, 3, 85, 22},
    {2, 3, 90, 25},		// 18  //
    {3, 4, 95, 30},
    {3, 5, 100, 35},		// 20  //
    {4, 6, 105, 40},
    {4, 6, 110, 45},
    {5, 7, 115, 50},
    {5, 8, 120, 55},
    {6, 9, 125, 60},	
	{-5, -4, 130, 0},		// 0  //
    {-5, -4, 135, 1},		// 1  //
    {-3, -2, 140, 2},
    {-3, -1, 145, 3},		// 3  //
    {-2, -1, 150, 4},
    {-2, -1, 155, 5},		// 5  //
    {-1, 0, 80, 6},
    {-1, 0, 90, 7},
    {0, 0, 100, 8},
    {0, 0, 100, 9},
    {0, 0, 115, 10},		// 10  //
    {0, 0, 115, 11},
    {0, 0, 130, 12},
    {0, 0, 130, 13},		// 13  //
    {0, 1, 140, 14},
    {1, 1, 150, 15},		// 15  //
    {1, 2, 165, 16},
    {2, 3, 180, 22},
    {2, 3, 200, 25},		// 18  //
    {3, 4, 225, 30},
    {3, 5, 250, 35},		// 20  //
    {4, 6, 300, 40},
    {4, 6, 350, 45},
    {5, 7, 400, 50},
    {5, 8, 450, 55},
    {6, 9, 500, 60} };
/*const struct str_app_type str_app[26] = {
//Hitroll, Damroll, Weight, Max Weight of Weapon)//
    {-5, -4, 0, 0},		// 0  //
    {-5, -4, 3, 1},		// 1  //
    {-3, -2, 3, 2},
    {-3, -1, 10, 3},		// 3  //
    {-2, -1, 25, 4},
    {-2, -1, 55, 5},		// 5  //
    {-1, 0, 80, 6},
    {-1, 0, 90, 7},
    {0, 0, 100, 8},
    {0, 0, 100, 9},
    {0, 0, 115, 10},		// 10  //
    {0, 0, 115, 11},
    {0, 0, 130, 12},
    {0, 0, 130, 13},		// 13  //
    {0, 1, 140, 14},
    {1, 1, 150, 15},		// 15  //
    {1, 2, 165, 16},
    {2, 3, 180, 22},
    {2, 3, 200, 25},		// 18  //
    {3, 4, 225, 30},
    {3, 5, 250, 35},		// 20  //
    {4, 6, 300, 40},
    {4, 6, 350, 45},
    {5, 7, 400, 50},
    {5, 8, 450, 55},
    {6, 9, 500, 60}		// 25   //
};*/



const struct int_app_type int_app[26] = {
    {3},			/*  0 */
    {5},			/*  1 */
    {7},
    {8},			/*  3 */
    {9},
    {10},			/*  5 */
    {11},
    {12},
    {13},
    {15},
    {17},			/* 10 */
    {19},
    {22},
    {25},
    {28},
    {31},			/* 15 */
    {34},
    {37},
    {40},			/* 18 */
    {44},
    {49},			/* 20 */
    {55},
    {60},
    {70},
    {80},
    {85}			/* 25 */
};



const struct wis_app_type wis_app[26] = {
/* Practices */
    {0},			/*  0 */
    {0},			/*  1 */
    {0},
    {0},			/*  3 */
    {0},
    {1},			/*  5 */
    {1},
    {1},
    {1},
    {1},
    {1},			/* 10 */
    {1},
    {1},
    {1},
    {1},
    {2},			/* 15 */
    {2},
    {2},
    {3},			/* 18 */
    {3},
    {3},			/* 20 */
    {3},
    {4},
    {4},
    {4},
    {5}				/* 25 */
};



const struct dex_app_type dex_app[26] = {
/* AC Adjustment */
    {60},			/* 0 */
    {50},			/* 1 */
    {50},
    {40},
    {30},
    {20},			/* 5 */
    {10},
    {0},
    {0},
    {0},
    {0},			/* 10 */
    {0},
    {0},
    {0},
    {0},
    {-10},			/* 15 */
    {-15},
    {-20},
    {-30},
    {-40},
    {-50},			/* 20 */
    {-60},
    {-75},
    {-90},
    {-105},
    {-120}			/* 25 */
};


const struct con_app_type con_app[26] = {
    {-4, 20},			/*  0 */
    {-3, 25},			/*  1 */
    {-2, 30},
    {-2, 35},			/*  3 */
    {-1, 40},
    {-1, 45},			/*  5 */
    {-1, 50},
    {0, 55},
    {0, 60},
    {0, 65},
    {0, 70},			/* 10 */
    {0, 75},
    {0, 80},
    {0, 85},
    {0, 88},
    {1, 90},			/* 15 */
    {1, 95},
    {1, 97},
    {2, 99},			/* 18 */
    {2, 99},
    {3, 99},			/* 20 */
    {3, 99},
    {4, 99},
    {4, 99},
    {5, 99},
    {5, 99}			/* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table[] = {
/*    name                      color   proof, full, thirst, food, ssize */
    {"water", "clear", {0, 1, 10, 0, 16}},
    {"beer", "amber", {12, 1, 8, 0, 12}},
    {"red wine", "burgundy", {30, 1, 8, 0, 5}},
    {"ale", "brown", {15, 1, 8, 0, 12}},
    {"dark ale", "dark", {16, 1, 8, 0, 12}},

    {"whisky", "golden", {120, 1, 5, 0, 2}},
    {"lemonade", "pink", {0, 1, 9, 0, 12}},
    {"firebreather", "boiling", {190, 0, 4, 0, 2}},
    {"local specialty", "clear", {151, 1, 3, 0, 2}},
    {"slime mold juice", "green", {0, 2, -8, 0, 2}},

    {"milk", "white", {0, 2, 9, 0, 12}},
    {"tea", "tan", {0, 1, 8, 0, 6}},
    {"coffee", "black", {0, 1, 8, 0, 6}},
    {"blood", "red", {0, 2, -1, 0, 6}},
    {"salt water", "clear", {0, 1, -2, 0, 1}},

    {"coke", "brown", {0, 2, 9, 0, 12}},
    {"root beer", "brown", {0, 2, 9, 0, 12}},
    {"elvish wine", "green", {35, 2, 8, 0, 5}},
    {"white wine", "golden", {28, 1, 8, 0, 5}},
    {"champagne", "golden", {32, 1, 8, 0, 5}},

    {"mead", "honey-colored", {34, 2, 8, 0, 12}},
    {"rose wine", "pink", {26, 1, 8, 0, 5}},
    {"benedictine wine", "burgundy", {40, 1, 8, 0, 5}},
    {"vodka", "clear", {130, 1, 5, 0, 2}},
    {"cranberry juice", "red", {0, 1, 9, 0, 12}},

    {"orange juice", "orange", {0, 2, 9, 0, 12}},
    {"absinthe", "green", {200, 1, 4, 0, 2}},
    {"brandy", "golden", {80, 1, 5, 0, 4}},
    {"aquavit", "clear", {140, 1, 5, 0, 2}},
    {"schnapps", "clear", {90, 1, 5, 0, 2}},

    {"icewine", "purple", {50, 2, 6, 0, 5}},
    {"amontillado", "burgundy", {35, 2, 8, 0, 5}},
    {"sherry", "red", {38, 2, 7, 0, 5}},
    {"framboise", "red", {50, 1, 7, 0, 5}},
    {"rum", "amber", {151, 1, 4, 0, 2}},

    {"cordial", "clear", {100, 1, 5, 0, 2}},
    {"hair of the dog", "clear", {-40, 0, 0, 0, 6}},
    {NULL, NULL, {0, 0, 0, 0, 0}}
};

const char *color_race_menu =
    "\r\n"
    "\r\n"
    "\r\n"
    "     {R+--------------------------------------------------------------+{x\r\n"
    "     {R|   Human      Wolf      Dwarf     Minotaur     Wraith         |{x\r\n"
    "     {R|        Sprite   Gargoyle    Demon       Vampire    Troll     |{x\r\n"
    "     {R|                                                              |{x\r\n"
    "     {R|                       {W ELVES {X{R                                |{x\r\n"
    "     {R|     {y     HighElf  {c   GreyElf  {g   Sylvan  {m   Drow{R             |{x\r\n"
    "     {R|                                                              |{x\r\n"
    "     {R|                           {WDRAGONS{R                            |{x\r\n"
    "     {R|  RedDrg    {GGrnDrg    {BBluDrg    {WWhtDrg   {DBlkDrg      {yGldDrg{R          |{x\r\n"
    "     {R+--------------------------------------------------------------+{x\r\n"
    "\r\n";

const char *bw_race_menu =
    "\r\n"
    "\r\n"
    "\r\n"
    "     +--------------------------------------------------------------+\r\n"
    "     |   Human      Wolf      Dwarf     Minotaur     Wraith         |\r\n"
    "     |        Sprite   Gargoyle    Demon       Vampire		Troll      |\r\n"
    "     |                                                              |\r\n"
    "     |                         Elves                                |\r\n"
    "     |          HighElf    GreyElf    Sylvan    Drow                |\r\n"
    "     |                                                              |\r\n"
    "     |                            DRAGONS                           |\r\n"
    "     |  RedDrg    GreDrg    BluDrg     WhtDrg    BlkDrg  Glddrg     |\r\n"
    "     +--------------------------------------------------------------+\r\n"
    "\r\n";

const char *color_class_menu =
    "\r\n"
    "\r\n"
    "\r\n"
    "     {Y+--------------------------------------------------------------+\r\n"
    "     |      {RMage         Thief      Warrior          Paladin        {Y|\r\n"
    "     |            {RCleric      Monk          Assassin                {Y|\r\n"
    "     +--------------------------------------------------------------+\r\n"
    "{x\r\n";

const char *bw_class_menu =
    "\r\n"
    "\r\n"
    "\r\n"
    "     +--------------------------------------------------------------+\r\n"
    "     |      Mage         Thief      Warrior          Paladin        |\r\n"
    "     |		         Cleric      Monk          Assassin              |\r\n"
    "     +--------------------------------------------------------------+\r\n"
    "\r\n";

const char *ansi_greeting =

" {b    {y,{b               Original DikuMUD by\r\n"
" {b   {y-{!*{y-{b            .Hans Staerfeldt, Katja..            {y,\r\n"  
" {b    {y'{b          .Nyboe, Tom Madsen, Michael.aa         {y-{!*{y-\r\n"
" {b             aSeifert, and aaa           aaaaaa        {y'\r\n"  
" {b           .Sebastian Hammer                 aaaa\r\n" 
" {b         .Based on MERC.aa                      aa\r\n"   
" {b        a2.1 Code by aa                           a         {y. \r\n" 
" {b       .Hatchet,aaaaaa{r                                     {Y-{R*{y-\r\n"
" {b      a.Furey, and.a  {r      ___ ____   ___   _______________{y'{r_ ___\r\n"
" {b      .Kahn.aaaaaaa   {r     /   \\|  |  /   \\ /   \\_    _/|  \\  |   \\\r\n"
" {b      aaaaaaaaaaaa    {r    /  {Go{r  \\  |_/_ {Co{r  \\  |  \\|  |  :  |  | {bo{r  \\\r\n"
" {b      .ROM 2.4.aaa    {r   /       \\  :  \\_   \\ |   \\  |     |  |     \\\r\n"
" {b      a.Copyright a   {r  /    _    \\____/ \\__/_|___/__/__|__/__| _    \\\r\n"
" {b      .(c) 1993-1996  {r  \\___/ \\___/.{gLand of the Three Moons{r.___/ \\___/\r\n"
" {b       .Russ Taylor.aa\r\n"
" {b        aaaaaaaaaaaaaaa                           a\r\n"    
" {b         aaaaaaaaaaaaaaaaa                      aa\r\n"   
" {b           aaaaaaaaaaaaaaaaa                 aaaa\r\n"
" {b             aaaaaaaaaaaaaaaaa           aaaaaa            {y,\r\n"
" {b   {y,{b          aaaa.Art - Bethor '98.aaaaaaaa              {y-{!*{y-\r\n"
" {b  {y-{!*{y-       {b     aaaa.Whisperwolf '02.aaa    {y,             {y'\r\n"
" {b   {y'{b                aaaaaaaaaaaaaaaa        {y-{!*{y-\r\n"
" {y                                             '\r\n"
"{gType Your Name To Enter:{X "
;



const char *no_ansi_greeting =
    "\r\n"
    "                                         /--\\\r\n"
    "          ___ ____   ___   ______________\\__/___\r\n"
    "         /   \\|  |  /   \\ /   \\_    _/|  \\  |   \\\r\n"
    "        /  o  \\  |_/_ o  \\  |  \\|  |  :  |  | o  \\\r\n"
    "       /       \\  :  \\_   \\ |   \\  |     |  |     \\\r\n"
    "      /    _    \\____/ \\__/_|___/__/__|__/__| _    \\\r\n"
    "      \\___/ \\___/                        \\___/ \\___/\r\n"
    "\r\n"
    "               . .. ...(Alanthia MUD)... .. .\r\n"
    "\r\n"
    "     Original DikuMUD by Hans Staerfeldt, Katja Nyboe,\r\n"
    "     Tom Madsen, Michael Seifert, and Sebastian Hammer\r\n"
    "     Based on MERC 2.1 Code by Hatchet, Furey, and Kahn\r\n"
    "        ROM 2.4 Copyright (c) 1993-1996 Russ Taylor\r\n"
    "                       Art - Bethor '98\r\n"
    "\r\n" "\r\n" "\r\n" "Type your name to enter: ";


const char *fill_color[] = {
    "{r", "{r", "{r", "{r", "{r",
    "{r", "{y", "{y", "{y", "{y",
    "{g", "{g", "{g", "{g", "{g",
    "{g", "{g", "{g", "{g", "{g",
    "{g", "{w", "{w", "{w", "{w",
    "{w", NULL
};



/* 
 * Senses
 */

const char *hea_name[] = {
    "None", "Deaf", "Deaf", "Deaf", "Deaf", "Deaf",
    "Bad", "Bad", "Bad", "Bad", "Bad",
    "Average", "Average", "Average", "Average", "Average",
    "Good", "Good", "Good", "Good", "Good",
    "Excellent", "Excellent", "Excellent", "Acute", "Acute"
};

const char *sme_name[] = {
    "None", "Useless", "Useless", "Useless", "Useless", "Useless",
    "Bad", "Bad", "Bad", "Bad", "Bad",
    "Average", "Average", "Average", "Average", "Average",
    "Good", "Good", "Good", "Good", "Good",
    "Excellent", "Excellent", "Excellent", "Acute", "Acute"
};

const char *sig_name[] = {
    "None", "Blind", "Blind", "Blind", "Blind", "Blind",
    "Bad", "Bad", "Bad", "Bad", "Bad",
    "Average", "Average", "Average", "Average", "Average",
    "Good", "Good", "Good", "Good", "Good",
    "Excellent", "Excellent", "Excellent", "Acute", "Acute"
};

const char *tou_name[] = {
    "None", "Numb", "Numb", "Numb", "Numb", "Numb",
    "Bad", "Bad", "Bad", "Bad", "Bad",
    "Average", "Average", "Average", "Average", "Average",
    "Good", "Good", "Good", "Good", "Good",
    "Excellent", "Excellent", "Excellent", "Acute", "Acute"
};

const char *tas_name[] = {
    "None", "Numb", "Numb", "Numb", "Numb", "Numb",
    "Bad", "Bad", "Bad", "Bad", "Bad",
    "Average", "Average", "Average", "Average", "Average",
    "Good", "Good", "Good", "Good", "Good",
    "Excellent", "Excellent", "Excellent", "Acute", "Acute"
};

const char *per_name[] = {
    "None", "Unaware", "Unaware", "Unaware", "Unaware", "Unaware",
    "Bad", "Bad", "Bad", "Bad", "Bad",
    "Average", "Average", "Average", "Average", "Average",
    "Good", "Good", "Good", "Good", "Good",
    "Excellent", "Excellent", "Excellent", "Acute", "Acute"
};

/*
 * Stats
 */

const char *str_name[] = {
    "Feeble", "Feeble", "Wimpy", "Wimpy", "Nebbish",
    "Nebbish", "Puny", "Puny", "Weak", "Weak",
    "Strong", "Strong", "Strong", "Strong", "Strong",
    "Muscular", "Muscular", "Muscular", "Powerful", "Powerful",
    "Powerful", "Forceful", "Forceful", "Mighty", "Mighty",
    "Titanic", NULL
};

const char *int_name[] = {
    "Stupid", "Stupid", "Slow", "Slow", "Dull",
    "Dull", "Bemused", "Bemused", "Uneducated", "Uneducated",
    "Intelligent", "Intelligent", "Intelligent", "Intelligent",
	"Intelligent",
    "Clever", "Clever", "Clever", "Brainy", "Brainy",
    "Brainy", "Sagacious", "Sagacious", "Brilliant", "Brilliant",
    "Genius", NULL
};

const char *wis_name[] = {
    "Servile", "Servile", "Weak-willed", "Weak-willed", "Pushover",
    "Pushover", "Irresolute", "Irresolute", "Flexible", "Flexible",
    "Willful", "Willful", "Willful", "Willful", "Willful",
    "Deliberate", "Deliberate", "Deliberate", "Headstrong", "Headstrong",
    "Headstrong", "Resolute", "Resolute", "Autocratic", "Autocratic",
    "Unconquerable", NULL
};

const char *dex_name[] = {
    "Bumbling", "Bumbling", "Stumbling", "Stumbling", "Lumbering",
    "Lumbering", "Clumsy", "Clumsy", "Awkward", "Awkward",
    "Dexterous", "Dexterous", "Dexterous", "Dexterous", "Dexterous",
    "Nimble", "Nimble", "Nimble", "Agile", "Agile",
    "Agile", "Quick", "Quick", "Spritely", "Spritely",
    "Mercurial", NULL
};

const char *con_name[] = {
    "Infirm", "Infirm", "Sickly", "Sickly", "Decrepit",
    "Decrepit", "Delicate", "Delicate", "Frail", "Frail",
    "Healthy", "Healthy", "Healthy", "Healthy", "Healthy",
    "Hearty", "Hearty", "Hearty", "Sanguine", "Sanguine",
    "Sanguine", "Vigorous", "Vigorous", "Robust", "Robust",
    "Indestructable", NULL
};

const char *god_name[] = {
    "CeleDi",
    "Ming-Dao",
    "Rudah",
    "Nikos",
    "Didimus",
    "Byzania",
    "Kazimain",
    "Shama",
    "Sadiq",
    "Aidan",
    NULL
};


const char *quality_name[] = {
    "shoddy",
    "substandard",
    "standard",
    "premium",
    "artifact",
    NULL
};

const char *devotion_name[] = {
    "wayward",
    "borderline",
    "moderate",
    "devout",
    "fanatical",
    NULL
};

const char *alignment_name[] = {
    "law",
    "neutrality",
    "chaos",
    NULL
};

const char *lawful_fame_name[] = {
    "unknown",
    "known",
    "distinguished",
    "famous",
    "celebrated",
    NULL
};

const char *chaotic_fame_name[] = {
    "unknown",
    "known",
    "notorius",
    "infamous",
    "hounded",
    NULL
};

const char *skill_level_name[] = {
    "{wUnlearned{W",		/*  0  -  0  */
    "{WUnskilled{W",		/*  1  -  5  */
    "{WInitiate{W",		/*  6  - 10  */
    "{gPoor{W",			/* 11  - 15  */
    "{gBelow Average{W",	/* 16  - 20  */
    "{GAverage{W",		/* 19  - 29  */
    "{GAbove Average{W",	/* 30  - 44  */
    "{yLearned{W",		/* 45  - 59  */
    "{YGood{W",			/* 60  - 77  */
    "{rVery Good{W",		/* 78  - 99  */
    "{rExcellent{W",		/* 100 - 119 */
    "{RMasterful{W",		/* 120 +     */
    NULL
};

const char *one_names[] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
    "ten",
    "eleven",
    "twelve",
    "thirteen",
    "fourteen",
    "fifteen",
    "sixteen",
    "seventeen",
    "eighteen",
    "nineteen"
};

const char *ten_names[] = {
    "zero",
    "ten",
    "twenty",
    "thirty",
    "forty",
    "fifty",
    "sixty",
    "seventy",
    "eighty",
    "ninety"
};

const char *color_names[] = {
    "white",
    "red",
    "green",
    "yellow",
    "blue",
    "pink",
    "orange",
    "black",
    NULL
};
