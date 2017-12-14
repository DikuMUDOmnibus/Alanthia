#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"
#include "utils.h"

DECLARE_DO_FUN(do_whirl);

void do_beer(CHAR_DATA * ch, char *argument)
{
    int hitroll, damroll;
    int rounds;
    char points[MAX_STRING_LENGTH];
    AFFECT_DATA af;

    argument = one_argument(argument, points);
    rounds = atoi(points);

    if (ch->race != race_lookup("dwarf")) {
	    send_to_char("Sorry. Try playing a dwarf instead.\n\r", ch);
	    return;
    }

    if (ch->blood < 10) {
	    send_to_char("You're not drunk enough.\n\r", ch);
	    return;
    }

    if (rounds < 1) {
    	send_to_char("You can't beerserk for zero rounds.\n\r", ch);
	    return;
    }

    if (ch->blood < (rounds)) {
	    send_to_char("You don't have enough beer points to do that!\n\r", ch);
      return;
    }

    if (is_affected(ch, gsn_beer)) {
      affect_strip(ch, gsn_beer);
    }

    hitroll = -(rounds / 4);
    damroll = (rounds / 2);

    af.type = gsn_beer;
    af.duration = rounds / 10;
    af.level = ch->level;

    /* damroll modifier */
    af.location = APPLY_DAMROLL;
    af.modifier = damroll;
    affect_to_char(ch, &af);

    /* hitroll modifier */
    af.location = APPLY_HITROLL;
    af.modifier = hitroll;
    affect_to_char(ch, &af);

    /* ac modifier */
    af.location = APPLY_AC;
    af.modifier = UMAX(10, 10 * (rounds));
    affect_to_char(ch, &af);

    /* blood drop */
    ch->blood -= (rounds);
    if (ch->blood < 0)
	    ch->blood = 0;

    send_to_char("The alcohol in your blood turns to rage!\n\r", ch);

    return;
}


void skill_quick_poison(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent, skill;

    if (argument == '\0') {
	send_to_char("What where you planning?\n\r", ch);
	return;
    }

    obj = get_obj_list(ch, argument, ch->carrying);

    if (obj == NULL) {
	send_to_char("You don't have that item.\n\r", ch);
	return;
    }
    if ((skill = get_skill(ch, gsn_quick_poison)) < 1) {
	send_to_char("This isn't a good plan.\n\r", ch);
	return;
    }
    if (obj->item_type == ITEM_WEAPON) {
	if (obj->value[3] < 0
	    || attack_table[obj->value[3]].damage == DAM_BASH) {
	    send_to_char("Didn't you think of a way to use it?\n\r", ch);
	    return;
	}

  WAIT_STATE(ch, skill_table[gsn_quick_poison].beats);

	percent = number_percent();
	if (percent < skill) {
	    af.where = TO_WEAPON;
	    af.type = gsn_quick_poison;
	    af.level = ch->level;
	    af.duration = (ch->level / 12);
	    af.location = 0;
	    af.modifier = 0;
	    af.bitvector = WEAPON_QUICKPOISON;
	    affect_to_obj(obj, &af);
	    act("You have put venom on $p.", ch, obj, NULL, TO_CHAR);
	    check_improve(ch, gsn_quick_poison, TRUE, 4);
	    return;
	} else {
	    send_to_char("You failed", ch);
	}
    }

    return;
}
