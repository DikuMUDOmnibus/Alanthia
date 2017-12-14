#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "merc.h"
#include "utils.h"
#include "tables.h"
#include "lookup.h"

void persist_container( long vnum ) {

	char buf[MSL];
    FILE *fp;

    fclose(fpReserve);

	buf[0] = '\0';
	sprintf( buf, "containers/%ld", vnum);

    if ((fp = fopen( buf, "w")) == NULL) {
        bug("fopen: can't open container file", 0);
    	fpReserve = fopen(NULL_FILE, "r");
		return;
    }

	//fwrite_obj( NULL, obj, fp, 0 );

    fclose(fp);

    fpReserve = fopen(NULL_FILE, "r");
    return;
}

void load_container() {

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

