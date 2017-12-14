#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"
#include "utils.h"

void do_preserve(CHAR_DATA * ch, char *argument)
{

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int cost = 0;

    if (argument[0] == '\0') {
	send_to_char("Preserve what?\n\r", ch);
	return;
    }
    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, arg);

    /* check for repairman */
    for (keeper = ch->in_room->people; keeper;
	 keeper = keeper->next_in_room) {
	if (IS_NPC(keeper) && IS_SET(keeper->act, ACT_REPAIR))
	    break;
    }

    if (keeper == NULL) {
	send_to_char("You can't do that here.\n\r", ch);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) == NULL) {
	send_to_char("You do not see that here.\n\r", ch);
	return;
    }
    if (!can_see_obj(ch, obj)) {
	act("$n tells you 'I can't see it!'", keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }

    switch (obj->item_type) {

    case ITEM_CORPSE_PC:
    case ITEM_CORPSE_NPC:
	cost = (obj->level * 5) + (obj->condition * 5);

	if (cost <= 0) {
	    act("$n tells you 'That isn't worth my time to preserve!'",
		keeper, NULL, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	if (ch->gold < cost) {
	    act
		("$n tells you 'You don't have enough money to preserve that.'",
		 keeper, NULL, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	obj->condition = 0;
	obj->weight = obj->level / 2;
	obj->item_type = ITEM_CONTAINER;
	obj->value[0] = obj->level;	// Weight
	obj->value[3] = obj->level;	// Capacity
	obj->value[4] = obj->level / 3;	// Multiplier
	obj->cost = obj->level / 2;
	obj->timer = 0;

	sprintf(buf, "bag corpse");
	free_string(obj->name);
	obj->name = str_dup(buf);

	sprintf(buf, "a bag made from %s", obj->short_descr);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(buf);
	break;

    case ITEM_FOOD:

	cost = obj->value[0] * 4;

	if (obj->timer < 1) {
	    act("$n tells you 'That doesn't need preserving.'",
		keeper, NULL, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	if (ch->gold < cost) {
	    act
		("$n tells you 'You don't have enough money to preserve that.'",
		 keeper, NULL, ch, TO_VICT);
	    ch->reply = keeper;
	    return;
	}
	obj->timer = 0;

	break;


    default:
	act("$n tells you 'I can't preserve that. Sorry, pal!'",
	    keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
	break;
	return;
    }

    act("$n preserves $p.", ch, obj, NULL, TO_ROOM);
    sprintf(buf, "You preserve $p for %d gold.", cost);
    act(buf, ch, obj, NULL, TO_CHAR);

    deduct_cost(ch, cost);
    keeper->gold += cost;

}

void do_repair(CHAR_DATA * ch, char *argument)
{

    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int cost;

    if (argument[0] == '\0') {
	send_to_char("Repair what?\n\r", ch);
	return;
    }
    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, arg);


    /* check for repairman */
    for (keeper = ch->in_room->people; keeper;
	 keeper = keeper->next_in_room) {
	if (IS_NPC(keeper) && IS_SET(keeper->act, ACT_REPAIR))
	    break;
    }

    if (keeper == NULL) {
	send_to_char("You can't do that here.\n\r", ch);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) == NULL) {
	send_to_char("You do not see that here.\n\r", ch);
	return;
    }
    if (!can_see_obj(ch, obj)) {
	act("$n tells you 'I can't see it!'", keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }
    cost = (obj->level * 5) + (obj->condition * 5);

    if (cost <= 0) {
	act("$n tells you 'That isn't worth my time to fix!'",
	    keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }
    if (obj->condition == 0) {
	act("$n tells you 'You'd only be wasting money repairing that.'",
	    keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }
    if (ch->gold < cost) {
	act("$n tells you 'You don't have enough money to repair that.'",
	    keeper, NULL, ch, TO_VICT);
	ch->reply = keeper;
	return;
    }
    act("$n repairs $p.", ch, obj, NULL, TO_ROOM);
    sprintf(buf, "You repair $p for %d gold.", cost);
    act(buf, ch, obj, NULL, TO_CHAR);

    obj->condition = 0;

    deduct_cost(ch, cost);
    keeper->gold += cost;

}

void do_bind(CHAR_DATA * ch, char *argument)
{


    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int cost = 0;

    return;

    if (argument[0] == '\0') {
	    send_to_char("bind what?\n\r", ch);
	    return;
    }
    if (IS_NPC(ch))
	    return;

    argument = one_argument(argument, arg);

    /* check for repairman */
    for (keeper = ch->in_room->people; keeper;
	    keeper = keeper->next_in_room) {
	    if (IS_NPC(keeper) && IS_SET(keeper->act, ACT_REPAIR))
	    break;
    }

    if (keeper == NULL) {
	    send_to_char("You can't do that here.\n\r", ch);
	  return;
    }
    if ((obj = get_obj_here(ch, arg)) == NULL) {
	    send_to_char("You do not see that here.\n\r", ch);
	  return;
    }
    if (!can_see_obj(ch, obj)) {
	    act("$n tells you 'I can't see it!'", keeper, NULL, ch, TO_VICT);
	    ch->reply = keeper;
	  return;
    }

    cost = obj->level * 100000;
    obj->owner = str_dup(ch->name);
    obj->bound = TRUE;

    act("$n binds $p.", ch, obj, NULL, TO_ROOM);
    sprintf(buf, "You bind $p for %d gold.", cost);
    act(buf, ch, obj, NULL, TO_CHAR);

    deduct_cost(ch, cost);
    ch->exp -= (obj->level * 10);

    if (ch->exp < 1) 
      ch->exp = 0;

    keeper->gold += cost;

}
