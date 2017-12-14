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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "recycle.h"

extern void do_land(CHAR_DATA * ch, char *arguement);

/*******************************************************************************
* FILE: new.spell.effects.c                                                    *
* PURPOSE: To get more data for the spells, mainly CHAR_DATA, and the ability  *
*   to send different values to the spells to make them	more robust.           *
* BASIC METHOD SIGNATURE:                                                      *
*   void affect_new_effect(CHAR_DATA *ch, void *vo, int x, int y, int z)       *
*     where x, y, z, can be any variable you want and where void can be another*
*     return type.                                                             *
* COMMON METHOD FORM:                                                          *
*   void affect_new_effect(CHAR_DATA *ch, void *vo, int bonus, int relative,   *
*     int absolute)                                                            *
*       where bonus is positive or negative, relative affects things like the  *
*       amount of time, and absolute affects things like time or whether       *
*       something is done regardless of other influences.                      *
* AUTHOR: Severhand                                                            *
*******************************************************************************/

/*******************************************************************************
* FUNCTION: displace_new_effect                                                *
* PURPOSE: Checks whether an attack hit the displaced version of the victim    *
* OPTIONS:                                                                     *
*   Bonus - Number (positive or negative) that affects the chance to hit the   *
*     displaced image.                                                         *
*   Absolute - Makes the character hit or miss regardless of anything else.    *
*     -A positive integer will make the character hit the displaced version.   *
*     -A negative integer will make the character hit the victim               *
*     -0 is the default value.                                                 *
* RETURNS:                                                                     *
*   TRUE if the character hits displaced version                               *
*   FALSE if the character hits victim                                         *
* NOTES: Can displace from whirls, circles, bashes, trips, regular attacks,    *
*   dirt kicks, backstabs, assassinates, kicks, arials, disarms, and rescue    *
*   attempts (although at a lesser chance).                                    *
* AUTHOR: Severhand                                                            *
*******************************************************************************/
bool displace_new_effect(CHAR_DATA *ch, void *vo, int bonus, int y, int absolute)
{
	char buf[MAX_STRING_LENGTH];//take out when finished testing
	CHAR_DATA *victim;
	int chance, int_difference, wis_difference, total_difference;

	//Base chance of 20%
	chance = 20 + bonus;
	victim = (CHAR_DATA *) vo;

	/*I know this doesn't seem logical getting the victim's stats first,
			but it works that way for some reason.*/
	int_difference = get_curr_stat(victim, STAT_INT) - get_curr_stat(ch, STAT_INT);
	wis_difference = get_curr_stat(victim, STAT_WIS) - get_curr_stat(ch, STAT_WIS);
	total_difference = int_difference + wis_difference;

	chance += number_percent() % 5 - number_percent() % 5;//randomness
	chance += total_difference;

	/*Spell Checks
		All numbers are arbitrary.  Feel free to change.*/
	if(is_affected(victim, skill_lookup("curse")))
		chance += 2;
	if(is_affected(victim, skill_lookup("berserk")))
		chance += 2;
	if(is_affected(victim, skill_lookup("frenzy")))
		chance += 2;
	if(is_affected(victim, skill_lookup("blind")))
		chance += 20;
	if(is_affected(victim, skill_lookup("bless")))
		chance -= 2;

	/*Tracing.  Comment out when finished testing.
	  Add your name if you wanna see how it goes.*/
	if(strcmp(ch->name, "Severhand") == 0)
	{
		sprintf( buf, "{YTotal Chance is %d"
		"Int_difference is %d"
		"Wis_difference is %d"
		"Total_difference is %d"
		"Bonus is %d"
		"Absolute is %d{x\r\n", chance, int_difference, wis_difference, total_difference,
		bonus, absolute);
		send_to_char(buf, victim); 
	}

	if(absolute < 0)
		return FALSE;//hit victim
	else if(number_percent() < chance || absolute > 0)
		return TRUE;//hit displaced

	return FALSE;//hit victim
}

