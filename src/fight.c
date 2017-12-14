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
#endif
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "recycle.h"

int hitchance;
int diceroll;

/*
 *  Tables for wrath
 */
char *const pos_table[] = {
    "dead", "mortal", "incap", "stun", "sleep", "rest", "sit", "fight",
    "stand",
    NULL
};

char *const eq_table[] = {
    "light", "finger_l", "finger_r", "neck_1", "neck_2", "torso", "head",
    "legs",
    "feet", "hands", "arms", "shield", "body", "waist", "wrist_l",
    "wrist_r",
    "wield", "hold", "float", NULL
};

/* command procedures needed */
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_assassinate);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_bash);
DECLARE_DO_FUN(do_berserk);
DECLARE_DO_FUN(do_trip);
DECLARE_DO_FUN(do_dirt);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_kill);
DECLARE_DO_FUN(do_circle);
DECLARE_DO_FUN(do_whirl);
DECLARE_DO_FUN(do_arial);
DECLARE_DO_FUN(do_stand);
DECLARE_DO_FUN(do_feed);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_land);
DECLARE_ATTACK_FUN(attack_hit);


/*
 * Local functions.
 */
void check_assist args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_dodge args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_duck
args((CHAR_DATA * ch, CHAR_DATA * victim, char *attack_type));
void check_killer args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_parry args((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_shield_block args((CHAR_DATA * ch, CHAR_DATA * victim));
void dam_message args((CHAR_DATA * ch, CHAR_DATA * victim, int dam,
		       int dt, bool immune));
void death_cry args((CHAR_DATA * ch, CHAR_DATA * victim));

void group_gain args((CHAR_DATA * ch, CHAR_DATA * victim));
int xp_compute
args((CHAR_DATA * gch, CHAR_DATA * victim, int total_levels));
bool is_safe args((CHAR_DATA * ch, CHAR_DATA * victim));
void make_corpse args((CHAR_DATA * ch, bool can_loot));
void death_penalty args((CHAR_DATA * ch));
void raw_kill args((CHAR_DATA * ch, CHAR_DATA * victim));
void one_hit args((CHAR_DATA * ch, CHAR_DATA * victim, int dt));
void mob_hit args((CHAR_DATA * ch, CHAR_DATA * victim, int dt));
void set_fighting args((CHAR_DATA * ch, CHAR_DATA * victim));
void disarm args((CHAR_DATA * ch, CHAR_DATA * victim));
bool has_number args((char *string));


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update(void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for (ch = char_list; ch != NULL; ch = ch->next) {
	ch_next = ch->next;

	if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
	    continue;

	if (IS_AWAKE(ch) && ch->in_room == victim->in_room)
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	else
	    stop_fighting(ch, FALSE);

	if ((victim = ch->fighting) == NULL)
	    continue;

	if (IS_NPC(ch)) {
	    if (HAS_TRIGGER(ch, TRIG_FIGHT))
		mp_percent_trigger(ch, victim, NULL, NULL, TRIG_FIGHT);
	    if (HAS_TRIGGER(ch, TRIG_HPCNT))
		mp_hprct_trigger(ch, victim);
	}
	/*
	 * Fun for the whole family!
	 */
	check_assist(ch, victim);
    }

    return;
}

/* for auto assisting */
void check_assist(CHAR_DATA * ch, CHAR_DATA * victim)
{
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next) {
	rch_next = rch->next_in_room;

	if (IS_AWAKE(rch) && rch->fighting == NULL) {

	    /* quick check for ASSIST_PLAYER */
	    if (!IS_NPC(ch) && IS_NPC(rch)
		&& IS_SET(rch->off_flags, ASSIST_PLAYERS)
		&& rch->level + 6 > victim->level) {
		do_emote(rch, "screams and attacks!");
		multi_hit(rch, victim, TYPE_UNDEFINED);
		continue;
	    }
	    /* PCs next */
	    if (!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM)) {
		if (((!IS_NPC(rch) && IS_SET(rch->act, PLR_AUTOASSIST))
		     || IS_AFFECTED(rch, AFF_CHARM))
		    && is_same_group(ch, rch) && !is_safe(rch, victim))
		    multi_hit(rch, victim, TYPE_UNDEFINED);

		continue;
	    }

	    /* now check the NPC cases */
	    if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM)) {
		if ((IS_NPC(rch) && IS_SET(rch->off_flags, ASSIST_ALL))
		    || (IS_NPC(rch) && rch->group
			&& rch->group == ch->group) || (IS_NPC(rch)
							&& rch->race ==
							ch->race
							&& IS_SET(rch->
								  off_flags,
								  ASSIST_RACE))
		    || (IS_NPC(rch) && IS_SET(rch->off_flags, ASSIST_ALIGN)
			&& ((IS_GOOD(rch) && IS_GOOD(ch))
			    || (IS_EVIL(rch) && IS_EVIL(ch))
			    || (IS_NEUTRAL(rch) && IS_NEUTRAL(ch))))
		    || (rch->pIndexData == ch->pIndexData
			&& IS_SET(rch->off_flags, ASSIST_VNUM))) {
		    CHAR_DATA *vch;
		    CHAR_DATA *target;
		    int number;

		    if (number_bits(1) == 0)
			continue;

		    target = NULL;
		    number = 0;
		    for (vch = ch->in_room->people; vch; vch = vch->next) {
			if (can_see(rch, vch)
			    && is_same_group(vch, victim)
			    && number_range(0, number) == 0) {
			    target = vch;
			    number++;
			}
		    }

		    if (target != NULL) {
			do_emote(rch, "screams and attacks!");
			multi_hit(rch, target, TYPE_UNDEFINED);
		    }
		}
	    }
	}
    }
}


/*
 * Do one group of attacks.
 */
void multi_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance;

    /* decrement the wait */
    if (ch->desc == NULL)
	ch->wait = UMAX(0, ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
	ch->daze = UMAX(0, ch->daze - PULSE_VIOLENCE);

    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
	return;

    if (dt == gsn_assassinate) {
	one_hit(victim, ch, dt);
	return;
    }

    if (IS_NPC(ch)) {
	mob_hit(ch, victim, dt);
	return;
    }

    one_hit(ch, victim, dt);

    if (ch->fighting != victim)
	return;

    if (IS_AFFECTED(ch, AFF_HASTE))
	one_hit(ch, victim, dt);

    if (ch->fighting != victim || dt == gsn_backstab)
	return;

    chance = get_skill(ch, gsn_second_attack) / 2;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt);
	check_improve(ch, gsn_second_attack, TRUE, 5);
	if (ch->fighting != victim)
	    return;
    }

    chance = get_skill(ch, gsn_third_attack) / 4;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt);
	check_improve(ch, gsn_third_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }

    chance = get_skill(ch, gsn_fourth_attack) / 4;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt);
	check_improve(ch, gsn_fourth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_fifth_attack) / 4;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt);
	check_improve(ch, gsn_fifth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }
    chance = get_skill(ch, gsn_sixth_attack) / 4;

    if (number_percent() < chance) {
	one_hit(ch, victim, dt);
	check_improve(ch, gsn_sixth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }
    return;
}

/* procedure for all mobile attacks */
/* Grb: deprecated. mobs attack just like players. left this here for historical reasons */
void mob_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    one_hit(ch, victim, dt);
    return;
}

/*
 * dt is the pointer to the element id in the attack_table
 */
int correct_dt(int dt, CHAR_DATA * ch, OBJ_DATA * wield)
{

    if (dt == TYPE_UNDEFINED) {
	dt = TYPE_HIT;

	if (wield != NULL && wield->item_type == ITEM_WEAPON)
	    dt = wield->value[3];
	else
	    dt = ch->dam_type;
    }

    return dt;
}

/* 
 * dam_type is the DAM_TYPE for the elemener in the attack_table
 */
int calc_dam_type(CHAR_DATA * ch, OBJ_DATA * wield, int dt)
{
    int dam_type;

    if (dt < TYPE_HIT)
	if (wield != NULL)
	    dam_type = attack_table[wield->value[3]].damage;
	else
	    dam_type = attack_table[ch->dam_type].damage;
    else
	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type < 0)
	dam_type = DAM_BASH;

    return dam_type;
}

/*
 * Calc damage from a hit.
 * Returns total damage, including from, etc special weapon flags
 */
int calc_damage(CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * wield,
		int sn, int skill, int dt)
{

    int dam;

    if (IS_NPC(ch) && wield == NULL)
	dam = dice(ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);
    else {
	if (sn != -1)
	    check_improve(ch, sn, TRUE, 5);
	if (wield != NULL) {
	    if (wield->pIndexData->new_format)
		dam = dice(wield->value[1], wield->value[2]) * skill / 100;
	    else
		dam = number_range(wield->value[1] * skill / 100,
				   wield->value[2] * skill / 100);

	    if (get_eq_char(ch, WEAR_SHIELD) == NULL)	/* no shield = more */
		dam = dam * 11 / 10;

	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield, WEAPON_SHARP)
		&& (wield->condition <= (wield->level * 2))) {
		int percent;

		if ((percent = number_percent()) <= (skill / 8))
		    dam = 2 * dam + (dam * 2 * percent / 100);

		/* Condition Flagging! Wdl */

		wield->condition++;

		if (wield->condition == (wield->level * 2)) {
		    REMOVE_BIT(wield->value[4], WEAPON_SHARP);
		    send_to_char
			("{gYour weapon seems to dull slightly...{x\r\n",
			 ch);
		}
	    }
	} else
	    dam = number_range(1 + 4 * skill / 100, 2 * ch->level / 3
			       * skill / 100);
    }

/*    
 * Bonuses.
 */
    if (get_skill(ch, gsn_enhanced_damage) > 0) {
	diceroll = number_percent();
	if (diceroll <= get_skill(ch, gsn_enhanced_damage)) {
	    check_improve(ch, gsn_enhanced_damage, TRUE, 6);
	    dam += (3 * (ch->level)) / 20;
	}
    }
    if (get_skill(ch, gsn_ultra_damage) > 0) {
	diceroll = number_percent();
	if (diceroll <= get_skill(ch, gsn_ultra_damage)) {
	    check_improve(ch, gsn_ultra_damage, TRUE, 6);
	    dam += (2 * (ch->level)) / 9;
	}
    }
    if (!IS_AWAKE(victim))
	dam *= 2;
    else if (victim->position < POS_FIGHTING)
	dam = dam * 3 / 2;

    /* Backstab Damage */
    if (dt == gsn_backstab && wield != NULL) {
	dam *= 1 + (ch->level / 16);
    }

    dam += GET_DAMROLL(ch) * UMIN(100, skill) / 100;

    if (wield != NULL && ch->fighting == victim) {

	if (IS_WEAPON_STAT(wield, WEAPON_POISON)) {
	    int level;
	    AFFECT_DATA *poison, af;

	    if ((poison =
		 affect_find(wield->affected, gsn_poison)) == NULL)
		level = wield->level;
	    else
		level = poison->level;

	    if (!saves_spell(level / 2, victim, DAM_POISON)) {
		send_to_char
		    ("You feel poison coursing through your veins.\n\r",
		     victim);
		act("$n is poisoned by the venom on $p.", victim, wield,
		    NULL, TO_ROOM);

		af.where = TO_AFFECTS;
		af.type = gsn_poison;
		af.level = level * 3 / 4;
		af.duration = level / 2;
		af.location = APPLY_STR;
		af.modifier = -1;
		af.bitvector = AFF_POISON;
		affect_join(victim, &af);
	    }
	    /* weaken the poison if it's temporary */
	    if (poison != NULL) {
		poison->level = UMAX(0, poison->level - 2);
		poison->duration = UMAX(0, poison->duration - 1);

		if (poison->level == 0 || poison->duration == 0)
		    act("The poison on $p has worn off.", ch, wield, NULL,
			TO_CHAR);
	    }

	    return dam;

	}


	if (IS_WEAPON_STAT(wield, WEAPON_VAMPIRIC)) {
	    dam += number_range(1, wield->level / 2 + 1);
	    act("$p draws life from $n.", victim, wield, NULL, TO_ROOM);
	    act("You feel $p drawing your life away.",
		victim, wield, NULL, TO_CHAR);
	    damage(ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
	    ch->alignment = UMAX(-1000, ch->alignment - 1);
	    ch->hit += dam / 2;
	    return dam;
	}

	if (IS_WEAPON_STAT(wield, WEAPON_HOLY)) {
	    dam += number_range(1, wield->level / 5 + 1);
	    act("$p absorbs life force from $n.", victim, wield, NULL,
		TO_ROOM);
	    act("You feel $p taking life force away from you.", victim,
		wield, NULL, TO_CHAR);
	    ch->alignment = UMAX(-1000, ch->alignment + 1);

	    if (ch->alignment > 1000) {
		ch->alignment = 1000;
	    }

	    return dam;
	}

	if (IS_WEAPON_STAT(wield, WEAPON_VORPAL)) {
	    dam += number_range(1, wield->level / 5 + 1);
	    act("$p makes $n weak and tired!", victim, wield, NULL,
		TO_ROOM);
	    act("You feel $p draining your strength!", victim, wield, NULL,
		TO_CHAR);
	    victim->move -= dam / 2;

	    if (victim->move < 1) {
		do_land(victim, "");
		victim->move = 0;
	    }

	    return dam;
	}

	if (IS_WEAPON_STAT(wield, WEAPON_FLAMING)) {
	    dam += number_range(1, wield->level / 4 + 1);
	    act("$n is burned by $p.", victim, wield, NULL, TO_ROOM);
	    act("$p sears your flesh.", victim, wield, NULL, TO_CHAR);
	    fire_effect((void *) victim, wield->level / 2, dam,
			TARGET_CHAR);
	    return dam;
	}

	if (IS_WEAPON_STAT(wield, WEAPON_FROST)) {
	    dam += number_range(1, wield->level / 6 + 2);
	    act("$p freezes $n.", victim, wield, NULL, TO_ROOM);
	    act("The cold touch of $p surrounds you with ice.",
		victim, wield, NULL, TO_CHAR);
	    cold_effect(victim, wield->level / 2, dam, TARGET_CHAR);
	    return dam;
	}

	if (IS_WEAPON_STAT(wield, WEAPON_SHOCKING)) {
	    dam += number_range(1, wield->level / 5 + 2);
	    act("$n is struck by lightning from $p.", victim, wield, NULL,
		TO_ROOM);
	    act("You are shocked by $p.", victim, wield, NULL, TO_CHAR);
	    shock_effect(victim, wield->level / 2, dam, TARGET_CHAR);
	    return dam;
	}

	if (IS_WEAPON_STAT(wield, WEAPON_QUICKPOISON)) {
	    dam += ((wield->level * 18) / 10);
	    act("$n is infected by $p.", victim, wield, NULL, TO_ROOM);
	    act("poison is flooding into your body", victim, wield, NULL,
		TO_CHAR);
	    return dam;
	}
    }

    return dam;


}

/* 
 * chance-to-hit calculations
 */
