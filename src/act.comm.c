/*AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH*/

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
 **************************************************************************/

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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "utils.h"
#include "recycle.h"
#include "tables.h"
#include "interp.h"

extern bool is_ignoring(CHAR_DATA * ch, CHAR_DATA * victim);

/* command procedures needed */
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_swear);
DECLARE_DO_FUN(do_replay);
DECLARE_DO_FUN(do_afk);

/*
 * Local Functions
 */
bool check_swear args((CHAR_DATA * ch, char *argument));
void color args((CHAR_DATA * ch, const char *string));

/* RT code to delete yourself */

void do_delet(CHAR_DATA * ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\n\r",
		 ch);
}

void do_delete(CHAR_DATA * ch, char *argument)
{
    char strsave[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    if (ch->pcdata->confirm_delete) {
	if (argument[0] != '\0') {
	    send_to_char("Delete status removed.\n\r", ch);
	    ch->pcdata->confirm_delete = FALSE;
	    return;
	} else {
	    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
	    wiznet("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
	    stop_fighting(ch, TRUE);
	    do_quit(ch, "");
	    unlink(strsave);
	    return;
	}
    }
    if (argument[0] != '\0') {
	send_to_char("Just type delete. No argument.\n\r", ch);
	return;
    }
    send_to_char("Type delete again to confirm this command.\n\r", ch);
    send_to_char("{RWARNING: this command is irreversible.{x\n\r", ch);
    send_to_char
	("Typing delete with an argument will undo delete status.\n\r",
	 ch);
    ch->pcdata->confirm_delete = TRUE;
    wiznet("$N is contemplating deletion.", ch, NULL, 0, 0, get_trust(ch));
}

/* RT code to become a pkiller */

void do_pkil(CHAR_DATA * ch, char *argument)
{
    send_to_char("You must type the full command to become a PKILLER.\n\r",
		 ch);
}

void do_pkill(CHAR_DATA * ch, char *argument)
{

    if (IS_NPC(ch))
	return;

    if (ch->clan == 11) {
	send_to_char("You're in a passive clan.\n\r", ch);
	return;
    }

    if (IS_SET(ch->act, PLR_PKILL)) {
	send_to_char("Death is the only way to remove pkill status.\n\r",
		     ch);
	return;
    }

    if (ch->level <= 1) {
	send_to_char
	    ("You must be at least level 2 to gain pkill status.\n\r", ch);
	return;
    }

    if (ch->pcdata->confirm_pkill) {
	if (argument[0] != '\0') {
	    send_to_char("Pkill status removed.\n\r", ch);
	    ch->pcdata->confirm_pkill = FALSE;
	    return;
	} else {
	    send_to_char("\n\r{r*** {RYou are now a PKILLER!! {r***{x\n\r",
			 ch);
	    SET_BIT(ch->act, PLR_PKILL);
	    announce("{c[{RPKILL{c] $n is now a PKILLER!{x", ch);
	    return;
	}
    }

    if (argument[0] != '\0') {
	send_to_char("Just type Pkill. No argument.\n\r", ch);
	return;
    }

    send_to_char("Type pkill again to confirm this command.\n\r", ch);
    send_to_char("{RWARNING: this command is irreversible.{x\n\r", ch);
    send_to_char
	("Typing pkill with an argument will undo pkill status.\n\r", ch);
    ch->pcdata->confirm_pkill = TRUE;
}

/* RT code to display channel status */

void do_channels(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    send_to_char("   channel     status\n\r", ch);
    send_to_char("---------------------\n\r", ch);

    color(ch, "gossip");	/* Customizable color */

    send_to_char("gossip         ", ch);
    if (!IS_SET(ch->comm, COMM_NOGOSSIP))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "auction");	/* Customizable color */

    send_to_char("auction        ", ch);
    if (!IS_SET(ch->comm, COMM_NOAUCTION))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "music");		/* Customizable color */

    send_to_char("music          ", ch);
    if (!IS_SET(ch->comm, COMM_NOMUSIC))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "question");	/* Customizable color */

    send_to_char("Q/A            ", ch);
    if (!IS_SET(ch->comm, COMM_NOQUESTION))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "qwest");		/* Customizable color */

    send_to_char("qwest          ", ch);
    if (!IS_SET(ch->comm, COMM_NOQWEST))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "gratz");		/* Customizable color */

    send_to_char("gratz          ", ch);
    if (!IS_SET(ch->comm, COMM_NOGRATZ))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "swear");		/* Customizable color */

    send_to_char("swear          ", ch);
    if (!IS_SET(ch->comm, COMM_NOSWEAR))
	send_to_char("ON{x\n\r", ch);
    else
	send_to_char("OFF{x\n\r", ch);

    if (IS_IMMORTAL(ch)) {
	send_to_char("{cgod channel    ", ch);
	if (!IS_SET(ch->comm, COMM_NOWIZ))
	    send_to_char("ON\n\r", ch);
	else
	    send_to_char("OFF\n\r", ch);
    }
    if (IS_IMMORTAL(ch) || ch->level <= 10) {
	send_to_char("{Gnewbie         ", ch);
	if (!IS_SET(ch->comm, COMM_NONEWBIE))
	    send_to_char("ON\n\r", ch);
	else
	    send_to_char("OFF\n\r", ch);
    }
    color(ch, "shout");		/* Customizable color */

    send_to_char("shouts         ", ch);
    if (!IS_SET(ch->comm, COMM_SHOUTSOFF))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    color(ch, "tell");		/* Customizable color */

    send_to_char("tells          ", ch);
    if (!IS_SET(ch->comm, COMM_DEAF))
	send_to_char("ON\n\r", ch);
    else
	send_to_char("OFF\n\r", ch);

    send_to_char("{Rannounce       ", ch);
    if (IS_SET(ch->comm, COMM_ANNOUNCE))
	send_to_char("ON{x\n\r", ch);
    else
	send_to_char("OFF{x\n\r", ch);

    color(ch, "pray");		/* Customizable color */

    send_to_char("pray channel   ", ch);
    if (!IS_SET(ch->comm, COMM_NOPRAY))
	send_to_char("ON{x\n\r", ch);
    else
	send_to_char("OFF{x\n\r", ch);

    color(ch, "argue");		/* Customizable color */

    send_to_char("argue channel  ", ch);
    if (!IS_SET(ch->comm, COMM_NOARGUE))
	send_to_char("ON{x\n\r", ch);
    else
	send_to_char("OFF{x\n\r", ch);

    send_to_char("fs channel     ", ch);
    if (!IS_SET(ch->comm, COMM_NOFARSPEAK))
	send_to_char("ON{x\n\r", ch);
    else
	send_to_char("OFF{x\n\r", ch);

    send_to_char("{bquiet mode     ", ch);
    if (IS_SET(ch->comm, COMM_QUIET))
	send_to_char("ON{x\n\r", ch);
    else
	send_to_char("OFF{x\n\r", ch);


    if (IS_SET(ch->comm, COMM_AFK))
	send_to_char("You are AFK.\n\r", ch);

    if (IS_SET(ch->comm, COMM_SNOOP_PROOF))
	send_to_char("You are immune to snooping.\n\r", ch);

    if (ch->lines != PAGELEN) {
	if (ch->lines) {
	    sprintf(buf, "You display %d lines of scroll.\n\r",
		    ch->lines + 2);
	    send_to_char(buf, ch);
	} else
	    send_to_char("Scroll buffering is off.\n\r", ch);
    }
    if (ch->prompt != NULL) {
	sprintf(buf, "Your current prompt is: %s\n\r", ch->prompt);
	send_to_char(buf, ch);
    }
    if (IS_SET(ch->comm, COMM_NOSHOUT))
	send_to_char("You cannot shout.\n\r", ch);

    if (IS_SET(ch->comm, COMM_NOTELL))
	send_to_char("You cannot use tell.\n\r", ch);

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
	send_to_char("You cannot use channels.\n\r", ch);

    if (IS_SET(ch->comm, COMM_NOEMOTE))
	send_to_char("You cannot show emotions.\n\r", ch);

}

/* RT deaf blocks out all shouts */

void do_deaf(CHAR_DATA * ch, char *argument)
{

    if (IS_SET(ch->comm, COMM_DEAF)) {
	send_to_char("{YYou can now hear tells again.{x\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_DEAF);
    } else {
	send_to_char("{YFrom now on, you won't hear tells.{x\n\r", ch);
	SET_BIT(ch->comm, COMM_DEAF);
    }
}

/* RT quiet blocks out all communication */

void do_quiet(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("{bQuiet mode removed.{x\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_QUIET);
    } else {
	send_to_char
	    ("{bFrom now on, you will only hear says and emotes.{x\n\r",
	     ch);
	SET_BIT(ch->comm, COMM_QUIET);
    }
}

void do_announce(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_ANNOUNCE)) {
	send_to_char("{RAnnounce channel is OFF.{x\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_ANNOUNCE);
    } else {
	send_to_char("{RAnnounce channel is ON.{x\n\r", ch);
	SET_BIT(ch->comm, COMM_ANNOUNCE);
    }
}

/* afk command */

void do_afk(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_AFK)) {
	send_to_char("AFK mode removed.\n\r", ch);
	do_replay(ch, "");
	act("$n has returned to $s keyboard.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->comm, COMM_AFK);
    } else {
	send_to_char("You are now in AFK mode.\n\r", ch);
	act("$n goes away from $s keyboard.", ch, NULL, NULL, TO_ROOM);
	SET_BIT(ch->comm, COMM_AFK);
    }
}

