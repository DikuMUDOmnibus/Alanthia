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
 * Loaders and util functions for dynamic race system.
 * Dec 24, 2003
 * Blue Lang, blue@b-side.org
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "utils.h"

/*
 * Load the race list.
 */
void load_races( FILE * fp )
{
    RACE_DATA *pRace;
    char *word;
    bool done = FALSE;
    bool fMatch;
    int count;
    // we have no easy way of reading in char arrays, so we fudge it
    int feeds_count = 0;
    int skills_count = 0;
   
	//FIXME: switch this to new_race
    pRace = alloc_perm(sizeof(*pRace));
    
    for (;;) {
        if (done)
            break;
            
        word = feof(fp) ? "$" : fread_word(fp);
        
        switch (UPPER(word[0])) {
            default:
                break;
                
            case '$':
                done = TRUE;
                break; 
                
            case 'A':
                KEY("Act", pRace->act, fread_flag(fp));
                KEY("Aff", pRace->aff, fread_flag(fp));
                break;
                
            case 'E':
                //clean up and get ready for the next one.. 
                if (!strcmp( word, "End" )) {
                
                    if (race_first == NULL)
                        race_first = pRace;
                        
                    if (race_last != NULL)
                        race_last->next = pRace;
                        
                    race_last = pRace;
                    pRace->next = NULL;
                    top_race++;
 
					printf( "name %s\r\n", pRace->name);
					printf(" who_name %s\r\n", pRace->who_name);
					printf(" gen_name %s\r\n", pRace->gen_name);
					printf(" pc_race %d\r\n", pRace->pc_race);

					//FIXME: switch this to new_race
                    pRace = alloc_perm(sizeof(*pRace));
                    break;
                }   
                break;
                
            case 'F':
                KEY("Form", pRace->form, fread_flag(fp));
                
                if (!strcmp( word, "Feeds" )) {
                    if ( feeds_count >= MAX_RACE_FEEDS ) {
                        printf("We have too many feeds saved.\r\n");
                        feeds_count = 0;
                    }   
                    
                    pRace->feeds[feeds_count] = str_dup(fread_word(fp));
                    feeds_count++;
                    break;
                }   
                
                break;
                
            case 'G':
                KEYS("Gen", pRace->gen_name, fread_string(fp));
                break;
                
            case 'I':
                KEY("Imm", pRace->imm, fread_flag(fp));
                break;
                
            case 'L':
                KEY("Lifespan", pRace->lifespan, fread_number(fp));
                break;
                
            case 'M':
                KEY("Max", pRace->max_level, fread_number(fp));
                KEY("Min", pRace->min_level, fread_number(fp));
                break;
            case 'N':
                KEYS("Name", pRace->name, fread_string(fp));
                break;

            case 'O':
                KEY("Off", pRace->off, fread_flag(fp));
                break;

            case 'P':
                KEY("Parts", pRace->parts, fread_flag(fp));
                KEY("PC", pRace->pc_race, fread_number(fp));
                KEY("Points", pRace->points, fread_number(fp));
                break;

            case 'R':
                KEY("Res", pRace->res, fread_flag(fp));
                break;

            case 'S':
                KEY("Sex", pRace->sex, fread_number(fp));
                KEY("Size", pRace->size, fread_number(fp));

                // we assume that stats get written in a row on a 
                // single line
                if (!strcmp( word, "Stats" )) {
                    for ( count = 0; count < MAX_STATS; count++) {
                        pRace->stats[count] = fread_number(fp);
                    }
                    break;
                }

                // we assume that senses get written in a row on a 
                // single line
                if (!strcmp( word, "Senses" )) {
                    for ( count = 0; count < MAX_SENSES; count++) {
                        pRace->senses[count] = fread_number(fp);
                    }
                    break;
                }

                if (!strcmp( word, "Skill" )) {
                    if ( skills_count >= MAX_RACE_SKILLS ) {
                        printf("We have too many racial skills saved.\r\n");
                        skills_count = 0;
                    }

                    pRace->skills[skills_count] = str_dup(fread_word(fp));
                    skills_count++;
                    break;
                }
                break;

            case 'V':
                KEY("Version", pRace->version, fread_number(fp));
                KEY("Vuln", pRace->vuln, fread_flag(fp));
                break;

            case 'W':
                KEYS("Who", pRace->who_name, fread_string(fp));
                break;

           } // switch (UPPER(word[0])) {

    } // per-race for loop 

    return;
}

/*
RACE_DATA *get_race_by_name(char *argument) {

	RACE_DATA *pRace;

    for (pRace = race_first; pRace != NULL; pRace = pRace->next) {
    	if (is_name(argument, pRace->name)) 
			return pRace;
	}

	return NULL;
}
*/