int calc_hit_chance(int hitchance, CHAR_DATA * ch, CHAR_DATA * victim,
		    int skill, int dt, int dam_type)
{

    int victim_ac;
#ifdef DEBUG
    char buf[MAX_STRING_LENGTH];
#endif


    hitchance = 50;
    hitchance += ((ch->level - victim->level) / 5);

#ifdef DEBUG
    sprintf(buf, "{YBased on level, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // size - smaller opponents are harder to hit
    hitchance -= ((ch->size - victim->size) * 3);

#ifdef DEBUG
    sprintf(buf, "{YBased on size, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // fatigue - we fight better when fresh
    // should reduce hit chance by 1 for every % below 50 char's mv is
    // should increase hit chance by 1 for every % above 90 char's mv is
    //FIXME:
    hitchance += (20 - ((ch->max_move - ch->move) / 25));

#ifdef DEBUG
    sprintf(buf, "{YBased on fatigue, chance to hit is %d{x\r\n",
	    hitchance);
    send_to_char(buf, ch);
#endif

    // strength - plus to hit if stronger than opponent
    hitchance += (get_curr_stat(ch, STAT_STR)
		  - get_curr_stat(victim, STAT_STR));

#ifdef DEBUG
    sprintf(buf, "{YBased on str, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // dexterity
    hitchance += (get_curr_stat(ch, STAT_DEX)
		  - get_curr_stat(victim, STAT_DEX));

#ifdef DEBUG
    sprintf(buf, "{YBased on dex, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // int
    hitchance += (get_curr_stat(ch, STAT_INT)
		  - get_curr_stat(victim, STAT_INT));

#ifdef DEBUG
    sprintf(buf, "{YBased on int, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // wisdom
    hitchance += (get_curr_stat(ch, STAT_WIS)
		  - get_curr_stat(victim, STAT_WIS));

#ifdef DEBUG
    sprintf(buf, "{YBased on wis, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // burden
    hitchance += (100 - (get_carry_weight(ch) / can_carry_w(ch) * 100));

#ifdef DEBUG
    sprintf(buf, "{YBased on burden, chance to hit is %d{x\r\n",
	    hitchance);
    send_to_char(buf, ch);
#endif

    // skill bonus
    hitchance += (skill / 10);

#ifdef DEBUG
    sprintf(buf, "{YBased on skill, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    // hitroll
    hitchance += ch->hitroll;

#ifdef DEBUG
    sprintf(buf, "{YAfter HR check, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    if (IS_AFFECTED(ch, AFF_BLIND))
	hitchance -= 125;

    if (IS_AFFECTED(victim, AFF_BLIND))
	hitchance += 60;

    if (dt == gsn_backstab) {
	hitchance -= 6 * (100 - get_skill(ch, gsn_backstab));
    }

    switch (dam_type) {
    case (DAM_PIERCE):
	victim_ac = GET_AC(victim, AC_PIERCE) / 25;
	break;
    case (DAM_BASH):
	victim_ac = GET_AC(victim, AC_BASH) / 25;
	break;
    case (DAM_SLASH):
	victim_ac = GET_AC(victim, AC_SLASH) / 25;
	break;
    default:
	victim_ac = GET_AC(victim, AC_EXOTIC) / 25;
	break;
    };

    if (victim->position < POS_FIGHTING)
	victim_ac += 12;

    if (victim->position < POS_RESTING)
	victim_ac += 20;

    hitchance += victim_ac / 100;

#ifdef DEBUG
    sprintf(buf, "{YOpponent's AC is %d{x\r\n", victim_ac);
    send_to_char(buf, ch);
    sprintf(buf, "{YAfter AC check, chance to hit is %d{x\r\n", hitchance);
    send_to_char(buf, ch);
#endif

    if (IS_DWARF(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	hitchance += ch->pcdata->condition[COND_DRUNK] / 3;
    else if (!IS_DWARF(ch) && !IS_NPC(ch)
	     && ch->pcdata->condition[COND_DRUNK] > 10)
	hitchance -= ch->pcdata->condition[COND_DRUNK] / 3;

#ifdef DEBUG
    sprintf(buf, "{YAfter drunk check, chance to hit is %d{x\r\n",
	    hitchance);
    send_to_char(buf, ch);
#endif

    /* turf bonus */
    if (ch->clan) {
	if (ch->in_room->turf_clan && (ch->in_room->turf_clan == ch->clan)) {
	    hitchance += UMIN(20, (ch->in_room->turf_level / 10));
	} else
	    if (ch->in_room->turf_clan
		&& (ch->in_room->turf_clan != ch->clan)) {
	    hitchance -= UMIN(20, (ch->in_room->turf_level / 10));
	}
    }

    if (hitchance < 0)
	hitchance = 0;

#ifdef DEBUFG
    sprintf(buf, "{YAfter turf check, chance to hit is %d{x\r\n",
	    hitchance);
    send_to_char(buf, ch);
#endif

    return hitchance;
}


int execute_attack(CHAR_DATA * ch, int attack, CHAR_DATA * victim,
		   OBJ_DATA * wield, int sn, int skill, int dam_type,
		   int dt)
{
    ATTACK_FUN *attack_fun;
    int damroll, dam;

    attack_fun = attack_table[attack].function;

    ch->cwait += attack_table[attack].cwait;

    damroll *= attack_table[attack].dr_mod;
    dam += (ch->damroll / 5);

    //FIXME: modify attacker's AC

    attack_fun(ch, victim, dt);

    dam = calc_damage(ch, victim, wield, sn, skill, dt);

    damage(ch, victim, dam, dt, dam_type, TRUE);

    return dam;
}


/*
* One attack stack
* The meat of the attacks has been moved to attacks.c
*/
void one_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    OBJ_DATA *wield;
    int sn, skill;
    int dam_type;
    int current_attack;
    char buf[MAX_STRING_LENGTH];
    sn = -1;

    //just in case
    if (victim == ch || ch == NULL || victim == NULL
	|| victim->position == POS_DEAD || ch->in_room != victim->in_room)
	return;

    // execute an attack stack, one at a time, until out of attacks or time.
    while ((ch->cwait + attack_table[ch->current_attack + 1].cwait)
	   < COMBAT_WAIT_LENGTH) {

	//FIXME: Make sure to check for zero-time attacks so this can't hang

	if (victim == ch || ch == NULL || victim == NULL
	    || victim->position == POS_DEAD
	    || ch->in_room != victim->in_room)
	    break;

	// if ch has no attack stack set, use default attack.. ("hit") 
	if (!ch->current_attack
	    && ch->attack_stack[ch->current_attack] == -1)
	    current_attack = 0;

	if (ch->attack_stack[ch->current_attack] == -1)
	    ch->current_attack = 0;

	current_attack = ch->attack_stack[ch->current_attack];

	hitchance = 0;

#ifdef DEBUG
	sprintf(buf, "{Ycwait is %d{x\r\n", ch->cwait);
	send_to_char(buf, ch);
#endif

	// decide if we're attacking with a weapon or hand to hand
	wield = get_eq_char(ch, WEAR_WIELD);
	dt = correct_dt(dt, ch, wield);
	dam_type = calc_dam_type(ch, wield, dt);

	// and set up the skill
	skill = get_weapon_skill(ch, get_weapon_sn(ch));

	// decide % chance to hit
	hitchance =
	    calc_hit_chance(hitchance, ch, victim, skill, dt, dam_type);

	diceroll = number_range(0, 99);

#ifdef DEBUG
	sprintf(buf, "{YDiceroll was %d{x\r\n", diceroll);
	send_to_char(buf, ch);
#endif

	if (diceroll == 0 || (diceroll != 100 && diceroll > hitchance)) {
	    // whiffed
	    tail_chain();
	    return;
	}
#ifdef DEBUG
	sprintf(buf, "{YExecuting attack %d\r\n", current_attack);
	send_to_char(buf, ch);
#endif

	// kick some ass 
	execute_attack(ch, current_attack, victim, wield, sn, skill,
		       dam_type, dt);

	if (ch->attack_stack[ch->current_attack] == -1
	    || ch->attack_stack[ch->current_attack++] == -1)
	    ch->current_attack = 0;

    }				//  while (GET_CWAIT (ch) +

#ifdef DEBUG
    sprintf(buf, "{YResetting CWAIT..\r\n");
    send_to_char(buf, ch);
#endif
    ch->cwait = 0;

    tail_chain();
    return;
}


/*
 * Inflict damage from a hit.
 */
bool damage(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
	    int dam_type, bool show)
{

    //char buf[MAX_STRING_LENGTH]; 
    char buf4[MAX_STRING_LENGTH];

    OBJ_DATA *corpse;
    bool immune;
    bool displaced = FALSE;	//displace spell

    if (victim->position == POS_DEAD)
	return FALSE;

    if (victim != ch) {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if (is_safe(ch, victim))
	    return FALSE;

	check_killer(ch, victim);

	if (victim->position > POS_STUNNED) {
	    if (!is_same_group(ch, victim)) {
		if (victim->fighting == NULL) {
		    if (ch->master) {
			int percent = number_range(1, 12);

			if (percent < 6) {
			    set_fighting(victim, ch->master);
			} else {
			    set_fighting(victim, ch);
			}
		    } else {
			set_fighting(victim, ch);
		    }
		    if (victim->timer <= 4)
			victim->position = POS_FIGHTING;
		}
	    }
	}
	if (victim->position > POS_STUNNED) {
	    if (!is_same_group(ch, victim)) {
		if (ch->fighting == NULL)
		    set_fighting(ch, victim);
	    }
	}

	/*
	 * More charm stuff.
	 */
	if (victim->master == ch)
	    stop_follower(victim);
    }

    /*
     * Inviso attacks ... not.
     */
    if (IS_AFFECTED(ch, AFF_INVISIBLE)) {
	affect_strip(ch, gsn_invis);
	affect_strip(ch, gsn_mass_invis);
	REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
	act("$n fades into existence.", ch, NULL, NULL, TO_ROOM);
	send_to_char("You fade into existence.\n\r", ch);
    }

    /*
     * Damage modifiers.
     */
    if IS_DWARF
	(victim) {
	if (dam > 1 && victim->pcdata->condition[COND_DRUNK] > 10)
	    dam -= dam / 5;
	if (dam < 0)
	    dam = 0;
	}

    if (dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY))
	dam = (3 * dam) / 4;

    if (dam > 1 && IS_AFFECTED(victim, AFF_HEAVENLY_SANCTUARY))
	dam = (23 * dam) / 25;

    if (dam > 1 && ((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch))
		    || (IS_AFFECTED(victim, AFF_PROTECT_GOOD)
			&& IS_GOOD(ch))))
	dam = (22 * dam) / 25;

    if (ch->clan) {
	if (ch->in_room->turf_clan && (ch->in_room->turf_clan == ch->clan)) {
	    dam += URANGE(0, (ch->in_room->turf_level / 10), 20);
	} else if (ch->in_room->turf_clan
		   && (ch->in_room->turf_clan != ch->clan)) {
	    dam -= URANGE(0, (ch->in_room->turf_level / 10), 20);
	}
    }

    if (dam < 0)
	dam = 0;

    immune = FALSE;

    /*
     * Check for parry, dodge, shield block, and displaced
     */
    if (dt >= TYPE_HIT && ch != victim && dt != gsn_whirl
	&& dt != gsn_circle) {

	if (check_parry(ch, victim))
	    return FALSE;
	if (check_dodge(ch, victim))
	    return FALSE;
	if (check_shield_block(ch, victim))
	    return FALSE;

	//Check if displaced
	if (is_affected(victim, skill_lookup("displace"))) {

	    if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

		//You're hitting the displaced version
		displaced = TRUE;
		if (IS_NPC(victim)
		    || !IS_SET(victim->act, PLR_AUTODEFENSE))
		    act("{y$n hits your displaced version.{x", ch, NULL,
			victim, TO_VICT);
		if (IS_NPC(ch) || !IS_SET(ch->act, PLR_AUTODEFENSE))
		    act("{yYour hit goes right through $N.{x", ch, NULL,
			victim, TO_CHAR);
		return FALSE;
	    }
	}
    }

    switch (check_immune(victim, dam_type)) {
    case (IS_IMMUNE):
	immune = TRUE;
	dam = 0;
	break;
    case (IS_RESISTANT):
	dam = (21 * dam) / 25;
	break;
    case (IS_VULNERABLE):
	dam = (113 * dam) / 100;
	break;
    }

    if (show)
	dam_message(ch, victim, dam, dt, immune);

    if (dam <= 0)
	return FALSE;


    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL
	&& victim->hit < 1)
	victim->hit = 1;

    if (IS_VAMPIRE(victim) && victim->hit < 1 && victim->blood > 5) {
	send_to_char("Your partial immortality has saved you!\n\r",
		     victim);
	act("$n's partial immortality has saved $m!", victim, NULL, NULL,
	    TO_ROOM);

	if ((victim->blood > 4) && (!DAYTIME)) {
	    victim->hit = victim->blood * 3;
	    victim->blood = 3;
	} else {
	    if ((victim->blood > 4) && (DAYTIME)) {
		victim->hit = victim->blood * 2;
		victim->blood = 3;
	    }
	}


    }

    update_pos(victim);

    switch (victim->position) {
    case POS_MORTAL:
	act("$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM);
	send_to_char
	    ("You are mortally wounded, and will die soon, if not aided.\n\r",
	     victim);
	break;

    case POS_INCAP:
	act("$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM);
	send_to_char
	    ("You are incapacitated and will slowly die, if not aided.\n\r",
	     victim);
	break;

    case POS_STUNNED:
	act("$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM);
	send_to_char("You are stunned, but will probably recover.\n\r",
		     victim);
	break;

    case POS_DEAD:
	act("{R$n is DEAD!!{x", victim, 0, 0, TO_ROOM);
	break;

    default:
	if (dam > victim->max_hit / 4)
	    send_to_char("{RThat really did HURT!{x\n\r", victim);
	if (victim->hit < victim->max_hit / 4) {
	    send_to_char("{RYou sure are BLEEDING!{x\n\r", victim);
	    create_blood_pool(victim);
	}
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (!IS_AWAKE(victim))
	stop_fighting(victim, FALSE);

    /*
     * Payoff for killing things.
     */

    // may kill imms in arena.
    if (victim->position == POS_DEAD
	|| (IS_IMMORTAL(victim) && (victim->hit < 2) && (victim->arena))) {

	// fake death in the arena, no corpse, no pain
	if (victim->arena == global_last_arena_id && ch->arena) {
	    stop_fighting(ch, FALSE);
	    stop_fighting(victim, FALSE);
	    sprintf(buf4,
		    "\r\n{c[{RARENA OF DEATH{c] %s just killed %s in the arena!{x",
		    ch->name, victim->name);
	    announce(buf4, ch);
	    arena_death(victim);

	    return FALSE;
	}

	group_gain(ch, victim);

	if (!IS_NPC(victim)) {
	    sprintf(log_buf, "%s killed by %s at %ld",
		    victim->name,
		    (IS_NPC(ch) ? ch->short_descr : ch->name),
		    ch->in_room->vnum);
	    log_string(log_buf);

	    /* For PKill, dying penalty is done in group_gain */
	    if (IS_NPC(ch)) {
		/*
		 * Dying penalty:
		 * 2/3 way back to previous level.
		 */
		gain_exp(victim, -250);
	    }
	    sprintf(log_buf, "%s has been killed by %s at %s [room %ld]",
		    (IS_NPC(victim) ? victim->short_descr : victim->name),
		    (IS_NPC(ch) ? ch->short_descr : ch->name),
		    ch->in_room->name, ch->in_room->vnum);
	}
	if (IS_NPC(victim))
	    wiznet(log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
	else
	    wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

	/*
	 * Death trigger
	 */

	/* RT new auto commands */

	if (!IS_NPC(ch)
	    && (corpse =
		get_obj_list(ch, "corpse", ch->in_room->contents)) != NULL
	    && corpse->item_type == ITEM_CORPSE_NPC
	    && can_see_obj(ch, corpse)) {
	    OBJ_DATA *coins;

	    corpse = get_obj_list(ch, "corpse", ch->in_room->contents);

	    if (IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains)	/* exists and not empty */
		do_get(ch, "all corpse");

	    if (IS_SET(ch->act, PLR_AUTOGOLD) && corpse && corpse->contains &&	/* exists and not empty */
		!IS_SET(ch->act, PLR_AUTOLOOT))
		if ((coins = get_obj_list(ch, "gcash", corpse->contains))
		    != NULL)
		    do_get(ch, "all.gcash corpse");

	    if (IS_SET(ch->act, PLR_AUTOSAC)) {
		if (IS_SET(ch->act, PLR_AUTOLOOT) && corpse
		    && corpse->contains)
		    return TRUE;	/* leave if corpse has treasure */
		else if (IS_VAMPIRE(ch)) {
		    do_feed(ch, "corpse");
		} else {
		    do_sacrifice(ch, "corpse");
		}
	    }
	}
	return TRUE;
    }
    if (victim == ch)
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if (!IS_NPC(victim) && victim->desc == NULL) {
	if (number_range(0, victim->wait) == 0) {
	    do_recall(victim, "");
	    return TRUE;
	}
    }
    /*
     * Wimp out?
     */
    if (IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2) {
	if ((IS_SET(victim->act, ACT_WIMPY) && number_bits(2) == 0
	     && victim->hit < victim->max_hit / 5)
	    || (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
		&& victim->master->in_room != victim->in_room))
	    do_flee(victim, "");
    }
    if (!IS_NPC(victim)
	&& victim->hit > 0
	&& victim->hit <= victim->wimpy
	&& victim->wait < PULSE_VIOLENCE / 2)
	do_flee(victim, "");

    tail_chain();
    return TRUE;
}


bool is_safe(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (IS_IMMORTAL(ch) || IS_IMMORTAL(victim))
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim)) {

	/* safe room? */
	if (IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
	    send_to_char("Not in this room.\n\r", ch);
	    return TRUE;
	}

	if (victim->pIndexData->pShop != NULL) {
	    send_to_char("No killing shopkeepers.\n\r", ch);
	    return TRUE;
	}
	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act, ACT_TRAIN)
	    || IS_SET(victim->act, ACT_PRACTICE)
	    || IS_SET(victim->act, ACT_REPAIR)
	    || IS_SET(victim->act, ACT_NO_ATTACK)
	    || IS_SET(victim->act, ACT_IS_HEALER)
	    || IS_SET(victim->act, ACT_BANKER)) {
	    send_to_char("No killing healers, trainers, etc.\n\r", ch);
	    return TRUE;
	}

	if (!IS_NPC(ch)) {
	    /* no pets */
	    if (IS_SET(victim->act, ACT_PET)) {
		act("But $N looks so cute and cuddly...",
		    ch, NULL, victim, TO_CHAR);
		return TRUE;
	    }

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim, AFF_CHARM) && (ch != victim->master)) {
		send_to_char("You don't own that monster.\n\r", ch);
		return TRUE;
	    }
	    if (IS_AFFECTED(ch, AFF_CHARM)
		&& IS_AFFECTED(victim, AFF_CHARM)) {
		return TRUE;
	    }
	}
    }
    /* killing players */
    else {
	/* NPC doing the killing */
	if (IS_NPC(ch)) {
	    /* safe room check */
	    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
		send_to_char("Not in this room.\n\r", ch);
		return TRUE;
	    }
	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL
		&& ch->master->fighting != victim) {
		send_to_char("Players are your friends!\n\r", ch);
		return TRUE;
	    }
	}
	/* player doing the killing */
	else {
	    /* Have to be a PKILL to kill */

	    if (!IS_SET(ch->act, PLR_PKILL) && (!ch->tmp_pkill)) {
		send_to_char
		    ("You must type {YPKILL{x twice before you can kill or steal from anyone.\n\r",
		     ch);
		return TRUE;
	    }
	    /* Not allowed killing NON-Pkillers */

	    if (!IS_SET(victim->act, PLR_PKILL) && (!victim->tmp_pkill)) {
		send_to_char("No killing non-pkillers.\n\r", ch);
		return TRUE;
	    }
	    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
		send_to_char("You're in a safe room.\n\r", ch);
		return TRUE;
	    }

	    if (!ch->tmp_pkill && !victim->tmp_pkill) {
		if (ch->level > victim->level + 25) {
		    send_to_char("Pick on someone your own size!\n\r", ch);
		    return TRUE;
		} else if (ch->level < victim->level - 25) {
		    send_to_char("Pick on someone your own size!\n\r", ch);
		    return TRUE;
		}
	    }
	}
    }
    return FALSE;
}

bool is_safe_spell(CHAR_DATA * ch, CHAR_DATA * victim, bool area)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;

    if (victim == ch && area)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL && !area)
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim)) {
	/* safe room? */
	if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
	    return TRUE;

	if (victim->pIndexData->pShop != NULL)
	    return TRUE;

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act, ACT_TRAIN)
	    || IS_SET(victim->act, ACT_PRACTICE)
	    || IS_SET(victim->act, ACT_IS_HEALER)
	    || IS_SET(victim->act, ACT_BANKER))
	    return TRUE;

	if (!IS_NPC(ch)) {
	    /* no pets */
	    if (IS_SET(victim->act, ACT_PET))
		return TRUE;

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim, AFF_CHARM)
		&& (area || ch != victim->master))
		return TRUE;

	    /* legal kill? -- cannot hit mob fighting non-group member */
	    if (victim->fighting != NULL
		&& !is_same_group(ch, victim->fighting))
		return TRUE;
	} else {
	    /* area effect spells do not hit other mobs */
	    if (area && !is_same_group(victim, ch->fighting))
		return TRUE;
	}
    }
    /* killing players */
    else {
	if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
	    return TRUE;

	/* NPC doing the killing */
	if (IS_NPC(ch)) {
	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL
		&& ch->master->fighting != victim)
		return TRUE;

	    /* safe room? */
	    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
		return TRUE;

	    /* legal kill? -- mobs only hit players grouped with opponent */
	    if (ch->fighting != NULL
		&& !is_same_group(ch->fighting, victim))
		return TRUE;
	}
	/* player doing the killing */
	else {
	    if (!IS_SET(ch->act, PLR_PKILL))
		return TRUE;

	    if (!IS_SET(victim->act, PLR_PKILL))
		return TRUE;

	    // PK range check
	    if ((IS_SET(ch->act, PLR_PKILL)
		 && (IS_SET(victim->act, PLR_PKILL))
		 && (victim->level < (ch->level - 25))))
		return TRUE;

	    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
		return TRUE;
	}

    }
    return FALSE;
}

