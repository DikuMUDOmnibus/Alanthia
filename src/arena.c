#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "recycle.h"
#include "tables.h"
#include "interp.h"



struct arena_type {

    char *name;
    sh_int players;
    int type;
    sh_int arena_id;

};

const struct arena_type arena_types_table[MAX_ARENAS] = {

    {"Free For All", 0, ARENA_FFA, 0},
    {"Challenge", 0, ARENA_CHALLENGE, 0},
    {"Capture The Flag", 0, ARENA_CTF, 0},
    {"Freeze Tag", 0, ARENA_FREEZETAG, 0},
    {"Clan War", 0, ARENA_CLANWAR, 0},

};
        

struct arena_type running_arenas[MAX_ARENAS];

long global_current_arena_id;
long global_last_arena_id;


/*
 * arena info/status/joining command for players
 */

void do_arena(CHAR_DATA * ch, char *argument)
{
  char *str, arg[MIL];

  str = one_argument(argument, arg);

// show list of running arenas

	if (!strcmp( arg, "show")) {
	show_running_arena_list( ch );
	return;
	}

// join an arena

	if (!strcmp( arg, "join")) {

		if ( ch->arena ) {
			send_to_char( "You're already in an arena.\r\n", ch );
			return;
		}

		//sucky check to make sure we're not just giving out free restores.. 

		if( 
		   	is_affected(ch, gsn_plague)
     		|| is_affected(ch, gsn_poison)
     		|| is_affected(ch, gsn_blindness)
     		|| is_affected(ch, gsn_sleep)
     		|| is_affected(ch, gsn_curse)
     		|| is_affected(ch, gsn_dirt)
     		|| is_affected(ch, skill_lookup("fire breath"))
     		|| is_affected(ch, skill_lookup("calm")) 
			|| is_affected(ch, skill_lookup("chill touch")) 
           ) {

		send_to_char( "You must be healthy before you can join an arena.\r\n" , 
			ch);

			return; 
		}

		ch->arena = global_current_arena_id;
		show_running_arena_list( ch );
		return;
	}

// get out of an arena

	if (!strcmp( arg, "quit")) {
		if(ch->arena) {
			if ( global_current_arena_id != global_last_arena_id ) {
  				arena_death(ch);
				return;
			} else {
				ch->arena = FALSE;
				send_to_char( "You've wussed out of an arena.\r\n", ch);
				return;
			} 
		} else {
			send_to_char( "You're not in an arena.\r\n", ch );
			return;
		}

	}

// challenge someone to a single player arena
// challenge another clan to an arena war
// cleanup arenas

    do_help(ch, "arena");
    return;
}

/*
 * show the list of ppl playing arenas
 */

void show_running_arena_list(CHAR_DATA * ch) {
	char buf[MSL];
  	DESCRIPTOR_DATA *d;

    
    sprintf(buf, "\r\nArena Status:\r\n");
    strcat(buf, "Player:                 Arena:                  Status:\r\n");
    send_to_char(buf, ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
      CHAR_DATA *victim;
      victim = d->original ? d->original : d->character;

      if ((d->connected == CON_PLAYING) && (victim->arena)) {
        sprintf(buf, "%-23s %-23ld %-23s\r\n",
          (can_see(ch,victim) ? victim->name : "Someone"),
          victim->arena,
          (victim->arena == global_last_arena_id) 
            ? "{gPlaying{x" 
            : "{rWaiting{x");
        send_to_char( buf, ch );
      }
    }

    return;
}

/*
 * cleanup an arena member
 */

void leave_arena(CHAR_DATA * ch)
{
    ch->position = POS_STANDING;
		affect_strip(ch, skill_lookup("weaken"));
		affect_strip(ch, skill_lookup("lethargy"));
    affect_strip(ch, gsn_plague);
    affect_strip(ch, gsn_poison);
    affect_strip(ch, gsn_blindness);
    affect_strip(ch, gsn_sleep);
    affect_strip(ch, gsn_curse);
    affect_strip(ch, gsn_dirt);
    affect_strip(ch, skill_lookup("fire breath"));
    affect_strip(ch, skill_lookup("calm"));
		affect_strip(ch, skill_lookup("chill touch"));
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move;
    ch->blood = ch->max_blood;
    update_pos(ch);

    ch->tmp_pkill = FALSE;
    ch->arena = FALSE;

    char_from_room(ch);
    char_to_room(ch, get_room_index(ch->pre_arena_room));

    return;
}

