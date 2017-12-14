



/*************************************************************************
 *  File: olc_save.c                                                       *
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
/* OLC_SAVE.C
 * This takes care of saving all the .are information.
 * Notes:
 * -If a good syntax checker is used for setting vnum ranges of areas
 *  then it would become possible to just cycle through vnums instead
 *  of using the iHash stuff and checking that the room or reset or
 *  mob etc is part of that area.
 */

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

/*
 *  Verbose writes reset data in plain english into the comments
 *  section of the resets.  It makes areas considerably larger but
 *  may aid in debugging.
 */

/* #define VERBOSE */

/*****************************************************************************
 Name:          find_mprog_type
 Purpose:       Returns the name of a trigger type
 ****************************************************************************/
char *find_mprog_type(int type)
{
    switch (type) {
    case TRIG_ACT:
	return "ACT";
    case TRIG_SPEECH:
	return "SPEECH";
    case TRIG_RANDOM:
	return "RANDOM";
    case TRIG_FIGHT:
	return "FIGHT";
    case TRIG_HPCNT:
	return "HPCNT";
    case TRIG_DEATH:
	return "DEATH";
    case TRIG_ENTRY:
	return "ENTRY";
    case TRIG_GREET:
	return "GREET";
    case TRIG_GRALL:
	return "GRALL";
    case TRIG_GIVE:
	return "GIVE";
    case TRIG_BRIBE:
	return "BRIBE";
    case TRIG_KILL:
	return "KILL";
    case TRIG_DELAY:
	return "DELAY";
    case TRIG_SURR:
	return "SURRENDER";
    case TRIG_EXIT:
	return "EXIT";
    case TRIG_EXALL:
	return "EXALL";
    default:
	return "ERROR";
    }
}

/*****************************************************************************
 Name:		fix_string
 Purpose:	Returns a string without \r and ~.
 ****************************************************************************/
char *fix_string(const char *str)
{
    static char strfix[MAX_STRING_LENGTH];
    int i;
    int o;

    if (str == NULL)
	return '\0';

    for (o = i = 0; i == 0 || str[i + o - 1] != '\0'; i++) {
	while (str[i + o] == '\r' || str[i + o] == '~')
	    o++;
	strfix[i] = str[i + o];
    }
    strfix[i] = '\0';
    return strfix;
}




/*****************************************************************************
 Name:		save_area_list
 Purpose:	Saves the listing of files to be loaded at startup.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area_list()
{
    FILE *fp;
    AREA_DATA *pArea;

    if ((fp = fopen("area.lst", "w")) == NULL) {
	bug("Save_area_list: fopen", 0);
	perror("area.lst");
    } else {
	/*
	 * Add any help files that need to be loaded at
	 * startup to this section.
	 */
	fprintf(fp, "help.are\n");
	fprintf(fp, "mobprog.prg\n");
	fprintf(fp, "social.are\n");

	for (pArea = area_first; pArea; pArea = pArea->next) {
	    fprintf(fp, "%s\n", pArea->file_name);
	}

	fprintf(fp, "$\n");
	fclose(fp);
    }

    return;
}


/*
 * ROM OLC
 * Used in save_mobile and save_object below.  Writes
 * flags on the form fread_flag reads.
 * 
 * buf[] must hold at least 32+1 characters.
 *
 * -- Hugin
 */
char *fwrite_flag(long flags, char buf[])
{
    char offset;
    char *cp;

    buf[0] = '\0';

    if (flags == 0) {
	strcpy(buf, "0");
	return buf;
    }
    /* 32 -- number of bits in a long */

    for (offset = 0, cp = buf; offset < 32; offset++)
	if (flags & ((long) 1 << offset)) {
	    if (offset <= 'Z' - 'A')
		*(cp++) = 'A' + offset;
	    else
		*(cp++) = 'a' + offset - ('Z' - 'A' + 1);
	}
    *cp = '\0';

    return buf;
}

/*****************************************************************************
 Name:		save_race
 Purpose:	Save one race entry to the race file.
 Called by:	save_races (below).
 ****************************************************************************/