void do_replay(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch)) {
	send_to_char("You can't replay.\n\r", ch);
	return;
    }
    if (buf_string(ch->pcdata->buffer)[0] == '\0') {
	send_to_char("You have no tells to replay.\n\r", ch);
	return;
    }
    send_to_char("You received the following tells:\n\r", ch);

    page_to_char(buf_string(ch->pcdata->buffer), ch);
    clear_buf(ch->pcdata->buffer);
}

/* RT auction rewritten in ROM style */
void do_auction(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "auction");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOAUCTION)) {
	    send_to_char("Auction channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOAUCTION);
	} else {
	    send_to_char("Auction channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOAUCTION);
	}
    } else {			/* auction message sent, turn auction on if it is off */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOAUCTION);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);


	color(ch, "auction");	/* Customizable color */
	sprintf(buf, "You auction '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOAUCTION) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "auction");	/* Customizable color */
		act_new("$n auctions '$t'{x",
			ch, argument, d->character, TO_VICT, POS_DEAD);
	    }
	}
    }
}


void do_swear(CHAR_DATA * ch, char *argument)
{
    color(ch, "swear");		/* Customizable color */

    if (IS_SET(ch->comm, COMM_NOSWEAR)) {
	send_to_char("Swear channel is now ON.{x\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_NOSWEAR);
    } else {
	send_to_char("Swear channel is now OFF.{x\n\r", ch);
	SET_BIT(ch->comm, COMM_NOSWEAR);
    }
    return;
}


/* RT chat replaced with ROM gossip */

void do_gossip(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "gossip");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOGOSSIP)) {
	    send_to_char("(OOC) Gossip channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
	} else {
	    send_to_char("(OOC) Gossip channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGOSSIP);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;

	}
	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "gossip");	/* Customizable color */

	sprintf(buf, "(OOC) You gossip '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGOSSIP) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "gossip");	/* Customizable color */
		act_new("(OOC) $n gossips '$t'{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* wdl 2/28/98 */

void do_farspeak(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "farspeak");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOFARSPEAK)) {
	    send_to_char("{YFarSpeak channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOFARSPEAK);
	} else {
	    send_to_char("{YFarSpeak channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOFARSPEAK);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;

	}
	REMOVE_BIT(ch->comm, COMM_NOFARSPEAK);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "farspeak");	/* Customizable color */

	sprintf(buf, "You FarSpeak '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOFARSPEAK) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "farspeak");	/* Customizable color */
		act_new("$n speaks from afar, '$t'{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}


void do_gratz(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "gratz");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOGRATZ)) {
	    send_to_char("GratZ channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGRATZ);
	} else {
	    send_to_char("GratZ channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGRATZ);
	}
    } else {			/* gratz message sent, turn gratz on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;

	}
	REMOVE_BIT(ch->comm, COMM_NOGRATZ);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "gratz");	/* Customizable color */

	sprintf(buf, "You gratZ '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGRATZ) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "gratz");	/* Customizable color */
		act_new("$n gratZes '$t'{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

void do_qwest(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "qwest");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOQWEST)) {
	    send_to_char("Qwest channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOQWEST);
	} else {
	    send_to_char("Qwest channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOQWEST);
	}
    } else {			/* qwest message sent, turn qwest on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;

	}
	REMOVE_BIT(ch->comm, COMM_NOQWEST);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);
	if ( strlen( argument ) > MAX_STRING_LENGTH - 4 )
		argument = "bonk.";

	color(ch, "qwest");	/* Customizable color */

	snprintf(buf, MSL, "You QWEST: * %s *{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOQWEST) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "qwest");	/* Customizable color */
		act_new("$n QWEST: * $t *{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* RT question channel */
void do_question(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "question");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOQUESTION)) {
	    send_to_char("Q/A channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOQUESTION);
	} else {
	    send_to_char("Q/A channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOQUESTION);
	}
    } else {			/* question sent, turn Q/A on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOQUESTION);


	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "question");	/* Customizable color */

	sprintf(buf, "You question '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOQUESTION) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "question");	/* Customizable color */
		act_new("$n questions '$t'{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* RT answer channel - uses same line as questions */
void do_answer(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "question");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOQUESTION)) {
	    send_to_char("Q/A channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOQUESTION);
	} else {
	    send_to_char("Q/A channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOQUESTION);
	}
    } else {			/* answer sent, turn Q/A on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOQUESTION);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "question");	/* Customizable color */

	sprintf(buf, "You answer '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOQUESTION) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "question");	/* Customizable color */
		act_new("$n answers '$t'{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}


void do_argue(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "argue");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOARGUE)) {
	    send_to_char("Argue channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOARGUE);
	} else {
	    send_to_char("Argue channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOARGUE);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("The gods have revoked your channel privileges.\n\r",
		     ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOARGUE);

    if (ch->level <= 1) {
	send_to_char
	    ("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
	     ch);
	return;
    }
    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    color(ch, "argue");		/* Customizable color */

    sprintf(buf, "You argue '%s'{x\n\r", argument);
    send_to_char(buf, ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    !IS_SET(victim->comm, COMM_NOARGUE) &&
	    !IS_SET(victim->comm, COMM_QUIET)) {
	    color(d->character, "argue");	/* Customizable color */
	    act_new("$n argues '$t'{x",
		    ch, argument, d->character, TO_VICT, POS_SLEEPING);
	}
    }
}


/* RT music channel */
void do_music(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "music");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOMUSIC)) {
	    send_to_char("Music channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOMUSIC);
	} else {
	    send_to_char("Music channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOMUSIC);
	}
    } else {			/* music sent, turn music on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char
		("The gods have revoked your channel privileges.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOMUSIC);

	if (ch->level <= 1) {
	    send_to_char
		("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
		 ch);
	    return;
	}
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "music");	/* Customizable color */

	sprintf(buf, "You MUSIC: '%s'{x\n\r", argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOMUSIC) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		color(d->character, "music");	/* Customizable color */
		act_new("$n MUSIC: '$t'{x",
			ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* clan channels */
void do_clantalk(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!is_clan(ch) || clan_table[ch->clan].independent) {
	send_to_char("You aren't in a clan.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	color(ch, "clantalk");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOCLAN)) {
	    send_to_char("Clan channel is now ON\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOCLAN);
	} else {
	    send_to_char("Clan channel is now OFF\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOCLAN);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("The gods have revoked your channel privileges.\n\r",
		     ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOCLAN);

    if (ch->level <= 1) {
	send_to_char
	    ("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
	     ch);
	return;
    }
    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    color(ch, "clantalk");	/* Customizable color */

    sprintf(buf, "You clan '%s'{x\n\r", argument);
    send_to_char(buf, ch);


    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    is_same_clan(ch, d->character) &&
	    !IS_SET(d->character->comm, COMM_NOCLAN) &&
	    !IS_SET(d->character->comm, COMM_QUIET)) {
	    color(d->character, "clantalk");	/* Customizable color */
	    act_new("$n clans '$t'{x", ch, argument, d->character, TO_VICT,
		    POS_DEAD);
	}
    }

    return;
}
void do_leader(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!IS_SET(ch->comm, PLR_DEL) || !is_clan(ch)) {
	send_to_char("WTF?\n\r", ch);
	return;
    }
    if (arg1[0] == '\0' || arg2[0] == '\0') {
	send_to_char("Syntax: leader + <char>\n\r", ch);
	send_to_char("Syntax: leader - <char>\n\r", ch);
	return;
    }
    if ((victim = get_char_world(ch, arg2)) == NULL) {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }
    if (!str_cmp(arg1, "+")) {
	if (!is_clan(victim) || victim->clan != ch->clan) {
	    send_to_char("They're not in your clan.\n\r", ch);
	    return;
	}
	if (IS_SET(victim->comm, PLR_LEADER)) {
	    send_to_char("They are already a leader.\n\r", ch);
	    return;
	}
	if (ch == victim) {
	    send_to_char("You can't leader yourself.\n\r", ch);
	    return;
	}
	if (!IS_SET(victim->comm, PLR_LEADER)) {
	    SET_BIT(victim->comm, PLR_LEADER);
	}
	sprintf(buf, "They are now a leader of the %s.\n\r",
		capitalize(clan_table[ch->clan].name));
	send_to_char(buf, ch);

	sprintf(buf, "You are now a leader of the %s.\n\r",
		capitalize(clan_table[ch->clan].name));
	send_to_char(buf, victim);

	victim->clan = ch->clan;
	return;
    }
    if (!str_cmp(arg1, "-")) {
	if (!is_clan(victim) || victim->clan != ch->clan) {
	    send_to_char("They aren't in your clan.\n\r", ch);
	    return;
	}
	if (IS_SET(victim->comm, PLR_LEADER))
	    REMOVE_BIT(victim->comm, PLR_LEADER);

	send_to_char("They are no longer a leader.\n\r", ch);
	send_to_char("You are no longer a leader.\n\r", victim);

	victim->clan = ch->clan;

	return;
    }
    send_to_char("Syntax: leader + <char>\n\r", ch);
    send_to_char("Syntax: leader - <char>\n\r", ch);

    return;
}
void do_member(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!IS_SET(ch->comm, PLR_LEADER) || !is_clan(ch)) {
	send_to_char("WTF?\n\r", ch);
	return;
    }
    if (arg1[0] == '\0' || arg2[0] == '\0') {
	send_to_char("Syntax: member + <char>\n\r", ch);
	send_to_char("Syntax: member - <char>\n\r", ch);
	return;
    }
    if ((victim = get_char_world(ch, arg2)) == NULL) {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }
    if (!IS_SET(ch->comm, PLR_DEL) && (IS_SET(victim->comm, PLR_LEADER))) {
	send_to_char("Only a delegate may remove a leader from power.\n\r",
		     ch);
	return;
    }
    if (!str_cmp(arg1, "+")) {
	if (is_clan(victim) && !clan_table[victim->clan].independent) {
	    send_to_char("They're already in a clan.\n\r", ch);
	    return;
	}
	sprintf(buf, "They are now a member of the %s.\n\r",
		capitalize(clan_table[ch->clan].name));
	send_to_char(buf, ch);

	sprintf(buf, "You are now a member of the %s.\n\r",
		capitalize(clan_table[ch->clan].name));
	send_to_char(buf, victim);

	if ((ch->clan == 3) || (ch->clan == 4) || (ch->clan == 7)
	    || (ch->clan == 10) || (ch->clan == 2) || (ch->clan == 9)
	    || (ch->clan == 13)) {
	    SET_BIT(victim->act, PLR_PKILL);
	}
	if (ch->clan == 11) {
	    REMOVE_BIT(victim->act, PLR_PKILL);
	}
	victim->clan = ch->clan;
	return;
    }
    if (!str_cmp(arg1, "-")) {
	if (!is_clan(victim) || victim->clan != ch->clan) {
	    send_to_char("They aren't in your clan.\n\r", ch);
	    return;
	}
	if (IS_SET(victim->comm, PLR_LEADER))
	    REMOVE_BIT(victim->comm, PLR_LEADER);

	if (IS_SET(victim->act, PLR_PKILL))
	    REMOVE_BIT(victim->act, PLR_PKILL);

	send_to_char("They are now clanless.\n\r", ch);
	send_to_char("You are now clanless.\n\r", victim);

	victim->clan = 0;

	return;
    }
    send_to_char("Syntax: member + <char>\n\r", ch);
    send_to_char("Syntax: member - <char>\n\r", ch);

    return;
}


void do_bounty(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    int value;

    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0') {

	send_to_char("\r\n{yBounties on current players:\r\n\r\n", ch);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *wch;

	    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
		continue;

	    wch = (d->original != NULL) ? d->original : d->character;

	    if (!can_see(ch, wch))
		continue;

	    sprintf(buf, "{g%-30s    {w%d\r\n", wch->name,
		    wch->pcdata->bounty);
	    send_to_char(buf, ch);

	}
	send_to_char
	    ("{x\r\nUse 'bounty <char> <amount>' to set a bounty.\r\n",
	     ch);
	return;
    }
    if (!IS_SET(ch->act, PLR_PKILL)) {
	send_to_char("Only killers may place bounties.\n\r", ch);
	return;
    }
    if ((victim = get_char_world(ch, arg1)) == NULL) {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You can't bounty yourself!\n\r", ch);
	return;
    }
    if (!IS_SET(victim->act, PLR_PKILL)) {
	send_to_char("Only killers may be bountied.\n\r", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("You can only place a bounty on players.\n\r", ch);
	return;
    }
    if (IS_IMMORTAL(victim)) {
	send_to_char("You can't place a bounty on an immortal!\n\r", ch);
	return;
    }
    if (arg2[0] == '\0') {
	if (victim->pcdata->bounty <= 0) {
	    sprintf(buf, "There is currently no bounty on %s.\n\r",
		    victim->name);
	} else {
	    sprintf(buf, "%s has placed a bounty of %d gold on %s.\n\r",
		    victim->pcdata->bountier, victim->pcdata->bounty,
		    victim->name);
	}
	send_to_char(buf, ch);
	return;
    }
    if (!IS_SET(victim->act, PLR_PKILL)) {
	send_to_char("You may only place a bounty on killers!\n\r", ch);
	return;
    }
    if (!is_number(arg2)) {
	send_to_char("The bounty must be a number.\n\r", ch);
	return;
    }
    value = atoi(arg2);

    if (value <= 1000 || value >= 10000000) {
	send_to_char("I don't think so.\n\r", ch);
	return;
    }
    if (value <= victim->pcdata->bounty) {
	send_to_char
	    ("Your bounty needs to be higher than the current one!\n\r",
	     ch);
	return;
    }
    sprintf(buf, "%s has put a bounty of %d gold on you!\n\r",
	    ch->name, value);
    send_to_char(buf, victim);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *wch;

	wch = d->original ? d->original : d->character;

	if (d->connected == CON_PLAYING
	    && d->character != ch
	    && d->character != victim && IS_SET(wch->comm, COMM_ANNOUNCE)
	    && !IS_SET(wch->comm, COMM_QUIET)) {
	    sprintf(buf,
		    "{c[{RBOUNTY{c] %s has put a bounty of {w%d {Ygold {con {R%s{c!{x",
		    ch->name, value, victim->name);
	    act_new(buf, ch, NULL, d->character, TO_VICT, POS_DEAD);
	}
    }

    sprintf(buf, "You put a bounty of %d gold on %s!\n\r",
	    value, victim->name);
    send_to_char(buf, ch);

    victim->pcdata->bounty = value;
    free_string(victim->pcdata->bountier);
    victim->pcdata->bountier = str_dup(ch->name);

    return;
}


void do_newbie(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (ch->level > 10 && !IS_IMMORTAL(ch)) {
	send_to_char("WTF?\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NONEWBIE)) {
	    send_to_char("{GNewbie channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NONEWBIE);
	} else {
	    send_to_char("{GNewbie channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NONEWBIE);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("The gods have revoked your channel privileges.\n\r",
		     ch);
	return;
    }
    /* Take it to the cussing channel */

    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    REMOVE_BIT(ch->comm, COMM_NONEWBIE);

    act_new("{G[{MNEWBIE{G] $n: $t{x", ch, argument, NULL, TO_CHAR,
	    POS_DEAD);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    (victim->level <= 10 || IS_IMMORTAL(victim)) &&
	    !IS_SET(victim->comm, COMM_NONEWBIE) &&
	    !IS_SET(victim->comm, COMM_QUIET)) {
	    act_new("{G[{MNEWBIE{G] $n: $t{x",
		    ch, argument, d->character, TO_VICT, POS_DEAD);
	}
    }
    return;
}


void do_immtalk(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOWIZ)) {
	    send_to_char("Immortal channel is now ON\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOWIZ);
	} else {
	    send_to_char("Immortal channel is now OFF\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOWIZ);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("The gods have revoked your channel privileges.\n\r",
		     ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOWIZ);

    sprintf(buf, "{c[{y$n{c]: %s{x", argument);
    act_new("{c[{y$n{c]: $t{x", ch, argument, NULL, TO_CHAR, POS_DEAD);

    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING &&
	    IS_IMMORTAL(d->character) &&
	    !IS_SET(d->character->comm, COMM_NOWIZ)) {
	    act_new("{c[{y$n{c]: $t{x", ch, argument, d->character,
		    TO_VICT, POS_DEAD);
	}
    }
    return;
}



void do_say(CHAR_DATA * ch, char *argument)
{
    color(ch, "say");		/* Customizable color */

    if (argument[0] == '\0') {
	send_to_char("Say what?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch)) {		/* Mobprogs */
	CHAR_DATA *mob, *mob_next;
	for (mob = ch->in_room->people; mob != NULL; mob = mob_next) {
	    mob_next = mob->next_in_room;
	    if (IS_NPC(mob) && HAS_TRIGGER(mob, TRIG_SPEECH)
		&& mob->position == mob->pIndexData->default_pos)
		mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
	}
    }
    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    act("$n says '$T'", ch, NULL, argument, TO_ROOM);
    act("You say '$T'", ch, NULL, argument, TO_CHAR);
    return;
}



void do_shout(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	color(ch, "shout");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_SHOUTSOFF)) {
	    send_to_char("You can hear shouts again.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_SHOUTSOFF);
	} else {
	    send_to_char("You will no longer hear shouts.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_SHOUTSOFF);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_NOSHOUT)) {
	send_to_char("You can't shout.\n\r", ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_SHOUTSOFF);

    if (ch->level <= 1) {
	send_to_char
	    ("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
	     ch);
	return;
    }
    WAIT_STATE(ch, 12);

    /* Take it to the cussing channel */

    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    color(ch, "shout");		/* Customizable color */

    act("You shout '$T'{x", ch, NULL, argument, TO_CHAR);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
	    !IS_SET(victim->comm, COMM_QUIET)) {
	    color(d->character, "shout");	/* Customizable color */
	    act("$n shouts '$t'{x", ch, argument, d->character, TO_VICT);
	}
    }

    return;
}



void do_tell(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm, COMM_DEAF)) {
	color(ch, "tell");	/* Customizable color */
	send_to_char("Your message didn't get through.{x\n\r", ch);
	return;
    }

    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }

    if (IS_SET(ch->comm, COMM_DEAF)) {
	send_to_char("You must turn off deaf mode first.\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
	color(ch, "tell");	/* Customizable color */
	send_to_char("Tell whom what?{x\n\r", ch);
	return;
    }
    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ((victim = get_char_world(ch, arg)) == NULL
	|| (IS_NPC(victim) && victim->in_room != ch->in_room)) {
	color(ch, "tell");	/* Customizable color */
	send_to_char("They aren't here.{x\n\r", ch);
	return;
    }

    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    if (IS_SET(ch->comm, COMM_AFK))
	do_afk(ch, "");

    if (victim->desc == NULL && !IS_NPC(victim)) {
	color(ch, "tell");	/* Customizable color */
	act_new("$N seems to have misplaced $S link...try again later.{x",
		ch, NULL, victim, TO_CHAR, POS_DEAD);
	sprintf(buf, "%s tells you '%s'{x\n\r", PERS(ch, victim),
		argument);
	buf[0] = UPPER(buf[0]);
    if (victim->pcdata->buffer->size < 10000) {
        add_buf(victim->pcdata->buffer, buf); 
    }   

	return;
    }

    if (
	(IS_SET(victim->comm, COMM_QUIET)
	 || IS_SET(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
	color(ch, "tell");	/* Customizable color */
	act("$E is not receiving tells.{x", ch, 0, victim, TO_CHAR);
	return;
    }
    if (IS_SET(victim->comm, COMM_AFK)) {
	if (IS_NPC(victim)) {
	    color(ch, "tell");	/* Customizable color */
	    act_new("$E is AFK, and not receiving tells.{x", ch, NULL,
		    victim, TO_CHAR, POS_DEAD);
	    return;
	}

	color(ch, "tell");	/* Customizable color */
	act_new
	    ("$E is AFK, but your tell will go through when $E returns.{x",
	     ch, NULL, victim, TO_CHAR, POS_DEAD);
	sprintf(buf, "%s tells you '%s'{x\n\r", PERS(ch, victim),
		argument);
	buf[0] = UPPER(buf[0]);

	if (victim->pcdata->buffer->size < 10000) {
		add_buf(victim->pcdata->buffer, buf);
	}

	return;
    }
    color(ch, "tell");		/* Customizable color */

    /* Ignore Code */
    if (IS_NPC(victim)) {
	send_to_char
	    ("Maybe talking to yourself would be more productive.\n\r",
	     ch);
	return;
    } else {
	if (is_ignoring(victim, ch)) {
	    sprintf(buf, "%s is ignoring you.\n\r", victim->name);
	    send_to_char(buf, ch);
	    return;
	}
    }

    act_new("You tell $N '$t'{x", ch, argument, victim, TO_CHAR, POS_DEAD);

    color(victim, "tell");	/* Customizable color */

    act_new("$n tells you '$t'{x", ch, argument, victim, TO_VICT,
	    POS_DEAD);

    victim->reply = ch;

    if (ch->reply == NULL) {
	ch->reply = victim;
    }
    return;
}



void do_reply(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if (IS_SET(ch->comm, COMM_NOTELL)) {
	color(ch, "tell");	/* Customizable color */
	send_to_char("Your message didn't get through.{x\n\r", ch);
	return;
    }
    if ((victim = ch->reply) == NULL) {
	color(ch, "tell");	/* Customizable color */
	send_to_char("They aren't here.{x\n\r", ch);
	return;
    }
    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    if (IS_SET(ch->comm, COMM_AFK))
	do_afk(ch, "");

    if (victim->desc == NULL && !IS_NPC(victim)) {
	color(ch, "tell");	/* Customizable color */
	act_new("$N seems to have misplaced $S link...try again later.{x",
		ch, NULL, victim, TO_CHAR, POS_DEAD);
	sprintf(buf, "%s tells you '%s'{x\n\r", PERS(ch, victim),
		argument);
	buf[0] = UPPER(buf[0]);

	return;
    }
    if (
	(IS_SET(victim->comm, COMM_QUIET)
	 || IS_SET(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)
	&& !IS_IMMORTAL(victim)) {
	color(ch, "tell");	/* Customizable color */
	act_new("$E is not receiving tells.{x", ch, 0, victim, TO_CHAR,
		POS_DEAD);
	return;
    }
    if (IS_SET(victim->comm, COMM_AFK)) {
	if (IS_NPC(victim)) {
	    color(ch, "tell");	/* Customizable color */
	    act_new("$E is AFK, and not receiving tells.{x",
		    ch, NULL, victim, TO_CHAR, POS_DEAD);
	    return;
	}
	color(ch, "tell");	/* Customizable color */
	act_new
	    ("$E is AFK, but your tell will go through when $E returns.{x",
	     ch, NULL, victim, TO_CHAR, POS_DEAD);
	sprintf(buf, "%s tells you '%s'{x\n\r", PERS(ch, victim),
		argument);
	buf[0] = UPPER(buf[0]);
    if (victim->pcdata->buffer->size < 10000) {
        add_buf(victim->pcdata->buffer, buf); 
    }   

	return;
    }
    color(ch, "tell");		/* Customizable color */

    act_new("You tell $N '$t'{x", ch, argument, victim, TO_CHAR, POS_DEAD);

    color(victim, "tell");	/* Customizable color */

    act_new("$n tells you '$t'{x", ch, argument, victim, TO_VICT,
	    POS_DEAD);
    victim->reply = ch;

    return;
}


void do_pray(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    if (argument[0] == '\0') {
	color(ch, "pray");	/* Customizable color */
	if (IS_SET(ch->comm, COMM_NOPRAY)) {
	    send_to_char("Pray channel is now ON.{x\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOPRAY);
	} else {
	    send_to_char("Pray channel is now OFF.{x\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOPRAY);
	}
	return;
    }
    argument = one_argument(argument, arg);

    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	color(ch, "pray");	/* Customizable color */
	send_to_char("You are forbidden from praying.{x\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (arg[0] == '\0' || argument[0] == '\0') {
	color(ch, "pray");	/* Customizable color */
	send_to_char("Implore what, to whom?{x\n\r", ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOPRAY);

    if (!str_prefix(arg, "all")) {
	/* Take it to the cussing channel */

	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	color(ch, "pray");	/* Customizable color */

	act_new("You implore all '$t'{x", ch, argument, NULL, TO_CHAR,
		POS_DEAD);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    if (d->connected == CON_PLAYING
		&& d->character != ch
		&& !IS_SET(d->character->comm, COMM_QUIET)
		&& !IS_SET(d->character->comm, COMM_NOPRAY)) {
		color(d->character, "pray");	/* Customizable color */
		if (d->character->level >= LEVEL_IMMORTAL) {
		    act_new("$n implores all '$t'{x", ch, argument,
			    d->character, TO_VICT, POS_SLEEPING);
		} else {
		    act_new("$n cries out to the heavens.{x", ch, NULL,
			    d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}

    } else {
	if ((victim = get_char_world(ch, arg)) == NULL) {
	    color(ch, "pray");	/* Customizable color */
	    send_to_char("They aren't here.{x\n\r", ch);
	    return;
	}
	if (!IS_IMMORTAL(victim) || victim->level < LEVEL_IMMORTAL
	    || IS_NPC(victim)) {
	    color(ch, "pray");	/* Customizable color */
	    send_to_char("What the!? They're not a God!{x\n\r", ch);
	    return;
	}
	if (IS_SET(victim->comm, COMM_QUIET)
	    || IS_SET(victim->comm, COMM_NOPRAY)) {
	    color(ch, "pray");	/* Customizable color */
	    act("$E is not recieving prayers.{x", ch, NULL, victim,
		TO_CHAR);
	    return;
	}
	/* Make the words drunk if needed... */
	argument = makedrunk(argument, ch);

	sprintf(buf, "$n invokes the mighty powers of %s.{x",
		victim->name);

	for (d = descriptor_list; d != NULL; d = d->next) {
	    if (d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character != victim
		&& !IS_SET(d->character->comm, COMM_QUIET)
		&& !IS_SET(d->character->comm, COMM_NOPRAY)) {
		color(d->character, "pray");	/* Customizable color */
		act_new(buf, ch, NULL, d->character, TO_VICT,
			POS_SLEEPING);
	    }
	}

	color(ch, "pray");	/* Customizable color */
	act_new("You implore $N '$t'{x", ch, argument, victim, TO_CHAR,
		POS_DEAD);
	color(victim, "pray");	/* Customizable color */
	act_new("$n implores you '$t'{x", ch, argument, victim, TO_VICT,
		POS_DEAD);

    }
    return;
}


void do_yell(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (IS_SET(ch->comm, COMM_NOSHOUT)) {
	color(ch, "yell");	/* Customizable color */
	send_to_char("You can't yell.{x\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	color(ch, "yell");	/* Customizable color */
	send_to_char("Yell what?{x\n\r", ch);
	return;
    }
    if (ch->level <= 1) {
	send_to_char
	    ("\r\nSorry, you must be level two to use any channel other than 'newbie'.\r\n",
	     ch);
	return;
    }
    /* Take it to the cussing channel */

    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    color(ch, "yell");		/* Customizable color */

    act("You yell '$t'{x", ch, argument, NULL, TO_CHAR);

    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING
	    && d->character != ch
	    && d->character->in_room != NULL
	    && d->character->in_room->area == ch->in_room->area
	    && !IS_SET(d->character->comm, COMM_QUIET)) {
	    color(d->character, "yell");	/* Customizable color */
	    act("$n yells '$t'{x", ch, argument, d->character, TO_VICT);
	}
    }

    return;
}


void do_emote(CHAR_DATA * ch, char *argument)
{

    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE)) {
	send_to_char("You can't show your emotions.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Emote what?\n\r", ch);
	return;
    }
    MOBtrigger = FALSE;
    act("$n $T", ch, NULL, argument, TO_ROOM);
    act("$n $T", ch, NULL, argument, TO_CHAR);
    MOBtrigger = TRUE;
    return;
}


void do_pmote(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE)) {
	send_to_char("You can't show your emotions.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Emote what?\n\r", ch);
	return;
    }
    act("$n $t", ch, argument, NULL, TO_CHAR);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
	if (vch->desc == NULL || vch == ch)
	    continue;

	if ((letter = strstr(argument, vch->name)) == NULL) {
	    MOBtrigger = FALSE;
	    act("$N $t", vch, argument, ch, TO_CHAR);
	    MOBtrigger = TRUE;
	    continue;
	}
	strcpy(temp, argument);
	temp[strlen(argument) - strlen(letter)] = '\0';
	last[0] = '\0';
	name = vch->name;

	for (; *letter != '\0'; letter++) {
	    if (*letter == '\'' && matches == strlen(vch->name)) {
		strcat(temp, "r");
		continue;
	    }
	    if (*letter == 's' && matches == strlen(vch->name)) {
		matches = 0;
		continue;
	    }
	    if (matches == strlen(vch->name)) {
		matches = 0;
	    }
	    if (*letter == *name) {
		matches++;
		name++;
		if (matches == strlen(vch->name)) {
		    strcat(temp, "you");
		    last[0] = '\0';
		    name = vch->name;
		    continue;
		}
		strncat(last, letter, 1);
		continue;
	    }
	    matches = 0;
	    strcat(temp, last);
	    strncat(temp, letter, 1);
	    last[0] = '\0';
	    name = vch->name;
	}

	MOBtrigger = FALSE;
	act("$N $t", vch, temp, ch, TO_CHAR);
	MOBtrigger = TRUE;
    }

    return;
}


/*
 * All the posing stuff.
 */
struct pose_table_type {
    char *message[2 * MAX_CLASS];
};

const struct pose_table_type pose_table[] = {
    {
     {
      "You sizzle with energy.",
      "$n sizzles with energy.",
      "You feel very holy.",
      "$n looks very holy.",
      "You perform a small card trick.",
      "$n performs a small card trick.",
      "You show your bulging muscles.",
      "$n shows $s bulging muscles.",
      "You attempt to pick your nose with your lockpick.",
      "$n attempts to pick $s nose with $s lockpick.",
      "Y'golan encompasses you in a golden nimbus of light.",
      "Y'golan encompasses $n in a golden nimbus of light.",
      "You ride off into the sunset playing on your mandolin.",
      "$n plays on an old mandolin while riding off into the sunset."}
     },

    {
     {
      "You turn into a butterfly, then return to your normal shape.",
      "$n turns into a butterfly, then returns to $s normal shape.",
      "You nonchalantly turn wine into water.",
      "$n nonchalantly turns wine into water.",
      "You wiggle your ears alternately.",
      "$n wiggles $s ears alternately.",
      "You crack nuts between your fingers.",
      "$n cracks nuts between $s fingers.",
      "You deftly grab a fly out of mid-air.",
      "$n deftly grabs a fly out of mid-air.",
      "You use the grass to wipe demon's blood off your weapon.",
      "$n uses grass to wipe demon's blood off $s weapon.",
      "A slightly deranged snail offers you a lift.",
      "A slightly 'disturbed' looking snail offers $n a lift."}
     },

    {
     {
      "Blue sparks fly from your fingers.",
      "Blue sparks fly from $n's fingers.",
      "A halo appears over your head.",
      "A halo appears over $n's head.",
      "You nimbly tie yourself into a knot.",
      "$n nimbly ties $mself into a knot.",
      "You grizzle your teeth and look mean.",
      "$n grizzles $s teeth and looks mean.",
      "You assassinate a harmless flea and take the head for a bounty!",
      "$n assassinates a harmless flea and takes the head for a bounty!",
      "You offer light to this poor darken world.",
      "$n offers light to this poor darkened world.",
      "A large brown troll saddles you up and shouts, 'GIDDYAP!'",
      "A large brown troll saddles $n up and shouts, 'GIDDYAP!'"}
     },

    {
     {
      "Little red lights dance in your eyes.",
      "Little red lights dance in $n's eyes.",
      "You recite words of wisdom.",
      "$n recites words of wisdom.",
      "You juggle with daggers, apples, and eyeballs.",
      "$n juggles with daggers, apples, and eyeballs.",
      "You hit your head, and your eyes roll.",
      "$n hits $s head, and $s eyes roll.",
      "You offer poisoned beer to everyone in the room.",
      "$n offers beer to everyone present.  It smells a little funny...",
      "Your prayers have converted Grbrthrxs from his evil ways.",
      "The prayers of $n have converted Grbrthrxs from his evil ways.",
      "Elven Warhorses answer your call to battle.",
      "$n screams an ancient battlecry and Elven Warhorses arrive immediately.",
      }
     },

    {
     {
      "A slimy green monster appears before you and bows.",
      "A slimy green monster appears before $n and bows.",
      "Deep in prayer, you levitate.",
      "Deep in prayer, $n levitates.",
      "You steal the underwear off every person in the room.",
      "Your underwear is gone!  $n stole it!",
      "Crunch, crunch -- you munch a bottle.",
      "Crunch, crunch -- $n munches a bottle.",
      "You string up the hangman with his own noose.",
      "$n strings up the hangman with his own noose.",
      "A great Silver Dragon offers you a ride.",
      "A great Silver Dragon offers $n a ride.",
      "The King begs you to guide his chariot.",
      "The King begs $n to guide his chariot."}
     },

    {
     {
      "You turn everybody into a little pink elephant.",
      "You are turned into a little pink elephant by $n.",
      "An angel consults you.",
      "An angel consults $n.",
      "The dice roll ... and you win again.",
      "The dice roll ... and $n wins again.",
      "... 98, 99, 100 ... you do pushups.",
      "... 98, 99, 100 ... $n does pushups.",
      "The shadows slide apart and you emerge.",
      "The shadows slide apart and $n emerges.",
      "Brilliant colors explode in front of your eyes!",
      "$n utters a word in error and explodes violently!",
      "With a sweep of your weapon, you knock down everyone in the room.",
      "$n sweeps with $s weapon, and you find yourself on the ground."}
     },

    {
     {
      "A small ball of light dances on your fingertips.",
      "A small ball of light dances on $n's fingertips.",
      "Your body glows with an unearthly light.",
      "$n's body glows with an unearthly light.",
      "You count the money in everyone's pockets.",
      "Check your money, $n is counting it.",
      "The gods admire your physique.",
      "The gods admire $n's physique.",
      "You appear from nowhere.",
      "$n appears mysteriously from the gleam of your eye.",
      "You utter a few words and the earth glistens with new green life.",
      "$n utters a few words and the earth listens with new green life.",
      "Y'golan offers you the reins of his steed.",
      "Y'golan offers $n the reins of his steed."}
     },

    {
     {
      "Smoke and fumes leak from your nostrils.",
      "Smoke and fumes leak from $n's nostrils.",
      "A spot light hits you.",
      "A spot light hits $n.",
      "You balance a pocket knife on your tongue.",
      "$n balances a pocket knife on your tongue.",
      "Watch your feet, you are juggling granite boulders.",
      "Watch your feet, $n is juggling granite boulders.",
      "You bare your dagger.",
      "A silver-hilted dagger appears in mid-air.",
      "Balls of light dance in front of you.",
      "Tiny suns rotate endlessly in front of $n's face.",
      "A fiery stallion kneels for you to mount.",
      "A fiery stallion kneels so that $n may mount."}
     },

    {
     {
      "The light flickers as you rap in magical languages.",
      "The light flickers as $n raps in magical languages.",
      "Everyone levitates as you pray.",
      "You levitate as $n prays.",
      "You produce a coin from everyone's ear.",
      "$n produces a coin from your ear.",
      "Oomph!  You squeeze water out of a granite boulder.",
      "Oomph!  $n squeezes water out of a granite boulder.",
      "Rulers flee from your presence.",
      "Rulers flee from $n's presence.",
      "Time collapses as you blink.",
      "Time collapses in a blink of $n's eye.",
      "You mount the winds and ride into infinity.",
      "$n mounts the winds and rides into infinity."}
     },

    {
     {
      "Your head disappears.",
      "$n's head disappears.",
      "A cool breeze refreshes you.",
      "A cool breeze refreshes $n.",
      "You step behind your shadow.",
      "$n steps behind $s shadow.",
      "You pick your teeth with a spear.",
      "$n picks $s teeth with a spear.",
      "You flick your wrist and a throwing star appears from nowhere.",
      "$n flicks $s wrist and a throwing star appears magically.",
      "A demon host bows in submission to your holy might.",
      "A demon host bows in submission to $n's holy might.",
      "Clods of dirt fly as you thunder across the land.",
      "Clods of dirt fly as $n thunders by."}
     },

    {
     {
      "A fire elemental singes your hair.",
      "A fire elemental singes $n's hair.",
      "The sun pierces through the clouds to illuminate you.",
      "The sun pierces through the clouds to illuminate $n.",
      "Your eyes dance with greed.",
      "$n's eyes dance with greed.",
      "Everyone is swept off their foot by your hug.",
      "You are swept off your feet by $n's hug.",
      "A change purse appears in your hand.",
      "Your change purse disappears.",
      "A flash of calm envelops the room as you enter.",
      "The room feels suddenly calm as $n enters.",
      "You slip comfortably into the saddle.",
      "$n slips comfortably into the saddle."}
     },

    {
     {
      "The sky changes color to match your eyes.",
      "The sky changes color to match $n's eyes.",
      "The ocean parts before you.",
      "The ocean parts before $n.",
      "You deftly steal everyone's weapon.",
      "$n deftly steals your weapon.",
      "Your karate chop splits a tree.",
      "$n's karate chop splits a tree.",
      "You drop some coins.",
      "$n drops some coins and you find a dagger at your back as you bend to retrieve them.",
      "Your body crackles with holy light.",
      "$n's armor crackles with a holy light.",
      "Your mount nuzzles your shoulder softly.",
      "$n's mount nuzzles $s shoulder softly."}
     },

    {
     {
      "The stones dance to your command.",
      "The stones dance to $n's command.",
      "A thunder cloud kneels to you.",
      "A thunder cloud kneels to $n.",
      "The Grey Mouser buys you a beer.",
      "The Grey Mouser buys $n a beer.",
      "A strap of your armor breaks over your mighty thews.",
      "A strap of $n's armor breaks over $s mighty thews.",
      "You are but a face in the crowd.",
      "$n melts into the crowd.",
      "The oracle asks for your advice.",
      "The oracle asks for $n's advice.",
      "You ride off to victory.",
      "$n rides off to victory."}
     },

    {
     {
      "The heavens and grass change colour as you smile.",
      "The heavens and grass change colour as $n smiles.",
      "The Burning Man speaks to you.",
      "The Burning Man speaks to $n.",
      "Everyone's pocket explodes with your fireworks.",
      "Your pocket explodes with $n's fireworks.",
      "A boulder cracks at your frown.",
      "A boulder cracks at $n's frown.",
      "The Angel of Death asks you for a few pointers.",
      "The Angel of Death gets a few pointers from $n.",
      "A multitude of angels sing your praises for all the world to hear.",
      "A multitude of angels sing $n's praises for all the world to hear.",
      "You command a legendary Daemon Steed.",
      "An ill wind blows as a Daemon Steed approaches $n and offers its services."}
     },

    {
     {
      "Everyone's clothes are transparent, and you are laughing.",
      "Your clothes are transparent, and $n is laughing.",
      "An eye in a pyramid winks at you.",
      "An eye in a pyramid winks at $n.",
      "Everyone discovers your dagger a centimeter from their eye.",
      "You discover $n's dagger a centimeter from your eye.",
      "Mercenaries arrive to do your bidding.",
      "Mercenaries arrive to do $n's bidding.",
      "You post a 'Murder for Hire' sign.",
      "$n puts out $s 'Murder for Hire' sign.",
      "You burn the bridge behind you, leaving no retreat.",
      "$n burns $s bridges, leaving only the path to victory.",
      "The echoes of thundering hooves fill the room.",
      "As you notice $n, the echoes of thundering hooves sound in your mind."}
     },

    {
     {
      "A black hole swallows you.",
      "A black hole swallows $n.",
      "An archangel offers you a glass of water.",
      "An archangel offers $n a glass of water.",
      "Where did you go?",
      "Where did $n go?",
      "Millions cringe at the thought of your blows.",
      "Millions cringe at the thought of $n's might.",
      "You emerge from a dark alley.",
      "$n emerges from the dark alley of your mind.",
      "Your holy aura drives stakes into the heart of evil.",
      "$n's holy aura drives stakes into the heart of evil.",
      "Four matched Percherons bring in your chariot.",
      "Four matched Percherons bring in $n's chariot."}
     },

    {
     {
      "The world shimmers in time with your whistling.",
      "The world shimmers in time with $n's whistling.",
      "The great god Y'golan gives you a staff.",
      "The great god Y'golan gives $n a staff.",
      "Click.",
      "Click.",
      "Atlas asks you to relieve him.",
      "Atlas asks $n to relieve him.",
      "The Angel of Death asks you for a few pointers.",
      "The Angel of Death gets a few pointers from $n.",
      "The Allmighty offers you a seat at his right hand.",
      "$n is offered a seat at the right hand of the Allmighty.",
      "You ride a horse called Death.",
      "$n rides a horse called Death."}
     }
};



void do_pose(CHAR_DATA * ch, char *argument)
{
    int level;
    int pose;

    if (IS_NPC(ch))
	return;

    level =
	UMIN(ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1);
    pose = number_range(0, level);

    act(pose_table[pose].message[2 * ch->class + 0], ch, NULL, NULL,
	TO_CHAR);
    act(pose_table[pose].message[2 * ch->class + 1], ch, NULL, NULL,
	TO_ROOM);

    return;
}


void do_bug(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    append_file(ch, BUG_FILE, argument);
    act_new("{*{RYou logged this BUG: {r$t{x", ch, argument, NULL, TO_CHAR,
	    POS_DEAD);

    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING && IS_IMMORTAL(d->character)) {
	    act_new("{*{R$n logged this BUG: {r$t{x", ch, argument,
		    d->character, TO_VICT, POS_DEAD);
	}
    }

    WAIT_STATE(ch, 24);

    return;
}

void do_typo(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    append_file(ch, TYPO_FILE, argument);
    act_new("{*{BYou logged this TYPO: {b$t{x", ch, argument, NULL,
	    TO_CHAR, POS_DEAD);

    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING && IS_IMMORTAL(d->character)) {
	    act_new("{*{B$n logged this TYPO: {b$t{x", ch, argument,
		    d->character, TO_VICT, POS_DEAD);
	}
    }

    WAIT_STATE(ch, 24);

    return;
}

void do_rent(CHAR_DATA * ch, char *argument)
{
    send_to_char("There is no rent here.  Just save and quit.\n\r", ch);
    return;
}

void do_qui(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to {BQUIT{x, you have to spell it out.\n\r",
		 ch);
    return;
}



void do_quit(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d, *d_next;
    int id;

    if (IS_NPC(ch))
	return;


    if (ch->position == POS_FIGHTING) {
	send_to_char("No way! You are fighting.\n\r", ch);
	return;
    }
    if (ch->position <= POS_STUNNED) {
	send_to_char("You're not {RDEAD{x yet.\n\r", ch);
	return;
    }

	arena_death(ch);
    send_to_char("May the Moons bless your travels.\n\r", ch);
    act("$n slowly fades away.", ch, NULL, NULL, TO_ROOM);

    /* For the Announce Channel */

    announce("{c[{RANNOUNCE{c] $n has fled from this world.{x", ch);

    sprintf(log_buf, "%s has quit.", ch->name);
    log_string(log_buf);
    wiznet("$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
	   get_trust(ch));

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj(ch);
    id = ch->id;
    d = ch->desc;
    extract_char(ch, TRUE);
    if (d != NULL)
	close_socket(d);

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next) {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if (tch && tch->id == id) {
	    extract_char(tch, TRUE);
	    close_socket(d);
	}
    }

    return;
}



void do_save(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    save_char_obj(ch);
    send_to_char
	("Saving. Remember that the Alanthia has automatic saving now.\n\r",
	 ch);
    return;
}



void do_follow(CHAR_DATA * ch, char *argument)
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Follow whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL) {
	act("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
	return;
    }

    if (victim == ch) {
	if (ch->master == NULL) {
	    send_to_char("You already follow yourself.\n\r", ch);
	    return;
	}
	stop_follower(ch);
	return;
    }
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOFOLLOW)
	&& !IS_IMMORTAL(ch)) {
	act("$N doesn't seem to want any followers.\n\r", ch, NULL, victim,
	    TO_CHAR);
	return;
    }
    REMOVE_BIT(ch->act, PLR_NOFOLLOW);

    if (ch->master != NULL)
	stop_follower(ch);

    add_follower(ch, victim);
    return;
}


