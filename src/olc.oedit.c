/***************************************************************************
 *  File: olc_act.c                                                        *
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

OEDIT(oedit_show)
{
    OBJ_INDEX_DATA *pObj;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int cnt;

    EDIT_OBJ(ch, pObj);

    sprintf(buf, "Name:        [%s]\n\rArea:        [%5ld] %s\n\r",
	    pObj->name,
	    !pObj->area ? -1 : pObj->area->vnum,
	    !pObj->area ? "No Area" : pObj->area->name);
    send_to_char(buf, ch);


    sprintf(buf, "Vnum:        [%5ld]\n\rType:        [%s]\n\r",
	    pObj->vnum, flag_string(type_flags, pObj->item_type));
    send_to_char(buf, ch);

    sprintf(buf, "Level:       [%5d]\n\r", pObj->level);
    send_to_char(buf, ch);

    sprintf(buf, "Wear flags:  [%s]\n\r",
	    flag_string(wear_flags, pObj->wear_flags));
    send_to_char(buf, ch);

    sprintf(buf, "Extra flags: [%s]\n\r",
	    flag_string(extra_flags, pObj->extra_flags));
    send_to_char(buf, ch);

    sprintf(buf, "Material:    [%s]\n\r",	/* ROM */
	    pObj->material);
    send_to_char(buf, ch);

    sprintf(buf, "Condition:   [%5d]\n\r",	/* ROM */
	    pObj->condition);
    send_to_char(buf, ch);

    sprintf(buf, "Timer:       [%5d]\n\r",	/* ROM */
	    pObj->timer);
    send_to_char(buf, ch);

    sprintf(buf, "Weight:      [%5d]\n\rCost:        [%5d]\n\r",
	    pObj->weight, pObj->cost);
    send_to_char(buf, ch);

    if (pObj->extra_descr) {
	EXTRA_DESCR_DATA *ed;

	send_to_char("Ex desc kwd: ", ch);

	for (ed = pObj->extra_descr; ed; ed = ed->next) {
	    send_to_char("[", ch);
	    send_to_char(ed->keyword, ch);
	    send_to_char("]", ch);
	}

	send_to_char("\n\r", ch);
    }
    sprintf(buf, "Short desc:  %s\n\rLong desc:\n\r     %s\n\r",
	    pObj->short_descr, pObj->description);
    send_to_char(buf, ch);

    for (cnt = 0, paf = pObj->affected; paf; paf = paf->next) {
	if (cnt == 0) {
	    send_to_char("Number Modifier Affects\n\r", ch);
	    send_to_char("------ -------- -------\n\r", ch);
	}
	sprintf(buf, "[%4d] %-8d %s\n\r", cnt,
		paf->modifier, flag_string(apply_flags, paf->location));
	send_to_char(buf, ch);
	cnt++;
    }

    show_obj_values(ch, pObj);

    if (pObj->deleted) {
	send_to_char
	    ("{R      THIS OBJECT IS MARKED FOR DELETION AND WILL NOT BE SAVED{X\r\n",
	     ch);
    }
    return FALSE;
}


/*
 * Need to issue warning if flag isn't valid. -- does so now -- Hugin.
 */
OEDIT(oedit_addaffect)
{
    int value;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument(argument, loc);
    one_argument(argument, mod);

    if (loc[0] == '\0' || mod[0] == '\0' || !is_number(mod)) {
	send_to_char("Syntax:  addaffect [location] [#mod]\n\r", ch);
	return FALSE;
    }
    if ((value = flag_value(apply_flags, loc)) == NO_FLAG) {	/* Hugin */
	send_to_char("Valid affects are:\n\r", ch);
	show_help(ch, "apply");
	return FALSE;
    }
    pAf = new_affect();
    pAf->location = value;
    pAf->modifier = atoi(mod);
    pAf->type = -1;
    pAf->duration = -1;
    pAf->bitvector = 0;
    pAf->next = pObj->affected;
    pObj->affected = pAf;

    send_to_char("Affect added.\n\r", ch);
    return TRUE;
}