void save_race(FILE * fp, RACE_DATA *pRace)
{
	int value;
    char buf[MAX_STRING_LENGTH];
	
	fprintf( fp,"Name %s~\n", fix_string(pRace->name) );
	fprintf( fp,"Who %s~\n", fix_string(pRace->who_name) );
	fprintf( fp,"Gen %s~\n", fix_string(pRace->gen_name) );
	fprintf( fp,"PC %d\n",  pRace->pc_race );
	fprintf( fp,"Act %s\n", fwrite_flag(pRace->act, buf) );
	fprintf( fp,"Aff %s\n", fwrite_flag(pRace->aff, buf) );
	fprintf( fp,"Off %s\n", fwrite_flag(pRace->off, buf) );
	fprintf( fp,"Imm %s\n", fwrite_flag(pRace->imm, buf) );
	fprintf( fp,"Res %s\n", fwrite_flag(pRace->res, buf) );
	fprintf( fp,"Vuln %s\n", fwrite_flag(pRace->vuln, buf) );
	fprintf( fp,"Form %s\n", fwrite_flag(pRace->form, buf) );
	fprintf( fp,"Parts %s\n", fwrite_flag(pRace->parts, buf) );
	fprintf( fp,"Sex %d\n", pRace->sex );
	fprintf( fp,"Size %d\n", pRace->size );
	fprintf( fp,"Min %d\n", pRace->min_level );
	fprintf( fp,"Max %d\n", pRace->max_level );
	fprintf( fp,"Version %d\n", pRace->version );
	fprintf( fp,"Lifespan %d\n", pRace->lifespan );
	fprintf( fp,"Points %d\n", pRace->points );
	
	fprintf( fp,"Stats ");
    for (value = 0; value < MAX_STATS; value++)
        fprintf( fp, "%d ", pRace->stats[value]);

	fprintf( fp,"\nSenses ");

    for (value = 0; value < MAX_SENSES; value++)
        fprintf( fp, "%d ", pRace->senses[value]);
	
	fprintf( fp,"\n");

    for (value = 0; value < MAX_RACE_SKILLS; value++)
        if ( pRace->skills[value] != NULL )
			fprintf( fp, "Skill '%s'\n", pRace->skills[value] );  
		else
			continue;

    for (value = 0; value < MAX_RACE_FEEDS; value++)
        fprintf( fp, "Feeds '%s'\n" , pRace->feeds[value] != NULL 
				?  pRace->feeds[value] 
				: "" );

	fprintf( fp,"End\n");
    return;

}

/*****************************************************************************
 Name:      save_races
 Purpose:   Save all the races, Noah's Arc style.
 Called by: asave and whatnot.
 ****************************************************************************/
void save_races() 
{
	RACE_DATA *pRace;
	FILE *fp;

    if ((fp = fopen(RACE_LIST, "w")) == NULL) {
     	perror(RACE_LIST);
     	exit(1);
	}

	for (pRace = race_first; pRace != NULL; pRace = pRace->next) {
		if ( !pRace->deleted )
			save_race( fp, pRace );
	}

	fprintf( fp, "$\n" );
	fclose( fp );

	return;
}


/*****************************************************************************
 Name:		save_mobile
 Purpose:	Save one mobile to file, new format -- Hugin
 Called by:	save_mobiles (below).
 ****************************************************************************/
void save_mobile(FILE * fp, MOB_INDEX_DATA * pMobIndex)
{
    MPROG_LIST *pMprog;
    char buf[MAX_STRING_LENGTH];

    fprintf(fp, "#%ld\n", pMobIndex->vnum);
    fprintf(fp, "%s~\n", pMobIndex->player_name);
    fprintf(fp, "%s~\n", pMobIndex->short_descr);
    fprintf(fp, "%s~\n", fix_string(pMobIndex->long_descr));
    fprintf(fp, "%s~\n", fix_string(pMobIndex->description));
    fprintf(fp, "%s~\n", pMobIndex->race->name);
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->act, buf));
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->affected_by, buf));
    fprintf(fp, "%d %d\n", pMobIndex->alignment, pMobIndex->group);
    fprintf(fp, "%d ", pMobIndex->level);
    fprintf(fp, "%d ", pMobIndex->hitroll);
    fprintf(fp, "%dd%d+%d ", pMobIndex->hit[DICE_NUMBER],
	    pMobIndex->hit[DICE_TYPE], pMobIndex->hit[DICE_BONUS]);
    fprintf(fp, "%dd%d+%d ", pMobIndex->mana[DICE_NUMBER],
	    pMobIndex->mana[DICE_TYPE], pMobIndex->mana[DICE_BONUS]);
    fprintf(fp, "%dd%d+%d ", pMobIndex->damage[DICE_NUMBER],
	    pMobIndex->damage[DICE_TYPE], pMobIndex->damage[DICE_BONUS]);
    fprintf(fp, "%s\n", attack_table[pMobIndex->dam_type].name);
    fprintf(fp, "%d %d %d %d\n", pMobIndex->ac[AC_PIERCE] / 10,
	    pMobIndex->ac[AC_BASH] / 10,
	    pMobIndex->ac[AC_SLASH] / 10, pMobIndex->ac[AC_EXOTIC] / 10);
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->off_flags, buf));
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->imm_flags, buf));
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->res_flags, buf));
    fprintf(fp, "%s\n", fwrite_flag(pMobIndex->vuln_flags, buf));
    fprintf(fp, "%s %s %s %ld\n",
	    position_table[pMobIndex->start_pos].short_name,
	    position_table[pMobIndex->default_pos].short_name,
	    sex_table[pMobIndex->sex].name, pMobIndex->wealth);
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->form, buf));
    fprintf(fp, "%s ", fwrite_flag(pMobIndex->parts, buf));

    fprintf(fp, "%s ", size_table[pMobIndex->size].name);
    fprintf(fp, "%s\n",
	    ((pMobIndex->material[0] != '\0') ? pMobIndex->
	     material : "unknown"));

    for (pMprog = pMobIndex->mprogs; pMprog; pMprog = pMprog->next) {
	fprintf(fp, "M %s", mprog_type_name(pMprog->trig_type));
	fprintf(fp, "%ld ", pMprog->vnum);
	fprintf(fp, "%s~\n", pMprog->trig_phrase);
    }

    return;
}