void add_follower(CHAR_DATA * ch, CHAR_DATA * master)
{
    if (ch->master != NULL) {
	bug("Add_follower: non-null master.", 0);
	return;
    }
    ch->master = master;
    ch->leader = NULL;

    if (can_see(master, ch))
	act("$n now follows you.", ch, NULL, master, TO_VICT);

    act("You now follow $N.", ch, NULL, master, TO_CHAR);

    return;
}



void stop_follower(CHAR_DATA * ch)
{
    if (ch->master == NULL) {
	bug("Stop_follower: null master.", 0);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM)) {
	REMOVE_BIT(ch->affected_by, AFF_CHARM);
	REMOVE_BIT(ch->act, ACT_PET);
	affect_strip(ch, gsn_charm_person);
    }
    if (can_see(ch->master, ch) && ch->in_room != NULL) {
	act("$n stops following you.", ch, NULL, ch->master, TO_VICT);
	act("You stop following $N.", ch, NULL, ch->master, TO_CHAR);
    }
    if (ch->master->pet == ch)
	ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    if (IS_NPC(ch)) {
	ch->max_hit = ch->max_hit * 2;
	ch->hitroll = ch->hitroll * 2;
	ch->damroll = ch->damroll * 2;
	ch->hit = ch->max_hit;
	stop_fighting(ch, TRUE);
    }
    return;

}