/*
 * See if an attack justifies a KILLER flag.
 */

void check_killer(CHAR_DATA * ch, CHAR_DATA * victim)
{
    /* This function is currenly unused. Taken out by Bloodbath. */
    return;
}



/*
 * Check for parry.
 */
bool check_parry(CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE(victim))
	return FALSE;

    if (get_skill(victim, gsn_parry) == 0)
	return FALSE;

    /*Mobs don't need weapons to parry anymore.  Only
       players do.  Severhand */
    if (victim->pIndexData == NULL)
	if (get_eq_char(victim, WEAR_WIELD) == NULL) {
	    return FALSE;
	}

    chance = get_skill(victim, gsn_parry);
    if ((!can_see(ch, victim)) && (!IS_NPC(ch)))
	chance *= 2;

    if (!can_see(ch, victim))
	chance = (chance * 3) / (chance * 2);
    if (!can_see(victim, ch))
	chance /= 2;
    if (number_percent() >= ((chance - 60) / 2))
	return FALSE;

    /*if (number_percent() >= chance + victim->level - ch->level)
       return FALSE; */

    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_AUTODEFENSE))
	act("{yYou parry $n's attack.{x", ch, NULL, victim, TO_VICT);

    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_AUTODEFENSE))
	act("{y$N parries your attack.{x", ch, NULL, victim, TO_CHAR);

    check_improve(victim, gsn_parry, TRUE, 6);
    return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block(CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE(victim))
	return FALSE;

    if (get_skill(victim, gsn_shield_block) == 0)
	return FALSE;

    chance = get_skill(victim, gsn_shield_block);
    if ((!can_see(ch, victim)) && (!IS_NPC(ch)))
	chance *= 2;

    if (!can_see(ch, victim))
	chance *= 1, 5;

    if (!can_see(victim, ch))
	chance /= 2;

    chance += get_curr_stat(victim, STAT_STR);
    chance -= get_curr_stat(ch, STAT_STR);


    if (get_eq_char(victim, WEAR_SHIELD) == NULL)
	return FALSE;

    if (number_percent() >= ((chance - 40) / 2))
	return FALSE;
/*    if (number_percent() >= chance + victim->level - ch->level)
	return FALSE;*/

    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_AUTODEFENSE))
	act("{yYou block $n's attack with your shield.{x", ch, NULL,
	    victim, TO_VICT);

    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_AUTODEFENSE))
	act("{y$N blocks your attack with a shield.{x", ch, NULL, victim,
	    TO_CHAR);

    check_improve(victim, gsn_shield_block, TRUE, 6);
    return TRUE;
}


/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE(victim))
	return FALSE;

    if (get_skill(victim, gsn_dodge) == 0)
	return FALSE;

    chance = get_skill(victim, gsn_dodge);

    if ((!can_see(ch, victim)) && (!IS_NPC(ch)))
	chance *= 2;

    if (!can_see(ch, victim))
	chance *= 1, 5;

    if (!can_see(victim, ch))
	chance /= 2;

    chance -= get_curr_stat(ch, STAT_DEX);
    chance += get_curr_stat(victim, STAT_DEX);

    if (number_percent() >= ((chance - 60) / 2))
	return FALSE;