/*****************************************************************************
 Name:		save_mobiles
 Purpose:	Save #MOBILES secion of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_mobiles(FILE * fp, AREA_DATA * pArea)
{
    long i;
    MOB_INDEX_DATA *pMob;

    fprintf(fp, "#MOBILES\n");

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
	if ((pMob = get_mob_index(i)))
	    if (!pMob->deleted)
		save_mobile(fp, pMob);
    }

    fprintf(fp, "#0\n\n\n\n");
    return;
}

/*****************************************************************************
 Name:		save_object
 Purpose:	Save one object to file.
                new ROM format saving -- Hugin
 Called by:	save_objects (below).
 ****************************************************************************/
void save_object(FILE * fp, OBJ_INDEX_DATA * pObjIndex)
{
    char letter;
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;
    char buf[MAX_STRING_LENGTH];

    fprintf(fp, "#%ld\n", pObjIndex->vnum);
    fprintf(fp, "%s~\n", pObjIndex->name);
    fprintf(fp, "%s~\n", pObjIndex->short_descr);
    fprintf(fp, "%s~\n", fix_string(pObjIndex->description));
    fprintf(fp, "%s~\n", pObjIndex->material);
    fprintf(fp, "%s ", item_name(pObjIndex->item_type));
    fprintf(fp, "%s ", fwrite_flag(pObjIndex->extra_flags, buf));
    fprintf(fp, "%s\n", fwrite_flag(pObjIndex->wear_flags, buf));

/*
 *  Using fwrite_flag to write most values gives a strange
 *  looking area file, consider making a case for each
 *  item type later.
 */

    switch (pObjIndex->item_type) {
    default:
	fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[0], buf));
	fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[1], buf));
	fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[2], buf));
	fprintf(fp, "%s ", fwrite_flag(pObjIndex->value[3], buf));
	fprintf(fp, "%s\n", fwrite_flag(pObjIndex->value[4], buf));
	break;

    case ITEM_LIGHT:
	fprintf(fp, "0 0 %d 0 0\n", pObjIndex->value[2] < 1 ? 999	/* infinite */
		: pObjIndex->value[2]);
	break;

    case ITEM_MONEY:
	fprintf(fp, "%d %d 0 0 0\n",
		pObjIndex->value[0], pObjIndex->value[1]);
	break;

    case ITEM_DRINK_CON:
	fprintf(fp, "%d %d '%s' %d 0\n",
		pObjIndex->value[0],
		pObjIndex->value[1],
		liq_table[pObjIndex->value[2]].liq_name,
		pObjIndex->value[3]);
	break;

    case ITEM_BLOOD_CON:
	fprintf(fp, "%d %d '%s' %d 0\n",
		pObjIndex->value[0],
		pObjIndex->value[1],
		liq_table[pObjIndex->value[2]].liq_name,
		pObjIndex->value[3]);
	break;

    case ITEM_FOUNTAIN:
	fprintf(fp, "%d %d '%s' 0 0\n",
		pObjIndex->value[0],
		pObjIndex->value[1],
		liq_table[pObjIndex->value[2]].liq_name);
	break;

    case ITEM_CONTAINER:
	fprintf(fp, "%d %s %d %d %d\n",
		pObjIndex->value[0],
		fwrite_flag(pObjIndex->value[1], buf),
		pObjIndex->value[2],
		pObjIndex->value[3], pObjIndex->value[4]);
	break;

    case ITEM_FOOD:
	fprintf(fp, "%d %d 0 %s 0\n",
		pObjIndex->value[0],
		pObjIndex->value[1],
		fwrite_flag(pObjIndex->value[3], buf));
	break;

    case ITEM_PORTAL:
	fprintf(fp, "%d %s %s %d 0\n",
		pObjIndex->value[0],
		fwrite_flag(pObjIndex->value[1], buf),
		fwrite_flag(pObjIndex->value[2], buf),
		pObjIndex->value[3]);
	break;

    case ITEM_FURNITURE:
	fprintf(fp, "%d %d %s %d %d\n",
		pObjIndex->value[0],
		pObjIndex->value[1],
		fwrite_flag(pObjIndex->value[2], buf),
		pObjIndex->value[3], pObjIndex->value[4]);
	break;

    case ITEM_WEAPON:
	fprintf(fp, "%s %d %d %s %s\n",
		weapon_name(pObjIndex->value[0]),
		pObjIndex->value[1],
		pObjIndex->value[2],
		attack_table[pObjIndex->value[3]].name,
		fwrite_flag(pObjIndex->value[4], buf));
	break;

    case ITEM_ARMOR:
	fprintf(fp, "%d %d %d %d %d\n",
		pObjIndex->value[0],
		pObjIndex->value[1],
		pObjIndex->value[2],
		pObjIndex->value[3], pObjIndex->value[4]);
	break;

    case ITEM_PILL:
    case ITEM_POTION:
    case ITEM_SCROLL:
	fprintf(fp, "%d '%s' '%s' '%s' '%s'\n", pObjIndex->value[0] > 0 ?
		pObjIndex->value[0]
		: 0,
		pObjIndex->value[1] != -1 ?
		skill_table[pObjIndex->value[1]].name
		: "",
		pObjIndex->value[2] != -1 ?
		skill_table[pObjIndex->value[2]].name
		: "",
		pObjIndex->value[3] != -1 ?
		skill_table[pObjIndex->value[3]].name
		: "",
		pObjIndex->value[4] != -1 ?
		skill_table[pObjIndex->value[4]].name : "");
	break;

    case ITEM_STAFF:
    case ITEM_WAND:
	fprintf(fp, "%d ", pObjIndex->value[0]);
	fprintf(fp, "%d ", pObjIndex->value[1]);
	fprintf(fp, "%d '%s' 0\n",
		pObjIndex->value[2],
		pObjIndex->value[3] != -1 ?
		skill_table[pObjIndex->value[3]].name : 0);
	break;
    }

    fprintf(fp, "%d ", pObjIndex->level);
    fprintf(fp, "%d ", pObjIndex->weight);
    fprintf(fp, "%d ", pObjIndex->cost);
    fprintf(fp, "%d ", pObjIndex->timer);

    if (pObjIndex->condition > 90)
	letter = 'P';
    else if (pObjIndex->condition > 75)
	letter = 'G';
    else if (pObjIndex->condition > 50)
	letter = 'A';
    else if (pObjIndex->condition > 25)
	letter = 'W';
    else if (pObjIndex->condition > 10)
	letter = 'D';
    else if (pObjIndex->condition > 0)
	letter = 'B';
    else
	letter = 'R';

    fprintf(fp, "%c\n", letter);

    for (pAf = pObjIndex->affected; pAf; pAf = pAf->next) {
	fprintf(fp, "A\n%d %d\n", pAf->location, pAf->modifier);
    }

    for (pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next) {
	fprintf(fp, "E\n%s~\n%s~\n", pEd->keyword,
		fix_string(pEd->description));
    }

    return;
}




