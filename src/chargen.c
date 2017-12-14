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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <crypt.h>

#include "merc.h"
#include "utils.h"
#include "telnet.h"
#include "recycle.h"
#include "tables.h"
#include "utils.h"

DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_skills);
DECLARE_DO_FUN(do_spells);
DECLARE_DO_FUN(do_outfit);
DECLARE_DO_FUN(do_unread);
DECLARE_DO_FUN(do_asave);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_replay);
DECLARE_DO_FUN(do_quit);

/*
 * Socket and TCP/IP stuff.
 */

const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] = { IAC, GA, '\0' };
bool rolled;
int stat1[5], stat2[5], stat3[5], stat4[5], stat5[5];

// send a class menu during chargen in a box
void class_menu( CHAR_DATA *ch ) {

	char buf[MIL];
	int count;
	char *sep = " {g+----------------------------------------------------------------------------+{x\r\n";
	char *blank = "{g |                                                                            |{x\r\n";

	send_to_char( "\r\n", ch );
	send_to_char( sep, ch );
   	send_to_char( blank, ch );
	send_to_char( "{g |     {WCurrent Classes            Exp Per Level at 40 CP                      {g|{x\r\n", ch );
   	send_to_char( blank, ch );
	send_to_char( " {g|           {y", ch );

	for( count = 0; count < MAX_CLASS; count++) {
		if ( !class_table[count].valid )
			continue;

			ch->class = count;
			sprintf( buf, "%-30s %d", 
			class_table[count].name,
			exp_per_level( ch, 40 ) );

			buf[0] = UPPER(buf[0]);
			send_to_char( buf, ch );
			send_to_char( "                              {g| \r\n{g |           {y", ch );
			

	}
	send_to_char( "                                                                 {g| \r\n", ch );
   	send_to_char( sep, ch );

	return;
}

void do_cm ( CHAR_DATA *ch, char *argument ) {

	class_menu( ch );
	return;
}



void race_menu( CHAR_DATA *ch ) {

	char buf[MIL];
	RACE_DATA *race;
	int i = 0;
	char *sep = " {g+----------------------------------------------------------------------------+{x\r\n";
	char *blank = "{g |                                                                            |{x\r\n";

	send_to_char( "\r\n", ch );
	send_to_char( sep, ch );
   	send_to_char( blank, ch );

			send_to_char( "{g |{W    Race        Points   Max Stats      Race       Points    Max Stats      {g| ", ch);
			send_to_char( "{g |{W                      St In Wi Co De                      St In Wi Co De    {g| ", ch);
	send_to_char( "{g |    ", ch );
   for (race = race_first; race != NULL; race = race->next) {

		if ( !race->pc_race )
			continue;

			sprintf( buf, "%15s{x %-3d %-2d %-2d %-2d %-2d %-2d    ", 
			race->gen_name,
			race->points, 
			race->stats[STAT_STR],
			race->stats[STAT_INT],
			race->stats[STAT_WIS],
			race->stats[STAT_CON],
			race->stats[STAT_DEX] );

			buf[0] = UPPER(buf[0]);
			send_to_char( buf, ch );
			i++;

			if (i % 2 == 0)
				send_to_char("{g|\r\n |    ", ch);
	}

	if (i % 2 != 0)
		send_to_char("                                    {g|\r\n", ch);

   	send_to_char( blank, ch );
   	send_to_char( sep, ch );

	return;
}

