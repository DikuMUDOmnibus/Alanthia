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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "recycle.h"

/*
===========================================================================
This snippet was written by Erwin S. Andreasen, erwin@andreasen.org. You may 
use this code freely, as long as you retain my name in all of the files. You
also have to mail me telling that you are using it. I am giving this,
hopefully useful, piece of source code to you for free, and all I require
from you is some feedback.

Please mail me if you find any bugs or have any new ideas or just comments.

All my snippets are publically available at:

http://www.andreasen.org/

If you do not have WWW access, try ftp'ing to pip.dknet.dk and examine
the /pub/pip1773 directory.
===========================================================================
LIMITATIONS:

Only one level of recursion is supported for P resets. An obj resetting inside
an obj inside an obj will NOT be found, an obj resetting inside an obj will be.

The level of items inside a shop is not shown correctly.


LAST UPDATE: Oct 96 (version 4)

FIXED SINCE LAST UPDATE:

	Fixed free_reset/new_reset
	A number of local functions made static
	do_rkill was invalidly counting P resets
	do_rdrop would move O resets to current room, if used with an object
	previously resetting in the area
	Rdrop now will take either a vnum or keyword to a mob reset that
	it will place the O reset after.
	
(2) do_rkill removed invalid resets due to a misplaced brace.
(2) IS_BUILDER here renamed to IS_BUILDER_HERE to avoid conflicts.

More information inside redit.txt file!

 */

const char redit_id [] = "$Id: resedit.c,v 1.1.1.1 2002/12/26 06:55:35 bluelang Exp $";

/*
 * MACROS AND EXTERNALS
 */

#define IS_BUILDER_HERE(ch) (1) /* replace this to check if ch is builder for ch->in_room->area */

#define MAX_WEAR_LOCATIONS ( sizeof(wear_locations) / sizeof(wear_locations[0]) )

#define NUL '\0'

extern char * const where_name[]; /* act_info.c */
extern char * const dir_name[];   /* act_move.c */
extern AREA_DATA * area_first; /* db.c */
extern ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH]; 
extern OBJ_INDEX_DATA *		   obj_index_hash          [MAX_KEY_HASH];
extern MOB_INDEX_DATA *		   mob_index_hash		   [MAX_KEY_HASH];

void reset_area (AREA_DATA *area);

/*
 * Tables
 */

struct wear_location
{
	int wear_bits; /* the ITEM_ bit set in the item */
	int wear_loc1; /* where is this worn */
	int wear_loc2; /* WEAR_NONE or secondary location (i.e. left ring-finger) */
};
 
const struct wear_location wear_locations [] =
{
/* Item_flag       prim loc         sec loc or WEAR_NONE */
/*	{ITEM_LIGHT,    	WEAR_LIGHT,     WEAR_NONE}, */
	{ITEM_WEAR_FINGER,  WEAR_FINGER_L,  WEAR_FINGER_R},
	{ITEM_WEAR_NECK, 	WEAR_NECK_1, 	WEAR_NECK_2},
	{ITEM_WEAR_BODY, 	WEAR_BODY, 		WEAR_NONE},
	{ITEM_WEAR_HEAD, 	WEAR_HEAD, 		WEAR_NONE},
	{ITEM_WEAR_LEGS, 	WEAR_LEGS, 		WEAR_NONE},
	{ITEM_WEAR_FEET, 	WEAR_FEET, 		WEAR_NONE},
	{ITEM_WEAR_HANDS, 	WEAR_HANDS, 	WEAR_NONE},
	{ITEM_WEAR_ARMS, 	WEAR_ARMS, 		WEAR_NONE},
	{ITEM_WEAR_SHIELD, 	WEAR_SHIELD, 	WEAR_NONE},
	{ITEM_WEAR_ABOUT, 	WEAR_ABOUT, 	WEAR_NONE},
	{ITEM_WEAR_WAIST, 	WEAR_WAIST, 	WEAR_NONE},
	{ITEM_WEAR_WRIST, 	WEAR_WRIST_L, 	WEAR_WRIST_R},
	{ITEM_WIELD, 		WEAR_WIELD, 	WEAR_NONE},
	{ITEM_HOLD, 		WEAR_HOLD, 		WEAR_NONE}
/* insert extra locations here */	

};

/*
 * MEMORY MANAGEMENT
 */

/* Remove a reset */
static void dispose_reset (RESET_DATA *pReset)
{
	free_reset_data (pReset); /* mem.c */
}

/* Creates a new, empty reset */
static RESET_DATA* new_reset ()
{
	return new_reset_data(); /* mem.c */
}

/*
 * UTILITY FUNCTIONS
 */

/* Remove a reset from that Area, adjusting the linked list accordingly */
/* also adjust reset_first/reset_last */
static void remove_reset (AREA_DATA *pArea, RESET_DATA *pReset)
{
	RESET_DATA *q = NULL;

	/* is pReset the first reset of the area? */
	if (pReset == pArea->reset_first)
		pArea->reset_first = pReset->next;
	else
		/* Find the preceeding reset */		
		for (q = pArea->reset_first; q->next != pReset; q = q->next);

	/* Did we find whatever was before pReset ? */
	if (q)
		q->next = pReset->next;	
		
	/* Now, take care of pReset being the last reset 
	   If q == NULL, pReset was the first reset.. if it is also the last,
	   it was the ONLY reset in the area
	*/
	   
	if (pArea->reset_last == pReset)
		pArea->reset_last = q;
		
	/* dispose or recycle the reset */
	
	dispose_reset (pReset);
} 

/* Adds a reset to the end of the list of resets of that area. */
static void add_reset (ROOM_INDEX_DATA *pRoom, RESET_DATA *pReset)
{
	if (!pRoom->reset_first)
		pRoom->reset_first = pReset;
	
	if (pRoom->reset_last)
		pRoom->reset_last->next = pReset;
		
	pRoom->reset_last = pReset;
	pReset->next = NULL;
}

/* Parses a door state and returns a number, or -1 if state is invalid */
static int parse_door_state (char* state)
{
	if (!str_cmp(state, "open"))
		return 0;
	else if (!str_cmp(state,"close"))
		return 1;
	else if (!str_cmp(state,"lock"))
		return 2;
	else if (!str_cmp(state,"magicclose"))
		return 3;
	else if (!str_cmp(state,"magiclock"))
		return 4;
	else
		return -1;
}

/* Returns a ptr to a string describing state of the door */
static char* door_reset_desc (sh_int state)
{
	switch (state)
	{
		case 0 :  return "completely open";
		case 1 :  return "closed";
		case 2 :  return "closed and locked";
		case 3 :  return "magically closed";
		case 4 :  return "magically locked";
		default:  return "(invalid door state)";
	}
}

/* Checks if a given reset directly affects the given room vnum */
static bool is_here (RESET_DATA* pReset, long vnum, bool last_mob_here)
{
	switch (pReset->command)
	{

	/* Give and Equip are here if last mob was also here */
		case 'G':
		case 'E':
			return last_mob_here;

	/* Object/mob is here if vnum of room (arg3) is here */			
		case 'O':
		case 'M':
			return (pReset->arg3 == vnum);

	/* for Randomize and Door, vnum of room is arg1 */
		case 'R':
		case 'D':
			return (pReset->arg1 == vnum);

	/* Put_in resets are handled recursively by another function */
		case 'P': 
			return FALSE;

		default: /* boggle */
			return FALSE;			
	}
}


/* Find a mob with keyword arg _in this area_ Accepts number prefixes, e.g.
   2.guard etc. 
 */