/*****************************************************************************
 Name:		save_objects
 Purpose:	Save #OBJECTS section of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_objects(FILE * fp, AREA_DATA * pArea)
{
    long i;
    OBJ_INDEX_DATA *pObj;

    fprintf(fp, "#OBJECTS\n");

    for (i = pArea->min_vnum; i <= pArea->max_vnum; i++) {
	if ((pObj = get_obj_index(i)))
		if( !pObj->deleted )
			save_object(fp, pObj);
    }

    fprintf(fp, "#0\n\n\n\n");
    return;
}



/*****************************************************************************
 Name:		save_rooms
 Purpose:	Save #ROOMS section of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_rooms(FILE * fp, AREA_DATA * pArea)
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA *pExit;
    int iHash;
    int door;

    fprintf(fp, "#ROOMS\n");
    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
	     pRoomIndex = pRoomIndex->next) {
	    if (pRoomIndex->area == pArea && !pRoomIndex->deleted) {
		fprintf(fp, "#%ld\n", pRoomIndex->vnum);
		fprintf(fp, "%s~\n", pRoomIndex->name);
		fprintf(fp, "%s~\n", fix_string(pRoomIndex->description));
		fprintf(fp, "0 ");
		fprintf(fp, "%d ", pRoomIndex->room_flags);
		fprintf(fp, "%d\n", pRoomIndex->sector_type);

		for (pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next) {
		    fprintf(fp, "E\n%s~\n%s~\n", pEd->keyword,
			    fix_string(pEd->description));
		}
		for (door = 0; door < MAX_DIR; door++) {	/* I hate this! */
		    if ((pExit = pRoomIndex->exit[door])
			&& pExit->u1.to_room
			&& !pExit->u1.to_room->deleted) {
			int locks = 0;
			if (IS_SET(pExit->rs_flags, EX_ISDOOR)
			    && (!IS_SET(pExit->rs_flags, EX_PICKPROOF))
			    && (!IS_SET(pExit->rs_flags, EX_NOPASS)))
			    locks = 1;
			if (IS_SET(pExit->rs_flags, EX_ISDOOR)
			    && (IS_SET(pExit->rs_flags, EX_PICKPROOF))
			    && (!IS_SET(pExit->rs_flags, EX_NOPASS)))
			    locks = 2;
			if (IS_SET(pExit->rs_flags, EX_ISDOOR)
			    && (!IS_SET(pExit->rs_flags, EX_PICKPROOF))
			    && (IS_SET(pExit->rs_flags, EX_NOPASS)))
			    locks = 3;
			if (IS_SET(pExit->rs_flags, EX_ISDOOR)
			    && (IS_SET(pExit->rs_flags, EX_PICKPROOF))
			    && (IS_SET(pExit->rs_flags, EX_NOPASS)))
			    locks = 4;
			if (IS_SET(pExit->rs_flags, EX_ISDOOR)
			    && (IS_SET(pExit->rs_flags, EX_HIDDEN)))
			    locks = 5;

			fprintf(fp, "D%d\n", pExit->orig_door);
			fprintf(fp, "%s~\n",
				fix_string(pExit->description));
			fprintf(fp, "%s~\n", pExit->keyword);
			fprintf(fp, "%d %d %ld\n", locks,
				pExit->key, pExit->u1.to_room->vnum);
		    }
		}
		if (pRoomIndex->mana_rate != 100
		    || pRoomIndex->heal_rate != 100)
		    fprintf(fp, "M %d H %d\n", pRoomIndex->mana_rate,
			    pRoomIndex->heal_rate);
		if (pRoomIndex->clan > 0)
		    fprintf(fp, "C %s~\n",
			    clan_table[pRoomIndex->clan].name);

		fprintf(fp, "S\n");
	    }
	}
    }
    fprintf(fp, "#0\n\n\n\n");
    return;
}

