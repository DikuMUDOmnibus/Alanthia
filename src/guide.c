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

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "utils.h"

/* command procedures needed */
DECLARE_DO_FUN(do_follow);
DECLARE_DO_FUN(do_say);

/* fetch the dirs from a help file
char *get_directions( CHAR_DATA *ch, char *argument ) {

	if mob

		match area name to help file

		read dirs
}
*/

// find a guide mob
CHAR_DATA *find_mob_by_act_flag(CHAR_DATA * ch, long act_flag)
{
    CHAR_DATA *guide;

    if (IS_NPC(ch))
    return NULL;

    for (guide = ch->in_room->people; guide; guide = guide->next_in_room) {
    	if (IS_NPC(guide) && IS_SET(guide->act, act_flag))
        	break;
    }

    if (guide && !can_see(guide, ch)) {
    	do_say(guide, "I'm not leading someone I can't see!");
    	return NULL;
    }

    return guide;
}


// lead a char along a path



// 
void do_guide(  CHAR_DATA *ch, char *argument ) {

	CHAR_DATA *guide;

	if (IS_NPC(ch))
	    return;
    
    if ((guide = find_mob_by_act_flag(ch, ACT_GUIDE)) == NULL) {
    	send_to_char("There is no one here to guide you.\r\n", ch);
    	return;
    }

	do_follow( ch, guide->name );

}