static MOB_INDEX_DATA* find_mob_area (AREA_DATA *area, char* keyword)
{
	MOB_INDEX_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
	int i,number, count;
	bool found = FALSE;
	
    number = number_argument( keyword, arg );
    count  = 0;
    
    for (i = 0 ; (i < MAX_KEY_HASH) && (!found); i++)
    	for (mob = mob_index_hash[i]; mob ; mob = mob->next)
    		if ((mob->area == area) && is_name (arg, mob->player_name))
    			if (++count == number)
    			{
    				found = TRUE; /* break will only take us out of the  */
    				break;        /* innermost for, therefore found flag */
    			}
    			
    if (found)
       return mob;
    else	   	
       return NULL;
}

/* Find object in current area only */
static OBJ_INDEX_DATA* find_obj_area (AREA_DATA *area, char* keyword)
{
	OBJ_INDEX_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
	int i,number, count;
	bool found = FALSE;
	
    number = number_argument( keyword, arg );
    count  = 0;
    
    for (i = 0 ; (i < MAX_KEY_HASH) && (!found); i++)
    	for (obj = obj_index_hash[i]; obj ; obj = obj->next)
    		if ((obj->area == area) && is_name (arg, obj->name))
    			if (++count == number)
    			{
    				found = TRUE;
    				break;
    			}
    			
    if (found)
       return obj;
    else	   	
       return NULL;

	return NULL;
}

/* find mob resetting in room */

static RESET_DATA *find_mob_here (ROOM_INDEX_DATA *room, char* argument)
{
	char mob_keyword[MAX_INPUT_LENGTH];
	int count;
	int number;
	RESET_DATA *mob_reset = NULL;

	/* Find the reset for the mob with this keyword in this room! */
	/* mob is allowed to be phrased as e.g. 2.guard */
	
	number = number_argument (argument, mob_keyword);
	
	count = 0;
	
	for (mob_reset = room->area->reset_first; mob_reset ; mob_reset = mob_reset->next)
		if ((mob_reset->command == 'M') && (mob_reset->arg3 == room->vnum))
		{
			MOB_INDEX_DATA *mob = get_mob_index (mob_reset->arg1);
			if (mob && is_name (mob_keyword, mob->player_name))
				if (++count == number) /* check for X.guard */
					break;
		}
		
	return mob_reset;
}

/* Returns the level this *item* reset would reset at */
static int reset_level (AREA_DATA *pArea, RESET_DATA *pReset)
{
	RESET_DATA *p;
	MOB_INDEX_DATA *mob = NULL;
	int level = 0;
	
	for (p = pArea->reset_first; p && (p != pReset); p = p->next)
		if (p->command == 'M')
			mob = get_mob_index (p->arg1);

	if (mob)
		if (mob->pShop && (pReset->command == 'G' || pReset->command =='E')) /* level of items in shops varies */
		{
			return 0;
		}
		else
			level = URANGE(0,mob->level - 2, LEVEL_HERO);
		
	return level;		
}

/* find object resetting in room here (only O/G/E are supported, not P) */
static RESET_DATA *find_obj_here (ROOM_INDEX_DATA *room, char* argument)
{
	char obj_keyword[MAX_INPUT_LENGTH];
	int count;
	int number;
	RESET_DATA *p = NULL;
	OBJ_INDEX_DATA *obj;
	bool last_here = FALSE;
	RESET_DATA *found = NULL;

	/* Find the reset for the obj with this keyword in this room!
	   obj is allowed to be phrased as e.g. 2.bag
	   Not that object that are inside something are NOT found: 
	   P resets are not searched through.
	*/
	
	number = number_argument (argument, obj_keyword);
	
	count = 0;
	
	printf ("looking for: %dth %s\n\r",number, obj_keyword);
	
	for (p = room->area->reset_first ; p && (!found); p = p->next)
	{
		switch (p->command)
		{
			case 'M': /* remember if this is the last room */
				last_here = (p->arg3 == room->vnum);
				break;
				
			case 'G':
			case 'E':
				if (!last_here)
					continue;
			/* NOTE: No break! */					

			case 'O':					
				obj = get_obj_index (p->arg1);
				if (!obj)
					continue;
				if (is_name (obj_keyword, obj->name))
					if (++count == number)
						found = p;
			break;
			default:
		}
	}

	return found;
}


/*
 * OUTPUT FUNCTIONS
 */

/* Returns TRUE if this reset has "dependants". Checks for objs in mob inventory
   and resetted via P if fInside set, checks for levelchanges if fLevelChange
   set.
   Only shows direct dependants, e.g. if a Mob is reset with an Item that later
   some reset Puts something into, the Put reset is not shown.
*/   
static bool show_dependants (CHAR_DATA *ch, const RESET_DATA *p, bool fInside, bool fLevelChange)
{
	char buf[100];
	bool found = FALSE;
	RESET_DATA *q;
	
	if ((p->command == 'R') || (p->command == 'D'))
		return FALSE; /* R/D have never any dependants */
		
	if (p->command == 'M')
		for (q = p->next; q && (q->command != 'M'); q = q->next)
		{
			/* Resets on the mob ? */
			if (fInside && ((q->command == 'G') || (q->command == 'E')))
			{
				sprintf (buf,"Warning: Reset %c %d %d %d is keyed to this mob.\n\r",
				             q->command, q->arg1, q->arg2, q->arg3);
				send_to_char (buf,ch);
				found = TRUE;				             
			}

			/* Check if level MIGHT change */			
			if (fLevelChange && ((q->command == 'O') || (q->command == 'P')))
			{
				sprintf (buf,"Warning: Reset %c %d %d %d might change its level.\n\r",
				             q->command, q->arg1, q->arg2, q->arg3);
				send_to_char (buf,ch);
				found = TRUE;				             
			}
		}
	else /* G O E P */
		for (q = p->next; q ; q = q->next)
		{
			/* All that matters here is P resets I guess? */
			
			if (fInside && (q->command == 'P') && (q->arg1 == p->arg1))
			{
				sprintf (buf,"Warning: Reset %c %d %d %d resets an item inside this container.\n\r",
				             q->command, q->arg1, q->arg2, q->arg3);
				send_to_char (buf,ch);
				found = TRUE;				             
			}
		}
	 
		
	return found;
}


/* Shows a reset to a character. Output form is:

Random resets:
 This room's first <number> exits are randomized.

Door resets:
 <direction> door is <locked/closed/magically locked>.

Mobs:
 (???) [ vnum] short_desc (load_limit)

Objs on floor:
 (???) [ vnum] short_desc (load_limit)

Objs worn
 (???)   <wear_location> [ vnum] short_desc (load_limit)

Objs in inventory
 (???)   <inventory>     [ vnum] short_desc (load_limit)

Objs inside objs
 (???)   <inside>        [ vnum] short_desc (load_limit)

(???) is the number of the reset in the room.
(load_limit) is the maximum number of mobs loaded into this room, or (reboot) 
             for mobs/objs loaded at reboot only.
Objs inside objs have (nesting*2) spaces inserted between (???) and <inside>.
*/
   