/*****************************************************************************
 Name:		save_specials
 Purpose:	Save #SPECIALS section of area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_specials(FILE * fp, AREA_DATA * pArea)
{
    int iHash;
    MOB_INDEX_DATA *pMobIndex;

    fprintf(fp, "#SPECIALS\n");

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	for (pMobIndex = mob_index_hash[iHash]; pMobIndex;
	     pMobIndex = pMobIndex->next) {
	    if (pMobIndex && pMobIndex->area == pArea
		&& pMobIndex->spec_fun && !pMobIndex->deleted) {
#if defined( VERBOSE )
		fprintf(fp, "M %d %s Load to: %s\n", pMobIndex->vnum,
			spec_name(pMobIndex->spec_fun),
			pMobIndex->short_descr);
#else
		fprintf(fp, "M %ld %s\n", pMobIndex->vnum,
			spec_name(pMobIndex->spec_fun));
#endif
	    }
	}
    }

    fprintf(fp, "S\n\n\n\n");
    return;
}



/*
 * This function is obsolete.  It it not needed but has been left here
 * for historical reasons.  It is used currently for the same reason.
 *
 * I don't think it's obsolete in ROM -- Hugin.
 */
void save_door_resets(FILE * fp, AREA_DATA * pArea)
{
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pExit;
    int door;

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
	     pRoomIndex = pRoomIndex->next) {
	    if (pRoomIndex->area == pArea && !pRoomIndex->deleted) {
		for (door = 0; door < MAX_DIR; door++) {
		    if ((pExit = pRoomIndex->exit[door])
			&& pExit->u1.to_room
			&& !pExit->u1.to_room->deleted
			&& (IS_SET(pExit->rs_flags, EX_CLOSED)
			    || IS_SET(pExit->rs_flags, EX_LOCKED)))
#if defined( VERBOSE )
			fprintf(fp,
				"D 0 %d %d %d The %s door of %s is %s\n",
				pRoomIndex->vnum, pExit->orig_door,
				IS_SET(pExit->rs_flags, EX_LOCKED) ? 2 : 1,
				dir_name[pExit->orig_door],
				pRoomIndex->name, IS_SET(pExit->rs_flags,
							 EX_LOCKED) ?
				"closed and locked" : "closed");
#endif
#if !defined( VERBOSE )
		    fprintf(fp, "D 0 %ld %d %d\n",
			    pRoomIndex->vnum,
			    pExit->orig_door,
			    IS_SET(pExit->rs_flags, EX_LOCKED) ? 2 : 1);
#endif
		}
	    }
	}
    }
    return;
}




