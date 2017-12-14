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


/*
 * Class table.
 */
const struct class_type class_table[MAX_CLASS] = {
    { "mage", "Mag", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
     {3018, 9618}, 65, 20, 6, 4, 14, TRUE,
     "mage basics", "mage default", TRUE},

    { "cleric", "Cle", STAT_WIS, OBJ_VNUM_SCHOOL_MACE,
     {3003, 9619}, 65, 20, 2, 4, 14, TRUE,
     "cleric basics", "cleric default", TRUE},

    { "thief", "Thi", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
     {3028, 9639}, 65, 20, -4, 4, 14, FALSE,
     "thief basics", "thief default", TRUE},

    { "warrior", "War", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
     {3022, 9633}, 65, 20, -4, 6, 16, FALSE,
     "warrior basics", "warrior default", TRUE},

    { "assassin", "Asn", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
     {3361, 9801}, 65, 20, -4, 4, 14, FALSE,
     "assassin basics", "assassin default", TRUE},

    { "paladin", "Pdn", STAT_CON, OBJ_VNUM_SCHOOL_SWORD,
     {3364, 9803}, 65, 20, -8, 4, 14, TRUE,
     "paladin basics", "paladin default", TRUE},

    { "dragoon", "Dgn", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
     {3062}, 65, 20, -8, 12, 16, FALSE,
     "dragoon basics", "dragoon default", FALSE},

    { "monk", "Mnk", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, TRUE,
     "monk basics", "monk default", TRUE},

    { "druid", "Drd", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, FALSE,
     "monk basics", "monk default", FALSE},

    { "ranger", "Rng", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, FALSE,
     "monk basics", "monk default", FALSE},

    { "blighter", "Bli", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, FALSE,
     "monk basics", "monk default", FALSE},

    { "barbarian", "Brb", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, FALSE,
     "monk basics", "monk default", FALSE},

    { "battlemage", "Bmg", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, FALSE,
     "monk basics", "monk default", FALSE},

    { "necromancer", "Nmr", STAT_STR, OBJ_VNUM_SCHOOL_STAFF,
     {3369}, 65, 20, -8, 4, 14, FALSE,
     "monk basics", "monk default", FALSE}
};
