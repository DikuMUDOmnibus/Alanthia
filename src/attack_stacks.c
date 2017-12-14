/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59 
 * Temple Place - Suite 330, Boston, MA  02111-1307, USA. 
 */

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "utils.h"

DECLARE_DO_FUN( do_help );

/*************************************************************************
*
* Utility functions to modify attack stacks.
* The attack stack is an integer array in char_data. It is terminated with
* a -1. The attack nouns and functions are dynamically looked up.
* astack add crush kick whirl stab
* astack show
* astack clear
*/

void astack_show( CHAR_DATA *ch, char *argument ) {

	int i;
	char buf[MAX_STRING_LENGTH];

	send_to_char( "Your current attack stack:\r\n", ch );

	for ( i = 0; ch->attack_stack[i] != -1; i++ ) {
		if ( i == ch->current_attack) {
		sprintf( buf, "%d: %s <-\r\n", i, 
			attack_table[ch->attack_stack[i]].name);
		send_to_char( buf, ch );
		} else {
		sprintf( buf, "%d: %s\r\n", i, attack_table[ch->attack_stack[i]].name);
		send_to_char( buf, ch );
		}
	}
	return;
}

void astack_reset( CHAR_DATA *ch, char *argument ) {

	ch->current_attack = 0;
	astack_show( ch, NULL );
	return;
}

void astack_add( CHAR_DATA *ch, char *argument) {

	int i = 0;
	int attack;
    char arg[MAX_STRING_LENGTH];

	one_argument( argument, arg );

	while ( arg[0] ) {
		argument = one_argument( argument, arg );
		attack = attack_index_lookup( arg );

		if ( attack > -1 )
			ch->attack_stack[i] = attack;
		else
			break;

		i++;

		if ( i == MAX_ATTACKS )
			break;
	} 

	ch->attack_stack[i] = -1;
	astack_show( ch, NULL );

	return;
}

void do_astack( CHAR_DATA *ch, char *argument ) {

    char arg[MAX_STRING_LENGTH];

	// parse args
	// show help if empty
	if ( argument[0] == '\0') { 
		do_help( ch, "astack" );	
		return;
	}
	
	argument = one_argument( argument, arg );

	// show stack
	if ( !strcmp( arg, "show" )) {
		astack_show( ch, NULL );
		return;
	}

	// clear stack
	if ( !strcmp( arg, "clear" )) {
		ch->attack_stack[0] = -1;
		astack_show( ch, NULL);
		return;
	}

	// insert items into stack
	if ( !strcmp( arg, "add" )) {
		astack_add( ch, argument );
		return;
	}

	// reset current attack to top of stack
	if ( !strcmp( arg, "reset" )) {
		astack_reset( ch, NULL );
		return;
	}

	return;
}



























