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
#include "recycle.h"


REDIT(redit_delete)
{

    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *pRoom;
    EXIT_DATA *pExit;
    BUFFER *buffer;
    char buf[MAX_INPUT_LENGTH];
    int iHash, door;


    EDIT_ROOM(ch, pRoom);

    pRoomIndex = pRoom;

    if (get_trust(ch) < IMMORTAL) {
	send_to_char("Only imps may delete rooms, sorry.", ch);
	return FALSE;
    }
    if (pRoom->deleted) {
	pRoom->deleted = FALSE;
	send_to_char("Deletion mark removed.\n\r", ch);
	return TRUE;
    } else {
	if (strcmp(argument, "room now")) {
	    send_to_char
		("You must type 'delete room now' in order to remove an room permanently.\r\n",
		 ch);
	    return FALSE;
	}
	pRoom->deleted = TRUE;
	sprintf(buf, "%s deleted room %ld.", ch->name, pRoom->vnum);
	log_printf(buf);
	send_to_char("{RRoom marked for deletion.\r\n{x", ch);

	buffer = new_buf();

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	    for (pRoom = room_index_hash[iHash]; pRoom;
		 pRoom = pRoom->next) {
		for (door = 0; door < MAX_DIR; door++) {
		    if ((pExit = pRoom->exit[door])
			&& pExit->u1.to_room->vnum == pRoomIndex->vnum
			&& pExit->u1.to_room->deleted) {
			sprintf(buf,
				"Found door to this room at room %ld\r\n",
				pRoom->vnum);
			add_buf(buffer, buf);
		    }
		}
	    }
	}


	page_to_char(buf_string(buffer), ch);
	free_buf(buffer);
	return TRUE;
    }

    return FALSE;
}



OEDIT(oedit_delete)
{
    OBJ_INDEX_DATA *pObj;
    ROOM_INDEX_DATA *pRoom;
    RESET_DATA *pReset;
    BUFFER *buffer;
    char buf[MAX_INPUT_LENGTH];
    int iHash;
    int count = 0;

    EDIT_OBJ(ch, pObj);

    if (get_trust(ch) < IMMORTAL) {
	send_to_char("Only imps may delete objects, sorry.\r\n", ch);
	return FALSE;

    }
    if (pObj->deleted) {
	pObj->deleted = FALSE;
	send_to_char("Deletion mark removed.\n\r", ch);
	return TRUE;
    } else {
	if (strcmp(argument, "object now")) {
	    send_to_char
		("You must type 'delete object now' in order to remove an object permanently.\r\n",
		 ch);
	    return FALSE;
	}
	pObj->deleted = TRUE;
	buffer = new_buf();

	sprintf(buf,
		"********** %s deleted object %ld ***********************",
		ch->name, pObj->vnum);
	log_printf(buf);

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	    for (pRoom = room_index_hash[iHash]; pRoom;
		 pRoom = pRoom->next) {
		for (pReset = pRoom->reset_first; pReset;
		     pReset = pReset->next) {
		    if (
			(pReset->arg1 == pObj->vnum
			 && (pReset->command == '0'
			     || pReset->command == 'P'
			     || pReset->command == 'G'
			     || pReset->command == 'E'))) {

			sprintf(buf,
				"Deleting object %ld ( %s ) in room %ld.",
				pObj->vnum, pObj->short_descr,
				pRoom->vnum);
			log_printf(buf);

			add_buf(buffer, buf);
			add_buf(buffer, "\r\n");	/*
							 * guh. log_printf adds newline for ya 
							 */

			count++;
		    }
		}
	    }
	}

	sprintf(buf, "%d instances of item removed.\r\n", count);
	add_buf(buffer, buf);
	sprintf(buf, "{RObject Marked For Deletion.{x\n\r");
	add_buf(buffer, buf);
	page_to_char(buf_string(buffer), ch);
	free_buf(buffer);
    }

    return TRUE;
}

MEDIT(medit_delete)
{
    MOB_INDEX_DATA *pMob;
    ROOM_INDEX_DATA *pRoom;
    RESET_DATA *pReset;
    BUFFER *buffer;
    char buf[MAX_INPUT_LENGTH];
    int iHash;
    int count = 0;

    EDIT_MOB(ch, pMob);

    if (get_trust(ch) < IMMORTAL) {
	medit_show(ch, "");
	send_to_char("Only imps may delete mobiles, sorry.\r\n", ch);
	return FALSE;
    }
    if (pMob->deleted) {
	pMob->deleted = FALSE;
	medit_show(ch, "");
	send_to_char("Deletion mark removed.\n\r", ch);
	return TRUE;
    } else {
	if (strcmp(argument, "mob now")) {
	    medit_show(ch, "");
	    send_to_char
		("You must type 'delete mob now' in order to remove an object permanently.\r\n",
		 ch);
	    return FALSE;
	}
	pMob->deleted = TRUE;
	buffer = new_buf();

	sprintf(buf,
		"********** %s deleted mobile %ld ***********************",
		ch->name, pMob->vnum);
	log_printf(buf);

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	    for (pRoom = room_index_hash[iHash]; pRoom;
		 pRoom = pRoom->next) {
		for (pReset = pRoom->reset_first; pReset;
		     pReset = pReset->next) {
		    if (pReset->arg1 == pMob->vnum
			&& pReset->command == 'M') {
			sprintf(buf,
				"Deleting mobile %ld ( %s ) in room %ld.",
				pMob->vnum, pMob->short_descr,
				pRoom->vnum);
			log_printf(buf);

			add_buf(buffer, buf);
			add_buf(buffer, "\r\n");	/*
							 * guh. log_printf() adds newline for ya 
							 */

			count++;
		    }
		}
	    }
	}

	sprintf(buf, "%d instances of mobile removed.\r\n", count);
	add_buf(buffer, buf);
	sprintf(buf, "{RMobile Marked For Deletion. *snif*{x\n\r");
	add_buf(buffer, buf);
	page_to_char(buf_string(buffer), ch);
	free_buf(buffer);
    }

    return TRUE;
}
