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

//    char *name;                 /* name of the race for lookups */
//    char *who_name;             /* colorized name of the race for who list */
//    bool pc_race;               /* can be used by pcs */
//    long act;                   /* "ACT" flags */
//    long aff;                   /* affected by flags */
//    long off;                   /* offensive capabilities */
//    long imm;                   /* immune to */
//    long res;                   /* resistant to */
//    long vuln;                  /* vulnerable to */
//    long form;                  /* animal, mammal, edible, etc */
//    long parts;                 /* body parts */
//    sh_int sex;                 /* allowable sexes */
//    char *skills[5];            /* automatically granted skills */
//    sh_int stats[MAX_STATS];    /* max stats.. */
//    sh_int max_senses[6];       /* smell, sight, hearing, etc */
//    sh_int size;                /* how big is it */
//    sh_int min_level;           /* likely level of a baby of this race */
//    sh_int max_level;           /* level of an adult in its prime */
//    sh_int lifespan;            /* in mud years */
//    char *feeds_on[10];         /* always aggro to food races */
/*




	 {
     "prsmdrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_MAGIC | RES_BASH, VULN_WEAPON,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},



    {
     "reddrg", "{RRed Dragon   ", 63, "red dragon",
     {120, 120, 200, 120, 200, 120, 120, 150},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE, TRUE},

    {
     "grndrg", "{GGreen Dragon ", 63, "green dragon",
     {120, 120, 200, 120, 200, 120, 120, 150},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE, TRUE},

    {
     "bludrg", "{BBlue Dragon  ", 63, "blue dragon",
     {120, 120, 200, 120, 200, 120, 120, 150},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE, TRUE},

    {
     "whtdrg", "{WWhite Dragon ", 63, "white dragon",
     {120, 120, 200, 120, 200, 120, 120, 150},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE, TRUE},

    {
     "blkdrg", "{DBlack Dragon ", 63, "black dragon",
     {120, 120, 200, 120, 200, 120, 120, 150},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE, TRUE},

    {
     "glddrg", "{yGold Dragon  ", 63, "gold dragon",
     {120, 120, 200, 120, 200, 120, 120, 150},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE, TRUE},

    {
     "highelf", "{yHigh Elf     ", 15, "high elf",
     {100, 100, 100, 100, 100, 100, 100, 150},
     {"sneak"},
     {12, 14, 13, 15, 11},
     {32, 38, 42, 38, 32}, SIZE_SMALL, TRUE},

    {
     "greyelf", "{cGrey Elf     ", 15, "grey elf",
     {100, 100, 100, 100, 100, 100, 100, 150},
     {"sneak"},
     {12, 14, 13, 15, 11},
     {30, 42, 36, 38, 30}, SIZE_SMALL, TRUE},

    {
     "sylvan", "{gSylvan Elf   ", 15, "sylvan elf",
     {100, 125, 100, 120, 100, 100, 100, 150},
     {"sneak"},
     {12, 14, 13, 15, 11},
     {38, 34, 36, 40, 34}, SIZE_SMALL, TRUE},

     { "chicken", "{YChicken      ", 60, "chicken",
     {100, 100, 100, 100, 100, 100, 100, 150},
     {"dodge" "berserk", "second attack", "third attack"},
     {8, 10, 10, 16, 10},
     {30, 32, 32, 50, 34}, SIZE_SMALL , FALSE},

     { "prsmdrg", "{GP{Br{Ri{Ys{Mm {WDragon ", 63, "prismatic dragon",
     {110, 110, 110, 110, 110, 110, 110, 110},
     {"bash"},
     {16, 16, 16, 16, 16},
     {44, 36, 38, 26, 42}, SIZE_HUGE , TRUE},

     { NULL, NULL, 0, "",
     {100, 100, 100, 100, 100, 100, 100},
     {""},
     {13, 13, 13, 13, 13},
     {18, 18, 18, 18, 18}, 0, FALSE}
};
*/
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
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   
*	ROM has been brought to you by the ROM consortium
*	    Russ Taylor (rtaylor@efn.org)				   
*	    Gabrielle Taylor						   
*	    Brian Moore (zump@rom.org)					  
*	By using this code, you have agreed to follow the terms of the	   
*	ROM license, in the file Rom24/doc/rom.license			   
***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"