static void show_reset (CHAR_DATA *ch, sh_int number, RESET_DATA *pReset, sh_int nesting, sh_int last_level)
{
	char reboot[20]; /* description of reboot status */
	char buf[200];   /* buffer to hold reset line */
	char spaces[11]; /* for indentation */
	
	/* This buffer is only used for the items where arg2 really IS reboot arg */
	if (pReset->arg2 < 0)
		strcpy (reboot, "reboot");
	else
	if (pReset->command == 'M')
		sprintf (reboot, "max %d", pReset->arg2);
	else /* non-zero limit for objects means chance of loading */
		sprintf (reboot, "limit: %d", pReset->arg2);

	/* check for silly programmer mistake */		
	if ((nesting > 5) || (nesting < 0))
	{
		bug ("show_resest called with nesting %d",nesting);
		return;
	}
		
	/* the big switch */
	switch (pReset->command)
	{

	case 'M': /* MOB */
	{
		MOB_INDEX_DATA *mob = get_mob_index (pReset->arg1);
		sprintf (buf, "%2d> [%5d] %s %s (%s)",
					number, pReset->arg1, 
					(mob && mob->pShop) ? "(shop)" : "",
					mob ? mob->short_descr : "(non-existant)",
					reboot);
		break;
	} /* case MOB */
	
	case 'O': /* Obj on the floor */
	{
		OBJ_INDEX_DATA* obj = get_obj_index (pReset->arg1);
		sprintf (buf, "%2d> [%5d] L%d %s (%s)",
					number, pReset->arg1, last_level,
					obj ? obj->short_descr : "(non-existant)",
					reboot);
		break;
		
	} /* case OBJ */
	
	case 'P': /* Put inside */
	{
		OBJ_INDEX_DATA *obj = get_obj_index (pReset->arg1);
		OBJ_INDEX_DATA *container = get_obj_index (pReset->arg3);
		
		strcpy (spaces, "          "); /* fill spaces.. with spaces! */
		spaces[nesting*2] = '\0'; /* spaces now has nesting*2 spaces */
		
		sprintf (buf, "%2d>  %s<inside [%5d] %s> [%5d] L%d %s (%s)",
				    number, spaces, /* num and indentation */
				    pReset->arg3, /* vnum of container */
				    container ? container->short_descr : "(non-existant)",
				    pReset->arg1, last_level, /* obj vnum, last level */
				    obj ? obj->short_descr : "(non-existant)",
				    reboot);
		break;
	} /* case PUT */
	
	case 'G': /* Give to mob */
	{
		OBJ_INDEX_DATA *obj = get_obj_index (pReset->arg1);
		
		sprintf (buf, "%2d>  <inventory>         [%5d] L%d %s (%s)",
					  number, pReset->arg1, last_level,
					  obj ? obj->short_descr : "(non-existant)",
					  reboot);
		break;
	} /* case GIVE */
	
	case 'E': /* Equip mob */
	{
		OBJ_INDEX_DATA *obj = get_obj_index (pReset->arg1);
		
		sprintf (buf, "%2d>  %s[%5d] L%d %s (%s)",
					   number,

					   /* check for correct location */
					   ((pReset->arg3 < 0) || (pReset->arg3 >= MAX_WEAR))
					   ? " <invalid location> "
					   : where_name[pReset->arg3],
					   pReset->arg1, last_level,/* obj vnum */
					   obj ? obj->short_descr : "(non-existant)",
					   reboot); /* reboot status */
		break;
					   
	} /* case EQUIP */
	
	case 'D': /* Door */
	{
		sprintf (buf,"%2d> Door leading %s is %s.",
					   number,
					   ( (pReset->arg2 < 0) || (pReset->arg2 > DIR_DOWN) )
					   ? "(invalid direction)" : dir_name[pReset->arg2],
					   door_reset_desc (pReset->arg3));

		break;
	} /* case DOOR */
	
	case 'R': /* randomize exits */
	{
		/* arg2 is COUNT of doors randomized, not last # of door */
		if ((pReset->arg2 < 1) || (pReset->arg2 > 6))
			sprintf (buf, "%2d> Exits are randomized: (invalid door count)",number);
		else
		{
			int i;
			sprintf (buf, "%2d> Exits: ", number);
			for (i = 0; i < pReset->arg2; i++)
			{
				strcat (buf, dir_name[i]);
				strcat (buf, " ");
			}
			strcat (buf, "are randomized.");
		}
		break;
	}
	
	default:
		sprintf (buf, "Invalid reset: %c %d %d %d", 
		          pReset->command, pReset->arg1, pReset->arg2, pReset->arg3);

	} /* switch */
	
	send_to_char (buf,ch);
	send_to_char ("\n\r",ch);
}


/*
 * COMMANDS
 */

/*
 * Shows to the character what resets in this room.
 */
void do_rlook (CHAR_DATA *ch, char *argument)
{
	char buf[200];
	int number = 1;
	int last_level = 0;
	RESET_DATA *p, *q;
	bool last_mob_here = FALSE;
	
	sprintf (buf, "Resets for room %d (%s)\n\r", 
	              ch->in_room->vnum, ch->in_room->name);
	send_to_char (buf,ch);	        
	              
	for (p = ch->in_room->area->reset_first; p ; p = p->next)
	{
		if (is_here (p, ch->in_room->vnum, last_mob_here))
		{
			show_reset (ch, number, p, 0, last_level);
			number++;

		/* IF an object is loaded, given or equipped, check if any objects
		   are PUT into it later in the area. Only one level of recursion! */

    		if ((p->command == 'O') || (p->command == 'E') || (p->command == 'G'))
    		{
				/* IF there is a MOB loaded between here and the P, the level
				   might change.. */
	   			int last_level_2 = last_level; 
	   			
    			for (q = p; q ; q = q->next)	
    				if ((q->command == 'P') && (q->arg3 == p->arg1))
    				{
    					show_reset (ch, number, q, 1, last_level_2);
    					number++;
    				} /* if */
    				else
    					if (q->command == 'M')
    					{
							MOB_INDEX_DATA *mob = get_mob_index (p->arg1);
							if (mob)
								last_level_2 = URANGE( 0, mob->level - 2, LEVEL_HERO);
    					}
    		}
		}
		
		/* Remember if the last mob was reset here */
		if (p->command == 'M')
		{
			MOB_INDEX_DATA *mob = get_mob_index (p->arg1);
			last_mob_here = is_here (p,ch->in_room->vnum,last_mob_here);
			if (mob)
				last_level = URANGE( 0, mob->level - 2, LEVEL_HERO);
		}
		
	} /* for */
}

/*
 * Adds a MOB Reset to the current room (M Reset)
 * rmob <mob vnum|mob keyword> [limit]
 * Default limit is 1.
 */
void do_rmob (CHAR_DATA *ch, char *argument)
{
	long vnum;
	int count;
	RESET_DATA *pReset;
	MOB_INDEX_DATA *mob;
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	if (argument[0] == NUL)
	{
		send_to_char ("Add which mob reset to this room?\n\r",ch);
		return;
	}
	
	argument = one_argument(argument,arg1);

	/* find out if the user supplied a vnum or a keyword */
	

	if (is_number(arg1))
	{
		vnum = atoi (arg1);
		mob = get_mob_index (vnum);
		if (!mob)
		{
			send_to_char ("No mob with such a vnum.\n\r",ch);
			return;
		}
	}
	else
	{
		mob = find_mob_area (ch->in_room->area,arg1);
		if (!mob)
		{
			send_to_char ("No mob with such keywords in this area.\n\r",ch);
			return;
		}
	}
	
	if (mob->area != ch->in_room->area)
		send_to_char ("Warning: This mob is from another area.\n\r",ch);
	
	/* okie, we have a valid mob now.. */

	if (argument[0]) /* Did the luser supply a max count ? */
		count = atoi (argument); /* find the rest of the line */
	else
		count = 1;
	
	if ((count < -1) || (count == 0))
	{
		send_to_char ("Max must be either -1 for reboot or a positive value.\n\r",ch);
		return;
	}
	
	pReset = new_reset(); /* okie, allocate memory for a new reset.. */
	
	pReset->command = 'M';
	pReset->arg1    = mob->vnum;
	pReset->arg2    = count;
	pReset->arg3	= ch->in_room->vnum;
	pReset->arg4	= 100;

	add_reset (ch->in_room,pReset); /* add reset */
	
	sprintf (buf, "Added reset: M %d %d %d\n\r", pReset->arg1, pReset->arg2, pReset->arg3);
	
	send_to_char (buf,ch);
}



