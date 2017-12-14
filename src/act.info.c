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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <crypt.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_exits);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_affects);
DECLARE_DO_FUN(do_play);
DECLARE_DO_FUN(do_count);



char *const where_name[] = {
    "<used as light>{x        ",
    "<worn on left finger>{x  ",
    "<worn on right finger>{x ",
    "<worn around neck>{x     ",
    "<worn around neck>{x     ",
    "<worn on torso>{x        ",
    "<worn on head>{x         ",
    "<worn on legs>{x         ",
    "<worn on feet>{x         ",
    "<worn on hands>{x        ",
    "<worn on arms>{x         ",
    "<worn as shield>{x       ",
    "<worn about body>{x      ",
    "<worn around waist>{x    ",
    "<worn on left wrist>{x   ",
    "<worn on right wrist>{x  ",
    "<wielded>{x              ",
    "<held>{x                 ",
    "<floating nearby>{x      ",
    "<worn on face>{x         ",
    "<worn in left ear>{x     ",
    "<worn in right ear>{x    ",
    "<worn on shoulders>{x    ",
};

char *const empty_name[] = {
    "light",
    "left finger",
    "right finger",
    "neck",
    "neck",
    "torso",
    "head",
    "legs",
    "feet",
    "hands",
    "arms",
    "shield",
    "body",
    "waist",
    "left wrist",
    "right wrist",
    "wielded",
    "holding",
    "floating nearby",
    "face",
    "left ear",
    "right ear",
    "shoulders",
};

char *const room_class_name[] = { "",
};


/* for do_count */
int max_on = 0;

/*
 * Local functions.
 */
char *format_obj_to_char args((OBJ_DATA * obj, CHAR_DATA * ch,
			       bool fShort));
char *format_lev_obj_to_char args((OBJ_DATA * obj, CHAR_DATA * ch,
				   bool fShort));
void show_list_to_char args((OBJ_DATA * list, CHAR_DATA * ch,
			     bool fShort, bool fShowNothing));
void show_lev_list_to_char args((OBJ_DATA * list, CHAR_DATA * ch,
				 bool fShort, bool fShowNothing));
