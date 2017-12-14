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

/***************************************************************************
*       File utils.h:                                                      *
*       utility macro definitions for all files.                           *
*       Alanthia MUD, wdl, 4.8.98                                          *
***************************************************************************/

/*
 * Utility macros.
 */
#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))


/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_LEADER(ch)		(IS_SET((ch)->act, PLR_LEADER))
#define IS_DEL(ch)              (IS_SET((ch)->act, PLR_DEL))
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_UNDEAD(victim)	(!IS_NPC(victim) && ( (victim)->race == race_lookup("vampire") || (victim)->race == race_lookup("wraith") || (victim)->race == race_lookup("demon") || (victim)->race == race_lookup("gargoyle")))
#define IS_VAMPIRE(ch)          (!IS_NPC(ch) && (ch)->race == race_lookup("vampire"))
#define IS_WRAITH(ch)	(!IS_NPC(ch) && (ch)->race == race_lookup("wraith"))
#define IS_DWARF(ch)	(!IS_NPC(ch) && (ch)->race == race_lookup("dwarf"))
									

#define DOESNT_USE_BLOOD(ch)	(!IS_DRAGON(ch)	\
							&&	!IS_VAMPIRE(ch)	\
                            &&  !IS_DWARF(ch))

/* Don't put any race definitions below here. */


#define GET_AGE(ch)		((int) (10 + ((ch)->played \
				    + current_time - (ch)->logon )/72000))
#define GET_LEVEL(ch)		(ch->level)

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch,type)		((ch)->armor[type]			    \
		        + ( IS_AWAKE(ch)			    \
			? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch)	\
		((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
		((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define get_carry_weight(ch)    ((ch)->carry_weight)
#define HAS_TRIGGER(ch, trig)   (IS_SET((ch)->pIndexData->mprog_flags,(trig)))



/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? \
	(obj)->value[4] : 100)



/*
 * Description macros.
 */
#define PERS(ch, looker) ( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : "someone" )

#define OBJS(ch, obj)	( can_see_obj( ch, (obj) ) ?		\
				(obj)->short_descr	\
				: "something" )

/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
                                }

#define KEYS( literal, field, value )                   \
                if ( !str_cmp( word, literal ) )    \
                {                   \
                    free_string(field);         \
                    field  = value;         \
                    fMatch = TRUE;          \
                    break;              \
                }




#define IS_PKILL(ch) (IS_SET(ch->act, PLR_PKILL) || (ch->tmp_pkill))
#define HAS_IMMUNITY(ch) (ch->imm_flags != 0)
#define DAYTIME ((time_info.hour >= 6) && (time_info.hour < 19))

#define GET_HUNGER(ch) (IS_NPC(ch) ? -1 : (ch->pcdata->condition[COND_HUNGER]))
#define GET_THIRST(ch) (IS_NPC(ch) ? -1 : (ch->pcdata->condition[COND_THIRST]))
#define GET_DRUNK(ch)  (IS_NPC(ch) ? -1 : (ch->pcdata->condition[COND_DRUNK]))

#define IS_FIGHTING(ch)	(ch->position == POS_FIGHTING)

#define GET_CWAIT(ch)   (ch->cwait)
#define GET_TIME_COST(attack)   (attack_table[attack].ctimer)
#define COMBAT_WAIT_LENGTH 100
#define GET_CURRENT_ATTACK(ch)  (ch->attack_stack[ch->current_attack])
#define GET_NEXT_ATTACK(ch) (ch->attack_stack[ch->current_attack +1])

#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))


/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_LEADER(ch)		(IS_SET((ch)->act, PLR_LEADER))
#define IS_DEL(ch)              (IS_SET((ch)->act, PLR_DEL))
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)	(get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_VAMPIRE(ch)          (!IS_NPC(ch) && (ch)->race == race_lookup("vampire"))
#define IS_WRAITH(ch)	(!IS_NPC(ch) && (ch)->race == race_lookup("wraith"))
#define IS_DWARF(ch)	(!IS_NPC(ch) && (ch)->race == race_lookup("dwarf"))
#define IS_DRAGON(ch)	(!IS_NPC(ch) && ( (ch)->race == race_lookup("whtdrg") \
									|| (ch)->race == race_lookup("blkdrg") \
									|| (ch)->race == race_lookup("bludrg") \
									|| (ch)->race == race_lookup("grndrg") \
									|| (ch)->race == race_lookup("reddrg")))

#define GET_LEVEL(ch)		(ch->level)

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch,type)		((ch)->armor[type]			    \
		        + ( IS_AWAKE(ch)			    \
			? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch)	\
		((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
		((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, \
				(npulse * 10)))

#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, \
				(npulse * 10)))

#define get_carry_weight(ch)    ((ch)->carry_weight)
#define HAS_TRIGGER(ch, trig)   (IS_SET((ch)->pIndexData->mprog_flags,(trig)))



/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)	((obj)->item_type == ITEM_CONTAINER ? \
	(obj)->value[4] : 100)



/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : "someone" )

#define HAS_IMMUNITY(ch) (ch->imm_flags != 0)
#define DAYTIME ((time_info.hour >= 6) && (time_info.hour < 19))
#define IS_FIGHTING(ch)	(ch->position == POS_FIGHTING)
#define GET_CWAIT(ch)	(ch->cwait)

/*
 * character macros to trap pc / npc
 */

#define NAME(ch) (IS_NPC(ch) ? ch->short_descr : ch->name)
#define SEX_NAME(ch) (ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female")
#define RACE_NAME(ch)  (ch->race->name)
#define DEVOTION_NAME(ch) (IS_NPC(ch) ? "nonparticipant" : devotion_name[ch->devotion])
#define GOD_NAME(ch) (IS_NPC(ch) ? "no god" : god_name[ch->diety])
#define ALIGN_FAME(ch) (IS_NPC(ch) ? "unknown" : \
(ch->alignment < 3 ? lawful_fame_name[ch->fame] \
: chaotic_fame_name[ch->fame]))
#define ALIGN_NAME(ch) (alignment_name[ch->alignment])

#define IS_VOWEL(c)	\
	((c) == 'a' || (c) == 'A'	\
	|| (c) == 'e' || (c) == 'E'	\
	|| (c) == 'i' || (c) == 'I'	\
	|| (c) == 'o' || (c) == 'O'	\
	|| (c) == 'u' || (c) == 'U')