/*
 * My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
 * for really teaching me how to manipulate pointers.
 */
OEDIT(oedit_delaffect)
{
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument(argument, affect);

    if (!is_number(affect) || affect[0] == '\0') {
	send_to_char("Syntax:  delaffect [#xaffect]\n\r", ch);
	return FALSE;
    }
    value = atoi(affect);

    if (value < 0) {
	send_to_char("Only non-negative affect-numbers allowed.\n\r", ch);
	return FALSE;
    }
    if (!(pAf = pObj->affected)) {
	send_to_char("OEdit:  Non-existant affect.\n\r", ch);
	return FALSE;
    }
    if (value == 0) {		/* First case: Remove first affect */
	pAf = pObj->affected;
	pObj->affected = pAf->next;
	free_affect(pAf);
    } else {			/* Affect to remove is not the first */
	while ((pAf_next = pAf->next) && (++cnt < value))
	    pAf = pAf_next;

	if (pAf_next) {		/* See if it's the next affect */
	    pAf->next = pAf_next->next;
	    free_affect(pAf_next);
	} else {		/* Doesn't exist */
	    send_to_char("No such affect.\n\r", ch);
	    return FALSE;
	}
    }

    send_to_char("Affect removed.\n\r", ch);
    return TRUE;
}



OEDIT(oedit_name)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
	send_to_char("Syntax:  name [string]\n\r", ch);
	return FALSE;
    }
    free_string(pObj->name);
    pObj->name = str_dup(argument);

    send_to_char("Name set.\n\r", ch);
    return TRUE;
}



OEDIT(oedit_short)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
	send_to_char("Syntax:  short [string]\n\r", ch);
	return FALSE;
    }
    free_string(pObj->short_descr);
    pObj->short_descr = str_dup(argument);
    pObj->short_descr[0] = LOWER(pObj->short_descr[0]);

    send_to_char("Short description set.\n\r", ch);
    return TRUE;
}



OEDIT(oedit_long)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
	send_to_char("Syntax:  long [string]\n\r", ch);
	return FALSE;
    }
    free_string(pObj->description);
    pObj->description = str_dup(argument);
    pObj->description[0] = UPPER(pObj->description[0]);

    send_to_char("Long description set.\n\r", ch);
    return TRUE;
}



bool set_value(CHAR_DATA * ch, OBJ_INDEX_DATA * pObj, char *argument,
	       int value)
{
    if (argument[0] == '\0') {
	set_obj_values(ch, pObj, -1, "");	/* '\0' changed to "" -- Hugin */
	return FALSE;
    }
    if (set_obj_values(ch, pObj, value, argument))
	return TRUE;

    return FALSE;
}



/*****************************************************************************
 Name:		oedit_values
 Purpose:	Finds the object and sets its value.
 Called by:	The four valueX functions below. (now five -- Hugin )
 ****************************************************************************/
bool oedit_values(CHAR_DATA * ch, char *argument, int value)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (set_value(ch, pObj, argument, value))
	return TRUE;

    return FALSE;
}


OEDIT(oedit_value0)
{
    if (oedit_values(ch, argument, 0))
	return TRUE;

    return FALSE;
}



OEDIT(oedit_value1)
{
    if (oedit_values(ch, argument, 1))
	return TRUE;

    return FALSE;
}



OEDIT(oedit_value2)
{
    if (oedit_values(ch, argument, 2))
	return TRUE;

    return FALSE;
}



OEDIT(oedit_value3)
{
    if (oedit_values(ch, argument, 3))
	return TRUE;

    return FALSE;
}



OEDIT(oedit_value4)
{
    if (oedit_values(ch, argument, 4))
	return TRUE;

    return FALSE;
}



OEDIT(oedit_weight)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
	send_to_char("Syntax:  weight [number]\n\r", ch);
	return FALSE;
    }
    pObj->weight = atoi(argument);

    send_to_char("Weight set.\n\r", ch);
    return TRUE;
}