void show_char_to_char_0 args((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char_1 args((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char args((CHAR_DATA * list, CHAR_DATA * ch));
bool check_blind args((CHAR_DATA * ch));
void lore args((CHAR_DATA * ch, OBJ_DATA * obj));

char *format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
    static char buf[MAX_STRING_LENGTH];
    char buf2[MAX_INPUT_LENGTH];

    buf[0] = '\0';

    if (
	(fShort
	 && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
	|| (obj->description == NULL || obj->description[0] == '\0'))
	return buf;
/*
    if (obj->bound) 
        strcat(buf, "{w({WBOUND{w) ");
*/
    if (IS_OBJ_STAT(obj, ITEM_INVIS))
	strcat(buf, "{w({WInvis{w) ");
    if (IS_AFFECTED(ch, AFF_DETECT_EVIL)
	&& IS_OBJ_STAT(obj, ITEM_EVIL))
	strcat(buf, "{r({RRed Aura{r) ");
    if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
	&& IS_OBJ_STAT(obj, ITEM_BLESS))
	strcat(buf, "{b({BBlue Aura{b) ");
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC)
	&& IS_OBJ_STAT(obj, ITEM_MAGIC))
	strcat(buf, "{m({MMagical{m) ");
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
	strcat(buf, "{g({GGlowing{g) ");
    if (IS_OBJ_STAT(obj, ITEM_HUM))
	strcat(buf, "{c({CHumming{c) ");
    if (IS_OBJ_STAT(obj, ITEM_VIB))
	strcat(buf, "{c({MVibrating{c) ");

    if (IS_SET(ch->act, PLR_HOLYLIGHT)) {
	sprintf(buf2, "{w({W%ld{w) ", obj->pIndexData->vnum);
	strcat(buf, buf2);
    }
    strcat(buf, "{y");


    if (fShort) {
	if (obj->short_descr != NULL)
	    strcat(buf, obj->short_descr);
    } else {
	if (obj->description != NULL)
	    strcat(buf, obj->description);
    }

    return buf;
}

char *format_lev_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
    static char buf[MAX_STRING_LENGTH];
    static char buf2[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (
	(fShort
	 && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
	|| (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    sprintf(buf2, "{y[%3d]{x ", obj->level);
    strcat(buf, buf2);

    if (IS_OBJ_STAT(obj, ITEM_INVIS))
	strcat(buf, "{w({WInvis{w) ");
    if (IS_AFFECTED(ch, AFF_DETECT_EVIL)
	&& IS_OBJ_STAT(obj, ITEM_EVIL))
	strcat(buf, "{r({RRed Aura{r) ");
    if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
	&& IS_OBJ_STAT(obj, ITEM_BLESS))
	strcat(buf, "{b({BBlue Aura{b) ");
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC)
	&& IS_OBJ_STAT(obj, ITEM_MAGIC))
	strcat(buf, "{m({MMagical{m) ");
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
	strcat(buf, "{g({GGlowing{g) ");
    if (IS_OBJ_STAT(obj, ITEM_HUM))
	strcat(buf, "{c({CHumming{c) ");
    if (IS_OBJ_STAT(obj, ITEM_VIB))
	strcat(buf, "{c({MVibrating{c) ");

    strcat(buf, "{y");

    if (fShort) {
	if (obj->short_descr != NULL)
	    strcat(buf, obj->short_descr);
    } else {
	if (obj->description != NULL)
	    strcat(buf, obj->description);
    }

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
		       bool fShowNothing)
{
    char buf[MSL];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    long iShow;
    long count;
    bool fCombine;

    if (ch->desc == NULL)
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for (obj = list; obj != NULL; obj = obj->next_content)
	count++;



    prgpstrShow = alloc_mem(count * sizeof(char *));
    prgnShow = alloc_mem(count * sizeof(int));
    nShow = 0;

	//printf("counter is at: %ld, buffer is at %ld\n\n",
	//	count,
	//	count * (sizeof(char *)));
    /*
     * Format the list of objects.
     */
    for (obj = list; obj != NULL; obj = obj->next_content) {
	if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj)) {
	    pstrShow = format_obj_to_char(obj, ch, fShort);

	    fCombine = FALSE;

	    if (IS_NPC(ch) 
			|| IS_SET(ch->comm, COMM_COMBINE)) {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for (iShow = nShow - 1; iShow >= 0 && iShow < 5000; iShow--) {
		    if (!strcmp(prgpstrShow[iShow], pstrShow)) {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }
	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if (!fCombine) {
		prgpstrShow[nShow] = str_dup(pstrShow);
		prgnShow[nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */

    for (iShow = 0; iShow < nShow && iShow > -1; iShow++) {
	if (prgpstrShow[iShow][0] == '\0') {
	    free_string(prgpstrShow[iShow]);
	    continue;
	}
	if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE)) {
	    if (prgnShow[iShow] != 1) {
		sprintf(buf, "{y(%2d) ", prgnShow[iShow]);
		add_buf(output, buf);
	    } else {
		add_buf(output, "{y     ");
	    }
	}
	//printf("adding a buffer1\n");
	add_buf(output, prgpstrShow[iShow]);
	//printf("adding a buffer2\n");
	add_buf(output, "\n\r{x");
	//printf("about to free a string!\n");
	free_string(prgpstrShow[iShow]);

    }

    if (fShowNothing && nShow == 0) {
	if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
	    send_to_char("     ", ch);
	send_to_char("{yNothing.\n\r{x", ch);
    }

	//printf("paging to character\n");
    page_to_char(buf_string(output), ch);

	//printf("about to free_buf output\n");
    free_buf(output);
	//printf("about to free_mem prgpstrShowt\n");
    free_mem(prgpstrShow);
	//printf("about to free_mem prgnShow, count\n");
    free_mem(prgnShow);

    return;
}

/*
 * Show a list to a character, with levels of obj.
 * Can coalesce duplicated items.
 */
void show_lev_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
			   bool fShowNothing)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if (ch->desc == NULL)
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for (obj = list; obj != NULL; obj = obj->next_content)
	count++;
    prgpstrShow = alloc_mem(count * sizeof(char *));
    prgnShow = alloc_mem(count * sizeof(int));
    nShow = 0;

    /*
     * Format the list of objects.
     */
    for (obj = list; obj != NULL; obj = obj->next_content) {
	if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj)) {
	    pstrShow = format_lev_obj_to_char(obj, ch, fShort);

	    fCombine = FALSE;

	    if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE)) {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for (iShow = nShow - 1; iShow >= 0; iShow--) {
		    if (!strcmp(prgpstrShow[iShow], pstrShow)) {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }
	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if (!fCombine) {
		prgpstrShow[nShow] = str_dup(pstrShow);
		prgnShow[nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for (iShow = 0; iShow < nShow; iShow++) {
	if (prgpstrShow[iShow][0] == '\0') {
	    free_string(prgpstrShow[iShow]);
	    continue;
	}
	if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE)) {
	    if (prgnShow[iShow] != 1) {
		sprintf(buf, "{y(%2d) ", prgnShow[iShow]);
		add_buf(output, buf);
	    } else {
		add_buf(output, "{y     ");
	    }
	}
	add_buf(output, prgpstrShow[iShow]);
	add_buf(output, "\n\r{x");
	free_string(prgpstrShow[iShow]);
    }

    if (fShowNothing && nShow == 0) {
	if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
	    send_to_char("     ", ch);
	send_to_char("{yNothing.\n\r{x", ch);
    }
    page_to_char(buf_string(output), ch);

    /*
     * Clean up.
     */
    free_buf(output);
    free_mem(prgpstrShow);
    free_mem(prgnShow);

    return;
}



void show_char_to_char_0(CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char message[MAX_STRING_LENGTH];

    buf[0] = '\0';
    buf2[0] = '\0';

    if (IS_SET(victim->comm, COMM_AFK))
	strcat(buf, "{m[{MA{mF{MK{m] ");
    if (IS_AFFECTED(victim, AFF_INVISIBLE))
	strcat(buf, "{w({WInvis{w) ");
    if (victim->invis_level >= LEVEL_HERO)
	strcat(buf, "{w({WWizi{w) ");
    if (IS_AFFECTED(victim, AFF_HIDE))
	strcat(buf, "{w({WHide){w ");
    if (IS_AFFECTED(victim, AFF_CHARM))
	strcat(buf, "{g({GCharmed{g) ");
    if (IS_AFFECTED(victim, AFF_PASS_DOOR))
	strcat(buf, "{w({CTrans{w) ");
    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
	strcat(buf, "{w({WPink Aura{w) ");
    if (IS_EVIL(victim)
	&& IS_AFFECTED(ch, AFF_DETECT_EVIL))
	strcat(buf, "{R({rEvil{R) ");
    if (IS_GOOD(victim)
	&& IS_AFFECTED(ch, AFF_DETECT_GOOD))
	strcat(buf, "{y({YGood{y) ");
    if (IS_AFFECTED(victim, AFF_SANCTUARY))
	strcat(buf, "{w({WWAura{w) ");
    if (IS_AFFECTED(victim, AFF_HEAVENLY_SANCTUARY))
	strcat(buf, "{y({YGAura{y){x ");
    if (IS_AFFECTED(victim, AFF_POISON))
	strcat(buf, "{G({gPoison{G) ");
    if (IS_AFFECTED(victim, AFF_PLAGUE))
	strcat(buf, "{G({gPlague{G) ");
    if (IS_AFFECTED(victim, AFF_FLYING))
	strcat(buf, "{w({WFly{w) ");
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_PKILL))
	strcat(buf, "{r({RPK{r) ");
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF))
	strcat(buf, "{w({WTHIEF{w) ");
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KING))
	strcat(buf, "{YKING{x ");
    if (victim->pcdata != NULL && victim->pcdata->god[0] != '\0') {
	sprintf(message, "{w({W%s{w) ", victim->pcdata->god);
	strcat(buf, message);
    }
    if (IS_NPC(victim) && IS_SET(ch->act, PLR_HOLYLIGHT)) {
	sprintf(message, "{w({W%ld{w) ", victim->pIndexData->vnum);
	strcat(buf, message);
    }
    /* added to help color out */
    /* stupid bastard. fixed broken caps - WDL - 3/9/98 */
    if (!IS_NPC(victim))
	strcat(buf, "{x");
    if (IS_NPC(victim)) {
	strcat(buf, "{G");
    }
    if (victim->position == victim->start_pos
	&& victim->long_descr[0] != '\0') {
	strcat(buf, victim->long_descr);
	send_to_char(buf, ch);
	return;
    }
    strcat(buf2, PERS(victim, ch));
    buf2[0] = UPPER(buf2[0]);
    strcat(buf, buf2);
    if (!IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF)
	&& victim->position == POS_STANDING && ch->on == NULL) {
	sprintf(buf2, " the %s %s",
		victim->race->gen_name,
		class_table[victim->class].name);
	strcat(buf, buf2);
    }
    switch (victim->position) {
    case POS_DEAD:
	strcat(buf, " is DEAD!!");
	break;
    case POS_MORTAL:
	strcat(buf, " is mortally wounded.");
	break;
    case POS_INCAP:
	strcat(buf, " is incapacitated.");
	break;
    case POS_STUNNED:
	strcat(buf, " is lying here stunned.");
	break;
    case POS_SLEEPING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], SLEEP_AT)) {
		sprintf(message, " is sleeping at %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], SLEEP_ON)) {
		sprintf(message, " is sleeping on %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is sleeping in %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is sleeping here.");
	break;
    case POS_RESTING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], REST_AT)) {
		sprintf(message, " is resting at %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], REST_ON)) {
		sprintf(message, " is resting on %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is resting in %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is resting here.");
	break;
    case POS_SITTING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], SIT_AT)) {
		sprintf(message, " is sitting at %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], SIT_ON)) {
		sprintf(message, " is sitting on %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is sitting in %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is sitting here.");
	break;
    case POS_STANDING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], STAND_AT)) {
		sprintf(message, " is standing at %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], STAND_ON)) {
		sprintf(message, " is standing on %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is standing in %s.",
			victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is here.");
	break;
    case POS_FIGHTING:
	strcat(buf, " is here, fighting ");
	if (victim->fighting == NULL)
	    strcat(buf, "thin air??");
	else if (victim->fighting == ch)
	    strcat(buf, "YOU!");
	else if (victim->in_room == victim->fighting->in_room) {
	    strcat(buf, PERS(victim->fighting, ch));
	    strcat(buf, ".");
	} else
	    strcat(buf, "someone who left??");
	break;
    }

    strcat(buf, "{x\n\r");
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    return;
}



void show_char_to_char_1(CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if (can_see(victim, ch)) {
	if (ch == victim)
	    act("$n looks at $mself.", ch, NULL, NULL, TO_ROOM);
	else {
	    act("$n looks at you.", ch, NULL, victim, TO_VICT);
	    act("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
	}
    }
    if (victim->description[0] != '\0') {
	send_to_char(victim->description, ch);
    } else {
	act("You see nothing special about $M.", ch, NULL, victim,
	    TO_CHAR);
    }

    if (victim->max_hit > 0)
	percent = (100 * victim->hit) / victim->max_hit;
    else
	percent = -1;

    strcpy(buf, PERS(victim, ch));

    if (percent >= 100)
	strcat(buf, " is in excellent condition.\n\r");
    else if (percent >= 90)
	strcat(buf, " has a few scratches.\n\r");
    else if (percent >= 75)
	strcat(buf, " has some small wounds and bruises.\n\r");
    else if (percent >= 50)
	strcat(buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
	strcat(buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
	strcat(buf, " looks pretty hurt.\n\r");
    else if (percent >= 0)
	strcat(buf, " is in awful condition.\n\r");
    else
	strcat(buf, " is bleeding to death.\n\r");

    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);

    found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
	if ((obj = get_eq_char(victim, iWear)) != NULL
	    && can_see_obj(ch, obj)) {
	    if (!found) {
		send_to_char("{x\n\r", ch);
		act("$N is using:", ch, NULL, victim, TO_CHAR);
		found = TRUE;
	    }
	    send_to_char(where_name[iWear], ch);
	    send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
	    send_to_char("{x\n\r", ch);
	}
    }

    if (victim != ch && !IS_NPC(ch)
	&& number_percent() < get_skill(ch, gsn_peek)) {
	send_to_char("\n\r{yYou peek at the inventory:\n\r", ch);
	check_improve(ch, gsn_peek, TRUE, 4);
	show_list_to_char(victim->carrying, ch, TRUE, TRUE);
    }
    return;
}



void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
    CHAR_DATA *rch;

    for (rch = list; rch != NULL; rch = rch->next_in_room) {
	if (rch == ch)
	    continue;

	if (get_trust(ch) < rch->invis_level)
	    continue;

	if (can_see(ch, rch)) {
	    show_char_to_char_0(rch, ch);
	} else if (room_is_dark(ch->in_room)
		   && IS_AFFECTED(rch, AFF_INFRARED)) {
	    send_to_char("You see glowing {Rred{x eyes watching YOU!\n\r",
			 ch);
	}
    }

    return;
}



bool check_blind(CHAR_DATA * ch)
{

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
	return TRUE;

    if (IS_AFFECTED(ch, AFF_BLIND)) {
	send_to_char("You can't see a thing!\n\r", ch);
	return FALSE;
    }
    return TRUE;
}

/* Show info about an object */

void lore(CHAR_DATA * ch, OBJ_DATA * obj)
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    if (get_skill(ch, gsn_lore) <= 1
	|| number_percent() > get_skill(ch, gsn_lore))  {
	check_improve(ch, gsn_lore, FALSE, 4);
	return;
	}
    sprintf(buf,
	     "The %s is %s.\n\rThis level %d %s weighs %d stones, and is valued at %d gold.\n\r",
	     obj->name, extra_bit_name(obj->extra_flags), obj->level,
	     item_name(obj->item_type), obj->weight / 10, obj->cost);
    send_to_char(buf, ch);

    switch (obj->item_type)
	 {
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf(buf, "Level %d spells of:", obj->value[0]);
	send_to_char(buf, ch);

	if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL)
	     {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[1]].name, ch);
	    send_to_char("'", ch);
	    }
	if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL)
	     {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[2]].name, ch);
	    send_to_char("'", ch);
	    }
	if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
	     {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[3]].name, ch);
	    send_to_char("'", ch);
	    }
	if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
	     {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[4]].name, ch);
	    send_to_char("'", ch);
	    }
	send_to_char(".\n\r", ch);
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	sprintf(buf, "Has %d charges of level %d",
		obj->value[2], obj->value[0]);
	send_to_char(buf, ch);

	if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
	     {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[3]].name, ch);
	    send_to_char("'", ch);
	    }
	send_to_char(".\n\r", ch);
	break;

    case ITEM_DRINK_CON:
	sprintf(buf, "It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name);
	send_to_char(buf, ch);
	break;

    case ITEM_BLOOD_CON:
	sprintf(buf, "It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name);
	send_to_char(buf, ch);
	break;

    case ITEM_CONTAINER:
	sprintf(buf,
		"Can hold %d stones of items, each item weighing up to %d stones. It is %s\n\r",
		obj->value[0], obj->value[3],
		cont_bit_name(obj->value[1]));
	send_to_char(buf, ch);
	if (obj->value[4] != 100)
	     {
	    sprintf(buf, "Weight multiplier: %d%%\n\r", obj->value[4]);
	    send_to_char(buf, ch);
	    }
	break;

    case ITEM_WEAPON:
	send_to_char("Weapon is ", ch);
	switch (obj->value[0])
	     {
	case (WEAPON_EXOTIC):
	    send_to_char("an exotic.\n\r", ch);
	    break;
	case (WEAPON_SWORD):
	    send_to_char("a sword.\n\r", ch);
	    break;
	case (WEAPON_DAGGER):
	    send_to_char("a dagger.\n\r", ch);
	    break;
	case (WEAPON_SPEAR):
	    send_to_char("a spear/staff.\n\r", ch);
	    break;
	case (WEAPON_MACE):
	    send_to_char("a mace/club.\n\r", ch);
	    break;
	case (WEAPON_AXE):
	    send_to_char("an axe.\n\r", ch);
	    break;
	case (WEAPON_FLAIL):
	    send_to_char("a flail.\n\r", ch);
	    break;
	case (WEAPON_WHIP):
	    send_to_char("a whip.\n\r", ch);
	    break;
	case (WEAPON_POLEARM):
	    send_to_char("a polearm.\n\r", ch);
	    break;
	default:
	    send_to_char("an unknown.\n\r", ch);
	    break;
	    }
	if (obj->pIndexData->new_format)
	    sprintf(buf, "Average damage of %d.\n\r",
		    (1 + obj->value[2]) * obj->value[1] / 2);
	else
	    sprintf(buf, "Average damage is %d.\n\r",
		    (obj->value[1] + obj->value[2]) / 2);
	send_to_char(buf, ch);
	if (obj->value[4]) {	/* weapon flags */
	    sprintf(buf, "This weapon is affected by %s.\n\r",
		    weapon_bit_name(obj->value[4]));
	    send_to_char(buf, ch);
	}
	break;

    case ITEM_ARMOR:
	sprintf(buf,
		"It offers %d pierce, %d bash, %d slash, and %d magic protection.\n\r",
		obj->value[0], obj->value[1], obj->value[2],
		obj->value[3]);
	send_to_char(buf, ch);
	break;
	}

    if (!obj->enchanted)
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	     {
	    if (paf->location != APPLY_NONE && paf->modifier != 0)
		 {
		sprintf(buf, "Affects %s by %d.\n\r",
			affect_loc_name(paf->location), paf->modifier);
		send_to_char(buf, ch);
		if (paf->bitvector)
		     {
		    switch (paf->where)
			 {
		    case TO_AFFECTS:
			sprintf(buf, "Adds %s affect.\n",
				affect_bit_name(paf->bitvector));
			break;
		    case TO_OBJECT:
			sprintf(buf, "Adds %s object flag.\n",
				extra_bit_name(paf->bitvector));
			break;
		    case TO_IMMUNE:
			sprintf(buf, "Adds immunity to %s.\n",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_RESIST:
			sprintf(buf, "Adds resistance to %s.\n\r",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_VULN:
			sprintf(buf, "Adds vulnerability to %s.\n\r",
				imm_bit_name(paf->bitvector));
			break;
		    default:
			sprintf(buf, "Unknown bit %d: %d\n\r",
				paf->where, paf->bitvector);
			break;
			}
		    send_to_char(buf, ch);
		    }
		}
	    }
    for (paf = obj->affected; paf != NULL; paf = paf->next)
	 {
	if (paf->location != APPLY_NONE && paf->modifier != 0)
	     {
	    sprintf(buf, "Affects %s by %d",
		    affect_loc_name(paf->location), paf->modifier);
	    send_to_char(buf, ch);
	    if (paf->duration > -1)
		sprintf(buf, ", %d hours.\n\r", paf->duration);
	    else
		sprintf(buf, ".\n\r");
	    send_to_char(buf, ch);
	    if (paf->bitvector)
		 {
		switch (paf->where)
		     {
		case TO_AFFECTS:
		    sprintf(buf, "Adds %s affect.\n",
			    affect_bit_name(paf->bitvector));
		    break;
		case TO_OBJECT:
		    sprintf(buf, "Adds %s object flag.\n",
			    extra_bit_name(paf->bitvector));
		    break;
		case TO_WEAPON:
		    sprintf(buf, "Adds %s weapon flags.\n",
			    weapon_bit_name(paf->bitvector));
		    break;
		case TO_IMMUNE:
		    sprintf(buf, "Adds immunity to %s.\n",
			    imm_bit_name(paf->bitvector));
		    break;
		case TO_RESIST:
		    sprintf(buf, "Adds resistance to %s.\n\r",
			    imm_bit_name(paf->bitvector));
		    break;
		case TO_VULN:
		    sprintf(buf, "Adds vulnerability to %s.\n\r",
			    imm_bit_name(paf->bitvector));
		    break;
		default:
		    sprintf(buf, "Unknown bit %d: %d\n\r",
			    paf->where, paf->bitvector);
		    break;
		    }
		send_to_char(buf, ch);
		}
	    }
	}

    if (number_percent() < get_skill(ch, gsn_lore) * 1 / 20) {
	obj->cost = obj->cost + ch->level;
	send_to_char
	    ("Your vast knowledge increased the value of this item!\n\r",
	     ch);
    }
    check_improve(ch, gsn_lore, TRUE, 4);
    return;
}


/* changes your scroll */
void do_scroll(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	if (ch->lines == 0)
	    send_to_char("You do not page long messages.\n\r", ch);
	else {
	    sprintf(buf, "You currently display %d lines per page.\n\r",
		    ch->lines + 2);
	    send_to_char(buf, ch);
	}
	return;
    }
    if (!is_number(arg)) {
	send_to_char("You must provide a number.\n\r", ch);
	return;
    }
    lines = atoi(arg);

    if (lines == 0) {
	send_to_char("Paging set to max (100) lines.\n\r", ch);
	ch->lines = 100;
	return;
    }
    if (lines < 10 || lines > 100) {
	send_to_char("You must provide a reasonable number.\n\r", ch);
	return;
    }
    sprintf(buf, "Scroll set to %d lines.\n\r", lines);
    send_to_char(buf, ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++) {
	sprintf(buf, "%-12s", social_table[iSocial].name);
	send_to_char(buf, ch);
	if (++col % 6 == 0)
	    send_to_char("\n\r", ch);
    }

    if (col % 6 != 0)
	send_to_char("\n\r", ch);
    return;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "motd");
}

