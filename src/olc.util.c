/***************************************************************************
 *  File: olc.util.c                                                        *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/

/* utility functions created to help OLC*/

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
#include "olc.h"
#include "tables.h"
#include "recycle.h"
#include "lookup.h"


bool show_version(CHAR_DATA * ch, char *argument)
{
    send_to_char(VERSION, ch);
    send_to_char("\n\r", ch);
    send_to_char(AUTHOR, ch);
    send_to_char("\n\r", ch);
    send_to_char(DATE, ch);
    send_to_char("\n\r", ch);
    send_to_char(CREDITS, ch);
    send_to_char("\n\r", ch);

    return FALSE;
}

const struct olc_help_type help_table[] = {
    {"area", area_flags, "Area attributes."},
    {"room", room_flags, "Room attributes."},
    {"sector", sector_flags, "Sector types, terrain."},
    {"exit", exit_flags, "Exit types."},
    {"type", type_flags, "Types of objects."},
    {"extra", extra_flags, "Object attributes."},
    {"wear", wear_flags, "Where to wear object."},
    {"spec", spec_table, "Available special programs."},
    {"sex", sex_flags, "Sexes."},
    {"act", act_flags, "Mobile attributes."},
    {"affect", affect_flags, "Mobile affects."},
    {"wear-loc", wear_loc_flags, "Where mobile wears object."},
    {"spells", skill_table, "Names of current spells."},
    {"container", container_flags, "Container status."},
    {"armor", ac_type, "Ac for different attacks."},
    {"apply", apply_flags, "Apply flags"},
    {"form", form_flags, "Mobile body form."},
    {"part", part_flags, "Mobile body parts."},
    {"imm", imm_flags, "Mobile immunity."},
    {"res", res_flags, "Mobile resistance."},
    {"vuln", vuln_flags, "Mobile vlnerability."},
    {"off", off_flags, "Mobile offensive behaviour."},
    {"size", size_flags, "Mobile size."},
    {"position", position_flags, "Mobile positions."},
    {"wclass", weapon_class, "Weapon class."},
    {"wtype", weapon_type2, "Special weapon type."},
    {"portal", portal_flags, "Portal types."},
    {"furniture", furniture_flags, "Furniture types."},
    {"liquid", liq_table, "Liquid types."},
    {"mpflag", mprog_flags, "Mobprog trigger flags."},
    {NULL, NULL, NULL}
};