OEDIT(oedit_cost)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
	send_to_char("Syntax:  cost [number]\n\r", ch);
	return FALSE;
    }
    pObj->cost = atoi(argument);

    send_to_char("Cost set.\n\r", ch);
    return TRUE;
}



OEDIT(oedit_create)
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int value;
    int iHash;

    value = atoi(argument);

    if (argument[0] == '\0' || value == 0) {
        if ((value = get_next_obj_vnum_area( ch->in_room->area )) == 0) {
            send_to_char("This area is out of vnums for objs.\r\n", ch);
            return FALSE;
        }
    }

    pArea = get_vnum_area(value);
    if (!pArea) {
	send_to_char("OEdit:  That vnum is not assigned an area.\n\r", ch);
	return FALSE;
    }
    if (!IS_BUILDER(ch, pArea)) {
	send_to_char("OEdit:  Vnum in an area you cannot build in.\n\r",
		     ch);
	return FALSE;
    }
    if (get_obj_index(value)) {
	send_to_char("OEdit:  Object vnum already exists.\n\r", ch);
	return FALSE;
    }
    pObj = new_obj_index();
    pObj->vnum = value;
    pObj->area = pArea;

    if (value > top_vnum_obj)
	top_vnum_obj = value;

    iHash = value % MAX_KEY_HASH;
    pObj->next = obj_index_hash[iHash];
    obj_index_hash[iHash] = pObj;
    ch->desc->pEdit = (void *) pObj;

    send_to_char("Object Created.\n\r", ch);
    return TRUE;
}



OEDIT(oedit_ed)
{
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument(argument, command);
    one_argument(argument, keyword);

    if (command[0] == '\0') {
	send_to_char("Syntax:  ed add [keyword]\n\r", ch);
	send_to_char("         ed delete [keyword]\n\r", ch);
	send_to_char("         ed edit [keyword]\n\r", ch);
	send_to_char("         ed format [keyword]\n\r", ch);
	return FALSE;
    }
    if (!str_cmp(command, "add")) {
	if (keyword[0] == '\0') {
	    send_to_char("Syntax:  ed add [keyword]\n\r", ch);
	    return FALSE;
	}
	ed = new_extra_descr();
	ed->keyword = str_dup(keyword);
	ed->next = pObj->extra_descr;
	pObj->extra_descr = ed;

	string_append(ch, &ed->description);

	return TRUE;
    }
    if (!str_cmp(command, "edit")) {
	if (keyword[0] == '\0') {
	    send_to_char("Syntax:  ed edit [keyword]\n\r", ch);
	    return FALSE;
	}
	for (ed = pObj->extra_descr; ed; ed = ed->next) {
	    if (is_name(keyword, ed->keyword))
		break;
	}

	if (!ed) {
	    send_to_char
		("OEdit:  Extra description keyword not found.\n\r", ch);
	    return FALSE;
	}
	string_append(ch, &ed->description);

	return TRUE;
    }
    if (!str_cmp(command, "delete")) {
	EXTRA_DESCR_DATA *ped = NULL;

	if (keyword[0] == '\0') {
	    send_to_char("Syntax:  ed delete [keyword]\n\r", ch);
	    return FALSE;
	}
	for (ed = pObj->extra_descr; ed; ed = ed->next) {
	    if (is_name(keyword, ed->keyword))
		break;
	    ped = ed;
	}

	if (!ed) {
	    send_to_char
		("OEdit:  Extra description keyword not found.\n\r", ch);
	    return FALSE;
	}
	if (!ped)
	    pObj->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr(ed);

	send_to_char("Extra description deleted.\n\r", ch);
	return TRUE;
    }
    if (!str_cmp(command, "format")) {
	EXTRA_DESCR_DATA *ped = NULL;

	if (keyword[0] == '\0') {
	    send_to_char("Syntax:  ed format [keyword]\n\r", ch);
	    return FALSE;
	}
	for (ed = pObj->extra_descr; ed; ed = ed->next) {
	    if (is_name(keyword, ed->keyword))
		break;
	    ped = ed;
	}

	if (!ed) {
	    send_to_char
		("OEdit:  Extra description keyword not found.\n\r", ch);
	    return FALSE;
	}
	ed->description = format_string(ed->description);

	send_to_char("Extra description formatted.\n\r", ch);
	return TRUE;
    }
    oedit_ed(ch, "");
    return FALSE;
}





