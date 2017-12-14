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
#include "lookup.h"

char *turf_name( int turf ) {

	static char name[MSL];

	name[0] = '\0';

	if ( turf < 5 )
		sprintf( name, "beginning to be" );
	else if ( turf < 10 )
        sprintf( name, "barely" );
    else if ( turf < 20 )
        sprintf( name, "moderately" );
    else if ( turf < 50 )
        sprintf( name, "well" );
    else 
        sprintf( name, "completely" );
    
	return name;

}

void do_turf( CHAR_DATA *ch, char *argument ) {

	char buf[MSL];

	sprintf( buf, "This room is %s turfed by %s.\r\n",
		turf_name( ch->in_room->turf_level ),
		clan_table[ch->in_room->turf_clan].name);
	send_to_char( buf, ch );

	return;

}

void turf_mod( CHAR_DATA *ch, CHAR_DATA *victim) {

    if (!IS_NPC(ch) && (ch->clan)) {
        if (!ch->in_room->turf_level) {
            ch->in_room->turf_clan = ch->clan;
          send_to_char("{yYour clan has taken over control of this room.{x\r\n",
                ch); 
        }       
        
        if (ch->in_room->turf_clan == ch->clan) {
            if (ch->in_room->turf_level < 100 )
                ch->in_room->turf_level++;
        } else {
            ch->in_room->turf_level--;
        }
    }
    
    return;
}

void turf_save() {

	ROOM_INDEX_DATA *pRoomIndex;
	AREA_DATA *pArea;
	int iHash;
	bool fWrote = FALSE;
    FILE *fp;

    fclose(fpReserve);

	if ((fp = fopen("turf.dat", "w")) == NULL) {
        bug("turf_save: turf.dat: fopen", 0);
    }

	// seems like a lot of work to check for some rooms.. 
	for (pArea = area_first; pArea; pArea = pArea->next) {
		for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
    		for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
         		pRoomIndex = pRoomIndex->next) {
        		if (pRoomIndex->area == pArea 
					&& !pRoomIndex->deleted
					&& pRoomIndex->turf_level) {
        			fprintf(fp, "%ld %s %d\n", 
						pRoomIndex->vnum, 
						clan_table[pRoomIndex->turf_clan].name,
						pRoomIndex->turf_level);
						pRoomIndex->turf_level--;
						fWrote = TRUE;
				}
			}
		}

   	}
  
    fclose(fp);

	// turf_load() handles not having a file, but doesn't like empty files
	if (!fWrote)
		unlink("turf.dat");

    fpReserve = fopen(NULL_FILE, "r");
	return;
}


void turf_load() {

	ROOM_INDEX_DATA *pRoomIndex;
    FILE *fp;
    
    fclose(fpReserve);
    
    if ((fp = fopen("turf.dat", "r")) == NULL) {
        bug("turf_load: turf.dat: fopen", 0);
    	fpReserve = fopen(NULL_FILE, "r");
		return;
    }   
    
	for (;;) {

		if (feof(fp))
			break;
	
		pRoomIndex = get_room_index(fread_number(fp));
		if ( pRoomIndex != NULL ) {
		pRoomIndex->turf_clan = clan_lookup(fread_word(fp));
		pRoomIndex->turf_level = fread_number(fp);

		if (pRoomIndex->turf_level > 100)
			pRoomIndex->turf_level = 100;
		}
		fread_to_eol(fp);	

	}

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");
	return;
}