/*****************************************************************************
 Name:		save_resets
 Purpose:	Saves the #RESETS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_resets(FILE * fp, AREA_DATA * pArea)
{
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj;
    ROOM_INDEX_DATA *pRoom;
    char buf[MAX_STRING_LENGTH];
    int iHash;

    fprintf(fp, "#RESETS\n");

    save_door_resets(fp, pArea);

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	for (pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next) {
	    if (pRoom->area == pArea && !pRoom->deleted) {
		for (pReset = pRoom->reset_first; pReset;
		     pReset = pReset->next) {
		    switch (pReset->command) {
		    default:
			bug("Save_resets: bad command %c.",
			    pReset->command);
			break;

		    case 'M':
			pLastMob = get_mob_index(pReset->arg1);
			if (!pLastMob->deleted) {
			    pRoom = get_room_index(pReset->arg3);
			    if (!pRoom->deleted) {
				fprintf(fp, "M 0 %d %d %d %d\n",
					pReset->arg1,
					pReset->arg2,
					pReset->arg3, pReset->arg4);
			    }
			}
			break;


		    case 'O':
			pLastObj = get_obj_index(pReset->arg1);
			if (!pLastObj->deleted) {
			  pRoom = get_room_index(pReset->arg3);
			    if (!pRoom->deleted) {
				fprintf(fp, "O 0 %d 0 %d\n",
					pReset->arg1, pReset->arg3);
			}
			}
			break;

		    case 'P':
			pLastObj = get_obj_index(pReset->arg1);
			if (!pLastObj->deleted) {
			    pLastObj = get_obj_index(pReset->arg3);
			    if (!pLastObj->deleted) {
				fprintf(fp, "P 0 %d %d %d %d\n",
					pReset->arg1,
					pReset->arg2,
					pReset->arg3, pReset->arg4);
			    }
			}
			break;

		    case 'G':	/* bad mobile handled at load time */
			pLastObj = get_obj_index(pReset->arg1);
			if (!pLastObj->deleted) {
			    fprintf(fp, "G 0 %d 0\n", pReset->arg1);
			    if (!pLastMob) {
				sprintf(buf,
					"Save_resets: !NO_MOB! in [%s]",
					pArea->file_name);
				bug(buf, 0);
			    }
			}
			break;

		    case 'E':
			pLastObj = get_obj_index(pReset->arg1);
			if (!pLastObj->deleted) {
			    fprintf(fp, "E 0 %d 0 %d\n",
				    pReset->arg1, pReset->arg3);
			    if (!pLastMob) {
				sprintf(buf,
					"Save_resets: !NO_MOB! in [%s]",
					pArea->file_name);
				bug(buf, 0);
			    }
			}
			break;

		    case 'D':
			break;

		    case 'R':
			pRoom = get_room_index(pReset->arg1);
			fprintf(fp, "R 0 %d %d\n",
				pReset->arg1, pReset->arg2);
			break;
		    }
		}
	    }			/* End if correct area */
	}			/* End for pRoom */
    }				/* End for iHash */
    fprintf(fp, "S\n\n\n\n");
    return;
}