/*
 * Gives an item to a mob (G Reset)
 * rgive <mob-keyword> <item-vnum|item-keyword> ["reboot|limit-number]
 */
void do_rgive (CHAR_DATA *ch, char *argument)
{
	char mob_name[MAX_INPUT_LENGTH];
	char obj_name[MAX_INPUT_LENGTH];
	char buf[100];
	RESET_DATA *p, *mob_reset;
	OBJ_INDEX_DATA *obj;
	int obj_vnum;
	int limit;

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	argument = one_argument (argument, mob_name);
	argument = one_argument (argument, obj_name);
	
	if (!mob_name[0])
	{
		send_to_char ("Give an object to which mob resetting here?\n\r",ch);
		return;
	}
	
	mob_reset = find_mob_here (ch->in_room, mob_name);		
	
	/* mob_reset is non-NULL if we found the right mob 	*/
	
	if (!mob_reset)
	{
		send_to_char ("No such mob resets here.\n\r",ch);
		return;
	}
	
	if (!obj_name[0])
	{
		send_to_char ("Which object do you want to give to this mob?\n\r",ch);
		return;
	}
	
	if (!is_number(obj_name))
	{
		obj = find_obj_area (ch->in_room->area, obj_name);
		if (!obj)
		{
			send_to_char ("No object with this keyword in this area.\n\r",ch);
			return;
		}
		obj_vnum = obj->vnum;
	}
	else
	{
		obj_vnum = atoi(obj_name);
		obj = get_obj_index (obj_vnum);
		if (!obj)
		{
			send_to_char ("That object does not exist.\n\r",ch);
			return;
		}
	}
	
	if (ch->in_room->area != obj->area)
		send_to_char ("Warning: This object is from another area.\n\r",ch);
			
	if (argument[0] == NUL)
		limit = 0;
	else if (!str_cmp(argument, "reboot"))
		limit = -1;
	else
		limit = atoi (argument);

	/* we have an obj_vnum now.. insert a G <obj_vnum> just right after the mob*/
	
	p = new_reset();
	
	p->command = 'G';
	p->arg1 = obj_vnum;
	p->arg2 = limit;
	p->arg3 = 0;
	
	p->next = mob_reset->next;
	mob_reset->next = p;
	
	if (ch->in_room->area->reset_last == mob_reset)
		ch->in_room->area->reset_last = p;
		
	sprintf (buf, "Added reset: %c %d %d %d\n\r",p->command,p->arg1,p->arg2,p->arg3);
	send_to_char (buf,ch);

}

/*
 * Adds an E Reset to a mob
 */
void do_rwear (CHAR_DATA *ch, char *argument)
{
	char mob_name[MAX_INPUT_LENGTH];
	char obj_name[MAX_INPUT_LENGTH];
	char buf[100];
	RESET_DATA *p, *mob_reset;
	OBJ_INDEX_DATA *obj;
	MOB_INDEX_DATA *mob;
	int obj_vnum,i,limit, wear_bits,wear_loc;
	bool worn_table[MAX_WEAR];

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	argument = one_argument (argument, mob_name);
	argument = one_argument (argument, obj_name);
	
	if (!mob_name[0])
	{
		send_to_char ("Equip which mob with what?\n\r",ch);
		return;
	}
	
	mob_reset = find_mob_here (ch->in_room, mob_name);		
	
	/* mob_reset is non-NULL if we found the right mob 	*/
	
	if (!mob_reset)
	{
		send_to_char ("No such mob resets here.\n\r",ch);
		return;
	}
	
	mob = get_mob_index(mob_reset->arg1);
	
	if (!mob) /* mob should be valid, how the heck else would find_mob_here find it ? */
	{
		send_to_char ("This mob does not seem to exist after all???\n\r",ch);
		return;
	}
	
	if (!obj_name[0])
	{
		send_to_char ("Which object do you want this mob to wear?\n\r",ch);
		return;
	}
	
	if (!is_number(obj_name))
	{
		obj = find_obj_area (ch->in_room->area, obj_name);
		if (!obj)
		{
			send_to_char ("No object with this keyword in this area.\n\r",ch);
			return;
		}
	}
	else
	{
		obj_vnum = atoi(obj_name);
		obj = get_obj_index(obj_vnum);
		if (!obj)
		{
			send_to_char ("The object with that VNUM does not exist.\n\r",ch);
			return;
		}
	}
	
	if (obj->area != ch->in_room->area)
		send_to_char ("Warning: This object is from another area.\n\r",ch);
	
	if (argument[0] == NUL)
		limit = 0;
	else if (!str_cmp(argument, "reboot"))
		limit = -1;
	else
		limit = atoi (argument);

	/* check some stuff about the object */

	/* Takeable at all ? */	
	if (!IS_SET(obj->wear_flags, ITEM_TAKE))
	{
		send_to_char ("This item is not even takeable.\n\r",ch);
		return;
	}
	
	/* now: run through all the current resets for the mob, noting
	   which places are E'ed
	*/
	for (i = 0; i < MAX_WEAR; i++) /* clear table */
		worn_table[i] = FALSE;
	
	for (p = mob_reset->next ; p && (p->command != 'M'); p = p->next)
		if (p->command == 'E')
			if ((p->arg3 < MAX_WEAR) && (p->arg3 > 0))
				if (worn_table[p->arg3]) /* give the luser some info */
					send_to_char ("Warning: This mob is equipped in the same location more than once.\n\r",ch);
				else					
					worn_table[p->arg3] = TRUE;
				
	/* we now have a table filled with locations, TRUE denotes they are occupied */
	
	/* special case: light sources */
	if ((obj->item_type == ITEM_LIGHT) && (worn_table[WEAR_LIGHT]))
	{
		send_to_char ("This mob already is equipped with a light source.\n\r",ch);
		return;
	}
	
	/* Find out the most significant wear_bit on the piece of ef EQ */
	/* keep rightshifting wear_bits till they are 0, then left shift them back
	   as many times as they were rightshifted, to only gain a single flag.
	*/   
	
	for (i = 0,wear_bits = obj->wear_flags; wear_bits ; wear_bits >>= 1, i++);
	wear_bits = 1 << (i-1);
	
	if (wear_bits == ITEM_TAKE)
	{
		send_to_char ("This item can only be taken, not worn.\n\r",ch);
		return;	
	}
	
	/* Look through the table to find the struct that fits */

	for ( i = 0; i < MAX_WEAR_LOCATIONS; i++)
		if (wear_locations[i].wear_bits == wear_bits)
			break;
			
	if (i >= MAX_WEAR_LOCATIONS)
	{
		send_to_char ("The wear bits for this item are most likely corrupt.\n\r",ch);
		return;
	}
	
	/* we have a location now.. see if it is taken */
	
	if (worn_table[wear_locations[i].wear_loc1])
	{
/*		send_to_char ("Primary location already used, checking secondary..\n\r",ch); */
		/* check alternative location */
		if (wear_locations[i].wear_loc2 != WEAR_NONE)
			if (!worn_table[wear_locations[i].wear_loc2])
				wear_loc = wear_locations[i].wear_loc2; /* not used */
			else
			{
				send_to_char ("The secondary location is used up, too.\n\r",ch);
				return;
			}
		else
		{
			send_to_char ("There is only one place you can wear this equipment, and that's already taken.\n\r",ch);
			return;
		}
			
	}
	else /* it is not worn there */
		wear_loc = wear_locations[i].wear_loc1;	
		
		
	/* Check for mob alignment */
	if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(mob)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(mob)    )
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(mob) ))
    {
		send_to_char ("This mob's alignment does not agree with this item.\n\r",ch);
		return;
    }
	
	/* we have an obj_vnum now.. insert a E <obj_vnum> just right after the mob*/
	
	p = new_reset();
	
	p->command = 'E';
	p->arg1 = obj->vnum;
	p->arg2 = limit;
	p->arg3 = wear_loc;
	
	p->next = mob_reset->next;
	mob_reset->next = p;
	
	if (ch->in_room->area->reset_last == mob_reset)
		ch->in_room->area->reset_last = p;
		
	sprintf (buf, "%s [%5d] %s\n\r", where_name[p->arg3],
	              obj->vnum, obj->short_descr);
	send_to_char (buf,ch);	              

		
	sprintf (buf, "Added reset: %c %d %d %d\n\r",p->command,p->arg1,p->arg2,p->arg3);
	send_to_char (buf,ch);
}