void do_imotd(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "imotd");
}

void do_rules(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "rules");
}

void do_story(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "story");
}

void do_wizlist(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA * ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
	return;

    send_to_char("   action     status\n\r", ch);
    send_to_char("---------------------\n\r", ch);

    send_to_char("autoassist     ", ch);
    if (IS_SET(ch->act, PLR_AUTOASSIST))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autoexit       ", ch);
    if (IS_SET(ch->act, PLR_AUTOEXIT))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autogold       ", ch);
    if (IS_SET(ch->act, PLR_AUTOGOLD))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autoloot       ", ch);
    if (IS_SET(ch->act, PLR_AUTOLOOT))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autosac        ", ch);
    if (IS_SET(ch->act, PLR_AUTOSAC))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autosplit      ", ch);
    if (IS_SET(ch->act, PLR_AUTOSPLIT))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autodefense    ", ch);
    if (IS_SET(ch->act, PLR_AUTODEFENSE))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("autodamage     ", ch);
    if (IS_SET(ch->act, PLR_AUTODAMAGE))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("compact mode   ", ch);
    if (IS_SET(ch->comm, COMM_COMPACT))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("prompt         ", ch);
    if (IS_SET(ch->comm, COMM_PROMPT))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("combine items  ", ch);
    if (IS_SET(ch->comm, COMM_COMBINE))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    if (!IS_SET(ch->act, PLR_CANLOOT))
	send_to_char("Your corpse is safe from thieves.\n\r", ch);
    else
	send_to_char("Your corpse may be looted.\n\r", ch);

    if (IS_SET(ch->act, PLR_NOSUMMON))
	send_to_char("You cannot be summoned.\n\r", ch);
    else
	send_to_char("You can be summoned.\n\r", ch);

    if (IS_SET(ch->act, PLR_NOFOLLOW))
	send_to_char("You do not welcome followers.\n\r", ch);
    else
	send_to_char("You accept followers.\n\r", ch);
}