/*****************************************************************************
 Name:		save_shops
 Purpose:	Saves the #SHOPS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_shops(FILE * fp, AREA_DATA * pArea)
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;
    int iHash;

    fprintf(fp, "#SHOPS\n");

    for (iHash = 0; iHash < MAX_KEY_HASH; iHash++) {
	for (pMobIndex = mob_index_hash[iHash]; pMobIndex;
	     pMobIndex = pMobIndex->next) {
	    if (pMobIndex 
			&& pMobIndex->area == pArea 
			&& pMobIndex->pShop 
			&& !pMobIndex->deleted) {
		pShopIndex = pMobIndex->pShop;

		fprintf(fp, "%d ", pShopIndex->keeper);
		for (iTrade = 0; iTrade < MAX_TRADE; iTrade++) {
		    if (pShopIndex->buy_type[iTrade] != 0) {
			fprintf(fp, "%d ", pShopIndex->buy_type[iTrade]);
		    } else
			fprintf(fp, "0 ");
		}
		fprintf(fp, "%d %d ", pShopIndex->profit_buy,
			pShopIndex->profit_sell);
		fprintf(fp, "%d %d\n", pShopIndex->open_hour,
			pShopIndex->close_hour);
	    }
	}
    }

    fprintf(fp, "0\n\n\n\n");
    return;
}



/*****************************************************************************
 Name:		save_area
 Purpose:	Save an area, note that this format is new.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area(AREA_DATA * pArea)
{
    FILE *fp;

    fclose(fpReserve);
    if (!(fp = fopen(pArea->file_name, "w"))) {
	bug("Open_area: fopen", 0);
	perror(pArea->file_name);
    }
    fprintf(fp, "#AREADATA\n");
    fprintf(fp, "Name %s~\n", pArea->name);
    fprintf(fp, "Builders %s~\n", fix_string(pArea->builders));
    fprintf(fp, "VNUMs %ld %ld\n", pArea->min_vnum, pArea->max_vnum);
    fprintf(fp, "Credits %s~\n", pArea->credits);
    fprintf(fp, "Security %d\n", pArea->security);
    fprintf(fp, "Version %d\n", 1);
    fprintf(fp, "End\n\n\n\n");

    save_mobiles(fp, pArea);
    save_objects(fp, pArea);
    save_rooms(fp, pArea);
    save_specials(fp, pArea);
    save_resets(fp, pArea);
    save_shops(fp, pArea);

    fprintf(fp, "#$\n");

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");
    return;
}


/*****************************************************************************
 Name:		do_asave
 Purpose:	Entry point for saving area data.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_asave(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    AREA_DATA *pArea;
    FILE *fp;
    int value;

    fp = NULL;

    if (!ch) {			/* Do an autosave */
	bool changed = FALSE;

	for (pArea = area_first; pArea; pArea = pArea->next) {
	    if (IS_SET(pArea->area_flags, AREA_CHANGED)) {
		save_area(pArea);
		REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
		changed = TRUE;
	    }
	}
	if (changed)
	    save_area_list();
	return;
    }
    smash_tilde(argument);
    strcpy(arg1, argument);
    if (arg1[0] == '\0') {
	send_to_char("Syntax:\n\r", ch);
	send_to_char("  asave <vnum>   - saves a particular area\n\r", ch);
	send_to_char("  asave world    - saves everything\n\r", ch);
	send_to_char("  asave list     - saves the area.lst file\n\r", ch);
	send_to_char("  asave area     - saves the area being edited\n\r", ch);
	send_to_char("  asave changed  - saves all changed zones\n\r", ch);
	send_to_char("  asave helps    - saves the help files\n\r", ch);
	send_to_char("  asave races    - saves the races\n\r", ch);
	send_to_char("\n\r", ch);
	return;
    }
    if (IS_NPC(ch))
	return;

    /* Snarf the value (which need not be numeric). */
    value = atoi(arg1);
    if (!(pArea = get_area_data(value)) && is_number(arg1)) {
	send_to_char("That area does not exist.\n\r", ch);
	return;
    }
    /* Save area of given vnum. */
    /* ------------------------ */

    if (is_number(arg1)) {
	if (!IS_BUILDER(ch, pArea)) {
	    send_to_char("You are not a builder for this area.\n\r", ch);
	    return;
	}
	save_area_list();
	save_area(pArea);
	return;
    }
    if (!str_cmp("world", arg1)) {
	save_races();
	save_area_list();
	save_helps();
	for (pArea = area_first; pArea; pArea = pArea->next) {
	    if (!IS_BUILDER(ch, pArea))
		continue;

	    save_area(pArea);
	    REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
	}
	send_to_char("You saved the world.\n\r", ch);
	return;
    }
    /* Save changed areas, only authorized areas. */
    /* ------------------------------------------ */

    if (!str_cmp("changed", arg1)) {
	char buf[MAX_INPUT_LENGTH];

	save_area_list();

	send_to_char("Saved zones:\n\r", ch);
	sprintf(buf, "None.\n\r");

	for (pArea = area_first; pArea; pArea = pArea->next) {
	    /* Builder must be assigned this area. */
	    if (!IS_BUILDER(ch, pArea))
		continue;

	    /* Save changed areas. */
	    if (IS_SET(pArea->area_flags, AREA_CHANGED)) {
		save_area(pArea);
		sprintf(buf, "%24s - '%s'\n\r", pArea->name,
			pArea->file_name);
		send_to_char(buf, ch);
		REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
	    }
	}
	if (!str_cmp(buf, "None.\n\r"))
	    send_to_char(buf, ch);
	return;
    }
    /* Save the area.lst file. */
    /* ----------------------- */
    if (!str_cmp(arg1, "list")) {
	save_area_list();
	return;
    }

    /* Save area being edited, if authorized. */
    /* -------------------------------------- */
    if (!str_cmp(arg1, "area")) {
	/* Is character currently editing. */
	if (ch->desc->editor == 0) {
	    send_to_char("You are not editing an area, "
			 "therefore an area vnum is required.\n\r", ch);
	    return;
	}
	/* Find the area to save. */
	switch (ch->desc->editor) {
	case ED_AREA:
	    pArea = (AREA_DATA *) ch->desc->pEdit;
	    break;
	case ED_ROOM:
	    pArea = ch->in_room->area;
	    break;
	case ED_OBJECT:
	    pArea = ((OBJ_INDEX_DATA *) ch->desc->pEdit)->area;
	    break;
	case ED_MOBILE:
	    pArea = ((MOB_INDEX_DATA *) ch->desc->pEdit)->area;
	    break;
	default:
	    pArea = ch->in_room->area;
	    break;
	}

	if (!IS_BUILDER(ch, pArea)) {
	    send_to_char("You are not a builder for this area.\n\r", ch);
	    return;
	}
	save_area_list();
	save_area(pArea);
	REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Area saved.\n\r", ch);
	return;
    }

    /* Save Races */
    if (!str_cmp(arg1, "races")) {
		save_races();
		send_to_char("Races Saved.\n\r", ch);
	return;
    }

    /* Save Help File */
    if (!str_cmp(arg1, "helps")) {
		save_helps();
		send_to_char("Helps Saved.\n\r", ch);
	return;
    }

    /* Show correct syntax. */
    /* -------------------- */
    do_asave(ch, "");
    return;
}