/*
 * Adds an O Reset to the room
 * rdrop <obj vnum|obj keyword> <0|after mob keyword/vnum> [load limit]
 * if second argument is 0, the object is placed in front of the list, level
 * being 0.
 */
void do_rdrop (CHAR_DATA *ch, char *argument)
{
	OBJ_INDEX_DATA* obj;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[200];
	AREA_DATA* area = ch->in_room->area;
	MOB_INDEX_DATA *mob = NULL;
	RESET_DATA *p, *pReset;
	
	int count;
	long vnum;

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	if (argument[0] == NUL)
	{
		send_to_char ("RDROP which object here?\n\r",ch);
		return;
	}
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg2[0] == NUL)
	{
		send_to_char ("You must supply a vnum or keyword for a mob after which this object will \n\r"
		              "be placed. You can supply 0, if you want this object to be reset at level 0.\n\r",ch);
		return;
	}
	
	if (argument[0] == NUL)
		count = 1;
	else
		count = atoi(argument);
		
	if ((count < -1) || (count == 0))
	{
		/* insert info about the new random loading stuff here! */
		send_to_char ("The max limit has to be either -1 or ???\n\r",ch);
		return;
	}
	
	/* Try to find the obj */
	if (is_number (arg1)) /* obj vnum supplied */
	{ 
		obj = get_obj_index(atoi(arg1));
		if (!obj)
		{
			send_to_char ("No object with such a VNUM.\n\r",ch);
			return;
		}
	}
	else
	{
		obj = find_obj_area (ch->in_room->area, arg1);
		if (!obj)
		{
			send_to_char ("No object with such a keyword in this area.\n\r",ch);
			return;
		}
	}

	if (obj->area != ch->in_room->area)
		send_to_char ("Warning: This object is from another area.\n\r",ch);
	
	/* We have valid obj in obj... now check for vnum of the mob */

	/* Extra sanity check: check that the SAME object does not reset in the 
	   very same room */
	for (p = area->reset_first; p ; p = p->next)
		if ((p->command == 'O') && (p->arg1 == obj->vnum) && (p->arg3 == ch->in_room->vnum))
		{
			send_to_char ("Adding this reset is useless, as the very same object is already reset\n\r"
			              "in the very same room.\n\r",ch);
			return;			            
		}


	if (is_number(arg2))
		vnum = atoi(arg2);
	else
	{
		MOB_INDEX_DATA *mob = find_mob_area (ch->in_room->area, arg2);
		if (!mob)
		{
			send_to_char ("No mob with this keyword exists in this area.\n\r",ch);
			return;
		}
		
		vnum = mob->vnum;
	}
	
	
	if (vnum != 0) /* 0 - at the beginning of the area */
	{
		/* Find out where that mobs reset in the current area, if anywhere */	
		for (p = area->reset_first; p ; p = p->next)
			if ((p->command == 'M') && (p->arg1 == vnum))
			{
				/* continue looping until a non-M or end of list is found */
				for (; p->next && (p->next->command != 'M') ; p = p->next);
				break;
			}

		/* This mob is not reset at all ! */
		if (!p)
		{
			send_to_char ("That mob does not reset anywhere in the area.\n\r",ch);
			return;
		}
		
		mob = get_mob_index (vnum);
		if (!mob)
		{
			send_to_char ("That mobile does not even exist.\n\r",ch);
			return;
		}
	}

	/* Create the reset */
	
	pReset = new_reset();
	
	pReset->command = 'O';
	pReset->arg1 = obj->vnum;
	pReset->arg2 = count;
	pReset->arg3 = ch->in_room->vnum;
	
	/* Now, insert the reset */
	
	if (vnum == 0) /* insert as very first reset */
	{
		pReset->next = area->reset_first;
		area->reset_first = pReset;
		if (!area->reset_last)
			area->reset_last = pReset;
	}
	else /* insert JUST after p */
	{
		pReset->next = p->next;
		p->next = pReset;
		if (area->reset_last == p)
			area->reset_last = pReset;
	}
	
	sprintf (buf, "Added reset: O %d %d %d\n\rObject will reset at level %d.\n\r", 
					pReset->arg1, pReset->arg2, pReset->arg3,
					mob ? mob->level : 0); /* if mob = NULL, reset at beginning */
	send_to_char (buf,ch);	
}

/*
 * Adds a R Reset to the room
 * rrandom <1-6|"remove">
 */
void do_rrandom (CHAR_DATA *ch, char *argument)
{
	RESET_DATA *pReset;
	int count;
	char buf[100]; /* just one line, used for showing the new reset */

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	if (argument[0] == NUL)
	{
		send_to_char ("Set number of doors randomized here to what? RRANDOM REMOVE to remove entirely.\n\r",ch);
		return;		
	}

	/* ok, first find out if the reset exists */
	for (pReset=ch->in_room->area->reset_first;pReset;pReset=pReset->next)
		if ((pReset->command == 'R') && (pReset->arg1 == ch->in_room->vnum))
			break;

	/* remove reset */	
	if (!str_cmp(argument, "remove"))
	{
		if (!pReset)
		{
			send_to_char ("There exists no randomize-exits-reset for this room.\n\r",ch);
			return;
		}
		
		/* Remove the found reset */
		remove_reset (ch->in_room->area,pReset);
		send_to_char ("Reset removed.\n\r",ch);
		return;
	}
	
	/* possible add/replace a new one */
	count = atoi(argument);
	
	if ((count < 1) || (count > 6))
	{
		send_to_char ("The door count must be from 1 to 6.\n\r",ch);
		return;
	}
	
	/* Are we replacing or adding a new one ?*/
	if (pReset) /* There is one already */
	{
		pReset->arg2 = count;
		send_to_char ("OK, old reset replaced with: ",ch);
	}
	else /* we have to make our own */
	{
		pReset = new_reset();
		pReset->command = 'R';
		pReset->arg1 = ch->in_room->vnum;
		pReset->arg2 = count;
		add_reset (ch->in_room,pReset);
		send_to_char ("OK, new reset added: ",ch);
	}
	
	sprintf (buf, "%c %d %d\n\r", pReset->command, pReset->arg1, pReset->arg2);
	send_to_char (buf,ch);
}

/*
 * Finds every reset in this area that involves something with the given keyword
 */