void do_autoassist(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOASSIST)) {
	send_to_char("Autoassist removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOASSIST);
    } else {
	send_to_char("You will now assist when needed.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOASSIST);
    }
}

void do_autoexit(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOEXIT)) {
	send_to_char("Exits will no longer be displayed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOEXIT);
    } else {
	send_to_char("Exits will now be displayed.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOEXIT);
    }
}

void do_autogold(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOGOLD)) {
	send_to_char("Autogold removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOGOLD);
    } else {
	send_to_char("Automatic gold looting set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOLOOT)) {
	send_to_char("Autolooting removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOLOOT);
    } else {
	send_to_char("Automatic corpse looting set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOLOOT);
    }
}

void do_autotitle(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOTITLE)) {
	send_to_char("Automagic title updating removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOTITLE);
    } else {
	send_to_char("Automagic title updating set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOTITLE);
    }
}

void do_nomiss(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_NOMISS)) {
	send_to_char("You will now see miss messages.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_NOMISS);
    } else {
	send_to_char("You will no longer see miss messages.\n\r", ch);
	SET_BIT(ch->act, PLR_NOMISS);
    }
}

void do_autosac(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOSAC)) {
	send_to_char("Autosacrificing removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOSAC);
    } else {
	send_to_char("Automatic corpse sacrificing set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOSPLIT)) {
	send_to_char("Autosplitting removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOSPLIT);
    } else {
	send_to_char("Automatic gold splitting set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOSPLIT);
    }
}

void do_autodefense(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTODEFENSE)) {
	send_to_char
	    ("You will now see parries, blocks, and dodges during combat.\n\r",
	     ch);
	REMOVE_BIT(ch->act, PLR_AUTODEFENSE);
    } else {
	send_to_char
	    ("You will no longer see parries, blocks, and dodges during combat.\n\r",
	     ch);
	SET_BIT(ch->act, PLR_AUTODEFENSE);
    }
}

void do_autodamage(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTODAMAGE)) {
	send_to_char("Damage will no longer be shown during combat.\n\r",
		     ch);
	REMOVE_BIT(ch->act, PLR_AUTODAMAGE);
    } else {
	send_to_char("Damage will now be shown during combat.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTODAMAGE);
    }
}

void do_brief(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_BRIEF)) {
	send_to_char("Full descriptions activated.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_BRIEF);
    } else {
	send_to_char("Short descriptions activated.\n\r", ch);
	SET_BIT(ch->comm, COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_COMPACT)) {
	send_to_char("Compact mode removed.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_COMPACT);
    } else {
	send_to_char("Compact mode set.\n\r", ch);
	SET_BIT(ch->comm, COMM_COMPACT);
    }
}

void do_show(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_SHOW_AFFECTS)) {
	send_to_char("Affects will no longer be shown in score.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_SHOW_AFFECTS);
    } else {
	send_to_char("Affects will now be shown in score.\n\r", ch);
	SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_PROMPT)) {
	    send_to_char("You will no longer see prompts.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_PROMPT);
	} else {
	    send_to_char("You will now see prompts.\n\r", ch);
	    SET_BIT(ch->comm, COMM_PROMPT);
	}
	return;
    }
    if (!str_cmp(argument, "all"))
	if (IS_VAMPIRE(ch) || IS_DWARF(ch))
	    strcpy(buf, "{c<%hhp %mm %vmv %bbp>{x ");
	else
	    strcpy(buf, "{c<%hhp %mm %vmv>{x ");

    else if (!str_cmp(argument, "1"))
	strcpy(buf, "{c<Hp:%h/%H M:%m/%M{c> (Exp:%X){x ");

    else if (!str_cmp(argument, "2"))
	strcpy(buf, "{w<{c%hhit %mman %bbp %vend %gg %Xxp{w> %P{/ ");

    else if (!str_cmp(argument, "build"))
	strcpy(buf, "{R<{r%z {g%o {b%O{R>{x{/ ");

    else {
	strcat(argument, "{x");

	if (strlen(argument) > 65) {
	    argument[65] = '{';
	    argument[66] = 'x';
	    argument[67] = '\0';
	}
	strcpy(buf, argument);
	smash_tilde(buf);
	if (str_suffix("%c", buf))
	    strcat(buf, " ");

    }

    free_string(ch->prompt);
    ch->prompt = str_dup(buf);
    sprintf(buf, "Prompt set to %s\n\r", ch->prompt);
    send_to_char(buf, ch);
    return;
}

