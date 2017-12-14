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
DECLARE_DO_FUN(do_split);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_inventory);



/*
 * Local functions.
 */
#define CD CHAR_DATA
#define OD OBJ_DATA
bool remove_obj args((CHAR_DATA * ch, int iWear, bool fReplace));
void wear_obj args((CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace));
int get_cost args((CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy));
void obj_to_keeper args((OBJ_DATA * obj, CHAR_DATA * ch));
OD *get_obj_keeper
args((CHAR_DATA * ch, CHAR_DATA * keeper, char *argument));

#undef OD
#undef	CD

/* RT part of the corpse looting code */

bool can_loot(CHAR_DATA * ch, OBJ_DATA * obj)
{
    CHAR_DATA *owner, *wch;

    if (IS_IMMORTAL(ch))
	return TRUE;

    if (!obj->owner || obj->owner == NULL)
	return TRUE;

    owner = NULL;
    for (wch = char_list; wch != NULL; wch = wch->next)
	if (!str_cmp(wch->name, obj->owner))
	    owner = wch;

    if (owner == NULL)
	return TRUE;

    if (!str_cmp(ch->name, owner->name))
	return TRUE;

    if (!IS_NPC(owner) && IS_SET(owner->act, PLR_CANLOOT))
	return TRUE;

    if (is_same_group(ch, owner))
	return TRUE;

    return FALSE;
}


void get_obj(CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container)
{
    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];

    if (!CAN_WEAR(obj, ITEM_TAKE)) {
	send_to_char("You can't take that.\n\r", ch);
	return;
    }
    if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch)) {
	act("$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR);
	return;
    }
    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
	&& (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch))) {
	act("$d: you can't carry that much weight.",
	    ch, NULL, obj->name, TO_CHAR);
	return;
    }

    if (!can_loot(ch, obj)) {
		act("Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR);
		return;
    }

    if (obj->in_room != NULL) {
		for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room) 
			if (gch->on == obj) {
				act("$N appears to be using $p.", ch, obj, gch, TO_CHAR);
			return;
	    }
    }

    if (container != NULL) {
	if (((container->pIndexData->vnum == OBJ_VNUM_MISC_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT))
	    && get_trust(ch) < (obj->level - 5)) {
	    send_to_char("You are not powerful enough to use it.\n\r", ch);
	    return;
	}
	if (((container->pIndexData->vnum == OBJ_VNUM_MISC_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT))
	    && !CAN_WEAR(container, ITEM_TAKE)
	    && !IS_OBJ_STAT(obj, ITEM_HAD_TIMER))
	    obj->timer = 0;
	act("You get $p from $P.", ch, obj, container, TO_CHAR);
	act("$n gets $p from $P.", ch, obj, container, TO_ROOM);
	REMOVE_BIT(obj->extra_flags, ITEM_HAD_TIMER);
	obj_from_obj(obj);
    } else {
	act("You get $p.", ch, obj, container, TO_CHAR);
	act("$n gets $p.", ch, obj, container, TO_ROOM);
	obj_from_room(obj);
    }

    if (obj->item_type == ITEM_MONEY) {
	ch->gold += obj->value[1];
	if (IS_SET(ch->act, PLR_AUTOSPLIT)) {	/* AUTOSPLIT code */
	    members = 0;
	    for (gch = ch->in_room->people; gch != NULL;
		 gch = gch->next_in_room) {
		if (!IS_AFFECTED(gch, AFF_CHARM) && is_same_group(gch, ch))
		    members++;
	    }

	    if (members > 1 && obj->value[1]) {
		sprintf(buffer, "%d", obj->value[1]);
		do_split(ch, buffer);
	    }
	}
	extract_obj(obj);
    } else {
	obj_to_char(obj, ch);
    }

    return;
}



void do_get(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!str_cmp(arg2, "from"))
	argument = one_argument(argument, arg2);

    /* Get type. */
    if (arg1[0] == '\0') {
	send_to_char("Get what?\n\r", ch);
	return;
    }
    if (arg2[0] == '\0') {
	if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
	    /* 'get obj' */
	    obj = get_obj_list(ch, arg1, ch->in_room->contents);
	    if (obj == NULL) {
		act("I see no $T here.", ch, NULL, arg1, TO_CHAR);
		return;
	    }
	    get_obj(ch, obj, NULL);
	} else {
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
		obj_next = obj->next_content;
		if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
		    && can_see_obj(ch, obj)) {
		    found = TRUE;
		    get_obj(ch, obj, NULL);
		}
	    }

	    if (!found) {
		if (arg1[3] == '\0')
		    send_to_char("I see nothing here.\n\r", ch);
		else
		    act("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR);
	    }
	}
    } else {
	/* 'get ... container' */
	if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2)) {
	    send_to_char("You can't do that.\n\r", ch);
	    return;
	}
	if ((container = get_obj_here(ch, arg2)) == NULL) {
	    act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
	    return;
	}
	switch (container->item_type) {
	default:
	    send_to_char("That's not a container.\n\r", ch);
	    return;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    {
		if (!can_loot(ch, container)) {
		    send_to_char("You can't do that.\n\r", ch);
		    return;
		}
	    }

	    if (IS_SET(container->value[1], CONT_CLOSED)) {
		act("The $d is closed.", ch, NULL, container->name,
		    TO_CHAR);
		return;
	    }
	    if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
		/* 'get obj container' */
		obj = get_obj_list(ch, arg1, container->contains);
		if (obj == NULL) {
		    act("I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR);
		    return;
		}
		get_obj(ch, obj, container);
	    } else {
		/* 'get all container' or 'get all.obj container' */
		found = FALSE;
		for (obj = container->contains; obj != NULL;
		     obj = obj_next) {
		    obj_next = obj->next_content;
		    if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
			&& can_see_obj(ch, obj)) {
			found = TRUE;
			if (
			    ((container->
			      pIndexData->vnum == OBJ_VNUM_MISC_PIT)
			     || (container->pIndexData->vnum ==
				 OBJ_VNUM_WEAPON_PIT)
			     || (container->pIndexData->vnum ==
				 OBJ_VNUM_ARMOR_PIT)) && !IS_IMMORTAL(ch)) {
			    send_to_char("Don't be so greedy!\n\r", ch);
			    return;
			}
			get_obj(ch, obj, container);
		    }
		}

		if (!found) {
		    if (arg1[3] == '\0')
			act("I see nothing in the $T.",
			    ch, NULL, arg2, TO_CHAR);
		    else
			act("I see nothing like that in the $T.",
			    ch, NULL, arg2, TO_CHAR);
		}
	    }
	}

	return;
    }
}


void do_put(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!str_cmp(arg2, "in") || !str_cmp(arg2, "on"))
	argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
	send_to_char("Put what in what?\n\r", ch);
	return;
    }
    if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2)) {
	send_to_char("You can't do that.\n\r", ch);
	return;
    }
    if ((container = get_obj_here(ch, arg2)) == NULL) {
	act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
	return;
    }
    if (container->item_type != ITEM_CONTAINER) {
	send_to_char("That's not a container.\n\r", ch);
	return;
    }
    if (IS_SET(container->value[1], CONT_CLOSED)) {
	act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
	return;
    }
    if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) {
	/* 'put obj container' */
	if ((obj = get_obj_carry(ch, arg1, ch)) == NULL) {
	    send_to_char("You do not have that item.\n\r", ch);
	    return;
	}
	if (obj == container) {
	    send_to_char("You can't fold it into itself.\n\r", ch);
	    return;
	}
	if (!can_drop_obj(ch, obj)) {
	    send_to_char("You can't let go of it.\n\r", ch);
	    return;
	}
	if (WEIGHT_MULT(obj) != 100) {
	    send_to_char
		("You have a feeling that would be a bad idea.\n\r", ch);
	    return;
	}
	if (get_obj_weight(obj) + get_true_weight(container)
	    > (container->value[0] * 10)
	    || get_obj_weight(obj) > (container->value[3] * 10)) {
	    send_to_char("It won't fit.\n\r", ch);
	    return;
	}
	if ((container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	    && (obj->item_type != ITEM_WEAPON)) {
	    send_to_char("You may only place weapons in this pit.\n\r",
			 ch);
	    return;
	}
	if ((container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT)
	    && (obj->item_type != ITEM_ARMOR)) {
	    send_to_char("You may only place armor in this pit.\n\r", ch);
	    return;
	}
	if (((container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_MISC_PIT))
	    && (container->in_room->vnum == ROOM_VNUM_LOWBIE_ALTAR)
	    && (obj->level > 30)) {
	    send_to_char
		("This pit is designed to hold items of a level lower than 30.\n\r",
		 ch);
	    return;
	}
	if (((container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_MISC_PIT))
	    && (container->in_room->vnum == ROOM_VNUM_MIDBIE_ALTAR)
	    && ((obj->level < 31) || (obj->level > 70))) {
	    send_to_char
		("This pit holds items between the levels of 31 and 70.\n\r",
		 ch);
	    return;
	}
	if (((container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_MISC_PIT))
	    && (container->in_room->vnum == ROOM_VNUM_HIGHBIE_ALTAR)
	    && (obj->level < 71)) {
	    send_to_char
		("This pit is designed to hold items of a level above 70.\n\r",
		 ch);
	    return;
	}
	if (((container->pIndexData->vnum == OBJ_VNUM_MISC_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_WEAPON_PIT)
	     || (container->pIndexData->vnum == OBJ_VNUM_ARMOR_PIT))
	    && !CAN_WEAR(container, ITEM_TAKE)) {
	    if (obj->timer)
		SET_BIT(obj->extra_flags, ITEM_HAD_TIMER);
	    else
		obj->timer = number_range(100, 200);
	}
	obj_from_char(obj);
	obj_to_obj(obj, container);

	if (IS_SET(container->value[1], CONT_PUT_ON)) {
	    act("$n puts $p on $P.", ch, obj, container, TO_ROOM);
	    act("You put $p on $P.", ch, obj, container, TO_CHAR);
	} else {
	    act("$n puts $p in $P.", ch, obj, container, TO_ROOM);
	    act("You put $p in $P.", ch, obj, container, TO_CHAR);
	}
    } else {
	/* 'put all container' or 'put all.obj container' */
	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

	    if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
		&& can_see_obj(ch, obj)
		&& WEIGHT_MULT(obj) == 100
		&& obj->wear_loc == WEAR_NONE
		&& obj != container && can_drop_obj(ch, obj)
		&& get_obj_weight(obj) + get_true_weight(container)
		<= (container->value[0] * 10)
		&& get_obj_weight(obj) < (container->value[3] * 10)) {

		if (((container->pIndexData->vnum == OBJ_VNUM_MISC_PIT)
		     || (container->pIndexData->vnum ==
			 OBJ_VNUM_WEAPON_PIT)
		     || (container->pIndexData->vnum ==
			 OBJ_VNUM_ARMOR_PIT)) && !CAN_WEAR(obj, ITEM_TAKE)) {
		    if (obj->timer)
			SET_BIT(obj->extra_flags, ITEM_HAD_TIMER);
		    else
			obj->timer = number_range(100, 200);
		}
		obj_from_char(obj);
		obj_to_obj(obj, container);

	    }
	}

	if (IS_SET(container->value[1], CONT_PUT_ON)) {
	    act("$n empties $s inventory onto $P.", ch, obj, container,
		TO_ROOM);
	    act("You empty your inventory into $P.", ch, obj, container,
		TO_CHAR);
	} else {
	    act("$n empties $s inventory into $P.", ch, obj, container,
		TO_ROOM);
	    act("You empty your inventory into $P.", ch, obj, container,
		TO_CHAR);
	}


    }

    return;
}