void do_rm( CHAR_DATA *ch, char * argument ) {

	race_menu( ch );

	return;

}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name(char *name)
{
    /*
     * Reserved words.
     */
    if (is_name (name,
	 "all auto immortal self someone something the you demise balance circle loner honor"))
	    return FALSE;


    /*
     * Length restrictions.
     */

    if (strlen(name) < 2)
	return FALSE;

    if (strlen(name) > 12)
	return FALSE;

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll, adjcaps = FALSE, cleancaps = FALSE;
	int total_caps = 0;

	fIll = TRUE;
	for (pc = name; *pc != '\0'; pc++) {
	    if (!isalpha(*pc))
		return FALSE;

	    if (isupper(*pc)) {	/* ugly anti-caps hack */
		if (adjcaps)
		    cleancaps = TRUE;
		total_caps++;
		adjcaps = TRUE;
	    } else
		adjcaps = FALSE;

	    if (LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
		fIll = FALSE;
	}

	if (fIll)
	    return FALSE;

	if (cleancaps
	    || (total_caps > (strlen(name)) / 2
		&& strlen(name) < 3)) return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	    for (pMobIndex = mob_index_hash[iHash];
		 pMobIndex != NULL; pMobIndex = pMobIndex->next) {
		if (is_name(name, pMobIndex->player_name))
		    return FALSE;
	    }
	}
    }

    return TRUE;
}

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny(DESCRIPTOR_DATA * d, char *argument)
{
    DESCRIPTOR_DATA *d_old, *d_next;
	RACE_DATA *race;
    extern const char *ansi_greeting;
    extern const char *no_ansi_greeting;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int iClass, i, x, weapon, iStat;
    bool fOld;
    bool BadStat = FALSE;


    /* Delete leading spaces UNLESS character is writing a note */


    while (isspace(*argument))
	argument++;

    ch = d->character;

    switch (d->connected) {

    default:
	bug("Nanny: bad d->connected %d.", d->connected);
	close_socket(d);
	return;

    case CON_ASK_IF_COLOUR:

	if ((argument[0] == 'n') || (argument[0] == 'N'))
	    d->ansi = FALSE;
	else
	    d->ansi = TRUE;

	/*
	 * Send the greeting.
	 */
	{

	    if (d->ansi) {

		write_color_to_buffer(ansi_greeting, d);

	    } else {

		write_to_buffer(d, no_ansi_greeting, 0);
	    }
	}

	d->connected = CON_GET_NAME;
	return;

    case CON_GET_NAME:
	if (argument[0] == '\0') {
	    close_socket(d);
	    return;
	}
	argument[0] = UPPER(argument[0]);
	if (!check_parse_name(argument)) {
	    write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
	    return;
	}
	fOld = load_char_obj(d, argument);
	ch = d->character;

	if (IS_SET(ch->act, PLR_DENY)) {
	    sprintf(log_buf, "Denying access to %s@%s.", argument,
		    d->host);
	    log_string(log_buf);
	    write_to_buffer(d, "You are denied access.\n\r", 0);
	    close_socket(d);
	    return;
	}
	if (check_reconnect(d, argument, FALSE)) {
	    fOld = TRUE;
	} else {
	    if (wizlock && !IS_HERO(ch)) {
		write_to_buffer(d, "The game is wizlocked.\n\r", 0);
		close_socket(d);
		return;
	    }
	}

	if (fOld) {
	    if (d->ansi)
		write_to_buffer(d, C_GREEN, 0);
	    write_to_buffer(d, "Password: ", 0);
	    write_to_buffer(d, CLEAR, 0);
	    write_to_buffer(d, echo_off_str, 0);
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	} else {
	    /* New player */
	    if (newlock) {
		write_to_buffer(d, "The game is newlocked.\n\r", 0);
		close_socket(d);
		return;
	    }
	    sprintf(buf,
		    "Please make sure that your name is appropriate for a fantasy realm.\nDid I get that right, %s (Y/N)? ",
		    argument);
	    write_to_buffer(d, buf, 0);
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}

	if (check_playing(d, ch->name))
	    return;

	break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
	write_to_buffer(d, "\n\r", 2);
#endif

	if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)) {
	    write_to_buffer(d, "Wrong password.\n\r", 0);
	    close_socket(d);
	    return;
	}
	if (ch->pcdata->pwd[0] == 0) {
	    write_to_buffer(d, "Warning! Null password!\n\r", 0);
	    write_to_buffer(d, "Please report old password with bug.\n\r",
			    0);
	    write_to_buffer(d,
			    "Type 'password null <new password>' to fix.\n\r",
			    0);
	}
	write_to_buffer(d, echo_on_str, 0);

	if (check_reconnect(d, ch->name, TRUE))
	    return;

	if (check_playing(d, ch->name))
	    return;

	sprintf(log_buf, "%s@%s has connected.", ch->name, d->host);

	log_string(log_buf);
	wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
	if (IS_HERO(ch)) {
	    do_help(ch, "imotd");
	    d->connected = CON_READ_IMOTD;
	} else {
	    do_help(ch, "motd");
	    d->connected = CON_READ_MOTD;
	}
	break;

