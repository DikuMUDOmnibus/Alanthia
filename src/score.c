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
*   ROM 2.4 is copyright 1993-1996 Russ Taylor             *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@efn.org)                  *
*       Gabrielle Taylor                           *
*       Brian Moore (zump@rom.org)                     *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/

/*
 * The abominable Alanthia score screen, and assorted adjudicantum.
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

DECLARE_DO_FUN(do_affects);

void do_rp_score( CHAR_DATA *ch, char *argument )
{

    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;

    buffer = new_buf();

    sprintf(buf, "{W\r\n  You are {G%s{W, a %s %s.\r\n",
	    NAME(ch),
			SEX_NAME(ch),
	    RACE_NAME(ch));
    add_buf(buffer, buf);

    sprintf(buf, "  You are a %s follower of %s.\r\n",
	    DEVOTION_NAME(ch), 
		  GOD_NAME(ch));
    add_buf(buffer, buf);

    sprintf(buf, "  You are %s as a follower of %s.\r\n\r\n",
			ALIGN_FAME(ch),
			ALIGN_NAME(ch));
    add_buf(buffer, buf);

    sprintf(buf, "  {gAge:    {W%6d      {gHours:   {W%5d       "
	    "{gMkills: {W%6ld{g\r\n", get_age(ch),
	    (ch->played + (int) (current_time - ch->logon)) / 3600,
	    (IS_NPC(ch) ? 0 : ch->pcdata->mkills));
    add_buf(buffer, buf);

    sprintf(buf, "  Wimpy:  {W%6d      {gDeaths: {W%6d       "
	    "{gPkills:    {W%3ld{g\r\n\r\n",
	    ch->wimpy, 
			(IS_NPC(ch) ? 0 : ch->pcdata->deaths), 
			(IS_NPC(ch) ? 0 : ch->pcdata->pkills));
    add_buf(buffer, buf);

    sprintf(buf, "  {cHp:{W %5ld{C/{W%5ld     {cStr:%s %-14s  "
	    "{cHearing    : %s\r\n",
	    ch->hit,
	    ch->max_hit,
	    fill_color[get_curr_stat(ch, STAT_STR)],
	    str_name[get_curr_stat(ch, STAT_STR)],
	    get_sense_name(ch, SENSE_HEA));
    add_buf(buffer, buf);

    sprintf(buf, "  {cMn:{W %5ld{C/{W%5ld     {cInt:%s %-14s  "
	    "{cSmell      : %s\r\n",
	    ch->mana,
	    ch->max_mana,
	    fill_color[get_curr_stat(ch, STAT_INT)],
	    int_name[get_curr_stat(ch, STAT_INT)],
	    get_sense_name(ch, SENSE_SME));
    add_buf(buffer, buf);

    sprintf(buf, "  {cMv:{W %5ld{C/{W%5ld     {cWil:%s %-14s  "
	    "{cEyesight   : %s\r\n",
	    ch->move,
	    ch->max_move,
	    fill_color[get_curr_stat(ch, STAT_WIS)],
	    wis_name[get_curr_stat(ch, STAT_WIS)],
	    get_sense_name(ch, SENSE_SIG));
    add_buf(buffer, buf);

    sprintf(buf, "  {cBp:{W %5d{C/{W%5d     {cDex:%s %-14s  "
	    "{cTaste      : %s\r\n",
	    ch->blood,
	    ch->max_blood,
	    fill_color[get_curr_stat(ch, STAT_DEX)],
	    dex_name[get_curr_stat(ch, STAT_DEX)],
	    get_sense_name(ch, SENSE_TAS));
    add_buf(buffer, buf);

    sprintf(buf, "  {cSz: {W%-3d fingers     {cCon:%s %-14s  "
	    "{cTouch      : %s\r\n",
	    ch->size,
	    fill_color[get_curr_stat(ch, STAT_CON)],
	    con_name[get_curr_stat(ch, STAT_CON)],
	    get_sense_name(ch, SENSE_TOU));
    add_buf(buffer, buf);

    sprintf(buf, "                                           "
	    "{cPerception : %s\r\n\r\n", get_sense_name(ch, SENSE_PER));
    add_buf(buffer, buf);

    sprintf(buf, "{W  You are carrying {w10{W stone and {w4{W pebble out "
	    "of a total {w100{W stoneweight.\r\n");
    add_buf(buffer, buf);


    sprintf(buf, "  You have %s on hand.\r\n", expand_cost(ch->gold));
    add_buf(buffer, buf);

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return;
}

void do_score(CHAR_DATA * ch, char *argument)
{
	// a bunch of buffer buckets to make counting spaces
	// for formatting easier
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char buf4[MAX_STRING_LENGTH];
    char buf5[MAX_STRING_LENGTH];
    
	#define HOR_BAR " {g+----------------------------------------------------------------------------+\n\r"

    send_to_char("\n\r", ch);

	send_to_char(HOR_BAR, ch);
    
	// Level, Sex, PKILL Status
    sprintf(buf2, "%d", ch->level);
    sprintf(buf3, "%d (%d)", ch->level, get_trust(ch));
	sprintf(buf4, "%s", ch->sex == 0 ? "Sexless" : ch->sex == 1 ? "Male" : "Female");
	sprintf(buf5, "%s", IS_SET(ch->act, PLR_PKILL) ? "Yes" : "No");
    
    sprintf(buf,
        " |  {wLevel:{c %-15s {wSex  :{m %-15s {wPkill   :{y %-15s   {g|\n\r",
        ch->level == get_trust(ch) ? buf2 : buf3, buf4, buf5 );
    send_to_char(buf, ch);

	
    // Age, Race, Clan
	sprintf(buf2, "%d", get_age(ch));
	sprintf(buf3, "%s", capitalize(ch->race->name));
	sprintf(buf4, "%s", (!is_clan(ch) || clan_table[ch->clan].independent) 
						? "None" : clan_table[ch->clan].name );

    sprintf(buf, " |  {wAge  :{c %-15s {wRace :{m %-15s {wClan    :{y %-15s   {g|\n\r", 
		buf2, buf3, buf4); 
    send_to_char(buf, ch);
    
	// Hours, Class, EXP
	sprintf(buf2, "%d", (ch->played + (int) (current_time - ch->logon)) / 3600);
	sprintf(buf3, "%s", IS_NPC(ch) ? "Mobile" : capitalize(class_table[ch->class].name));
	sprintf(buf4, "%ld", ch->exp);

    sprintf(buf,
        " |  {wHours:{c %-15s {wClass:{m %-15s {wTotal XP:{y %-15s   {g|\n\r",
		buf2, buf3, buf4);
    send_to_char(buf, ch); 

	send_to_char(HOR_BAR, ch);
    
	// HMVB
    sprintf(buf2, "{g%ld{w/{G%ld", ch->hit, ch->max_hit);
    sprintf(buf3, "{g%ld{w/{G%ld", ch->mana, ch->max_mana);
    sprintf(buf4, "{g%ld{w/{G%ld", ch->move, ch->max_move);                         
    sprintf(buf5, "{g%d{w/{G%d", ch->blood, ch->max_blood);                         

    if (DOESNT_USE_BLOOD(ch)) {                     
		sprintf(buf,
        " |  {wHit  : %-21s {wMana : %-21s {wMove    : %-21s   {g|\n\r",                      
		buf2, buf3, buf4);                                                          
		send_to_char(buf, ch);
    } else {
    	sprintf(buf,
        " |  {wHit: %-19s{w Mana: %-19s{w  Mv: %-19s{w Bl: %-18s{g|\n\r",
        buf2, buf3, buf4, buf5);
    	send_to_char(buf, ch);
    }

	// pracs, qwest, items
	send_to_char(HOR_BAR, ch);
    
	sprintf(buf2, "%d", ch->practice );
	sprintf(buf3, "%d", (!IS_NPC(ch) ? ch->pcdata->qwest : 0));
    sprintf(buf4, "%d/%d", ch->carry_number, can_carry_n(ch));

    sprintf(buf,
        " |  {wPracs:{c %-15s {wQwest:{m %-15s {wItems   :{y %-15s   {g|\n\r",
         buf2, buf3, buf4);
    send_to_char(buf, ch);

	// trains, gold, weight carried
    sprintf(buf2, "%d", ch->train);
    sprintf(buf3, "%ld", ch->gold);
    sprintf(buf4, "%d/%d", get_carry_weight(ch) / 10, can_carry_w(ch) / 10);
        
    sprintf(buf,
        " |  {wTrain:{c %-15s {wGold :{m %-15s {wWeight  :{y %-15s   {g|\n\r",
        buf2, buf3, buf4);
    send_to_char(buf, ch);

	// hunger, thirst, drunk 
	sprintf(buf2, "%s", (GET_HUNGER(ch) < 10 && GET_HUNGER(ch) > -1) ? "Yes" : "No");
	sprintf(buf3, "%s", (GET_THIRST(ch) < 10 && GET_THIRST(ch) > -1) ? "Yes" : "No"); 
	sprintf(buf4, "%s", GET_DRUNK(ch) > 0 ? "Yes" : "No");

    sprintf(buf,
        " | {wHunger:{c %-15s{wThirst:{m %-15s {wDrunk   :{y %-15s   {g|\n\r",
		buf2, buf3, buf4);
    send_to_char(buf, ch);
    
	send_to_char(HOR_BAR, ch);
   
	// str int dex con wis 
    sprintf(buf,
        " |   {wStr: {g%2d{w({G%2d{w)   Int: {g%2d{w({G%2d{w)   Wis: {g%2d{w({G%2d{w)   Dex: {g%2d{w({G%2d{w)   Con: {g%2d{w({G%2d{w)      {g|\n\r",
ch->perm_stat[STAT_STR], get_curr_stat(ch, STAT_STR),
        ch->perm_stat[STAT_INT], get_curr_stat(ch, STAT_INT),
        ch->perm_stat[STAT_WIS], get_curr_stat(ch, STAT_WIS),
        ch->perm_stat[STAT_DEX], get_curr_stat(ch, STAT_DEX),
        ch->perm_stat[STAT_CON], get_curr_stat(ch, STAT_CON));
    send_to_char(buf, ch);
    
	send_to_char(HOR_BAR, ch);
    
	// pierce, HR, deaths
	sprintf(buf2, "%d", GET_AC(ch, AC_PIERCE));
	sprintf(buf3, "%d", GET_HITROLL(ch));
	sprintf(buf4, "%d", IS_NPC(ch) ? 0 : ch->pcdata->deaths);

    sprintf(buf,
        " | {wPierce:{c %-14s{wHitroll:{m %-15s {wDeaths  :{y %-15s   {g|\n\r",
		buf2, buf3, buf4);
    send_to_char(buf, ch);
   
	// bash, damroll, pkills 
    sprintf(buf2, "%d", GET_AC(ch, AC_BASH));
    sprintf(buf3, "%d", GET_DAMROLL(ch));
    sprintf(buf4, "%d", IS_NPC(ch) ? 0 : ch->pcdata->kills);

    sprintf(buf,
        " | {wBash  :{c %-14s{wDamroll:{m %-15s {wPkills  :{y %-15s   {g|\n\r",
		buf2, buf3, buf4);
    send_to_char(buf, ch);
    
	// slash, wimpy, tnl
    sprintf(buf2, "%d", GET_AC(ch, AC_SLASH));
    sprintf(buf3, "%d", ch->wimpy);
    sprintf(buf4, "%ld", IS_NPC(ch) ? 0 : ((ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp));

    sprintf(buf,
        " | {wSlash :{c %-14s{wWimpy  :{m %-15s {wTo Level:{y %-15s   {g|\n\r",
		buf2, buf3, buf4);
    send_to_char(buf, ch);
    
	// magic, align, last lvl
    sprintf(buf2, "%d", GET_AC(ch, AC_EXOTIC));
    sprintf(buf3, "%d", ch->alignment);
    sprintf(buf4, "%d", (IS_NPC(ch) ? 0 : ch->pcdata->last_level));

    sprintf(buf,
        " | {wMagic :{c %-14s{wAlign  :{m %-15s {wLast Lvl:{y %-15s   {g|\n\r",
        buf2, buf3, buf4);
    send_to_char(buf, ch);

    sprintf(buf,
        " | {wRemort:{c %-13d {wSlots  :{m %-15d                             {g|\n\r",
        ch->remorts, ch->slots);
    send_to_char(buf, ch);
    
    if (IS_IMMORTAL(ch)) { 
		send_to_char(HOR_BAR, ch);
    
    sprintf(buf,
        " | {wHoly light: {g%-3s  {wWiznet: {g%-3s  {wWizi: {g%-3d  {wIncog: {g%-3d  {wSecurity: {g%-2d          {g|\n\r",
		IS_SET(ch->act, PLR_HOLYLIGHT) ? "On" : "Off",
        IS_SET(ch->wiznet, WIZ_ON) ? "On" : "Off",
        ch->invis_level ? ch->invis_level : 0,
        ch->incog_level ? ch->incog_level : 0,
        IS_NPC(ch) ? 0 : ch->pcdata->security);
    send_to_char(buf, ch);
    
    }
		send_to_char(HOR_BAR, ch);
    
    send_to_char("\n\r{y", ch);
    
    if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
    do_affects(ch, "");
    
    send_to_char("{x", ch);
    
    return;
}   


