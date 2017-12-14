
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
#include "merc.h"
#include "utils.h"

#define ANY 	0
#define	UNARMED 1
#define BLUNT	2
#define BLADED	3
#define	KNIFE	4
#define WHIP	5

DECLARE_ATTACK_FUN(attack_noop);
DECLARE_ATTACK_FUN(attack_crush);

/*************************************************************************
*
* The attack table.
* Format of an attack is:
*   name   message function type ctimer hr_mod dr_mod ac_mod fatigue dtype
*/

const struct attack_type attack_table[] =
{
    {"none", "hit", attack_noop, ANY, 20, 1, 1, 1, 1, -1},
    {"hit", "hit", attack_noop, ANY, 43, 1, 1, 1, 1, -1},

    {"digestion", "digestion", attack_noop, UNARMED, 50, 1, 1, 1, 1, DAM_ACID},

    {"bash", "bash", attack_noop, BLUNT, 50, 1, 1, 1, 1, DAM_BASH},
    {"beat", "beating", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_BASH},
    {"bludgeon", "bludgeon", attack_noop, BLUNT, 50, 1, 1, 1, 1, DAM_BASH},
    {"crush", "crush", attack_crush, BLUNT, 50, 1, 1, 1, 1, DAM_BASH},
    {"kick", "kick", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_BASH},
    {"knee", "knee", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_BASH},
    {"pound", "pound", attack_noop, BLUNT, 50, 1, 1, 1, 1, DAM_BASH},
    {"punch", "punch", attack_noop, UNARMED, 50, 1, 1, 1, 1, DAM_BASH},
    {"slap", "slap", attack_noop, UNARMED, 50, 1, 1, 1, 1, DAM_BASH},
    {"smash", "smash", attack_noop, UNARMED, 50, 1, 1, 1, 1, DAM_BASH},
    {"suction", "suction", attack_noop, BLUNT, 50, 1, 1, 1, 1, DAM_BASH},
    {"throw", "throw", attack_noop, UNARMED, 50, 1, 1, 1, 1, DAM_BASH},
    {"peckb", "peck", attack_noop, UNARMED, 50, 1, 1, 1, 1, DAM_BASH},

    {"chop", "chop", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_SLASH},
    {"claw", "claw", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_SLASH},
    {"slash", "slash", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_SLASH},
    {"slice", "slice", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_SLASH},

    {"bite", "bite", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"chomp", "chomp", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"lunge", "lunge", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"peck", "peck", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"pierce", "pierce", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"stab", "stab", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"sting", "sting", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_PIERCE},
    {"thrust", "thrust", attack_noop, BLADED, 50, 1, 1, 1, 1, DAM_PIERCE},

    {"backstab", "backstab", attack_noop, KNIFE, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"charge", "charge", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"circle", "circle", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"disarm", "disarm", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"entangle", "entangle", attack_noop, WHIP, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"gblow", "greatblow", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"headshot", "headshot", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"lash", "lash", attack_noop, WHIP, 50, 1, 1, 1, 1, DAM_SPECIAL},
    {"wait", "wait", attack_noop, ANY, 0, 0, 0, 0, 0, DAM_SPECIAL},
    {"whirl", "whirl", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_SPECIAL},

    {"blast", "blast", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_ENERGY},
    {"wrath", "wrath", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_ENERGY},

    {"chill", "chill", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_COLD},
    {"frbite", "frozen bite", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_COLD},

    {"fibite", "fiery bite", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_FIRE},
    {"flame", "flame", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_FIRE},


    {"shbite", "shocking bite", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_LIGHTNING},
    {"shock", "shock", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_LIGHTNING},

    {"acbite", "acid bite", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_ACID},
    {"slime", "slime", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_ACID},

    {"divine", "divine fury", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_HOLY},

    {"drain", "life drain", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_NEGATIVE},

    {"dodge", "dodge", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_DEFENSE},
    {"duck", "duck", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_DEFENSE},
    {"parry", "parry", attack_noop, ANY, 50, 1, 1, 1, 1, DAM_DEFENSE},

    {NULL, NULL, attack_noop, 0, 100, 100, 100, 100, 100, 0}
};

/*************************************************************************
*
* Utility functions to grab elements from the attack_table 
*
*
*/

ATTACK_FUN *attack_fun_lookup(char *name) {
    int i;

    for (i = 0; attack_table[i].name != NULL; i++) {
	if (LOWER(name[0]) == LOWER(attack_table[i].name[0])
	    && !str_prefix(name, attack_table[i].name))
	    return attack_table[i].function;
    }

    return 0;
}

char *attack_name_lookup(ATTACK_FUN * function) {
    int i;

    for (i = 0; attack_table[i].name != NULL; i++) {
	if (function == attack_table[i].function)
	    return attack_table[i].name;
    }

    return NULL;
}

int attack_index_lookup(char *name) {
    int i;

    for (i = 0; attack_table[i].name != NULL; i++) {
	if (LOWER(name[0]) == LOWER(attack_table[i].name[0])
	    && !str_prefix(name, attack_table[i].name))
	    return i;
    }

    return -1;
}

/*************************************************************************
*
* 
*
*
*/


/*************************************************************************
*
* The attack functions themselves
*
*
*/

// for attacks that have no special function
int attack_noop(CHAR_DATA * ch, CHAR_DATA * victim, int dt) {
	return 0;
}

// attempt to deliver a crushing blow
int attack_crush(CHAR_DATA * ch, CHAR_DATA * victim, int dt) {
    return 0;
}