/*******************************************************************************
* FUNCTION: stun_new_effect                                                    *
* PURPOSE: Stuns a victim.                                                     *
* OPTIONS:                                                                     *
*   Bonus - Number (positive or negative) that affects the chance to stun a    *
*     victim.                                                                  *
*   Rlength - Changes relative length of stun time (positive or negative)      *
*   Alength - Sets an absolute time the victim is stunned regardless of        *
*     anything else.  Value must be positive.                                  *
* RETURNS: (nothing)                                                           *
* NOTES: Stun time not cumulative.  If victim already stunned, check will fail.*
*   This is a wait_state, not a daze_state.  Immortals cannot be affected.     *
* AUTHOR: Severhand                                                            *
*******************************************************************************/
void stun_new_effect(CHAR_DATA *ch, void *vo, int bonus, int rlength, int alength)
{
	char buf[MAX_STRING_LENGTH];//take out when finished testing
	CHAR_DATA *victim;
	int chance, level_difference, int_difference, wis_difference, total_difference;

	victim = (CHAR_DATA *) vo;
	chance = bonus; 

	/*Spell Checks
		All numbers are arbitrary.  Feel free to change.*/
	if(is_affected(victim, skill_lookup("curse")))
		chance += 5;
	if(IS_GOOD(ch) && is_affected(victim, skill_lookup("protection good")))
		chance -= 5;
	if(IS_EVIL(ch) && is_affected(victim, skill_lookup("protection evil")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("berserk")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("frenzy")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("bless")))
		chance -= 5;

	level_difference = GET_LEVEL(ch) - GET_LEVEL(victim);
	int_difference = get_curr_stat(victim, STAT_INT) - get_curr_stat(ch, STAT_INT);
	wis_difference = get_curr_stat(victim, STAT_WIS) - get_curr_stat(ch, STAT_WIS);
	total_difference = level_difference + int_difference + wis_difference;

	if(total_difference + rlength <= 0 || alength < 0)
		return;//Stun duration <= 0

	chance += total_difference;

	//Randomness
	chance += number_percent() % 20 - number_percent() % 20;

	/*Tracing.  Comment out when finished testing.
	  Add your name if you wanna see how it goes.*/
	if(strcmp(ch->name, "Severhand") == 0)
	{
		sprintf( buf, "{YTotal Chance is %d\r\n"
		"Total_difference is %d\r\n"
		"Bonus is %d\r\n"
		"Relative length is %d\r\n"
		"Absolute length is %d\r\n"
		"Total + Relative is %d{x\r\n", chance,total_difference, bonus, rlength,
		alength, (total_difference + rlength));
		send_to_char(buf, ch); 
	}

	/*No screwing player by cumulative stun spells.  Must wait for current one
			to end*/
	if(!IS_IMMORTAL(victim) && number_percent() < chance && victim->wait == 0)
	{
		victim->position = POS_STUNNED;
		if(alength != 0)
			WAIT_STATE(victim, alength);
		else
			WAIT_STATE(victim, (total_difference + rlength));
		update_pos(victim);
		
		act("You are stunned.{x", victim, NULL, NULL, TO_CHAR);
    act("$n suddenly appears to be stunned.", victim, NULL, NULL, TO_ROOM);
	}
	return;
}

//Change race
void polymorph_new_effect(CHAR_DATA *ch, void *vo, int bonus, int absolute, int z)
{
	return;
}

