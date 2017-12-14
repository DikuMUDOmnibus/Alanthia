/***************************************************************************
 *  File: olc.h                                                            *
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
/*
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  -Jason Dinkel
 */


/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define VERSION	"ILAB Online Creation [Beta 1.0, ROM 2.3 modified]\n\r" \
				"     Port a ROM 2.4 v1.00\n\r"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)\n\r" \
                "     Modified for use with ROM 2.3\n\r"        \
                "     By Hans Birkeland (hansbi@ifi.uio.no)\n\r" \
                "     Modificado para uso en ROM 2.4v4\n\r"	\
                "     Por Birdie (itoledo@ramses.centic.utem.cl)\n\r"
#define DATE	"     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r" \
				"     (Port a ROM 2.4 - Nov 2, 1996)\n\r"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"



/*
 * New typedefs.
 */
typedef bool OLC_FUN args((CHAR_DATA * ch, char *argument));
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun


/* Command procedures needed ROM OLC */
DECLARE_DO_FUN(do_help);
DECLARE_SPELL_FUN(spell_null);





/*
 * Connected states for editor.
 */
#define ED_AREA   1
#define ED_ROOM   2
#define ED_OBJECT 3
#define ED_MOBILE 4
#define ED_HELP   5
#define ED_RACE	  6



/*
 * Interpreter Prototypes
 */
void aedit args((CHAR_DATA * ch, char *argument));
void redit args((CHAR_DATA * ch, char *argument));
void medit args((CHAR_DATA * ch, char *argument));
void oedit args((CHAR_DATA * ch, char *argument));
void hedit args((CHAR_DATA * ch, char *argument));
void raedit args((CHAR_DATA * ch, char *argument));



/*
 * OLC Constants
 */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */



/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type {
    char *const name;
    OLC_FUN *olc_fun;
};



/*
 * Structure for an OLC editor startup command.
 */
struct editor_cmd_type {
    char *const name;
    DO_FUN *do_fun;
};



/*
 * Utils.
 */
AREA_DATA *get_vnum_area args((long vnum));
AREA_DATA *get_area_data args((long vnum));
int flag_value args((const struct flag_type * flag_table,
		     char *argument));
char *flag_string args((const struct flag_type * flag_table,
			int bits));
void add_reset args((ROOM_INDEX_DATA * room,
		     RESET_DATA * pReset, int index));



/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type aedit_table[];
extern const struct olc_cmd_type redit_table[];
extern const struct olc_cmd_type oedit_table[];
extern const struct olc_cmd_type medit_table[];
extern const struct olc_cmd_type hedit_table[];
extern const struct olc_cmd_type raedit_table[];


/*
 * Editor Commands.
 */
DECLARE_DO_FUN(do_aedit);
DECLARE_DO_FUN(do_redit);
DECLARE_DO_FUN(do_oedit);
DECLARE_DO_FUN(do_medit);
DECLARE_DO_FUN(do_hedit);
DECLARE_DO_FUN(do_raedit);



/*
 * General Functions
 */
bool show_commands args((CHAR_DATA * ch, char *argument));
bool show_help args((CHAR_DATA * ch, char *argument));
bool edit_done args((CHAR_DATA * ch));
bool show_version args((CHAR_DATA * ch, char *argument));
bool check_range args((int lower, int upper));
void show_obj_values args((CHAR_DATA * ch, OBJ_INDEX_DATA * obj));
bool set_obj_values args((CHAR_DATA * ch, OBJ_INDEX_DATA * pObj, int value_num,
            char *argument));
int wear_bit args((int loc));




/*
 * Race Editor Protos
 */

DECLARE_OLC_FUN(raedit_act);
DECLARE_OLC_FUN(raedit_aff);
DECLARE_OLC_FUN(raedit_create);
DECLARE_OLC_FUN(raedit_delete);
DECLARE_OLC_FUN(raedit_feeds);
DECLARE_OLC_FUN(raedit_form);
DECLARE_OLC_FUN(raedit_gname);
DECLARE_OLC_FUN(raedit_imm);
DECLARE_OLC_FUN(raedit_life);
DECLARE_OLC_FUN(raedit_max);
DECLARE_OLC_FUN(raedit_min);
DECLARE_OLC_FUN(raedit_name);
DECLARE_OLC_FUN(raedit_off);
DECLARE_OLC_FUN(raedit_parts);
DECLARE_OLC_FUN(raedit_pc);
DECLARE_OLC_FUN(raedit_points);
DECLARE_OLC_FUN(raedit_res);
DECLARE_OLC_FUN(raedit_senses);
DECLARE_OLC_FUN(raedit_sex);
DECLARE_OLC_FUN(raedit_show);
DECLARE_OLC_FUN(raedit_size);
DECLARE_OLC_FUN(raedit_skills);
DECLARE_OLC_FUN(raedit_stats);
DECLARE_OLC_FUN(raedit_ver);
DECLARE_OLC_FUN(raedit_vuln);
DECLARE_OLC_FUN(raedit_wname);