/*****************************************************************************
 Name:		show_flag_cmds
 Purpose:	Displays settable flags and stats.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds(CHAR_DATA * ch, const struct flag_type *flag_table)
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int flag;
    int col;

    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++) {
	if (flag_table[flag].settable) {
	    sprintf(buf, "%-19.18s", flag_table[flag].name);
	    strcat(buf1, buf);
	    if (++col % 4 == 0)
		strcat(buf1, "\n\r");
	}
    }

    if (col % 4 != 0)
	strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
}


/*****************************************************************************
 Name:		show_skill_cmds
 Purpose:	Displays all skill functions.
 		Does remove those damn immortal commands from the list.
 		Could be improved by:
 		(1) Adding a check for a particular class.
 		(2) Adding a check for a level range.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds(CHAR_DATA * ch, int tar)
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH * 2];
    int sn;
    int col;

    buf1[0] = '\0';
    col = 0;
    for (sn = 0; sn < MAX_SKILL; sn++) {
	if (!skill_table[sn].name)
	    break;

	if (!str_cmp(skill_table[sn].name, "reserved")
	    || skill_table[sn].spell_fun == spell_null)
	    continue;

	if (tar == -1 || skill_table[sn].target == tar) {
	    sprintf(buf, "%-19.18s", skill_table[sn].name);
	    strcat(buf1, buf);
	    if (++col % 4 == 0)
		strcat(buf1, "\n\r");
	}
    }

    if (col % 4 != 0)
	strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
}



/*****************************************************************************
 Name:		show_spec_cmds
 Purpose:	Displays settable special functions.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int spec;
    int col;

    buf1[0] = '\0';
    col = 0;
    send_to_char("Preceed special functions with 'spec_'\n\r\n\r", ch);
    for (spec = 0; spec_table[spec].function != NULL; spec++) {
	sprintf(buf, "%-19.18s", &spec_table[spec].name[5]);
	strcat(buf1, buf);
	if (++col % 4 == 0)
	    strcat(buf1, "\n\r");
    }

    if (col % 4 != 0)
	strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
}



/*****************************************************************************
 Name:		show_help
 Purpose:	Displays help for many tables used in OLC.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_help(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char spell[MAX_INPUT_LENGTH];
    int cnt;

    argument = one_argument(argument, arg);
    one_argument(argument, spell);

    /*
     * Display syntax.
     */
    if (arg[0] == '\0') {
	send_to_char("Syntax:  ? [command]\n\r\n\r", ch);
	send_to_char("[command]  [description]\n\r", ch);
	for (cnt = 0; help_table[cnt].command != NULL; cnt++) {
	    sprintf(buf, "%-10.10s -%s\n\r",
		    capitalize(help_table[cnt].command),
		    help_table[cnt].desc);
	    send_to_char(buf, ch);
	}
	return FALSE;
    }
    /*
     * Find the command, show changeable data.
     * ---------------------------------------
     */
    for (cnt = 0; help_table[cnt].command != NULL; cnt++) {
	if (arg[0] == help_table[cnt].command[0]
	    && !str_prefix(arg, help_table[cnt].command)) {
	    if (help_table[cnt].structure == spec_table) {
		show_spec_cmds(ch);
		return FALSE;
	    } else if (help_table[cnt].structure == liq_table) {
		show_liqlist(ch);
		return FALSE;
	    } else if (help_table[cnt].structure == attack_table) {
		show_damlist(ch);
		return FALSE;
	    } else if (help_table[cnt].structure == skill_table) {

		if (spell[0] == '\0') {
		    send_to_char("Syntax:  ? spells "
				 "[ignore/attack/defend/self/object/all]\n\r",
				 ch);
		    return FALSE;
		}
		if (!str_prefix(spell, "all"))
		    show_skill_cmds(ch, -1);
		else if (!str_prefix(spell, "ignore"))
		    show_skill_cmds(ch, TAR_IGNORE);
		else if (!str_prefix(spell, "attack"))
		    show_skill_cmds(ch, TAR_CHAR_OFFENSIVE);
		else if (!str_prefix(spell, "defend"))
		    show_skill_cmds(ch, TAR_CHAR_DEFENSIVE);
		else if (!str_prefix(spell, "self"))
		    show_skill_cmds(ch, TAR_CHAR_SELF);
		else if (!str_prefix(spell, "object"))
		    show_skill_cmds(ch, TAR_OBJ_INV);
		else
		    send_to_char("Syntax:  ? spell "
				 "[ignore/attack/defend/self/object/all]\n\r",
				 ch);

		return FALSE;
	    } else {
		show_flag_cmds(ch, help_table[cnt].structure);
		return FALSE;
	    }
	}
    }

    show_help(ch, "");
    return FALSE;
}


/*****************************************************************************
 Name:		check_range( lower vnum, upper vnum )
 Purpose:	Ensures the range spans only one area.
 Called by:	aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range(int lower, int upper)
{
    AREA_DATA *pArea;
    int cnt = 0;

    for (pArea = area_first; pArea; pArea = pArea->next) {
	/*
	 * lower < area < upper
	 */
	if ((lower <= pArea->min_vnum && pArea->min_vnum <= upper)
	    || (lower <= pArea->max_vnum && pArea->max_vnum <= upper))
	    ++cnt;

	if (cnt > 1)
	    return FALSE;
    }
    return TRUE;
}

struct wear_type {
    int wear_loc;
    int wear_bit;
};