void do_combine(CHAR_DATA * ch, char *argument)
{

/* commented out - everyone should always use combine. 
  
    if (IS_SET(ch->comm, COMM_COMBINE)) {
	send_to_char("Long inventory selected.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_COMBINE);
    } else {
	send_to_char("Combined inventory selected.\n\r", ch);
	SET_BIT(ch->comm, COMM_COMBINE);
    }
*/

	return;

}

void do_noloot(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_CANLOOT)) {
	send_to_char("Your corpse is now safe from thieves.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_CANLOOT);
    } else {
	send_to_char("Your corpse may now be looted.\n\r", ch);
	SET_BIT(ch->act, PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_NOFOLLOW)) {
	send_to_char("You now accept followers.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_NOFOLLOW);
    } else {
	send_to_char("You no longer accept followers.\n\r", ch);
	SET_BIT(ch->act, PLR_NOFOLLOW);
	die_follower(ch);
    }
}

void do_nosummon(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch)) {
	if (IS_SET(ch->imm_flags, IMM_SUMMON)) {
	    send_to_char("You are no longer immune to summon.\n\r", ch);
	    REMOVE_BIT(ch->imm_flags, IMM_SUMMON);
	} else {
	    send_to_char("You are now immune to summoning.\n\r", ch);
	    SET_BIT(ch->imm_flags, IMM_SUMMON);
	}
    } else {
	if (IS_SET(ch->act, PLR_NOSUMMON)) {
	    send_to_char("You are no longer immune to summon.\n\r", ch);
	    REMOVE_BIT(ch->act, PLR_NOSUMMON);
	} else {
	    send_to_char("You are now immune to summoning.\n\r", ch);
	    SET_BIT(ch->act, PLR_NOSUMMON);
	}
    }
}

void do_look(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number, count;

    if (ch->desc == NULL)
	return;

    if (ch->position < POS_SLEEPING) {
	send_to_char("You can't see anything but stars!\n\r", ch);
	return;
    }
    if (ch->position == POS_SLEEPING) {
	send_to_char("You can't see anything, you're sleeping!\n\r", ch);
	return;
    }
    if (!check_blind(ch))
	return;

    if (!IS_NPC(ch)
	&& !IS_SET(ch->act, PLR_HOLYLIGHT)
	&& room_is_dark(ch->in_room)
	&& !IS_AFFECTED(ch, AFF_DARK_VISION)) {
	send_to_char("It is pitch black ... \n\r", ch);
	show_char_to_char(ch->in_room->people, ch);
	return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    number = number_argument(arg1, arg3);
    count = 0;

    if (arg1[0] == '\0' || !str_cmp(arg1, "auto")) {
	/* 'look' or 'look auto' */
	send_to_char("{R", ch);
	send_to_char(ch->in_room->name, ch);
	send_to_char("{x", ch);

	if (
	    (IS_IMMORTAL(ch)
	     && (IS_NPC(ch) || IS_SET(ch->act, PLR_HOLYLIGHT)))
	    || (!IS_IMMORTAL(ch) && IS_BUILDER(ch, ch->in_room->area))) {
	    sprintf(buf, " [Room {C%ld{x] ", ch->in_room->vnum);
	    send_to_char(buf, ch);
	}
	send_to_char("\n\r{x", ch);

	if (arg1[0] == '\0'
	    || (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF))) {
	    send_to_char("{w  ", ch);
	    send_to_char(ch->in_room->description, ch);
	}
	if (IS_NPC(ch) || IS_SET(ch->act, PLR_AUTOEXIT)) {
	    send_to_char("\n\r{W", ch);
	    do_exits(ch, "auto");
	}
	send_to_char("{y", ch);
	show_list_to_char(ch->in_room->contents, ch, FALSE, FALSE);
	show_char_to_char(ch->in_room->people, ch);
	send_to_char("{x", ch);
	return;
    }
    if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in")
	|| !str_cmp(arg1, "on")) {
	/* 'look in' */
	if (arg2[0] == '\0') {
	    send_to_char("Look in what?\n\r", ch);
	    return;
	}
	if ((obj = get_obj_here(ch, arg2)) == NULL) {
	    send_to_char("You do not see that here.\n\r", ch);
	    return;
	}
	switch (obj->item_type) {
	default:
	    send_to_char("That is not a container.\n\r", ch);
	    break;

	case ITEM_DRINK_CON:
	    if (obj->value[1] <= 0) {
		send_to_char("It is empty.\n\r", ch);
		break;
	    }
	    sprintf(buf, "It's %sfilled with  a %s liquid.\n\r",
		    obj->value[1] < obj->value[0] / 4
		    ? "less than half-" :
		    obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-" : "more than half-",
		    liq_table[obj->value[2]].liq_color);

	    send_to_char(buf, ch);
	    break;

	case ITEM_BLOOD_CON:
	    if (obj->value[1] <= 0) {
		send_to_char("It is empty.\n\r", ch);
		break;
	    }
	    sprintf(buf, "It's %sfilled with  a %s liquid.\n\r",
		    obj->value[1] < obj->value[0] / 4
		    ? "less than half-" :
		    obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-" : "more than half-",
		    liq_table[obj->value[2]].liq_color);

	    send_to_char(buf, ch);
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if (IS_SET(obj->value[1], CONT_CLOSED)) {
		send_to_char("The container is closed.\n\r", ch);
		break;
	    }
	    if (((obj->pIndexData->vnum == OBJ_VNUM_MISC_PIT)
		 || (obj->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
		 || (obj->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT))) {
		act("$p holds:", ch, obj, NULL, TO_CHAR);
		act("{yQuan  Lev  Item{x", ch, obj, NULL, TO_CHAR);
		show_lev_list_to_char(obj->contains, ch, TRUE, TRUE);
	    } else {
		act("$p holds:", ch, obj, NULL, TO_CHAR);
		show_list_to_char(obj->contains, ch, TRUE, TRUE);
	    }
	    break;
	}
	return;
    }
    if ((victim = get_char_room(ch, arg1)) != NULL) {
	show_char_to_char_1(victim, ch);
	return;
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
	if (can_see_obj(ch, obj)) {	/* player can see object */
	    pdesc = get_extra_descr(arg3, obj->extra_descr);
	    if (pdesc != NULL) {
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    lore(ch, obj);
		    return;
		} else
		    continue;
	    }
	    pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
	    if (pdesc != NULL) {
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    lore(ch, obj);
		    return;
		} else
		    continue;
	    }
	    if (is_name(arg3, obj->name))
		if (++count == number) {
		    send_to_char(obj->description, ch);
		    send_to_char("\n\r", ch);
		    lore(ch, obj);
		    return;
		}
	}
    }

    for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content) {
	if (can_see_obj(ch, obj)) {
	    pdesc = get_extra_descr(arg3, obj->extra_descr);
	    if (pdesc != NULL)
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    lore(ch, obj);
		    return;
		}
	    pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
	    if (pdesc != NULL)
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    lore(ch, obj);
		    return;
		}
	    if (is_name(arg3, obj->name))
		if (++count == number) {
		    send_to_char(obj->description, ch);
		    send_to_char("\n\r", ch);
		    lore(ch, obj);
		    return;
		}
	}
    }

    pdesc = get_extra_descr(arg3, ch->in_room->extra_descr);
    if (pdesc != NULL) {
	if (++count == number) {
	    send_to_char(pdesc, ch);
	    return;
	}
    }
    if (count > 0 && count != number) {
	if (count == 1)
	    sprintf(buf, "You only see one %s here.\n\r", arg3);
	else
	    sprintf(buf, "You only see %d of those here.\n\r", count);

	send_to_char(buf, ch);
	return;
    }
    if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
	door = 0;
    else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
	door = 1;
    else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
	door = 2;
    else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
	door = 3;
    else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
	door = 4;
    else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
	door = 5;
    else {
	send_to_char("You do not see that here.\n\r", ch);
	return;
    }

    /* 'look direction' */
    if ((pexit = ch->in_room->exit[door]) == NULL) {
	send_to_char("Nothing special there.\n\r", ch);
	return;
    }
    if (pexit->description != NULL && pexit->description[0] != '\0')
	send_to_char(pexit->description, ch);
    else
	send_to_char("Nothing special there.\n\r", ch);

    if (pexit->keyword != NULL
	&& pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ') {
	if (IS_SET(pexit->exit_info, EX_CLOSED)) {
	    act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
	} else if (IS_SET(pexit->exit_info, EX_ISDOOR)) {
	    act("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
	}
    }
    return;
}