void do_rwhere (CHAR_DATA *ch, char *argument)
{
	RESET_DATA *p;
	char buf[200];
	int number = 1;
	MOB_INDEX_DATA *mob = NULL;
	
	if (argument[0] == NUL)
	{
		send_to_char ("Usage is RWHERE <keyword>.\n\r",ch);
		return;
	}

	/* First, mobs */
	
	send_to_char ("Mobs resetting in this area:\n\r",ch);

	for (p = ch->in_room->area->reset_first ; p ; p=p->next)
		if (p->command == 'M')
		{
			ROOM_INDEX_DATA *room = get_room_index (p->arg3);
		
			mob = get_mob_index (p->arg1);
			if (mob && room && is_name (argument,mob->player_name))
			{
				sprintf (buf, "%3d> [%4d] %s resets in [%4d] %s\n\r",
						      number, mob->vnum, mob->short_descr,
						      room->vnum, room->name);
				send_to_char (buf,ch);						   
				number++;
			}
		}

	/* Then, objects */
	number = 1;
	send_to_char ("Objects resetting in this area:\n\r",ch);
	for (p = ch->in_room->area->reset_first ; p ; p=p->next)
		switch (p->command)
		{

		case 'M':
			mob = get_mob_index (p->arg1);
			break;
		
		case 'O':
		{
			OBJ_INDEX_DATA *obj = get_obj_index (p->arg1);
			ROOM_INDEX_DATA *room = get_room_index (p->arg3);
			
			if (obj && room && is_name (argument,obj->name))
			{
				sprintf (buf, "%3d> [%4d] L%d %s resets in [%4d] %s\n\r",
						      number, obj->vnum, 
						      mob ? mob->level -2 : 0,
						      obj->short_descr,
						      room->vnum, room->name);
				send_to_char (buf,ch);						   
				number++;
			}
		} /* case */
		break;
		
		case 'P':
		{
			OBJ_INDEX_DATA *obj = get_obj_index (p->arg1);
			OBJ_INDEX_DATA *container = get_obj_index (p->arg3);
			
			if (obj && container && is_name (argument,obj->name))
			{
				sprintf (buf, "%3d> [%4d] L%d %s resets inside [%4d] %s\n\r",
						      number, obj->vnum, 
						      mob ? mob->level-2 :0,
						      obj->short_descr,
						      container->vnum, container->short_descr
						      );
				send_to_char (buf,ch);						   
				number++;
			}

		} /* case */
		break;
		
		case 'G':
		{
			OBJ_INDEX_DATA *obj = get_obj_index (p->arg1);
			
			if (obj && is_name (argument,obj->name))
			{
				sprintf (buf, "%3d> [%4d] L%d %s on [%4d] %s\n\r",
						      number, obj->vnum, 
						      mob ? mob->level - 2 : 0,
						      obj->short_descr,
						      mob ? mob->vnum : 0,
						      mob ? mob->short_descr : "(non-existant)"
						      );
				send_to_char (buf,ch);						   
				number++;
			}
	
		} /* case */
		break;
		
		case 'E':
		{
			OBJ_INDEX_DATA *obj = get_obj_index (p->arg1);
			
			if (obj && is_name (argument,obj->name))
			{
				sprintf (buf, "%3d> [%4d] L%d %s worn by [%4d] %s\n\r",
						      number, obj->vnum, 
						      mob ? mob->level - 2 : 0,
						      obj->short_descr,
						      mob ? mob->vnum : 0,
						      mob ? mob->short_descr : "(non-existant)"
						      );
				send_to_char (buf,ch);						   
				number++;
			} /* if */
	
		} /* case */
		break;
		
		} /* switch */
}

/*
 * Raw view of resets, for debugging purposes
 */
void do_rview (CHAR_DATA *ch, char *argument)
{
	char buf2[100];
	BUFFER *buffer = new_buf();
	RESET_DATA *p;

	add_buf (buffer, "Resets for this area:\n\r");
	
	for (p = ch->in_room->area->reset_first; p; p = p->next)
	{
		sprintf (buf2, "%c %d %d %d\n\r", p->command, p->arg1, p->arg2, p->arg3);
		add_buf (buffer, buf2);
	}
	
	page_to_char(buf_string(buffer), ch);
	free_buf (buffer);
}


/*
 * Finds every mob or object in the current area with the specified keyword
 */
void do_rfind (CHAR_DATA *ch, char *argument)
{
	MOB_INDEX_DATA *mob;
	OBJ_INDEX_DATA *obj;
	int i,number;
	char buf[MAX_STRING_LENGTH];
	
	if (!argument[0])
	{
		send_to_char ("Find mobs/objects with which keyword?\n\r",ch);
		return;
	}

	number = 1;	
	for (i = 0; i < MAX_KEY_HASH; i++)
		for (mob = mob_index_hash[i]; mob ; mob = mob->next)
			if ((mob->area == ch->in_room->area) &&
			    is_name (argument, mob->player_name) )
			   {
					sprintf (buf, "%3d> [%5d] %s\n\r", number, mob->vnum, mob->short_descr);
					send_to_char (buf,ch);
					number++;
			   }

	number = 1;
	for (i = 0; i < MAX_KEY_HASH; i++)
		for (obj = obj_index_hash[i]; obj ; obj = obj->next)
			if ((obj->area == ch->in_room->area) &&
			    is_name (argument, obj->name))
			   {
					sprintf (buf, "%3d> [%5d] %s\n\r", number, obj->vnum, obj->short_descr);
					send_to_char (buf,ch);
					number++;
			   }
}

/*
 * Removes a reset in the current room
 * rkill <number> ["confirm" ["all"]]
 * Will warn if the mob had any G/E on it, if so ALL is required to purge
 * those too.
 * CONFIRM just removes the item no matter what.
 */