/* nukes charmed monsters and pets */
void nuke_pets(CHAR_DATA * ch)
{
    CHAR_DATA *pet;

    if ((pet = ch->pet) != NULL) {
	stop_follower(pet);
    }
    ch->pet = NULL;

    return;
}



void die_follower(CHAR_DATA * ch)
{
    CHAR_DATA *fch;

    if (ch->master != NULL) {
	if (ch->master->pet == ch)
	    ch->master->pet = NULL;
	stop_follower(ch);
    }
    ch->leader = NULL;

    for (fch = char_list; fch != NULL; fch = fch->next) {
	if (fch->master == ch)
	    stop_follower(fch);
	if (fch->leader == ch)
	    fch->leader = fch;
    }

    return;
}



void do_order(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument(argument, arg);
    one_argument(argument, arg2);

    if (!str_cmp(arg2, "delete") || !str_cmp(arg2, "circle")
	|| !str_cmp(arg2, "circl") || !str_cmp(arg2, "circ")
	|| !str_cmp(arg2, "ci") || !str_cmp(arg2, "c")
	|| !str_cmp(arg2, "cir") || !str_cmp(arg2, "whirl")
	|| !str_cmp(arg2, "whir") || !str_cmp(arg2, "whi")
	|| !str_cmp(arg2, "wh") || !str_cmp(arg2, "w")
	|| !str_cmp(arg2, "steal") || !str_cmp(arg2, "stea")
	|| !str_cmp(arg2, "ste") || !str_cmp(arg2, "ste")
	|| !str_cmp(arg2, "st") || !str_cmp(arg2, "s")
	|| !str_cmp(arg2, "mob") || !str_cmp(arg2, "rescue")) {
//      char_from_room(ch);
//      char_to_room(ch, get_room_index (ROOM_VNUM_JAIL));
	send_to_char("You didn't say the magic word!.\n\r", ch);
//      send_to_char("Maybe a nice imm will take you out of here",ch);
	return;
    }

    if (!str_cmp(arg2, "kill") || !str_cmp(arg2, "kil")
	|| !str_cmp(arg2, "ki") || !str_cmp(arg2, "k")) {
//              char_from_room(ch);
//              char_to_room(ch, get_room_index (ROOM_VNUM_JAIL));
	send_to_char("Do it yourself will you", ch);
//              send_to_char("Maybe a nice imm will take you out of here",ch);
	return;
    }

    if (!str_cmp(arg2, "rescu") || !str_cmp(arg2, "resc")
	|| !str_cmp(arg2, "res") || !str_cmp(arg2, "re")
	|| !str_cmp(arg2, "r")) {
//              char_from_room(ch);
//              char_to_room(ch, get_room_index (ROOM_VNUM_JAIL));
	send_to_char("Now what have people told you?", ch);
//              send_to_char("Maybe a nice imm will take you out of here",ch);
	return;
    }

    if (!str_cmp(arg2, "murder") || !str_cmp(arg2, "murde")
	|| !str_cmp(arg2, "murd") || !str_cmp(arg2, "mur")
	|| !str_cmp(arg2, "mu") || !str_cmp(arg2, "m")) {
	send_to_char("Yeah right.", ch);
	return;
    }
    if (arg[0] == '\0' || argument[0] == '\0') {
	send_to_char("Order whom to do what?\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM)) {
	send_to_char("You feel like taking, not giving, orders.\n\r", ch);
	return;
    }
    if (!str_cmp(arg, "all")) {
	fAll = TRUE;
	victim = NULL;
    } else {
	fAll = FALSE;
	if ((victim = get_char_room(ch, arg)) == NULL) {
	    send_to_char("They aren't here.\n\r", ch);
	    return;
	}
	if (victim == ch) {
	    send_to_char("Aye aye, right away!\n\r", ch);
	    return;
	}
/*
	if (!IS_AFFECTED(victim, AFF_CHARM) 
	//	|| victim->master != ch
	    || (IS_IMMORTAL(victim) && victim->trust >= ch->trust)) {
	    send_to_char("Do it yourself!\n\r", ch);
	    return;
	}
*/
	if (IS_AFFECTED(victim, AFF_CHARM)
	    && (victim->master == ch)
	    && (ch->pet != victim)
	    && (victim->fighting != NULL)
	    && (!str_cmp(arg2, "recall") || !str_cmp(arg2, "flee"))) {
	    send_to_char
		("Your charmie is overcome with bloodlust, and will not stop fighting!\n\r",
		 ch);
	    return;
	}
    }


    found = FALSE;
    for (och = ch->in_room->people; och != NULL; och = och_next) {
	och_next = och->next_in_room;

	if (IS_AFFECTED(och, AFF_CHARM)
	    && och->master == ch && (fAll || och == victim)) {
	    found = TRUE;
	    sprintf(buf, "$n orders you to '%s'.", argument);
	    act(buf, ch, NULL, och, TO_VICT);
	    sprintf(buf, "$n gives whispered orders.");
	    act(buf, ch, NULL, NULL, TO_ROOM);
	    interpret(och, argument);
	}
    }

    if (found) {
	WAIT_STATE(ch, 12);
	send_to_char("Ok.\n\r", ch);
    } else
	send_to_char("You have no followers here.\n\r", ch);
    return;
}



void do_group(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf(buf, "{m%s's group:{x\n\r", PERS(leader, ch));
	send_to_char(buf, ch);

	for (gch = char_list; gch != NULL; gch = gch->next) {
	    if (is_same_group(gch, ch)) {
		sprintf(buf,
			"{m[%2d %s] %-16s %4ld/%4ldhp %4ld/%4ldmana %4ld/%4ldmv{x\n\r",
			gch->level,
			IS_NPC(gch) ? "Mob" : class_table[gch->class].
			who_name, capitalize(PERS(gch, ch)), gch->hit,
			gch->max_hit, gch->mana, gch->max_mana, gch->move,
			gch->max_move);
		send_to_char(buf, ch);
	    }
	}
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("{mThey aren't here.{x\n\r", ch);
	return;
    }
    if (IS_FIGHTING(ch)) {
	send_to_char("You're much too busy to be giving orders.\n\r", ch);
	return;
    }
    if (IS_FIGHTING(victim)) {
	send_to_char
	    ("They don't have time to join your party right now.\n\r", ch);
	return;
    }
    if (ch->master != NULL || (ch->leader != NULL && ch->leader != ch)) {
	send_to_char("{mBut you are following someone else!{x\n\r", ch);
	return;
    }
    if (victim->master != ch && ch != victim) {
	act_new("{m$N isn't following you.{x", ch, NULL, victim, TO_CHAR,
		POS_SLEEPING);
	return;
    }
/*
   if ( victim->level > (ch->level + 6) || (victim->level < (ch->level - 10) )
   {
   act_new("{mThere is too great of a difference in levels.{x",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
   return;
   }
 */

    if (IS_AFFECTED(victim, AFF_CHARM)) {
	send_to_char
	    ("{mYou can't remove charmed mobs from your group.{x\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM)) {
	act_new("{mYou like your master too much to leave $m!{x",
		ch, NULL, victim, TO_VICT, POS_SLEEPING);
	return;
    }
    if (is_same_group(victim, ch) && ch != victim) {
	victim->leader = NULL;
	act_new("{m$n removes $N from $s group.{x",
		ch, NULL, victim, TO_NOTVICT, POS_RESTING);
	act_new("{m$n removes you from $s group.{x",
		ch, NULL, victim, TO_VICT, POS_SLEEPING);
	act_new("{mYou remove $N from your group.{x",
		ch, NULL, victim, TO_CHAR, POS_SLEEPING);
	return;
    }
    victim->leader = ch;
    act_new("{m$N joins $n's group.{x", ch, NULL, victim, TO_NOTVICT,
	    POS_RESTING);
    act_new("{mYou join $n's group.{x", ch, NULL, victim, TO_VICT,
	    POS_SLEEPING);
    act_new("{m$N joins your group.{x", ch, NULL, victim, TO_CHAR,
	    POS_SLEEPING);
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_gold = 0;
    int share_gold;
    int extra_gold;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Split how much?\n\r", ch);
	return;
    }
    amount_gold = atoi(arg);

    if (amount_gold < 0) {
	send_to_char("Your group wouldn't like that.\n\r", ch);
	return;
    }
    if (amount_gold == 0) {
	send_to_char("You hand out zero coins, but no one notices.\n\r",
		     ch);
	return;
    }
    if (ch->gold < amount_gold) {
	send_to_char("You don't have that much to split.\n\r", ch);
	return;
    }
    members = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM))
	    members++;
    }

    if (members < 2) {
	send_to_char("Just keep it all.\n\r", ch);
	return;
    }
    share_gold = amount_gold / members;
    extra_gold = amount_gold % members;

    if (share_gold == 0) {
	send_to_char("Don't even bother, cheapskate.\n\r", ch);
	return;
    }
    ch->gold -= amount_gold;
    ch->gold += share_gold + extra_gold;

    sprintf(buf, "You split %d gold coins. Your share is %d gold.\n\r",
	    amount_gold, share_gold + extra_gold);
    send_to_char(buf, ch);

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (gch != ch && is_same_group(gch, ch)
	    && !IS_AFFECTED(gch, AFF_CHARM)) {
	    act(buf, ch, NULL, gch, TO_VICT);
	    gch->gold += share_gold;
	}
    }

    return;
}



