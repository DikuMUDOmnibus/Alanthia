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

 /* Help Editor - kermit 1/98 */
HEDIT(hedit_make)
{
    HELP_DATA *pHelp;

    if (argument[0] == '\0') {
	send_to_char("Syntax: mpedit make [keyword(s)]\n\r", ch);
	return FALSE;
    }
    pHelp = new_help();
    pHelp->keyword = str_dup(argument);
    pHelp->next = help_first;
    help_first = pHelp;
    ch->desc->pEdit = (void *) pHelp;

    send_to_char("New Help Entry Created.\n\r", ch);
    return TRUE;
}


HEDIT(hedit_show)
{
    HELP_DATA *pHelp;
    char buf[MSL];

    EDIT_HELP(ch, pHelp);

    if (pHelp->delete) {
	send_to_char("\n\nTHIS HELP IS MARKED FOR DELETION!\n\r", ch);
	return FALSE;
    }
    sprintf(buf, "Level:       [%d]\n\r"
	    "Keywords: %s\n\r"
	    "\n\r%s\n\r", pHelp->level, pHelp->keyword, pHelp->text);
    send_to_char(buf, ch);

    return FALSE;
}

HEDIT(hedit_desc)
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);

    if (argument[0] == '\0') {
	string_append(ch, &pHelp->text);
	return TRUE;
    }
    send_to_char(" Syntax: desc\n\r", ch);
    return FALSE;
}

HEDIT(hedit_keywords)
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);

    if (argument[0] == '\0') {
	send_to_char(" Syntax: keywords [keywords]\n\r", ch);
	return FALSE;
    }
    pHelp->keyword = str_dup(argument);
    send_to_char("Keyword(s) Set.\n\r", ch);
    return TRUE;
}

HEDIT(hedit_level)
{
    HELP_DATA *pHelp;

    EDIT_HELP(ch, pHelp);

    if (argument[0] == '\0' || !is_number(argument)) {
	send_to_char("Syntax:  level [number]\n\r", ch);
	return FALSE;
    }
    pHelp->level = atoi(argument);

    send_to_char("Level set.\n\r", ch);
    return TRUE;
}

HEDIT(hedit_delete)
{
    HELP_DATA *pHelp;

    EDIT_HELP(ch, pHelp);

    if (!pHelp->delete) {
	pHelp->delete = TRUE;
	send_to_char("YOU HAVE MARKED THIS HELP FOR DELETION!\n\r", ch);
	return TRUE;
    }
    pHelp->delete = FALSE;
    send_to_char("YOU HAVE UNMARKED THIS HELP FOR DELETION!\n\r", ch);
    return TRUE;
}