/*
 * replacemant death function for arenites
 */

void arena_death(CHAR_DATA * ch)
{
	// so that arena death can be called from other parts of the code
	if (!ch->arena)
		return;

    leave_arena(ch);
    send_to_char("{R\r\n           0\\/\\/Z3d!! You lose.\r\n\r\n{x", ch);
    do_look(ch, "auto");
    arena_update();

    return;
}


/*
 * challenge someone to an arena fight
 */

void do_challenge(CHAR_DATA * ch, char *argument)
{


    return;
}


/*
 * bet on an arena fight
 */

void do_bet(CHAR_DATA * ch, char *argument)
{




    return;
}




/*
 * start arena main code
 */

void arena_start()
{

    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *ch;

	    ch = d->original ? d->original : d->character;

	    // check for arena bit and notify em.
	    if ((d->connected == CON_PLAYING) && (ch->arena)) {
	        send_to_char("\r\n The arena has started, "
			 "transferring you to the battlegrounds... \r\n\r\n ",  ch);

          // set temp pkill flag
          ch->tmp_pkill = TRUE;

	        // save the old room so we can xfer em back
	        // hopefully into the arms of an aggro mob ;)        
	        ch->pre_arena_room = ch->in_room->vnum;

	        // xfer em to a random room in the arena
	        char_from_room(ch);
	        char_to_room(ch,
		        get_room_index(number_range
					  (ROOM_ARENA_LOWER,
					  ROOM_ARENA_UPPER)));
	        do_look(ch, "auto");

	        // fight!
	        send_to_char
		("\r\n                              {RFIGHT!!{x\r\n", ch);
	    }
    }

    global_current_arena_id++;

    return;

}

/*
 * stop arena function
 */

void arena_stop()
{

    return;
}

/*
 * we have a winner!
 */

void arena_win(CHAR_DATA * ch)
{

    send_to_char("{R\r\n      w00t!! You won the arena! Go J0o!\r\n\r\n{x",
		 ch);
    global_last_arena_id++;

	leave_arena(ch);
    announce("\r\n{c[{RARENA OF DEATH{c] $n just won the arena!{x", ch);
	do_look(ch, "auto");

    return;
}

/*
 * arena update handler, called from update.c
 */
/*
    arena update checks to see if any players have an arena id
    if so, and it's less than the current arena ID, then an arena is still 
    running.

    if only one player has an arena ID, and that ID is the last arena ID,
    then that player has won a previous arena.

    if no players have an arena ID, or if any player has the current arena ID,
    we increment the ID.

*/

void arena_update()
{

    DESCRIPTOR_DATA *d;
    CHAR_DATA *winner = NULL;
    int players = 0;

    if (!global_current_arena_id) {
	global_current_arena_id++;
	global_last_arena_id++;
    }

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *ch;

	ch = d->original ? d->original : d->character;

	// check for arena and increment arena ID.
	if ((d->connected == CON_PLAYING) && (ch->arena)) {

	    // someone has an old arena ID. we have an arena running,
	    // and we need to start checking for a winner.
	    if ((ch->arena == global_last_arena_id)) {
			players++;
			winner = ch;
	    }
	    // no one has the arena ID. leave it alone.
	    // no code goes here.
	}

	if (players > 1)
	    winner = NULL;
    }

    if ( ( winner )
		&& (winner->arena == global_last_arena_id)
		&& global_last_arena_id
		&& (global_current_arena_id != global_last_arena_id)) {

		arena_win(winner);
    }

    if ((players > 1) && (global_current_arena_id == global_last_arena_id)) {
	arena_start();
    }

    //printf(" current global_current_arena_id: %ld\r\n", global_current_arena_id);
    //printf(" current global_last_arena_id: %ld\r\n", global_last_arena_id);
    //printf(" current PLAYERS: %d\r\n", players);
    if (winner)
	printf(" winner is %s\r\n", winner->name);

    return;

}