/* ROM object functions : */

OEDIT(oedit_extra)
{				/* Moved out of oedit() due to naming conflicts -- Hugin */
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0') {
	EDIT_OBJ(ch, pObj);

	if ((value = flag_value(extra_flags, argument)) != NO_FLAG) {
	    TOGGLE_BIT(pObj->extra_flags, value);

	    send_to_char("Extra flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char("Syntax:  extra [flag]\n\r"
		 "Type '? extra' for a list of flags.\n\r", ch);
    return FALSE;
}


OEDIT(oedit_wear)
{				/* Moved out of oedit() due to naming conflicts -- Hugin */
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0') {
	EDIT_OBJ(ch, pObj);

	if ((value = flag_value(wear_flags, argument)) != NO_FLAG) {
	    TOGGLE_BIT(pObj->wear_flags, value);

	    send_to_char("Wear flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char("Syntax:  wear [flag]\n\r"
		 "Type '? wear' for a list of flags.\n\r", ch);
    return FALSE;
}


OEDIT(oedit_type)
{				/* Moved out of oedit() due to naming conflicts -- Hugin */
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0') {
	EDIT_OBJ(ch, pObj);

	if ((value = flag_value(type_flags, argument)) != NO_FLAG) {
	    pObj->item_type = value;

	    send_to_char("Type set.\n\r", ch);

	    /*
	     * Clear the values.
	     */
	    pObj->value[0] = 0;
	    pObj->value[1] = 0;
	    pObj->value[2] = 0;
	    pObj->value[3] = 0;
	    pObj->value[4] = 0;	/* ROM */

	    return TRUE;
	}
    }
    send_to_char("Syntax:  type [flag]\n\r"
		 "Type '? type' for a list of flags.\n\r", ch);
    return FALSE;
}

OEDIT(oedit_material)
{
    OBJ_INDEX_DATA *pObj;
    char arg[MAX_INPUT_LENGTH];

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0') {
	send_to_char("Syntax:  material [string]\n\r", ch);
	return FALSE;
    }
    argument = one_argument(argument, arg);

    free_string(pObj->material);
    pObj->material = str_dup(arg);

    send_to_char("Material set.\n\r", ch);
    return TRUE;
}

OEDIT(oedit_level)
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if (argument[0] == '\0' || !is_number(argument)) {
	send_to_char("Syntax:  level [number]\n\r", ch);
	return FALSE;
    }
    pObj->level = atoi(argument);

    send_to_char("Level set.\n\r", ch);
    return TRUE;
}


OEDIT(oedit_condition)
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0'
	&& (value = atoi(argument)) >= 0 && (value <= 100)) {
	EDIT_OBJ(ch, pObj);

	pObj->condition = value;
	send_to_char("Condition set.\n\r", ch);

	return TRUE;
    }
    send_to_char("Syntax:  condition [number]\n\r"
		 "Where number can range from 0 (ruined) to 100 (perfect).\n\r",
		 ch);
    return FALSE;
}

OEDIT(oedit_timer)
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if (argument[0] != '\0' && (value = atoi(argument)) >= 0) {
	EDIT_OBJ(ch, pObj);

	pObj->timer = value;
	send_to_char("Timer set.\n\r", ch);

	return TRUE;
    }
    send_to_char("Syntax:  timer [number]\n\r"
		 "Where timer is the number of ticks an object will last once a player\n\r"
		 "gets it..\n\r", ch);
    return FALSE;
}
