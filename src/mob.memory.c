/*
 *
 * ACT_MOB.C:			Trilby@Refugee MUD
 *				Copyright 1997, Lord Thomas Burbridge
 *
 * Mobile memory procedures.
 *
 * This work is a derivative of ROM2.4
 * Copyright 1995, Russ Taylor.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "merc.h"
#include "tables.h"
#include "recycle.h"
#include "utils.h"

MEM_DATA *get_mem_data(CHAR_DATA * ch, CHAR_DATA * target)
{
    MEM_DATA *remember;

    if (!IS_NPC(ch)) {
	bug("get_mem_data: ch not NPC", 0);
	return NULL;
    }

    if (ch == NULL) {
	bug("get_mem_data:  NULL ch", 0);
	return NULL;
    }

    if (target == NULL) {
	bug("get_mem_data:  NULL target", 0);
	return NULL;
    }

    for (remember = ch->memory; remember != NULL;
	 remember = remember->next) {
	if (remember->id == target->id)
	    return remember;
    }

    return NULL;
}

void mob_remember(CHAR_DATA * ch, CHAR_DATA * target, int reaction)
{
    MEM_DATA *remember;
    char buf4[MAX_STRING_LENGTH];


    if (!IS_NPC(ch)) {
	sprintf(buf4, "mob_remember: ch %s not NPC from mob %s", ch->name,
		target->name);
	bug(buf4, 0);
	return;
    }

    if (ch == NULL) {
	bug("mob_remember:  NULL ch", 0);
	return;
    }

    if (target == NULL) {
	bug("mob_remember:  NULL target", 0);
	return;
    }


    if ((remember = get_mem_data(ch, target)) == NULL) {
	remember = new_mem_data();

	remember->next = ch->memory;
	ch->memory = remember;

    }

    remember->id = target->id;
    remember->when = current_time;

    SET_BIT(remember->reaction, reaction);

    return;
}

void mem_fade(CHAR_DATA * ch)
{				/* called from char_update */
    MEM_DATA *remember, *remember_next;

    if (ch == NULL) {
	bug("mem_fade:  NULL ch", 0);
	return;
    }

    if (!IS_NPC(ch)) {
	bug("mem_fade: ch not NPC", 0);
	return;
    }

    if (ch->memory == NULL)
	return;

    for (remember = ch->memory; remember != NULL; remember = remember_next) {
	remember_next = remember->next;

	if (IS_NPC(ch)
	    && IS_SET(ch->off_flags, OFF_HUNT)
	    && ch->hunting == NULL
	    && IS_SET(remember->reaction, MEM_HOSTILE))
		ch->hunting = get_char_id(remember->id);

	if (current_time - remember->when < (96 * 60))
	    mob_forget(ch, remember);
    }

    return;
}

void mob_forget(CHAR_DATA * ch, MEM_DATA * memory)
{

    if (!IS_NPC(ch)) {
	bug("mob_forget:  ch not NPC", 0);
	return;
    }
    if (ch == NULL) {
	bug("mob_forget:  NULL ch", 0);
	return;
    }

    if (memory == NULL)
	return;

    if (memory == ch->memory) {
	ch->memory = memory->next;
    } else {
	MEM_DATA *prev;

	for (prev = ch->memory; prev != NULL; prev = prev->next) {
	    if (prev->next == memory) {
		prev->next = memory->next;
		break;
	    }

	    if (prev == NULL) {
		bug("mob_forget:  memory not found", 0);
		return;
	    }
	}
    }
    free_mem_data(memory);
    return;
}