const struct wear_type wear_table[] = {
    {WEAR_NONE, ITEM_TAKE},
    {WEAR_LIGHT, ITEM_LIGHT},
    {WEAR_FINGER_L, ITEM_WEAR_FINGER},
    {WEAR_FINGER_R, ITEM_WEAR_FINGER},
    {WEAR_NECK_1, ITEM_WEAR_NECK},
    {WEAR_NECK_2, ITEM_WEAR_NECK},
    {WEAR_BODY, ITEM_WEAR_BODY},
    {WEAR_HEAD, ITEM_WEAR_HEAD},
    {WEAR_LEGS, ITEM_WEAR_LEGS},
    {WEAR_FEET, ITEM_WEAR_FEET},
    {WEAR_HANDS, ITEM_WEAR_HANDS},
    {WEAR_ARMS, ITEM_WEAR_ARMS},
    {WEAR_SHIELD, ITEM_WEAR_SHIELD},
    {WEAR_ABOUT, ITEM_WEAR_ABOUT},
    {WEAR_WAIST, ITEM_WEAR_WAIST},
    {WEAR_WRIST_L, ITEM_WEAR_WRIST},
    {WEAR_WRIST_R, ITEM_WEAR_WRIST},
    {WEAR_WIELD, ITEM_WIELD},
    {WEAR_HOLD, ITEM_HOLD},
    {WEAR_FLOAT, ITEM_WEAR_FLOAT},
    {WEAR_FACE, ITEM_WEAR_FACE},
    {WEAR_EAR_L, ITEM_WEAR_EAR},
    {WEAR_EAR_R, ITEM_WEAR_EAR},
    {WEAR_SHOULDERS, ITEM_WEAR_SHOULDERS},
    {NO_FLAG, NO_FLAG}
};



/*****************************************************************************
 Name:		wear_loc
 Purpose:	Returns the location of the bit that matches the count.
 		1 = first match, 2 = second match etc.
 Called by:	oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc(int bits, int count)
{
    int flag;

    for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++) {
	if (IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
	    return wear_table[flag].wear_loc;
    }

    return NO_FLAG;
}



/*****************************************************************************
 Name:		wear_bit
 Purpose:	Converts a wear_loc into a bit.
 Called by:	redit_oreset(olc_act.c).
 ****************************************************************************/
int wear_bit(int loc)
{
    int flag;

    for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++) {
	if (loc == wear_table[flag].wear_loc)
	    return wear_table[flag].wear_bit;
    }

    return 0;
}





/*
 * Object Editor Functions.
 */
