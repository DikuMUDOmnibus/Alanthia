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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdarg.h>

#include "merc.h"
#include "utils.h"
#include "db.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"
#include "olc.h"

int getpid();
time_t time(time_t * tloc);

extern int flag_lookup
args((const char *name, const struct flag_type * flag_table));
struct social_type social_table[MAX_SOCIALS];
int social_count;
long rnum = 0;


/*
 * externals for counting purposes 
 */
extern OBJ_DATA *obj_free;
extern CHAR_DATA *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern PC_DATA *pcdata_free;
extern AFFECT_DATA *affect_free;

void do_dump(CHAR_DATA * ch, char *argument)
{
    int i, lcount, count, count2, num_pcs, aff_count;
    CHAR_DATA *fch;
    CHAR_DATA *victim;
    MOB_INDEX_DATA *pMobIndex;
    PC_DATA *pc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *room;
    EXIT_DATA *exit;
    DESCRIPTOR_DATA *d;
    AFFECT_DATA *af;
    FILE *fp;
    long vnum, nMatch = 0;

    /*
     * open file 
     */
    fclose(fpReserve);
    fp = fopen("../../public_html/mem.dmp", "w");

    /*
     * report use of data structures 
     */

    num_pcs = 0;
    aff_count = 0;

    /*
     * mobile prototypes 
     */
    fprintf(fp, "MobProt	%4ld (%8ld bytes)\n",
	    top_mob_index, top_mob_index * (sizeof(*pMobIndex)));

    /*
     * mobs 
     */
    count = 0;
    count2 = 0;
    for (fch = char_list; fch != NULL; fch = fch->next) {
	count++;
	if (fch->pcdata != NULL)
	    num_pcs++;
	for (af = fch->affected; af != NULL; af = af->next)
	    aff_count++;
    }
    for (fch = char_free; fch != NULL; fch = fch->next)
	count2++;

    fprintf(fp, "Mobs	%4d (%8d bytes), %2d free (%d bytes)\n",
	    count, count * (sizeof(*fch)), count2,
	    count2 * (sizeof(*fch)));

    /*
     * pcdata 
     */
    count = 0;
    for (pc = pcdata_free; pc != NULL; pc = pc->next)
	count++;

    fprintf(fp, "Pcdata	%4d (%8d bytes), %2d free (%d bytes)\n",
	    num_pcs, num_pcs * (sizeof(*pc)), count,
	    count * (sizeof(*pc)));

    /*
     * descriptors 
     */
    count = 0;
    count2 = 0;
    for (d = descriptor_list; d != NULL; d = d->next)
	count++;
    for (d = descriptor_free; d != NULL; d = d->next)
	count2++;

    fprintf(fp, "Descs	%4d (%8d bytes), %2d free (%d bytes)\n",
	    count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));

    /*
     * object prototypes 
     */
    for (vnum = 0; nMatch < top_obj_index; vnum++)
	if ((pObjIndex = get_obj_index(vnum)) != NULL) {
	    for (af = pObjIndex->affected; af != NULL; af = af->next)
		aff_count++;
	    nMatch++;
	}
    fprintf(fp, "ObjProt	%4ld (%8ld bytes)\n",
	    top_obj_index, top_obj_index * (sizeof(*pObjIndex)));


    /*
     * objects 
     */
    count = 0;
    count2 = 0;
    for (obj = object_list; obj != NULL; obj = obj->next) {
	count++;
		for (af = obj->affected; af != NULL; af = af->next)
	    	aff_count++;
    	}
    	for (obj = obj_free; obj != NULL; obj = obj->next)
		count2++;

    fprintf(fp, "Objs	%4d (%8d bytes), %2d free (%d bytes)\n",
	    count, count * (sizeof(*obj)), count2,
	    count2 * (sizeof(*obj)));

    /*
     * affects 
     */
    count = 0;
    for (af = affect_free; af != NULL; af = af->next)
	count++;

    fprintf(fp, "Affects	%4d (%8d bytes), %2d free (%d bytes)\n",
	    aff_count, aff_count * (sizeof(*af)), count,
	    count * (sizeof(*af)));

    /*
     * rooms 
     */
    fprintf(fp, "Rooms	%4d (%8d bytes)\n",
	    top_room, top_room * (sizeof(*room)));

    /*
     * exits 
     */
    fprintf(fp, "Exits	%4d (%8d bytes)\n",
	    top_exit, top_exit * (sizeof(*exit)));

    fclose(fp);

    /*
     * start printing out mobile data 
     */
    fp = fopen("../../public_html/mob.html", "w");
    fprintf(fp, "\n<HTML><BODY BGCOLOR=WHITE><PRE>\n");
    fprintf(fp, "\nMobile Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;


    for (lcount = 300; lcount > -1; lcount--) {
	for (victim = char_list; victim != NULL; victim = victim->next) {
	    if (victim->in_room != NULL && victim->level == lcount) {
		count++;
		fprintf(fp,
			"Num: %d) Vnum: %-5ld\n\tName: %s\n\tLevel: %d\n\tGold: %ld\n",
			count,
			IS_NPC(victim) ? victim->pIndexData->vnum : 0,
			IS_NPC(victim) ? victim->short_descr : victim->
			name, victim->level, victim->gold);
		fprintf(fp,
			"\tHP %5ld MN %5ld \n\tHR %3d DR %3d \n\tDAM: %dd%d\n\n",
			victim->max_hit, victim->max_mana, victim->hitroll,
			victim->damroll, victim->damage[DICE_NUMBER],
			victim->damage[DICE_TYPE]);
		fprintf(fp, "Act Flags:\n");
		fprintf(fp, "\t%s\n", flag_string(act_flags, victim->act));
		fprintf(fp, "Aff Flags:\n");
		fprintf(fp, "\t%s\n",
			flag_string(affect_flags, victim->affected_by));
		fprintf(fp, "Imm Flags:\n");
		fprintf(fp, "\t%s\n",
			flag_string(imm_flags, victim->imm_flags));
		fprintf(fp, "Res Flags:\n");
		fprintf(fp, "\t%s\n",
			flag_string(res_flags, victim->res_flags));
		fprintf(fp, "Vuln Flags:\n");
		fprintf(fp, "\t%s\n",
			flag_string(vuln_flags, victim->vuln_flags));
		fprintf(fp, "Off Flags:\n");
		fprintf(fp, "\t%s\n",
			flag_string(off_flags, victim->off_flags));
		fprintf(fp, "Partses:\n");
		fprintf(fp, "\t%s\n",
			flag_string(part_flags, victim->parts));
		fprintf(fp, "Form:\n");
		fprintf(fp, "\t%s\n\n",
			flag_string(form_flags, victim->form));
		fprintf(fp,
			"----------------------------------------------------\n\n");

	    }
	}
    }
    fprintf(fp, "</PRE></BODY></HTML>");
    fclose(fp);

    /*
     * start printing out object data 
     */
    fp = fopen("../../public_html/obj-weight-by-level.dmp", "w");

    fprintf(fp, "\nObject Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;
	for ( i=1; i < 310; i++) {
		nMatch = 0;
    	for (vnum = 0; nMatch < top_obj_index; vnum++) {
			if ((pObjIndex = get_obj_index(vnum)) != NULL) {
	    		nMatch++;
				if (pObjIndex->level == i) {
	    		fprintf(fp, "vnum: %-4ld %-40s level: %-6d weight: %-6d\n\n",
		    	pObjIndex->vnum, pObjIndex->short_descr,
		    	pObjIndex->level, pObjIndex->weight);
				}
			}
		}
	}
    /*
     * close file 
     */
    fclose(fp);

	/*
     * Objects by level with avg damn
     */

    fp = fopen("../../public_html/weapons-by-level.dmp", "w");

    fprintf(fp, "\nObject Analysis\n");
    fprintf(fp, "---------------\n");
    nMatch = 0;
	for ( i=1; i < 310; i++) {
		nMatch = 0;
    	for (vnum = 0; nMatch < top_obj_index; vnum++) {
			if ((pObjIndex = get_obj_index(vnum)) != NULL) {
	    		nMatch++;
				if ((pObjIndex->level == i) 
						&& (pObjIndex->item_type == ITEM_WEAPON)) {
	    		fprintf(fp, 
					"vnum: %-4ld %-40s level: %-6d weight: %-6d avg dam: %d\n",
		    	pObjIndex->vnum, pObjIndex->short_descr,
		    	pObjIndex->level, pObjIndex->weight,
        		(1 + pObjIndex->value[2]) * pObjIndex->value[1] / 2);
				}
			}
		}
	}
    /*
     * close file 
     */
    fclose(fp);
/*
    fp = fopen("../../public_html/races.dmp", "w");

    fprintf(fp, "\n<HTML>\n");
    fprintf(fp, "<PRE>\n");
    fprintf(fp, "\nRacial Analysis\n");

    for (race = 0; race_table[race].name != NULL; race++) {
	fprintf(fp, "\n----------------------------------------\n");

	fprintf(fp, "\nName\t%s\n", race_table[race].name);
	if (race_table[race].pc_race)
	    fprintf(fp, "Player race.\n");

	fprintf(fp, "Act Flags:\n");
	fprintf(fp, "%s\n", flag_string(act_flags, race_table[race].act));
	fprintf(fp, "Aff Flags:\n");
	fprintf(fp, "%s\n",
		flag_string(affect_flags, race_table[race].aff));
	fprintf(fp, "Imm Flags:\n");
	fprintf(fp, "%s\n", flag_string(imm_flags, race_table[race].imm));
	fprintf(fp, "Res Flags:\n");
	fprintf(fp, "%s\n", flag_string(res_flags, race_table[race].res));
	fprintf(fp, "Vuln Flags:\n");
	fprintf(fp, "%s\n",
		flag_string(vuln_flags, race_table[race].vuln));
	fprintf(fp, "Off Flags:\n");
	fprintf(fp, "%s\n", flag_string(off_flags, race_table[race].off));
	fprintf(fp, "Partses:\n");
	fprintf(fp, "%s\n",
		flag_string(part_flags, race_table[race].parts));
	fprintf(fp, "Form:\n");
	fprintf(fp, "%s\n",
		flag_string(form_flags, race_table[race].form));


	fprintf(fp, "\n");

    }
    fprintf(fp, "</PRE>\n");
    fprintf(fp, "</HTML>\n");
*/
    fclose(fp);

    fpReserve = fopen(NULL_FILE, "r");
}