struct race_type *race_sorted;

//    char *name;         /* name of the race for lookups */
//    char *who_name;     /* colorized name of the race for who list */
//    char *gen_name;     /* awful, awful hack to make 2 word names work */
//    bool pc_race;       /* can be used by pcs */
//    long act;           /* "ACT" flags */
//    long aff;           /* affected by flags */
//    long off;           /* offensive capabilities */
//    long imm;           /* immune to */
//    long res;           /* resistant to */
//    long vuln;          /* vulnerable to */
//    long form;          /* animal, mammal, edible, etc */
//    long parts;         /* body parts */
//    sh_int sex;         /* allowable sexes */
//    char *skills[5];        /* automatically granted skills */
//    sh_int stats[MAX_STATS];    /* max stats.. */
//    sh_int max_senses[6];   /* smell, sight, hearing, etc */
//    sh_int size;        /* how big is it */

/*
 * Standard Humanoid Parts List:  
 *      A|B|C|D|E|F|G|H|I|J|K,
 *
 */

/* race table */
const struct race_type race_table[] = {

    {
     "dont_use_me",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Perm bits    */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Work         */
     0,				/* Parts        */
     SEX_ANY,
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Sensess  */
     100			/* Size         */
     },

    {
     "unique",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Perm bits    */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Work         */
     0,				/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     100			/* Size         */
     },

    {
     "kesthai bakshar",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"corrode"},		/* Intrinsics   */
     {20, 21, 18, 18, 17},	/* Max Stats    */
     {10, 15, 20, 25, 5, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "kesthai svetu",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"freeze"},		/* Intrinsics   */
     {20, 18, 21, 18, 17},	/* Max Stats    */
     {20, 18, 21, 18, 17, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "kesthai verde",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"spray"},			/* Intrinsics   */
     {21, 18, 18, 17, 20},	/* Max Stats    */
     {21, 18, 18, 17, 20, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "kesthai flagrare",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"spray"},			/* Intrinsics   */
     {21, 18, 18, 17, 20},	/* Max Stats    */
     {21, 18, 18, 17, 20, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "kesthai azul",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"jolt"},			/* Intrinsics   */
     {21, 20, 18, 18, 17},	/* Max Stats    */
     {21, 20, 18, 18, 17, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "pygmy",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"fast", "sneak"},		/* Intrinsics   */
     {16, 16, 18, 22, 18},	/* Max Stats    */
     {16, 16, 18, 22, 18, 25},	/* Max Stats    */
     70				/* Size         */
     },

    {
     "taiga giant",		/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"bash", "stonewalk"},	/* Intrinsics   */
     {22, 16, 17, 17, 21},	/* Max Stats    */
     {22, 16, 17, 17, 21, 25},	/* Max Stats    */
     175			/* Size         */
     },
    {
     "forest giant",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"bash", "stonewalk"},	/* Intrinsics   */
     {22, 16, 17, 17, 21},	/* Max Stats    */
     {22, 16, 17, 17, 21, 25},	/* Max Stats    */
     175			/* Size         */
     },

    {
     "stone giant",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"bash", "stonewalk"},	/* Intrinsics   */
     {22, 16, 17, 17, 21},	/* Max Stats    */
     {22, 16, 17, 17, 21, 25},	/* Max Stats    */
     175			/* Size         */
     },
    {
     "giant",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"bash", "stonewalk"},	/* Intrinsics   */
     {22, 16, 17, 17, 21},	/* Max Stats    */
     {22, 16, 17, 17, 21, 25},	/* Max Stats    */
     175			/* Size         */
     },
    {
     "mul",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_MALE,			/* Sex allowed  */
     {"berserk"},		/* Intrinsics   */
     {20, 16, 19, 17, 20},	/* Max Stats    */
     {20, 16, 19, 17, 20, 25},	/* Max Stats    */
     125			/* Size         */
     },

    {
     "human",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {18, 18, 18, 18, 18},	/* Max Stats    */
     {18, 18, 18, 18, 18, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "grey elf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {"infravision"},		/* Intrinsics   */
     {17, 21, 18, 20, 17},	/* Max Stats    */
     {17, 21, 18, 20, 17, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "high elf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 18, 21, 20, 17},	/* Max Stats    */
     {17, 18, 21, 20, 17, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "sylvan elf",		/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 17, 18, 21, 20},	/* Max Stats    */
     {17, 17, 18, 21, 20, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "drow",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 18, 18, 21, 18},	/* Max Stats    */
     {17, 18, 18, 21, 18, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "thri-kreen",		/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_INSECT,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 18, 18, 21, 18},	/* Max Stats    */
     {17, 18, 18, 21, 18, 25},	/* Max Stats    */
     150			/* Size         */
     },
    {
     "half elf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 18, 18, 19, 18},	/* Max Stats    */
     {17, 18, 18, 19, 18, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "troll",			/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {21, 16, 17, 17, 22},	/* Max Stats    */
     {21, 16, 17, 17, 22, 25},	/* Max Stats    */
     192			/* Size         */
     },

    {
     "gargoyle",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 18, 17, 16, 20},	/* Max Stats    */
     {20, 18, 17, 16, 20, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "demonspawn",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {19, 21, 17, 17, 19},	/* Max Stats    */
     {19, 21, 17, 17, 19, 25},	/* Max Stats    */
     155			/* Size         */
     },

    {
     "minotaur",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | I | J | K | R,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 17, 18, 20},	/* Max Stats    */
     {20, 16, 17, 18, 20, 25},	/* Max Stats    */
     145			/* Size         */
     },

    {
     "skaven",			/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | I | J | K | R,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 17, 18, 20},	/* Max Stats    */
     {20, 16, 17, 18, 20, 25},	/* Max Stats    */
     145			/* Size         */
     },
    {
     "bastetti",		/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 19, 17, 19, 18},	/* Max Stats    */
     {17, 19, 17, 19, 18, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "orc",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {19, 17, 17, 16, 21},	/* Max Stats    */
     {19, 17, 17, 16, 21, 25},	/* Max Stats    */
     145			/* Size         */
     },

    {
     "sprite",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {16, 20, 17, 21, 17},	/* Max Stats    */
     {16, 20, 17, 21, 17, 25},	/* Max Stats    */
     16				/* Size         */
     },

    {
     "nixie",			/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {16, 20, 17, 21, 17},	/* Max Stats    */
     {16, 20, 17, 21, 17, 25},	/* Max Stats    */
     16				/* Size         */
     },
    {
     "pixie",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {16, 20, 17, 21, 17},	/* Max Stats    */
     {16, 20, 17, 21, 17, 25},	/* Max Stats    */
     16				/* Size         */
     },
    {
     "nymph",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {16, 20, 19, 19, 17},	/* Max Stats    */
     {16, 20, 19, 19, 17, 25},	/* Max Stats    */
     24				/* Size         */
     },

    {
     "leprechaun",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {16, 20, 17, 21, 17},	/* Max Stats    */
     {16, 20, 17, 21, 17, 25},	/* Max Stats    */
     96				/* Size         */
     },

    {
     "slith",			/* Name         */
     TRUE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_REPTILE,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {17, 18, 21, 17, 19},	/* Max Stats    */
     {17, 18, 21, 17, 19, 25},	/* Max Stats    */
     120			/* Size         */
     },

    {
     "brownie",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAGICAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {16, 17, 18, 20, 20},	/* Max Stats    */
     {16, 17, 18, 20, 20, 25},	/* Max Stats    */
     48				/* Size         */
     },

    {
     "grey dwarf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {18, 16, 20, 16, 20},	/* Max Stats    */
     {18, 16, 20, 16, 20, 25},	/* Max Stats    */
     96				/* Size         */
     },

    {
     "mountain dwarf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 18, 16, 21},	/* Max Stats    */
     {20, 16, 18, 16, 21, 25},	/* Max Stats    */
     96				/* Size         */
     },

    {
     "deep dwarf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {19, 16, 17, 19, 20},	/* Max Stats    */
     {19, 16, 17, 19, 20, 25},	/* Max Stats    */
     96				/* Size         */
     },


    {
     "kender",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {18, 16, 17, 20, 20},	/* Max Stats    */
     {18, 16, 17, 20, 20, 25},	/* Max Stats    */
     96				/* Size         */
     },


    {
     "black dwarf",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE | FORM_BIPED | FORM_MAMMAL,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {19, 16, 17, 20, 19},	/* Max Stats    */
     {19, 16, 17, 20, 19, 25},	/* Max Stats    */
     96				/* Size         */
     },


    {
     "bat",
     FALSE,
     0,
     AFF_FLYING | AFF_DARK_VISION,
     OFF_DODGE | OFF_FAST,
     0,
     0,
     VULN_LIGHT,
     FORM_EDIBLE | FORM_BIRD | FORM_MAMMAL | FORM_ANIMAL,			/* Form         */
     A | C | D | E | F | H | J | K | P,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     8				/* Size         */
     },

    {
     "bear",
     FALSE,
     ACT_OUTDOORS,
     0,
     OFF_CRUSH | OFF_DISARM | OFF_BERSERK,
     0,
     RES_BASH | RES_COLD,
     0,
     FORM_EDIBLE | FORM_MAMMAL | FORM_ANIMAL,			/* Form         */
     A | B | C | D | E | F | H | J | K | U | V,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 16, 19, 14, 21},	/* Max Stats    */
     {25, 16, 19, 14, 21, 25},	/* Max Stats    */
     120			/* Size         */
     },

    {
     "cat",
     FALSE,
     0,
     AFF_DARK_VISION,
     OFF_FAST | OFF_DODGE,
     0,
     0,
     0,
     FORM_EDIBLE | FORM_MAMMAL | FORM_ANIMAL,			/* Form         */
     A | C | D | E | F | H | J | K | Q | U | V,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {5, 16, 19, 14, 21, 25},	/* Max Stats    */
     12				/* Size         */
     },

    {
     "centipede",
     FALSE,
     ACT_SCAVENGER,
     0,
     0,
     0,
     RES_PIERCE | RES_COLD,
     VULN_BASH,
     FORM_POISON | FORM_EDIBLE | FORM_INSECT,	/* Form         */
     A | C | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {5, 16, 19, 14, 21, 25},	/* Max Stats    */
     20				/* Size         */
     },

    {
     "insect",
     FALSE,
     ACT_SCAVENGER,
     0,
     0,
     0,
     RES_PIERCE | RES_COLD,
     VULN_BASH,
     FORM_POISON | FORM_EDIBLE | FORM_INSECT,	/* Form         */
     A | C | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {5, 16, 19, 14, 21, 25},	/* Max Stats    */
     20				/* Size         */
     },

    {
     "dog",
     FALSE,
     0,
     0,
     OFF_FAST | ASSIST_RACE,
     0,
     0,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_MAMMAL,	/* Form         */
     A | C | D | E | F | H | J | K | U | V,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     72				/* Size         */
     },

    {
     "doll",
     FALSE,
     0,
     0,
     0,
     IMM_COLD | IMM_POISON | IMM_HOLY | IMM_NEGATIVE | IMM_MENTAL |
     IMM_DISEASE | IMM_DROWNING,
     RES_BASH | RES_LIGHT,
     VULN_SLASH | VULN_FIRE | VULN_ACID | VULN_LIGHTNING | VULN_ENERGY,
		 FORM_CONSTRUCT | FORM_BIPED,
     A | B | C | G | H | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     24				/* Size         */
     },

    {
     "fox",
     FALSE,
     ACT_OUTDOORS,
     AFF_DARK_VISION,
     OFF_FAST | OFF_DODGE,
     0,
     0,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_MAMMAL,	/* Form         */
     A | C | D | E | F | H | J | K | Q | V,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     24				/* Size         */
     },

    {
     "goblin",
     FALSE,
     ACT_WIMPY | ACT_SCAVENGER,
     AFF_INFRARED,
     0,
     0,
     RES_DISEASE,
     VULN_MAGIC,
     FORM_EDIBLE | FORM_ANIMAL | FORM_MAMMAL | FORM_BIPED,	/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     42				/* Size         */
     },

    {
     "hobgoblin",
     FALSE,
     ACT_SCAVENGER,
     AFF_INFRARED,
     0,
     0,
     RES_DISEASE | RES_POISON,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_MAMMAL | FORM_BIPED,	/* Form         */
     A | B | C | D | E | F | G | H | I | J | K | Y,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "kobold",
     FALSE,
     ACT_WIMPY | ACT_SCAVENGER,
     AFF_INFRARED,
     0,
     0,
     RES_POISON,
     VULN_MAGIC,
     FORM_EDIBLE | FORM_ANIMAL | FORM_MAMMAL | FORM_BIPED,	/* Form         */
     A | B | C | D | E | F | G | H | I | J | K | Q,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     45				/* Size         */
     },

    {
     "lizard",
     FALSE,
     0,
     0,
     0,
     0,
     RES_POISON,
     VULN_COLD,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | C | D | E | F | H | K | Q | V,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     20				/* Size         */
     },

    {
     "frog",
     FALSE,
     0,
     0,
     0,
     0,
     RES_POISON,
     VULN_COLD,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | C | D | E | F | H | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     10				/* Size         */
     },

    {
     "pig",
     FALSE,
     0,
     0,
     0,
     0,
     0,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | C | D | E | F | H | J | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     72				/* Size         */
     },

    {
     "rabbit",
     FALSE,
     0,
     0,
     OFF_DODGE | OFF_FAST,
     0,
     0,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | C | D | E | F | H | J | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     20				/* Size         */
     },

    {
     "snake",
     FALSE,
     0,
     0,
     0,
     0,
     RES_POISON,
     VULN_COLD,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | D | E | F | K | L | Q | V | X,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     10				/* Size         */
     },

    {
     "bird",
     FALSE,
     0,
     AFF_FLYING,
     OFF_FAST | OFF_DODGE,
     0,
     0,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | C | D | E | F | H | K | P,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     10				/* Size         */
     },

    {
     "water fowl",
     FALSE,
     0,
     AFF_SWIM | AFF_FLYING,
     0,
     0,
     RES_DROWNING,
     0,
     FORM_EDIBLE | FORM_ANIMAL | FORM_REPTILE | FORM_COLD_BLOOD,	/* Form   */
     A | C | D | E | F | H | K | P,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {20, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     48				/* Size         */
     },

    {
     "wyvern",
     FALSE,
     0,
     AFF_FLYING | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
     OFF_BASH | OFF_FAST | OFF_DODGE,
     IMM_POISON,
     0,
     VULN_LIGHT,
     A | B,
     A | C | D | E | F | H | J | K | Q | V | X,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     240			/* Size         */
     },


    {
     "spider",
     FALSE,
     0,
     AFF_DARK_VISION,
     0,
     0,
     RES_PIERCE | RES_COLD,
     VULN_BASH,
     A | B,
     A | C | C | C | C | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     24				/* Size         */
     },

    {
     "horse",
     FALSE,
     0,
     AFF_DARK_VISION,
     0,
     0,
     RES_PIERCE | RES_COLD,
     VULN_BASH,
     A | B,
     A | C | C | C | C | K,
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 16, 19, 14, 21},	/* Max Stats    */
     {20, 16, 19, 14, 21, 25},	/* Max Stats    */
     24				/* Size         */
     },


    {
     "dragon",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_BASH,			/* Res bits     */
     0,				/* Vuln bits    */
     A | B,
     A | C | D | E | F | J | K | P | Q | U | V | X | L,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     600			/* Size         */
     },

    {
     "hydra",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_BASH,			/* Res bits     */
     0,				/* Vuln bits    */
     A | B,
     A | C | D | E | F | J | K | P | Q | U | V | X | L,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     600			/* Size         */
     },
    {
     "halfling",		/* Name         */
     FALSE,			/* PC Race?     */
     ACT_SCAVENGER,		/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     A | B,
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     72				/* Size         */
     },

    {
     "gnoll",			/* Name         */
     FALSE,			/* PC Race?     */
     ACT_WIMPY,			/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     A | B,
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "giant worm",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     VULN_PIERCE,		/* Vuln bits    */
     A | B,
     A | D | E | F | M,		/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     300			/* Size         */
     },

    {
     "ice worm",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     VULN_PIERCE,		/* Vuln bits    */
     A | B,
     A | D | E | F | M,		/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     325			/* Size         */
     },

    {
     "ogre",			/* Name         */
     FALSE,			/* PC Race?     */
     ACT_SCAVENGER,		/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_BASH,			/* Res bits     */
     0,				/* Vuln bits    */
     A | B,
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     180			/* Size         */
     },

    {
     "drakyri",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_CHARM | RES_MAGIC,	/* Res bits     */
     VULN_IRON,			/* Vuln bits    */
     A | B,
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     150			/* Size         */
     },

    {
     "cow",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     A | B,
     A | C | D | E | F | J | K | R,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     120			/* Size         */
     },

    {
     "shade",			/* Name         */
     FALSE,			/* PC Race?     */
     ACT_UNDEAD,		/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_PIERCE | RES_SLASH,	/* Res bits     */
     0,				/* Vuln bits    */
     C,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "golem",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_PIERCE | RES_BASH,	/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | B | C | D | E | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     230			/* Size         */
     },

    {
     "skeleton",		/* Name         */
     FALSE,			/* PC Race?     */
     ACT_UNDEAD,		/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_PIERCE,		/* Res bits     */
     VULN_BASH,			/* Vuln bits    */
     0,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "gnome",			/* Name         */
     FALSE,			/* PC Race?     */
     ACT_SCAVENGER,		/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     60				/* Size         */
     },

    {
     "mouse",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     C | D | E | F | G | J | K | U,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {1, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     2				/* Size         */
     },

    {
     "rat",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | C | D | E | F | G | J | K | U,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {5, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     6				/* Size         */
     },

    {
     "keddig",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     D | E | F | J | T | V | W,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     20				/* Size         */
     },

    {
     "thrask",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | B | J | R | S | U | V,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     96				/* Size         */
     },

    {
     "basilisk",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | C | E | F | H | J | K | L | O | Q | U | V | X,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     48				/* Size         */
     },

    {
     "gremlin",			/* Name         */
     FALSE,			/* PC Race?     */
     ACT_WIMPY,			/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K | U | V | X,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     36				/* Size         */
     },

    {
     "deer",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | C | E | F | G | J | K | R | T,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     144			/* Size         */
     },

    {
     "githyanki",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Aff bits     */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     0,				/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Form         */
     A | B | C | D | E | F | G | H | I | J | K,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     140			/* Size         */
     },

    {
     "earth elemental",		/* Name         */
     FALSE,			/* PC Race?     */
     ACT_CONSTRUCT,		/* Act bits     */
     0,				/* Aff bits     */
     OFF_BASH,			/* Off bits     */
     IMM_POISON | IMM_DISEASE,	/* Imm bits     */
     RES_BASH | RES_FIRE | RES_COLD | RES_DISEASE,	/* Res bits */
     VULN_MAGIC,		/* Vuln bits    */
     0,				/* Form         */
     A | B | C | G | H,		/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     200			/* Size         */
     },

    {
     "fire elemental",		/* Name         */
     FALSE,			/* PC Race?     */
     ACT_CONSTRUCT,		/* Act bits     */
     0,				/* Aff bits     */
     OFF_BASH,			/* Off bits     */
     IMM_POISON | IMM_DISEASE,	/* Imm bits     */
     RES_BASH | RES_FIRE | RES_COLD | RES_DISEASE,	/* Res bits */
     VULN_MAGIC | VULN_COLD,	/* Vuln bits    */
     0,				/* Form         */
     A | B | C | G | H,		/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     200			/* Size         */
     },

    {
     "wind elemental",		/* Name         */
     FALSE,			/* PC Race?     */
     ACT_CONSTRUCT,		/* Act bits     */
     0,				/* Aff bits     */
     OFF_BASH,			/* Off bits     */
     IMM_POISON | IMM_DISEASE,	/* Imm bits     */
     RES_BASH | RES_FIRE | RES_COLD,	/* Res bits */
     VULN_MAGIC,		/* Vuln bits    */
     0,				/* Form         */
     A | B | C | G | H,		/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     200			/* Size         */
     },

    {
     "water elemental",		/* Name         */
     FALSE,			/* PC Race?     */
     ACT_CONSTRUCT,		/* Act bits     */
     0,				/* Aff bits     */
     OFF_BASH,			/* Off bits     */
     IMM_POISON | IMM_DISEASE,	/* Imm bits     */
     RES_SLASH | RES_FIRE | RES_COLD,	/* Res bits */
     VULN_MAGIC,		/* Vuln bits    */
     0,				/* Form         */
     A | B | C | G | H,		/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     200			/* Size         */
     },

    {
     "wolf",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     AFF_INFRARED,		/* Aff bits     */
     OFF_DODGE | OFF_FAST,	/* Off bits     */
     0,				/* Imm bits     */
     RES_COLD,			/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE,		/* Work         */
     A | C | D | E | F | J | K | Q | T | V,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     72				/* Size         */
     },

    {
     "demon",			/* Name         */
     FALSE,			/* PC Race?     */
     ACT_INTELLIGENT,		/* Act bits     */
     AFF_DARK_VISION,		/* Perm bits    */
     0,				/* Off bits     */
     IMM_DISEASE,		/* Imm bits     */
     RES_CHARM | RES_POISON,	/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Work         */
     A | B | C | D | E | F | G | H | I | J | K | W | L,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     216			/* Size         */
     },

    {
     "abomination",		/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     AFF_DARK_VISION,		/* Perm bits    */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_CHARM | RES_POISON,	/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Work         */
     A | B | C | D | E | F | G | H | I | J | K | W | L,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     250			/* Size         */
     },

    {
     "yeti",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     0,				/* Perm bits    */
     0,				/* Off bits     */
     0,				/* Imm bits     */
     RES_COLD,			/* Res bits     */
     0,				/* Vuln bits    */
     0,				/* Work         */
     A | B | C | D | E | F | G | H | I | J | K | W | L,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     250			/* Size         */
     },

    {
     "lion",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     AFF_INFRARED,		/* Aff bits     */
     OFF_DODGE | OFF_FAST,	/* Off bits     */
     0,				/* Imm bits     */
     RES_COLD,			/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE,		/* Work         */
     A | C | D | E | F | J | K | Q | T | V,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     72				/* Size         */
     },

    {
     "leopard",			/* Name         */
     FALSE,			/* PC Race?     */
     0,				/* Act bits     */
     AFF_SNEAK,			/* Aff bits     */
     OFF_DODGE | OFF_FAST,	/* Off bits     */
     0,				/* Imm bits     */
     RES_COLD,			/* Res bits     */
     0,				/* Vuln bits    */
     FORM_EDIBLE,		/* Work         */
     A | C | D | E | F | J | K | Q | T | V,	/* Parts        */
     SEX_ANY,			/* Sex allowed  */
     {""},			/* Intrinsics   */
     {25, 25, 25, 25, 25},	/* Max Stats    */
     {25, 25, 25, 25, 25, 25},	/* Max Stats    */
     72				/* Size         */
     },

    {
     NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

    {
     "reddrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_FIRE | RES_BASH, VULN_ACID,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},

    {
     "grndrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_POISON | RES_BASH, VULN_FIRE,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},

    {
     "bludrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_LIGHTNING | RES_BASH, VULN_POISON,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},

    {
     "whtdrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_COLD | RES_BASH, VULN_FIRE,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},

    {
     "blkdrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_ACID | RES_BASH, VULN_LIGHTNING,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},

    {
     "glddrg", TRUE,
     0, AFF_FLYING, 0,
     0, RES_LIGHT | RES_BASH, VULN_COLD,
     A | H | Z,
     A | C | D | E | F | H | J | K | L | O | P | Q | U | V | W | X | Y},


    {
     "chicken", FALSE,
     0, 0, 0,
     0, RES_MAGIC, VULN_BASH | VULN_WEAPON,
     A | H | G | V | W, A | C | D | E | F | H | K | P},