void show_obj_values(CHAR_DATA * ch, OBJ_INDEX_DATA * obj)
{
    char buf[MAX_STRING_LENGTH];

    switch (obj->item_type) {
    default:			/* No values. */
	break;

    case ITEM_LIGHT:
	if (obj->value[2] == -1 || obj->value[2] == 999)	/* ROM OLC */
	    sprintf(buf, "[v2] Light:  Infinite[-1]\n\r");
	else
	    sprintf(buf, "[v2] Light:  [%d]\n\r", obj->value[2]);
	send_to_char(buf, ch);
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	sprintf(buf,
		"[v0] Level:          [%d]\n\r"
		"[v1] Charges Total:  [%d]\n\r"
		"[v2] Charges Left:   [%d]\n\r"
		"[v3] Spell:          %s\n\r",
		obj->value[0],
		obj->value[1],
		obj->value[2],
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		: "none");
	send_to_char(buf, ch);
	break;

    case ITEM_PORTAL:
	sprintf(buf,
		"[v0] Charges:        [%d]\n\r"
		"[v1] Exit Flags:     %s\n\r",
		obj->value[0], flag_string(exit_flags, obj->value[1]));
	send_to_char(buf, ch);
	sprintf(buf,
		"[v2] Portal Flags:   %s\n\r"
		"[v3] Goes to (vnum): [%d]\n\r",
		flag_string(portal_flags, obj->value[2]), obj->value[3]);
	send_to_char(buf, ch);
	break;

    case ITEM_FURNITURE:
	sprintf(buf,
		"[v0] Max people:      [%d]\n\r"
		"[v1] Max weight:      [%d]\n\r"
		"[v2] Furniture Flags: %s\n\r"
		"[v3] Heal bonus:      [%d]\n\r"
		"[v4] Mana bonus:      [%d]\n\r",
		obj->value[0],
		obj->value[1],
		flag_string(furniture_flags, obj->value[2]),
		obj->value[3], obj->value[4]);
	send_to_char(buf, ch);
	break;

    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf(buf,
		"[v0] Level:  [%d]\n\r"
		"[v1] Spell:  %s\n\r"
		"[v2] Spell:  %s\n\r"
		"[v3] Spell:  %s\n\r"
		"[v4] Spell:  %s\n\r",
		obj->value[0],
		obj->value[1] != -1 ? skill_table[obj->value[1]].name
		: "none",
		obj->value[2] != -1 ? skill_table[obj->value[2]].name
		: "none",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		: "none",
		obj->value[4] != -1 ? skill_table[obj->value[4]].name
		: "none");
	send_to_char(buf, ch);
	break;

/* ARMOR for ROM */

    case ITEM_ARMOR:
	sprintf(buf,
		"[v0] Ac pierce       [%d]\n\r"
		"[v1] Ac bash         [%d]\n\r"
		"[v2] Ac slash        [%d]\n\r"
		"[v3] Ac exotic       [%d]\n\r",
		obj->value[0],
		obj->value[1], obj->value[2], obj->value[3]);
	send_to_char(buf, ch);
	break;

/* WEAPON changed in ROM: */
/* I had to split the output here, I have no idea why, but it helped -- Hugin */
/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
    case ITEM_WEAPON:
	sprintf(buf, "[v0] Weapon class:   %s\n\r",
		flag_string(weapon_class, obj->value[0]));
	send_to_char(buf, ch);
	sprintf(buf, "[v1] Number of dice: [%d]\n\r", obj->value[1]);
	send_to_char(buf, ch);
	sprintf(buf, "[v2] Type of dice:   [%d]\n\r", obj->value[2]);
	send_to_char(buf, ch);
    sprintf(buf, "	{cAverage damage of %d.{x\n\r",
   		(1 + obj->value[2]) * obj->value[1] / 2);
    send_to_char(buf, ch);
	sprintf(buf, "[v3] Damage Message:  %s\n\r",
		attack_table[obj->value[3]].name);
	send_to_char(buf, ch);
	sprintf(buf, "[v4] Special type:   %s\n\r",
		flag_string(weapon_type2, obj->value[4]));
	send_to_char(buf, ch);
	break;

    case ITEM_CONTAINER:
	sprintf(buf,
		"[v0] Weight:     [%d kg]\n\r"
		"[v1] Flags:      [%s]\n\r"
		"[v2] Key:     %s [%d]\n\r"
		"[v3] Capacity    [%d]\n\r"
		"[v4] Weight Mult [%d]\n\r",
		obj->value[0],
		flag_string(container_flags, obj->value[1]),
		get_obj_index(obj->value[2])
		? get_obj_index(obj->value[2])->short_descr
		: "none", obj->value[2], obj->value[3], obj->value[4]);
	send_to_char(buf, ch);
	break;

    case ITEM_DRINK_CON:
	sprintf(buf,
		"[v0] Liquid Total: [%d]\n\r"
		"[v1] Liquid Left:  [%d]\n\r"
		"[v2] Liquid:       %s\n\r"
		"[v3] Poisoned:     %s\n\r",
		obj->value[0],
		obj->value[1],
		liq_table[obj->value[2]].liq_name,
		obj->value[3] != 0 ? "Yes" : "No");
	send_to_char(buf, ch);
	break;

    case ITEM_BLOOD_CON:
	sprintf(buf,
		"[v0] Liquid Total: [%d]\n\r"
		"[v1] Liquid Left:  [%d]\n\r"
		"[v2] Liquid:       %s\n\r"
		"[v3] Poisoned:     %s\n\r",
		obj->value[0],
		obj->value[1],
		liq_table[obj->value[2]].liq_name,
		obj->value[3] != 0 ? "Yes" : "No");
	send_to_char(buf, ch);
	break;

    case ITEM_FOUNTAIN:
	sprintf(buf,
		"[v0] Liquid Total: [%d]\n\r"
		"[v1] Liquid Left:  [%d]\n\r"
		"[v2] Liquid:	    %s\n\r",
		obj->value[0],
		obj->value[1], liq_table[obj->value[2]].liq_name);
	send_to_char(buf, ch);
	break;

    case ITEM_FOOD:
	sprintf(buf,
		"[v0] Food hours: [%d]\n\r"
		"[v3] Poisoned:   %s\n\r",
		obj->value[0], obj->value[3] != 0 ? "Yes" : "No");
	send_to_char(buf, ch);
	break;

    case ITEM_MONEY:
	sprintf(buf, "[v1] Gold:   [%d]\n\r", obj->value[1]);
	send_to_char(buf, ch);
	break;
    }

    return;
}