void do_rkill (CHAR_DATA *ch, char *argument)
{
	bool fAll = FALSE; /* set to remove recursively */
	bool fConfirm = FALSE; /* set to confirm remove */
	bool found; /* when searching for dependants */
	bool last_mob_here = FALSE; /* did the last mob reset in this room? */
	int number = 0, count = 0;
	char arg[MAX_INPUT_LENGTH];
	char buf[100]; /* will contain "removing reset: %c %d %d %d" */
	RESET_DATA *p, *q, *q_next;

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	argument = one_argument (argument, arg);
	
	if (!is_number(arg))
	{
		send_to_char ("Usage is RKILL <number of reset to remove> [\"confirm\" [\"all\"]]\n\r",ch);
		return;
	}
	
	number = atoi(arg);
	argument = one_argument(argument,arg);

	if (arg[0])
		if (str_cmp(arg, "confirm"))
		{
			send_to_char ("Usage is RKILL <number of reset to remove> [\"confirm\" [\"all\"]]\n\r",ch);
			return;
		}	
		else
			fConfirm = TRUE;
			
	if (argument[0])
		if (str_cmp(argument, "all"))
		{
			send_to_char ("Usage is RKILL <number of reset to remove> [\"confirm\" [\"all\"]]\n\r",ch);
			return;
		}
		else
			fAll = TRUE;
			
/* Now, find the particular reset... */

	last_mob_here = FALSE;
	for (p = ch->in_room->area->reset_first; p; p = p->next)
	{
		if (is_here (p, ch->in_room->vnum, last_mob_here))
		{
			if (++count == number)
				break;

		/* IF an object is loaded, given or equipped, check if any objects
		   are PUT into it later in the area. Only one level of recursion! */

    		if ((p->command == 'O') || (p->command == 'E') || (p->command == 'G'))
    		{
    			for (q = p; q ; q = q->next)	
    				if ((q->command == 'P') && (q->arg3 == p->arg1))
    				{
						if (++count == number)
						{
							p = q;
							break;
						}
    				} /* if */
    		}

    		if (number == count)
    			break;
    			
		}

   		if (p->command == 'M') 
    		if (p->arg3 == ch->in_room->vnum)
    			last_mob_here = TRUE;
    		else
    			last_mob_here = FALSE;
    			
		
	} /* for */
	
	if (!p)
	{
		send_to_char ("There are not that many resets here; try typing RLOOK.\n\r",ch);
		return;
	}

	/* We have a Reset in P */
	/* Find out if it affects any other resets */
	
	if (!fConfirm)
	{
		/* Check if the reset has any dependants */
		if (show_dependants (ch, p, TRUE, TRUE))
		{
			send_to_char ("Reset NOT removed. Type RKILL <number> CONFIRM to remove anyway\n\r"
			              "RKILL <number> CONFIRM ALL to remove this reset and all direct\n\r"
			              "dependants.\n\r",ch);
			return;			              
		}
		
		/* Remove reset */
		sprintf (buf, "Removing reset: %c %d %d %d\n\r",p->command,p->arg1,p->arg2,p->arg3);
		send_to_char (buf,ch);
		remove_reset (ch->in_room->area,p);
		return;
	}

	/* CONFIRM but not ALL - just remove reset without any questions (dangerous!) */
	if (fConfirm && !fAll)
	{
		show_dependants (ch,p, TRUE, TRUE);
		sprintf (buf, "Removing reset: %c %d %d %d\n\r",p->command,p->arg1,p->arg2,p->arg3);
		send_to_char (buf,ch);
		remove_reset (ch->in_room->area,p);
		return;
	}

	/* Remove a mob reset's "children" */
	if (p->command == 'M') 
	{
	
	/* Check up on all the children first */
	/* Basically, checks for P resets for any of the objs the mob had */
	
	found = FALSE;
	for (q = p->next ; q && (q->command != 'M') && (q->command != 'O'); q = q->next)
	{
		if ((q->command == 'E') || (q->command == 'G'))
			found = found || show_dependants (ch, q, TRUE, FALSE);
	}
	
	if (found) /* were there any dependandts to any dependands? */
	{
		send_to_char ("Removing all this reset's direct dependants may cause THEIR dependandts\n\r"
		              "to become invalid. You will have to remove those dependandts before you can\n\r"
		              "remove this reset safely.\n\r",ch);
		return;		              
	}

	for (q = p->next ; q && (q->command != 'M') && (q->command != 'O'); q = q_next)
	{
		q_next = q->next;
		
		if ((q->command == 'E') || (q->command == 'G'))
		{
			sprintf (buf, "Removing reset: %c %d %d %d\n\r",q->command,q->arg1,q->arg2,q->arg3);
			send_to_char (buf,ch);
			remove_reset (ch->in_room->area,q);
		}
	}

	/* Remove the head reset */
	sprintf (buf, "Removing reset: %c %d %d %d\n\r",p->command,p->arg1,p->arg2,p->arg3);
	send_to_char (buf,ch);
	remove_reset (ch->in_room->area,p); /* remove, update linked list */
	return;

	}
	else if ((p->command == 'P') || (p->command == 'G') || (p->command == 'E') || (p->command == 'O'))
	{
		/* First check if any objects reset with P inside this one have any reset
		   inside them */
		   
		found = FALSE;
		
		for (q = p->next; q ; q = q->next)
			if ((q->command == 'P') && (q->arg3 == p->arg1))
				found = found || show_dependants (ch,q, TRUE, FALSE);
				
		if (found) /* were there any ? */
		{
			send_to_char ("You cannot remove this reset and all of it contents, because some of\n\r"
						  "the contents have contents of themselves! Remove those contents first.\n\r",ch);
			return;						  
		}
		
	for (q = p->next ; q ; q = q_next)
	{
		q_next = q->next;
		
		if ((q->command == 'P') && (q->arg3 == p->arg1))
		{
			sprintf (buf, "Removing reset: %c %d %d %d\n\r",q->command,q->arg1,q->arg2,q->arg3);
			send_to_char (buf,ch);
			remove_reset (ch->in_room->area,q);
		}
	}

	/* Remove the head reset */
	sprintf (buf, "Removing reset: %c %d %d %d\n\r",p->command,p->arg1,p->arg2,p->arg3);
	send_to_char (buf,ch);
	remove_reset (ch->in_room->area,p); /* remove, update linked list */
	return;
	}

}

/*
 * Inserts a P Reset
 * rput <keyword|vnum of item> <keyword of container> [limit|"reboot"]
 */
void do_rput (CHAR_DATA *ch, char *argument)
{
	char item_name[MAX_INPUT_LENGTH];
	char container_name[MAX_INPUT_LENGTH]; 
	char buf[100]; /* small buffer for output about the reset */
	int count;
	OBJ_INDEX_DATA *obj, *container;
	RESET_DATA *container_reset, *pReset, *p;

	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	argument = one_argument (argument, item_name);
	argument = one_argument (argument, container_name);
	
	if (!item_name[0])
	{
		send_to_char ("Put what where?\n\r",ch);
		return;
	}
	
	/* Try to find the obj */
	if (is_number (item_name)) /* obj vnum supplied */
	{ 
		obj = get_obj_index(atoi(item_name));
		if (!obj)
		{
			send_to_char ("No object with such a VNUM.\n\r",ch);
			return;
		}
	}
	else
	{
		obj = find_obj_area (ch->in_room->area, item_name);
		if (!obj)
		{
			send_to_char ("No object with such a keyword in this area.\n\r",ch);
			return;
		}
	}

	if (obj->area != ch->in_room->area)
		send_to_char ("Warning: This object is from another area.\n\r",ch);
		
	if (!IS_SET(obj->wear_flags, ITEM_TAKE))
	{
		send_to_char ("This item is not takeable. Noone would be able to get it out!\n\r",ch);
		return;
	}
				
	/* Now, try to find the container resetting in here */
	
	if (!container_name[0])
	{
		send_to_char ("Put this item in what container here?\n\r",ch);
		return;
	}
	
	/* Also check for valid limit argument */

	if (argument[0] == NUL)
		count = 1;
	else
		count = atoi(argument);
		
	if ((count < -1) || (count == 0))
	{
		/* insert info about the new random loading stuff here! */
		send_to_char ("The max limit has to be either -1 or ???\n\r",ch);
		return;
	}

	/* Try to find container resetting in this room */
	container_reset = find_obj_here (ch->in_room, container_name);
	
	if (!container_reset)
	{
		send_to_char ("That object does not reset here. Please not that you cannot place\n\r"
		              "objects into objects reset by P resets for now.\n\r",ch);
		return;		              
	}
	
	printf ("Reset found was: %c %d %d %d\n\r",container_reset->command,container_reset->arg1,container_reset->arg2,container_reset->arg3);
	
	
	container = get_obj_index (container_reset->arg1); /* obj vnum is first arg for O/G/E */
	if (!container) /* this should not happen */
	{
		send_to_char ("For some reason, the container does not exist...\n\r",ch);
		return;
	}

	/* you might want to extend this to something else but ITEM_CONTAINER */	
	if (container->item_type != ITEM_CONTAINER)
	{
		send_to_char ("The target container is not of type ITEM_CONTAINER.\n\r",ch);
		return;
	}

	/* Check if the container resets EARLIER in the resets */

	for (p = ch->in_room->area->reset_first ; p && (p != container_reset); p = p->next)
		if ( ((p->command == 'O') || (p->command == 'E') || (p->command == 'G')) &&
		     p->arg1 == container->vnum)
		{
			/* fire off some information */
			send_to_char ("The object you want to be container is reset at an earlier point in this area.\n\r"
			              "Inserting a Put reset now would not give the result you want; the object would\n\r"
			              "instead be inserted into the earlier instance of the container. Instead, try\n\r"
			              "creating a copy of the container with a different vnum and insert the item into\n\r"
			              "that instead. See more information in your Builders' Guide.\n\r",ch);
			return;			              
		}		     

	   
	/* All ready to go! */
	pReset = new_reset(); /* P <object to be inserted> <limit> <target object> */
	pReset->command = 'P';
	pReset->arg1 = obj->vnum;
	pReset->arg2 = count;
	pReset->arg3 = container->vnum;

	/* Tricky one: A P reset is always executed, no matter the state of last.
	   If then, you have M, G, P, G reset sequence, and M does NOT reset,
	   P will set last to TRUE, and G will choke, as it sees last==TRUE,
	   but there is no mob!

	   Therefore, I advance to the first non-G/E reset if the current reset is
	   a G/E reset. 
	*/

	while (container_reset->next && ((container_reset->command == 'E') || (container_reset->command == 'G')))
		container_reset = container_reset->next;


	pReset->next = container_reset->next;

	/*	Update linked list */
	container_reset->next = pReset; 
	if (ch->in_room->area->reset_last == container_reset)
		ch->in_room->area->reset_last = pReset;
	
 	sprintf (buf, "Item will reset at level %d.\n\rAdded reset: %c %d %d %d\n\r",
				reset_level(ch->in_room->area,pReset),pReset->command,pReset->arg1,pReset->arg2,pReset->arg3);
	send_to_char (buf,ch);
}

