/*************************************************************************
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

/* 
 * I'm the model of the modern major general.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "utils.h"

int get_curr_sense(CHAR_DATA * ch, int sense)
{
    return (ch->perm_sense[sense]);
}



char *get_stat_name(CHAR_DATA * ch, int stat)
{

    static char buf[MAX_STRING_LENGTH];

    sprintf(buf, fill_color[get_curr_stat(ch, stat)]);

    switch (stat) {
    case STAT_STR:
	strcat(buf, str_name[get_curr_stat(ch, stat)]);
	strcat(buf, "{x");
	break;
    case STAT_INT:
	strcat(buf, int_name[get_curr_stat(ch, stat)]);
	strcat(buf, "{x");
	break;
    case STAT_DEX:
	strcat(buf, dex_name[get_curr_stat(ch, stat)]);
	strcat(buf, "{x");
	break;
    case STAT_WIS:
	strcat(buf, wis_name[get_curr_stat(ch, stat)]);
	strcat(buf, "{x");
	break;
    case STAT_CON:
	strcat(buf, con_name[get_curr_stat(ch, stat)]);
	strcat(buf, "{x");
	break;
    }

    return buf;
};

char *get_sense_name(CHAR_DATA * ch, int sense)
{

    static char buf[MAX_STRING_LENGTH];

    sprintf(buf, fill_color[get_curr_sense(ch, sense)]);

    switch (sense) {
    case SENSE_HEA:
	strcat(buf, hea_name[get_curr_sense(ch, sense)]);
	strcat(buf, "{x");
	break;
    case SENSE_SME:
	strcat(buf, sme_name[get_curr_sense(ch, sense)]);
	strcat(buf, "{x");
	break;
    case SENSE_SIG:
	strcat(buf, sig_name[get_curr_sense(ch, sense)]);
	strcat(buf, "{x");
	break;
    case SENSE_TAS:
	strcat(buf, tas_name[get_curr_sense(ch, sense)]);
	strcat(buf, "{x");
	break;
    case SENSE_TOU:
	strcat(buf, tou_name[get_curr_sense(ch, sense)]);
	strcat(buf, "{x");
	break;
    case SENSE_PER:
	strcat(buf, per_name[get_curr_sense(ch, sense)]);
	strcat(buf, "{x");
	break;
    }
    return buf;
};


char *get_size_name(CHAR_DATA * ch)
{


    return "butt";
};

char *expand_cost(int cost)
{
    int copper, silver, gold;
    static char buf[MAX_INPUT_LENGTH];

    if (!cost)
	return "no money";

    gold = div(cost, 100).quot;
    silver = div(cost, 100).rem / 10;
    copper = div(cost, 100).rem - (silver * 10);

    buf[0] = '\0';

    if (gold)
	sprintf(buf, "%s %s", strnum(gold), (gold > 1 ? "obol" : "obol"));

    if (gold && silver && !copper)
	sprintf(buf, "%s and ", buf);
    else if (silver && gold)
	sprintf(buf, "%s, ", buf);

    if (silver)
	sprintf(buf, "%s%s %s",
		buf, strnum(silver), (silver > 1 ? "potin" : "potin"));

    if ((gold || silver) && copper)
	sprintf(buf, "%s and ", buf);

    if (copper)
	sprintf(buf, "%s%s %s", buf, strnum(copper), "minissimi");

    return buf;
}

// WDL 6/1/1999
// returns english string for numbers up to 19,999.
// anything > 19999 returns 'many'
// does not return trailing space.
char *strnum(int num)
{
    static char buf[MAX_INPUT_LENGTH];
    int ones;
    int tens;
    int hundreds;
    int thousands;
    char buf2[MAX_STRING_LENGTH];

    buf[0] = '\0';
    buf2[0] = '\0';

    thousands = div(num, 1000).quot;
    hundreds = div(num, 1000).rem / 100;
    tens = div(num, 100).rem / 10;
    ones = div(num, 100).rem - (tens * 10);

    if (num > 19999)
	return "many";
    if (thousands) {
	sprintf(buf2, "%s thousand", one_names[thousands]);
	strcat(buf, buf2);
    }
    if (thousands && hundreds) {
	strcat(buf, ", ");
    } else if (thousands && ones && (!hundreds)) {
	strcat(buf, " and ");
    } else if (thousands && ones) {
	strcat(buf, " ");
    }
    if (hundreds) {
	sprintf(buf2, "%s hundred ", one_names[hundreds]);
	strcat(buf, buf2);
    }
    if (hundreds && (tens || ones)) {
	strcat(buf, "and ");
    }
    if ((tens) && (tens < 2)) {
	sprintf(buf2, "%s ", one_names[ones]);
	strcat(buf, buf2);
    } else if (tens) {
	sprintf(buf2, "%s ", ten_names[tens]);
	strcat(buf, buf2);
    }
    if ((ones) && (tens != 1)) {
	sprintf(buf2, "%s", one_names[ones]);
	strcat(buf, buf2);
    }

    return buf;
}

// WDL - 6/6/1999
// Returns either ch->name if player, or (num/a/an) (short descrip)
// if mob. Returns a single string, no leading or trailing spaces.
char *format_short(CHAR_DATA * ch, int count)
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if IS_NPC
	(ch) {
	sprintf(buf, "%s %s%s", ((count > 1) ? strnum(count)
				 : (!IS_VOWEL(ch->short_descr[0]) ? "a" :
				    "an")), ch->short_descr,
		((count > 1) ? "s" : ""));
    } else {
	sprintf(buf, "%s", ch->name);
    }
    return buf;
}
