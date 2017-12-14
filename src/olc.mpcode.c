/***************************************************************************
 *  File: olc_mpcode.c                                                     *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 * The following code is based on ILAB OLC by Jason Dinkel                 *
 * Mobprogram code by Lordrom for Nevermore Mud                            *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *  ROM OLC for 2.4 was made to work by Beowolf and Nikki. We thank        *
 *  Airius and all the others from the ROM mailing list and the Merc       *
 *  mailing lists as well. Many hours have been put into this version      *
 *  We at The Shadow Realms MUD (tsr.org 8000) hope you enjoy this as      *
 *  much as we do.                                                         *
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
#include "olc.h"

#define MPEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
#define EDIT_MPCODE(Ch, Code)   ( Code = (MPROG_CODE*)Ch->desc->pEdit )

DECLARE_OLC_FUN(mpedit_create);
DECLARE_OLC_FUN(mpedit_desc);
DECLARE_OLC_FUN(mpedit_show);

const struct olc_cmd_type mpedit_table[] = {
/*     { command           function    }, */

    {"commands", show_commands},
    {"create", mpedit_create},
    {"desc", mpedit_desc},
    {"show", mpedit_show},
    {"?", show_help},

    {"", 0}
};

void mpedit(CHAR_DATA * ch, char *argument)
{
    MPROG_CODE *pMcode;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    EDIT_MPCODE(ch, pMcode);
    if (ch->pcdata->security < 7)
		{
			send_to_char("MPEdit: Insufficient security to modify code \n\r ",ch);
			edit_done(ch);
			return;
		}

           if (command[0] == '\0') {
           mpedit_show(ch, argument); return;}
           if (!str_cmp(command, "done")) {
           save_mobprog(pMcode); edit_done(ch); return;}

           for (cmd = 0; mpedit_table[cmd].name[0] != '\0';
          cmd++) {
           if (!str_prefix(command, mpedit_table[cmd].name)) {
           (*mpedit_table[cmd].olc_fun) (ch, argument); return;}
           }

           interpret(ch, arg); return;}

           void do_mpedit(CHAR_DATA * ch, char *argument) {
           MPROG_CODE * pMcode;
           char command[MAX_INPUT_LENGTH];
           argument = one_argument(argument, command);
           if (is_number(command)) {
           if (!(pMcode = get_mprog_index(atoi(command)))) {
           send_to_char("MPEdit: That vnum does not exist.\n\r",
            ch); return;}
           ch->desc->pEdit = (void *) pMcode;
           ch->desc->editor = ED_MPCODE; return;}

           if (!str_cmp(command, "create")) {
           if (argument[0] == '\0') {
           send_to_char("Syntax: edit code create [vnum]\n\r",
            ch); return;}
           if (mpedit_create(ch, argument))
           ch->desc->editor = ED_MPCODE;}

           return;}

           MPEDIT(mpedit_create) {
           MPROG_CODE * pMcode;
           int value;
           value = atoi(argument);
           if (argument[0] == '\0' || value == 0) {
           send_to_char("Syntax: mpedit create [vnum]\n\r", ch);
           return FALSE;}

           if (get_mprog_index(value)) {
           send_to_char("MPEdit: Code vnum already exists.\n\r",
            ch); return FALSE;}

           pMcode = new_mpcode();
           pMcode->vnum = value;
           pMcode->next = mprog_list;
           mprog_list = pMcode;
           ch->desc->pEdit = (void *) pMcode;
           send_to_char("MobProgram Code Created.\n\r", ch);
           return TRUE;}

           MPEDIT(mpedit_show) {
           MPROG_CODE * pMcode;
           char buf[MAX_STRING_LENGTH];
           EDIT_MPCODE(ch, pMcode);
           sprintf(buf,
             "Vnum:       [%ld]\n\r"
             "Code:\n\r%s\n\r",
             pMcode->vnum, pMcode->code);
           send_to_char(buf, ch); return FALSE;}

           MPEDIT(mpedit_desc) {
           MPROG_CODE * pMcode;
           EDIT_MPCODE(ch, pMcode); if (argument[0] == '\0') {
           string_append(ch, &pMcode->code); return TRUE;}

           send_to_char(" Syntax: desc\n\r", ch); return FALSE;}