void do_drop(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Drop what?\n\r", ch);
	return;
    }
    if (is_number(arg)) {
	/* 'drop NNNN coins' */
	int amount, gold = 0;

	amount = atoi(arg);
	argument = one_argument(argument, arg);
	if (amount <= 0
	    || (str_cmp(arg, "coins") && str_cmp(arg, "coin") &&
		str_cmp(arg, "gold"))) {
	    send_to_char("Sorry, you can't do that.\n\r", ch);
	    return;
	} else {
	    if (ch->gold < amount) {
		send_to_char("You don't have that much gold.\n\r", ch);
		return;
	    }
	    ch->gold -= amount;
	    gold = amount;
	}

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

	    switch (obj->pIndexData->vnum) {
	    case OBJ_VNUM_GOLD_ONE:
		gold += 1;
		extract_obj(obj);
		break;

	    case OBJ_VNUM_GOLD_SOME:
		gold += obj->value[1];
		extract_obj(obj);
		break;
	    }
	}

	obj_to_room(create_money(gold), ch->in_room);
	act("$n drops some coins.", ch, NULL, NULL, TO_ROOM);
	send_to_char("OK.\n\r", ch);
	return;
    }
    if (str_cmp(arg, "all") && str_prefix("all.", arg)) {
	/* 'drop obj' */
	if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	    send_to_char("You do not have that item.\n\r", ch);
	    return;
	}
	if (!can_drop_obj(ch, obj)) {
	    send_to_char("You can't let go of it.\n\r", ch);
	    return;
	}
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
	act("$n drops $p.", ch, obj, NULL, TO_ROOM);
	act("You drop $p.", ch, obj, NULL, TO_CHAR);
	if (IS_OBJ_STAT(obj, ITEM_MELT_DROP)) {
	    act("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM);
	    act("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR);
	    extract_obj(obj);
	}
    } else {
	/* 'drop all' or 'drop all.obj' */
	found = FALSE;
	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

	    if ((arg[3] == '\0' || is_name(&arg[4], obj->name))
		&& can_see_obj(ch, obj)
		&& obj->wear_loc == WEAR_NONE && can_drop_obj(ch, obj)) {
		found = TRUE;
		obj_from_char(obj);
		obj_to_room(obj, ch->in_room);
		if (IS_OBJ_STAT(obj, ITEM_MELT_DROP)) {
		    act("$p dissolves into smoke.", ch, obj, NULL,
			TO_ROOM);
		    act("$p dissolves into smoke.", ch, obj, NULL,
			TO_CHAR);
		    extract_obj(obj);
		}
	    }
	}
	act("$n empties $s inventory onto the floor.", ch, ch, NULL,
	    TO_ROOM);
	act("You drop everything.", ch, NULL, NULL, TO_CHAR);

	if (!found) {
	    if (arg[3] == '\0')
		act("You are not carrying anything.",
		    ch, NULL, arg, TO_CHAR);
	    else
		act("You are not carrying any $T.",
		    ch, NULL, &arg[4], TO_CHAR);
	}
    }

    return;
}