/* RT added back for the hell of it */
void do_read(CHAR_DATA * ch, char *argument)
{
    do_look(ch, argument);
}

void do_examine(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Examine what?\n\r", ch);
	return;
    }
    do_look(ch, arg);

    if ((obj = get_obj_here(ch, arg)) != NULL) {
	switch (obj->item_type) {
	default:
	    break;

	case ITEM_JUKEBOX:
	    do_play(ch, "list");
	    break;

	case ITEM_MONEY:
	    if (obj->value[1] == 0)
		sprintf(buf, "Odd...there's no coins in the pile.\n\r");
	    else if (obj->value[1] == 1)
		sprintf(buf, "Wow. One gold coin.\n\r");
	    else
		sprintf(buf, "There are %d gold coins in the pile.\n\r",
			obj->value[1]);
	    send_to_char(buf, ch);
	    break;

	case ITEM_DRINK_CON:
	case ITEM_BLOOD_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    sprintf(buf, "in %s", argument);
	    do_look(ch, buf);
	}
    }
    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits(CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    bool cansee = FALSE;
    int door;

    fAuto = !str_cmp(argument, "auto");

    if (!check_blind(ch))
	return;

    if (fAuto)
	sprintf(buf, "[Exits:");
    else if (IS_IMMORTAL(ch))
	sprintf(buf, "Obvious exits from room %ld:\n\r", ch->in_room->vnum);
    else
	sprintf(buf, "Obvious exits:\n\r");

    found = FALSE;
    for (door = 0; door <= (MAX_DIR - 1); door++) {
	if ((pexit = ch->in_room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && can_see_room(ch, pexit->u1.to_room)) {
	    found = TRUE;
	    if (fAuto) {
		if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
		    strcat(buf, " ");
		    strcat(buf, dir_name[door]);
		    cansee = TRUE;
		} else if (IS_SET(pexit->exit_info, EX_CLOSED)
			   && (!IS_SET(pexit->exit_info, EX_HIDDEN))) {
		    strcat(buf, " {g({W");
		    strcat(buf, dir_name[door]);
		    strcat(buf, "{g){W");
		    cansee = TRUE;
		} else if ((IS_SET(pexit->exit_info, EX_CLOSED)
			    || (IS_SET(pexit->exit_info, EX_HIDDEN)))
			   && IS_IMMORTAL(ch)) {
		    strcat(buf, " {g({R");
		    strcat(buf, dir_name[door]);
		    strcat(buf, "{g){W");
		}
	    } else {
		if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
		    sprintf(buf + strlen(buf), "%-5s - %s",
			    capitalize(dir_name[door]),
			    room_is_dark(pexit->u1.to_room)
			    ? "Too dark to tell"
			    : pexit->u1.to_room->name);
		    cansee = TRUE;
		} else if (IS_SET(pexit->exit_info, EX_CLOSED)
			   && (!IS_SET(pexit->exit_info, EX_HIDDEN))) {
		    sprintf(buf + strlen(buf), "%-5s - A closed door.",
			    capitalize(dir_name[door]));
		    cansee = TRUE;
		} else if ((IS_SET(pexit->exit_info, EX_CLOSED)
			    || (IS_SET(pexit->exit_info, EX_HIDDEN)))
			   && IS_IMMORTAL(ch)) {
		    sprintf(buf + strlen(buf), "%-5s ({rhidden{x)- %s",
			    capitalize(dir_name[door]),
			    pexit->u1.to_room->name);
		}
		if (IS_IMMORTAL(ch))
		    sprintf(buf + strlen(buf),
			    " (room {C%ld{x)\n\r", pexit->u1.to_room->vnum);
		else if (cansee)
		    sprintf(buf + strlen(buf), "\n\r");
	    }
	}			// if pexit
    }				// for

    if (!found)
	strcat(buf, fAuto ? " none" : "None.\n\r");

    if (fAuto)
	strcat(buf, "]\n\r");

    send_to_char(buf, ch);
    return;
}

void do_worth(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
	sprintf(buf, "You have {Y%ld{x gold.\n\r", ch->gold);
	send_to_char(buf, ch);
	return;
    }
    sprintf(buf,
	    "You have %ld gold and %ld experience (%ld exp to level).\n\r",
	    ch->gold, ch->exp, (ch->level + 1) * exp_per_level(ch,
							       ch->pcdata->
							       points) -
	    ch->exp);

    send_to_char(buf, ch);

    return;
}

void do_affects(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];

    if (ch->affected != NULL) {
	send_to_char("You are affected by the following spells:\n\r", ch);
	for (paf = ch->affected; paf != NULL; paf = paf->next) {
	    if (paf_last != NULL && paf->type == paf_last->type)
		if (ch->level >= 20)
		    sprintf(buf, "                      ");
		else
		    continue;
	    else
		sprintf(buf, "Spell: %-15s", skill_table[paf->type].name);

	    send_to_char(buf, ch);

	    if (ch->level >= 20) {
		sprintf(buf,
			": modifies %s by %d ",
			affect_loc_name(paf->location), paf->modifier);
		send_to_char(buf, ch);
		if (paf->duration == -1)
		    sprintf(buf, "permanently");
		else
		    sprintf(buf, "for %d hours", paf->duration);
		send_to_char(buf, ch);
	    }
	    send_to_char("\n\r", ch);
	    paf_last = paf;
	}
    } else
	send_to_char("You are not affected by any spells.\n\r", ch);

    return;
}