/* RT code for breaking link */

    case CON_BREAK_CONNECT:
	switch (*argument) {
	case 'y':
	case 'Y':
	    for (d_old = descriptor_list; d_old != NULL; d_old = d_next) {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
		    continue;

		if (str_cmp(ch->name, d_old->character->name))
		    continue;

		close_socket(d_old);
	    }
	    if (check_reconnect(d, ch->name, TRUE))
		return;
	    write_to_buffer(d, "Reconnect attempt failed.\n\rName: ", 0);
	    if (d->character != NULL) {
		free_char(d->character);
		d->character = NULL;
	    }
	    d->connected = CON_GET_NAME;
	    break;

	case 'n':
	case 'N':
	    write_to_buffer(d, "Name: ", 0);
	    if (d->character != NULL) {
		free_char(d->character);
		d->character = NULL;
	    }
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d, "Please type Y or N? ", 0);
	    break;
	}
	close_socket(ch->desc);
	break;

    case CON_CONFIRM_NEW_NAME:
	switch (*argument) {
	case 'y':
	case 'Y':
	    sprintf(buf, "New character.\n\rGive me a password for %s: %s",
		    ch->name, echo_off_str);
	    write_to_buffer(d, buf, 0);
	    d->connected = CON_GET_NEW_PASSWORD;
	    if (d->ansi)
		SET_BIT(ch->act, PLR_COLOUR);
	    break;

	case 'n':
	case 'N':
	    write_to_buffer(d, "Ok, what IS it, then? ", 0);
	    free_char(d->character);
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d, "Please type Yes or No? ", 0);
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer(d, "\n\r", 2);
#endif

	if (strlen(argument) < 5) {
	    write_to_buffer(d,
			    "Password must be at least five characters long.\n\rPassword: ",
			    0);
	    return;
	}
	pwdnew = crypt(argument, ch->name);
	for (p = pwdnew; *p != '\0'; p++) {
	    if (*p == '~') {
		write_to_buffer(d,
				"New password not acceptable, try again.\n\rPassword: ",
				0);
		return;
	    }
	}

	free_string(ch->pcdata->pwd);
	ch->pcdata->pwd = str_dup(pwdnew);
	write_to_buffer(d, "Please retype password: ", 0);
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer(d, "\n\r", 2);
#endif

	if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)) {
	    write_to_buffer(d,
			    "Passwords don't match.\n\rRetype password: ",
			    0);
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}
	write_to_buffer(d, echo_on_str, 0);
	write_to_buffer(d, "The following races are available:\n\r ", 0);

	race_menu( ch );

	write_to_buffer(d, "\r\n ", 1);
	write_to_buffer(d, "\n\r", 0);
	write_to_buffer(d,
			"What is your race (help for more information)? ",
			0);
	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	one_argument(argument, arg);

	if (!strcmp(arg, "help")) {
	    argument = one_argument(argument, arg);
	    if (argument[0] == '\0')
		do_help(ch, "race help");
	    else
		do_help(ch, argument);

		race_menu( ch );

	    write_to_buffer(d, "\r\n ", 1);
	    write_to_buffer(d, "\n\r", 0);
	    write_to_buffer(d,
			    "What is your race (help for more information)? ",
			    0);
	    break;
	}
	race = gen_race_lookup(argument);

	if ((!race) || !race->pc_race ) {
	    write_to_buffer(d, "That's not a valid race!\n\r", 0);
	    write_to_buffer(d, "The following races are available:\n\r ",
			    0);

		race_menu( ch );

	    write_to_buffer(d,
			    "What is your race? (help for more information) ",
			    0);
	    break;
	}
	ch->race = race;
	/* initialize stats */
	for (i = 0; i < MAX_STATS; i++)
	    ch->perm_stat[i] = race->stats[i];

	ch->affected_by = ch->affected_by | race->aff;
	ch->imm_flags = ch->imm_flags | race->imm;
	ch->res_flags = ch->res_flags | race->res;
	ch->vuln_flags = ch->vuln_flags | race->vuln;
	ch->form = race->form;
	ch->parts = race->parts;

	/* add racial skills */
	for (i = 0; i < 5; i++) {
	    if (race->skills[i] == NULL)
		break;
	    group_add(ch, race->skills[i], FALSE);
	}
	/* add cost */
	ch->pcdata->points = race->points;
	ch->size = race->size;
	write_to_buffer(d, "The Following classes are available:\r\n", 0);
	class_menu( ch );
	write_to_buffer(d, "\r\nSelect a class: ", 0);
	d->connected = CON_GET_NEW_CLASS;
	break;


    case CON_GET_NEW_CLASS:

	if (!strcmp(arg, "help")) {
	    argument = one_argument(argument, arg);
	    if (argument[0] == '\0')
		do_help(ch, "class help");
	    else
		do_help(ch, argument);
	}

	iClass = class_lookup(argument);

	if ((iClass == -1) || !class_table[iClass].valid ) {
	    write_to_buffer(d, "That's not a class!\r\n", 0);
		class_menu( ch );
	    write_to_buffer(d, "\r\nSelect a class: ", 0);
	    return;
	}
	ch->class = iClass;

	sprintf(log_buf, "%s@%s new player.", ch->name, d->host);
	log_string(log_buf);
	wiznet("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
	wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
	write_to_buffer(d, "\n\r", 2);
	write_to_buffer(d, "What is your sex (M/F)? ", 0);
	d->connected = CON_GET_NEW_SEX;
	break;

    case CON_GET_NEW_SEX:
	switch (argument[0]) {
	case 'm':
	case 'M':
	    ch->sex = SEX_MALE;
	    ch->pcdata->true_sex = SEX_MALE;
	    break;
	case 'f':
	case 'F':
	    ch->sex = SEX_FEMALE;
	    ch->pcdata->true_sex = SEX_FEMALE;
	    break;
	default:
	    write_to_buffer(d, "That's not a sex.\n\rWhat IS your sex? ",
			    0);
	    return;
	}
	write_to_buffer(d, "\r\nPress enter to start rolling your stats.",
			0);
	d->connected = CON_GET_STATS;
	break;

    case CON_GET_STATS:
	if (rolled == TRUE && (argument[0] > 4) && (argument[1] < 1))
	    switch (argument[0]) {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
		ch->perm_stat[0] = stat1[atoi(argument)];
		ch->perm_stat[1] = stat2[atoi(argument)];
		ch->perm_stat[2] = stat3[atoi(argument)];
		ch->perm_stat[3] = stat4[atoi(argument)];
		ch->perm_stat[4] = stat5[atoi(argument)];
		write_to_buffer(d,
				"\r\nYou may be good, neutral, or evil.\n\r",
				0);
		write_to_buffer(d, "Which alignment (G/N/E)? ", 0);
		d->connected = CON_GET_ALIGNMENT;

		break;
	    default:
		write_to_buffer(d, "\n\r", 0);
		write_to_buffer(d,
				"                       0    1    2    3    4\n\r",
				0);
		write_to_buffer(d, "     Strength     :", 0);
		for (x = 0; x < 5; x++) {
		    stat1[x] = number_range(9, 16);
		    sprintf(buf, "   %2d", stat1[x]);
		    write_to_buffer(d, buf, 0);
		}
		write_to_buffer(d, "\n\r     Intelligence :", 0);
		for (x = 0; x < 5; x++) {
		    stat2[x] = number_range(9, 16);
		    sprintf(buf, "   %2d", stat2[x]);
		    write_to_buffer(d, buf, 0);
		}
		write_to_buffer(d, "\n\r     Wisdom       :", 0);
		for (x = 0; x < 5; x++) {
		    stat3[x] = number_range(9, 16);
		    sprintf(buf, "   %2d", stat3[x]);
		    write_to_buffer(d, buf, 0);
		}
		write_to_buffer(d, "\n\r     Dexterity    :", 0);
		for (x = 0; x < 5; x++) {
		    stat4[x] = number_range(9, 16);
		    sprintf(buf, "   %2d", stat4[x]);
		    write_to_buffer(d, buf, 0);
		}
		write_to_buffer(d, "\n\r     Constitution :", 0);
		for (x = 0; x < 5; x++) {
		    stat5[x] = number_range(9, 16);
		    sprintf(buf, "   %2d", stat5[x]);
		    write_to_buffer(d, buf, 0);
		}
		write_to_buffer(d,
				"\n\r\n\r     Press enter to roll again, else enter number of column: ",
				0);
		rolled = TRUE;
		break;
	} else {
	    write_to_buffer(d, "\r\n", 0);
	    write_to_buffer(d,
			    "                       0    1    2    3    4\n\r",
			    0);
	    write_to_buffer(d, "     Strength     :", 0);
	    for (x = 0; x < 5; x++) {
		stat1[x] = number_range(9, 16);
		sprintf(buf, "   %2d", stat1[x]);
		write_to_buffer(d, buf, 0);
	    }
	    write_to_buffer(d, "\n\r     Intelligence :", 0);
	    for (x = 0; x < 5; x++) {
		stat2[x] = number_range(9, 16);
		sprintf(buf, "   %2d", stat2[x]);
		write_to_buffer(d, buf, 0);
	    }
	    write_to_buffer(d, "\n\r     Wisdom       :", 0);
	    for (x = 0; x < 5; x++) {
		stat3[x] = number_range(9, 16);
		sprintf(buf, "   %2d", stat3[x]);
		write_to_buffer(d, buf, 0);
	    }
	    write_to_buffer(d, "\n\r     Dexterity    :", 0);
	    for (x = 0; x < 5; x++) {
		stat4[x] = number_range(9, 16);
		sprintf(buf, "   %2d", stat4[x]);
		write_to_buffer(d, buf, 0);
	    }
	    write_to_buffer(d, "\n\r     Constitution :", 0);
	    for (x = 0; x < 5; x++) {
		stat5[x] = number_range(9, 16);
		sprintf(buf, "   %2d", stat5[x]);
		write_to_buffer(d, buf, 0);
	    }
	    write_to_buffer(d,
			    "\n\r\n\r     Press enter to roll again, else enter number of column: ",
			    0);
	    rolled = TRUE;
	}
	break;

    case CON_GET_ALIGNMENT:
	switch (argument[0]) {
	case 'g':
	case 'G':
	    ch->alignment = 750;
	    break;
	case 'n':
	case 'N':
	    ch->alignment = 0;
	    break;
	case 'e':
	case 'E':
	    ch->alignment = -750;
	    break;
	default:
	    write_to_buffer(d, "That's not a valid alignment.\n\r", 0);
	    write_to_buffer(d, "Which alignment (G/N/E)? ", 0);
	    return;
	}

	write_to_buffer(d, "\n\r", 0);

	group_add(ch, "rom basics", FALSE);
	group_add(ch, class_table[ch->class].base_group, FALSE);
	ch->pcdata->learned[gsn_recall] = 50;
	write_to_buffer(d, "Do you wish to customize this character?\n\r",
			0);
	write_to_buffer(d,
			"Customization takes time, but allows a wider range of skills and abilities.\n\r",
			0);
	write_to_buffer(d, "Customize (Y/N)? ", 0);
	d->connected = CON_DEFAULT_CHOICE;
	break;

    case CON_DEFAULT_CHOICE:
	write_to_buffer(d, "\n\r", 2);
	switch (argument[0]) {
	case 'y':
	case 'Y':
	    ch->gen_data = alloc_perm(sizeof(*ch->gen_data));
	    ch->gen_data->points_chosen = ch->pcdata->points;
	    do_help(ch, "group header");
	    list_group_costs(ch);
	    write_to_buffer(d,
			    "You already have the following skills:\n\r",
			    0);
	    do_skills(ch, "");
	    do_help(ch, "menu choice");
	    d->connected = CON_GEN_GROUPS;
	    break;
	case 'n':
	case 'N':
	    group_add(ch, class_table[ch->class].default_group, TRUE);
	    write_to_buffer(d, "\n\r", 2);
	    write_to_buffer(d,
			    "Please pick a weapon from the following choices:\n\r\r\n",
			    0);
	    buf[0] = '\0';
	    for (i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0) {
		    strcat(buf, weapon_table[i].name);
		    strcat(buf, " ");
		}
	    strcat(buf, "\n\r\n\rYour choice? ");
	    write_to_buffer(d, buf, 0);
	    d->connected = CON_PICK_WEAPON;
	    break;
	default:
	    write_to_buffer(d, "Please answer (Y/N)? ", 0);
	    return;
	}
	break;

    case CON_PICK_WEAPON:
	write_to_buffer(d, "\n\r", 2);
	weapon = weapon_lookup(argument);
	if (weapon == -1
	    || ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0) {
	    write_to_buffer(d,
			    "That's not a valid selection. Choices are:\n\r",
			    0);
	    buf[0] = '\0';
	    for (i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0) {
		    strcat(buf, weapon_table[i].name);
		    strcat(buf, " ");
		}
	    strcat(buf, "\n\rYour choice? ");
	    write_to_buffer(d, buf, 0);
	    return;
	}
	ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;

	if (IS_IMMORTAL(ch)) {
	    do_help(ch, "imotd");
	    d->connected = CON_READ_IMOTD;
	} else {
	    do_help(ch, "motd");
	    d->connected = CON_READ_MOTD;
	}
	break;

    case CON_GEN_GROUPS:
	send_to_char("\n\r", ch);
	if (!str_cmp(argument, "done")) {
	    sprintf(buf, "Creation points: %d\n\r", ch->pcdata->points);
	    send_to_char(buf, ch);
	    if (ch->pcdata->points < 40)
		ch->train = (40 - ch->pcdata->points + 1) / 2;
	    send_to_char(buf, ch);
	    send_to_char
		("\r\nPlease pick a weapon from the following choices:\n\r",
		 ch);
	    buf[0] = '\0';
	    for (i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0) {
		    strcat(buf, weapon_table[i].name);
		    strcat(buf, "\r\n ");
		}
	    strcat(buf, "\n\rYour choice? ");
	    write_to_buffer(d, buf, 0);
	    d->connected = CON_PICK_WEAPON;
	    break;
	}
	if (!parse_gen_groups(ch, argument))
	    send_to_char
		("Choices are: list, learned, premise, add, drop, info, help, and done.\n\r",
		 ch);

	send_to_char( "Choice (add,drop,list,help)? ", ch );
	break;

    case CON_READ_IMOTD:
	write_to_buffer(d, "\n\r", 2);
	do_help(ch, "motd");
	d->connected = CON_READ_MOTD;
	break;

    case CON_READ_MOTD:

	if (ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0') {
	    write_to_buffer(d, "Warning! Null password!\n\r", 0);
	    write_to_buffer(d, "Please report old password with bug.\n\r",
			    0);
	    write_to_buffer(d,
			    "Type 'password null <new password>' to fix.\n\r",
			    0);
	}
	send_to_char
	    ("Welcome to Alanthia! Please do not feed the mobiles.\n\r",
	     ch);

	ch->next = char_list;
	char_list = ch;
	d->connected = CON_PLAYING;
	reset_char(ch);

	if (ch->max_blood < ch->level) {
	    ch->max_blood = ch->level;
	    ch->pcdata->perm_blood = ch->level;
	}
	if (ch->level == 0) {
	    char color_title[] = { 'G', 'M', 'B', 'C', 'Y', 'R' };

	    char color = color_title[number_range(0, 5)];
	    ch->perm_stat[class_table[ch->class].attr_prime] += 1;
	    ch->level = 1;
	    ch->exp = exp_per_level(ch, ch->pcdata->points);
	    ch->hit = ch->max_hit;
	    ch->mana = ch->max_mana;
	    ch->move = ch->max_move;
	    ch->blood = ch->max_blood;
	    ch->train = 3;
	    ch->practice = 5;
	    ch->outfit = FALSE;

	    if (IS_VAMPIRE(ch) || IS_WRAITH(ch)) {
		ch->pcdata->condition[COND_THIRST] = -1;
		ch->pcdata->condition[COND_HUNGER] = -1;
		ch->pcdata->condition[COND_DRUNK] = -1;
	    }
	    SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
	    SET_BIT(ch->comm, COMM_ANNOUNCE);
	    SET_BIT(ch->act, PLR_AUTOEXIT);
	    SET_BIT(ch->act, PLR_AUTODAMAGE);
	    SET_BIT(ch->act, PLR_AUTOLOOT);
	    SET_BIT(ch->act, PLR_AUTOSAC);
	    SET_BIT(ch->act, PLR_AUTOASSIST);
	    SET_BIT(ch->act, PLR_AUTOSPLIT);

	    ch->prompt = str_dup("{c<%hhp %mm %vmv %bbp>{x ");

	    sprintf(buf, "the {%cN{%ceWBiE{x", color, LOWER(color));
	    set_title(ch, buf);

	    write_to_buffer(d, "\n\r", 2);

	    char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));

	    do_outfit(ch, "");

	    send_to_char("\n\r", ch);
	    do_help(ch, "NEWBIE INFO");
	    send_to_char("\n\r", ch);
	} else if (ch->in_room != NULL) {
	    char_to_room(ch, ch->in_room);
	} else if (IS_IMMORTAL(ch)) {
	    char_to_room(ch, get_room_index(ROOM_VNUM_CHAT));
	} else {
	    char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
	}