/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN(aedit_show);
DECLARE_OLC_FUN(aedit_create);
DECLARE_OLC_FUN(aedit_name);
DECLARE_OLC_FUN(aedit_file);
DECLARE_OLC_FUN(aedit_age);
DECLARE_OLC_FUN(aedit_reset);
DECLARE_OLC_FUN(aedit_security);
DECLARE_OLC_FUN(aedit_builder);
DECLARE_OLC_FUN(aedit_vnum);
DECLARE_OLC_FUN(aedit_lvnum);
DECLARE_OLC_FUN(aedit_uvnum);
DECLARE_OLC_FUN(aedit_credits);


/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN(redit_show);
DECLARE_OLC_FUN(redit_create);
DECLARE_OLC_FUN(redit_name);
DECLARE_OLC_FUN(redit_desc);
DECLARE_OLC_FUN(redit_ed);
DECLARE_OLC_FUN(redit_format);
DECLARE_OLC_FUN(redit_north);
DECLARE_OLC_FUN(redit_south);
DECLARE_OLC_FUN(redit_east);
DECLARE_OLC_FUN(redit_west);
DECLARE_OLC_FUN(redit_up);
DECLARE_OLC_FUN(redit_down);
DECLARE_OLC_FUN(redit_mreset);
DECLARE_OLC_FUN(redit_oreset);
DECLARE_OLC_FUN(redit_mlist);
DECLARE_OLC_FUN(redit_rlist);
DECLARE_OLC_FUN(redit_olist);
DECLARE_OLC_FUN(redit_mshow);
DECLARE_OLC_FUN(redit_oshow);
DECLARE_OLC_FUN(redit_heal);
DECLARE_OLC_FUN(redit_mana);
DECLARE_OLC_FUN(redit_clan);
DECLARE_OLC_FUN(redit_delete);


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN(oedit_show);
DECLARE_OLC_FUN(oedit_create);
DECLARE_OLC_FUN(oedit_name);
DECLARE_OLC_FUN(oedit_short);
DECLARE_OLC_FUN(oedit_long);
DECLARE_OLC_FUN(oedit_addaffect);
DECLARE_OLC_FUN(oedit_delaffect);
DECLARE_OLC_FUN(oedit_value0);
DECLARE_OLC_FUN(oedit_value1);
DECLARE_OLC_FUN(oedit_value2);
DECLARE_OLC_FUN(oedit_value3);
DECLARE_OLC_FUN(oedit_value4);	/* ROM */
DECLARE_OLC_FUN(oedit_weight);
DECLARE_OLC_FUN(oedit_cost);
DECLARE_OLC_FUN(oedit_ed);

DECLARE_OLC_FUN(oedit_extra);	/* ROM */
DECLARE_OLC_FUN(oedit_wear);	/* ROM */
DECLARE_OLC_FUN(oedit_type);	/* ROM */
DECLARE_OLC_FUN(oedit_affect);	/* ROM */
DECLARE_OLC_FUN(oedit_material);	/* ROM */
DECLARE_OLC_FUN(oedit_level);	/* ROM */
DECLARE_OLC_FUN(oedit_condition);	/* ROM */
DECLARE_OLC_FUN(oedit_timer);	/* ROM */
DECLARE_OLC_FUN(oedit_delete);	/* ROM */

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN(medit_show);
DECLARE_OLC_FUN(medit_create);
DECLARE_OLC_FUN(medit_name);
DECLARE_OLC_FUN(medit_short);
DECLARE_OLC_FUN(medit_long);
DECLARE_OLC_FUN(medit_shop);
DECLARE_OLC_FUN(medit_desc);
DECLARE_OLC_FUN(medit_level);
DECLARE_OLC_FUN(medit_align);
DECLARE_OLC_FUN(medit_spec);