/*    if (number_percent() >= chance + victim->level - ch->level)
		return FALSE;*/

    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_AUTODEFENSE))
	act("{yYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT);

    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_AUTODEFENSE))
	act("{y$N dodges your attack.{x", ch, NULL, victim, TO_CHAR);

    check_improve(victim, gsn_dodge, TRUE, 6);
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos(CHAR_DATA * victim)
{
    if (victim->hit > 0) {
	if (victim->position <= POS_STUNNED)
	    victim->position = POS_STANDING;
	return;
    }
    if ((victim->desc != NULL) && (victim->position == POS_MORPHED)) {
	victim->position = POS_STANDING;
	return;
    }
    if (IS_NPC(victim) && victim->hit < 1) {
	victim->position = POS_DEAD;
	return;
    }
    if (victim->hit <= -11) {
	victim->position = POS_DEAD;
	return;
    }
    if (victim->hit <= -6)
	victim->position = POS_MORTAL;
    else if (victim->hit <= -3)
	victim->position = POS_INCAP;
    else
	victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (ch->fighting != NULL) {
	bug("Set_fighting: already fighting", 0);
	return;
    }
    if (IS_AFFECTED(ch, AFF_SLEEP))
	affect_strip(ch, gsn_sleep);

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



void stop_fighting(CHAR_DATA * ch, bool fBoth)
{
    CHAR_DATA *fch;

    for (fch = char_list; fch != NULL; fch = fch->next) {
	if (fch == ch || (fBoth && fch->fighting == ch)) {
	    fch->fighting = NULL;
	    fch->position = IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	    update_pos(fch);
	}
    }

    return;
}



/*
 * Take 1 point from the highest modified stat on death.
 * Reset char's affs and whatnot. 
 * Called by raw_kill and kill_self.
 */
void death_penalty(CHAR_DATA * ch)
{
    int h, i, j = 0, highstat = 0;
    if (IS_NPC(ch)) {
	ch->pIndexData->killed++;
	kill_table[URANGE(0, ch->level, MAX_LEVEL - 1)].killed++;
	extract_char(ch, TRUE);
	return;
    }
    ch->pcdata->deaths++;	/* Increase deaths by 1 */

    if (ch->pcdata->deaths % 10 == 0) {
	for (h = 0; h <= (ch->level / 50); h++) {
	    for (i = 0; i < MAX_STATS; i++) {
		if (ch->perm_stat[i] > highstat) {
		    highstat = ch->perm_stat[i];
		    j = i;
		}
	    }
	}
	ch->perm_stat[j] -= 1;	/*Decrease Stat by 1 */
    }

    extract_char(ch, FALSE);

    while (ch->affected)
	affect_remove(ch, ch->affected);

    ch->affected_by = ch->race->aff;

    for (i = 0; i < 4; i++)
	ch->armor[i] = 100;

    ch->position = POS_RESTING;
    ch->hit = 1;
    ch->mana = 1;
    ch->move = 1;

    if (IS_VAMPIRE(ch))
	ch->blood = UMAX(10, ch->blood);

    if (ch->max_mana > 10)
	ch->max_mana -= 3;
    if (ch->max_move > 10)
	ch->max_move -= 3;
    if (ch->max_hit > 10)
	ch->max_hit -= 3;

    if (!IS_WRAITH(ch) && !IS_VAMPIRE(ch)) {
	ch->pcdata->condition[COND_HUNGER] = 12;
	ch->pcdata->condition[COND_THIRST] = 12;
    }
    ch->pcdata->condition[COND_DRUNK] = 0;
    ch->move = ch->max_move / 75;
    ch->mana = ch->max_mana / 75;
    if ((ch->clan == 2) || (ch->clan == 9) || (ch->clan == 10)
	|| (ch->clan == 7) || (ch->clan == 4) || (ch->clan == 3)) {
	SET_BIT(ch->comm, PLR_PKILL);
    }
    return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse(CHAR_DATA * ch, bool can_loot)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if (IS_NPC(ch)) {
	name = ch->short_descr;
	corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer = number_range(3, 6);
	if (ch->gold > 0) {
	    obj_to_obj(create_money(ch->gold), corpse);
	    ch->gold = 0;
	}
	corpse->cost = 0;
    } else {
	name = ch->name;
	corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer = number_range(25, 40);
	if (!can_loot)
	    REMOVE_BIT(ch->act, PLR_CANLOOT);
	if (!IS_SET(ch->act, PLR_PKILL)) {
	    corpse->owner = str_dup(ch->name);
	    corpse->oname = str_dup(ch->name);
	} else {
	    corpse->owner = NULL;
	    corpse->oname = str_dup(ch->name);
	    if (ch->gold > 1) {
		obj_to_obj(create_money(ch->gold / 2), corpse);
		ch->gold -= ch->gold / 2;
	    }
	}

	corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf(buf, corpse->short_descr, name);
    free_string(corpse->short_descr);
    corpse->short_descr = str_dup(buf);

    sprintf(buf, corpse->description, name);
    free_string(corpse->description);
    corpse->description = str_dup(buf);

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
	bool floating = FALSE;

	obj_next = obj->next_content;

	if (obj->wear_loc == WEAR_FLOAT)
	    floating = TRUE;

	obj_from_char(obj);

	if (obj->item_type == ITEM_POTION)
	    obj->timer = number_range(500, 1000);

	if (obj->item_type == ITEM_SCROLL)
	    obj->timer = number_range(1000, 2500);

	if (IS_SET(obj->extra_flags, ITEM_ROT_DEATH) && !floating) {
	    obj->timer = number_range(5, 10);
	    REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
	}

	REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

	if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
	    extract_obj(obj);

	else if (floating) {
	    if (IS_OBJ_STAT(obj, ITEM_ROT_DEATH)) {	/* get rid of it! */
		if (obj->contains != NULL) {
		    OBJ_DATA *in, *in_next;
		    act("$p evaporates,scattering its contents.",
			ch, obj, NULL, TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next) {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in, ch->in_room);
		    }
		} else
		    // floating, rot death, not a container
		    act("$p evaporates.", ch, obj, NULL, TO_ROOM);
		extract_obj(obj);
	    } else {
		// floating, not rot death
		act("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
		obj_to_room(obj, ch->in_room);
	    }
	    // not floating, not rot death
	} else {
	    obj_to_obj(obj, corpse);
/*
        if (obj->bound) {
          obj_to_char(obj, ch);
        } else
*/
	}

    }				// end for loop

    obj_to_room(corpse, ch->in_room);
    return;
}

void death_cry(CHAR_DATA * ch, CHAR_DATA * victim)
{

    ROOM_INDEX_DATA *was_in_room;
    char buf[MAX_STRING_LENGTH];
    char *msg;
    char *msg2;
    char *msg3;
    int door, i;
    long vnum[4];
    int number;

    if (ch == victim)
	return;

    buf[0] = '\0';

/*
 *   msg  goes only to the killer
 *   msg2 goes only to the victim
 *   msg3 goes to everyone else

 act ($p =  weapon
 act $n = victim
 msg $N = victim
 msg $n = agressor
 */

    vnum[0] = 0;
    vnum[1] = 0;
    vnum[2] = 0;
    vnum[3] = 0;
    msg = "You grab $N's neck and twist it sharply. *SNAP*";
    msg2 = "$n grabs your neck and gives it a sharp twist! *SNAP*";
    msg3 = "$n grabs $N's neck and gives it a sharp twist. *SNAP*";


    number = number_range(0, 8);

    // vorpal weapons always yield heads. ;)
    if (get_eq_char(ch, WEAR_WIELD)
	&& IS_WEAPON_STAT(get_eq_char(ch, WEAR_WIELD), WEAPON_VORPAL))
	number = 3;

    switch (number) {

    case 0:
	msg =
	    "Gurgling blood, $N falls to the ground and spasms violently.";
	msg2 =
	    "Gurgling blood, you fall to the ground and spasm violently!";
	msg3 =
	    "Gurgling blood, $N falls to the ground and spasms violently.";

	break;

    case 1:
	if (IS_SET(victim->parts, PART_GUTS)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"Slicing $N's stomach open, you watch as $E trips over $S own guts.";
	    msg2 =
		"$n slices your stomach open, and you trip over your own intestines!";
	    msg3 =
		"$n slices $N's stomach open, causing $M to trip over $S own intestines.";
	    vnum[0] = OBJ_VNUM_GUTS;
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg =
		"Thrusting your dagger through $N's stomach, you unmercifully tear $S guts from $S body.";
	    msg2 =
		"$n thrusts $s dagger into your stomach, wrapping your slimy guts around $s blade.";
	    msg3 =
		"Intestines spill on the ground as $n's dagger pierces $N one final time.";
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg = "Hacking open $N's stomach, you chop at $S guts.";
	    msg2 = "$n savagely rips your stomach open with $s axe!";
	    msg3 =
		"$n hacks into $N's midsection, and cracked ribs pop out amid bloody ribbons of tissue.";
	    vnum[0] = OBJ_VNUM_GUTS;
	    vnum[1] = OBJ_VNUM_TORN_HEART;
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg =
		"Your whip easily slices into $N's stomach, and $S guts slide from $S body.";
	    msg2 =
		"The head of $n's whip carves an arc deep into your stomach, blood oozing in its wake!";
	    msg3 =
		"$n carves out $N's guts with a graceful flick of $s whip.";
	    vnum[0] = OBJ_VNUM_GUTS;
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg =
		"Your polearm thrusts into $N's stomach, exposing $S insides to the open air.";
	    msg2 =
		"$n brings $s polearm down into your stomach, flinging your guts into the air as it exits.";
	    msg3 =
		"$n brings $s polearm down into $N's stomach, and guts explode from $N's body.";
	    vnum[1] = OBJ_VNUM_TORN_HEART;
	    vnum[0] = OBJ_VNUM_GUTS;
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg =
		"Thrusting your sharp spear into $N's stomach, you force $S entrails from $S body.";
	    msg2 =
		"A single thrust of $n's spear guts you, and your entrails spill out of the ragged wound.";
	    msg3 =
		"A single thrust of $n's spear carves a hole in $N's stomach, spilling $S guts down $S legs.";
	    vnum[0] = OBJ_VNUM_GUTS;
	} else {
	    msg =
		"Ripping $N's stomach open, you watch as $E trips over $S own guts.";
	    msg2 =
		"$n rips your stomach open, and you trip over your own guts!";
	    msg3 =
		"$n rips $N stomach open, causing $N to trip over $S own guts.";
	    vnum[0] = OBJ_VNUM_GUTS;
	}
	break;

    case 2:
	if (IS_SET(victim->parts, PART_GUTS)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"With one mighty swipe of your sword, you sever $N's spine and $S guts spill from both halves.";
	    msg2 =
		"$n's cold blade slices you in half, and your warm, frothy guts pour down over your body!";
	    msg3 =
		"$n slices $N in half, causing $N to trip over $S own guts.";
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg = "Piercing $N's stomach, $S entrails are visable.";
	    msg2 =
		"$n pierces your stomach, you die staring in awe of them!";
	    msg3 =
		"$n pierces $N stomach, $N dies while they are exposed from $S body.";
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg = "Hacking $N's stomach, $S entrails are exposed.";
	    msg2 = "$n opens your stomach!";
	    msg3 =
		"$n pierces $N stomach, $N dies watching $S innards leave $S body.";
	    vnum[0] = OBJ_VNUM_GUTS;
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg = "Whipping $N's stomach, $S entrails are exposed.";
	    msg2 = "$n opens your stomach!";
	    msg3 = "$n's whip opens $N's torso!";
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg =
		"$n opens $N's stomach with a long stroke from $S polearm forcing $S guts from $S body.";
	    msg2 = "$n opens your stomach!";
	    msg3 = "$n opens $N's stomach with his polearm.";
	    vnum[0] = OBJ_VNUM_GUTS;
	} else if (IS_SET(victim->parts, PART_GUTS)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg = "Thrusting at $N's stomach you expose $N's entrails.";
	    msg2 =
		"$n thrusts $S spear into your stomach, exposing your entrails.";
	    msg3 = "$n thrusts $S spear deep into $N's stomach.";
	} else {
	    msg = "As you pound $N, $S entrails become visable.";
	    msg2 = "$n's pounding rips you open!";
	    msg3 = "$n rips $N open and exposes $S entrails.";
	}
	break;

    case 3:
	if (IS_SET(victim->parts, PART_HEAD)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"$n connects $s sword just under $N's head, instantly decapitating $M.";
	    msg2 =
		"$n's sword connects with your neck, instantly decapitating you!";
	    msg3 = "$n's sword decapitates $N, sending blood everywhere!";
	    vnum[0] = OBJ_VNUM_SEVERED_HEAD;
	} else if (IS_SET(victim->parts, PART_HEAD)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg = "You dig deep into $N's head and dislodge an eye!";
	    msg2 = "$n's plundges $S dagger deep into your eye!";
	    msg3 =
		"$n's dagger plunges deep into $N's head, poping out $S eye.";
	    vnum[0] = OBJ_VNUM_EYE;
	    vnum[1] = OBJ_VNUM_SEVERED_HEAD;
	} else if (IS_SET(victim->parts, PART_HEAD)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg =
		"Your hack off $N's ear, then swing around, instantly decapitating $M.";
	    msg2 =
		"$n's axe hacks off your ear, swings around, and decapitates you!";
	    msg3 =
		"$n's axe connects with $N's head with explosive force! Body parts fly everywhere!";
	    vnum[0] = OBJ_VNUM_EAR;
	    vnum[1] = OBJ_VNUM_EYE;
	    vnum[2] = OBJ_VNUM_SEVERED_HEAD;
	} else if (IS_SET(victim->parts, PART_HEAD)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg =
		"Your whip wraps tightly around $N's neck, gruesomely pulling it from $S body.";
	    msg2 =
		"$n's whip wraps around your neck and rips your head off!";
	    msg3 =
		"$n's whip wraps $N's neck, $n yanks, and messily decapitates $M.";
	    vnum[0] = OBJ_VNUM_SEVERED_HEAD;
	} else if (IS_SET(victim->parts, PART_HEAD)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg =
		"Your polearm connects with $N's neck, instantly decapitating $M.";
	    msg2 =
		"$n's polearm connects with your neck, instantly decapitating you!";
	    msg3 =
		"$n's polearm connects with $N's neck, instantly decapitating $M.";
	    vnum[0] = OBJ_VNUM_SEVERED_HEAD;
	    vnum[1] = OBJ_VNUM_EAR;
	} else if (IS_SET(victim->parts, PART_HEAD)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg = "Your thrust separates $N's neck from his head.";
	    msg2 = "$n's thrust separates $S neck from $S head!";
	    msg3 = "$n's thrust takes $N's head, from $M neck.";
	    vnum[0] = OBJ_VNUM_SEVERED_HEAD;
	} else {
	    msg =
		"You knock $N's head off! $S head plops onto the ground.";
	    msg2 =
		"$n knocks your head off! Your head plops onto the ground.";
	    msg3 =
		"$n knocks $N's head off! $S head plops onto the ground.";
	    vnum[0] = OBJ_VNUM_SEVERED_HEAD;
	    vnum[1] = OBJ_VNUM_EYE;
	}

	break;

    case 4:
	if (IS_SET(victim->parts, PART_HEART)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"Your intricate swordwork cuts $N's heart from $S chest.";
	    msg2 = "$n sword cuts your heart from your chest!";
	    msg3 = "$n cuts $N's heart from $S chest.";
	    vnum[0] = OBJ_VNUM_TORN_HEART;
	} else if (IS_SET(victim->parts, PART_HEART)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg =
		"You plunge your dagger deep into $N's heart, extinguishing $S life.";
	    msg2 =
		"$n pokes $s dagger into your chest, penetrating your heart!";
	    msg3 = "$n stabs $s dagger deep into $N's chest!";
	} else if (IS_SET(victim->parts, PART_HEART)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg = "Your axe tears $N's heart and entrails from $S torso.";
	    msg2 = "$n tears your guts from your torso!";
	    msg3 = "$n tears $N's heart and entrails from $S torso.";
	    vnum[0] = OBJ_VNUM_TORN_HEART;
	    vnum[1] = OBJ_VNUM_GUTS;
	} else if (IS_SET(victim->parts, PART_HEART)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg =
		"You send your whip into $N's chest and tear out $S heart.";
	    msg2 = "$n tears your heart from your chest!";
	    msg3 = "$n's tears into $N's chest, tearing out $S heart.";
	    vnum[0] = OBJ_VNUM_TORN_HEART;
	} else if (IS_SET(victim->parts, PART_HEART)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg = "You hack $N's insides from $S torso.";
	    msg2 = "$n hacks your heart and entrails from your torso!";
	    msg3 =
		"$n's polearm hacks $N's heart and entrails from $S torso.";
	    vnum[0] = OBJ_VNUM_GUTS;
	    vnum[1] = OBJ_VNUM_TORN_HEART;
	} else if (IS_SET(victim->parts, PART_HEART)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg =
		"Using labyrinthine thrusts, you remove $N's heart from $S torso.";
	    msg2 =
		"$n removes your heart from your chest with a series of complex thrusts!";
	    msg3 =
		"$n removes $N's heart from $S chest after a series of complex thrusts.";
	    vnum[0] = OBJ_VNUM_TORN_HEART;
	} else {
	    msg =
		"You tear $N's heart from $S chest and crush it in your hand.";
	    msg2 =
		"$n tears your heart from your chest and crushes it in $s hand!";
	    msg3 =
		"$n tears $N's heart from $S chest and crushes it in $s hand.";
	    vnum[0] = OBJ_VNUM_TORN_HEART;
	}
	break;

    case 5:
	if (IS_SET(victim->parts, PART_ARMS)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"Savagely, you slice $N's arm and watch it fall from $S corpse.";
	    msg2 = "Savagely, $n slices your arm off!";
	    msg3 = "Savagely, $n cuts $N's arm off.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	} else if (IS_SET(victim->parts, PART_ARMS)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg =
		"Savagely, you cut into some of $N's fingers as $S raises his arm in defense.";
	    msg2 =
		"Raising your hand in defense $n cuts into some of your fingers!";
	    msg3 =
		"$N raises $S arm in defense, $n cuts into some of $S fingers.";
	} else if (IS_SET(victim->parts, PART_ARMS)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg = "Savagely, you hack $N's arm off!";
	    msg2 = "Savagely, $n hacks your arm off!";
	    msg3 = "Savagely, $n hacks $N's arm off.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	} else if (IS_SET(victim->parts, PART_ARMS)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg =
		"Savagely, you rip $N's arm from $S torso off with your whip.";
	    msg2 = "Savagely, $n rips your arm off with $s whip!";
	    msg3 = "Savagely, $n rips $N's arm off with $s whip.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	} else if (IS_SET(victim->parts, PART_ARMS)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg = "Savagely, you separate $N's arm from $S torso.";
	    msg2 = "Savagely, $n separates your arm from your torso!";
	    msg3 = "Savagely, $n separates $N's arm from his torso.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	} else if (IS_SET(victim->parts, PART_ARMS)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg =
		"Savagely, you thrust your spear between $N's arm and $S torso.";
	    msg2 =
		"Savagely, $n thrusts {s spear between your arm and torso!";
	    msg3 =
		"Savagely, $n thrusts {s spear between $N's arm and $S torso.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	} else {
	    msg = "Savagely, you rip $N's arm from $S corpse.";
	    msg2 = "Savagely, $n rips your arm off!";
	    msg3 = "Savagely, $n rips $N's arm off.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	}
	break;

    case 6:
	if (IS_SET(victim->parts, PART_LEGS)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg = "You slice $N's leg off with your sword.";
	    msg2 = "$n slices your leg off with $s sword!";
	    msg3 = "$n slices $N's leg with $s sword.";
	    vnum[0] = OBJ_VNUM_SLICED_LEG;
	} else if (IS_SET(victim->parts, PART_LEGS)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg = "You stab at $N's legs as $E dies.";
	    msg2 = "$n stabs into your legs with $s dagger!";
	    msg3 = "$n stabs into $N's legs with $s dagger as $N dies.";
	} else if (IS_SET(victim->parts, PART_LEGS)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg = "You hack both of $N's legs off with your axe!";
	    msg2 = "$n hacks your legs off with $s axe!";
	    msg3 = "$n hacks $N's legs off with $s axe.";
	    vnum[0] = OBJ_VNUM_SLICED_LEG;
	    vnum[1] = OBJ_VNUM_SLICED_LEG;
	} else if (IS_SET(victim->parts, PART_LEGS)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg = "You wrap your whip around $N's leg, and rip it off!";
	    msg2 = "$n wraps his whip around your leg and rips it off!";
	    msg3 = "$n wraps $s whip around $N's leg, ripping it off.";
	    vnum[0] = OBJ_VNUM_SLICED_LEG;
	} else if (IS_SET(victim->parts, PART_LEGS)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg =
		"Your sweeping polearm takes $N's legs from $S fresh corpse.";
	    msg2 = "$n hacks your legs from your body!";
	    msg3 = "$n's polearm hacks $N's leg from $S body.";
	    vnum[0] = OBJ_VNUM_SLICED_LEG;
	    vnum[1] = OBJ_VNUM_SLICED_LEG;
	} else if (IS_SET(victim->parts, PART_LEGS)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg = "Your thrust snaps $N's leg off.";
	    msg2 = "$n snaps your leg off with $s spear!";
	    msg3 = "$n spear separates $N's leg from $S body.";
	    vnum[0] = OBJ_VNUM_SLICED_LEG;
	} else {
	    msg = "You knock $N's leg off and stuff it down $S throat.";
	    msg2 =
		"$n knocks your leg off and stuffs it down your throat!";
	    msg3 = "$n knocks $N's leg off and stuffs it down $S throat.";
	    vnum[0] = OBJ_VNUM_SLICED_LEG;
	}
	break;

    case 7:
	if (IS_SET(victim->parts, PART_BRAINS)
	    && get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"Your slice cuts $N's head wide open, and $S brains spill out.";
	    msg2 =
		"$n's slice splits your head wide open, and your brains spill out!";
	    msg3 =
		"$n's slice splits $N's head wide open, and $S brains spill out.";
	    vnum[0] = OBJ_VNUM_BRAINS;
	} else if (IS_SET(victim->parts, PART_BRAINS)
		   && get_weapon_sn(ch) == gsn_dagger) {
	    msg =
		"Your stab cracks $N's head wide open, and $S brains spill out.";
	    msg2 =
		"$n's stab cracks your head wide open, and your brains spill out!";
	    msg3 =
		"$n's stab cracks $N's head wide open, and $S brains spill out.";
	    vnum[0] = OBJ_VNUM_BRAINS;
	} else if (IS_SET(victim->parts, PART_BRAINS)
		   && get_weapon_sn(ch) == gsn_axe) {
	    msg = "Your hack splits $N's head wide open.";
	    msg2 = "$n's hack splits your head wide open!";
	    msg3 = "$n's hack splits $N's head wide open.";
	    vnum[0] = OBJ_VNUM_EYE;
	    vnum[1] = OBJ_VNUM_EAR;
	    vnum[2] = OBJ_VNUM_BRAINS;
	} else if (IS_SET(victim->parts, PART_BRAINS)
		   && get_weapon_sn(ch) == gsn_whip) {
	    msg =
		"Your whip splits $N's head wide open, and $S brains spill out.";
	    msg2 =
		"$n's whip splits your head wide open, and your brains spill out!";
	    msg3 =
		"$n's whip splits $N's head wide open, and $S brains spill out.";
	    vnum[0] = OBJ_VNUM_BRAINS;
	} else if (IS_SET(victim->parts, PART_BRAINS)
		   && get_weapon_sn(ch) == gsn_polearm) {
	    msg = "Your slice splits $N's head wide open.";
	    msg2 = "$n's blow splits your head wide open!";
	    msg3 = "$n's polearm splits $N's head wide open.";
	    vnum[0] = OBJ_VNUM_EYE;
	    vnum[1] = OBJ_VNUM_BRAINS;
	} else if (IS_SET(victim->parts, PART_BRAINS)
		   && get_weapon_sn(ch) == gsn_spear) {
	    msg = "Your thrust takes off the top of $N's head.";
	    msg2 = "$n's thrust take off the top of your head!";
	    msg3 = "$n's thrust takes off the top of $N's head.";
	    vnum[0] = OBJ_VNUM_BRAINS;
	} else {
	    msg =
		"Your blow splits $N's head wide open, and $S brains spill out.";
	    msg2 =
		"$n's blow splits your head wide open, and your brains spill out!";
	    msg3 =
		"$n's blow splits $N's head wide open, and $S brains spill out.";
	    vnum[0] = OBJ_VNUM_BRAINS;
	}
	break;

    case 8:
	if (get_weapon_sn(ch) == gsn_sword) {
	    msg =
		"Dropping to a crouch, you drive your blade upward, impaling $N.";
	    msg2 =
		"Dropping to a crouch, $n drives $s blade upward, impaling you!";
	    msg3 =
		"Dropping to a crouch, $n drives $s blade upward, impaling $N.";
	} else if (get_weapon_sn(ch) == gsn_dagger) {
	    msg =
		"Dropping to a crouch, you drive your dagger up into $N.";
	    msg2 =
		"Dropping to a crouch, $n drives $s dagger deep into you!";
	    msg3 = "Dropping to a crouch, $n drives $s dagger into $N.";
	} else if (get_weapon_sn(ch) == gsn_axe) {
	    msg =
		"Dropping to a crouch, your axe sends parts of $N everywhere.";
	    msg2 =
		"Dropping to a crouch, $n sends parts of you everywhere!";
	    msg3 =
		"Dropping to a crouch, $n sends parts of $N everywhere.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	    vnum[1] = OBJ_VNUM_EAR;
	    vnum[2] = OBJ_VNUM_BRAINS;
	    vnum[3] = OBJ_VNUM_SLICED_LEG;
	} else if (get_weapon_sn(ch) == gsn_whip) {
	    msg = "Backing up, you strink deep into $N.";
	    msg2 = "Backing up, $n strikes $s whip deep into you!";
	    msg3 = "Backing up, $n strikes his whip deep into $N.";
	} else if (get_weapon_sn(ch) == gsn_polearm) {
	    msg =
		"Dropping to a crouch, your polearm send parts of $N everywhere.";
	    msg2 =
		"Dropping to a crouch, $n sends parts of you everywhere!";
	    msg3 =
		"Dropping to a crouch, $n sends parts of $N everywhere.";
	    vnum[0] = OBJ_VNUM_SLICED_ARM;
	    vnum[1] = OBJ_VNUM_EYE;
	    vnum[2] = OBJ_VNUM_SEVERED_HEAD;
	    vnum[3] = OBJ_VNUM_SLICED_LEG;
	} else if (get_weapon_sn(ch) == gsn_spear) {
	    msg =
		"Dropping to a crouch, you drive your spear upward, impaling $N.";
	    msg2 =
		"Dropping to a crouch, $n drives $s spear upward, impaling you!";
	    msg3 =
		"Dropping to a crouch, $n drives $s spear upward, impaling $N.";
	} else {
	    msg = "Dropping to a crouch, you swing upward, destroying $N.";
	    msg2 =
		"Dropping to a crouch, $n swings upward, destroying you!";
	    msg3 =
		"Dropping to a crouch, $n swings upward, destroying $N.";
	    vnum[0] = OBJ_VNUM_EAR;
	    vnum[1] = OBJ_VNUM_EYE;
	    vnum[2] = OBJ_VNUM_SEVERED_HEAD;
	    vnum[3] = OBJ_VNUM_SLICED_LEG;
	}

	break;
    }

    if (!IS_NPC(victim) && victim->pcdata->bounty > 0) {
	msg =
	    "You smash $N's head off! $S head plops onto the ground...perhaps there's a reward.";
	msg2 =
	    "$n smashes your head off! Your head plops onto the ground.";
	msg3 = "$n smashes $N's head off! $S head plops onto the ground.";
	vnum[0] = OBJ_VNUM_SEVERED_HEAD;
    }
    act_new(msg, ch, NULL, victim, TO_CHAR, POS_RESTING);	/* Only to Killer */
    act_new(msg2, ch, NULL, victim, TO_VICT, POS_DEAD);	/* Only to victim */
    act_new(msg3, ch, NULL, victim, TO_NOTVICT, POS_RESTING);	/* Everyone else  */


    for (i = 0; i < 4; i++) {
	if (vnum[i] != 0) {
	    EXTRA_DESCR_DATA *ed;
	    OBJ_DATA *obj;
	    char *name;

	    name = IS_NPC(victim) ? victim->short_descr : victim->name;
	    obj = create_object(get_obj_index(vnum[i]), 0);

	    if (!IS_NPC(victim) && victim->pcdata->bounty > 0)
		obj->timer = 0;
	    else
		obj->timer = number_range(10, 20);

	    sprintf(buf, obj->short_descr, name);
	    free_string(obj->short_descr);
	    obj->short_descr = str_dup(buf);

	    sprintf(buf, obj->description, name);
	    free_string(obj->description);
	    obj->description = str_dup(buf);

	    if (!IS_NPC(victim) && victim->pcdata->bounty > 0) {
		sprintf(buf,
			"{MTake this to %s and get a bounty of %d gold!{x\n\r",
			victim->pcdata->bountier, victim->pcdata->bounty);

		ed = new_extra_descr();

		ed->keyword = str_dup("head");
		ed->description = str_dup(buf);
		ed->next = obj->extra_descr;
		obj->extra_descr = ed;

		victim->pcdata->bounty = 0;
		free_string(victim->pcdata->bountier);
		victim->pcdata->bountier = str_dup("");
	    }

	    if (obj->item_type == ITEM_FOOD) {
		if (IS_SET(victim->form, FORM_POISON))
		    obj->value[3] = 1;
		else if (!IS_SET(ch->form, FORM_EDIBLE))
		    obj->item_type = ITEM_TRASH;
	    }

	    if (obj->item_type == ITEM_PILL)
		obj->value[0] = (obj->level / 2) + 1;

	    obj_to_room(obj, victim->in_room);
	}
    }

    if (IS_NPC(victim))
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for (door = 0; door <= 5; door++) {
	EXIT_DATA *pexit;

	if ((pexit = was_in_room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && pexit->u1.to_room != was_in_room) {
	    ch->in_room = pexit->u1.to_room;
	    act(msg, ch, NULL, victim, TO_NOTVICT);
	}
    }

    ch->in_room = was_in_room;

    buf[0] = '\0';
    return;
}

void raw_kill(CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf4[MSL];

    stop_fighting(victim, TRUE);

    if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_DEATH)) {
	victim->position = POS_STANDING;
	mp_percent_trigger(victim, ch, NULL, NULL, TRIG_DEATH);
    }
    death_cry(ch, victim);

    // fake death in the arena, no corpse, no pain
    if (victim->arena == global_last_arena_id && ch->arena) {
	stop_fighting(ch, FALSE);
	stop_fighting(victim, FALSE);
	sprintf(buf4,
		"{c[{RARENA OF DEATH{c] %s just killed %s in the arena!{x",
		ch->name, victim->name);
	announce(buf4, ch);
	arena_death(victim);

	return;
    }


    /* For the Announce Channel */

    if (!IS_NPC(victim)) {
	DESCRIPTOR_DATA *d;

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *wch;

	    wch = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character != victim
		&& IS_SET(wch->comm, COMM_ANNOUNCE)
		&& !IS_SET(wch->comm, COMM_QUIET)) {
		sprintf(log_buf,
			"{c[{RK{rI{RL{rL{RE{rD{c] %s forces $n into Death's willing embrace!{x",
			capitalize(PERS(ch, d->character)));

		act_new(log_buf, victim, NULL, d->character, TO_VICT,
			POS_DEAD);
	    }
	}
    }

    make_corpse(victim, IS_NPC(ch) ? FALSE : TRUE);
    death_penalty(victim);

    return;
}


