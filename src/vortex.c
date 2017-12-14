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
 * Code to allow players to set and manipulate vortices for recall.
 */

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

DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_look);

void do_vortex( CHAR_DATA *ch, char *argument ) {

	OBJ_DATA *vortex;
	char arg[MIL];

	argument = one_argument (argument, arg);

	// player didn't type an argument and so gets fed a help file.
	if (arg[0] == '\0') {
		do_help( ch, "vortex");
		return;
	}

	vortex = get_obj_room_by_vnum( ch->in_room->vnum, OBJ_VNUM_VORTEX );	

	if (!vortex || !can_see_obj(ch, vortex)) {
		send_to_char("You can't find that in this room.\r\n", ch);
		return;
	}
	
	if (vortex->pIndexData->vnum != OBJ_VNUM_VORTEX) {
		send_to_char("That's not a vortex.\r\n", ch);
		return;
	}

	if (!str_prefix(arg, "activate")) {

		// first time activation.. 
		if (vortex->level < 2) {
			vortex->level = ch->level;
			vortex->owner = str_dup( ch->name );
			vortex->hidden = TRUE;
			ch->vortex = vortex->in_room->vnum;
			act("$p turns red and fades from view.", ch, vortex, NULL, TO_CHAR);
			act("$p turns red and fades from view.", ch, vortex, NULL, TO_ROOM);
			send_to_char("You will now recall into this room.\r\n", ch );
			return;
		}

		// vortex is in use, just adding it to some chars
		ch->vortex = vortex->in_room->vnum;
		send_to_char("You will now recall into this room.\r\n", ch );
		return;

	} else if (!str_prefix(arg, "reveal")) {
		
		vortex->hidden = FALSE;
		vortex->level -= 3;
		send_to_char("You've revealed a vortex!\r\n", ch);
		act("$n has revealed $p.", ch, vortex, NULL, TO_ROOM);
	
		if ( vortex->level < 2 ) {
			act("$p shimmers and dissapears in a puff of smoke.", ch, vortex, NULL, TO_CHAR);
			act("$p shimmers and dissapears in a puff of smoke.", ch, vortex, NULL, TO_ROOM);
			extract_obj(vortex);
			if ( ch->vortex == ch->in_room->vnum )
				ch->vortex = 0;
		}

		return;

	} else if (!str_prefix(arg, "hide")) {

		if (str_cmp(vortex->owner, ch->name)) {
			send_to_char("That's not yours to hide.\r\n", ch);
			return;
		}

        vortex->hidden = TRUE;
        send_to_char("You've hidden the vortex.\r\n", ch);
        act("$n hides $p from sight.", ch, vortex, NULL, TO_ROOM);

        vortex->level -= 3;
        if ( vortex->level < 2 ) {
            act("$p shimmers and dissapears in a puff of smoke.", ch, vortex, NULL, TO_ROOM);
            act("$p shimmers and dissapears in a puff of smoke.", ch, vortex, NULL, TO_CHAR);
            extract_obj(vortex);
			if( ch->vortex == ch->in_room->vnum )
            ch->vortex = 0;
        }

        return;
	}

	// probably should never get here, but.. 
	do_help( ch, "vortex");
	return;

}

/*
 * if player has a vortex set, give a choice of going there
 * otherwise, fall back to the vortex at temple
 * unless that doesn't exist, in which case the player is fucked
 */