void save_helps()
{
    HELP_DATA *pHelp;
    FILE *fp;

    if (!(fp = fopen("help.are", "w"))) {
	bug("Open_help: fopen", 0);
	perror("help.are");
    }
    fprintf(fp, "#HELPS\n");

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
	if (pHelp->delete)
	    continue;

	fprintf(fp, "%d %s~\n\n%s~\n",
		pHelp->level, pHelp->keyword, fix_string(pHelp->text));
    }

    fprintf(fp, "0 $~\n\n#$\n");
    fclose(fp);
    return;
}

void save_mobprogs(FILE * fp, MPROG_CODE * pMprog)
{
    int i;

    fprintf(fp, "#MOBPROGS\n");

    for (i = 0; i < MAX_VNUM; i++) {
	if ((pMprog = get_mprog_index(i)) != NULL)
	    fprintf(fp, "#%d\n%s~\n", i, fix_string(pMprog->code));
    }

    fprintf(fp, "#0\n\n");
    return;
}

void save_mobprog(MPROG_CODE * pMcode)
{
    FILE *fp;

    fclose(fpReserve);
    if (!(fp = fopen("mobprog.prg", "w"))) {
	bug("Open_help: fopen", 0);
	perror("mobprog.prg");
    }
    save_mobprogs(fp, pMcode);
    fprintf(fp, "#$\n");

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    return;
}

void do_mpsave(CHAR_DATA * ch, char *argument)
{
    MPROG_CODE *pMcode;
    FILE *fp;

    if (!IS_IMMORTAL(ch)) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    fp = NULL;
    if (ch->desc->editor == 0) {
	send_to_char("You are not editing a mobprogram code\n\r", ch);
	return;
    }

    pMcode = (MPROG_CODE *) ch->desc->pEdit;

    save_mobprog(pMcode);
    send_to_char("Code saved.\n\r", ch);
    return;
}
