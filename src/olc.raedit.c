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

/* local utils */

/*
 * Race Editor Functions.
 */
RAEDIT(raedit_name)
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0') {
		send_to_char("Syntax:  name [string]\n\r", ch);
		return FALSE;
    }

    free_string(pRace->name);
    pRace->name = str_dup(argument);

    send_to_char("Name set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_wname)
{

    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0') {
		send_to_char("Syntax:  wname [string]\n\r", ch);
		send_to_char("Syntax:  wname [string]\n\r", ch);
		return FALSE;
    }

    free_string(pRace->who_name);
    pRace->who_name = str_dup(argument);

    send_to_char("Who name set.\n\r", ch);
    return TRUE;

}

RAEDIT(raedit_gname)
{
    RACE_DATA *pRace;
    
    EDIT_RACE(ch, pRace);
    
    if (argument[0] == '\0') {
	    send_to_char("Gname is the full name of the race. ie: {GHalf Elf{x\n\r", 		ch);
	    send_to_char("Syntax:  gname [string]\n\r", ch);
  	  return FALSE;
    }

    free_string(pRace->gen_name);
    pRace->gen_name = str_dup(argument);
    
    send_to_char("Chargen name set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_pc)
{
       RACE_DATA *pRace;
       EDIT_RACE(ch, pRace);

        if ( pRace->pc_race )
            pRace->pc_race = FALSE;
        else
            pRace->pc_race = TRUE;

        send_to_char("RAedit: PC attribute toggled.\n\r", ch);
        return TRUE;
}

RAEDIT(raedit_aff)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(affect_flags, argument)) != NO_FLAG) {

        	pRace->aff ^= value;
        	send_to_char("Affect flag toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: affect [flag]\n\r"
         "Type '? affect' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_act)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(act_flags, argument)) != NO_FLAG) {

        	pRace->act ^= value;
        	send_to_char("Act flag toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: act [flag]\n\r"
         "Type '? act' for a list of flags.\n\r", ch);
    return FALSE;
}


RAEDIT(raedit_off)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(off_flags, argument)) != NO_FLAG) {

        	pRace->off ^= value;
        	send_to_char("Off flag toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: off [flag]\n\r"
         "Type '? off' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_imm)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(imm_flags, argument)) != NO_FLAG) {

        	pRace->imm ^= value;
        	send_to_char("Imm flag toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: imm [flag]\n\r"
         "Type '? imm' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_res)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(res_flags, argument)) != NO_FLAG) {

        	pRace->res ^= value;
        	send_to_char("Res flag toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: res [flag]\n\r"
         "Type '? res' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_vuln)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(vuln_flags, argument)) != NO_FLAG) {

        	pRace->vuln ^= value;
        	send_to_char("Vuln flag toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: vuln [flag]\n\r"
         "Type '? vuln' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_form)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(form_flags, argument)) != NO_FLAG) {

        	pRace->form ^= value;
        	send_to_char("Form flags toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: form [flag]\n\r"
         "Type '? form' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_parts)
{       

	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
    	EDIT_RACE(ch, pRace);

    	if ((value = flag_value(part_flags, argument)) != NO_FLAG) {

        	pRace->parts ^= value;
        	send_to_char("Part flags toggled.\n\r", ch);
        	return TRUE;
    	}

    }

    send_to_char("Syntax: part [flag]\n\r"
         "Type '? part' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_sex)
{       
	RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
	EDIT_RACE(ch, pRace);

		if ((value = flag_value(sex_flags, argument)) != NO_FLAG) {

	    	pRace->sex = value;
	    	send_to_char("Sex set.\n\r", ch);
	    	return TRUE;
		}
    }

    send_to_char("Syntax: sex [sex]\n\r"
		 "Type '? sex' for a list of flags.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_size)
{ 
    RACE_DATA *pRace;
    int value;

    if (argument[0] != '\0') {
		EDIT_RACE(ch, pRace);

	if ((value = flag_value(size_flags, argument)) != NO_FLAG) {
	    pRace->size = value;
	    send_to_char("Size set.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char("Syntax: size [size]\n\r"
		 "Type '? size' for a list of sizes.\n\r", ch);
    return FALSE;
}

RAEDIT(raedit_min)
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0' || !is_number(argument)) {
		send_to_char("Min level is the level for a baby of this race.\n\r", ch);
		send_to_char("Syntax:  min [number]\n\r", ch);
		return FALSE;
    }

    pRace->min_level = atoi(argument);
    send_to_char("Min level set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_max)
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0' || !is_number(argument)) {
		send_to_char("Max level is the level for an adult of this race.\n\r", 
			ch);
		send_to_char("Syntax:  max [number]\n\r", ch);
		return FALSE;
    }

    pRace->max_level = atoi(argument);
    send_to_char("Max level set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_points)
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0' || !is_number(argument)) {
		send_to_char("This is the cost of the race in points.\n\r", 
			ch);
		send_to_char("Syntax:  points [number]\n\r", ch);
		return FALSE;
    }

    pRace->points = atoi(argument);
    send_to_char("Points set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_life)
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0' || !is_number(argument)) {
		send_to_char("This is the life span of the race in mud years.\n\r", 
			ch);
		send_to_char("Syntax:  life [number]\n\r", ch);
		return FALSE;
    }

    pRace->lifespan = atoi(argument);
    send_to_char("Lifespan set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_ver)
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if (argument[0] == '\0' || !is_number(argument)) {
		send_to_char("This is the version number of the race. Should be 0\n\r", 
			ch);
		send_to_char("Syntax:  ver [number]\n\r", ch);
		return FALSE;
    }

    pRace->version = atoi(argument);
    send_to_char("Version set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_stats)
{
    RACE_DATA *pRace;
	char arg[MIL];
	int count = 0;


    EDIT_RACE(ch, pRace);

	while ( ( argument = one_argument( argument, arg ) ) )  { 

    if (arg[0] == '\0' || !is_number(arg)) {
		send_to_char("You need to enter five numbers for the race's max stats."
			"\n\r", 
			ch);
		send_to_char("Stats are STR INT WIS DEX CON\n\r", ch);
		send_to_char("Syntax:  stats [num] [num] [num] [num] [num]\n\r", ch);
		return FALSE;
    }

	    pRace->stats[count] = atoi(argument);
		count++;
		if (count >= MAX_STATS)
			break;
	}

    send_to_char("Stats set.\n\r", ch);
    return TRUE;
}

RAEDIT(raedit_senses)
{
    RACE_DATA *pRace;
	char arg[MIL];
	int count = 0;

    EDIT_RACE(ch, pRace);

	while ( ( argument = one_argument( argument, arg )) ) { 

    if (arg[0] == '\0' || !is_number(arg)) {
		send_to_char("You need to enter six numbers for the race's max senses."
			"\n\r", 
			ch);
		send_to_char("Senses are HEAR SMELL SIGHT TASTE TOUCH PERCEPTION\n\r", 
			ch);
		send_to_char("Syntax:  senses [num] [num] [num] [num] [num]\n\r", ch);
		return FALSE;
    }

	    pRace->senses[count] = atoi(argument);
		count++;
		if (count >= MAX_SENSES)
			break;
	}

    send_to_char("Senses set.\n\r", ch);
    return TRUE;
}


/* 
 * This one is a little fucked up.. To keep it compact, I return from within
 * the loop instead of breaking out. This will make processing things after
 * the loop difficult. C'est la.
 */
RAEDIT(raedit_skills)
{  
	RACE_DATA *pRace;
	int i;

	EDIT_RACE( ch, pRace);

	//FIXME: This should be a utility function
	if ( argument[0] == '\0' ) {
		send_to_char( "\r\n", ch );
		send_to_char( pRace->name, ch );
		send_to_char( " racial ", ch );

	    send_to_char( "skills:   ", ch ); 
	    for (i = 0; i < MAX_RACE_SKILLS; i++) {
	        if ( pRace->skills[i] != NULL ) {
	            send_to_char ("[", ch );
	            send_to_char (pRace->skills[i], ch );
	            send_to_char ("] ", ch );
	        }
	        else
	            continue;
	    }
	    send_to_char( "\r\n", ch );

		return FALSE;
	}
		
	if ( !strcmp( argument, "clear" ) ) {

		for (i = 0; i < MAX_RACE_SKILLS; i++)
			pRace->skills[i] = str_dup("");

		send_to_char( "All skill slots cleared.\r\n", ch );
		raedit_skills( ch, "" );
		return TRUE;
	}

	// check to see if all slots are filled.. if so, make em delete one first
	// otherwise, flip the skill in/out
	for ( i = 0; i < MAX_RACE_SKILLS; i++ ) {
		// if we already have the skill, turn it off.. 
		if ( !str_prefix( argument, pRace->skills[i] ) ) {
			pRace->skills[i] = str_dup("");
			send_to_char( "Skill removed.\r\n", ch );
			raedit_skills( ch, "" );
			return TRUE;
		} else {
			// if we find a null spot, and the skill is legal, add it
			if ( !pRace->skills[i] || !strcmp( pRace->skills[i], "" ) ) {

				if ( skill_lookup( argument ) != -1 ) {
					pRace->skills[i] = str_dup(argument);
					send_to_char( "Skill added.\r\n", ch );
					raedit_skills( ch, "" );
					return TRUE;
				} else {
					send_to_char( "That's not a legal skill.\r\n", ch );
					raedit_skills( ch, "" );
					return FALSE;
				}
			} else {
				// we haven't matched a skill and there are no nulls yet
				continue;
			}
		}
	}
	
	// no free slots and no skill matches. fail and bitch
	send_to_char( "No free skill slots. Clear one out.\r\n\r\n", ch );
	raedit_skills( ch, "" );
	return FALSE;
}

RAEDIT(raedit_feeds)
{      
	RACE_DATA *pRace;
	int i;

	EDIT_RACE( ch, pRace);

	//FIXME: This should be a utility function
	if ( argument[0] == '\0' ) {
		send_to_char( "\r\n", ch );
		send_to_char( pRace->name, ch );
		send_to_char( " racial ", ch );

	    send_to_char( "feeds:   ", ch ); 
	    for (i = 0; i < MAX_RACE_FEEDS; i++) {
	        if ( pRace->feeds[i] != NULL ) {
	            send_to_char ("[", ch );
	            send_to_char (pRace->feeds[i], ch );
	            send_to_char ("] ", ch );
	        }
	    }
	    send_to_char( "\r\n", ch );
		return FALSE;
	}
		
	if ( !strcmp( argument, "clear" ) ) {

		for (i = 0; i < MAX_RACE_FEEDS; i++)
			pRace->feeds[i] = str_dup("");

		send_to_char( "All feed slots cleared.\r\n", ch );
		raedit_feeds( ch, "" );
		return TRUE;
	}

	// check to see if all slots are filled.. if so, make em delete one first
	// otherwise, flip the feed in/out
	for ( i = 0; i < MAX_RACE_FEEDS; i++ ) {
		// if we already have the feed, turn it off.. 
		if ( !str_prefix( argument, pRace->feeds[i] ) ) {
			pRace->feeds[i] = str_dup("");
			send_to_char( "Skill removed.\r\n", ch );
			raedit_feeds( ch, "" );
			return TRUE;
		} else {
			// if we find a null spot, and the feed is legal, add it
			if ( !pRace->feeds[i] || !strcmp( pRace->feeds[i], "" ) ) {

				// there's no NULL from race_lookup.. unique == NULL
				if ( race_lookup( argument ) != race_lookup("unique") ) {
					pRace->feeds[i] = str_dup(argument);
					send_to_char( "Food added.\r\n", ch );
					raedit_feeds( ch, "" );
					return TRUE;
				} else {
					send_to_char( "That's not a legal race.\r\n", ch );
					raedit_feeds( ch, "" );
					return FALSE;
				}
			} else {
				// we haven't matched a feed and there are no nulls yet
				continue;
			}
		}
	}
	
	// no free slots and no feed matches. fail and bitch
	send_to_char( "No free feed slots. Clear one out.\r\n\r\n", ch );
	raedit_feeds( ch, "" );
	return FALSE;
}

RAEDIT(raedit_create)
{  
    RACE_DATA *pRace;

	pRace = new_race();

	if (!pRace) {
		send_to_char( "Unable to get a race to edit.\r\n", ch );
		return FALSE;
	}

	ch->desc->pEdit = (void *) pRace;
    ch->desc->editor = ED_RACE;
	
	raedit_show(ch, "");

	return TRUE;
}

RAEDIT(raedit_delete)
{      
    RACE_DATA *pRace;

	EDIT_RACE( ch, pRace );

	if ( !pRace->deleted ) {
		pRace->deleted = TRUE;
		send_to_char( "Race is marked for deletion and will not save.\r\n", 
			ch );
		return TRUE;
	} else {
		pRace->deleted = FALSE;
		send_to_char( "Race is no longer marked for deletion and will save."
			"\r\n", 
			ch );
		return TRUE;
	}

	return FALSE;
}

RAEDIT(raedit_show)
{ 
    RACE_DATA *pRace;
	char buf[MIL];
	int value;

	EDIT_RACE( ch, pRace );

    sprintf( buf, "Name     [%s]	", pRace->name );
	send_to_char( buf, ch );
    sprintf( buf, "Wname    [%s]	", pRace->who_name);
	send_to_char( buf, ch );
    sprintf( buf, "Gname    [%s{x]\r\n", pRace->gen_name );
	send_to_char( buf, ch );
    sprintf( buf, "PC       [%d]		",  pRace->pc_race );
	send_to_char( buf, ch );
    sprintf( buf, "Points   [%d]\r\n\r\n",  pRace->points );
	send_to_char( buf, ch );
    sprintf( buf, "Act      [%s]\r\n", flag_string(act_flags, pRace->act) );
	send_to_char( buf, ch );
    sprintf( buf, "Aff      [%s]\r\n", flag_string(affect_flags, pRace->aff) );
	send_to_char( buf, ch );
    sprintf( buf, "Off      [%s]\r\n", flag_string(off_flags, pRace->off) );
	send_to_char( buf, ch );
    sprintf( buf, "Imm      [%s]\r\n", flag_string(imm_flags, pRace->imm) );
	send_to_char( buf, ch );
    sprintf( buf, "Res      [%s]\r\n", flag_string(res_flags, pRace->res) );
	send_to_char( buf, ch );
    sprintf( buf, "Vuln     [%s]\r\n", flag_string(vuln_flags, pRace->vuln) );
	send_to_char( buf, ch );
    sprintf( buf, "Form     [%s]\r\n", flag_string(form_flags, pRace->form) );
	send_to_char( buf, ch );
    sprintf( buf, "Parts    [%s]\r\n\r\n", flag_string(part_flags, 
		pRace->parts) );
	send_to_char( buf, ch );
    sprintf( buf, "Sex      [%s]	", 
   		pRace->sex == SEX_MALE ? "male" :
        pRace->sex == SEX_FEMALE ? "female" :
        pRace->sex == 3 ? "random" : "neutral");
	send_to_char( buf, ch );
    sprintf( buf, "Size     [%s]\r\n", flag_string(size_flags, pRace->size) );
	send_to_char( buf, ch );
    sprintf( buf, "Min      [%d]		", pRace->min_level );
	send_to_char( buf, ch );
    sprintf( buf, "Max      [%d]	", pRace->max_level );
	send_to_char( buf, ch );
    sprintf( buf, "Version  [%d]	", pRace->version );
	send_to_char( buf, ch );
    sprintf( buf, "Lifespan [%d]\r\n", pRace->lifespan );
	send_to_char( buf, ch );

    send_to_char( "           {gST IN WI DE CO{x\r\nStats    [ ", ch );

    for (value = 0; value < MAX_STATS; value++) {
        sprintf( buf, "%-2d ", pRace->stats[value]);
		send_to_char( buf, ch );
	}

	send_to_char( "]\r\n           {yHE SM SI TA TO PE{x\r\n", ch );
    sprintf( buf, "Sense    [ ");
	send_to_char( buf, ch );

    for (value = 0; value < MAX_SENSES; value++) {
        sprintf( buf, "%-2d ", pRace->senses[value]);
		send_to_char( buf, ch );
	}

    sprintf( buf, "]\r\n\r\n");
	send_to_char( buf, ch );

	send_to_char( "Skills:	 ", ch ); 
    for (value = 0; value < MAX_RACE_SKILLS; value++) {
        if ( pRace->skills[value] != NULL ) {
            send_to_char ("[", ch );
            send_to_char (pRace->skills[value], ch );
            send_to_char ("] ", ch );
		}
        else
            continue;
	}
	send_to_char( "\r\n", ch );

	send_to_char( "Feeds:	 ", ch ); 
    for (value = 0; value < MAX_RACE_FEEDS; value++) {
        if ( pRace->feeds[value] != NULL ) {
            send_to_char ("[", ch );
            send_to_char (pRace->feeds[value], ch );
            send_to_char ("] ", ch );
		}
        else
            continue;
	}
	send_to_char( "\r\n", ch );


	if (pRace->deleted) 
		send_to_char( "\r\n{RThis race is marked for deletion.{x\r\n", ch );
	
	return FALSE;
}