void kill_self(CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;

    stop_fighting(ch, TRUE);

    send_to_char("{RYou have DIED!!{x\n\r\n\r", ch);

    if (IS_NPC(ch))
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;

    for (door = 0; door <= 5; door++) {
	EXIT_DATA *pexit;

	if ((pexit = was_in_room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && pexit->u1.to_room != was_in_room) {
	    ch->in_room = pexit->u1.to_room;
	    act(msg, ch, NULL, NULL, TO_ROOM);
	}
    }

    ch->in_room = was_in_room;

    /* For the Announce Channel */

    if (!IS_NPC(ch)) {
	DESCRIPTOR_DATA *d;

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *wch;

	    wch = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING
		&& d->character != ch && IS_SET(wch->comm, COMM_ANNOUNCE)
		&& !IS_SET(wch->comm, COMM_QUIET)) {
		act_new("{c[{RANNOUNCE{c] $n has killed $mself!{x", ch,
			NULL, d->character, TO_VICT, POS_DEAD);
	    }
	}
    }

    make_corpse(ch, FALSE);
    death_penalty(ch);

    return;
}


void group_gain(CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;
    int high_level;


    if (ch == victim)
	kill_self(victim);
    else {
	create_blood_pool(victim);
	raw_kill(ch, victim);
	send_to_char
	    ("\n{R{*Ow! Ow! Ow! Ow! Ow! That SUCKED! You're dead. :({x\n\r\n\r",
	     victim);
    }

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if (victim == ch)
	return;

    if (!victim->no_exp)
	turf_mod(ch, victim);

    members = 0;
    group_levels = 0;
    high_level = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (is_same_group(gch, ch)) {
	    members++;
	    group_levels += gch->level;

	    if (gch->level > high_level)
		high_level = gch->level;
	}
    }

    if (members == 0) {
	bug("Group_gain: members.", members);
	members = 1;
	group_levels = ch->level;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (!is_same_group(gch, ch) || IS_NPC(gch))
	    continue;

	xp = (xp_compute(gch, victim, high_level));

	xp = xp / members;

	if ((ch->desc && victim->desc)
	    && (!str_cmp(victim->desc->host, ch->desc->host))) {
	    send_to_char
		("Cheat attempt logged, no experience for you!\r\n", ch);
	    return;
	}
	// to stop guardian/summon farming.
	if (victim->no_exp)
	    xp = 0;

	sprintf(buf, "{RYou receive %d experience points.{x\n\r", xp);
	send_to_char(buf, gch);
	gain_exp(gch, xp);

	if (!IS_NPC(gch) && !IS_NPC(victim))
	    ++gch->pcdata->kills;

	/* Now for the poor PKillers who lost */
	if (!IS_NPC(gch) && !IS_NPC(victim)) {
	    gain_exp(victim, xp * -1);

	    if (IS_SET(victim->act, PLR_PKILL))	/* No longer a PKILLER */
		REMOVE_BIT(victim->act, PLR_PKILL);

	    sprintf(buf, "{RYou lose %d experience points.{x\n\r", xp);
	    send_to_char(buf, victim);
	}
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute(CHAR_DATA * gch, CHAR_DATA * victim, int total_levels)
{
    int xp, base_exp;
    int level_range;
//      int base_align;

    level_range = victim->level - total_levels;

    //    exp = 0;
    /* compute the base exp */
    switch (level_range) {
    default:
	base_exp = 0;
	break;
    case -9:
	base_exp = 5;
	break;
    case -8:
	base_exp = 7;
	break;
    case -7:
	base_exp = 9;
	break;
    case -6:
	base_exp = 11;
	break;
    case -5:
	base_exp = 13;
	break;
    case -4:
	base_exp = 17;
	break;
    case -3:
	base_exp = 35;
	break;
    case -2:
	base_exp = 45;
	break;
    case -1:
	base_exp = 55;
	break;
    case 0:
	base_exp = 100;
	break;
    case 1:
	base_exp = 115;
	break;
    case 2:
	base_exp = 120;
	break;
    case 3:
	base_exp = 125;
	break;
    case 4:
	base_exp = 130;
	break;
    case 5:
	base_exp = 135;
	break;
    case 6:
	base_exp = 140;
	break;
    case 7:
	base_exp = 145;
	break;
    case 8:
	base_exp = 150;
	break;
    case 9:
	base_exp = 155;
	break;
    case 10:
	base_exp = 160;
	break;
    case 11:
	base_exp = 165;
	break;
    case 12:
	base_exp = 170;
	break;
    case 13:
	base_exp = 175;
	break;
    case 14:
	base_exp = 180;
	break;
    case 15:
	base_exp = 185;
	break;
    case 16:
	base_exp = 190;
	break;

    case 17:
	base_exp = 195;
	break;

    case 18:
	base_exp = 200;
	break;
    case 19:
	base_exp = 205;
	break;
    case 20:
	base_exp = 215;
	break;
    case 21:
	base_exp = 225;
	break;
    case 22:
	base_exp = 235;
	break;
    case 23:
	base_exp = 245;
	break;
    case 24:
	base_exp = 255;
	break;
    case 25:
	base_exp = 265;
	break;
    case 26:
	base_exp = 275;
	break;
    case 27:
	base_exp = 285;
	break;
    case 28:
	base_exp = 295;
	break;
    case 29:
	base_exp = 305;
	break;
    case 30:
	base_exp = 315;
	break;
    case 31:
	base_exp = 325;
	break;
    case 32:
	base_exp = 335;
	break;
    case 33:
	base_exp = 345;
	break;
    case 34:
	base_exp = 355;
	break;
    case 35:
	base_exp = 365;
	break;
    case 36:
	base_exp = 375;
	break;
    case 37:
	base_exp = 385;
	break;
    case 38:
	base_exp = 395;
	break;
    case 39:
	base_exp = 405;
	break;
    case 40:
	base_exp = 415;
	break;
    }

    if (level_range > 40) {
	base_exp = 450;
    }


    xp = base_exp;


	/***********************************************************
	*  checks for remort					   *
	***********************************************************/
    xp = (xp / (gch->remorts + 1));



    if (xp < 0) {
	xp = 0;
    }

    /* randomize the rewards */
    xp = number_range(xp * 4 / 5, xp * 5 / 4);

    if (gch->level < 10)
	xp = xp * 4;
    if (gch->level < 20 && gch->level >= 10)
	xp = xp * 3;
    if (gch->level < 30 && gch->level >= 20)
	xp = xp * 2;
/*	if (victim->alignment > 0)
		gch->alignment -= base_align;
	if (gch->alignment < -1000)
		gch->alignment = -1000;
					
	else
		gch->alignment += base_align;
		if (gch->alignment > 1000)
			gch->alignment = 1000;*/

    if (xp > 450)
	xp = 450;

    return xp;
}


void
dam_message(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
	    bool immune)
{
    char buf1[256], buf2[256], buf3[256], buf4[256];
    char cdam[256], vdam[256], selfdam[256];

    const char *vs;
    const char *vp;
    const char *attack;
    char punct;

    if (ch == NULL || victim == NULL)
	return;

    if (dam == 0) {
	vs = "{Wmiss{x";
	vp = "{Wmisses{x";
    } else if (dam <= 4) {
	vs = "{Cscratch{x";
	vp = "{Cscratches{x";
    } else if (dam <= 8) {
	vs = "{Cgraze{x";
	vp = "{Cgrazes{x";
    } else if (dam <= 12) {
	vs = "{Chit{x";
	vp = "{Chits{x";
    } else if (dam <= 16) {
	vs = "{Rinjure{x";
	vp = "{Rinjures{x";
    } else if (dam <= 22) {
	vs = "{Rwound{x";
	vp = "{Rwounds{x";
    } else if (dam <= 29) {
	vs = "{Rmaul{x";
	vp = "{Rmauls{x";
    } else if (dam <= 36) {
	vs = "{Ydecimate{x";
	vp = "{Ydecimates{x";
    } else if (dam <= 43) {
	vs = "{Ydevastate{x";
	vp = "{Ydevastates{x";
    } else if (dam <= 50) {
	vs = "{Ymaim{x";
	vp = "{Ymaims{x";
    } else if (dam <= 59) {
	vs = "{GMUTILATE{x";
	vp = "{GMUTILATES{x";
    } else if (dam <= 72) {
	vs = "{GDISEMBOWEL{x";
	vp = "{GDISEMBOWELS{x";
    } else if (dam <= 88) {
	vs = "{GDISMEMBER{x";
	vp = "{GDISMEMBERS{x";
    } else if (dam <= 105) {
	vs = "{MMASSACRE{x";
	vp = "{MMASSACRES{x";
    } else if (dam <= 125) {
	vs = "{MMANGLE{x";
	vp = "{MMANGLES{x";
    } else if (dam <= 150) {
	vs = "{M*** DEMOLISH ***{x";
	vp = "{M*** DEMOLISHES ***{x";
    } else if (dam <= 175) {
	vs = "{m*** DEVASTATE ***{x";
	vp = "{M*** DEVASTATES ***{x";
    } else if (dam <= 245) {
	vs = "{M=== OBLITERATE ==={x";
	vp = "{M=== OBLITERATES ==={x";
    } else if (dam <= 350) {
	vs = "{M>>> ANNIHILATE <<<{x";
	vp = "{M>>> ANNIHILATES <<<{x";
    } else if (dam <= 550) {
	vs = "{M<<< ERADICATE >>>{x";
	vp = "{M<<< ERADICATES >>>{x";
    } else if (dam <= 850) {
	vs = "{M<<< LIQUIDATE >>>{x";
	vp = "{M<<< LIQUIDATES >>>{x";
    } else {
	vs = "{Ydo {RUNSPEAKABLE {Ythings to{x";
	vp = "{Ydoes {RUNSPEAKABLE {Ythings to{x";
    }

    punct = (dam <= 24) ? '.' : '!';

    if (IS_SET(ch->act, PLR_AUTODAMAGE)) {
	sprintf(cdam, " {W[{C%d{W]", dam);
	sprintf(selfdam, " {W[{R%d{W]", dam);
    } else {
	cdam[0] = '\0';
	selfdam[0] = '\0';
    }

    if (IS_SET(victim->act, PLR_AUTODAMAGE))
	sprintf(vdam, " {W[{R%d{W]", dam);
    else
	vdam[0] = '\0';

    if (dt == TYPE_HIT) {
	if (ch == victim) {
	    sprintf(buf1, "$n %s $melf%c", vp, punct);
	    sprintf(buf2, "You %s yourself%c%s{x", vs, punct, selfdam);
	} else {
	    sprintf(buf1, "$n %s $N%c", vp, punct);
	    sprintf(buf2, "{gYou %s{g $N%c%s{x", vs, punct, cdam);
	    sprintf(buf3, "{g$n %s{g you%c%s{x", vp, punct, vdam);
	}
    } else {
	if (dt >= 0 && dt < MAX_SKILL)
	    attack = skill_table[dt].noun_damage;
	else if (dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE)
	    attack = attack_table[dt - TYPE_HIT].noun;
	else {
	    bug("Dam_message: bad dt %d.", dt);
	    sprintf(buf4, "char: did it! %s target was %s", ch->name,
		    victim->name);
	    bug(buf4, 0);
	    dt = TYPE_HIT;
	    attack = attack_table[0].name;
	}

	if (immune) {
	    if (ch == victim) {
		sprintf(buf1, "$n is unaffected by $s own %s.", attack);
		sprintf(buf2, "Luckily, you are immune to that.");
	    } else {
		sprintf(buf1, "$N is unaffected by $n's %s!", attack);
		sprintf(buf2, "{G$N is unaffected by your %s!{x", attack);
		sprintf(buf3, "{G$n's %s is powerless against you.{x",
			attack);
	    }
	} else {
	    if (ch == victim) {
		sprintf(buf1, "$n's %s %s $m%c", attack, vp, punct);
		sprintf(buf2, "Your %s %s you%c%s{x", attack, vp, punct,
			selfdam);
	    } else {
		sprintf(buf1, "$n's %s %s $N%c", attack, vp, punct);
		sprintf(buf2, "{gYour %s %s{g $N%c%s{x", attack, vp, punct,
			cdam);
		sprintf(buf3, "{g$n's %s %s{g you%c%s{x", attack, vp,
			punct, vdam);
	    }
	}
    }

    if (ch == victim) {
	act(buf1, ch, NULL, NULL, TO_ROOM);
	act(buf2, ch, NULL, NULL, TO_CHAR);
    } else {
	act(buf1, ch, NULL, victim, TO_NOTVICT);
	act(buf2, ch, NULL, victim, TO_CHAR);
	act(buf3, ch, NULL, victim, TO_VICT);
    }

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm(CHAR_DATA * ch, CHAR_DATA * victim)
{
    OBJ_DATA *obj;
    bool displaced;
    displaced = FALSE;		//displace spell

    if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
	return;

    if (IS_OBJ_STAT(obj, ITEM_NOREMOVE)) {
	act("$S weapon won't budge!", ch, NULL, victim, TO_CHAR);
	act("$n tries to disarm you, but your weapon won't budge!",
	    ch, NULL, victim, TO_VICT);
	act("$n tries to disarm $N, but fails.", ch, NULL, victim,
	    TO_NOTVICT);
	return;
    }
    if (IS_AFFECTED(victim, AFF_TALON)) {
	act("$S weapon sticks to $M like glue!", ch, NULL, victim,
	    TO_CHAR);
	act("$n tries to disarm you, but your weapon sticks like glue!",
	    ch, NULL, victim, TO_VICT);
	act("$n tries to disarm $N, but $N's weapon sticks to $M like glue.", ch, NULL, victim, TO_NOTVICT);
	return;
    }
    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're disarming the displaced version
	    send_to_char("Your aim goes right through the weapon...\n\r",
			 ch);
	    displaced = TRUE;
	    return;
	}
    act("{M$n {YDISARMS {Myou and sends your weapon flying!{x",
	ch, NULL, victim, TO_VICT);
    act("You disarm $N!", ch, NULL, victim, TO_CHAR);
    act("$n disarms $N!", ch, NULL, victim, TO_NOTVICT);

    obj_from_char(obj);
    if (IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_INVENTORY))
	obj_to_char(obj, victim);
    else {
	obj_to_room(obj, victim->in_room);
	if (IS_NPC(victim) && victim->wait == 0
	    && can_see_obj(victim, obj))
	    get_obj(victim, obj, NULL);
    }
    return;
}