bool set_obj_values(CHAR_DATA * ch, OBJ_INDEX_DATA * pObj, int value_num,
		    char *argument)
{
    switch (pObj->item_type) {
    default:
	break;

    case ITEM_LIGHT:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_LIGHT");
	    return FALSE;
	case 2:
	    send_to_char("HOURS OF LIGHT SET.\n\r\n\r", ch);
	    pObj->value[2] = atoi(argument);
	    break;
	}
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_STAFF_WAND");
	    return FALSE;
	case 0:
	    send_to_char("SPELL LEVEL SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch);
	    pObj->value[2] = atoi(argument);
	    break;
	case 3:
	    send_to_char("SPELL TYPE SET.\n\r", ch);
	    pObj->value[3] = skill_lookup(argument);
	    break;
	}
	break;

    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_SCROLL_POTION_PILL");
	    return FALSE;
	case 0:
	    send_to_char("SPELL LEVEL SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("SPELL TYPE 1 SET.\n\r\n\r", ch);
	    pObj->value[1] = skill_lookup(argument);
	    break;
	case 2:
	    send_to_char("SPELL TYPE 2 SET.\n\r\n\r", ch);
	    pObj->value[2] = skill_lookup(argument);
	    break;
	case 3:
	    send_to_char("SPELL TYPE 3 SET.\n\r\n\r", ch);
	    pObj->value[3] = skill_lookup(argument);
	    break;
	case 4:
	    send_to_char("SPELL TYPE 4 SET.\n\r\n\r", ch);
	    pObj->value[4] = skill_lookup(argument);
	    break;
	}
	break;

/* ARMOR for ROM: */

    case ITEM_ARMOR:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_ARMOR");
	    return FALSE;
	case 0:
	    send_to_char("AC PIERCE SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("AC BASH SET.\n\r\n\r", ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("AC SLASH SET.\n\r\n\r", ch);
	    pObj->value[2] = atoi(argument);
	    break;
	case 3:
	    send_to_char("AC EXOTIC SET.\n\r\n\r", ch);
	    pObj->value[3] = atoi(argument);
	    break;
	}
	break;

/* WEAPONS changed in ROM */

    case ITEM_WEAPON:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_WEAPON");
	    return FALSE;
	case 0:
	    send_to_char("WEAPON CLASS SET.\n\r\n\r", ch);
	    pObj->value[0] = flag_value(weapon_class, argument);
	    break;
	case 1:
	    send_to_char("NUMBER OF DICE SET.\n\r\n\r", ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("TYPE OF DICE SET.\n\r\n\r", ch);
	    pObj->value[2] = atoi(argument);
	    break;
	case 3:
	    send_to_char("WEAPON TYPE SET.\n\r\n\r", ch);
	    pObj->value[3] = attack_lookup(argument);
	    break;
	case 4:
	    send_to_char("SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch);
	    pObj->value[4] ^=
		(flag_value(weapon_type2, argument) !=
		 NO_FLAG ? flag_value(weapon_type2, argument) : 0);
	    break;
	}
	break;

    case ITEM_PORTAL:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_PORTAL");
	    return FALSE;

	case 0:
	    send_to_char("CHARGES SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("EXIT FLAGS SET.\n\r\n\r", ch);
	    pObj->value[1] = flag_value(exit_flags, argument);
	    break;
	case 2:
	    send_to_char("PORTAL FLAGS SET.\n\r\n\r", ch);
	    pObj->value[2] = flag_value(portal_flags, argument);
	    break;
	case 3:
	    send_to_char("EXIT VNUM SET.\n\r\n\r", ch);
	    pObj->value[3] = atoi(argument);
	    break;
	}
	break;

    case ITEM_FURNITURE:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_FURNITURE");
	    return FALSE;

	case 0:
	    send_to_char("NUMBER OF PEOPLE SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("MAX WEIGHT SET.\n\r\n\r", ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("FURNITURE FLAGS TOGGLED.\n\r\n\r", ch);
	    pObj->value[2] ^=
		(flag_value(furniture_flags, argument) !=
		 NO_FLAG ? flag_value(furniture_flags, argument) : 0);
	    break;
	case 3:
	    send_to_char("HEAL BONUS SET.\n\r\n\r", ch);
	    pObj->value[3] = atoi(argument);
	    break;
	case 4:
	    send_to_char("MANA BONUS SET.\n\r\n\r", ch);
	    pObj->value[4] = atoi(argument);
	    break;
	}
	break;

    case ITEM_CONTAINER:
	switch (value_num) {
	    int value;

	default:
	    do_help(ch, "ITEM_CONTAINER");
	    return FALSE;
	case 0:
	    send_to_char("WEIGHT CAPACITY SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    if ((value = flag_value(container_flags, argument))
		!= NO_FLAG)
		TOGGLE_BIT(pObj->value[1], value);
	    else {
		do_help(ch, "ITEM_CONTAINER");
		return FALSE;
	    }
	    send_to_char("CONTAINER TYPE SET.\n\r\n\r", ch);
	    break;
	case 2:
	    if (atoi(argument) != 0) {
		if (!get_obj_index(atoi(argument))) {
		    send_to_char("THERE IS NO SUCH ITEM.\n\r\n\r", ch);
		    return FALSE;
		}
		if (get_obj_index(atoi(argument))->item_type != ITEM_KEY) {
		    send_to_char("THAT ITEM IS NOT A KEY.\n\r\n\r", ch);
		    return FALSE;
		}
	    }
	    send_to_char("CONTAINER KEY SET.\n\r\n\r", ch);
	    pObj->value[2] = atoi(argument);
	    break;
	case 3:
	    send_to_char("CONTAINER MAX WEIGHT SET.\n\r", ch);
	    pObj->value[3] = atoi(argument);
	    break;
	case 4:
	    send_to_char("WEIGHT MULTIPLIER SET.\n\r\n\r", ch);
	    pObj->value[4] = atoi(argument);
	    break;
	}
	break;

    case ITEM_DRINK_CON:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_DRINK");
/* OLC              do_help( ch, "liquids" );    */
	    return FALSE;
	case 0:
	    send_to_char("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r",
			 ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("LIQUID TYPE SET.\n\r\n\r", ch);
	    pObj->value[2] = (liq_lookup(argument) != -1 ?
			      liq_lookup(argument) : 0);
	    break;
	case 3:
	    send_to_char("POISON VALUE TOGGLED.\n\r\n\r", ch);
	    pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
	    break;
	}
	break;

    case ITEM_BLOOD_CON:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_DRINK");
/* OLC              do_help( ch, "liquids" );    */
	    return FALSE;
	case 0:
	    send_to_char("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r",
			 ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("LIQUID TYPE SET.\n\r\n\r", ch);
	    pObj->value[2] = (liq_lookup(argument) != -1 ?
			      liq_lookup(argument) : 0);
	    break;
	case 3:
	    send_to_char("POISON VALUE TOGGLED.\n\r\n\r", ch);
	    pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
	    break;
	}
	break;

    case ITEM_FOUNTAIN:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_FOUNTAIN");
/* OLC              do_help( ch, "liquids" );    */
	    return FALSE;
	case 0:
	    send_to_char("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r",
			 ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 2:
	    send_to_char("LIQUID TYPE SET.\n\r\n\r", ch);
	    pObj->value[2] = (liq_lookup(argument) != -1 ?
			      liq_lookup(argument) : 0);
	    break;
	}
	break;

    case ITEM_FOOD:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_FOOD");
	    return FALSE;
	case 0:
	    send_to_char("HOURS OF FOOD SET.\n\r\n\r", ch);
	    pObj->value[0] = atoi(argument);
	    break;
	case 1:
	    send_to_char("HOURS OF FULL SET.\n\r\n\r", ch);
	    pObj->value[1] = atoi(argument);
	    break;
	case 3:
	    send_to_char("POISON VALUE TOGGLED.\n\r\n\r", ch);
	    pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
	    break;
	}
	break;

    case ITEM_MONEY:
	switch (value_num) {
	default:
	    do_help(ch, "ITEM_MONEY");
	    return FALSE;
	case 1:
	    send_to_char("GOLD AMOUNT SET.\n\r\n\r", ch);
	    pObj->value[1] = atoi(argument);
	    break;
	}
	break;
    }

    show_obj_values(ch, pObj);

    return TRUE;
}

