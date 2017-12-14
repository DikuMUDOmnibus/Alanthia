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
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const struct skill_type skill_table[MAX_SKILL] = {

/*
 * Magic spells.
 *

 * Name of skill                *
 * Level needed by class        * {Mage, Cleric, Thief, Warrior, Asassin, Paladin, Dragoon}
 * How hard it is to learn      *
 * Spell pointer (for spells)   *
 * Legal targets                *
 * Position for caster / user   *
 * Pointer to associated gsn    *
 * Slot for #OBJECT loading     *
 * Minimum mana used            *
 * Waiting time after use       *
 * Damage message               *
 * Wear off message             *
 * Wear off message for objects *

 */
    {
     "reserved",
     {201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {99, 99, 99, 99, 99, 99, 99, 201, 201, 201, 201, 201, 201, 201 },
     0, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(0), 0, 0,
     "", "", ""},

    {
     "acid blast",
     {35, 130, 100, 100, 100, 130, 100, 201, 201, 201, 201, 201, 201, 201},
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_acid_blast, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(70), 35, 48,
     "acid blast", "!Acid Blast!", ""},

    {
     "animate dead",
     {20, 125, 125, 125, 125, 125, 1, 201, 201, 201, 201, 201, 201, 201},
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_animate_dead, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(681), 100, 30,
     "", "!Animate dead!", ""},

    {
     "armor",
     {5, 2, 60, 100, 60, 40, 1, 201, 201, 201, 201, 201, 201, 201},
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_armor, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(1), 5, 24,
     "", "You feel less armored.", ""},

    {
     "bless",
     {150, 7, 150, 150, 150, 9, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_bless, TAR_OBJ_CHAR_DEF, POS_STANDING,
     NULL, SLOT(3), 5, 24,
     "", "You feel less righteous.", "$p's holy aura fades."},

    {
     "blindness",
     {12, 8, 17, 15, 17, 75, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_blindness, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_blindness, SLOT(4), 5, 24,
     "", "You can see again.", ""},

		{//Severhand
     "boulder",
     {120, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_boulder, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(553), 15, 48,
     "boulder", "!Boulder!", ""},

		{//Severhand's Experiment Spell
		 "bullshit",
		 {208,208,208,208,208,208,208,208,208,208,208,208,208,208},
		 {208,208,208,208,208,208,208,208,208,208,208,208,208,208},
		 spell_bullshit, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
		 NULL, SLOT(550), 50, 15,
		 "bullshit", "!Bullshit!", ""},
		 

    {
     "burning hands",
     {7, 35, 100, 100, 100, 75, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_burning_hands, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(5), 15, 24,
     "burning hands", "!Burning Hands!", ""},

    {
     "call lightning",
     {40, 18, 1, 1, 1, 35, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_call_lightning, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT(6), 15, 24,
     "lightning bolt", "!Call Lightning!", ""},

    {
     "calm",
     {48, 16, 50, 20, 50, 24, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_calm, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT(509), 30, 24,
     "", "You have lost your peace of mind.", ""},

    {
     "cancellation",
     {18, 26, 34, 34, 34, 52, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cancellation, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(507), 20, 24,
     "" "!cancellation!", ""},

    {
     "cause critical",
     {75, 13, 150, 150, 150, 19, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cause_critical, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(63), 20, 24,
     "spell", "!Cause Critical!", ""},

    {
     "cause light",
     {70, 1, 149, 3, 149, 5, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cause_light, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(62), 15, 24,
     "spell", "!Cause Light!", ""},

    {
     "cause serious",
     {73, 7, 150, 150, 150, 13, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cause_serious, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(64), 17, 24,
     "spell", "!Cause Serious!", ""},

    {
     "chain lightning",
     {33, 130, 39, 36, 39, 110, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_chain_lightning, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(500), 25, 24,
     "lightning", "!Chain Lightning!", ""},

    {
     "change sex",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_change_sex, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(82), 15, 24,
     "", "Your body feels familiar again.", ""},

    {
     "charm person",
     {20, 1, 25, 1, 25, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_charm_person, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_charm_person, SLOT(7), 5, 24,
     "", "You feel more self-confident.", ""},

    {
     "chill touch",
     {4, 45, 6, 6, 6, 45, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_chill_touch, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(8), 15, 24,
     "chilling touch", "You feel less cold.", ""},

    {
     "colour spray",
     {16, 80, 22, 20, 22, 75, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_colour_spray, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(10), 15, 24,
     "colour spray", "!Colour Spray!", ""},

    {
     "continual light",
     {6, 4, 6, 9, 6, 10, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_continual_light, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(57), 7, 24,
     "", "!Continual Light!", ""},

    {
     "control weather",
     {15, 19, 28, 22, 28, 80, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_control_weather, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(11), 25, 24,
     "", "!Control Weather!", ""},

    {
     "create food",
     {10, 5, 11, 12, 11, 13, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_create_food, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(12), 5, 24,
     "", "!Create Food!", ""},

    {
     "create rose",
     {16, 11, 10, 24, 10, 13, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_create_rose, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(511), 30, 24,
     "", "!Create Rose!", ""},

    {
     "sober",
     {7, 3, 14, 10, 12, 13, 2, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_sober, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(14), 5, 24,
     "", "!Sober!", ""},

    {
     "create spring",
     {14, 17, 23, 20, 23, 23, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_create_spring, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(80), 20, 24,
     "", "!Create Spring!", ""},

    {
     "create water",
     {8, 3, 12, 11, 12, 13, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_create_water, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(13), 5, 24,
     "", "!Create Water!", ""},

    {
     "cure blindness",
     {30, 6, 90, 8, 90, 9, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cure_blindness, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(14), 5, 24,
     "", "!Cure Blindness!", ""},

    {
     "cure critical",
     {90, 13, 90, 19, 90, 20, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cure_critical, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(15), 20, 24,
     "", "!Cure Critical!", ""},

    {
     "cure disease",
     {30, 13, 90, 14, 90, 16, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cure_disease, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(501), 20, 24,
     "", "!Cure Disease!", ""},

    {
     "cure light",
     {20, 1, 30, 30, 45, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cure_light, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(16), 10, 24,
     "", "!Cure Light!", ""},

    {
     "cure poison",
     {65, 14, 65, 16, 65, 15, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cure_poison, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(43), 5, 24,
     "", "!Cure Poison!", ""},

    {
     "cure serious",
     {60, 7, 60, 10, 60, 9, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_cure_serious, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(61), 15, 24,
     "", "!Cure Serious!", ""},

    {
     "curse",
     {18, 18, 26, 22, 26, 45, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_curse, TAR_OBJ_CHAR_OFF, POS_FIGHTING,
     &gsn_curse, SLOT(17), 20, 24,
     "curse", "The curse wears off.", "$p is no longer impure."},

		{//Severhand
     "death ray",
     {180, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_death_ray, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(555), 15, 48,
     "death ray", "!Death Ray!", ""},

    {
     "demonfire",
     {150, 34, 150, 45, 150, 40, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_demonfire, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(505), 20, 48,
     "torments", "!Demonfire!", ""},

    {
     "detect evil",
     {11, 4, 12, 45, 201, 45, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 201, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_detect_evil, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(18), 5, 24,
     "", "The red in your vision disappears.", ""},

    {
     "detect good",
     {11, 4, 12, 45, 201, 40, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 201, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_detect_good, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(513), 5, 24,
     "", "The gold in your vision disappears.", ""},

    {
     "detect hidden",
     {15, 11, 12, 45, 22, 40, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_detect_hidden, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(44), 5, 24,
     "", "You feel less aware of your surroundings.",
     ""},

    {
     "detect invis",
     {3, 8, 6, 48, 40, 42, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_detect_invis, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(19), 5, 24,
     "", "You no longer see invisible objects.",
     ""},

    {
     "detect magic",
     {2, 6, 5, 90, 201, 41, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 201, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_detect_magic, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(20), 5, 24,
     "", "The detect magic wears off.", ""},

    {
     "detect poison",
     {15, 7, 9, 65, 15, 23, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_detect_poison, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(21), 5, 24,
     "", "!Detect Poison!", ""},

    {
     "diamond skin",
     {115, 110, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {6, 5, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_diamond_skin, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(667), 60, 36,
     "", "Your skin loses it's diamond-like quality.", ""},

    {
     "dispel evil",
     {90, 15, 100, 21, 100, 28, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_dispel_evil, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(22), 15, 24,
     "dispel evil", "!Dispel Evil!", ""},

    {
     "dispel good",
     {90, 15, 100, 21, 100, 150, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_dispel_good, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(512), 15, 24,
     "dispel good", "!Dispel Good!", ""},

    {
     "dispel magic",
     {16, 24, 30, 30, 30, 90, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_dispel_magic, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(59), 15, 24,
     "", "!Dispel Magic!", ""},

		{//Severhand
     "displace",
     {100, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_displace, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(558), 100, 100,
     "", "You are no longer displaced.", ""},

    {
     "earthquake",
     {75, 10, 90, 14, 90, 25, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_earthquake, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT(23), 15, 24,
     "earthquake", "!Earthquake!", ""},

    {
     "enchant armor",
     {16, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {2, 2, 4, 4, 4, 4, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_enchant_armor, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(510), 100, 48,
     "", "!Enchant Armor!", ""},

    {
     "enchant weapon",
     {17, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {2, 2, 4, 4, 4, 4, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_enchant_weapon, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(24), 100, 48,
     "", "!Enchant Weapon!", ""},

    {
     "energy drain",
     {19, 22, 26, 23, 26, 45, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_energy_drain, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(25), 35, 24,
     "energy drain", "!Energy Drain!", ""},

    {
     "faerie fire",
     {6, 3, 5, 8, 5, 15, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_faerie_fire, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(72), 5, 24,
     "faerie fire", "The pink aura around you fades away.",
     ""},

    {
     "faerie fog",
     {14, 21, 16, 24, 16, 18, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_faerie_fog, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(73), 12, 24,
     "faerie fog", "!Faerie Fog!", ""},

    {
     "farsight",
     {14, 16, 16, 60, 201, 15, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 201, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_farsight, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(521), 36, 40,
     "farsight", "!Farsight!", ""},

    {
     "fireball",
     {22, 80, 30, 26, 30, 80, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_fireball, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(26), 15, 48,
     "fireball", "!Fireball!", ""},

    {
     "fireproof",
     {13, 12, 19, 18, 19, 201, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 201, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_fireproof, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(523), 10, 24,
     "", "", "$p's protective aura fades."},

		 {//Severhand
     "firestream",
     {100, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_firestream, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(552), 15, 48,
     "firestream", "!Firestream!", ""},

    {
     "flamestrike",
     {40, 20, 50, 27, 50, 28, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_flamestrike, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(65), 20, 24,
     "flamestrike", "!Flamestrike!", ""},

    {
     "fly",
     {10, 18, 20, 22, 20, 35, 20, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_fly, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(56), 10, 36,
     "", "You slowly float to the ground.", ""},

    {
     "floating disc",
     {4, 10, 7, 16, 7, 32, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_floating_disc, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(522), 40, 48,
     "", "!Floating disc!", ""},

    {
     "force shield",
     {135, 130, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     {6, 5, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     spell_force_shield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(669), 70, 36,
     "", "Your force shield shimmers then fades away.",
     ""},

    {
     "frenzy",
     {75, 24, 90, 26, 90, 34, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_frenzy, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(504), 30, 48,
     "", "Your rage ebbs.", ""},

    {
     "gate",
     {27, 17, 32, 28, 32, 40, 35, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_gate, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT(83), 80, 24,
     "", "!Gate!", ""},

    {
     "giant strength",
     {11, 75, 22, 20, 22, 60, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_giant_strength, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(39), 20, 24,
     "", "You feel weaker.", ""},

		 /*
		{//Severhand
     "gravity",
     {50, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_gravity, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(557), 15, 48,
     "gravity", "!Gravity!", ""},
		 */

    {
     "might of heros",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_might_of_heros, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(700), 20, 24,
     "", "You feel weaker.", ""},

    {
     "mind of sages",
     {1, 2, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_mind_of_sages, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(701), 20, 24,
     "", "Your mind dulls.", ""},

    {
     "hands of wind",
     {1, 3, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_hands_of_wind, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(702), 20, 24,
     "", "You feel less springy.", ""},

    {
     "sight of ages",
     {1, 4, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_sight_of_ages, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(703), 20, 24,
     "", "You feel less wise.", ""},

    {
     "heart of the dragon",
     {1, 5, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_heart_of_the_dragon, TAR_CHAR_DEFENSIVE,
     POS_STANDING,
     NULL, SLOT(704), 20, 24,
     "", "You feel less healthy.", ""},

    {
     "guardian",
     {20, 60, 25, 80, 25, 201, 65, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 201, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_guardian, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(680), 100, 30,
     "", "!guardian!", "",
     },

    {
     "harm",
     {75, 23, 90, 28, 90, 35, 201, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201, 201 },
     spell_harm, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(27), 35, 24,
     "harm spell", "!Harm!,		" ""},

    {
     "haste",
     {21, 45, 26, 29, 26, 40, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_haste, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(502), 30, 24,
     "", "You feel yourself slow down.", ""},

    {
     "heal",
     {90, 21, 33, 30, 33, 35, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_heal, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(28), 50, 24,
     "", "!Heal!", ""},

    {
     "heat metal",
     {18, 16, 45, 23, 55, 35, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_heat_metal, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(516), 25, 36,
     "spell", "!Heat Metal!", ""},

    {
     "heavenly sanctuary",
     {145, 140, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     {7, 6, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     spell_heavenly_sanctuary, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_heavenly_sanctuary, SLOT(668), 20, 24,
     "", "The silver aura around your body fades.",
     ""},

    {
     "holy armor",
     {201, 105, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     {201, 6, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     spell_holy_armor, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(670), 65, 24,
     "", "You feel less holy armored.", ""},

    {
     "holy word",
     {77, 36, 93, 42, 93, 50, 1, 201, 201, 201, 201, 201, 201, 201 },
     {2, 2, 4, 4, 4, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_holy_word, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT(506), 200, 48,
     "divine wrath", "!Holy Word!", ""},

		{//Severhand
     "ice shard",
     {80, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_ice_shard, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(551), 15, 48,
     "ice shard", "!Ice Shard!", ""},

    {
     "identify",
     {15, 16, 18, 35, 48, 25, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_identify, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(53), 12, 48,
     "", "!Identify!", ""},

    {
     "infravision",
     {9, 13, 10, 16, 10, 20, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_infravision, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(77), 5, 36,
     "", "You no longer see in the dark.", ""},

    {
     "invisibility",
     {5, 20, 9, 30, 9, 20, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_invis, TAR_OBJ_CHAR_DEF, POS_STANDING,
     &gsn_invis, SLOT(29), 5, 24,
     "", "You are no longer invisible.",
     "$p fades into view."},

    {
     "know alignment",
     {12, 9, 20, 98, 201, 15, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 201, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_know_alignment, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(58), 9, 24,
     "", "!Know Alignment!", ""},

    {
     "lethargy",
     {20, 25, 28, 30, 28, 35, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_lethargy, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(690), 30, 24,
     "", "You feel less lethargic.", ""},

    {
     "bolt",
     {13, 23, 18, 16, 18, 25, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_bolt, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(30), 15, 24,
     "bolt", "!Bolt!", ""},

    {
     "locate object",
     {9, 15, 11, 50, 1, 201, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 201, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_locate_object, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(31), 20, 36,
     "", "!Locate Object!", ""},

    {
     "magic missile",
     {1, 8, 2, 2, 2, 8, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2},
     spell_magic_missile, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(32), 15, 24,
     "magic missile", "!Magic Missile!", ""},

    {
     "mass healing",
     {40, 38, 80, 46, 80, 45, 1, 201, 201, 201, 201, 201, 201, 201 },
     {2, 2, 4, 4, 4, 4, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_mass_healing, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(508), 100, 72,
     "", "!Mass Healing!", ""},

    {
     "mass invis",
     {22, 25, 31, 60, 31, 40, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_mass_invis, TAR_IGNORE, POS_STANDING,
     &gsn_mass_invis, SLOT(69), 20, 48,
     "", "You are no longer invisible.", ""},

    {
     "nexus",
     {40, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201 },
     spell_nexus, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(520), 150, 72,
     "", "!Nexus!", ""},

    {
     "nova",
     {65, 65, 65, 65, 65, 65, 65, 201, 201, 201, 201, 201, 201, 201 },
     {2, 2, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_nova, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(26), 55, 50,
     "nova", "!Nova!", ""},

		 {//Severhand
     "paralysis",
     {100, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_paralysis, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(557), 15, 48,
     "paralysis", "!Paralysis!", ""},

    {
     "pass door",
     {24, 32, 25, 37, 25, 33, 28, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_pass_door, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(74), 20, 24,
     "", "You feel solid again.", ""},

    {
     "plague",
     {23, 17, 36, 26, 36, 42, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_plague, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_plague, SLOT(503), 20, 24,
     "sickness", "Your sores vanish.", ""},

    {
     "poison",
     {17, 12, 15, 21, 15, 23, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_poison, TAR_OBJ_CHAR_OFF, POS_FIGHTING,
     &gsn_poison, SLOT(33), 10, 24,
     "poison", "You feel less sick.",
     "The poison on $p dries up."},

    {
     "portal",
     {35, 30, 45, 40, 45, 55, 45, 201, 201, 201, 201, 201, 201, 201 },
     {2, 2, 4, 4, 4, 4, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_portal, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(519), 100, 48,
     "", "!Portal!", ""},

    {
     "protection evil",
     {12, 9, 17, 11, 17, 35, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_protection_evil, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(34), 5, 24,
     "", "You feel less protected.", ""},

    {
     "protection good",
     {12, 9, 17, 11, 17, 201, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 201, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_protection_good, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(514), 5, 24,
     "", "You feel less protected.", ""},

			{//Severhand
     "psychic crush",
     {140, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_psychic_crush, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(554), 15, 48,
     "psychic crush", "!Psychic Crush!", ""},

    {
     "ray of truth",
     {53, 35, 82, 47, 80, 45, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_ray_of_truth, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(518), 20, 48,
     "ray of truth", "!Ray of Truth!", ""},

    {
     "recharge",
     {9, 60, 90, 90, 90, 60, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_recharge, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT(517), 60, 48,
     "", "!Recharge!", ""},

    {
     "refresh",
     {8, 5, 12, 9, 12, 25, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 1, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_refresh, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(81), 12, 36,
     "refresh", "!Refresh!", ""},

    {
     "regeneration",
     {21, 46, 26, 29, 26, 31, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_regeneration, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(524), 30, 24,
     "", "You feel less vibrant.", ""},

    {
     "remove curse",
     {33, 18, 52, 22, 52, 21, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_remove_curse, TAR_OBJ_CHAR_DEF, POS_STANDING,
     NULL, SLOT(35), 5, 24,
     "", "!Remove Curse!", ""},

    {
     "remove invis",
     {37, 18, 48, 22, 48, 22, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_remove_invis, TAR_OBJ_CHAR_DEF, POS_STANDING,
     NULL, SLOT(682), 5, 24,
     "", "!Remove Invis!", ""},

    {
     "sanctuary",
     {36, 20, 42, 30, 42, 75, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_sanctuary, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_sanctuary, SLOT(36), 75, 24,
     "", "The white aura around your body fades.",
     ""},

    {
     "shield",
     {20, 35, 35, 40, 35, 55, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_shield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(67), 12, 36,
     "", "Your force shield shimmers then fades away.",
     ""},

    {
     "shocking grasp",
     {10, 42, 14, 13, 14, 48, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_shocking_grasp, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(53), 15, 24,
     "shocking grasp", "!Shocking Grasp!", ""},

    {
     "sleep",
     {10, 64, 11, 89, 11, 69, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_sleep, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_sleep, SLOT(38), 15, 24,
     "", "You feel less tired.", ""},

		 {//Severhand
     "sonic boom",
     {160, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     {1, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201, 201},
     spell_sonic_boom, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(556), 15, 48,
     "sonic boom", "!Sonic Boom!", ""},

    {
     "stone skin",
     {25, 40, 40, 45, 40, 56, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_stone_skin, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT(66), 12, 36,
     "", "Your skin feels soft again.", ""},

    {
     "summon",
     {24, 12, 29, 22, 29, 48, 25, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_summon, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(40), 50, 24,
     "", "!Summon!", ""},

    {
     "sunray",
     {15, 15, 15, 15, 15, 15, 15, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_sunray, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(26), 15, 24,
     "sunray", "!sunray!", ""},

    {
     "talon",
     {20, 30, 40, 40, 40, 35, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_talon, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT(697), 25, 24,
     "", "You lose your taloned grip.", ""},


    {
     "teleport",
     {13, 22, 25, 36, 25, 71, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_teleport, TAR_CHAR_SELF, POS_FIGHTING,
     NULL, SLOT(2), 35, 24,
     "", "!Teleport!", ""},

    {
     "teleport object",
     {40, 40, 70, 70, 70, 58, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_teleport_object, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(685), 35, 24,
     "", "!Teleport Object!", ""},

    {
     "thorn armor",
     {105, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {6, 2, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_thorn_armor, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT(671), 65, 24,
     "", "Your armor of thorns vanishes.", ""},

    {
     "ventriloquate",
     {1, 34, 68, 68, 68, 34, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 1, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_ventriloquate, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(41), 5, 24,
     "", "!Ventriloquate!", ""},

    {
     "weaken",
     {11, 14, 16, 17, 16, 23, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_weaken, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(68), 20, 24,
     "spell", "You feel stronger.", ""},

    {
     "word of recall",
     {32, 28, 40, 30, 40, 25, 1, 201, 201, 201, 201, 201, 201, 201 },
     {1, 1, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_word_of_recall, TAR_CHAR_SELF, POS_RESTING,
     NULL, SLOT(42), 5, 24,
     "", "!Word of Recall!", ""},

/*
 * Dragon breath
 */


    {
     "corrosive breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_corrosive_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(204), 100, 48,
     "blast of corrosive death", "!Corrosive Death Breath!", ""},

    {
     "fire breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_fire_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(201), 100, 48,
     "blast of flame", "The smoke leaves your eyes.", ""},


    {
     "frost breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_frost_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(202), 100, 48,
     "blast of frost", "!Frost Breath!", ""},

    {
     "venom breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_venom_breath, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT(203), 125, 48,
     "blast of venom mist", "!Venom Breath!", ""},

    {
     "stormsfury breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_stormsfury_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(204), 150, 48,
     "fury of the storm", "!Storms Fury Breath!", ""},

    {
     "ray breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_ray_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(205), 100, 48,
     "ray of light", "!Ray Breath!", ""},

    {
     "prismatic breath",
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     {200, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201, 201 },
     spell_prismatic_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(205), 100, 48,
     "prismatic blast", "!Prismatic Breath!", ""},
/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
     "general purpose",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {0, 0, 0, 0, 0, 0, 201, 201, 201, 201, 201, 201, 201 },
     spell_general_purpose, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(401), 0, 24,
     "general purpose ammo", "!General Purpose Ammo!", ""},

    {
     "high explosive",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {0, 0, 0, 0, 0, 0, 201, 201, 201, 201, 201, 201, 201 },
     spell_high_explosive, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT(402), 0, 24,
     "high explosive ammo", "!High Explosive Ammo!", ""},


/* combat and weapons skills *


   * Name of skill                *
   * Level needed by class        * {Mage, Cleric, Thief, Warrior, Assassin, Paladin, Dragoon}
   * How hard it is to learn      *
   * Spell pointer (for spells)   *
   * Legal targets                *
   * Position for caster / user   *
   * Pointer to associated gsn    *
   * Slot for #OBJECT loading     *
   * Minimum mana used            *
   * Waiting time after use       *
   * Damage message               *
   * Wear off message             *
   * Wear off message for obects  */


    {
     "axe",
     {192, 1, 192, 1, 192, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {8, 6, 8, 4, 8, 5, 4, 201, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_axe, SLOT(0), 0, 0,
     "", "!Axe!", ""},

    {
     "dagger",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {2, 3, 2, 2, 2, 3, 2, 201, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dagger, SLOT(0), 0, 0,
     "", "!Dagger!", ""},

    {
     "flail",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {6, 3, 6, 4, 3, 4, 3, 201, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_flail, SLOT(0), 0, 0,
     "", "!Flail!", ""},

    {
     "mace",
     {201, 1, 1, 1, 201, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {201, 2, 3, 5, 201, 3, 3, 201, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_mace, SLOT(0), 0, 0,
     "", "!Mace!", ""},

    {
     "polearm",
     {1, 1, 1, 1, 1, 5, 1, 1, 201, 201, 201, 201, 201, 201 },
     {-1, 6, -1, 4, -1, 5, 2, 2, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_polearm, SLOT(0), 0, 0,
     "", "!Polearm!", ""},

    {
     "shield block",
     {1, 1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201 },
     {-1, 6, -1, 4, -1, 5, 4, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_shield_block, SLOT(0), 0, 0,
     "", "!Shield!", ""},

    {
     "spear",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, 4, -1, 3, -1, 3, 2, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_spear, SLOT(0), 0, 0,
     "", "!Spear!", ""},

    {
     "sword",
     {1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {5, 6, 3, 2, 9, 4, 5, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_sword, SLOT(0), 0, 0,
     "", "!sword!", ""},

    {
     "whip",
     {1, 1, 1, 1, 1, 201, 1, 201, 201, 201, 201, 201, 201, 201 },
     {6, 5, 5, 4, 3, 201, 3, 201, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_whip, SLOT(0), 0, 0,
     "", "!Whip!", ""},

    {
     "invade",
     {1, 1, 30, 1, 20, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 9, -1, 7, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_invade, SLOT(0), 0, 0,
     "", "!Invade!", ""},

    {
     "assassinate",
     {1, 1, 1, 1, 25, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, -1, 5, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_assassinate, SLOT(0), 0, 48,
     "", "!Assassinate!", ""},

    {
     "backstab",
     {1, 1, 1, 1, 15, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 5, -1, 7, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_backstab, SLOT(0), 0, 48,
     "backstab", "!Backstab!", ""},

    {
     "bash",
     {1, 9, 1, 1, 1, 3, 5, 1, 15, 201, 201, 201, 201, 201 },
     {-1, 8, -1, 4, -1, 6, 4, 1, 6, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_bash, SLOT(0), 0, 48,
     "bash", "!Bash!", ""},

    {
     "berserk",
     {1, 1, 1, 18, 1, 60, 4, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, 5, -1, 9, 5, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_berserk, SLOT(0), 0, 48,
     "", "You feel your pulse slow down.", ""},

    {
     "circle",
     {1, 1, 30, 1, 30, 1, 1, 50, 50, 201, 201, 201, 201, 201 },
     {-1, -1, 7, -1, 7, -1, -1, 7, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_circle, SLOT(0), 0, 72,
     "circle", "!Circle!", ""},

    {
     "dirt kicking",
     {10, 6, 3, 3, 3, 3, 3, 3, 201, 201, 201, 201, 201, 201 },
     {6, 6, 4, 4, 3, 5, 3, 3, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dirt, SLOT(0), 0, 48,
     "kicked dirt", "You rub the dirt out of your eyes.", ""},

    {
     "disarm",
     {1, 1, 12, 11, 5, 10, 10, 8, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 6, 4, 4, 4, 4, 3, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_disarm, SLOT(0), 0, 48,
     "", "!Disarm!", ""},

    {
     "dodge",
     {20, 1, 1, 13, 1, 13, 11, 10, 201, 201, 201, 201, 201, 201 },
     {8, -1, 4, 6, 5, 6, 6, 6, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dodge, SLOT(0), 0, 0,
     "", "!Dodge!", ""},

    {
     "duck",
     {20, 20, 20, 20, 20, 20, 20, 15, 201, 201, 201, 201, 201, 201 },
     {10, 10, 7, 5, 4, 7, 5, 5, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_duck, SLOT(0), 0, 0,
     "", "!Duck!", ""},

    {
     "enhanced damage",
     {1, 30, 25, 1, 20, 14, 5, 1, 1, 201, 201, 201, 201, 201 },
     {-1, 9, 7, 3, 8, 6, 3, 3, 1, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_enhanced_damage, SLOT(0), 0, 0,
     "", "!Enhanced Damage!", ""},

    {
     "envenom",
     {1, 1, 10, 1, 30, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 4, -1, 6, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_envenom, SLOT(0), 0, 72,
     "", "!Envenom!", ""},

    {
     "hand to hand",
     {25, 10, 5, 1, 1, 1, 5, 1, 1, 201, 201, 201, 201, 201 },
     {8, 5, 6, 4, 3, 4, 4, 2, 1, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_hand_to_hand, SLOT(0), 0, 0,
     "", "!Hand to Hand!", ""},

    {
     "kick",
     {1, 12, 14, 8, 14, 8, 9, 1, 201, 201, 201, 201, 201, 201 },
     {-1, 4, 6, 3, 6, 3, 3, 1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_kick, SLOT(0), 0, 24,
     "kick", "!Kick!", ""},

    {
     "arial",
     {192, 192, 192, 192, 192, 192, 5, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, -1, -1, -1, 3, 4, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_arial, SLOT(0), 0, 24,
     "arial", "!Arial!", ""},

    {
     "parry",
     {26, 20, 13, 1, 10, 1, 7, 1, 201, 201, 201, 201, 201, 201 },
     {9, 8, 6, 4, 6, 5, 6, 3, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_parry, SLOT(0), 0, 0,
     "", "!Parry!", ""},

    {
     "rescue",
     {1, 1, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, 4, -1, 5, 4, 5, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_rescue, SLOT(0), 0, 24,
     "", "!Rescue!", ""},

    {
     "trip",
     {25, 19, 1, 15, 8, 15, 7, 5, 201, 201, 201, 201, 201, 201 },
     {7, 5, 4, 4, 3, 4, 3, 3, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_trip, SLOT(0), 0, 48,
     "trip", "!Trip!", ""},

    {
     "second attack",
     {30, 24, 12, 5, 12, 7, 10, 1, 201, 201, 201, 201, 201, 201 },
     {10, 8, 5, 3, 5, 5, 4, 6, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_second_attack, SLOT(0), 0, 0,
     "", "!Second Attack!", ""},

    {
     "third attack",
     {90, 52, 28, 18, 24, 22, 19, 20, 201, 201, 201, 201, 201, 201 },
     {15, 12, 10, 4, 10, 6, 5, 5, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_third_attack, SLOT(0), 0, 0,
     "", "!Third Attack!", ""},

    {
     "fourth attack",
     {1, 1, 80, 50, 38, 65, 45, 40, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 15, 5, 12, 7, 7, 8, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_fourth_attack, SLOT(0), 0, 0,
     "", "!Fourth Attack!", ""},

    {
     "fifth attack",
     {1, 1, 1, 100, 120, 1, 120, 120, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, 7, 8, -1, 9, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_fifth_attack, SLOT(0), 0, 0,
     "", "!Fifth Attack!", ""},

    {
     "sixth attack",
     {1, 1, 1, 110, 1, 1, 1, 150, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, 9, -1, -1, -1, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_sixth_attack, SLOT(0), 0, 0,
     "", "!Sixth Attack!", ""},

    {
     "ultra damage",
     {1, 1, 1, 125, 1, 1, 115, 150, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, 7, -1, -1, 9, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_ultra_damage, SLOT(0), 0, 0,
     "", "!Ultra Damage!", ""},

    {
     "whirl",
     {1, 1, 1, 30, 1, 38, 25, 25, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, 5, -1, 7, 7, 7, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_whirl, SLOT(0), 0, 70,
     "whirl", "!Whirl!", ""},

    {
     "quick poison",
     {1, 1, 150, 1, 150, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 12, -1, 7, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_quick_poison, SLOT(0), 0, 48,
     "Quick poison", "Quick poison leaves your body",
     "The quick poison evaporates"},

/* non-combat skills */

    {
     "fast healing",
     {23, 9, 9, 10, 6, 12, 9, 25, 201, 201, 201, 201, 201, 201 },
     {9, 5, 6, 4, 5, 4, 6, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_fast_healing, SLOT(0), 0, 0,
     "", "!Fast Healing!", ""},

    {
     "haggle",
     {75, 75, 75, 75, 75, 75, 75, 75, 201, 201, 201, 201, 201, 201 },
     {2, 2, 2, 2, 2, 2, 2, 2, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_haggle, SLOT(0), 0, 0,
     "", "!Haggle!", ""},

    {
     "hide",
     {1, 1, 1, 1, 1, 1, 1, 50, 201, 201, 201, 201, 201, 201 },
     {-1, -1, -1, -1, -1, -1, -1, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_hide, SLOT(0), 0, 24,
     "", "!Hide!", ""},

    {
     "lore",
     {10, 10, 6, 20, 15, 20, 18, 30, 201, 201, 201, 201, 201, 201 },
     {6, 6, 4, 8, 4, 8, 5, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_lore, SLOT(0), 0, 72,
     "", "!Lore!", ""},

    {
     "meditation",
     {6, 6, 1, 1, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201 },
     {5, 5, -1, -1, -1, -1, 1, 4, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_meditation, SLOT(0), 0, 0,
     "", "Meditation", ""},

    {
     "peek",
     {8, 1, 1, 1, 14, 1, 1, 100, 201, 201, 201, 201, 201, 201 },
     {5, -1, 3, -1, 3, -1, 5, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_peek, SLOT(0), 0, 0,
     "", "!Peek!", ""},

    {
     "pick lock",
     {1, 1, 7, 1, 7, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 4, -1, 4, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_pick_lock, SLOT(0), 0, 24,
     "", "!Pick!", ""},

    {
     "sneak",
     {1, 1, 4, 1, 30, 1, 1, 50, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 4, -1, 5, -1, -1, 9, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_sneak, SLOT(0), 0, 24,
     "", "You no longer feel stealthy.", ""},

    {
     "steal",
     {1, 1, 10, 1, 1, 1, 1, 201, 201, 201, 201, 201, 201, 201 },
     {-1, -1, 4, -1, -1, -1, -1, -1, 201, 201, 201, 201, 201, 201 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_steal, SLOT(0), 0, 48,
     "", "!Steal!", ""},

    {
     "scrolls",
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
     {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_scrolls, SLOT(0), 0, 48,
     "", "!Scrolls!", ""},

    {
     "staves",
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
     {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_staves, SLOT(0), 0, 48,
     "", "!Staves!", ""},

    {
     "wands",
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
     {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_wands, SLOT(0), 0, 24,
     "", "!Wands!", ""},

    {
     "recall",
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
     {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_recall, SLOT(0), 0, 24,
     "", "!Recall!", ""},

    {
     "beer",
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_beer, SLOT(0), 0, 24,
     "", "Your drunken fervor subsides.", ""}
};

const struct group_type group_table[MAX_GROUP] =
	/* 
	   * Group Name                         *
	   * Cost Per Class                     *       
	   * Spells in group                    *
	 */
{

    {
     "rom basics",
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  },
     {"scrolls", "staves", "wands", "recall"}
     },

    {
     "mage basics",
     {0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"dagger", "whip"}
     },

    {
     "cleric basics",
     {-1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"mace", "whip"}
     },

    {
     "thief basics",
     {-1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"dagger", "steal", "sneak"}
     },

    {
     "warrior basics",
     {-1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"sword", "second attack"}
     },

    {
     "assassin basics",
     {-1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"dagger", "assassinate", "sneak"}
     },

    {
     "paladin basics",
     {-1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"sword", "mace", "paladin healing"}
     },

    {
     "dragoon basics",
     {-1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1 },
     {"sword", "second attack", "enhanced damage"}
     },

    {
     "monk basics",
     {-1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1 },
     {"polearm", "second attack", "enhanced damage", "hand to hand"}
     },

    {
     "mage default",
     {40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"lore", "beguiling", "combat", "detection", "enhancement", "illusion",
      "maladictions", "protective", "transportation"}
     },

    {
     "cleric default",
     {-1, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"flail", "attack", "creation", "curative", "benedictions",
      "healing", "protective", "shield block", "weather", "body minor",
      "animate dead"}
     },

    {
     "thief default",
     {-1, -1, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"mace", "sword", "backstab", "disarm", "dodge", "second attack",
      "trip", "peek", "pick lock", "sneak"}
     },

    {
     "warrior default",
     {-1, -1, -1, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"weaponsmaster", "shield block", "bash", "disarm", "enhanced damage",
      "parry", "rescue", "third attack"}
     },

    {
     "assassin default",
     {-1, -1, -1, -1, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"mace", "backstab", "disarm", "dodge", "second attack", "trip",
      "peek", "pick lock", "sneak"}
     },

    {
     "paladin default",
     {-1, -1, -1, -1, -1, 40, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"bash", "curative", "shield block", "disarm", "second attack",
      "rescue", "duck"}
     },

    {
     "dragoon default",
     {-1, -1, -1, -1, -1, -1, 40, -1, -1, -1, -1, -1, -1, -1 },
     {"bash", "dodge", "parry", "third attack", "kick", "duck",
      "whirl"}
     },

    {
     "monk default",
     {-1, -1, -1, -1, -1, -1, -1, 40, -1, -1, -1, -1, -1, -1 },
     {"bash", "dodge", "parry", "third attack", "kick", "duck",
      "whirl"}
     },

    {
     "weaponsmaster",
     {40, 40, 40, 20, 40, 40, 20, -1, -1, -1, -1, -1, -1, -1 },
     {"axe", "dagger", "flail", "mace", "polearm", "spear", "sword", "whip"}
     },

		{//Severhand
     "advanced combat",
     {8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"ice shard", "firestream", "boulder", "psychic crush", "death ray",
			"sonic boom", "paralysis"}
     },

		 {//Severhand
			 "advanced defensive",
			 {8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
			 {"displace"}
		 },

    {
     "advanced protective",
     {12, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"force shield", "diamond skin", "heavenly sanctuary",
      "holy armor", "thorn armor"}
     },

    {
     "attack",
     {-1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"demonfire", "dispel evil", "dispel good", "earthquake",
      "flamestrike", "heat metal", "ray of truth"}
     },

    {
     "beguiling",
     {4, -1, 6, -1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"calm", "charm person", "sleep"}
     },

    {
     "benedictions",
     {-1, 4, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"bless", "calm", "frenzy", "holy word", "remove curse"}
     },

    {
     "combat",
     {6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"acid blast", "burning hands", "chain lightning", "chill touch",
      "colour spray", "fireball", "bolt", "magic missile",
      "shocking grasp", "nova", "sunray"}
     },

    {
     "creation",
     {-1, 4, -1, -1, -1, 8, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"continual light", "create food", "create spring", "create water",
      "create rose", "floating disc", "sober"}
     },

    {
     "curative",
     {-1, 4, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"cure blindness", "cure disease", "cure poison"}
     },

    {
     "detection",
     {4, -1, -1, -1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"detect evil", "detect good", "detect hidden", "detect invis",
      "detect magic", "detect poison", "farsight", "identify",
      "know alignment", "locate object", "remove invis"}
     },

    {
     "enchantment",
     {6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"enchant armor", "enchant weapon", "fireproof", "recharge"}
     },

    {
     "enhancement",
     {5, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"giant strength", "haste", "infravision", "refresh", "regeneration"}
     },

    {
     "harmful",
     {-1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"cause critical", "cause light", "cause serious", "harm"}
     },

    {
     "healing",
     {-1, 3, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"cure critical", "cure light", "cure serious", "heal",
      "mass healing", "refresh"}
     },

    {
     "paladin healing",
     {-1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"cure light", "cure serious", "mass healing", "refresh"}
     },

    {
     "illusion",
     {4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"invis", "mass invis", "ventriloquate"}
     },

    {
     "maladictions",
     {5, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"blindness", "change sex", "curse", "energy drain", "lethargy",
      "plague", "poison", "weaken", "animate dead"}
     },

    {
     "protective",
     {4, 6, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"armor", "cancellation", "dispel magic", "fireproof",
      "protection evil", "protection good", "sanctuary", "shield",
      "stone skin", "guardian"}
     },

    {
     "specialties",
     {3, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"talon", "teleport object"}
     },

    {
     "transportation",
     {4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"fly", "gate", "nexus", "pass door", "portal", "summon", "teleport",
      "word of recall"}
     },

    {
     "weather",
     {-1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"call lightning", "control weather", "faerie fire", "faerie fog",
      "bolt"}
     },

    {
     "body minor",
     {-1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
     {"might of heros", "mind of sages", "hands of wind", "sight of ages",
      "heart of the dragon"}
     }
};