void do_gtell(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *gch;

    if (argument[0] == '\0') {
	send_to_char("Tell your group what?\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOTELL)) {
	send_to_char("{mYour message didn't get through!{x\n\r", ch);
	return;
    }
    act_new("{mYou tell your group '$t'{x",
	    ch, argument, NULL, TO_CHAR, POS_SLEEPING);

    for (gch = char_list; gch != NULL; gch = gch->next) {
	if (is_same_group(gch, ch))
	    act_new("{m$n tells the group '$t'{x",
		    ch, argument, gch, TO_VICT, POS_SLEEPING);
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group(CHAR_DATA * ach, CHAR_DATA * bch)
{
    if (ach == NULL || bch == NULL)
	return FALSE;

    if (ach->leader != NULL)
	ach = ach->leader;
    if (bch->leader != NULL)
	bch = bch->leader;
    return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
void do_colour(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *orig;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    bool found;
    int i;
    int col;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg1[0] == '\0') {
	if (!IS_SET(ch->act, PLR_COLOUR)) {
	    SET_BIT(ch->act, PLR_COLOUR);
	    send_to_char("You now see the world in {bC{ro{yl{co{gr{x!\n\r",
			 ch);
	} else {
	    send_to_char_bw("Everything is but shades of gray to you.\n\r",
			    ch);
	    REMOVE_BIT(ch->act, PLR_COLOUR);
	}
	return;
    }
    if (!ch->desc)
	return;

    orig = ch->desc->original ? ch->desc->original : ch->desc->character;

    if (!str_cmp(arg1, "reset")) {
	for (i = 0; i < MAX_CHANNELS; i++)
	    orig->pcdata->channel[i] = str_dup(color_table[i].color);

	send_to_char("Color reset.\n\r", ch);
	return;
    }
    if (arg2[0] == '\0' || arg3[0] == '\0') {
	send_to_char("Syntax: color <channel> BRIGHT <color>\n\r", ch);
	send_to_char("Syntax: color <channel> DARK <color>\n\r", ch);
	send_to_char("Syntax: color RESET\n\r", ch);
	return;
    }
    found = FALSE;

    for (i = 0; color_table[i].name != NULL; i++) {
	if (!str_cmp(arg1, color_table[i].name)) {
	    found = TRUE;
	    break;
	}
    }

    if (!found) {
	send_to_char("That's not a channel. Try one of these:\n\r\n\r",
		     ch);

	for (i = 0, col = 0; color_table[i].name != NULL; i++) {
	    sprintf(buf, "%-10s", color_table[i].name);
	    send_to_char(buf, ch);
	    if (++col % 5 == 0)
		send_to_char("\n\r", ch);
	}

	if (col % 5 != 0)
	    send_to_char("\n\r", ch);
	return;
    }
    buf[0] = '{';
    buf[1] = arg3[0] == '{' ? arg3[1] : arg3[0];
    buf[2] = '\0';

    if (!str_cmp(arg2, "bright"))
	buf[1] = UPPER(buf[1]);
    else if (str_cmp(arg2, "dark")) {
	send_to_char("Syntax: color <channel> BRIGHT <color>\n\r", ch);
	send_to_char("Syntax: color <channel> DARK <color>\n\r", ch);
	send_to_char("Syntax: color RESET\n\r", ch);
	return;
    }
    free_string(orig->pcdata->channel[i]);
    orig->pcdata->channel[i] = str_dup(buf);

    sprintf(buf, "The %s channel's color is now %sTHIS{x color.\n\r",
	    color_table[i].name, orig->pcdata->channel[i]);
    send_to_char(buf, ch);

    return;
}

void color(CHAR_DATA * ch, const char *string)
{
    CHAR_DATA *orig;
    int i;

    if (!ch->desc)
	    return;

    orig = ch->desc->original ? ch->desc->original : ch->desc->character;

    for (i = 0; color_table[i].name != NULL; i++) {
	    if (!str_cmp(string, color_table[i].name)) {
	      if (orig->pcdata->channel[i])
		      send_to_char(orig->pcdata->channel[i], ch);
	      else
		      send_to_char(color_table[i].color, ch);
	      break;
	    }
    }
    return;
}

bool check_word(const char *word, const char *argument)
{
    bool found = FALSE;
    int i, b;

    for (i = 0; argument[i]; i++) {
	if (LOWER(argument[i]) != LOWER(word[0]))
	    continue;

	if (!str_cmp(word, "twat") && i > 0
	    && i < strlen(argument) - 1 && !isspace(argument[i - 1])
	    && isspace(argument[i + 1]))
	    continue;

	if (!str_cmp(word, "shit") && i > 0
	    && i < strlen(argument) - 2 && !isspace(argument[i - 1])
	    && (isspace(argument[i + 1]) || isspace(argument[i + 2])))
	    continue;

	if (strlen(argument) - i < strlen(word))
	    continue;

	found = TRUE;

	for (b = 0; word[b]; b++) {
	    while (argument[i] && (argument[i] < 65 || argument[i] > 122))
		i++;

	    if (LOWER(argument[i]) != LOWER(word[b])) {
		found = FALSE;
		break;
	    }
	    while (argument[i] &&
		   (argument[i] < 65 || argument[i] > 122 ||
		    LOWER(argument[i]) == LOWER(word[b])))
		i++;
	}

	if (found)
	    return TRUE;
    }
    return FALSE;
}

void swear(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("The gods have revoked your channel privileges.\n\r",
		     ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOSWEAR);

    /* Make the words drunk if needed... */
    argument = makedrunk(argument, ch);

    color(ch, "swear");		/* Customizable color */

    sprintf(buf, "You vulgarly spit '%s'{x\n\r", argument);
    send_to_char(buf, ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    !IS_SET(victim->comm, COMM_NOSWEAR) &&
	    !IS_SET(victim->comm, COMM_QUIET)) {
	    color(d->character, "swear");	/* Customizable color */
	    act_new("$n vulgarly spits '$t'{x",
		    ch, argument, d->character, TO_VICT, POS_SLEEPING);
	}
    }
    return;
}

/* See if we need to send it to the cuss channel */

bool check_swear(CHAR_DATA * ch, char *argument)
{
	return FALSE;
}

/********************************************************************
* This is the code for gocials, or socials over the gossip channel. *
* It is fairly long and complicated and took me a whole hour and a  *
* half to write up. I finally did get it working though, but I am   *
* sure there are neater ways to code it...the code towards the      *
* end where it tries to do the replacing done normally by acts      *
* so that it can get the victim's name/etc in and then send it to   *
* another victim with the act command. Feel free to edit it if you  *
* can do a quick re-write using much neater code. This should work  *
* for all possible socials that normally work in the mud.           *
********************************************************************/

/********************************************************************
* I ask for no credit on the mud for this, nor do i ask for any-    *
* thing in exchange. I believe in the philosophy of giving what you *
* can to the mud community and then taking what others offer. Use   *
* this, copy this, give it to friends...whatever you like.          *
*                                                 -Bangle           *
********************************************************************/



void do_gocial(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    int counter;
    int count;
    int cmd;
    bool found;

		//Quick but dirty fix so mortals don't use gocial - Severhand
		if(!IS_IMMORTAL(ch))
			return;

    argument = one_argument(argument, command);

    if (command[0] == '\0') {
	send_to_char("What do you wish to gocial?\n\r", ch);
	return;
    }
    found = FALSE;
    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++) {
	if (command[0] == social_table[cmd].name[0]
	    && !str_prefix(command, social_table[cmd].name)) {
	    found = TRUE;
	    break;
	}
    }

    if (!found) {
	send_to_char("What kind of social is that?!?!\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOGOSSIP)) {
	send_to_char("But you have the gossip channel turned off!\n\r",
		     ch);
	return;
    }
    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("The gods have revoked your channel priviliges.\n\r",
		     ch);
	return;
    }
    switch (ch->position) {
    case POS_DEAD:
	send_to_char("Lie still; you are DEAD!\n\r", ch);
	return;
    case POS_INCAP:
    case POS_MORTAL:
	send_to_char("You are hurt far too bad for that.\n\r", ch);
	return;
    case POS_STUNNED:
	send_to_char("You are too stunned for that.\n\r", ch);
	return;
    }

    one_argument(argument, arg);
    victim = NULL;
    buf[0] = '\0';
    if (arg[0] == '\0') {
	sprintf(buf, "{m%s{x", social_table[cmd].char_no_arg);
	act_new(buf, ch, NULL, NULL, TO_CHAR, POS_DEAD);
	sprintf(buf, "{m%s{x", social_table[cmd].others_no_arg);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *vch;
	    vch = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(vch->comm, COMM_NOGOSSIP) &&
		!IS_SET(vch->comm, COMM_QUIET)) {
		act_new(buf, ch, NULL, vch, TO_VICT, POS_DEAD);
	    }
	}
    } else if ((victim = get_char_world(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    } else if (victim == ch) {
	if (social_table[cmd].char_auto)
	    sprintf(buf, "{m%s{x", social_table[cmd].char_auto);

	act_new(buf, ch, NULL, NULL, TO_CHAR, POS_DEAD);

	if (social_table[cmd].others_auto)
	    sprintf(buf, "{m%s{x", social_table[cmd].others_auto);
	else
	    buf[0] = '\0';

	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *vch;
	    vch = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(vch->comm, COMM_NOGOSSIP) &&
		!IS_SET(vch->comm, COMM_QUIET)) {
		act_new(buf, ch, NULL, vch, TO_VICT, POS_DEAD);
	    }
	}
    } else {
	if (social_table[cmd].char_found)
	    sprintf(buf, "{m%s{x", social_table[cmd].char_found);

	act_new(buf, ch, NULL, victim, TO_CHAR, POS_DEAD);

	if (social_table[cmd].vict_found)
	    sprintf(buf, "{m%s{x", social_table[cmd].vict_found);
	else
	    buf[0] = '\0';

	act_new(buf, ch, NULL, victim, TO_VICT, POS_DEAD);

	if (social_table[cmd].others_found)
	    sprintf(buf, "{m%s{x", social_table[cmd].others_found);
	else
	    buf[0] = '\0';

	for (counter = 0; buf[counter + 1] != '\0'; counter++) {
	    if (buf[counter] == '$' && buf[counter + 1] == 'N') {
		strcpy(buf2, buf);
		buf2[counter] = '\0';
		if (!IS_NPC(victim)) {
		    strcat(buf2, victim->name);
		} else {
		    strcat(buf2, victim->short_descr);
		}
		for (count = 0; buf[count] != '\0'; count++) {
		    buf[count] = buf[count + counter + 2];
		}
		strcat(buf2, buf);
		strcpy(buf, buf2);

	    } else if (buf[counter] == '$' && buf[counter + 1] == 'E') {
		switch (victim->sex) {
		default:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "it");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		case 1:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "it");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		case 2:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "it");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		}
	    } else if (buf[counter] == '$' && buf[counter + 1] == 'M') {
		buf[counter] = '%';
		buf[counter + 1] = 's';
		switch (victim->sex) {
		default:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "it");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		case 1:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "him");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		case 2:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "her");
		    for (count = 0; buf[count] != '\0'; count++);
		    {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		}
	    } else if (buf[counter] == '$' && buf[counter + 1] == 'S') {
		switch (victim->sex) {
		default:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "its");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		case 1:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "his");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		case 2:
		    strcpy(buf2, buf);
		    buf2[counter] = '\0';
		    strcat(buf2, "hers");
		    for (count = 0; buf[count] != '\0'; count++) {
			buf[count] = buf[count + counter + 2];
		    }
		    strcat(buf2, buf);
		    strcpy(buf, buf2);
		    break;
		}
	    }
	}
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *vch;
	    vch = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		d->character != victim &&
		!IS_SET(vch->comm, COMM_NOGOSSIP) &&
		!IS_SET(vch->comm, COMM_QUIET)) {
		act_new(buf, ch, NULL, vch, TO_VICT, POS_DEAD);
	    }
	}
    }
    return;
}


/* EOF */