/*
 * Adds or changes a D reset.
 * D <Direction> <open|close|lock|magicclose|magiclock|"remove">
 */

void do_rdoor (CHAR_DATA *ch, char *argument)
{
	char dir_str[MAX_INPUT_LENGTH];
	char buf[100];
	RESET_DATA *p;
	int dir,state;
	
	if (!IS_BUILDER_HERE(ch))
	{
		send_to_char ("You have no rights to change this area.\n\r",ch);
		return;
	}
	
	argument = one_argument (argument, dir_str); /* find door name */
	
	if (!dir_str[0])
	{
		send_to_char ("Usage is: RDOOR <direction> <state>.\n\r",ch);
		return;
	}
	
	for (dir = 0; dir < 6; dir++)
		if (!str_prefix(dir_str, dir_name[dir]))
			break;
	
	if (dir >= 6)
	{
		send_to_char ("That is an invalid direction.\n\r",ch);
		return;
	}

	if (!argument[0])
	{
		send_to_char ("But what do you want set this door's reset state to?\n\r",ch);
		return;
	}
	
	/* Find the reset, if there was one, then replace with new one or create
	   a new one. 
	   I would like the new D reset to be placed after the D resets for that
	   room, if any exists, but I guess it'll have to wait until later.
	*/
	
	for (p = ch->in_room->area->reset_first; p ; p = p->next)
		if ((p->command == 'D') && 
		    (p->arg1 == ch->in_room->vnum) && 
		    (p->arg2 == dir))
			break;


	/* check for valid state name or remove */
	if (!str_cmp(argument, "remove"))
	{
		if (!p)
		{
			send_to_char ("There is no such reset here.\n\r",ch);
			return;
		}
		else
		{
			send_to_char ("Removed D reset.\n\r",ch);
			remove_reset (ch->in_room->area, p);
			return;
		}
	}
	
	state = parse_door_state (argument);
	if (state == -1)
	{
		send_to_char ("There is no such door state. Valid state include:\n\r",ch);
		for (state = 0; state < 4; state++)
		{
			send_to_char (door_reset_desc(state), ch);
			send_to_char (" ",ch);
		}
		return;
	}
			
	if (!p) /* No reset for that direction exists, create a new one */
	{
		p = new_reset(); /* create new reset */
		p->command = 'D';
		p->arg1 = ch->in_room->vnum;
		p->arg2 = dir;
		p->arg3 = state;
		add_reset (ch->in_room, p);
		send_to_char ("Added new reset: ",ch);
	}
	else /* there does exist an reset, change it */
	{
		p->arg2 = state;
		send_to_char ("Current reset changed to: ",ch);
	}
	
	sprintf (buf, "%c %d %d %d\n\r", p->command, p->arg1, p->arg2, p->arg3);
	send_to_char (buf,ch);
	
}

/*
 * utility function : forces unconditional reset of all areas/current area
 */

void do_freset (CHAR_DATA *ch, char* argument)
{
    ROOM_INDEX_DATA *pRoomIndex;
    AREA_DATA *area;

	if (!str_cmp(argument, "all"))
	{
		send_to_char ("Resetting all areas...\n\r",ch);
		for (area = area_first; area ; area = area->next)
		{
			reset_area (area);
			/* MUD School resets more often ! */
	        area->age = number_range( 0, 3 );
	        pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
	        if ( pRoomIndex != NULL && area == pRoomIndex->area )
	        	area->age = 15 - 3;
		}
	}
	else
	{
		send_to_char ("Resetting current area...\n\r",ch);
		area = ch->in_room->area;
		reset_area (area);
        area->age = number_range( 0, 3 );
        pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
        if ( pRoomIndex != NULL && area == pRoomIndex->area )
        	area->age = 15 - 3;
	}
}


/*
 * Find every exit/container requiring this given key
 *  findlock <key-vnum|key-keyword>
 */
void do_findlock (CHAR_DATA *ch, char *argument)
{
	OBJ_INDEX_DATA *obj, *container;
	char buf [MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *room;
	int i,j;
	
	if (!argument[0])
	{
		send_to_char("What object do you want to find locks for?\n\r",ch);
		return;
	}
	
	if (is_number(argument))
		obj = get_obj_index (atoi(argument));
	else
	{
		OBJ_DATA* odata;
		
		odata = get_obj_carry (ch, argument, ch);
		if (!odata)
		{
			send_to_char ("You do not have that.\n\r",ch);
			return;
		}
		
		obj = odata->pIndexData;
	}
	
	if (!obj)
	{
		send_to_char ("No such object exists.\n\r",ch);
		return;
	}
	
	/* We have a valid obj_index_data now... */
	if (obj->item_type != ITEM_KEY)
		send_to_char ("Warning: Item is not a key.\n\r",ch);
		
	/* First, search for doors */
	send_to_char ("\n\rFollowing doors depend on this key:\n\r",ch);
	
	for (i = 0 ; i < MAX_KEY_HASH; i++) /* each bucket */
		for (room = room_index_hash[i]; room ; room = room->next) /* each room */
			for (j = 0; j < 6; j++) /* each exit */
				if (room->exit[j] && room->exit[j]->key == obj->vnum)
				{
					sprintf (buf, "Door leading %s in [%5d] %s\n\r",
								   capitalize(dir_name[j]), room->vnum, room->name);
					send_to_char (buf,ch);								   
				}
				
	/* Then, search for containers */				
	send_to_char ("\n\rFollowing containers depend on this key:\n\r",ch);
	
	for (i = 0 ; i < MAX_KEY_HASH; i++) /* each bucket */
		for (container = obj_index_hash[i]; container ; container = container->next) /* each room */
			if (container->item_type == ITEM_CONTAINER &&
			    container->value[2] == obj->vnum)
			{
				sprintf (buf, "[%5d] %s\n\r",
							   container->vnum, container->short_descr);
				send_to_char (buf, ch);
			}
}