DECLARE_OLC_FUN(medit_sex);	/* ROM */
DECLARE_OLC_FUN(medit_act);	/* ROM */
DECLARE_OLC_FUN(medit_affect);	/* ROM */
DECLARE_OLC_FUN(medit_ac);	/* ROM */
DECLARE_OLC_FUN(medit_form);	/* ROM */
DECLARE_OLC_FUN(medit_part);	/* ROM */
DECLARE_OLC_FUN(medit_imm);	/* ROM */
DECLARE_OLC_FUN(medit_res);	/* ROM */
DECLARE_OLC_FUN(medit_vuln);	/* ROM */
DECLARE_OLC_FUN(medit_material);	/* ROM */
DECLARE_OLC_FUN(medit_off);	/* ROM */
DECLARE_OLC_FUN(medit_size);	/* ROM */
DECLARE_OLC_FUN(medit_hitdice);	/* ROM */
DECLARE_OLC_FUN(medit_manadice);	/* ROM */
DECLARE_OLC_FUN(medit_damdice);	/* ROM */
DECLARE_OLC_FUN(medit_race);	/* ROM */
DECLARE_OLC_FUN(medit_position);	/* ROM */
DECLARE_OLC_FUN(medit_gold);	/* ROM */
DECLARE_OLC_FUN(medit_tarifa);	/* ROM */
DECLARE_OLC_FUN(medit_hitroll);	/* ROM */
DECLARE_OLC_FUN(medit_camino);	/* ROM */
DECLARE_OLC_FUN(medit_damtype);	/* ROM */
DECLARE_OLC_FUN(medit_delete);	/* ROM */

/* Help Editor - kermit 1/98 */
DECLARE_OLC_FUN(hedit_show);
DECLARE_OLC_FUN(hedit_make);
DECLARE_OLC_FUN(hedit_desc);
DECLARE_OLC_FUN(hedit_level);
DECLARE_OLC_FUN(hedit_keywords);
DECLARE_OLC_FUN(hedit_delete);

/*
 * Macros
 */

#define IS_SWITCHED( ch )       ( ch->desc->original )	/* ROM OLC */

#define IS_BUILDER(ch, Area)    ( !IS_SWITCHED( ch ) &&                \
                                (  ch->pcdata->security >= 2           \
                                || !str_infix( Area->builders, ch->name )  \
                                || !str_infix( Area->builders, "All" ) ) )

#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)Ch->desc->pEdit )
#define EDIT_RACE(Ch, Race)	( Race = (RACE_DATA *)Ch->desc->pEdit )


/*
 * Prototypes
 */
/* mem.c - memory prototypes. */
#define ED	EXTRA_DESCR_DATA
RESET_DATA *new_reset_data args((void));
void free_reset_data args((RESET_DATA * pReset));
AREA_DATA *new_area args((void));
void free_area args((AREA_DATA * pArea));
EXIT_DATA *new_exit args((void));
void free_exit args((EXIT_DATA * pExit));
ED *new_extra_descr args((void));
void free_extra_descr args((ED * pExtra));
ROOM_INDEX_DATA *new_room_index args((void));
void free_room_index args((ROOM_INDEX_DATA * pRoom));
AFFECT_DATA *new_affect args((void));
void free_affect args((AFFECT_DATA * pAf));
SHOP_DATA *new_shop args((void));
void free_shop args((SHOP_DATA * pShop));
OBJ_INDEX_DATA *new_obj_index args((void));
void free_obj_index args((OBJ_INDEX_DATA * pObj));
MOB_INDEX_DATA *new_mob_index args((void));
void free_mob_index args((MOB_INDEX_DATA * pMob));
#undef	ED

void show_liqlist args((CHAR_DATA * ch));
void show_damlist args((CHAR_DATA * ch));

#define EDIT_HELP(Ch, Help)     ( Help = (HELP_DATA *)Ch->desc->pEdit )

HELP_DATA *new_help args((void));
RACE_DATA *new_race args((void));
void free_help args((HELP_DATA * pHelp));
void save_helps args((void));

#define REDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define OEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define MEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define AEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define HEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define RAEDIT( fun )       bool fun( CHAR_DATA *ch, char *argument )



struct olc_help_type {
    char *command;
    const void *structure;
    char *desc;
};

extern const struct olc_cmd_type        mpedit_table[];
DECLARE_DO_FUN( do_mpedit       );      
DECLARE_OLC_FUN( redit_mpshow           );
MPROG_CODE       *new_mpcode             args ( (void) );
void             free_mpcode             args ( ( MPROG_CODE *pMcode));

DECLARE_OLC_FUN( medit_addmprog         ); /* Lordrom */
DECLARE_OLC_FUN( medit_delmprog         ); /* Lordrom */
MPROG_LIST      *new_mprog              args ( ( void ) );
void            free_mprog              args ( ( MPROG_LIST *mp ) );