void do_give(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0') {
	send_to_char("Give what to whom?\n\r", ch);
	return;
    }
    if (is_number(arg1)) {
	/* 'give NNNN coins victim' */
	int amount;

	amount = atoi(arg1);
	if (amount <= 0
	    || (str_cmp(arg2, "coins") && str_cmp(arg2, "coin") &&
		str_cmp(arg2, "gold"))) {
	    send_to_char("Sorry, you can't do that.\n\r", ch);
	    return;
	}
	argument = one_argument(argument, arg2);
	if (arg2[0] == '\0') {
	    send_to_char("Give what to whom?\n\r", ch);
	    return;
	}
	if ((victim = get_char_room(ch, arg2)) == NULL) {
	    send_to_char("They aren't here.\n\r", ch);
	    return;
	}
	if (ch->gold < amount) {
	    send_to_char("You haven't got that much.\n\r", ch);
	    return;
	}
	ch->gold -= amount;
	victim->gold += amount;

	sprintf(buf, "$n gives you %d gold.", amount);
	act(buf, ch, NULL, victim, TO_VICT);
	act("$n gives $N some coins.", ch, NULL, victim, TO_NOTVICT);
	sprintf(buf, "You give $N %d gold.", amount);
	act(buf, ch, NULL, victim, TO_CHAR);

	/*
	 * Bribe trigger
	 */
	if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_BRIBE))
	    mp_bribe_trigger(victim, ch, amount);

	return;
    }
    if ((obj = get_obj_carry(ch, arg1, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    if (obj->wear_loc != WEAR_NONE) {
	send_to_char("You must remove it first.\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg2)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL) {
	act("$N tells you 'Sorry, you'll have to sell that.'",
	    ch, NULL, victim, TO_CHAR);
	ch->reply = victim;
	return;
    }
    if (!can_drop_obj(ch, obj)) {
	send_to_char("You can't let go of it.\n\r", ch);
	return;
    }
    if (victim->carry_number + get_obj_number(obj) > can_carry_n(victim)) {
	act("$N has $S hands full.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (get_carry_weight(victim) + get_obj_weight(obj) >
	can_carry_w(victim)) {
	act("$N can't carry that much weight.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (!can_see_obj(victim, obj)) {
	act("$N can't see it.", ch, NULL, victim, TO_CHAR);
	return;
    }
    obj_from_char(obj);
    MOBtrigger = FALSE;
    obj_to_char(obj, victim);
    act("$n gives $p to $N.", ch, obj, victim, TO_NOTVICT);
    act("$n gives you $p.", ch, obj, victim, TO_VICT);
    act("You give $p to $N.", ch, obj, victim, TO_CHAR);
    MOBtrigger = TRUE;

    /*
     * Give trigger
     */
    if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_GIVE))
	mp_give_trigger(victim, ch, obj);

    return;
}


/* for poisoning weapons and food/drink */
void do_envenom(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent, skill;

    /* find out what */
    if (argument == '\0') {
	send_to_char("Envenom what item?\n\r", ch);
	return;
    }
    obj = get_obj_list(ch, argument, ch->carrying);

    if (obj == NULL) {
	send_to_char("You don't have that item.\n\r", ch);
	return;
    }
    if ((skill = get_skill(ch, gsn_envenom)) < 1) {
	send_to_char("Are you crazy? You'd poison yourself!\n\r", ch);
	return;
    }
    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON
	|| obj->item_type == ITEM_BLOOD_CON) {
	if (IS_OBJ_STAT(obj, ITEM_BLESS)
	    || IS_OBJ_STAT(obj, ITEM_BURN_PROOF)) {
	    act("You fail to poison $p.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	if (number_percent() < skill) {	/* success! */
	    act("$n treats $p with deadly poison.", ch, obj, NULL,
		TO_ROOM);
	    act("You treat $p with deadly poison.", ch, obj, NULL,
		TO_CHAR);
	    if (!obj->value[3]) {
		obj->value[3] = 1;
		check_improve(ch, gsn_envenom, TRUE, 4);
	    }
	    WAIT_STATE(ch, skill_table[gsn_envenom].beats);
	    return;
	}
	act("You fail to poison $p.", ch, obj, NULL, TO_CHAR);
	if (!obj->value[3])
	    check_improve(ch, gsn_envenom, FALSE, 4);
	WAIT_STATE(ch, skill_table[gsn_envenom].beats);
	return;
    }
    if (obj->item_type == ITEM_WEAPON) {
	if (IS_WEAPON_STAT(obj, WEAPON_FLAMING)
	    || IS_WEAPON_STAT(obj, WEAPON_FROST)
	    || IS_WEAPON_STAT(obj, WEAPON_VAMPIRIC)
	    || IS_WEAPON_STAT(obj, WEAPON_SHARP)
	    || IS_WEAPON_STAT(obj, WEAPON_VORPAL)
	    || IS_WEAPON_STAT(obj, WEAPON_SHOCKING)
	    || IS_WEAPON_STAT(obj, WEAPON_HOLY)
	    || IS_OBJ_STAT(obj, ITEM_BLESS)
	    || IS_OBJ_STAT(obj, ITEM_BURN_PROOF)) {
	    act("You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	if (obj->value[3] < 0
	    || attack_table[obj->value[3]].damage == DAM_BASH) {
	    send_to_char("You can only envenom edged weapons.\n\r", ch);
	    return;
	}
	if (IS_WEAPON_STAT(obj, WEAPON_POISON)) {
	    act("$p is already envenomed.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	percent = number_percent();
	if (percent < skill) {

	    af.where = TO_WEAPON;
	    af.type = gsn_poison;
	    af.level = ch->level * percent / 100;
	    af.duration = ch->level / 2 * percent / 100;
	    af.location = 0;
	    af.modifier = 0;
	    af.bitvector = WEAPON_POISON;
	    affect_to_obj(obj, &af);

	    act("$n coats $p with deadly venom.", ch, obj, NULL, TO_ROOM);
	    act("You coat $p with venom.", ch, obj, NULL, TO_CHAR);
	    check_improve(ch, gsn_envenom, TRUE, 3);
	    WAIT_STATE(ch, skill_table[gsn_envenom].beats);
	    return;
	} else {
	    act("You fail to envenom $p.", ch, obj, NULL, TO_CHAR);
	    check_improve(ch, gsn_envenom, FALSE, 3);
	    WAIT_STATE(ch, skill_table[gsn_envenom].beats);
	    return;
	}
    }
    act("You can't poison $p.", ch, obj, NULL, TO_CHAR);
    return;
}

void do_fill(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Fill what?\n\r", ch);
	return;
    }
    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    found = FALSE;
    for (fountain = ch->in_room->contents; fountain != NULL;
	 fountain = fountain->next_content) {
	if (fountain->item_type == ITEM_FOUNTAIN) {
	    found = TRUE;
	    break;
	}
    }

    if (!found) {
	send_to_char("There is no fountain here!\n\r", ch);
	return;
    }
    if (!strcmp(liq_table[fountain->value[2]].liq_name, "blood")
	&& (obj->item_type != ITEM_BLOOD_CON)) {
	send_to_char("You may only fill a blood flask here.", ch);
	return;
    }
    if ((obj->item_type != ITEM_DRINK_CON)
	&& (obj->item_type != ITEM_BLOOD_CON)) {
	send_to_char("This is not a drink container.\n\r", ch);
	return;
    }
    if (obj->value[1] != 0 && obj->value[2] != fountain->value[2]) {
	send_to_char("There is already another liquid in it.\n\r", ch);
	return;
    }
    if (obj->value[1] >= obj->value[0]) {
	send_to_char("Your container is full.\n\r", ch);
	return;
    }
    sprintf(buf, "You fill $p with %s from $P.",
	    liq_table[fountain->value[2]].liq_name);
    act(buf, ch, obj, fountain, TO_CHAR);
    sprintf(buf, "$n fills $p with %s from $P.",
	    liq_table[fountain->value[2]].liq_name);
    act(buf, ch, obj, fountain, TO_ROOM);
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    return;
}

void do_pour(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
	send_to_char("Pour what into what?\n\r", ch);
	return;
    }
    if ((out = get_obj_carry(ch, arg, ch)) == NULL) {
	send_to_char("You don't have that item.\n\r", ch);
	return;
    }
    if ((out->item_type != ITEM_DRINK_CON)
	&& (out->item_type != ITEM_BLOOD_CON)) {
	send_to_char("That's not a drink container.\n\r", ch);
	return;
    }
    if (!str_cmp(argument, "out")) {
	if (out->value[1] == 0) {
	    send_to_char("It's already empty.\n\r", ch);
	    return;
	}
	out->value[1] = 0;
	out->value[3] = 0;
	sprintf(buf, "You invert $p, spilling %s all over the ground.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, out, NULL, TO_CHAR);

	sprintf(buf, "$n inverts $p, spilling %s all over the ground.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, out, NULL, TO_ROOM);
	return;
    }
    if ((in = get_obj_here(ch, argument)) == NULL) {
	vch = get_char_room(ch, argument);

	if (vch == NULL) {
	    send_to_char("Pour into what?\n\r", ch);
	    return;
	}
	in = get_eq_char(vch, WEAR_HOLD);

	if (in == NULL) {
	    send_to_char("They aren't holding anything.", ch);
	    return;
	}
    }
    if ((in->item_type != ITEM_DRINK_CON)
	&& (in->item_type != ITEM_BLOOD_CON)) {
	send_to_char("You can only pour into other drink containers.\n\r",
		     ch);
	return;
    }
    if ((out->item_type == ITEM_BLOOD_CON)
	|| (in->item_type != ITEM_BLOOD_CON)) {
	send_to_char
	    ("You can only transfer blood from one flask to another.\n\r",
	     ch);
	return;
    }
    if (in == out) {
	send_to_char("You cannot change the laws of physics!\n\r", ch);
	return;
    }
    if (in->value[1] != 0 && in->value[2] != out->value[2]) {
	send_to_char("They don't hold the same liquid.\n\r", ch);
	return;
    }
    if (out->value[1] == 0) {
	act("There's nothing in $p to pour.", ch, out, NULL, TO_CHAR);
	return;
    }
    if (in->value[1] >= in->value[0]) {
	act("$p is already filled to the top.", ch, in, NULL, TO_CHAR);
	return;
    }
    amount = UMIN(out->value[1], in->value[0] - in->value[1]);

    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];

    if (vch == NULL) {
	sprintf(buf, "You pour %s from $p into $P.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, out, in, TO_CHAR);
	sprintf(buf, "$n pours %s from $p into $P.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, out, in, TO_ROOM);
    } else {
	sprintf(buf, "You pour some %s for $N.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, NULL, vch, TO_CHAR);
	sprintf(buf, "$n pours you some %s.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, NULL, vch, TO_VICT);
	sprintf(buf, "$n pours some %s for $N.",
		liq_table[out->value[2]].liq_name);
	act(buf, ch, NULL, vch, TO_NOTVICT);

    }
}

void do_drink(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
    	for (obj = ch->in_room->contents; obj; obj = obj->next_content) {
    	    if (obj->item_type == ITEM_FOUNTAIN)
    		break;
    	}
    
    	if (obj == NULL) {
    	    send_to_char("Drink what?\n\r", ch);
    	    return;
    	}
    } else {
    	if ((obj = get_obj_here(ch, arg)) == NULL) {
    	    send_to_char("You can't find it.\n\r", ch);
    	    return;
    	}
   }
  switch (obj->item_type) {
    default:
      send_to_char("You can't drink from that.\n\r", ch);
    return;
    
      case ITEM_FOUNTAIN:
        if ((liquid = obj->value[2]) < 0) {
          bug("Do_drink: bad liquid number %d.", liquid);
          liquid = obj->value[2] = 0;
        }
        amount = liq_table[liquid].liq_affect[4] * 3;
      break;
    
        case ITEM_DRINK_CON:
    	if (obj->value[1] <= 0) {
    	    send_to_char("It is already empty.\n\r", ch);
    	    return;
    	}
    	if ((liquid = obj->value[2]) < 0) {
    	    bug("Do_drink: bad liquid number %d.", liquid);
    	    liquid = obj->value[2] = 0;
    	}
    	amount = liq_table[liquid].liq_affect[4];
    	amount = UMIN(amount, obj->value[1]);
      //printf("act.obj.c: amount = %d\r\n", amount);
    	break;
    
        case ITEM_BLOOD_CON:
    	if (!IS_VAMPIRE(ch)) {
    	    send_to_char("Only vampires may drink from blood flasks.\n\r",
    			 ch);
    	    return;
    	} else {
    	    if (obj->value[1] <= 0) {
    		send_to_char("It is already empty.\n\r", ch);
    		return;
    	    }
    	    if ((liquid = obj->value[2]) < 0) {
    		bug("Do_drink: bad liquid number %d.", liquid);
    		liquid = obj->value[2] = 0;
    	    }
    	    amount = liq_table[liquid].liq_affect[4];
    	    amount = UMIN(amount, obj->value[1]);
    	    break;
    	}
    
        case ITEM_BLOOD_POOL:
    	if (!IS_VAMPIRE(ch)) {
    	    send_to_char("Only vampires can drink blood pools.\n\r", ch);
    	    return;
    	} else {
    	    if (ch->position == POS_FIGHTING) {
    		send_to_char
    		    ("You don't have time to drink from the pool!\n\r",
    		     ch);
    		return;
    	    } else {
    		if (strstr(obj->short_descr, ch->name) != NULL) {
    		    send_to_char
    			("You can't drink from your own blood!\n\r", ch);
    		    return;
    		}
    		act("$n drinks from $p.", ch, obj, NULL, TO_ROOM);
    		act("You drink from $p.", ch, obj, NULL, TO_CHAR);
    		ch->blood += 1;
    		if (ch->blood > ch->max_blood)
    		    ch->blood = ch->max_blood;
    		extract_obj(obj);
    	    }
    	}
	return;
    }

    act("$n drinks $T from $p.",
	ch, obj, liq_table[liquid].liq_name, TO_ROOM);
    act("You drink $T from $p.",
	ch, obj, liq_table[liquid].liq_name, TO_CHAR);

    gain_condition(ch, COND_DRUNK,
		   liq_table[liquid].liq_affect[COND_DRUNK]);
    gain_condition(ch, COND_THIRST,
		   amount * liq_table[liquid].liq_affect[COND_THIRST]);
    gain_condition(ch, COND_HUNGER,
		   amount * liq_table[liquid].liq_affect[COND_HUNGER]);

    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	send_to_char("You feel drunk.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 20)
	send_to_char("You are full.\n\r", ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 20)
	send_to_char("Your thirst is quenched.\n\r", ch);

    if (obj->value[3] != 0) {
	/* The drink was poisoned ! */
	AFFECT_DATA af;

	act("$n chokes and gags.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You choke and gag.\n\r", ch);
	af.where = TO_AFFECTS;
	af.type = gsn_poison;
	af.level = number_fuzzy(amount);
	af.duration = 3 * amount;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_POISON;
	affect_join(ch, &af);
    }
    if (IS_VAMPIRE(ch) && !str_cmp(liq_table[liquid].liq_name, "blood")) {
	ch->blood += 5;
	if (ch->blood > ch->max_blood)
	    ch->blood = ch->max_blood;
    }
    if (obj->value[0] > 0)
	obj->value[1] -= amount;

    return;
}

void do_feed(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];

    one_argument(argument, arg);

    if (ch->position == POS_FIGHTING) {
	send_to_char("You don't have time to feed from the corpse!\n\r",
		     ch);
	return;
    }
    if ((!IS_VAMPIRE(ch)) && (ch->level <= LEVEL_IMMORTAL)) {
	send_to_char("Only vampires may feed from corpses.\r\n", ch);
	return;
    }
    if (arg[0] == '\0') {
	for (obj = ch->in_room->contents; obj; obj = obj->next_content) {
	    if (obj->item_type == ITEM_CORPSE_PC || ITEM_CORPSE_NPC)
		break;
	}

	if (obj == NULL) {
	    send_to_char("Feed from what?\n\r", ch);
	    return;
	}
    } else {
	if ((obj = get_obj_here(ch, arg)) == NULL) {
	    send_to_char("You can't find it.\n\r", ch);
	    return;
	}
    }

    switch (obj->item_type) {
    default:
	send_to_char("You can't feed from that.\n\r", ch);
	return;

    case ITEM_CORPSE_PC:
	if (IS_SET(obj->extra_flags, ITEM_BLOODLESS)) {
	    send_to_char("The corpse has no blood left to drink.\n\r", ch);
	    return;
	} else {
	    SET_BIT(obj->extra_flags, ITEM_BLOODLESS);
	    sprintf(buf, "The bloodless corpse of %s is laying here.",
		    obj->owner);
	    free_string(obj->description);
	    obj->description = str_dup(buf);
	    act("$n feeds from $p.", ch, obj, NULL, TO_ROOM);
	    act("You feed from $p.", ch, obj, NULL, TO_CHAR);
	    ch->blood += (obj->level / 2);
	    if (ch->blood > ch->max_blood)
		ch->blood = ch->max_blood;
	}
	break;
    case ITEM_CORPSE_NPC:
	if (IS_SET(obj->extra_flags, ITEM_BLOODLESS)) {
	    send_to_char("The corpse has no blood left to drink.\n\r", ch);
	    return;
	} else {
	    SET_BIT(obj->extra_flags, ITEM_BLOODLESS);
	    act("$n feeds from $p.", ch, obj, NULL, TO_ROOM);
	    act("You feed from $p.", ch, obj, NULL, TO_CHAR);
	    ch->blood += (obj->level / 2);
	    if (ch->blood > ch->max_blood)
		ch->blood = ch->max_blood;
	    act("$p decomposes.", ch, obj, NULL, TO_ROOM);
	    act("$p decomposes.", ch, obj, NULL, TO_CHAR);
	    obj_from_room(obj);
	}
	break;

	return;
    }
}


void do_eat(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
	send_to_char("Eat what?\n\r", ch);
	return;
    }
    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    if (obj->level > ch->level) {
	send_to_char("You are not high enough level to eat that.", ch);
	return;
    }
    if (!IS_IMMORTAL(ch)) {
	if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL) {
	    send_to_char("That's not edible.\n\r", ch);
	    return;
	}
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 72) {
	    send_to_char("You are too full to eat more.\n\r", ch);
	    return;
	}
    }
    act("$n eats $p.", ch, obj, NULL, TO_ROOM);
    act("You eat $p.", ch, obj, NULL, TO_CHAR);

    switch (obj->item_type) {

    case ITEM_FOOD:
	if (!IS_NPC(ch)) {

	    gain_condition(ch, COND_HUNGER, obj->value[0]);
	    if (ch->pcdata->condition[COND_HUNGER] > 71)
		send_to_char("You are full.\n\r", ch);
	    else if (ch->pcdata->condition[COND_HUNGER] > 60)
		send_to_char("You are no longer hungry.\n\r", ch);
	}
	if ((obj->value[3] != 0) && (!IS_SET(ch->imm_flags, IMM_POISON))) {
	    /* The food was poisoned! */
	    AFFECT_DATA af;

	    act("$n chokes and gags.", ch, 0, 0, TO_ROOM);
	    send_to_char("You choke and gag.\n\r", ch);

	    af.where = TO_AFFECTS;
	    af.type = gsn_poison;
	    af.level = number_fuzzy(obj->value[0]);
	    af.duration = 2 * obj->value[0];
	    af.location = APPLY_NONE;
	    af.modifier = 0;
	    af.bitvector = AFF_POISON;
	    affect_join(ch, &af);
	}
	break;

    case ITEM_PILL:
	obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
	obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
	obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);
	obj_cast_spell(obj->value[4], obj->value[0], ch, ch, NULL);
	WAIT_STATE(ch, 10);
	break;
    }

    extract_obj(obj);
    return;
}



/*
 * Remove an object.
 */
bool remove_obj(CHAR_DATA * ch, int iWear, bool fReplace)
{
    OBJ_DATA *obj;

    if ((obj = get_eq_char(ch, iWear)) == NULL)
	return TRUE;

    if (!fReplace)
	return FALSE;

    if (IS_SET(obj->extra_flags, ITEM_NOREMOVE)) {
	act("You can't remove $p.", ch, obj, NULL, TO_CHAR);
	return FALSE;
    }
    unequip_char(ch, obj);
    act("$n stops using $p.", ch, obj, NULL, TO_ROOM);
    act("You stop using $p.", ch, obj, NULL, TO_CHAR);
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj(CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace)
{
    char buf[MAX_STRING_LENGTH];

    if (ch->level < obj->level) {
	sprintf(buf, "You must be level %d to use this object.\n\r",
		obj->level);
	send_to_char(buf, ch);
	act("$n tries to use $p, but is too inexperienced.",
	    ch, obj, NULL, TO_ROOM);
	return;
    }
    if (obj->item_type == ITEM_LIGHT) {
	if (!remove_obj(ch, WEAR_LIGHT, fReplace))
	    return;
	act("$n lights $p and holds it.", ch, obj, NULL, TO_ROOM);
	act("You light $p and hold it.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_LIGHT);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_FINGER)) {
	if (get_eq_char(ch, WEAR_FINGER_L) != NULL
	    && get_eq_char(ch, WEAR_FINGER_R) != NULL
	    && !remove_obj(ch, WEAR_FINGER_L, fReplace)
	    && !remove_obj(ch, WEAR_FINGER_R, fReplace))
	    return;

	if (get_eq_char(ch, WEAR_FINGER_L) == NULL) {
	    act("$n wears $p on $s left finger.", ch, obj, NULL, TO_ROOM);
	    act("You wear $p on your left finger.", ch, obj, NULL,
		TO_CHAR);
	    equip_char(ch, obj, WEAR_FINGER_L);
	    return;
	}
	if (get_eq_char(ch, WEAR_FINGER_R) == NULL) {
	    act("$n wears $p on $s right finger.", ch, obj, NULL, TO_ROOM);
	    act("You wear $p on your right finger.", ch, obj, NULL,
		TO_CHAR);
	    equip_char(ch, obj, WEAR_FINGER_R);
	    return;
	}
	bug("Wear_obj: no free finger.", 0);
	send_to_char("You already wear two rings.\n\r", ch);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_NECK)) {
	if (get_eq_char(ch, WEAR_NECK_1) != NULL
	    && get_eq_char(ch, WEAR_NECK_2) != NULL
	    && !remove_obj(ch, WEAR_NECK_1, fReplace)
	    && !remove_obj(ch, WEAR_NECK_2, fReplace))
	    return;

	if (get_eq_char(ch, WEAR_NECK_1) == NULL) {
	    act("$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM);
	    act("You wear $p around your neck.", ch, obj, NULL, TO_CHAR);
	    equip_char(ch, obj, WEAR_NECK_1);
	    return;
	}
	if (get_eq_char(ch, WEAR_NECK_2) == NULL) {
	    act("$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM);
	    act("You wear $p around your neck.", ch, obj, NULL, TO_CHAR);
	    equip_char(ch, obj, WEAR_NECK_2);
	    return;
	}
	bug("Wear_obj: no free neck.", 0);
	send_to_char("You already wear two neck items.\n\r", ch);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_BODY)) {
	if (!remove_obj(ch, WEAR_BODY, fReplace))
	    return;
	act("$n wears $p on $s body.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your body.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_BODY);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_HEAD)) {
	if (!remove_obj(ch, WEAR_HEAD, fReplace))
	    return;
	act("$n wears $p on $s head.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your head.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_HEAD);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_LEGS)) {
	if (!remove_obj(ch, WEAR_LEGS, fReplace))
	    return;
	act("$n wears $p on $s legs.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your legs.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_LEGS);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_FEET)) {
	if (!remove_obj(ch, WEAR_FEET, fReplace))
	    return;
	act("$n wears $p on $s feet.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your feet.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_FEET);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_HANDS)) {
	if (!remove_obj(ch, WEAR_HANDS, fReplace))
	    return;
	act("$n wears $p on $s hands.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your hands.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_HANDS);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_ARMS)) {
	if (!remove_obj(ch, WEAR_ARMS, fReplace))
	    return;
	act("$n wears $p on $s arms.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your arms.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_ARMS);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_ABOUT)) {
	if (!remove_obj(ch, WEAR_ABOUT, fReplace))
	    return;
	act("$n wears $p about $s torso.", ch, obj, NULL, TO_ROOM);
	act("You wear $p about your torso.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_ABOUT);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_WAIST)) {
	if (!remove_obj(ch, WEAR_WAIST, fReplace))
	    return;
	act("$n wears $p about $s waist.", ch, obj, NULL, TO_ROOM);
	act("You wear $p about your waist.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_WAIST);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_WRIST)) {
	if (get_eq_char(ch, WEAR_WRIST_L) != NULL
	    && get_eq_char(ch, WEAR_WRIST_R) != NULL
	    && !remove_obj(ch, WEAR_WRIST_L, fReplace)
	    && !remove_obj(ch, WEAR_WRIST_R, fReplace))
	    return;

	if (get_eq_char(ch, WEAR_WRIST_L) == NULL) {
	    act("$n wears $p around $s left wrist.",
		ch, obj, NULL, TO_ROOM);
	    act("You wear $p around your left wrist.",
		ch, obj, NULL, TO_CHAR);
	    equip_char(ch, obj, WEAR_WRIST_L);
	    return;
	}
	if (get_eq_char(ch, WEAR_WRIST_R) == NULL) {
	    act("$n wears $p around $s right wrist.",
		ch, obj, NULL, TO_ROOM);
	    act("You wear $p around your right wrist.",
		ch, obj, NULL, TO_CHAR);
	    equip_char(ch, obj, WEAR_WRIST_R);
	    return;
	}
	bug("Wear_obj: no free wrist.", 0);
	send_to_char("You already wear two wrist items.\n\r", ch);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_SHIELD)) {
	OBJ_DATA *weapon;

	if (!remove_obj(ch, WEAR_SHIELD, fReplace))
	    return;

	weapon = get_eq_char(ch, WEAR_WIELD);
	if (weapon != NULL && ch->size < SIZE_LARGE
	    && IS_WEAPON_STAT(weapon, WEAPON_TWO_HANDS)) {
	    send_to_char("Your hands are tied up with your weapon!\n\r",
			 ch);
	    return;
	}
	act("$n wears $p as a shield.", ch, obj, NULL, TO_ROOM);
	act("You wear $p as a shield.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_SHIELD);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WIELD)) {
	int sn, skill;

	if (!remove_obj(ch, WEAR_WIELD, fReplace))
	    return;

	if (!IS_NPC(ch)
	    && get_obj_weight(obj) >
	    (str_app[get_curr_stat(ch, STAT_STR)].wield * 10)) {
	    send_to_char("It is too heavy for you to wield.\n\r", ch);
	    return;
	}
	if (!IS_NPC(ch) && ch->size < SIZE_LARGE
	    && IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS)
	    && get_eq_char(ch, WEAR_SHIELD) != NULL) {
	    send_to_char("You need two hands free for that weapon.\n\r",
			 ch);
	    return;
	}
	act("$n wields $p.", ch, obj, NULL, TO_ROOM);
	act("You wield $p.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_WIELD);

	sn = get_weapon_sn(ch);

	if (sn == gsn_hand_to_hand)
	    return;

	skill = get_weapon_skill(ch, sn);

	if (skill >= 100)
	    act("$p feels like a part of you!", ch, obj, NULL, TO_CHAR);
	else if (skill > 85)
	    act("You feel quite confident with $p.", ch, obj, NULL,
		TO_CHAR);
	else if (skill > 70)
	    act("You are skilled with $p.", ch, obj, NULL, TO_CHAR);
	else if (skill > 50)
	    act("Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR);
	else if (skill > 25)
	    act("$p feels a little clumsy in your hands.", ch, obj, NULL,
		TO_CHAR);
	else if (skill > 1)
	    act("You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR);
	else
	    act("You don't even know which end is up on $p.",
		ch, obj, NULL, TO_CHAR);

	return;
    }
    if (CAN_WEAR(obj, ITEM_HOLD)) {
	if (!remove_obj(ch, WEAR_HOLD, fReplace))
	    return;
	act("$n holds $p in $s hand.", ch, obj, NULL, TO_ROOM);
	act("You hold $p in your hand.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_HOLD);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_FLOAT)) {
	if (!remove_obj(ch, WEAR_FLOAT, fReplace))
	    return;
	act("$n releases $p to float next to $m.", ch, obj, NULL, TO_ROOM);
	act("You release $p and it floats next to you.", ch, obj, NULL,
	    TO_CHAR);
	equip_char(ch, obj, WEAR_FLOAT);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_FACE)) {
	if (!remove_obj(ch, WEAR_FACE, fReplace))
	    return;
	act("$n puts $p on $s face.", ch, obj, NULL, TO_ROOM);
	act("You put $p on your face.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_FACE);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_EAR)) {
	if (get_eq_char(ch, WEAR_EAR_L) != NULL
	    && get_eq_char(ch, WEAR_EAR_R) != NULL
	    && !remove_obj(ch, WEAR_EAR_L, fReplace)
	    && !remove_obj(ch, WEAR_EAR_R, fReplace))
	    return;

	if (get_eq_char(ch, WEAR_EAR_L) == NULL) {
	    act("$n wears $p in $s left ear.", ch, obj, NULL, TO_ROOM);
	    act("You wear $p in your left ear.", ch, obj, NULL, TO_CHAR);
	    equip_char(ch, obj, WEAR_EAR_L);
	    return;
	}
	if (get_eq_char(ch, WEAR_EAR_R) == NULL) {
	    act("$n wears $p in $s right ear.", ch, obj, NULL, TO_ROOM);
	    act("You wear $p in your right ear.", ch, obj, NULL, TO_CHAR);
	    equip_char(ch, obj, WEAR_EAR_R);
	    return;
	}
	bug("Wear_obj: no free ear.", 0);
	send_to_char("You already wear two ear items.\n\r", ch);
	return;
    }
    if (CAN_WEAR(obj, ITEM_WEAR_SHOULDERS)) {
	if (!remove_obj(ch, WEAR_SHOULDERS, fReplace))
	    return;
	act("$n wears $p on $s shoulders.", ch, obj, NULL, TO_ROOM);
	act("You wear $p on your shoulders.", ch, obj, NULL, TO_CHAR);
	equip_char(ch, obj, WEAR_SHOULDERS);
	return;
    }
    if (fReplace)
	send_to_char("You can't wear, wield, or hold that.\n\r", ch);

    return;
}



void do_wear(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Wear, wield, or hold what?\n\r", ch);
	return;
    }
    if (!str_cmp(arg, "all")) {
	OBJ_DATA *obj_next;

	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;
	    if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
		wear_obj(ch, obj, FALSE);
	}
	return;
    } else {
	if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	    send_to_char("You do not have that item.\n\r", ch);
	    return;
	}
	wear_obj(ch, obj, TRUE);
    }

    return;
}



void do_remove(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Remove what?\n\r", ch);
	return;
    }
    if (!str_cmp(arg, "all")) {
	OBJ_DATA *obj_next;

	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

	    if (obj->wear_loc == WEAR_NONE || !can_see_obj(ch, obj))
		continue;

	    remove_obj(ch, obj->wear_loc, TRUE);
	}
	return;
    }
    if ((obj = get_obj_wear(ch, arg)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    remove_obj(ch, obj->wear_loc, TRUE);
    return;
}



void do_sacrifice(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int gold = 0;

    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];

    one_argument(argument, arg);

    if (arg[0] == '\0' || !str_cmp(arg, ch->name)) {
	act("$n offers $mself to Y'golan, who graciously declines.",
	    ch, NULL, NULL, TO_ROOM);
	send_to_char
	    ("Y'golan appreciates your offer and may accept it later.\n\r",
	     ch);

	return;
    }
    if (!str_cmp(arg, "all")) {
	OBJ_DATA *obj_next;
	int total_gold = 0;
	bool found = FALSE;

	if (!ch->in_room->contents) {
	    send_to_char("There's nothing in the room you can sacrifice.\n\r",
			 ch);

	    return;
	}

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

		if( !can_see_obj(ch, obj) )
			continue;			

	    if ((obj->item_type == ITEM_CORPSE_PC && obj->contains)
		|| !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC))
			continue;

	    if (obj->in_room != NULL) {
		for (gch = obj->in_room->people; gch != NULL;
		     gch = gch->next_in_room) if (gch->on == obj)
			continue;
	    }
	    gold = UMAX(1, obj->level * 3);
		found = TRUE;

	    if (obj->item_type != ITEM_CORPSE_NPC
		&& obj->item_type != ITEM_CORPSE_PC) gold =
		    UMIN(gold, obj->cost);

	    total_gold += gold;

	    extract_obj(obj);
	}

	if (total_gold >= 2) {
	    sprintf(buf,
		    "Y'golan gives you %d gold coins for your sacrifice.\n\r",
		    total_gold);
	    send_to_char(buf, ch);
	} else if (total_gold >= 1)
	    send_to_char
		("Y'golan gives you one gold coin for your sacrifice.\n\r",
		 ch);
	else
		if (found) 
	    	send_to_char("Y'golan gives you nothing for your sacrifice.\n\r", ch);

	ch->gold += total_gold;

	if (IS_SET(ch->act, PLR_AUTOSPLIT)) {
	    members = 0;
	    for (gch = ch->in_room->people; gch != NULL;
		 gch = gch->next_in_room) {
		if (is_same_group(gch, ch))
		    members++;
	    }

	    if (members > 1 && total_gold > 1) {
		sprintf(buffer, "%d", total_gold);
		do_split(ch, buffer);
	    }
	}
	if (found) {
		act("$n sacrifices everything in the room to Y'golan.", ch, obj,
	    	NULL, TO_ROOM);
		return;
	}
	
    send_to_char("There's nothing in the room you can sacrifice.\n\r", ch);
	return;
    }

    obj = get_obj_list(ch, arg, ch->in_room->contents);
    if (obj == NULL) {
	send_to_char("You can't find it.\n\r", ch);
	return;
    }
    if (obj->item_type == ITEM_CORPSE_PC) {
	if (obj->contains) {
	    send_to_char("Y'golan wouldn't like that.\n\r", ch);
	    return;
	}
    }
    if (!CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC)) {
	act("$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR);
	return;
    }
    if (obj->in_room != NULL) {
	for (gch = obj->in_room->people; gch != NULL;
	     gch = gch->next_in_room) if (gch->on == obj) {
		act("$N appears to be using $p.", ch, obj, gch, TO_CHAR);
		return;
	    }
    }
    gold = UMAX(1, obj->level * 3);

    if (obj->item_type != ITEM_CORPSE_NPC
	&& obj->item_type != ITEM_CORPSE_PC) gold = UMIN(gold, obj->cost);

    if (gold == 1)
	send_to_char
	    ("Y'golan gives you one measly gold coin for your sacrifice.\n\r",
	     ch);
    else if (gold == 0)
	send_to_char
	    ("Y'golan is insulted by your pathetic sacrifice, and doesn't give you anything!\n\r",
	     ch);
    else {
	sprintf(buf,
		"Y'golan gives you %d gold coins for your sacrifice.\n\r",
		gold);
	send_to_char(buf, ch);
    }

    ch->gold += gold;

    if (IS_SET(ch->act, PLR_AUTOSPLIT)) {	/* AUTOSPLIT code */
	members = 0;
	for (gch = ch->in_room->people; gch != NULL;
	     gch = gch->next_in_room) {
	    if (is_same_group(gch, ch))
		members++;
	}

	if (members > 1 && gold > 1) {
	    sprintf(buffer, "%d", gold);
	    do_split(ch, buffer);
	}
    }
    act("$n sacrifices $p to Y'golan.", ch, obj, NULL, TO_ROOM);
    wiznet("$N sends up $p as a burnt offering.", ch, obj, WIZ_SACCING, 0,
	   0);
    extract_obj(obj);
    return;
}



void do_quaff(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Quaff what?\n\r", ch);
	return;
    }
    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	send_to_char("You do not have that potion.\n\r", ch);
	return;
    }
    if (obj->item_type != ITEM_POTION) {
	send_to_char("You can quaff only potions.\n\r", ch);
	return;
    }
    if (ch->level < obj->level) {
	send_to_char("This liquid is too powerful for you to drink.\n\r",
		     ch);
	return;
    }
    act("$n quaffs $p.", ch, obj, NULL, TO_ROOM);
    act("You quaff $p.", ch, obj, NULL, TO_CHAR);

    WAIT_STATE(ch, 10);

    obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
    obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
    obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);

    extract_obj(obj);
    return;
}



void do_recite(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if ((scroll = get_obj_carry(ch, arg1, ch)) == NULL) {
	send_to_char("You do not have that scroll.\n\r", ch);
	return;
    }
    if (scroll->item_type != ITEM_SCROLL) {
	send_to_char("You can recite only scrolls.\n\r", ch);
	return;
    }
    if (ch->level < scroll->level) {
	send_to_char
	    ("This scroll is too complex for you to comprehend.\n\r", ch);
	return;
    }
    obj = NULL;
    if (arg2[0] == '\0') {
	victim = ch;
    } else {
	if ((victim = get_char_room(ch, arg2)) == NULL
	    && (obj = get_obj_here(ch, arg2)) == NULL) {
	    send_to_char("You can't find it.\n\r", ch);
	    return;
	}
    }

    act("$n recites $p.", ch, scroll, NULL, TO_ROOM);
    act("You recite $p.", ch, scroll, NULL, TO_CHAR);

    WAIT_STATE(ch, 10);

    if (number_percent() >= 20 + get_skill(ch, gsn_scrolls) * 4 / 5) {
	send_to_char("You mispronounce a syllable.\n\r", ch);
	check_improve(ch, gsn_scrolls, FALSE, 2);
    } else {
	obj_cast_spell(scroll->value[1], scroll->value[0], ch, victim,
		       obj);
	obj_cast_spell(scroll->value[2], scroll->value[0], ch, victim,
		       obj);
	obj_cast_spell(scroll->value[3], scroll->value[0], ch, victim,
		       obj);
	obj_cast_spell(scroll->value[4], scroll->value[0], ch, victim,
		       obj);
	check_improve(ch, gsn_scrolls, TRUE, 2);
    }

    extract_obj(scroll);
    return;
}



void do_brandish(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    int sn;

    if ((staff = get_eq_char(ch, WEAR_HOLD)) == NULL) {
	send_to_char("You hold nothing in your hand.\n\r", ch);
	return;
    }
    if (staff->item_type != ITEM_STAFF) {
	send_to_char("You can brandish only with a staff.\n\r", ch);
	return;
    }
    if ((sn = staff->value[3]) < 0
	|| sn >= MAX_SKILL || skill_table[sn].spell_fun == 0) {
	bug("Do_brandish: bad sn %d.", sn);
	return;
    }
    WAIT_STATE(ch, 28);

    if (staff->value[2] > 0) {
	act("$n brandishes $p.", ch, staff, NULL, TO_ROOM);
	act("You brandish $p.", ch, staff, NULL, TO_CHAR);
	if (ch->level < staff->level
	    || number_percent() >= 20 + get_skill(ch, gsn_staves) * 4 / 5) {
	    act("You fail to invoke $p.", ch, staff, NULL, TO_CHAR);
	    act("...and nothing happens.", ch, NULL, NULL, TO_ROOM);
	    check_improve(ch, gsn_staves, FALSE, 2);
	} else
	    for (vch = ch->in_room->people; vch; vch = vch_next) {
		vch_next = vch->next_in_room;

		switch (skill_table[sn].target) {
		default:
		    bug("Do_brandish: bad target for sn %d.", sn);
		    return;

		case TAR_IGNORE:
		    if (vch != ch)
			continue;
		    break;

		case TAR_CHAR_OFFENSIVE:
		    if (IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch))
			continue;
		    break;

		case TAR_CHAR_DEFENSIVE:
		    if (IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch))
			continue;
		    break;

		case TAR_CHAR_SELF:
		    if (vch != ch)
			continue;
		    break;
		}

		obj_cast_spell(staff->value[3], staff->value[0], ch, vch,
			       NULL);
		check_improve(ch, gsn_staves, TRUE, 2);
	    }
    }
    if (--staff->value[2] <= 0) {
	act("$n's $p blazes bright and is gone.", ch, staff, NULL,
	    TO_ROOM);
	act("Your $p blazes bright and is gone.", ch, staff, NULL,
	    TO_CHAR);
	extract_obj(staff);
    }
    return;
}



void do_zap(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;

    one_argument(argument, arg);
    if (arg[0] == '\0' && ch->fighting == NULL) {
	send_to_char("Zap whom or what?\n\r", ch);
	return;
    }
    if ((wand = get_eq_char(ch, WEAR_HOLD)) == NULL) {
	send_to_char("You hold nothing in your hand.\n\r", ch);
	return;
    }
    if (wand->item_type != ITEM_WAND) {
	send_to_char("You can zap only with a wand.\n\r", ch);
	return;
    }
    obj = NULL;
    if (arg[0] == '\0') {
	if (ch->fighting != NULL) {
	    victim = ch->fighting;
	} else {
	    send_to_char("Zap whom or what?\n\r", ch);
	    return;
	}
    } else {
	if ((victim = get_char_room(ch, arg)) == NULL
	    && (obj = get_obj_here(ch, arg)) == NULL) {
	    send_to_char("You can't find it.\n\r", ch);
	    return;
	}
    }

    WAIT_STATE(ch, 28);

    if (wand->value[2] > 0) {
	if (victim != NULL) {
	    act("$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT);
	    act("You zap $N with $p.", ch, wand, victim, TO_CHAR);
	    act("$n zaps you with $p.", ch, wand, victim, TO_VICT);
	} else {
	    act("$n zaps $P with $p.", ch, wand, obj, TO_ROOM);
	    act("You zap $P with $p.", ch, wand, obj, TO_CHAR);
	}

	if (ch->level < wand->level
	    || number_percent() >= 20 + get_skill(ch, gsn_wands) * 4 / 5) {
	    act("Your efforts with $p produce only smoke and sparks.",
		ch, wand, NULL, TO_CHAR);
	    act("$n's efforts with $p produce only smoke and sparks.",
		ch, wand, NULL, TO_ROOM);
	    check_improve(ch, gsn_wands, FALSE, 2);
	} else {
	    obj_cast_spell(wand->value[3], wand->value[0], ch, victim,
			   obj);
	    check_improve(ch, gsn_wands, TRUE, 2);
	}
    }
    if (--wand->value[2] <= 0) {
	act("$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM);
	act("Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR);
	extract_obj(wand);
    }
    return;
}



void do_steal(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

		bool displaced = FALSE;//displace spell

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
	 {
	send_to_char("Steal what from whom?\n\r", ch);
	return;
	}
    if ((victim = get_char_room(ch, arg2)) == NULL)
	 {
	send_to_char("They aren't here.\n\r", ch);
	return;
	}
    if (victim == ch)
	 {
	send_to_char("That's pointless.\n\r", ch);
	return;
	}
    if (is_safe(ch, victim)) {
	send_to_char("you can't steal on safe places.\n\r", ch);
	return;
    }

		if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }

    if (IS_NPC(victim)
	&& victim->position == POS_FIGHTING)
	 {
	send_to_char("Kill stealing is not permitted.\n\r"
		     "You'd better not -- you might get hit.\n\r", ch);
	return;
	}

		//Check if displaced
		if(is_affected(victim, skill_lookup("displace")))
		{
			if(displace_new_effect(ch, (void *) victim, 0, 0, 0))
			{
				//You're stealing from the displaced version
				send_to_char("You work your hands to the purse... and go right through it...\n\r", ch);
				displaced = TRUE;
			}
			else //Picked the right version
				displaced = FALSE;
		}
		else //Victim isn't affected by displace spell
			displaced = FALSE;

    check_killer(ch, victim);

    WAIT_STATE(ch, skill_table[gsn_steal].beats);
    percent = number_percent();

    if (!IS_AWAKE(victim))
	percent -= 15;
    else if (!can_see(victim, ch))
	percent += 20;
    else
	percent += 40;


    if (
	((!IS_NPC(victim) && (!IS_PKILL(victim) || !IS_PKILL(ch)))
	 && !IS_NPC(ch)) || (!IS_NPC(ch)
			     && percent > get_skill(ch, gsn_steal))
					 || displaced == TRUE)
		{
	/*
	 * Failure.
	 */
	send_to_char("Oops.\n\r", ch);
	affect_strip(ch, gsn_sneak);
	REMOVE_BIT(ch->affected_by, AFF_SNEAK);

	act("$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT);
	act("$n tried to steal from $N.\n\r", ch, NULL, victim,
	    TO_NOTVICT);
	switch (number_range(0, 3))
	     {
	case 0:
	    sprintf(buf, "%s is a lousy thief!", ch->name);
	    break;
	case 1:
	    sprintf(buf, "%s couldn't rob %s way out of a paper bag!",
		    ch->name, (ch->sex == 2) ? "her" : "his");
	    break;
	case 2:
	    sprintf(buf, "%s tried to rob me!", ch->name);
	    break;
	case 3:
	    sprintf(buf, "Keep your hands out of there, %s!", ch->name);
	    break;
	    }
	if (!IS_AWAKE(victim))
	    do_wake(victim, "");
	if (IS_AWAKE(victim))
	    do_yell(victim, buf);
	if (!IS_NPC(ch)) {
	    if (IS_NPC(victim)) {
		check_improve(ch, gsn_steal, FALSE, 2);
		multi_hit(victim, ch, TYPE_UNDEFINED);
	    } else {
		sprintf(buf, "$N tried to steal from %s.", victim->name);
		wiznet(buf, ch, NULL, WIZ_FLAGS, 0, 0);
		if (!IS_SET(ch->act, PLR_THIEF)) {
		    SET_BIT(ch->act, PLR_THIEF);
		    send_to_char
			("{r*** {RYou are now a THIEF!! {r***{x\n\r", ch);
		    save_char_obj(ch);
		}
	    }
	}
	return;
	}

    if (!str_cmp(arg1, "coin")
	|| !str_cmp(arg1, "coins")
	|| !str_cmp(arg1, "gold"))
	 {
	int gold;

	gold = victim->gold * number_range(1, ch->level) / 60;

	if (gold <= 0) {
	    send_to_char("You couldn't get any coins.\n\r", ch);
	    return;
	}

	/*If victim's gold becomes negative */
	if (victim->gold - gold <= 0) {
	    send_to_char("Have pity on the poor soul!\n\r", ch);
	    gold = victim->gold;
	    ch->gold += gold;
	    victim->gold -= gold;
	    return;
	}

	ch->gold += gold;
	victim->gold -= gold;
	if (victim->gold < 0) {
	    victim->gold = 0;
	}

	sprintf(buf, "Bingo!  You got %d gold coins.\n\r", gold);

	send_to_char(buf, ch);
	check_improve(ch, gsn_steal, TRUE, 2);
	return;
	}
    if ((obj = get_obj_carry(victim, arg1, ch)) == NULL)
	 {
	send_to_char("You can't find it.\n\r", ch);
	return;
	}
    if (!can_drop_obj(ch, obj)
	 || IS_SET(obj->extra_flags, ITEM_INVENTORY)
	 || obj->level > ch->level)
	 {
	send_to_char("You can't pry it away.\n\r", ch);
	return;
	}
    if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
	 {
	send_to_char("You have your hands full.\n\r", ch);
	return;
	}
    if (ch->carry_weight + get_obj_weight(obj) > can_carry_w(ch))
	 {
	send_to_char("You can't carry that much weight.\n\r", ch);
	return;
	}
    obj_from_char(obj);
    obj_to_char(obj, ch);
    act("You pocket $p.", ch, obj, NULL, TO_CHAR);
    check_improve(ch, gsn_steal, TRUE, 2);
    send_to_char("Got it!\n\r", ch);
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper(CHAR_DATA * ch)
{
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for (keeper = ch->in_room->people; keeper;
	 keeper = keeper->next_in_room) {
	if (IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL)
	    break;
    }

    if (pShop == NULL) {
	send_to_char("You can't do that here.\n\r", ch);
	return NULL;
    }
    /*
     * Shop hours.
     */
    if (time_info.hour < pShop->open_hour) {
	do_say(keeper, "Sorry, I am closed. Come back later.");
	return NULL;
    }
    if (time_info.hour > pShop->close_hour) {
	do_say(keeper, "Sorry, I am closed. Come back tomorrow.");
	return NULL;
    }
    /*
     * Invisible or hidden people.
     */
    if (!can_see(keeper, ch)) {
	do_say(keeper, "I don't trade with folks I can't see.");
	return NULL;
    }
    return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper(OBJ_DATA * obj, CHAR_DATA * ch)
{
    OBJ_DATA *t_obj, *t_obj_next;

    /* see if any duplicates are found */
    for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next) {
	t_obj_next = t_obj->next_content;

	if (obj->pIndexData == t_obj->pIndexData
	    && !str_cmp(obj->short_descr, t_obj->short_descr)) {
	    /* if this is an unlimited item, destroy the new one */
	    if (IS_OBJ_STAT(t_obj, ITEM_INVENTORY)) {
		extract_obj(obj);
		return;
	    }
	    obj->cost = t_obj->cost;	/* keep it standard */
	    break;
	}
    }

    if (t_obj == NULL) {
	obj->next_content = ch->carrying;
	ch->carrying = obj;
    } else {
	obj->next_content = t_obj->next_content;
	t_obj->next_content = obj;
    }

    obj->carried_by = ch;
    obj->in_room = NULL;
    obj->in_obj = NULL;
    ch->carry_number += get_obj_number(obj);
    ch->carry_weight += get_obj_weight(obj);
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper(CHAR_DATA * ch, CHAR_DATA * keeper,
			 char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument(argument, arg);
    count = 0;
    for (obj = keeper->carrying; obj != NULL; obj = obj->next_content) {
	if (obj->wear_loc == WEAR_NONE && can_see_obj(keeper, obj)
	    && can_see_obj(ch, obj)
	    && is_name(arg, obj->name)) {
	    if (++count == number)
		return obj;

	    /* skip other objects of the same name */
	    while (obj->next_content != NULL
		   && obj->pIndexData == obj->next_content->pIndexData
		   && !str_cmp(obj->short_descr,
			       obj->next_content->short_descr)) obj =
		    obj->next_content;
	}
    }

    return NULL;
}

int get_cost(CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy)
{
    SHOP_DATA *pShop;
    int cost;

    if (obj == NULL || (pShop = keeper->pIndexData->pShop) == NULL)
	return 0;

    if (fBuy) {
	cost = obj->cost * pShop->profit_buy / 100;
    } else {
	OBJ_DATA *obj2;
	int itype;

	cost = 0;
	for (itype = 0; itype < MAX_TRADE; itype++) {
	    if (obj->item_type == pShop->buy_type[itype]) {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }
	}

	if (!IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT))
	    for (obj2 = keeper->carrying; obj2; obj2 = obj2->next_content) {
		if (obj->pIndexData == obj2->pIndexData
		    && !str_cmp(obj->short_descr, obj2->short_descr)) {
		    if (IS_OBJ_STAT(obj2, ITEM_INVENTORY))
			cost /= 2;
		    else
			cost = cost * 3 / 4;
		}
	    }
    }

    if (obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND) {
	if (obj->value[1] == 0)
	    cost /= 4;
	else
	    cost = cost * obj->value[2] / obj->value[1];
    }
    return cost;
}



void do_buy(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int cost, roll;

    if (argument[0] == '\0') {
	send_to_char("Buy what?\n\r", ch);
	return;
    }
    if (IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP)) {
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	ROOM_INDEX_DATA *in_room;

	smash_tilde(argument);

	if (IS_NPC(ch))
	    return;

	argument = one_argument(argument, arg);

	/* hack to make new thalos pets work */
	if (ch->in_room->vnum == 9621)
	    pRoomIndexNext = get_room_index(9706);
	else
	    pRoomIndexNext = get_room_index(ch->in_room->vnum + 1);
	if (pRoomIndexNext == NULL) {
	    bug("Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum);
	    send_to_char("Sorry, you can't buy that here.\n\r", ch);
	    return;
	}
	in_room = ch->in_room;
	ch->in_room = pRoomIndexNext;
	pet = get_char_room(ch, arg);
	ch->in_room = in_room;

	if (pet == NULL || !IS_SET(pet->act, ACT_PET)) {
	    send_to_char("Sorry, you can't buy that here.\n\r", ch);
	    return;
	}
	if (ch->pet != NULL) {
	    send_to_char("You already own a pet.\n\r", ch);
	    return;
	}
	cost = 2 * pet->level * pet->level;

	if (ch->gold < cost) {
	    send_to_char("You can't afford it.\n\r", ch);
	    return;
	}
	if (ch->level < pet->level) {
	    send_to_char
		("You're not powerful enough to master this pet.\n\r", ch);
	    return;
	}
	/* haggle */
	if (ch->in_room->vnum != ROOM_VNUM_JEWELER) {
	    roll = number_percent();
	    if (roll < get_skill(ch, gsn_haggle)) {
		cost -= cost / 2 * roll / 100;
		sprintf(buf, "You haggle the price down to %d coins.\n\r",
			cost);
		send_to_char(buf, ch);
		check_improve(ch, gsn_haggle, TRUE, 4);

	    }
	}
	deduct_cost(ch, cost);
	pet = create_mobile(pet->pIndexData);
	SET_BIT(pet->act, ACT_PET);
	SET_BIT(pet->affected_by, AFF_CHARM);
	pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;
	pet->no_exp = TRUE;

	argument = one_argument(argument, arg);
	if (arg[0] != '\0') {
	    sprintf(buf, "%s %s", pet->name, arg);
	    free_string(pet->name);
	    pet->name = str_dup(buf);
	}
	sprintf(buf, "%sA neck tag says 'I belong to %s'.\n\r",
		pet->description, ch->name);
	free_string(pet->description);
	pet->description = str_dup(buf);

	char_to_room(pet, ch->in_room);
	add_follower(pet, ch);
	pet->leader = ch;
	ch->pet = pet;
	send_to_char("Enjoy your pet.\n\r", ch);
	act("$n bought $N as a pet.", ch, NULL, pet, TO_ROOM);
	return;
    } else {
	CHAR_DATA *keeper;
	OBJ_DATA *obj, *t_obj;
	char arg[MAX_INPUT_LENGTH];
	int number, count = 1;

	if ((keeper = find_keeper(ch)) == NULL)
	    return;

	if(!can_see(ch, keeper))
	{
		send_to_char("You can't buy anything here.\n\r", ch);
		return;
	}

	number = mult_argument(argument, arg);
	obj = get_obj_keeper(ch, keeper, arg);
	cost = get_cost(keeper, obj, TRUE);

	if (number < 1) {
	    act("$n tells you 'Get real!", keeper, NULL, ch, TO_VICT);
	    return;
	}
	if (number > 100) {

	    sprintf(buf, "%s attempted the shopkeep HACK. *BUG*",
		    ch->name);
	    log_string(buf);
	    act("$n tells you 'Get real!", keeper, NULL, ch, TO_VICT);
	    return;
	}
	if (cost <= 0 || !can_see_obj(ch, obj)) {
	    act("$n tells you 'I don't sell that -- try 'list''.",
		keeper, NULL, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	if (!IS_OBJ_STAT(obj, ITEM_INVENTORY)) {
	    for (t_obj = obj->next_content;
		 count < number && t_obj != NULL;
		 t_obj = t_obj->next_content) {
		if (t_obj->pIndexData == obj->pIndexData
		    && !str_cmp(t_obj->short_descr, obj->short_descr))
		    count++;
		else
		    break;
	    }

	    if (count < number) {
		act("$n tells you 'I don't have that many in stock.",
		    keeper, NULL, ch, TO_VICT);
		ch->reply = keeper;
		return;
	    }
	}
	if (ch->gold < cost * number) {
	    if (number > 1)
		act("$n tells you 'You can't afford to buy that many.",
		    keeper, obj, ch, TO_VICT);
	    else
		act("$n tells you 'You can't afford to buy $p'.",
		    keeper, obj, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	if (obj->level > ch->level) {
	    act("$n tells you 'You can't use $p yet'.",
		keeper, obj, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	if (ch->carry_number + number * get_obj_number(obj) >
	    can_carry_n(ch)) {
	    send_to_char("You can't carry that many items.\n\r", ch);
	    return;
	}
	if (ch->carry_weight + number * get_obj_weight(obj) >
	    can_carry_w(ch)) {
	    send_to_char("You can't carry that much weight.\n\r", ch);
	    return;
	}
	/* haggle */
	if (ch->in_room->vnum != ROOM_VNUM_JEWELER) {
	    roll = number_percent();
	    if (!IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT)
		&& roll < get_skill(ch, gsn_haggle)) {
		cost -= obj->cost / 2 * roll / 100;
		act("You haggle with $N.", ch, NULL, keeper, TO_CHAR);
		check_improve(ch, gsn_haggle, TRUE, 4);
	    }
	}
	if (number > 1) {
	    sprintf(buf, "$n buys $p[%d].", number);
	    act(buf, ch, obj, NULL, TO_ROOM);
	    sprintf(buf, "You buy $p[%d] for %d gold.", number,
		    cost * number);
	    act(buf, ch, obj, NULL, TO_CHAR);
	} else {
	    act("$n buys $p.", ch, obj, NULL, TO_ROOM);
	    sprintf(buf, "You buy $p for %d gold.", cost);
	    act(buf, ch, obj, NULL, TO_CHAR);
	}
	deduct_cost(ch, cost * number);
	keeper->gold += cost * number;

	for (count = 0; count < number; count++) {
	    if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
		t_obj = create_object(obj->pIndexData, obj->level);
	    else {
		t_obj = obj;
		obj = obj->next_content;
		obj_from_char(t_obj);
	    }

	    if (t_obj->timer > 0 && !IS_OBJ_STAT(t_obj, ITEM_HAD_TIMER))
		t_obj->timer = 0;
	    REMOVE_BIT(t_obj->extra_flags, ITEM_HAD_TIMER);
	    obj_to_char(t_obj, ch);
	    if (cost < t_obj->cost)
		t_obj->cost = cost;
	}
    }
}



void do_list(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP)) {
	ROOM_INDEX_DATA *pRoomIndexNext;
	CHAR_DATA *pet;
	bool found;

	/* hack to make new thalos pets work */
	if (ch->in_room->vnum == 9621)
	    pRoomIndexNext = get_room_index(9706);
	else
	    pRoomIndexNext = get_room_index(ch->in_room->vnum + 1);

	if (pRoomIndexNext == NULL) {
	    bug("Do_list: bad pet shop at vnum %d.", ch->in_room->vnum);
	    send_to_char("You can't do that here.\n\r", ch);
	    return;
	}
	found = FALSE;
	for (pet = pRoomIndexNext->people; pet; pet = pet->next_in_room) {
	    if (IS_SET(pet->act, ACT_PET)) {
		if (!found) {
		    found = TRUE;
		    send_to_char("Pets for sale:\n\r", ch);
		}
		sprintf(buf, "[%2d] %8d - %s\n\r",
			pet->level,
			2 * pet->level * pet->level, pet->short_descr);
		send_to_char(buf, ch);
	    }
	}
	if (!found)
	    send_to_char("Sorry, we're out of pets right now.\n\r", ch);
	return;
    } else {
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost, count;
	bool found;
	char arg[MAX_INPUT_LENGTH];

	if ((keeper = find_keeper(ch)) == NULL)
	    return;

	if(!can_see(ch, keeper))
	{
		send_to_char("You can't buy anything here.\n\r", ch);
		return;
	}

	one_argument(argument, arg);

	found = FALSE;
	for (obj = keeper->carrying; obj; obj = obj->next_content) {
	    if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj)
		&& (cost = get_cost(keeper, obj, TRUE)) > 0
		&& (arg[0] == '\0' || is_name(arg, obj->name))) {
		if (!found) {
		    found = TRUE;
		    send_to_char("[Lv Price Qty] Item\n\r", ch);
		}
		if (IS_OBJ_STAT(obj, ITEM_INVENTORY))
		    sprintf(buf, "[%2d %5d -- ] %s\n\r",
			    obj->level, cost, obj->short_descr);
		else {
		    count = 1;

		    while (obj->next_content != NULL
			   && obj->pIndexData ==
			   obj->next_content->pIndexData
			   && !str_cmp(obj->short_descr,
				       obj->next_content->short_descr)) {
			obj = obj->next_content;
			count++;
		    }
		    sprintf(buf, "[%2d %5d %2d ] %s\n\r",
			    obj->level, cost, count, obj->short_descr);
		}
		send_to_char(buf, ch);
	    }
	}

	if (!found)
	    send_to_char("You can't buy anything here.\n\r", ch);
	return;
    }
}



void do_sell(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int cost, roll, tcost = 0, count = 0;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Sell what?\n\r", ch);
	return;
    }
    if ((keeper = find_keeper(ch)) == NULL)
	return;

		if(!can_see(ch, keeper))
	{
		send_to_char("You can't buy anything here.\n\r", ch);
		return;
	}
/*
    if (!str_cmp(arg, "all")) {

	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

	    if (can_drop_obj(ch, obj)
		&& obj->wear_loc == WEAR_NONE
		&& can_see_obj(keeper, obj)
		&& ((cost = get_cost(keeper, obj, FALSE)) >
		    0)
		&& (cost < keeper->gold))
		tcost += cost;
	    count++;
	    ch->gold += cost;
	    deduct_cost(keeper, cost);

	    if (keeper->gold < 0)
		keeper->gold = 0;

	    if (obj->item_type ==
		ITEM_TRASH ||
		IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT)) {
		extract_obj(obj);
	    } else {
		obj_from_char(obj);
		if (obj->timer)
		    SET_BIT(obj->extra_flags, ITEM_HAD_TIMER);
		else
		    obj->timer =
			number_range(50,
				     100);
		obj_to_keeper(obj, keeper);
	    }
	}			// salable item ifcheck                       

    }				// for loop

    sprintf(buf, "You sell %d items for %d gold.\r\n", count,
	    tcost);
    send_to_char(buf, ch);
    return;
}				// 'all' if check

*/

    if (!str_cmp(arg, "all")) {

	for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;

	    if (can_drop_obj(ch, obj)
		&& obj->wear_loc == WEAR_NONE && can_see_obj(keeper, obj)
		&& ((cost = get_cost(keeper, obj, FALSE)) > 0)
		&& (cost < keeper->gold)) {

		tcost += cost;
		count++;
		ch->gold += cost;
		deduct_cost(keeper, cost);

		if (keeper->gold < 0)
		    keeper->gold = 0;

		if (obj->item_type == ITEM_TRASH
		    || IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT)) {
		    extract_obj(obj);
		} else {
		    obj_from_char(obj);
		    if (obj->timer)
			SET_BIT(obj->extra_flags, ITEM_HAD_TIMER);
		    else
			obj->timer = number_range(50, 100);
		    obj_to_keeper(obj, keeper);
		}

	    }			// salable item ifcheck                       

	}			// for loop

	sprintf(buf, "You sell %d items for %d gold.\r\n", count, tcost);
	send_to_char(buf, ch);
	return;
    }				// 'all' if check



    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	act("$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }
    if (!can_drop_obj(ch, obj)) {
	send_to_char("You can't let go of it.\n\r", ch);
	return;
    }
    if (!can_see_obj(keeper, obj)) {
	act("$n doesn't see what you are offering.", keeper, NULL, ch,
	    TO_VICT);
	return;
    }
    if ((cost = get_cost(keeper, obj, FALSE)) <= 0) {
	act("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
	return;
    }
    if (cost > keeper->gold) {
	act
	    ("$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
	     keeper, obj, ch, TO_VICT);
	return;
    }
    act("$n sells $p.", ch, obj, NULL, TO_ROOM);
    /* haggle */
    roll = number_percent();
    if (!IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT)
	&& roll < get_skill(ch, gsn_haggle)) {
	send_to_char("You haggle with the shopkeeper.\n\r", ch);
	cost += obj->cost / 2 * roll / 100;
	cost = UMIN(cost, 95 * get_cost(keeper, obj, TRUE) / 100);
	cost = UMIN(cost, keeper->gold);
	check_improve(ch, gsn_haggle, TRUE, 4);
    }
    sprintf(buf, "You sell $p for %d gold piece%s.",
	    cost, cost == 1 ? "" : "s");
    act(buf, ch, obj, NULL, TO_CHAR);
    ch->gold += cost;
    deduct_cost(keeper, cost);
    if (keeper->gold < 0)
	keeper->gold = 0;

    if (obj->item_type == ITEM_TRASH
	|| IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT)) {
	extract_obj(obj);
    } else {
	obj_from_char(obj);
	if (obj->timer)
	    SET_BIT(obj->extra_flags, ITEM_HAD_TIMER);
	else
	    obj->timer = number_range(50, 100);
	obj_to_keeper(obj, keeper);
    }

    return;
}



void do_value(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Value what?\n\r", ch);
	return;
    }
    if ((keeper = find_keeper(ch)) == NULL)
	return;
	
		if(!can_see(ch, keeper))
	{
		send_to_char("You can't buy anything here.\n\r", ch);
		return;
	}

    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	act("$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }
    if (!can_see_obj(keeper, obj)) {
	act("$n doesn't see what you are offering.", keeper, NULL, ch,
	    TO_VICT);
	return;
    }
    if (!can_drop_obj(ch, obj)) {
	send_to_char("You can't let go of it.\n\r", ch);
	return;
    }
    if ((cost = get_cost(keeper, obj, FALSE)) <= 0) {
	act("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
	return;
    }
    sprintf(buf,
	    "$n tells you 'I'll give you %d gold coins for $p'.", cost);
    act(buf, keeper, obj, ch, TO_VICT);
    ch->reply = keeper;

    return;
}

void do_donate(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *donwiz;
    OBJ_DATA *container;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Donate what?\n\r", ch);
	return;
    }
    if ((obj = get_obj_carry(ch, arg, ch)) == NULL) {
	send_to_char("You aren't carrying that item.\n\r", ch);
	return;
    }
    if (!can_drop_obj(ch, obj)) {
	send_to_char("You can't let go of it.\n\r", ch);
	return;
    }
    if ((donwiz = get_char_world(ch, "bedweezle")) == NULL) {
	send_to_char("No wizard is available to retrieve your item.\n\r",
		     ch);
	return;
    }
    if (ch->in_room != donwiz->in_room) {
	act
	    ("With a gentle *pop*, $n disappears from the room.",
	     donwiz, NULL, NULL, TO_ROOM);
	char_from_room(donwiz);

	char_to_room(donwiz, ch->in_room);
	act("With a gentle *pop*, $n appears in the room.",
	    donwiz, NULL, NULL, TO_ROOM);
    }
    /* give obj to wizard */

    if (((obj->item_type != ITEM_CONTAINER) && (can_see_obj(donwiz, obj)
						&& (obj->level >= 2)))) {

	/* message to room */
	sprintf(buf, "$n takes $p from $N.");
	act(buf, donwiz, obj, ch, TO_NOTVICT);

	/* do the deed */
	if (can_drop_obj(ch, obj)) {
	    obj_from_char(obj);
	    obj_to_char(obj, donwiz);

	    do_say(donwiz, "Thank you!");
	} else {
	    do_say(donwiz, "I don't want that!");
	}
	/* no can put in pit, no can take */
    } else {

	do_say(donwiz, "I can't put that in the pit, sorry.");

	sprintf(buf, "With a gentle *pop*, $n disappears.");
	act(buf, donwiz, NULL, NULL, TO_ROOM);

	char_from_room(donwiz);
	char_to_room(donwiz, get_room_index(ROOM_VNUM_HIGHBIE_ALTAR));

	act("$N appears in the room!", donwiz, NULL, NULL,
	    TO_ROOM);
	return;
    }

    if (obj->level < 30) {
	if (donwiz->in_room->vnum != ROOM_VNUM_LOWBIE_ALTAR) {
	    sprintf(buf, "With a gentle *pop*, %s disappears.\r\n",
		  	PERS(ch, donwiz));
	    send_to_char(buf, ch);
	    sprintf(buf, "With a gentle *pop*, %s disappears.",
		    PERS(ch, donwiz));
	    act(buf, ch, NULL, NULL, TO_ROOM);

	    char_from_room(donwiz);
	    char_to_room(donwiz, get_room_index(ROOM_VNUM_LOWBIE_ALTAR));

	    act("$n appears in the room!", donwiz, NULL,
		NULL, TO_ROOM);
	}
    } else if ((obj->level > 29) && (obj->level < 70)) {
	if (donwiz->in_room->vnum != ROOM_VNUM_MIDBIE_ALTAR) {
	    sprintf(buf, "With a gentle *pop*, %s disappears.\r\n",
		    PERS(ch, donwiz));
	    send_to_char(buf, ch);
	    sprintf(buf, "With a gentle *pop*, %s disappears.",
		    PERS(ch, donwiz));
	    act(buf, ch, NULL, NULL, TO_ROOM);

	    char_from_room(donwiz);
	    char_to_room(donwiz, get_room_index(ROOM_VNUM_MIDBIE_ALTAR));

	    act("$n appears in the room!", donwiz, NULL,
		NULL, TO_ROOM);
	}
    } else {
	if (donwiz->in_room->vnum != ROOM_VNUM_HIGHBIE_ALTAR) {
	    sprintf(buf, "With a gentle *pop*, %s disappears.\r\n",
		    PERS(ch, donwiz));
	    send_to_char(buf, ch);

	    sprintf(buf, "With a gentle *pop*, %s disappears.",
		    PERS(ch, donwiz));
	    act(buf, ch, NULL, NULL, TO_ROOM);

	    char_from_room(donwiz);
	    char_to_room(donwiz, get_room_index(ROOM_VNUM_HIGHBIE_ALTAR));

	    act("$n appears in the room!", donwiz, NULL,
		NULL, TO_ROOM);
	}
    }

    if (obj->item_type == ITEM_WEAPON) {

	sprintf(buf, "$n tosses $p into the weapon pit.");

	act(buf, donwiz, obj, NULL, TO_ROOM);

	container = get_obj_here(donwiz, "weapon");
	obj_from_char(obj);
	if (container != NULL)
	    obj_to_obj(obj, container);

    } else if (obj->item_type == ITEM_ARMOR) {

	sprintf(buf, "%s tosses %s into the armor pit.",
		PERS(ch, donwiz), OBJS(ch, obj));
	act(buf, donwiz, NULL, NULL, TO_ROOM);

	container = get_obj_here(donwiz, "armor");
	obj_from_char(obj);
	if (container != NULL)
	    obj_to_obj(obj, container);

    } else {

	act("$n tosses $p into the miscellaneous pit.", 
		donwiz, obj, NULL, TO_ROOM);

	container = get_obj_here(donwiz, "misc");
	obj_from_char(obj);
	if (container != NULL)
	    obj_to_obj(obj, container);

    }

    return;
}