void do_berserk(CHAR_DATA * ch, char *argument)
{
    int chance, hp_percent;

    if ((chance = get_skill(ch, gsn_berserk)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BERSERK))
	|| (!IS_NPC(ch)
	    && ch->level <
	    skill_table[gsn_berserk].skill_level[ch->class])) {
	send_to_char("You turn red in the face, but nothing happens.\n\r",
		     ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_BERSERK) || is_affected(ch, gsn_berserk)
	|| is_affected(ch, skill_lookup("frenzy"))) {
	send_to_char("You get a little madder.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CALM)) {
	send_to_char("You're feeling to mellow to berserk.\n\r", ch);
	return;
    }
    if (ch->mana < 50) {
	send_to_char("You can't get up enough energy.\n\r", ch);
	return;
    }
    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
	chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit / ch->max_hit;
    chance += 25 - hp_percent / 2;

    if (number_percent() < chance) {
	AFFECT_DATA af;

	if (ch->level < 300)
	    WAIT_STATE(ch, 12);
	ch->mana -= 50;
	ch->move /= 2;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN(ch->hit, ch->max_hit);

	send_to_char("Your pulse races as you are consumed by rage!\n\r",
		     ch);
	act("$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_berserk, TRUE, 2);

	af.where = TO_AFFECTS;
	af.type = gsn_berserk;
	af.level = ch->level;
	af.duration = number_fuzzy(ch->level / 8);
	af.modifier = UMAX(1, ch->level / 5);
	af.bitvector = AFF_BERSERK;

	af.location = APPLY_HITROLL;
	affect_to_char(ch, &af);

	af.location = APPLY_DAMROLL;
	affect_to_char(ch, &af);

	af.modifier = UMAX(10, 10 * (ch->level / 5));
	af.location = APPLY_AC;
	affect_to_char(ch, &af);
    } else {
	if (ch->level < 300)
	    WAIT_STATE(ch, 36);
	ch->mana -= 25;
	ch->move /= 2;

	send_to_char("Your pulse speeds up, but nothing happens.\n\r", ch);
	check_improve(ch, gsn_berserk, FALSE, 2);
    }
}

void do_bash(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_bash)) == 0
	|| (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BASH))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_bash].skill_level[ch->class])) {
	send_to_char("Bashing? What's that?\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim->position < POS_FIGHTING) {
	act("You'll have to let $M get back up first.", ch, NULL, victim,
	    TO_CHAR);
	return;
    }
    if (victim == ch) {
	send_to_char("You try to bash your brains out, but fail.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;
    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_NPC(victim) &&
	victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 15;
    else
	chance += (ch->size - victim->size) * 10;


    /* stats */
    chance += get_curr_stat(ch, STAT_STR);
    chance -= (get_curr_stat(victim, STAT_DEX) * 4) / 3;
    chance -= GET_AC(victim, AC_BASH) / 25;
    /* speed */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST)
	|| IS_AFFECTED(victim, AFF_HASTE))
	chance -= 30;

    /* level */
    /*chance += (ch->level - victim->level); */

    if (!IS_NPC(victim) && chance < get_skill(victim, gsn_dodge)) {	/*
									   act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
									   act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
									   WAIT_STATE(ch,skill_table[gsn_bash].beats);
									   return; */
	chance -= 3 * (get_skill(victim, gsn_dodge) - chance);
    }
    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're bashing the displaced version
	    send_to_char("You charge and... fly through him...\n\r", ch);
	    chance = 0;
	    displaced = TRUE;
	}

    /* now the attack */
    if (number_percent() < chance) {

	act("$n sends you sprawling with a powerful bash!",
	    ch, NULL, victim, TO_VICT);
	act("You slam into $N, and send $M flying!", ch, NULL, victim,
	    TO_CHAR);
	act("$n sends $N sprawling with a powerful bash.", ch, NULL,
	    victim, TO_NOTVICT);
	check_improve(ch, gsn_bash, TRUE, 1);

	DAZE_STATE(victim, PULSE_VIOLENCE);
	WAIT_STATE(ch, skill_table[gsn_bash].beats);
	damage(ch, victim, number_range(2, 2 + 2 * ch->size + chance / 20),
	       gsn_bash, DAM_BASH, TRUE);

    } else {
	damage(ch, victim, 0, gsn_bash, DAM_BASH, FALSE);
	act("You fall flat on your face!", ch, NULL, victim, TO_CHAR);
	act("$n falls flat on $s face.", ch, NULL, victim, TO_NOTVICT);
	act("You evade $n's bash, causing $m to fall flat on $s face.",
	    ch, NULL, victim, TO_VICT);
	check_improve(ch, gsn_bash, FALSE, 1);
	WAIT_STATE(ch, skill_table[gsn_bash].beats * 3 / 2);
    }
    check_killer(ch, victim);
}

void do_dirt(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_dirt)) == 0 || (!IS_NPC(ch)
						    && ch->level <
						    skill_table[gsn_dirt].
						    skill_level[ch->
								class])) {
	send_to_char("You get your feet dirty.\n\r", ch);
	return;
    }

    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't in combat!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (IS_AFFECTED(victim, AFF_BLIND)) {
	act("$E's already been blinded.", ch, NULL, victim, TO_CHAR);
	return;
    }

    if (victim == ch) {
	send_to_char("Very funny.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;
    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_NPC(victim) &&
	victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch, STAT_DEX);
    chance -= 2 * get_curr_stat(victim, STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST)
	|| IS_AFFECTED(victim, AFF_HASTE))
	chance -= 25;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* terrain */

    switch (ch->in_room->sector_type) {
    case (SECT_INSIDE):
	chance -= 20;
	break;
    case (SECT_CITY):
	chance -= 10;
	break;
    case (SECT_FIELD):
	chance += 5;
	break;
    case (SECT_FOREST):
	break;
    case (SECT_HILLS):
	break;
    case (SECT_MOUNTAIN):
	chance -= 10;
	break;
    case (SECT_WATER_SWIM):
	chance = 0;
	break;
    case (SECT_WATER_NOSWIM):
	chance = 0;
	break;
    case (SECT_AIR):
	chance = 0;
	break;
    case (SECT_DESERT):
	chance += 10;
	break;
    }

    if (chance == 0) {
	send_to_char("There isn't any dirt to kick.\n\r", ch);
	return;
    }
    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're throwing dirt at the displaced version
	    send_to_char("The dirt... flies through him...\n\r", ch);
	    chance = 0;
	    displaced = TRUE;
	}

    /* now the attack */
    if (number_percent() < chance) {
	AFFECT_DATA af;
	act("$n is blinded by the dirt in $s eyes!", victim, NULL, NULL,
	    TO_ROOM);
	act("$n kicks dirt in your eyes!", ch, NULL, victim, TO_VICT);
	damage(ch, victim, number_range(2, 5), gsn_dirt, DAM_NONE, FALSE);
	send_to_char("You can't see a thing!\n\r", victim);
	check_improve(ch, gsn_dirt, TRUE, 2);
	WAIT_STATE(ch, skill_table[gsn_dirt].beats);

	af.where = TO_AFFECTS;
	af.type = gsn_dirt;
	af.level = ch->level;
	af.duration = 2;
	af.location = APPLY_HITROLL;
	af.modifier = -4;
	af.bitvector = AFF_BLIND;

	affect_to_char(victim, &af);
    } else {
	damage(ch, victim, 0, gsn_dirt, DAM_NONE, TRUE);
	check_improve(ch, gsn_dirt, FALSE, 2);
	WAIT_STATE(ch, skill_table[gsn_dirt].beats);
    }
    check_killer(ch, victim);
}

