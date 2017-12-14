/*
 *	WDL - stupid bank code, May, 1999.
 *  this uses come code recycled from, prolly, diku.
 *  all relevant and verbose licenses apply
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "merc.h"
#include "utils.h"
#include "tables.h"
#include "lookup.h"

/* command procedures needed */
DECLARE_DO_FUN(do_say);

CHAR_DATA *find_banker(CHAR_DATA * ch)
{
    CHAR_DATA *banker;

    if (IS_NPC(ch))
	return NULL;


    for (banker = ch->in_room->people; banker;
	 banker = banker->next_in_room) {
	if (IS_NPC(banker) && IS_SET(banker->act, ACT_BANKER))
	    break;
    }

    if (banker && !can_see(banker, ch)) {
	do_say(banker, "I'm not taking money from someone I can't see!");
	return NULL;
    }

    return banker;
}


void do_balance(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *banker;

    if (IS_NPC(ch))
	return;

    if ((banker = find_banker(ch)) == NULL) {
	send_to_char("You need to go find a bank, first.\r\n", ch);
	return;
    }

    sprintf(buf, "You currently have %ld gold in the bank, %s.\r\n",
	    ch->pcdata->balance, ch->name);
    do_say(banker, buf);

    return;
}

void do_withdraw(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *banker;
    long amount;

    if (IS_NPC(ch))
	return;

    amount = atoi(argument);

    if ((banker = find_banker(ch)) == NULL) {
	send_to_char("You need to go find a bank, first.\r\n", ch);
	return;
    }

    if (amount < 1) {
	send_to_char("Try a real amount of money, please.\r\n", ch);
	return;
    }

    if (amount > ch->pcdata->balance) {
	sprintf(buf,
		"You can't withdraw %ld gold when you have a balance of %ld gold, %s.\r\n",
		amount, ch->pcdata->balance, ch->name);
	do_say(banker, buf);
	return;
    }

    ch->gold += amount;
    ch->pcdata->balance -= amount;

    do_balance(ch, "");

    return;
}

void do_deposit(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *banker;
    long amount;


    if (IS_NPC(ch))
	return;

    amount = atoi(argument);

    if ((banker = find_banker(ch)) == NULL) {
	send_to_char("You need to go find a bank, first.\r\n", ch);
	return;
    }

    if (amount < 1) {
	send_to_char("Try a real amount of money, please.\r\n", ch);
	return;
    }

    if (amount > ch->gold) {
	sprintf(buf,
		"You can't deposit %ld gold when you're only carrying %ld gold, %s.\r\n",
		amount, ch->gold, ch->name);
	do_say(banker, buf);
	return;
    }

    ch->gold -= amount;
    ch->pcdata->balance += amount;

    do_balance(ch, "");

    return;
}

void do_ping(CHAR_DATA * ch, char *argument)
{
	send_to_char( "pong.\r\n", ch );
    return;
}

void do_cbalance(CHAR_DATA * ch, char *argument) {

	int i;
    CHAR_DATA *banker;
	char buf[MSL];

    if (IS_NPC(ch))
	return;

	if (IS_IMMORTAL(ch)) {
		for ( i = 1; i < MAX_CLAN; i++ ) {
			sprintf(buf, "Clan: %-10s Balance: %ld\r\n",
					clan_table[i].name,
					clan_table[i].balance);
			send_to_char(buf, ch);
		}
		return;
	}

	if (!ch->clan) {
    send_to_char( "You're not in a clan, silly.\r\n", ch );
	return;
	}

    if ((banker = find_banker(ch)) == NULL) {
	send_to_char("You need to go find a bank, first.\r\n", ch);
	return;
    }

	sprintf(buf, "Clan: %-10s Balance: %ld",
       		clan_table[ch->clan].name,
       		clan_table[ch->clan].balance);

	do_say(banker, buf);

	return;

}

void do_cdeposit(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *banker;
    long amount;


    if (IS_NPC(ch) || (!ch->clan)) {
    	send_to_char( "You're not in a clan, silly.\r\n", ch );
		return;
	}

    amount = atoi(argument);

    if ((banker = find_banker(ch)) == NULL) {
	send_to_char("You need to go find a bank, first.\r\n", ch);
	return;
    }

    if (amount < 1) {
	send_to_char("Try a real amount of money, please.\r\n", ch);
	return;
    }

    if (amount > ch->gold) {
	sprintf(buf,
		"You can't deposit %ld gold when you're only carrying %ld gold, %s.\r\n",
		amount, ch->gold, ch->name);
	do_say(banker, buf);
	return;
    }

    ch->gold -= amount;
	clan_table[ch->clan].balance += amount;

    do_cbalance(ch, "");

    return;
}

void do_cwithdraw(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *banker;
    long amount;

    if (IS_NPC(ch) || (!ch->clan)) {
    	send_to_char( "You're not in a clan, silly.\r\n", ch );
		return;
	}

    amount = atoi(argument);

    if ((banker = find_banker(ch)) == NULL) {
	send_to_char("You need to go find a bank, first.\r\n", ch);
	return;
    }

    if (amount < 1) {
	send_to_char("Try a real amount of money, please.\r\n", ch);
	return;
    }

    if (amount > clan_table[ch->clan].balance) {
	sprintf(buf,
		"You can't withdraw %ld gold when your clan has a balance of %ld gold, %s.\r\n",
		amount, clan_table[ch->clan].balance, ch->name);
	do_say(banker, buf);
	return;
    }

    ch->gold += amount;
    clan_table[ch->clan].balance -= amount;

    do_cbalance(ch, "");

    return;
}

void cbank_save() {

	int i;
    FILE *fp;

    fclose(fpReserve);

	if ((fp = fopen("cbank.dat", "w")) == NULL) {
        bug("cbank_save: cbank.dat: fopen", 0);
    	fpReserve = fopen(NULL_FILE, "r");
		return;
    }

    for ( i = 1; i < MAX_CLAN; i++ ) {
            fprintf(fp, "%s %ld\n",
                    clan_table[i].name,
                    clan_table[i].balance);
	}
  
    fclose(fp);

    fpReserve = fopen(NULL_FILE, "r");
	return;
}

void cbank_load() {

    FILE *fp;
	int clan;
    
    fclose(fpReserve);
    
    if ((fp = fopen("cbank.dat", "r")) == NULL) {
        bug("cbank_load: cbank.dat: fopen", 0);
    	fpReserve = fopen(NULL_FILE, "r");
		return;
    }   
    
	for (;;) {

		if (feof(fp))
			break;
		clan = clan_lookup(fread_word(fp));
		clan_table[clan].balance = fread_number(fp); 
		fread_to_eol(fp);	
	}

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");
	return;
}