void show_damlist(CHAR_DATA * ch)
{
    int att;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];

    buffer = new_buf();

    for (att = 0; attack_table[att].name != NULL; att++) {
	if ((att % 21) == 0)
	    add_buf(buffer, "Name                 Noun\n\r");

	sprintf(buf, "%-20s %-20s\n\r",
		attack_table[att].name, attack_table[att].noun);
	add_buf(buffer, buf);
    }

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return;
}


void do_damtype(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH - 1];
    int i;
    int count = 0;

    for (i = 0; attack_table[i].name != NULL; i++) {
		sprintf(buf, "%-20s", attack_table[i].noun);
		send_to_char(buf, ch);

		if (count % 4 == 3)
			send_to_char("\r\n", ch);

		count++;
   	}
	send_to_char("\r\n", ch);
	return;
}
// returns next unused mob vnum in an area.
// returns 0 if no vnums left. don't forget to catch it
long get_next_mob_vnum_area( AREA_DATA *pArea ) {

    long i;
    MOB_INDEX_DATA *pMob;

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
        if ((pMob = get_mob_index(i)) == NULL )
            return i;
    }

    return 0;
}

// returns next unused obj vnum in an area.
// returns 0 if no vnums left. don't forget to catch it
long get_next_obj_vnum_area( AREA_DATA *pArea ) {

    long i;
    OBJ_INDEX_DATA *pObj;

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
        if ((pObj = get_obj_index(i)) == NULL )
            return i;
    }

    return 0;
}

