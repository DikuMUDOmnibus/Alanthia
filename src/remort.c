#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "utils.h"

DECLARE_DO_FUN(do_quit);

void do_remort(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    char *pArg;
    char cEnd;
    int gn;
    int sn;
    int class;

    pArg = arg1;
    while (isspace(*argument)) {
	argument++;
    }
    cEnd = ' ';
    if (*argument == '\'' || *argument == '"') {
	cEnd = *argument++;
    }
    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';
    pArg = arg2;
    while (isspace(*argument)) {
	argument++;
    }
    cEnd = ' ';
    if (*argument == '\'' || *argument == '"') {
	cEnd = *argument++;
    }
    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if (IS_NPC(ch)) {
	return;
    }
    if (ch->level <= 289) {
	return;
    }
    if (ch->level >= 301) {
	return;
    }
    class = class_lookup(arg2);

    if (ch->pcdata->confirm_remort) {

		if (class == -1) {
		    send_to_char("Invalid class selected.\n\r", ch);
		    send_to_char("Syntax remort <class>\n\r", ch);
	    return;
	} else {
	    ch->class = class;
	}

	ch->level = 2;
	ch->exp = 2000;
	ch->train = 40;
	ch->practice = 6;
	ch->max_hit = 20 + (20 * ch->remorts);
	ch->pcdata->perm_hit = ch->max_hit;
	ch->hit = ch->max_hit;
	ch->max_mana = 20 + (20 * ch->remorts);
	ch->pcdata->perm_mana = ch->max_mana;
	ch->mana = ch->max_mana;
	ch->max_move = 100 + (100 * ch->remorts);
	ch->pcdata->perm_move = ch->max_move;
	ch->move = ch->max_move;
	ch->blood = 20;
	ch->max_blood = ch->blood;
	ch->pcdata->points = 39;
	ch->gold = 30000;
	ch->pcdata->confirm_remort = FALSE;
	ch->perm_stat[STAT_STR] = 22;
	ch->perm_stat[STAT_INT] = 22;
	ch->perm_stat[STAT_WIS] = 22;
	ch->perm_stat[STAT_DEX] = 22;
	ch->perm_stat[STAT_CON] = 22;
	for (gn = 0; gn < MAX_GROUP; gn++) {
	    if (group_table[gn].name != NULL
		&& ch->pcdata->group_known[gn]) {
		gn_remove(ch, gn);
	    }
	}
	for (sn = 0; sn < MAX_SKILL; sn++) {
	    if (skill_table[sn].name != NULL) {
		ch->pcdata->learned[sn] = 0;
	    }
	}

	for (sn = 0; sn < MAX_SKILL; sn++) {
	    if (skill_table[sn].name != NULL) {
		ch->pcdata->learned[sn] = ch->pcdata->slot[sn];
	    }
	}
	for (sn = 0; sn < MAX_SKILL; sn++) {
	    if (skill_table[sn].name != NULL) {
		ch->pcdata->slot[sn] = 0;
	    }
	}

	ch->remorts += 1;
	ch->slots += 5 + (5 * ch->remorts);
	reset_char(ch);
	save_char_obj(ch);
	stop_fighting(ch, TRUE);
	do_quit(ch, "");
	return;
    }

    send_to_char("You need to type this command twice to make it work\n\r",
		 ch);
    send_to_char
	("This command will REMORT you if your unsure read the helpfile\n\r",
	 ch);
    send_to_char("You can assign skills with the slot command\n\r", ch);
    send_to_char
	("For several checks the mud will log you out when you type this command again\n\r",
	 ch);
    ch->pcdata->confirm_remort = TRUE;
    wiznet("$N is thinking about REMORTING. ", ch, NULL, 0, 0,
	   get_trust(ch));
}

void do_slot(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    char buf[MAX_STRING_LENGTH];

    char *pArg;
    char cEnd;
    int sn;
    int col = 0;


    pArg = arg1;
    while (isspace(*argument)) {
	argument++;
    }
    cEnd = ' ';
    if (*argument == '\'' || *argument == '"') {
	cEnd = *argument++;
    }
    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while (isspace(*argument)) {
	argument++;
    }
    cEnd = ' ';
    if (*argument == '\'' || *argument == '"') {
	cEnd = *argument++;
    }
    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if (arg1[0] == '\0') {
	send_to_char("slots known\n\r", ch);
	for (sn = 0; sn < MAX_SKILL; sn++) {

	    if (skill_table[sn].name == NULL) {
		break;
	    }

	    if (ch->level < skill_table[sn].skill_level[ch->class]
		|| ch->pcdata->slot[sn] < 1	/* skill is not known */
		|| (skill_table[sn].rating[ch->class] == -1 &&
		    ch->level <= 16)) {
		continue;
	    }

	    sprintf(buf, "\n%-18s ", skill_table[sn].name);
	    send_to_char(buf, ch);

	    if (++col % 3 == 0) {
		send_to_char("\n\r", ch);
	    }
	}

	if (col % 3 != 0) {
	    send_to_char("\n\r", ch);
	}
	return;
    } else if ((arg1[0] == '+' && arg2[0] != '\0') && (ch->slots > 0)) {
	sn = skill_lookup(arg2);
	if (sn > -1) {
	    if (!ch->pcdata->learned[sn]) {
		send_to_char
		    (" you need to give a valid skill or spell\n\r", ch);
		return;
	    }
	}

	ch->slots -= 1;
	ch->pcdata->slot[sn] = 75;
	send_to_char("skill saved\n\r", ch);
	reset_char(ch);
	save_char_obj(ch);
    } else if ((arg1[0] == '-' && arg2[0] != '\0')) {
	sn = skill_lookup(arg2);
	if (sn > -1) {
	    if (!ch->pcdata->learned[sn]) {
		send_to_char
		    (" you need to give a valid skill or spell\n\r", ch);
		return;
	    }
	}

	if (!ch->pcdata->slot[sn] == 0) {
	    ch->pcdata->slot[sn] = 0;

	    ch->slots += 1;
	    ch->pcdata->slot[sn] = 0;
	    send_to_char("skill removed\n\r", ch);
	    reset_char(ch);
	    save_char_obj(ch);
	}
    } else if (arg1[0] == '\0') {
	send_to_char("you have to give a argument\n\r", ch);
	send_to_char("syntax slot <-/+> <skill/spell>\n\r", ch);
	send_to_char("syntax slot \n\r", ch);
	send_to_char
	    ("the first syntax's is to remove/add a skill/spell\n\r", ch);
	send_to_char("the second is to see the list of slots\n\r", ch);
	return;
    } else if (ch->slots == 0) {
	send_to_char("seems you are out of credits\n\r", ch);
	return;
    }

}