char *const day_name[] = {
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *const month_name[] = {
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time(CHAR_DATA * ch, char *argument)
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day = time_info.day + 1;

    if (day > 4 && day < 20)
	suf = "th";
    else if (day % 10 == 1)
	suf = "st";
    else if (day % 10 == 2)
	suf = "nd";
    else if (day % 10 == 3)
	suf = "rd";
    else
	suf = "th";

    sprintf(buf,
	    "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
	    (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	    time_info.hour >= 12 ? "pm" : "am",
	    day_name[day % 7], day, suf, month_name[time_info.month]);
    send_to_char(buf, ch);
    sprintf(buf, "ROM started up at %s\n\rThe system time is %s.\n\r",
	    str_boot_time, (char *) ctime(&current_time)
	);

    send_to_char(buf, ch);
    return;
}



void do_weather(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    static char *const sky_look[4] = {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if (!IS_OUTSIDE(ch)) {
	send_to_char("You can't see the weather indoors.\n\r", ch);
	return;
    }
    sprintf(buf, "The sky is %s and %s.\n\r",
	    sky_look[weather_info.sky],
	    weather_info.change >= 0
	    ? "a warm southerly breeze blows"
	    : "a cold northern gust blows");
    send_to_char(buf, ch);
    return;
}

void do_help(CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
    int level;

    output = new_buf();

    if (argument[0] == '\0')
	argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
	argument = one_argument(argument, argone);
	if (argall[0] != '\0')
	    strcat(argall, " ");
	strcat(argall, argone);
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

	if (level > get_trust(ch))
	    continue;

	if (is_name(argall, pHelp->keyword)) {
	    /* add seperator if found */
	    if (found)
		add_buf(output,
			"\n\r============================================================\n\r\n\r");
	    if (pHelp->level >= 0 && str_cmp(argall, "imotd")) {
		add_buf(output, pHelp->keyword);
		add_buf(output, "\n\r");
	    }
	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if (pHelp->text[0] == '.')
		add_buf(output, pHelp->text + 1);
	    else
		add_buf(output, pHelp->text);
	    found = TRUE;
	    /* small hack :) */
	    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
		&& ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if (!found)
	send_to_char("That doesn't need any help.\n\r", ch);
    else
	page_to_char(buf_string(output), ch);
    free_buf(output);
}

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 *
 * death to 300 lines of "who" code.
 */
void do_who(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
	CHAR_DATA *wch;

    output = new_buf();

    do_count(ch, " ");
	send_to_char("\r\n", ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
		if (d->connected != CON_PLAYING || !can_see(ch, d->character))
        	continue;

  		wch = (d->original != NULL) ? d->original : d->character;

		if ( IS_IMMORTAL(wch) ) {
	   		sprintf(buf, "{x  %s\n\r", wch->pcdata->title);
		} else {

	    	sprintf(buf, "{x  %s the %s %s %s.\n\r",
	        	wch->name,
	        	"young",
	        	wch->race->who_name, 
				class_table[wch->class].name);
		}
	    	add_buf(output, buf);
    }

	sprintf( buf, "\r\n");
    add_buf(output, buf);
    page_to_char(buf_string(output), ch);
    free_buf(output);

	return;
}


void do_count(CHAR_DATA * ch, char *argument)
{
    int count;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    count = 0;

    for (d = descriptor_list; d != NULL; d = d->next)
	if (d->connected == CON_PLAYING && can_see(ch, d->character))
	    count++;

    max_on = UMAX(count, max_on);

    if (max_on == count)
	sprintf(buf,
		"There are {C%d{x characters on, the most so far this boot.\n\r",
		count);
    else
	sprintf(buf,
		"There are {C%d{x characters on, the most on this boot was {C%d{x.\n\r",
		count, max_on);

    send_to_char(buf, ch);
}

void do_inventory(CHAR_DATA * ch, char *argument)
{
    send_to_char("{yYou are carrying:{x\n\r", ch);
    show_list_to_char(ch->carrying, ch, TRUE, TRUE);
    return;
}


void do_equipment(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    int iWear, count;
    char buf[MAX_STRING_LENGTH];

    send_to_char("{x\r\n", ch);

    for (iWear = 0; iWear < MAX_WEAR; iWear++) {

	if ((obj = get_eq_char(ch, iWear)) == NULL)
	    continue;

	if (ch->level - obj->level < 5) {
	    send_to_char("{g", ch);

	} else if (ch->level - obj->level < 10) {
	    send_to_char("{y", ch);

	} else {
	    send_to_char("{r", ch);
	}

	send_to_char(where_name[iWear], ch);
	if (can_see_obj(ch, obj)) {
	    send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
	    send_to_char("\n\r", ch);
	} else {
	    send_to_char("{ysomething.{x\n\r", ch);
	}
    }

    send_to_char("\r\n{RMissing EQ:{y\r\n\r\n", ch);

    count = 0;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
	if ((obj = get_eq_char(ch, iWear)) == NULL) {
	    sprintf(buf, "%-20s", empty_name[iWear]);
	    send_to_char(buf, ch);
	    count++;
	}
	if (count > 3) {
	    send_to_char("\r\n", ch);
	    count = 0;
	}
    }

    send_to_char("{x\r\n", ch);
    return;
}



void do_compare(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if (arg1[0] == '\0') {
	send_to_char("Compare what to what?\n\r", ch);
	return;
    }
    if ((obj1 = get_obj_carry(ch, arg1, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    if (arg2[0] == '\0') {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content) {
	    if (obj2->wear_loc != WEAR_NONE && can_see_obj(ch, obj2)
		&& obj1->item_type == obj2->item_type
		&& (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
		break;
	}

	if (obj2 == NULL) {
	    send_to_char("You aren't wearing anything comparable.\n\r",
			 ch);
	    return;
	}
    } else if ((obj2 = get_obj_carry(ch, arg2, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    msg = NULL;
    value1 = 0;
    value2 = 0;

    if (obj1 == obj2) {
	msg = "You compare $p to itself.  It looks about the same.";
    } else if (obj1->item_type != obj2->item_type) {
	msg = "You can't compare $p and $P.";
    } else {
	switch (obj1->item_type) {
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;

	case ITEM_WEAPON:
	    if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	    else
		value1 = obj1->value[1] + obj1->value[2];

	    if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	    else
		value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if (msg == NULL) {
	if (value1 == value2)
	    msg = "$p and $P look about the same.";
	else if (value1 > value2)
	    msg = "$p looks better than $P.";
	else
	    msg = "$p looks worse than $P.";
    }
    act(msg, ch, obj1, obj2, TO_CHAR);
    return;
}



void do_credits(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "diku");
    return;
}



void do_where(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Players near you:\n\r", ch);
	found = FALSE;
	for (d = descriptor_list; d; d = d->next) {
	    if (d->connected == CON_PLAYING
		&& (victim = d->character) != NULL && !IS_NPC(victim)
		&& victim->in_room != NULL
		&& !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE)
		&& (is_room_owner(ch, victim->in_room)
		    || !room_is_private(victim->in_room))
		&& victim->in_room->area == ch->in_room->area
		&& can_see(ch, victim)) {
		found = TRUE;
		sprintf(buf, "%-28s %s\n\r",
			victim->name, victim->in_room->name);
		send_to_char(buf, ch);
	    }
	}
	if (!found)
	    send_to_char("None\n\r", ch);
    } else {
	found = FALSE;
	for (victim = char_list; victim != NULL; victim = victim->next) {
	    if (victim->in_room != NULL
		&& victim->in_room->area == ch->in_room->area
		&& !IS_AFFECTED(victim, AFF_HIDE)
		&& !IS_AFFECTED(victim, AFF_SNEAK)
		&& can_see(ch, victim)
		&& is_name(arg, victim->name)) {
		found = TRUE;
		sprintf(buf, "%-28s %s\n\r",
			PERS(victim, ch), victim->in_room->name);
		send_to_char(buf, ch);
		break;
	    }
	}
	if (!found)
	    act("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
    }

    return;
}




void do_consider(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Consider killing whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They're not here.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim)) {
	send_to_char("Don't even think about it.\n\r", ch);
	return;
    }
    diff = victim->hit - ch->hit;

    if (diff <= -1000)
	msg = "You are MUCH healthier than $N.";
    else if (diff <= -600)
	msg = "You are a lot healthier than $N.";
    else if (diff <= -300)
	msg = "You are a little healthier than $N.";
    else if (diff <= -100 || diff <= 100)
	msg = "You both have about the same amount of health.";
    else if (diff <= 300)
	msg = "$N is a little healther than you.";
    else if (diff <= 600)
	msg = "$N is a lot healther than you";
    else if (diff <= 1000)
	msg = "$N is MUCH healthier than you.";
    else
	msg = "$N is so much healthier than you, it's not even funny.";

    act(msg, ch, NULL, victim, TO_CHAR);

    diff = victim->level - ch->level;

    if (diff <= -10)
	msg = "You can kill $N naked and weaponless.";
    else if (diff <= -5)
	msg = "$N is no match for you.";
    else if (diff <= -2)
	msg = "$N looks like an easy kill.";
    else if (diff <= 1)
	msg = "The perfect match!";
    else if (diff <= 4)
	msg = "$N says 'Do you feel lucky, punk?'.";
    else if (diff <= 9)
	msg = "$N laughs at you mercilessly.";
    else if (diff <= 19)
	msg = "$N says 'you and what army?'.";
    else if (diff <= 29)
	msg = "$N smiles at the thought of killing you.";
    else
	msg = "Death will thank you for your gift.";

    act(msg, ch, NULL, victim, TO_CHAR);
    return;
}



void set_title(CHAR_DATA * ch, char *title)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
	bug("Set_title: NPC.", 0);
	return;
    }
    if (title[0] != '.' && title[0] != ',' && title[0] != '!'
	&& title[0] != '?') {
	buf[0] = ' ';
	strcpy(buf + 1, title);
    } else {
	strcpy(buf, title);
    }

    free_string(ch->pcdata->title);
    ch->pcdata->title = str_dup(buf);
    return;
}



void do_title(CHAR_DATA * ch, char *argument)
{
	int count;

    if (IS_NPC(ch))
	return;
	
	for( count = 0; count < strlen(argument); count ++) {

		// castrate beeps, linefeeds
		if( ( argument[count] == '{')) {
			if ( (argument[count + 1] == '/')  
				|| (argument[count + 1] == '*') )
					argument[count + 1] = '!';
		}
	}


    smash_tilde(argument);
    set_title(ch, argument);
    send_to_char("Title set.\n\r", ch);
	return;
}



void do_description(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] != '\0') {
	buf[0] = '\0';
	smash_tilde(argument);

	if (argument[0] == '-') {
	    int len;
	    bool found = FALSE;

	    if (ch->description == NULL || ch->description[0] == '\0') {
		send_to_char("No lines left to remove.\n\r", ch);
		return;
	    }
	    strcpy(buf, ch->description);

	    for (len = strlen(buf); len > 0; len--) {
		if (buf[len] == '\r') {
		    if (!found) {	/* back it up */
			if (len > 0)
			    len--;
			found = TRUE;
		    } else {	/* found the second one */
			buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			send_to_char("Your description is:\n\r", ch);
			send_to_char(ch->description ? ch->description :
				     "(None).\n\r", ch);
			return;
		    }
		}
	    }
	    buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    send_to_char("Description cleared.\n\r", ch);
	    return;
	}
	if (argument[0] == '+') {
	    if (ch->description != NULL)
		strcat(buf, ch->description);
	    argument++;
	    while (isspace(*argument))
		argument++;
	}
	if (strlen(buf) >= 1024) {
	    send_to_char("Description too long.\n\r", ch);
	    return;
	}
	strcat(buf, argument);
	strcat(buf, "\n\r");
	free_string(ch->description);
	ch->description = str_dup(buf);
    }
    send_to_char("Your description is:\n\r", ch);
    send_to_char(ch->description ? ch->description : "(None).\n\r", ch);
    return;
}



void do_report(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    sprintf(buf,
	    "You say 'I have %ld/%ld hp %ld/%ld mana %ld/%ld mv %ld xp.'\n\r",
	    ch->hit, ch->max_hit,
	    ch->mana, ch->max_mana, ch->move, ch->max_move, ch->exp);

    send_to_char(buf, ch);

    sprintf(buf,
	    "$n says 'I have %ld/%ld hp %ld/%ld mana %ld/%ld mv %ld xp.'",
	    ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
	    ch->max_move, ch->exp);

    act(buf, ch, NULL, NULL, TO_ROOM);

    return;
}



void do_practice(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if (IS_NPC(ch)) {
	return;
    }

    if (argument[0] == '\0') {
	int col;

	col = 0;
	for (sn = 0; sn < MAX_SKILL; sn++) {
	    if (skill_table[sn].name == NULL) {
		break;
	    }
	    if (ch->level < skill_table[sn].skill_level[ch->class]
		|| ch->pcdata->learned[sn] < 1	/* skill is not known */
		|| (skill_table[sn].rating[ch->class] == -1 &&
		    ch->level <= 16)) {
		continue;
	    }

	    sprintf(buf, "%-18s %3d%%  ",
		    skill_table[sn].name, ch->pcdata->learned[sn]);
	    send_to_char(buf, ch);
	    if (++col % 3 == 0) {
		send_to_char("\n\r", ch);
	    }
	}

	if (col % 3 != 0) {
	    send_to_char("\n\r", ch);
	}

	sprintf(buf, "You have %d practice sessions left.\n\r",
		ch->practice);
	send_to_char(buf, ch);
    } else {
	CHAR_DATA *mob;
	int adept;

	if (!IS_AWAKE(ch)) {
	    send_to_char("In your dreams, or what?\n\r", ch);
	    return;
	}
	for (mob = ch->in_room->people; mob != NULL;
	     mob = mob->next_in_room) {
	    if (IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE)) {
		break;
	    }
	}

	if (mob == NULL) {
	    send_to_char("You can't do that here.\n\r", ch);
	    return;
	}

	if (ch->practice <= 0) {
	    send_to_char("You have no practice sessions left.\n\r", ch);
	    return;
	}

	if ((sn = skill_lookup(argument)) < 0 || (!IS_NPC(ch)
						  && (ch->level <
						      skill_table[sn].
						      skill_level[ch->
								  class]
						      || ch->pcdata->
						      learned[sn] < 1	/* skill is not known */
						      || (skill_table[sn].
							  rating[ch->
								 class] ==
							  -1
							  && ch->level <=
							  16)))) {
	    send_to_char("You can't practice that.\n\r", ch);
	    return;
	}
	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

	if (ch->pcdata->learned[sn] >= adept) {
	    sprintf(buf, "You are already learned at %s.\n\r",
		    skill_table[sn].name);
	    send_to_char(buf, ch);
	} else {
	    ch->practice--;
	    if (skill_table[sn].rating[ch->class] == -1) {
		ch->pcdata->learned[sn] +=
		    int_app[get_curr_stat(ch, STAT_INT)].learn / 1;
	    } else {
		ch->pcdata->learned[sn] +=
		    int_app[get_curr_stat(ch, STAT_INT)].learn /
		    skill_table[sn].rating[ch->class];
	    }

	    if (ch->pcdata->learned[sn] < adept) {
		act("You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR);
		act("$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM);
	    } else {
		ch->pcdata->learned[sn] = adept;
		act("You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR);
		act("$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM);
	    }
	}
    }
    return;
}


/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument(argument, arg);

    if (arg[0] == '\0')
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi(arg);

    if (wimpy < 0) {
	send_to_char("Your courage exceeds your wisdom.\n\r", ch);
	return;
    }
    if (wimpy > ch->max_hit / 2) {
	send_to_char("You cowardly wuss!\n\r", ch);
	return;
    }
    ch->wimpy = wimpy;
    sprintf(buf, "Wimpy set to {C%d{x hit points.\n\r", wimpy);
    send_to_char(buf, ch);
    return;
}



void do_password(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if (IS_NPC(ch))
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while (isspace(*argument))
	argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
	cEnd = *argument++;

    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while (isspace(*argument))
	argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
	cEnd = *argument++;

    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if (arg1[0] == '\0' || arg2[0] == '\0') {
	send_to_char("Syntax: password <old> <new>.\n\r", ch);
	return;
    }
    if (strcmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd)) {
	WAIT_STATE(ch, 40);
	send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
	return;
    }
    if (strlen(arg2) < 5) {
	send_to_char
	    ("New password must be at least five characters long.\n\r",
	     ch);
	return;
    }
    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt(arg2, ch->name);
    for (p = pwdnew; *p != '\0'; p++) {
	if (*p == '~') {
	    send_to_char("New password not acceptable, try again.\n\r",
			 ch);
	    return;
	}
    }

    free_string(ch->pcdata->pwd);
    ch->pcdata->pwd = str_dup(pwdnew);
    save_char_obj(ch);
    send_to_char("Ok.\n\r", ch);
    return;
}


void do_version(CHAR_DATA * ch)
{

    char buf[MAX_STRING_LENGTH];

    sprintf(buf, "\r\nAlanthia Code Revision %d.%d.%d\r\n", CODE_BASE,
	    CODE_REV, CODE_POINT);
    send_to_char(buf, ch);

    return;

}

void do_seduce(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0')
	 {
	send_to_char("Seduce whom?\n\r", ch);
	return;
	}
    if ((victim = get_char_room(ch, arg)) == NULL)
	 {
	send_to_char("They're not here.\n\r", ch);
	return;
	}
    if (!IS_SET(ch->vuln_flags, VULN_SEDUCE))
	 {
	send_to_char("They aren't interested in your affection.\n\r", ch);
	return;
	}
    if (ch->master != NULL)
	stop_follower(ch);

  add_follower(ch, victim);

    return;
}

/* EOF */