/* 
 *  Sanity checking for bogus levels, bad stats, etc. WDL, 4.10.98
 */

	/* Check for bad stats */

	for (iStat = 0; iStat < 5; iStat++) {
	    if ((ch->perm_stat[iStat] >= 26)
		|| (ch->perm_stat[iStat] <= 0)) {
		ch->perm_stat[iStat] = 12;
		BadStat = TRUE;
	    }
	}

	/* Holler loudly */
	if (BadStat == TRUE) {
	    wiznet("$N had bogus stat, fixing...", ch, NULL,
		   WIZ_SITES, 0, 0);

	    sprintf(log_buf, "**BUG** %s@%s had bogus stats.", ch->name,
		    d->host);
	    log_string(log_buf);
	}
	/* Check for bad levels */
	if ((ch->level < 0) || (ch->level > MAX_LEVEL)) {
	    ch->level = 1;

	    wiznet("$N has bogus level! Possible hack attempt!", ch, NULL,
		   WIZ_SITES, 0, 0);

	    sprintf(log_buf, "**BUG** %s@%s used bogus level! Kill them.",
		    ch->name, d->host);
	    log_string(log_buf);
	}
	act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);

	/* For the Announce Channel */
	announce("{c[{RANNOUNCE{c] $n has stumbled into Alanthia.{x", ch);

	do_look(ch, "auto");
	save_char_obj(ch);
	wiznet("$N has left real life behind.", ch, NULL, WIZ_LOGINS,
	       WIZ_SITES, get_trust(ch));

	if (ch->pet != NULL) {
	    char_to_room(ch->pet, ch->in_room);
	    act("$n appears in the room.", ch->pet, NULL, NULL, TO_ROOM);
	}
	do_unread(ch, "");
	break;
    }

    return;
}