// returns next unused room vnum in an area.
// returns 0 if no vnums left. don't forget to catch it
long get_next_room_vnum_area( AREA_DATA *pArea ) {

    long i;
    ROOM_INDEX_DATA *pRoom;

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
        if ((pRoom = get_room_index(i)) == NULL )
            return i;
    }

    return 0;
}

AREA_DATA *get_vnum_area(long vnum)
{
    AREA_DATA *pArea;

    for (pArea = area_first; pArea; pArea = pArea->next) {
    if (vnum >= pArea->min_vnum && vnum <= pArea->max_vnum)
        return pArea;
    }

    return 0;
}

void show_liqlist(CHAR_DATA * ch)
{
    int liq;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];

    buffer = new_buf();

    for (liq = 0; liq_table[liq].liq_name != NULL; liq++) {
    if ((liq % 21) == 0)
        add_buf(buffer,
            "Name                 Color          Proof Full Thirst Food Ssize\n\r");

    sprintf(buf, "%-20s %-14s %5d %4d %6d %4d %5d\n\r",
        liq_table[liq].liq_name, liq_table[liq].liq_color,
        liq_table[liq].liq_affect[0], liq_table[liq].liq_affect[1],
        liq_table[liq].liq_affect[2], liq_table[liq].liq_affect[3],
        liq_table[liq].liq_affect[4]);
    add_buf(buffer, buf);
    }

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return;
}