void do_trip(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell

    one_argument(argument, arg);

    if ((chance = get_skill(ch, gsn_trip)) == 0
	|| (IS_NPC(ch) && (IS_AFFECTED(ch, AFF_BLIND)))
	|| (!IS_NPC(ch)
	    && ch->level < skill_table[gsn_trip].skill_level[ch->class])) {
	send_to_char("Tripping?  What's that?\n\r", ch);
	return;
    }
    if (arg[0] == '\0') {
	victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("But you aren't fighting anyone!\n\r", ch);
	    return;
	}
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim))
	return;
    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_NPC(victim) &&
	victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(victim, AFF_FLYING)) {
	act("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim->position < POS_FIGHTING) {
	act("$N is already down.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim == ch) {
	send_to_char("You fall flat on your face!\n\r", ch);
	WAIT_STATE(ch, 2 * skill_table[gsn_trip].beats);
	act("$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
	return;
    }
    /* modifiers */

    /* size */
    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 10;	/* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch, STAT_DEX);
    chance -= get_curr_stat(victim, STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags, OFF_FAST)
	|| IS_AFFECTED(victim, AFF_HASTE))
	chance -= 20;

    /* level */
    /*chance += (ch->level - victim->level) * 2; */

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're trying to trip the displaced version
	    send_to_char("The attempt passes through the legs...\n\r", ch);
	    chance = 0;
	    displaced = TRUE;
	}

    /* now the attack */
    if (number_percent() < chance) {
	act("$n trips you and you go down!", ch, NULL, victim, TO_VICT);
	act("You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR);
	act("$n trips $N, sending $M to the ground.", ch, NULL, victim,
	    TO_NOTVICT);
	check_improve(ch, gsn_trip, TRUE, 1);

	DAZE_STATE(victim, PULSE_VIOLENCE);
	WAIT_STATE(ch, skill_table[gsn_trip].beats);
	victim->position = POS_RESTING;
	damage(ch, victim, number_range(2, 2 + 2 * victim->size), gsn_trip,
	       DAM_BASH, TRUE);
    } else {
	damage(ch, victim, 0, gsn_trip, DAM_BASH, TRUE);
	WAIT_STATE(ch, skill_table[gsn_trip].beats * 2 / 3);
	check_improve(ch, gsn_trip, FALSE, 1);
    }
    check_killer(ch, victim);
}



void do_kill(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Kill whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You hit yourself.  Ouch!\n\r", ch);
	multi_hit(ch, ch, TYPE_UNDEFINED);
	return;
    }
    if (is_safe(ch, victim))
	return;

    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (ch->position == POS_FIGHTING) {
	send_to_char("You do the best you can!\n\r", ch);
	return;
    }
    if (ch->level < 201)
	WAIT_STATE(ch, 12);

    if (IS_NPC(ch)) {
	mob_remember(victim, ch, MEM_HOSTILE);
    }

    check_killer(ch, victim);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
}



void do_murde(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to MURDER, spell it out.\n\r", ch);
    return;
}

void do_murder(CHAR_DATA * ch, char *argument)
{
    do_kill(ch, argument);
    return;
}



void do_backstab(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
//    OBJ_DATA *obj;
    OBJ_DATA *wield;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell

    one_argument(argument, arg);

    if (!IS_NPC(ch))
	chance = (get_skill(ch, gsn_backstab));
    else
	chance = UMIN(ch->level, 100);

    if (chance <= 1) {
	send_to_char("You don't know how to backstab, silly.\r\n", ch);
	return;
    }

    if (arg[0] == '\0') {
	send_to_char("Backstab whom?\n\r", ch);
	return;
    }

    if (ch->fighting != NULL) {
	send_to_char("You can't backstab someone you're fighting!\n\r",
		     ch);
	return;
    } else if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (victim == ch) {
	send_to_char("How can you sneak up on yourself?\n\r", ch);
	return;
    }

    if (is_safe(ch, victim))
	return;
    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_NPC(victim)
	&& victim->fighting != NULL
	&& !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }

    if (victim->hit < victim->max_hit / 3) {
	act("$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR);
	return;
    }

    if (!IS_NPC(ch)) {
	/*Only certain weapons can be used */
	wield = get_eq_char(ch, WEAR_WIELD);
	if (wield == NULL || wield->item_type != ITEM_WEAPON) {
	    send_to_char
		("You do need to use some sort of a weapon to backstab.\n\r",
		 ch);
	    return;
	} else {
	    switch (wield->value[0]) {
	    case (WEAPON_SWORD):
		chance -= 10;
		break;
	    case (WEAPON_DAGGER):
		chance = chance;
		break;
	    case (WEAPON_SPEAR):
		send_to_char
		    ("You realy should try a different weapon next time\n\r",
		     ch);
		return;
		break;
	    case (WEAPON_MACE):
		send_to_char
		    ("How do you expect to stab someone with a mace?\n\r",
		     ch);
		return;
		break;
	    case (WEAPON_AXE):
		send_to_char("A axe blow is to slow for a backstab\n\r",
			     ch);
		return;
		break;
	    case (WEAPON_FLAIL):
		send_to_char("Flails can't be used to backstab\n\r", ch);
		return;
		break;
	    case (WEAPON_EXOTIC):
		send_to_char
		    ("Your knowledge of this weapon isn't great enough\n\r",
		     ch);
		return;
		break;
	    case (WEAPON_WHIP):
		send_to_char("It's impossible to backstab with a whip\n\r",
			     ch);
		return;
		break;
	    case (WEAPON_POLEARM):
		send_to_char("You realy should use a other weapon\n\r",
			     ch);
		return;
		break;
	    default:
		send_to_char("You realy should use a weapon with a blade",
			     ch);
		return;
		break;
	    }
	}
	check_killer(ch, victim);
    }
    WAIT_STATE(ch, skill_table[gsn_backstab].beats);

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're backstabbing the displaced version
	    send_to_char
		("The stab mysteriously goes right through the back...\n\r",
		 ch);
	    chance = 0;
	    displaced = TRUE;
	}
    if ((number_percent() <= chance) || (chance >= 2 && !IS_AWAKE(victim))) {
	check_improve(ch, gsn_backstab, TRUE, 1);
	one_hit(ch, victim, gsn_backstab);
    }

    else {
	check_improve(ch, gsn_backstab, FALSE, 1);
	damage(ch, victim, 0, gsn_backstab, DAM_NONE, TRUE);
    }

    return;
}

void do_assassinate(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Assassinate whom?\n\r", ch);
	return;
    }
    if (ch->fighting != NULL) {
	send_to_char("It's a little too late for that now.\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("Perhaps you should try delete.\n\r", ch);
	return;
    }
    if (IS_IMMORTAL(victim) && victim->arena == FALSE) {
	send_to_char("Assassinate an immortal? Are you crazy!?\n\r", ch);
	return;
    }

    if (is_safe(ch, victim))
	return;
    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_NPC(victim) && victim->fighting != NULL) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (get_eq_char(ch, WEAR_WIELD) == NULL) {
	send_to_char("You need to wield a weapon to assassinate.\n\r", ch);
	return;
    }

    /*Assassination chance */
    chance = ((get_skill(ch, gsn_assassinate) / 3));

    if (!IS_AWAKE(victim))
	chance += 75;

    if (IS_AFFECTED(ch, AFF_SNEAK))
	chance += 15;
    else if (get_trust(victim) < ch->invis_level
	     || IS_AFFECTED(victim, AFF_BLIND)
	     || (IS_AFFECTED(ch, AFF_INVISIBLE)
		 && !IS_AFFECTED(victim, AFF_DETECT_INVIS))
	     || (room_is_dark(victim->in_room)
		 && !IS_AFFECTED(victim, AFF_INFRARED)))
	chance += 10;

    chance = URANGE(1, chance, 1100);

    if (can_see(victim, ch))
	chance -= 50;

    if (get_skill(ch, gsn_assassinate) <= 1) {
	send_to_char("You don't know how to assassinate, silly.\r\n", ch);
	return;
    }

    check_killer(ch, victim);

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're trying to kill the displaced version
	    send_to_char("Your attempt goes right through...\n\r", ch);
	    chance = 0;
	    displaced = TRUE;
	}
    if (number_percent() <= chance) {
	act("You assassinate $N in cold blood!", ch, NULL, victim,
	    TO_CHAR);
	act("$n assassinates you in cold blood!", ch, NULL, victim,
	    TO_VICT);
	act("$n assassinates $N in cold blood!", ch, NULL, victim,
	    TO_NOTVICT);
	check_improve(ch, gsn_assassinate, TRUE, 1);
	if (victim->arena == FALSE)
	    if (IS_SET(victim->act, PLR_PKILL))	/* No longer a PKILLER */
		REMOVE_BIT(victim->act, PLR_PKILL);

	victim->hit = 1;

	raw_kill(ch, victim);
    } else {
	WAIT_STATE(ch, skill_table[gsn_assassinate].beats);
	act("Your assassination attempt on $N failed!", ch, NULL, victim,
	    TO_CHAR);
	act("$n's assassination attempt on you failed!", ch, NULL, victim,
	    TO_VICT);
	act("$n's assassination attempt on $N failed!", ch, NULL, victim,
	    TO_NOTVICT);
	check_improve(ch, gsn_assassinate, FALSE, 1);
	multi_hit(ch, victim, TYPE_UNDEFINED);
    }

    return;
}


void do_flee(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ((victim = ch->fighting) == NULL) {
	if (ch->position == POS_FIGHTING)
	    ch->position = POS_STANDING;
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }

    if (ch->position < POS_FIGHTING) {
	do_stand(ch, "");
    }

    if (IS_AFFECTED(ch, AFF_LETHARGY)) {
	send_to_char("You are too lethargic to flee.\n\r", ch);
	return;
    }

    if (ch->daze) {
	send_to_char("You are too dazed to flee!\n\r", ch);
	return;
    }

    was_in = ch->in_room;

    for (attempt = 0; attempt < 6; attempt++) {
	EXIT_DATA *pexit;
	int door;

	door = number_door();
	if ((pexit = was_in->exit[door]) == 0
	    || pexit->u1.to_room == NULL
	    || IS_SET(pexit->exit_info, EX_CLOSED)
	    || number_range(0, ch->daze) != 0 || (IS_NPC(ch)
						  && IS_SET(pexit->u1.
							    to_room->
							    room_flags,
							    ROOM_NO_MOB)))
	    continue;

	move_char(ch, door, FALSE, FALSE);

	if ((now_in = ch->in_room) == was_in)
	    continue;

	ch->in_room = was_in;
	act("$n has fled!", ch, NULL, NULL, TO_ROOM);
	ch->in_room = now_in;

	if (!IS_NPC(ch)) {
	    send_to_char("You flee from combat!\n\r", ch);
	    if (IS_AFFECTED(ch, AFF_SNEAK)) {
		if ((ch->class == 2) && (number_percent() > 10)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
		if ((ch->class == 4) && (number_percent() > 10)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
		if ((ch->class == 0) && (number_percent() > 30)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
		if ((ch->class == 1) && (number_percent() > 30)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
		if ((ch->class == 3) && (number_percent() > 40)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
		if ((ch->class == 5) && (number_percent() > 60)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
		if ((ch->class == 6) && (number_percent() > 30)) {
		    send_to_char("You snuck away safely.\n\r", ch);
		}
	    }

	    else {
		if (ch->arena == FALSE) {
		    send_to_char("You lost 10 exp.\n\r", ch);
		    ch->exp -= 10;
		}
	    }
	}

	stop_fighting(ch, TRUE);
	return;
    }

    send_to_char("PANIC! You couldn't escape!\n\r", ch);
    return;
}



void do_rescue(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    bool displaced;
    displaced = FALSE;		//displace spell

    one_argument(argument, arg);
    if (arg[0] == '\0') {
	send_to_char("Rescue whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("What about fleeing instead?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && IS_NPC(victim)) {
	send_to_char("Doesn't need your help!\n\r", ch);
	return;
    }
    if (ch->fighting == victim) {
	send_to_char("Too late.\n\r", ch);
	return;
    }
    if ((fch = victim->fighting) == NULL) {
	send_to_char("That person is not fighting right now.\n\r", ch);
	return;
    }
    if (victim->fighting != NULL && !is_same_group(ch, victim->fighting)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    if (IS_NPC(fch) && !is_same_group(ch, victim)) {
	send_to_char("Kill stealing is not permitted.\n\r", ch);
	return;
    }
    WAIT_STATE(ch, skill_table[gsn_rescue].beats);

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, -50, 0, 0)) {

	    //You're rescuing the displaced version
	    send_to_char
		("You, amazingly, accidently step through the person while trying to rescue...\n\r",
		 ch);
	    displaced = TRUE;
	    send_to_char("You fail the rescue.\n\r", ch);
	    check_improve(ch, gsn_rescue, FALSE, 1);
	    return;
	}
    if (number_percent() > get_skill(ch, gsn_rescue)) {
	send_to_char("You fail the rescue.\n\r", ch);
	check_improve(ch, gsn_rescue, FALSE, 1);
	return;
    }
    act("You rescue $N!", ch, NULL, victim, TO_CHAR);
    act("$n rescues you!", ch, NULL, victim, TO_VICT);
    act("$n rescues $N!", ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, gsn_rescue, TRUE, 1);

    stop_fighting(fch, FALSE);
    stop_fighting(victim, FALSE);

    check_killer(ch, fch);
    set_fighting(ch, fch);
    set_fighting(fch, ch);
    return;
}



void do_kick(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    bool displaced;
    displaced = FALSE;		//displace spell

    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_kick].skill_level[ch->class]) {
	send_to_char("You better leave the martial arts to fighters.\n\r",
		     ch);
	return;
    }
    if (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_KICK))
	return;

    if (IS_NPC(ch) && (IS_AFFECTED(ch, AFF_BLIND))) {
	return;
    }
    if ((victim = ch->fighting) == NULL) {
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }
    WAIT_STATE(ch, skill_table[gsn_kick].beats);

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're kicking the displaced version
	    send_to_char("Your boot goes right through...\n\r", ch);
	    displaced = TRUE;
	    damage(ch, victim, 0, gsn_kick, DAM_BASH, TRUE);
	    check_improve(ch, gsn_kick, FALSE, 1);
	    return;
	}
    if (get_skill(ch, gsn_kick) > number_percent()) {
	damage(ch, victim, number_range(1, ch->level), gsn_kick, DAM_BASH,
	       TRUE);
	check_improve(ch, gsn_kick, TRUE, 1);
    } else {
	damage(ch, victim, 0, gsn_kick, DAM_BASH, TRUE);
	check_improve(ch, gsn_kick, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}

void do_arial(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    bool displaced;
    displaced = FALSE;		//displace spell

    if (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_arial].skill_level[ch->class]) {
	send_to_char("You better leave the flying to the birds.\n\r", ch);
	return;
    }
    if (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_ARIAL))
	return;

    if ((victim = ch->fighting) == NULL) {
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }
    WAIT_STATE(ch, skill_table[gsn_arial].beats);

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're trying to land on the displaced version
	    send_to_char("You land right on top... and through...\n\r",
			 ch);
	    displaced = TRUE;
	    damage(ch, victim, 0, gsn_arial, DAM_BASH, TRUE);
	    check_improve(ch, gsn_arial, FALSE, 1);
	    return;
	}
    if (get_skill(ch, gsn_arial) > number_percent()) {
	damage(ch, victim, number_range(15, ch->level), gsn_arial,
	       DAM_BASH, TRUE);
	check_improve(ch, gsn_arial, TRUE, 1);
    } else {
	damage(ch, victim, 0, gsn_arial, DAM_BASH, TRUE);
	check_improve(ch, gsn_arial, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}

void do_whirl(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell

    if (ch->position < POS_FIGHTING) {
	return;
    }

    if ((chance = get_skill(ch, gsn_whirl)) == 0) {
	send_to_char("You don't know how to whirl around opponents.\n\r",
		     ch);
	return;
    }
    if (IS_NPC(ch) && (IS_AFFECTED(ch, AFF_BLIND))) {
	return;
    }
    if ((victim = ch->fighting) == NULL) {
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }
    if (!can_see(ch, victim)) {
	send_to_char("You can't see your opponent well enough to aim.\n\r",
		     ch);
	return;
    }
    /* check duck */
    if (check_duck(ch, victim, "whirl")) {
	WAIT_STATE(ch, skill_table[gsn_whirl].beats);
	return;
    }
    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're whirling the displaced version
	    send_to_char("You whirl and swing... right through...\n\r",
			 ch);
	    chance = 0;
	    displaced = TRUE;
	}

    /* and now the attack */
    if (number_percent() < chance) {
	WAIT_STATE(ch, skill_table[gsn_whirl].beats);
	act("You whirl your body around $N and flank $M!", ch, NULL,
	    victim, TO_CHAR);
	act("$n whirls $s body around you and flanks you!", ch, NULL,
	    victim, TO_VICT);
	act("$n whirls $s body around $N and flanks $M!", ch, NULL, victim,
	    TO_NOTVICT);

	check_improve(ch, gsn_whirl, TRUE, 1);

	one_hit(ch, victim, gsn_whirl);

	if (ch->fighting != victim)
	    return;

	if (IS_AFFECTED(ch, AFF_HASTE))
	    one_hit(ch, victim, gsn_whirl);

	if (ch->fighting != victim)
	    return;

	chance = get_skill(ch, gsn_second_attack) / 4;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_whirl);
	    check_improve(ch, gsn_second_attack, TRUE, 5);
	    if (ch->fighting != victim)
		return;
	}

	chance = get_skill(ch, gsn_third_attack) / 6;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_whirl);
	    check_improve(ch, gsn_third_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}

	chance = get_skill(ch, gsn_fourth_attack) / 8;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_whirl);
	    check_improve(ch, gsn_fourth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}

	chance = get_skill(ch, gsn_fifth_attack) / 10;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_whirl);
	    check_improve(ch, gsn_fifth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}

	chance = get_skill(ch, gsn_sixth_attack) / 16;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_whirl);
	    check_improve(ch, gsn_sixth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}
	chance = get_skill(ch, gsn_fifth_attack) / 10;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_whirl);
	    check_improve(ch, gsn_fifth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}



    } else {
	WAIT_STATE(ch, skill_table[gsn_whirl].beats);
	act("You fail to whirl your body around $N.", ch, NULL, victim,
	    TO_CHAR);
	act("$n tries to whirl $s body around you, but fails.", ch, NULL,
	    victim, TO_VICT);
	act("$n tries to whirl $s body around $N, but fails.", ch, NULL,
	    victim, TO_NOTVICT);
	check_improve(ch, gsn_whirl, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}

void do_circle(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    int chance;
    bool displaced;
    displaced = FALSE;		//displace spell
    if (ch->position < POS_FIGHTING) {
	return;
    }

    if ((chance = get_skill(ch, gsn_circle)) == 0) {
	send_to_char("You don't know how to circle opponents.\n\r", ch);
	return;
    }

    if ((victim = ch->fighting) == NULL) {
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }

    if (!can_see(ch, victim)) {
	send_to_char("You can't see your opponent well enough to aim.\n\r",
		     ch);
	return;
    }
    /* check duck */
    if (check_duck(ch, victim, "circle")) {
	WAIT_STATE(ch, skill_table[gsn_circle].beats);
	return;
    }
    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're circling the displaced version
	    send_to_char("You circle and swing... right through...\n\r",
			 ch);
	    chance = 0;
	    displaced = TRUE;
	}

    /* and now the attack */
    if (number_percent() < chance) {
	WAIT_STATE(ch, (skill_table[gsn_circle].beats * 2) / 3);
	act("You circle around $N and attack!", ch, NULL, victim, TO_CHAR);
	act("$n circles around you and attacks!", ch, NULL, victim,
	    TO_VICT);
	act("$n circles around $N and attacks!", ch, NULL, victim,
	    TO_NOTVICT);

	check_improve(ch, gsn_circle, TRUE, 1);

	one_hit(ch, victim, gsn_circle);

	if (ch->fighting != victim)
	    return;

	if (IS_AFFECTED(ch, AFF_HASTE))
	    one_hit(ch, victim, gsn_circle);

	if (ch->fighting != victim)
	    return;

	chance = get_skill(ch, gsn_second_attack) / 4;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_circle);
	    check_improve(ch, gsn_second_attack, TRUE, 5);
	    if (ch->fighting != victim)
		return;
	}

	chance = get_skill(ch, gsn_third_attack) / 6;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_circle);
	    check_improve(ch, gsn_third_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}
	chance = get_skill(ch, gsn_fourth_attack) / 8;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_circle);
	    check_improve(ch, gsn_fourth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}

	chance = get_skill(ch, gsn_fifth_attack) / 12;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_circle);
	    check_improve(ch, gsn_fifth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}
	chance = get_skill(ch, gsn_sixth_attack) / 16;

	if (number_percent() < chance) {
	    one_hit(ch, victim, gsn_circle);
	    check_improve(ch, gsn_sixth_attack, TRUE, 6);
	    if (ch->fighting != victim)
		return;
	}
    } else {
	WAIT_STATE(ch, (skill_table[gsn_circle].beats * 2) / 3);
	act("You fail to circle around $N.", ch, NULL, victim, TO_CHAR);
	act("$n tries to circle around you, but fails.", ch, NULL, victim,
	    TO_VICT);
	act("$n tries to circle around $N, but fails.", ch, NULL, victim,
	    TO_NOTVICT);
	check_improve(ch, gsn_circle, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}

bool check_duck(CHAR_DATA * ch, CHAR_DATA * victim, char *attack_type)
{
    int chance;

    if (!IS_AWAKE(victim))
	return FALSE;

    chance = get_skill(victim, gsn_duck) * 3 / 4;

    if (!can_see(victim, ch))
	chance /= 2;

    if (number_percent() >= chance)
	return FALSE;

    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_AUTODEFENSE))
	act("{yYou duck $n's $t.{x", ch, attack_type, victim, TO_VICT);

    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_AUTODEFENSE))
	act("{y$N ducks your $t.{x", ch, attack_type, victim, TO_CHAR);

    check_improve(victim, gsn_duck, TRUE, 6);
    return TRUE;
}