/*******************************************************************************
* FUNCTION: disarm_new_effect                                                  *
* PURPOSE: Disarms a victim.                                                   *
* OPTIONS:                                                                     *
*   Bonus - Number (positive or negative) that affects the chance to disarm    *
*     the victim.                                                              *
*   Absolute - Determines if victim is automatically disarmed.                 *
*     -Positive integers automatically disarm the victim.                      *
*     -0 is default.                                                           *
*     -Negative integers automatically fail to disarm the victim.              *
* RETURNS: (nothing)                                                           *
* NOTES: Victim cannot be disarmed if they are not wielding a weapon, if the   *
*   weapon's flag is NO_REMOVE, or if they are affected by the "talon" spell.  *
* AUTHOR: Severhand                                                            *
*******************************************************************************/
void disarm_new_effect(CHAR_DATA *ch, void *vo, int bonus, int y, int absolute)
{
	char buf[MAX_STRING_LENGTH];//take out when finished testing
	CHAR_DATA *victim;
  OBJ_DATA *obj;
  int chance;

	victim = (CHAR_DATA *) vo;
	chance = bonus;

	if((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
		return;
	if(IS_OBJ_STAT(obj, ITEM_NOREMOVE))
		return;
	if(is_affected(victim, skill_lookup("talon")))
		return;
	
	/*Spell Checks
		All numbers are arbitrary.  Feel free to change.*/
	if(is_affected(victim, skill_lookup("haste")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("curse")))
		chance += 5;
	if(is_affected(victim, skill_lookup("berserk")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("lethargy")))
		chance += 5;
	if(is_affected(victim, skill_lookup("frenzy")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("bless")))
		chance -= 5;

	/*Need to get a better formula.  Until then, this is disabled
		because it adds too much.
	chance -= get_weapon_skill(victim, get_weapon_sn(victim));*/
	chance -= get_curr_stat(victim, STAT_STR);
	chance -= get_curr_stat(victim, STAT_DEX);

	//Randomness
	chance += number_percent() % 20 - number_percent() % 20;

	/*Tracing.  Comment out when finished testing.
	  Add your name if you wanna see how it goes.*/
	if(strcmp(ch->name, "Severhand") == 0)
	{
		sprintf( buf, "{YEnd chance is %d{x\r\n", chance);
		send_to_char(buf, ch); 
	}

	if(absolute < 0)
		return;//not disarmed
	else if(number_percent() < chance || absolute > 0)
	{
		act("You drop your weapon.{x", victim, NULL, NULL, TO_CHAR);
    act("$n drops $s weapon.", victim, NULL, NULL, TO_ROOM);

    obj_from_char(obj);
    if (IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_INVENTORY))
			obj_to_char(obj, victim);
    else
		{
			obj_to_room(obj, victim->in_room);
			if (IS_NPC(victim) && victim->wait == 0
					&& can_see_obj(victim, obj))
			DAZE_STATE(victim, PULSE_VIOLENCE);
    }
	}
	return;
}

/*******************************************************************************
* FUNCTION: floor_new_effect                                                   *
* PURPOSE: Knocks someone down to POS_RESTING.                                 *
* OPTIONS:                                                                     *
*   Bonus - Number (positive or negative) that affects the chance to knock down*
*     the victim.                                                              *
*   Absolute - Determines if victim is automatically knocked down to           *
*     POS_RESTING.                                                             *
*       -Positive integers automatically knock down the victim to POS_RESTING. *
*       -0 is default.                                                         *
*       -Negative integers automatically fail to knock down the victim to      *
*        POS_RESTING.                                                          *
* RETURNS: (nothing)                                                           *
* NOTES: Flying creatures land if they are knocked down.                       *
* AUTHOR: Severhand                                                            *
*******************************************************************************/
void floor_new_effect(CHAR_DATA *ch, void *vo, int bonus, int y, int absolute)
{
	int chance, random;
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];//take out when finished testing

	chance = bonus;
	random = number_percent() % 20 - number_percent() % 20;//randomness
	victim = (CHAR_DATA *) vo;
	

	if(victim->position < POS_FIGHTING)
		return; //already on ground

	/*Spell Checks
		All numbers are arbitrary.  Feel free to change.*/
	if(is_affected(victim, skill_lookup("flying")))
		chance -= 10;
	if(is_affected(victim, skill_lookup("haste")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("blind")))
		chance += 20;
	if(is_affected(victim, skill_lookup("curse")))
		chance += 5;
	if(is_affected(victim, skill_lookup("berserk")))
		chance -= 10;
	if(is_affected(victim, skill_lookup("lethargy")))
		chance += 10;
	if(is_affected(victim, skill_lookup("bless")))
		chance -= 5;
	if(is_affected(victim, skill_lookup("frenzy")))
		chance -= 10;

	chance -= victim->size * 5;//big guys hard to knock down
	chance -= get_curr_stat(victim, STAT_STR);
	chance -= get_curr_stat(victim, STAT_DEX);
	chance += random;

	/*Tracing.  Comment out when finished testing.
	  Add your name if you wanna see how it goes.*/
	if(strcmp(ch->name, "Severhand") == 0)
	{
		sprintf( buf, "{YEnd chance is %d\n"
		"Bonus is %d\n"
		"Absolute is %d\n{x\r\n", chance, bonus, absolute);
		send_to_char(buf, ch); 
	}

	if(absolute < 0)
		return;
	else if(number_percent() < chance || absolute > 0)
	{
		if(is_affected(victim, skill_lookup("flying"))
			|| IS_AFFECTED(victim, AFF_FLYING))
			do_land(victim, "");
		victim->position = POS_RESTING;
		update_pos(victim);
		act("You fall on your butt.", victim, NULL,
				NULL, TO_CHAR);
		act("$n falls on $s butt.", victim, NULL,
				NULL, TO_ROOM);
		DAZE_STATE(victim, PULSE_VIOLENCE);
	}
	return;
}