void do_recall(CHAR_DATA * ch, char *argument)
{   
    char buf[MAX_STRING_LENGTH];
    char arg[MIL];
    CHAR_DATA *victim;
	OBJ_DATA *vortex = NULL;
    ROOM_INDEX_DATA *location = NULL;
    ROOM_INDEX_DATA *old_room;
    
    act("$n prays for transportation!", ch, 0, 0, TO_ROOM);
	argument = one_argument (argument, arg);

	// we're going for the default of recalling to temple of putrifaction
	if ( arg[0] == '\0' ) {

		if ( (location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL ) {
        	send_to_char("Oh my. The temple seems to have dissapeared.\n\r", ch);
	        return;
		}
	
		if ( ( vortex = get_obj_room_by_vnum( ROOM_VNUM_TEMPLE, OBJ_VNUM_VORTEX ) ) == NULL ) {
        	send_to_char("There doesn't seem to be a vortex available.\n\r", ch);
			return;
		}	

		// fall thru to success
	
	// or we're trying to see a list of places we can recall
	} else if (!str_prefix(arg, "show")) {

   	// hardcoded hackery for testing
		int count = 0;

		send_to_char( "\r\nYou can currently recall to the following rooms:\r\n", ch );

		if ( ( vortex = get_obj_room_by_vnum( ROOM_VNUM_TEMPLE, OBJ_VNUM_VORTEX ) ) != NULL ) {
			sprintf( buf, "%2d) A level %d vortex left by %s at %s.\r\n", 
				count, vortex->level, ( vortex->owner ? vortex->owner : "no one"),
				vortex->in_room->name); 
			send_to_char( buf, ch );
        }

		count++;

		if ( ( vortex = get_obj_room_by_vnum( ch->vortex, OBJ_VNUM_VORTEX ) ) != NULL ) {
			sprintf( buf, "%2d) A level %d vortex left by %s at %s.\r\n", 
				count, vortex->level, ( vortex->owner ? vortex->owner : "no one"),
				vortex->in_room->name); 
			send_to_char( buf, ch );
		}

		return;

	// or we're trying to recall to one of them
	} else if ( is_number( arg ) ) {
 
		int num = atoi (arg );

		if ( num == 0 ) {
			do_recall(ch, "");
			return;
		}

		if ( ( location = get_room_index( ch->vortex ) ) == NULL) {
            send_to_char("Oh my. That rooms seems to have dissapeared.\n\r", ch);
            return;
        }

        if ( ( vortex = get_obj_room_by_vnum( ch->vortex, OBJ_VNUM_VORTEX ) ) == NULL ) {
            send_to_char("There doesn't seem to be a vortex available in that room.\n\r", ch);
			ch->vortex = 0;
            return;
        }

		// fall thru to success

	}

    if (ch->in_room == location) {
        send_to_char("You're already at recall.\n\r", ch);
        return;
    }
    
	if ( !vortex ) {
		do_help( ch, "vortex" );
		return;
	}

    if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	    || IS_AFFECTED(ch, AFF_CURSE)) {
	    send_to_char("Something seems to be interfering with your magic.\n\r", ch);
	    return;
    }
    
    if ((victim = ch->fighting) != NULL) {
	    int lose, skill;
	    
	    skill = get_skill(ch, gsn_recall);
	    
	    if (skill < number_percent()) {
	        check_improve(ch, gsn_recall, FALSE, 6);
	        WAIT_STATE(ch, 4);
	        sprintf(buf, "You failed!.\n\r");
	        send_to_char(buf, ch);
	        return;
	    }

	    lose = (ch->desc != NULL) ? 25 : 50;
	    gain_exp(ch, 0 - lose);
	    sprintf(buf, "You recall from combat!  You lose %d experience.\n\r",
	        lose);
	    send_to_char(buf, ch);
	    stop_fighting(ch, TRUE);
	    check_improve(ch, gsn_recall, TRUE, 4);
    }

    old_room = ch->in_room;

    ch->move /= 2;
    act("$n disappears.", ch, NULL, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, location);

	if ( location->vnum != ROOM_VNUM_TEMPLE ) {
	    vortex->level -= 3;
	    if ( vortex->level < 2 ) {
	        act("$p shimmers and dissapears in a puff of smoke.", ch, vortex, NULL, TO_ROOM);
	        act("$p shimmers and dissapears in a puff of smoke.", ch, vortex, NULL, TO_CHAR);
	        extract_obj(vortex);
			if( ch->vortex == ch->in_room->vnum )
	        ch->vortex = 0;
	    }
	}
    if (ch->pet) {
	    char_from_room(ch->pet);
	    char_to_room(ch->pet, location);
    }

    act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
    do_look(ch, "auto");

    return;
}