void do_disarm(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;
    bool displaced;
    displaced = FALSE;		//displace spell

    hth = 0;

    if ((chance = get_skill(ch, gsn_disarm)) == 0) {
	send_to_char("You don't know how to disarm opponents.\n\r", ch);
	return;
    }

    if (get_eq_char(ch, WEAR_WIELD) == NULL
	&& ((hth = get_skill(ch, gsn_hand_to_hand)) == 0
	    || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_DISARM)))) {
	send_to_char("You must wield a weapon to disarm.\n\r", ch);
	return;
    }

    if ((victim = ch->fighting) == NULL) {
	send_to_char("You aren't fighting anyone.\n\r", ch);
	return;
    }

    if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL) {
	send_to_char("Your opponent is not wielding a weapon.\n\r", ch);
	return;
    }

    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch, get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim, get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch, get_weapon_sn(victim));

    /* modifiers */

    /* skill */
    if (get_eq_char(ch, WEAR_WIELD) == NULL)
	chance = chance * hth / 150;
    else
	chance = chance * ch_weapon / 100;

    chance += (ch_vict_weapon / 2 - vict_weapon) / 2;

    /* dex vs. strength */
    chance += get_curr_stat(ch, STAT_DEX);
    chance -= 2 * get_curr_stat(victim, STAT_STR);

    //Check if displaced
    if (is_affected(victim, skill_lookup("displace")))
	if (displace_new_effect(ch, (void *) victim, 0, 0, 0)) {

	    //You're disarming the displaced version
	    send_to_char("You attempt to disarm goes right through...\n\r",
			 ch);
	    chance = 0;
	    displaced = TRUE;
	}

    /* and now the attack */
    if (number_percent() < chance) {
	WAIT_STATE(ch, skill_table[gsn_disarm].beats);
	disarm(ch, victim);
	check_improve(ch, gsn_disarm, TRUE, 1);
    } else {
	WAIT_STATE(ch, skill_table[gsn_disarm].beats);
	act("You fail to disarm $N.", ch, NULL, victim, TO_CHAR);
	act("$n tries to disarm you, but fails.", ch, NULL, victim,
	    TO_VICT);
	act("$n tries to disarm $N, but fails.", ch, NULL, victim,
	    TO_NOTVICT);
	check_improve(ch, gsn_disarm, FALSE, 1);
    }
    check_killer(ch, victim);
    return;
}


void do_surrender(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *mob;
    if ((mob = ch->fighting) == NULL) {
	send_to_char("But you're not fighting!\n\r", ch);
	return;
    }
    act("You surrender to $N!", ch, NULL, mob, TO_CHAR);
    act("$n surrenders to you!", ch, NULL, mob, TO_VICT);
    act("$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT);
    stop_fighting(ch, TRUE);

    if (!IS_NPC(ch) && IS_NPC(mob)
	&& (!HAS_TRIGGER(mob, TRIG_SURR)
	    || !mp_percent_trigger(mob, ch, NULL, NULL, TRIG_SURR))) {
	act("$N seems to ignore your cowardly act!", ch, NULL, mob,
	    TO_CHAR);
	multi_hit(mob, ch, TYPE_UNDEFINED);
	return;
    }
    if (!IS_NPC(mob)) {
	act("$N seems to ignore your cowardly act!", ch, NULL, mob,
	    TO_CHAR);
	multi_hit(mob, ch, TYPE_UNDEFINED);
	return;
    }
    send_to_char("You lost 10 exp.\n\r", ch);
    gain_exp(ch, -10);
}


void do_sla(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to SLAY, spell it out.\n\r", ch);
    return;
}


void do_slay(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument(argument, arg);

    if ((arg[0] == '\0') && ch->position != POS_FIGHTING) {
	send_to_char("Slay whom?\n\r", ch);
	return;
    }
    if (((victim = get_char_room(ch, arg)) == NULL) &&
	ch->position != POS_FIGHTING) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (ch->position == POS_FIGHTING)
	victim = ch->fighting;

    if (victim == NULL) {
	send_to_char("Victim == NULL, blue sucks.\n\r", ch);
	return;
    }
    if (ch == victim) {
	send_to_char("Suicide is a mortal sin.\n\r", ch);
	return;
    }
    if (IS_IMMORTAL(victim)) {
	if (!IS_NPC(ch)) {
	    send_to_char("You failed.\n\r", ch);
	    return;
	}
    }
    if (IS_IMMORTAL(victim) && ch->level <= victim->level) {
	send_to_char("You failed.\n\r", ch);
	return;
    }

    act("You slay $M in cold blood!", ch, NULL, victim, TO_CHAR);
    act("$n slays you in cold blood!", ch, NULL, victim, TO_VICT);
    act("$n slays $N in cold blood!", ch, NULL, victim, TO_NOTVICT);

    raw_kill(ch, victim);
    return;
}

void do_retribution(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
	return;

    if (argument[0] != '\0') {
	buf[0] = '\0';
	smash_tilde(argument);

	if (argument[0] == '-') {
	    int len;
	    bool found = FALSE;

	    if (ch->pcdata->wrath == NULL || ch->pcdata->wrath[0] == '\0') {
		send_to_char("No lines left to remove.\n\r", ch);
		return;
	    }
	    strcpy(buf, ch->pcdata->wrath);

	    for (len = strlen(buf); len > 0; len--) {
		if (buf[len] == '\r') {
		    if (!found) {	/* back it up */
			if (len > 0)
			    len--;
			found = TRUE;
		    } else
			/* found the second one */
		    {
			buf[len + 1] = '\0';
			free_string(ch->pcdata->wrath);
			ch->pcdata->wrath = str_dup(buf);
			send_to_char
			    ("Your divine fury takes the form of:\n\r",
			     ch);
			send_to_char(ch->pcdata->wrath ? ch->pcdata->
				     wrath : "(None).\n\r", ch);
			return;
		    }
		}
	    }
	    buf[0] = '\0';
	    free_string(ch->pcdata->wrath);
	    ch->pcdata->wrath = str_dup(buf);
	    send_to_char
		("Mortals no longer need fear your mark upon them.\n\r",
		 ch);
	    return;
	}
	if (argument[0] == '+') {
	    if (ch->pcdata->wrath != NULL)
		strcat(buf, ch->pcdata->wrath);
	    argument++;
	    while (isspace(*argument))
		argument++;
	}
	if (strlen(buf) >= 1024) {
	    send_to_char
		("Your fury is great indeed, too great.  Use less lines.\n\r",
		 ch);
	    return;
	}
	strcat(buf, argument);
	strcat(buf, "\n\r");
	free_string(ch->pcdata->wrath);
	ch->pcdata->wrath = str_dup(buf);
    }
    send_to_char("Your divine fury takes the form of:\n\r", ch);
    send_to_char(ch->pcdata->wrath ? ch->pcdata->wrath : "(None).\n\r",
		 ch);
    return;
}


void do_wrath(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];	/* Lot of arguments */
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char arg5[MAX_INPUT_LENGTH];
    char arg6[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *SmittenEQ;
    const int MAX_WRATH_FRACTION = 95;
    int hp_percent = 0;
    int mana_percent = 0;
    int move_percent = 0;
    int pos = 0;
    int eq = 0;

    argument = one_argument(argument, arg1);	/* Combine the arguments */
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    argument = one_argument(argument, arg4);
    argument = one_argument(argument, arg5);
    argument = one_argument(argument, arg6);

    if (IS_NPC(ch))
	return;

    if (arg1[0] == '\0') {
	send_to_char("Syntax:\n\r", ch);
	send_to_char
	    ("Wrath <victim> <hp> <mana> <move> <position> <equipment>\n\r",
	     ch);
	return;
    }
    if ((victim = get_char_world(ch, arg1)) == NULL) {
	send_to_char("They are saved only through their absence.\n\r", ch);
	return;
    }
    if (!IS_NPC(victim) && get_trust(victim) >= get_trust(ch)) {
	send_to_char("Your reach exceeds your grasp.\n\r", ch);
	return;
    }
    if (arg2[0] != '\0')
	hp_percent = atoi(arg2);
    else
	hp_percent = 50;

    if (hp_percent > MAX_WRATH_FRACTION || hp_percent < 0) {
	send_to_char("Hp percent must be between 0 and 95.\n\r", ch);
	return;
    }
    if (arg3[0] != '\0')
	mana_percent = atoi(arg3);
    else
	mana_percent = 0;

    if (mana_percent > MAX_WRATH_FRACTION || mana_percent < 0) {
	send_to_char("Mana percent must be between 0 and 95.\n\r", ch);
	return;
    }
    if (arg4[0] != '\0')
	move_percent = atoi(arg4);
    else
	move_percent = 0;

    if (move_percent > MAX_WRATH_FRACTION || move_percent < 0) {
	send_to_char("Move percent must be between 0 and 95.\n\r", ch);
	return;
    }
    if (ch->alignment > 300) {
	act_new("Your actions have brought the holy power of $n upon you!",
		ch, NULL, victim, TO_VICT, POS_DEAD);
	act_new("$N has brought the holy power of $n upon themselves!", ch,
		NULL, victim, TO_NOTVICT, POS_DEAD);
    }
    if (ch->alignment > -301 && ch->alignment < 301) {
	act_new("Your actions have called the divine fury of $n upon you!",
		ch, NULL, victim, TO_VICT, POS_DEAD);
	act_new("$N has called the divine fury of $n upon themselves!", ch,
		NULL, victim, TO_NOTVICT, POS_DEAD);
    }
    if (ch->alignment < -300) {
	act_new("You are struck down by the infernal power of $n!!", ch,
		NULL, victim, TO_VICT, POS_DEAD);
	act_new
	    ("The hellspawned, infernal power of $n has struck down $N!!",
	     ch, NULL, victim, TO_NOTVICT, POS_DEAD);
    }
    if (ch->pcdata->wrath[0] != '\0') {
	send_to_char(ch->pcdata->wrath, victim);
    }
    if (hp_percent > 75 && victim->hit > victim->max_hit / 4)
	send_to_char("{RThat really did HURT!{x\n\r", victim);

    for (eq = 0; eq_table[eq] != NULL; eq++) {
	if (!str_prefix(eq_table[eq], arg6)) {
	    if ((SmittenEQ = get_eq_char(victim, eq)) != NULL) {
		sprintf(buf, "$n's %s is blown away!",
			SmittenEQ->short_descr);
		act(buf, victim, NULL, NULL, TO_ROOM);
		sprintf(buf, "Your %s is blown away!\n\r",
			SmittenEQ->short_descr);
		send_to_char(buf, victim);

		unequip_char(victim, SmittenEQ);
		obj_from_char(SmittenEQ);
		obj_to_room(SmittenEQ, victim->in_room);
	    }
	    break;
	}
    }

    for (pos = 0; pos_table[pos] != NULL; pos++) {
	if (!str_prefix(pos_table[pos], arg5)) {
	    victim->position = pos;	/* This only works because of the way the pos_table is arranged. */
	    if (victim->position == POS_FIGHTING)
		victim->position = POS_STANDING;	/* POS_FIGHTING is bad */

	    if (victim->position < POS_STUNNED)
		victim->position = POS_STUNNED;

	    if (victim->position == POS_STUNNED) {
		act("$n is stunned, but will probably recover.", victim,
		    NULL, NULL, TO_ROOM);
		send_to_char
		    ("You are stunned, but will probably recover.\n\r",
		     victim);
	    }
	    if (victim->position == POS_RESTING
		|| victim->position == POS_SITTING) {
		act("$n is knocked onto $s butt!", victim, NULL, NULL,
		    TO_ROOM);
		send_to_char("You are knocked onto your butt!\n\r",
			     victim);
	    }
	    break;
	}
    }

    /* Calculate total hp loss */
    victim->hit -= ((victim->hit * hp_percent) / 100);
    if (victim->hit < 1)
	victim->hit = 1;

    /* Calculate total mana loss */
    victim->mana -= ((victim->mana * mana_percent) / 100);
    if (victim->mana < 1)
	victim->mana = 1;

    /* Calculate total move loss */
    victim->move -= ((victim->move * move_percent) / 100);
    if (victim->move < 1)
	victim->move = 1;

    send_to_char("Your will is done, your power felt.\n\r", ch);
    return;
}


bool has_number(char *string)
{
    if (string[0] == '\0')
	return FALSE;

    for (; *string != '\0'; string++) {
	if (isdigit(*string))
	    return TRUE;
    }
    return FALSE;
}
