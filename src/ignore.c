/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.						   *
 ***************************************************************************/
     
    
/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*       ROM has been brought to you by the ROM consortium		   *
*           Russ Taylor (rtaylor@efn.org)				   *
*           Gabrielle Taylor						   *
*           Brian Moore (zump@rom.org)					   *
*       By using this code, you have agreed to follow the terms of the	   *
*       ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/ 
    
#if defined(macintosh)
#include <types.h>
#else	/*  */
#include <sys/types.h>
#endif	/*  */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
    bool is_ignoring(CHAR_DATA * ch, CHAR_DATA * victim) 
{
    int pos;
    CHAR_DATA * rch;
    if (ch->desc == NULL)
	rch = ch;
    
    else
	rch = ch->desc->original ? ch->desc->original : ch;
    for (pos = 0; pos < MAX_IGNORE; pos++)
	 {
	if (rch->pcdata->ignore[pos] == NULL)
	    break;
	if (!str_cmp(rch->pcdata->ignore[pos], victim->name))
	    return TRUE;
	}
    return FALSE;
}
void do_ignore(CHAR_DATA * ch, char *argument) 
{
    CHAR_DATA * victim, *rch;
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    int pos;
    one_argument(argument, arg);
    if (ch->desc == NULL)
	rch = ch;
    
    else
	rch = ch->desc->original ? ch->desc->original : ch;
    if (IS_NPC(rch))
	return;
    if (arg[0] == '\0')
	 {
	send_to_char("Who do you want to ignore?\n\r", ch);
	return;
	}
    if ((victim = get_char_world(rch, argument)) == NULL)
	 {
	send_to_char("They aren't here.\n\r", ch);
	return;
	}
    if (IS_NPC(victim))
	 {
	send_to_char("Ignore a mob?  I don't think so.\n\r", ch);
	return;
	}
    if (ch == victim)
	 {
	send_to_char
	    ("I don't think you really want to ignore yourself.\n\r", ch);
	return;
	}
    for (pos = 0; pos < MAX_IGNORE; pos++)
	 {
	if (rch->pcdata->ignore[pos] == NULL)
	    break;
	if (!str_cmp(arg, rch->pcdata->ignore[pos]))
	     {
	    free_string(rch->pcdata->ignore[pos]);
	    rch->pcdata->ignore[pos] = NULL;
	    sprintf(buf, "You stop ignoring %s.\n\r", victim->name);
	    send_to_char(buf, ch);
	    sprintf(buf, "%s stops ignoring you.\n\r", ch->name);
	    send_to_char(buf, victim);
	    return;
	    }
	}
    if (pos >= MAX_IGNORE)
	 {
	send_to_char("You can't ignore anymore people\n\r", ch);
	return;
	}
    rch->pcdata->ignore[pos] = str_dup(arg);
    sprintf(buf, "You now ignore %s.\n\r", victim->name);
    send_to_char(buf, ch);
    sprintf(buf, "%s ignores you.\n\r", ch->name);
    send_to_char(buf, victim);
    return;
}


