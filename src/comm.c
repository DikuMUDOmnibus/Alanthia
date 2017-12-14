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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
/*
   #include "struct.h"
   #include "extern.h"
 */

#include "merc.h"
#include "utils.h"

#include "telnet.h"
#include "recycle.h"
#include "tables.h"
#include "utils.h"

/* command procedures needed */
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_skills);
DECLARE_DO_FUN(do_spells);
DECLARE_DO_FUN(do_outfit);
DECLARE_DO_FUN(do_unread);
DECLARE_DO_FUN(do_asave);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_replay);
DECLARE_DO_FUN(do_quit);

/*
 * Malloc debugging stuff.
 */

extern int malloc_debug args((int));
extern int malloc_verify args((void));

/*
 * Socket and TCP/IP stuff.
 */

extern const char echo_off_str[];
extern const char echo_on_str[]; 
extern const char go_ahead_str[];

/*
 * OS-dependent declarations.
 */

int select args((int width, fd_set * readfds, fd_set * writefds,
		 fd_set * exceptfds, struct timeval * timeout));
int socket args((int domain, int type, int protocol));


/*
 * Global variables.
 */
DESCRIPTOR_DATA *descriptor_list;	/* All open descriptors         */
DESCRIPTOR_DATA *d_next;	/* Next descriptor in loop      */
FILE *fpReserve;		/* Reserved file handle         */
bool god;			/* All new chars are gods!      */
bool merc_down;			/* Shutdown                     */
extern bool wizlock;			/* Game is wizlocked            */
extern bool newlock;			/* Game is newlocked            */
char str_boot_time[MAX_INPUT_LENGTH];
time_t current_time;		/* time of this pulse */
bool MOBtrigger = TRUE;		/* act() switch                 */


/* Needs to be global because of do_copyover */
int port, control;

int main(int argc, char **argv)
{
    struct timeval now_time;
    bool fCopyOver = FALSE;

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug(2);
#endif

#ifdef DEBUG
	setbuf(stdout, NULL);
#endif

    /*
     * Init time.
     */
    gettimeofday(&now_time, NULL);
    current_time = (time_t) now_time.tv_sec;
    strcpy(str_boot_time, ctime(&current_time));

    /*
     * Reserve one channel for our use.
     */
    if ((fpReserve = fopen(NULL_FILE, "r")) == NULL) {
	perror(NULL_FILE);
	exit(1);
    }
    /*
     * Get the port number.  !!! IMPORTANT !!!
     */
    port = 1538;
    if (argc > 1) {
	if (!is_number(argv[1])) {
	    fprintf(stderr, "Usage: %s [port #]\n", argv[0]);
	    exit(1);
	} else if ((port = atoi(argv[1])) <= 1024) {
	    fprintf(stderr, "Port number must be above 1024.\n");
	    exit(1);
	}
	/* Are we recovering from a copyover? */
	if (argv[2] && argv[2][0]) {
	    fCopyOver = TRUE;
	    control = atoi(argv[3]);
	} else
	    fCopyOver = FALSE;

    }
    /*
     * Run the game.
     */
    if (!fCopyOver)
	control = init_socket(port);

    boot_db();
	turf_load();
	cbank_load();
    sprintf(log_buf, "ROM is ready to rock on port %d.", port);
    log_string(log_buf);

    if (fCopyOver)
	copyover_recover();

    game_loop_unix(control);
    close(control);

    /*
     * That's all, folks.
     */
    log_string("Normal termination of game.");
    exit(0);
    return 0;
}



int init_socket(int port)
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Init_socket: socket");
	exit(1);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		   (char *) &x, sizeof(x)) < 0) {
	perror("Init_socket: SO_REUSEADDR");
	close(fd);
	exit(1);
    }
#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct linger ld;

	ld.l_onoff = 1;
	ld.l_linger = 1000;

	if (setsockopt(fd, SOL_SOCKET, SO_DONTLINGER,
		       (char *) &ld, sizeof(ld)) < 0) {
	    perror("Init_socket: SO_DONTLINGER");
	    close(fd);
	    exit(1);
	}
    }
#endif

    sa = sa_zero;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
	perror("Init socket: bind");
	close(fd);
	exit(1);
    }
    if (listen(fd, 3) < 0) {
	perror("Init socket: listen");
	close(fd);
	exit(1);
    }
    return fd;
}



void game_loop_unix(int control)
{
    static struct timeval null_time;
    struct timeval last_time;
    bool color;

    signal(SIGPIPE, SIG_IGN);
    gettimeofday(&last_time, NULL);
    current_time = (time_t) last_time.tv_sec;
    color = TRUE;

    /* Main loop */
    while (!merc_down) {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if (malloc_verify() != 1)
	    abort();
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO(&in_set);
	FD_ZERO(&out_set);
	FD_ZERO(&exc_set);
	FD_SET(control, &in_set);
	maxdesc = control;
	for (d = descriptor_list; d; d = d->next) {
	    maxdesc = UMAX(maxdesc, d->descriptor);
	    FD_SET(d->descriptor, &in_set);
	    FD_SET(d->descriptor, &out_set);
	    FD_SET(d->descriptor, &exc_set);
	}

	if (select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) <
	    0) {
	    perror("Game_loop: select: poll");
	    exit(1);
	}
	/*
	 * New connection?
	 */
	if (FD_ISSET(control, &in_set))
	    init_descriptor(control);

	/*
	 * Kick out the freaky folks.
	 */
	for (d = descriptor_list; d != NULL; d = d_next) {
	    d_next = d->next;
	    if (FD_ISSET(d->descriptor, &exc_set)) {
		FD_CLR(d->descriptor, &in_set);
		FD_CLR(d->descriptor, &out_set);
		if (d->character && d->connected == CON_PLAYING)
		    save_char_obj(d->character);
		d->outtop = 0;
		close_socket(d);
	    }
	}

	/*
	 * Process input.
	 */

	for (d = descriptor_list; d != NULL; d = d_next) {
	    d_next = d->next;
	    d->fcommand = FALSE;

	    if (FD_ISSET(d->descriptor, &in_set)) {
		if (d->character != NULL) {
		    d->character->timer = 0;
		    if (IS_SET(d->character->act, PLR_COLOUR))
			color = TRUE;
		    else
			color = TRUE;
		} else
		    color = TRUE;
		if (!read_from_descriptor(d, color)) {
		    FD_CLR(d->descriptor, &out_set);
		    if (d->character != NULL && d->character->level > 1)
			save_char_obj(d->character);
		    d->outtop = 0;
		    close_socket(d);
		    continue;
		}
	    }
	    if (d->character != NULL && d->character->daze > 0)
		--d->character->daze;

	    if (d->character != NULL && d->character->wait > 0) {
		--d->character->wait;
		continue;
	    }
	    read_from_buffer(d, color);
	    if (d->incomm[0] != '\0') {
		d->fcommand = TRUE;
		stop_idling(d->character);

		/* OLC */
		if (d->showstr_point)
		    show_string(d, d->incomm);
		else if (d->pString)
		    string_add(d->character, d->incomm);
		else
		    switch (d->connected) {
		    case CON_PLAYING:
			if (!run_olc_editor(d))
			    substitute_alias(d, d->incomm);
			break;
		    default:
			nanny(d, d->incomm);
			break;
		    }

		d->incomm[0] = '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler();



	/*
	 * Output.
	 */
	for (d = descriptor_list; d != NULL; d = d_next) {
	    d_next = d->next;

	    if ((d->fcommand || d->outtop > 0)
		&& FD_ISSET(d->descriptor, &out_set)) {
		if (!process_output(d, TRUE)) {
		    if (d->character != NULL
			&& d->connected ==
			CON_PLAYING) save_char_obj(d->character);
		    d->outtop = 0;
		    close_socket(d);
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday(&now_time, NULL);
	    usecDelta =
		((int) last_time.tv_usec) - ((int) now_time.tv_usec) +
		1000000 / PULSE_PER_SECOND;
	    secDelta = ((int) last_time.tv_sec) - ((int) now_time.tv_sec);
	    while (usecDelta < 0) {
		usecDelta += 1000000;
		secDelta -= 1;
	    }

	    while (usecDelta >= 1000000) {
		usecDelta -= 1000000;
		secDelta += 1;
	    }

	    if (secDelta > 0 || (secDelta == 0 && usecDelta > 0)) {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec = secDelta;
		if (select(0, NULL, NULL, NULL, &stall_time) < 0) {
		    perror("Game_loop: select: stall");
		    exit(1);
		}
	    }
	}

	gettimeofday(&last_time, NULL);
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}



void init_descriptor(int control)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
  	struct hostent *from; 
    int desc;
    int size;
    int addr;
    bool color;

    size = sizeof(sock);
    getsockname(control, (struct sockaddr *) &sock, &size);
    if ((desc = accept(control, (struct sockaddr *) &sock, &size)) < 0) {
	perror("New_descriptor: accept");
	return;
    }
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if (fcntl(desc, F_SETFL, FNDELAY) == -1) {
	perror("New_descriptor: fcntl: FNDELAY");
	return;
    }
    /*
     * Cons a new descriptor.
     */
    dnew = new_descriptor();

    dnew->descriptor = desc;
    dnew->connected = CON_ASK_IF_COLOUR;
    dnew->showstr_head = NULL;
    dnew->showstr_point = NULL;
    dnew->outsize = 2000;
    dnew->pEdit = NULL;		/* OLC */
    dnew->pString = NULL;	/* OLC */
    dnew->editor = 0;		/* OLC */
    dnew->outbuf = alloc_mem(dnew->outsize);

    size = sizeof(sock);
    if (dnew->pEdit == NULL) {
        perror ("New_descriptor: getpeername");
    	addr = ntohl(sock.sin_addr.s_addr);
    	sprintf(buf, "%d.%d.%d.%d",
	    (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
	    (addr >> 8) & 0xFF, (addr) & 0xFF);
    	sprintf(log_buf, "Sock.sinaddr:  %s", buf);
    	log_string(log_buf);

      from = gethostbyaddr ((char *) &sock.sin_addr,
			    sizeof (sock.sin_addr), AF_INET);

      dnew->host = str_dup (from ? from->h_name : buf);

    	//dnew->host = str_dup(buf);
      	//dnew->host = str_dup ("(unknown)");
    }
  	else if (getpeername (desc, (struct sockaddr *) &sock, &size) < 0)
   {
      
      int addr;

      addr = ntohl (sock.sin_addr.s_addr);
      sprintf (buf, "%d.%d.%d.%d",
	       (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
	       (addr >> 8) & 0xFF, (addr) & 0xFF);
      sprintf (log_buf, "Sock.sinaddr:  %s", buf);
      log_string (log_buf);

      from = gethostbyaddr ((char *) &sock.sin_addr,
			    sizeof (sock.sin_addr), AF_INET);

      dnew->host = str_dup (from ? from->h_name : buf);
    }

    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if (check_ban(dnew->host, BAN_ALL)) {
	color = FALSE;
	write_to_descriptor(desc,
			    "Your site has been banned from this mud.\n\r",
			    0, color);
	close(desc);
	free_descriptor(dnew);
	return;
    }
    /*
     * Init descriptor data.
     */
    dnew->next = descriptor_list;
    descriptor_list = dnew;

    write_to_buffer(dnew, "Alanthia awaits you...\n\r", 0);
    write_to_buffer(dnew, "Use ANSI Color? (Y/n) ", 0);

    /*
       * Send the greeting.
     */
    return;
}



void close_socket(DESCRIPTOR_DATA * dclose)
{
    CHAR_DATA *ch;

    if (dclose->outtop > 0)
	process_output(dclose, FALSE);

    if (dclose->snoop_by != NULL) {
	write_to_buffer(dclose->snoop_by,
			"Your victim has left the game.\n\r", 0);
    }
    {
	DESCRIPTOR_DATA *d;

	for (d = descriptor_list; d != NULL; d = d->next) {
	    if (d->snoop_by == dclose)
		d->snoop_by = NULL;
	}
    }

    if ((ch = dclose->character) != NULL) {
	sprintf(log_buf, "Closing link to %s.", ch->name);
	log_string(log_buf);
	/* cut down on wiznet spam when rebooting */
	if (dclose->connected == CON_PLAYING && !merc_down) {
	    act("$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
	    wiznet("Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0);
	    ch->desc = NULL;
	} else {
	    free_char(dclose->original ? dclose->original :
		      dclose->character);
	}
    }
    if (d_next == dclose)
	d_next = d_next->next;

    if (dclose == descriptor_list) {
	descriptor_list = descriptor_list->next;
    } else {
	DESCRIPTOR_DATA *d;

	for (d = descriptor_list; d && d->next != dclose; d = d->next);
	if (d != NULL)
	    d->next = dclose->next;
	else
	    bug("Close_socket: dclose not found.", 0);
    }

    close(dclose->descriptor);
    free_descriptor(dclose);
    return;
}



bool read_from_descriptor(DESCRIPTOR_DATA * d, bool color)
{
    int iStart;

    /* Hold horses if pending command already. */
    if (d->incomm[0] != '\0')
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if (iStart >= sizeof(d->inbuf) - 10) {
	sprintf(log_buf, "%s input overflow!", d->host);
	log_string(log_buf);
	write_to_descriptor(d->descriptor,
			    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0,
			    color);
	return FALSE;
    }
    for (;;) {
	int nRead;

	nRead = read(d->descriptor, d->inbuf + iStart,
		     sizeof(d->inbuf) - 10 - iStart);
	if (nRead > 0) {
	    iStart += nRead;
	    if (d->inbuf[iStart - 1] == '\n'
		|| d->inbuf[iStart - 1] == '\r') break;
	} else if (nRead == 0) {
	    log_string("EOF encountered on read.");
	    return FALSE;
	} else if (errno == EWOULDBLOCK)
	    break;
	else {
	    perror("Read_from_descriptor");
	    return FALSE;
	}
    }

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer(DESCRIPTOR_DATA * d, bool color)
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if (d->incomm[0] != '\0')
	return;

    /*
     * Look for at least one new line.
     */
    for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++) {
	if (d->inbuf[i] == '\0')
	    return;
    }

    /*
     * Canonical input processing.
     */
    for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++) {
	if (k >= MAX_INPUT_LENGTH - 2) {
	    write_to_descriptor(d->descriptor, "Line too long.\n\r", 0,
				color);

	    /* skip the rest of the line */
	    for (; d->inbuf[i] != '\0'; i++) {
		if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
		    break;
	    }
	    d->inbuf[i] = '\n';
	    d->inbuf[i + 1] = '\0';
	    break;
	}
	if (d->inbuf[i] == '\b' && k > 0)
	    --k;
	else if (isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if (k == 0)
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Do '!' substitution.
     */
    if (d->incomm[0] == '!')
	strcpy(d->incomm, d->inlast);
    else
	strcpy(d->inlast, d->incomm);

    /*
     * Shift the input buffer.
     */
    while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
	i++;
    for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++);
    return;
}



/*
 * Low level output function.
 */
bool process_output(DESCRIPTOR_DATA * d, bool fPrompt)
{

    extern bool merc_down;
    bool color = TRUE;
    /*
     * Bust a prompt.
     */
    if ((d->character != NULL) && IS_SET(d->character->act, PLR_COLOUR))
	color = TRUE;
    else
	color = FALSE;


    if (!merc_down) {
	if (d->showstr_point)
	    write_to_buffer(d, "[Hit Return to continue]\n\r", 0);
	else if (fPrompt && d->pString && d->connected == CON_PLAYING)
	    write_to_buffer(d, "> ", 2);
	else if (fPrompt && d->connected == CON_PLAYING) {
	    CHAR_DATA *ch;
	    CHAR_DATA *victim;

	    ch = d->character;
	    if (IS_SET(ch->act, PLR_COLOUR))
		color = TRUE;
	    else
		color = FALSE;
	    /* battle prompt */
	    if ((victim = ch->fighting) != NULL && can_see(ch, victim)) {
		int percent;
		char wound[100];
		char *pbuff;
		char buf[MAX_STRING_LENGTH];
		char buffer[MAX_STRING_LENGTH * 2];

		if (victim->max_hit > 0)
		    percent = victim->hit * 100 / victim->max_hit;
		else
		    percent = -1;

		if (percent >= 100)
		    sprintf(wound, "is in excellent condition.");
		else if (percent >= 90)
		    sprintf(wound, "has a few scratches.");
		else if (percent >= 75)
		    sprintf(wound, "has some small wounds and bruises.");
		else if (percent >= 50)
		    sprintf(wound, "has quite a few wounds.");
		else if (percent >= 30)
		    sprintf(wound,
			    "has some big nasty wounds and scratches.");
		else if (percent >= 15)
		    sprintf(wound, "looks pretty hurt.");
		else if (percent >= 0)
		    sprintf(wound, "is in awful condition.");
		else
		    sprintf(wound, "is bleeding to death.");

		sprintf(buf, "%s %s \n\r",
			IS_NPC(victim) ? victim->short_descr : victim->
			name, wound);
		buf[0] = UPPER(buf[0]);
		pbuff = buffer;
		colourconv(pbuff, buf, d->character);
		write_to_buffer(d, buffer, 0);
	    }
	    ch = d->original ? d->original : d->character;
	    if (!IS_SET(ch->comm, COMM_COMPACT))
		write_to_buffer(d, "\n\r", 2);


	    if (IS_SET(ch->comm, COMM_PROMPT))
		bust_a_prompt(d->character);

	    if (IS_SET(ch->comm, COMM_TELNET_GA))
		write_to_buffer(d, go_ahead_str, 0);
	}
    }

    /*
     * Short-circuit if nothing to write.
     */
    if (d->outtop == 0)
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if (d->snoop_by != NULL) {
	if (d->character != NULL)
	    write_to_buffer(d->snoop_by, d->character->name, 0);
	write_to_buffer(d->snoop_by, "> ", 2);
	write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
    }
    /*
     * OS-dependent output.
     */
    if (!write_to_descriptor(d->descriptor, d->outbuf, d->outtop, color)) {
	d->outtop = 0;
	return FALSE;
    } else {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    const char *str;
    const char *i;
    char *point;
    char *pbuff;
    char buffer[MAX_STRING_LENGTH * 2];
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    const char *dir_name[] = { "N", "E", "S", "W", "U", "D" };
    int door;

    point = buf;
    str = ch->prompt;
    if (!str || str[0] == '\0') {
	if (IS_VAMPIRE(ch)) {
	    sprintf(buf, "{C<%ldhp %ldm %ldmv %dbp>{x %s",
		    ch->hit, ch->mana, ch->move, ch->blood, ch->prefix);
	} else {
	    sprintf(buf, "{C<%ldhp %ldm %ldmv>{x %s",
		    ch->hit, ch->mana, ch->move, ch->prefix);
	}
	send_to_char(buf, ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_AFK))
	send_to_char("{m[{MA{mF{MK{m]{x ", ch);

    while (*str != '\0') {
	if (*str != '%') {
	    *point++ = *str++;
	    continue;
	}
	++str;
	switch (*str) {
	default:
	    i = " ";
	    break;
	case 'e':
	    found = FALSE;
	    doors[0] = '\0';
	    for (door = 0; door < 6; door++) {
		if ((pexit = ch->in_room->exit[door]) != NULL
		    && pexit->u1.to_room != NULL
		    && (can_see_room(ch, pexit->u1.to_room)
			|| (IS_AFFECTED(ch, AFF_INFRARED)
			    && !IS_AFFECTED(ch, AFF_BLIND)))) {
		    if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
			strcat(doors, " ");
			strcat(doors, dir_name[door]);
		    } else {
			if (!IS_SET(pexit->exit_info, EX_SECRET)
			    && (!IS_SET(pexit->exit_info, EX_HIDDEN))) {
			    strcat(doors, " {g({w");
			    strcat(doors, dir_name[door]);
			    strcat(doors, "{g){w");
			}
		    }
		}
	    }
	    if (!found)
		strcat(buf, "none");
	    sprintf(buf2, "%s", doors);
	    i = buf2;
	    break;
	case 'c':
	    sprintf(buf2, "%s", "\n\r");
	    i = buf2;
	    break;
	case 'h':
	    sprintf(buf2, "%ld", ch->hit);
	    i = buf2;
	    break;
	case 'H':
	    sprintf(buf2, "%ld", ch->max_hit);
	    i = buf2;
	    break;
	case 'm':
	    sprintf(buf2, "%ld", ch->mana);
	    i = buf2;
	    break;
	case 'M':
	    sprintf(buf2, "%ld", ch->max_mana);
	    i = buf2;
	    break;
	case 'v':
	    sprintf(buf2, "%ld", ch->move);
	    i = buf2;
	    break;
	case 'V':
	    sprintf(buf2, "%ld", ch->max_move);
	    i = buf2;
	    break;
	case 'b':
	    sprintf(buf2, "%d", ch->blood);
	    i = buf2;
	    break;
	case 'B':
	    sprintf(buf2, "%d", ch->max_blood);
	    i = buf2;
	    break;
	case 'x':
	    sprintf(buf2, "%ld", ch->exp);
	    i = buf2;
	    break;
	case 'X':
	    sprintf(buf2, "%ld", IS_NPC(ch) ? 0 :
		    (ch->level + 1) * exp_per_level(ch,
						    ch->pcdata->points) -
		    ch->exp);
	    i = buf2;
	    break;
	case 'g':
	    sprintf(buf2, "%ld", ch->gold);
	    i = buf2;
	    break;
	case 'a':
	    if (ch->level > 9)
		sprintf(buf2, "%d", ch->alignment);
	    else
		sprintf(buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
			"evil" : "neutral");
	    i = buf2;
	    break;
	case 'r':
	    if (ch->in_room != NULL)
		sprintf(buf2, "%s",
			((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) ||
			 (!IS_AFFECTED(ch, AFF_BLIND)
			  && !room_is_dark(ch->in_room))) ? ch->in_room->
			name : "darkness");
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case 'R':
	    if (IS_IMMORTAL(ch) && ch->in_room != NULL)
		sprintf(buf2, "%ld", ch->in_room->vnum);
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case 'z':
	    if (IS_IMMORTAL(ch) && ch->in_room != NULL)
		sprintf(buf2, "%s", ch->in_room->area->name);
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case '%':
	    sprintf(buf2, "%%");
	    i = buf2;
	    break;
	case 'o':
	    sprintf(buf2, "%s", olc_ed_name(ch));
	    i = buf2;
	    break;
	case 'O':
	    sprintf(buf2, "%s", olc_ed_vnum(ch));
	    i = buf2;
	    break;
	case 'p':
	    if (ch->position != POS_STANDING
		&& ch->position != POS_FIGHTING) sprintf(buf2, "(%s)",
							 position_table
							 [ch->position].
							 short_name);
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	case 'P':
	    if (ch->position != POS_STANDING
		&& ch->position != POS_FIGHTING) sprintf(buf2, "(%s)",
							 position_table
							 [ch->position].
							 name);
	    else
		sprintf(buf2, " ");
	    i = buf2;
	    break;
	}
	++str;
	while ((*point = *i) != '\0')
	    ++point, ++i;
    }
    *point = '\0';
    pbuff = buffer;
    colourconv(pbuff, buf, ch);
    write_to_buffer(ch->desc, buffer, 0);

    if (ch->prefix[0] != '\0')
	write_to_buffer(ch->desc, ch->prefix, 0);
    return;
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer(DESCRIPTOR_DATA * d, const char *txt, long length)
{
    /*
     * Find length in case caller didn't.
     */
    if (length <= 0)
	length = strlen(txt);
	//printf("strlen = %ld\n", length);

    /*
     * Initial \n\r if needed.
     */
    if (d->outtop == 0 && !d->fcommand) {
	d->outbuf[0] = '\n';
	d->outbuf[1] = '\r';
	d->outtop = 2;
    }
	//printf("crash here?\n");
    /*
     * Expand the buffer as needed.
     */
    while (d->outtop + length >= d->outsize) {
	char *outbuf;

	if (d->outsize >= 64000) {
	    bug("Buffer overflow. Closing.\n\r", 0);
	    close_socket(d);
	    return;
	}
	//printf("allocing mem in wtb\n");
	outbuf = alloc_mem(2 * d->outsize);
	strncpy(outbuf, d->outbuf, d->outtop);
	//printf("freeing mem in wtb\n");
	free_mem(d->outbuf);
	d->outbuf = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.
     */
	//printf("about to strcpy\n");
    strcpy(d->outbuf + d->outtop, txt);
	//printf("d-outtop\n");
    d->outtop += length;

	//printf("returning from wtb\n");
    return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor(int desc, char *txt, int length, bool color)
{
    int iStart;
    int nWrite;
    int nBlock;

    if (length <= 0)
	length = strlen(txt);

    for (iStart = 0; iStart < length; iStart += nWrite) {
	nBlock = UMIN(length - iStart, 4096);
	if ((nWrite = write(desc, txt + iStart, nBlock)) < 0) {
	    perror("Write_to_descriptor");
	    return FALSE;
	}
    }

    return TRUE;

}

/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect(DESCRIPTOR_DATA * d, char *name, bool fConn)
{
    CHAR_DATA *ch;

    for (ch = char_list; ch != NULL; ch = ch->next) {
	if (!IS_NPC(ch)
	    && (!fConn || ch->desc == NULL)
	    && !str_cmp(d->character->name, ch->name)) {
	    if (fConn == FALSE) {
		free_string(d->character->pcdata->pwd);
		d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
	    } else {
		free_char(d->character);
		d->character = ch;
		ch->desc = d;
		ch->timer = 0;
		send_to_char("Reconnecting.\n\r", ch);
		do_replay(ch, "");
		act("$n has reconnected.", ch, NULL, NULL, TO_ROOM);

		sprintf(log_buf, "%s@%s reconnected.", ch->name, d->host);
		log_string(log_buf);
		wiznet("$N groks the fullness of $S link.",
		       ch, NULL, WIZ_LINKS, 0, 0);
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing(DESCRIPTOR_DATA * d, char *name)
{
    DESCRIPTOR_DATA *dold;

    for (dold = descriptor_list; dold; dold = dold->next) {
	if (dold != d
	    && dold->character != NULL
	    && dold->connected != CON_GET_NAME
	    && dold->connected != CON_GET_OLD_PASSWORD
	    && !str_cmp(name, dold->original
			? dold->original->name : dold->character->name)) {
	    write_to_buffer(d, "That character is already playing.\n\r",
			    0);
	    write_to_buffer(d, "Do you wish to connect anyway (Y/N)?", 0);
	    d->connected = CON_BREAK_CONNECT;
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling(CHAR_DATA * ch)
{
    if (ch == NULL
	|| ch->desc == NULL
	|| ch->desc->connected != CON_PLAYING
	|| ch->was_in_room == NULL
	|| ch->in_room != get_room_index(ROOM_VNUM_LIMBO)) return;

    ch->timer = 0;
    char_from_room(ch);
    char_to_room(ch, ch->was_in_room);
    ch->was_in_room = NULL;
    act("$n has returned from the void.", ch, NULL, NULL, TO_ROOM);
    return;
}

void announce(char *string, CHAR_DATA * ch)
{
    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (d->connected == CON_PLAYING
	    && d->character != ch && IS_SET(victim->comm, COMM_ANNOUNCE)
	    && !IS_SET(victim->comm, COMM_QUIET)) {
	    if (!can_see(d->character, ch) && IS_IMMORTAL(ch))
		continue;

	    act_new(string, ch, NULL, d->character, TO_VICT, POS_DEAD);
	}
    }
    return;
}

/*
 * Write to one char.
 */

void send_to_char_bw(const char *txt, CHAR_DATA * ch)
{
    if (txt != NULL && ch->desc != NULL)
	write_to_buffer(ch->desc, txt, strlen(txt));
    return;
}

// send colored text to a descriptor
void write_color_to_buffer(const char *txt, DESCRIPTOR_DATA *desc)
{
    const char *point;
    char *point2;
    char buf[MAX_STRING_LENGTH * 4];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if (txt && desc) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, NULL, point2);
		    while (skip-- > 0)
			++point2;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    write_to_buffer(desc, buf, point2 - buf);
    }
    return;
}


/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char(const char *txt, CHAR_DATA * ch)
{
    const char *point;
    char *point2;
    char buf[MAX_STRING_LENGTH * 4];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if (txt && ch->desc) {
	if (IS_SET(ch->act, PLR_COLOUR)) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, ch, point2);
		    while (skip-- > 0)
			++point2;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    write_to_buffer(ch->desc, buf, point2 - buf);
	} else {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    write_to_buffer(ch->desc, buf, point2 - buf);
	}
    }
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char_bw(const char *txt, CHAR_DATA * ch)
{
    if (txt == NULL || ch->desc == NULL)
	if (ch->lines == 0) {
	    send_to_char(txt, ch);
	    return;
	}
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head, txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc, "");
}

/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char(const char *txt, CHAR_DATA * ch)
{
    const char *point;
    char *point2;
    char buf[MAX_STRING_LENGTH * 32 ];
    int skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if (txt && ch->desc) {
	if (IS_SET(ch->act, PLR_COLOUR)) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, ch, point2);
		    while (skip-- > 0)
			++point2;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    ch->desc->showstr_head = alloc_mem(strlen(buf) + 1);
	    strcpy(ch->desc->showstr_head, buf);
	    ch->desc->showstr_point = ch->desc->showstr_head;
	    show_string(ch->desc, "");
	} else {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    ch->desc->showstr_head = alloc_mem(strlen(buf) + 1);
	    strcpy(ch->desc->showstr_head, buf);
	    ch->desc->showstr_point = ch->desc->showstr_head;
	    show_string(ch->desc, "");
	}
    }
    return;
}


/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
    char buffer[4 * MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;

    one_argument(input, buf);
    if (buf[0] != '\0') {
	if (d->showstr_head) {
	    free_mem(d->showstr_head);
	    d->showstr_head = 0;
	}
	d->showstr_point = 0;
	return;
    }
    if (d->character)
	show_lines = d->character->lines;
    else
	show_lines = 0;

    for (scan = buffer;; scan++, d->showstr_point++) {
	if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
	    && (toggle = -toggle) < 0)
	    lines++;

	else if (!*scan || (show_lines > 0 && lines >= show_lines)) {
	    *scan = '\0';
	    write_to_buffer(d, buffer, strlen(buffer));
	    for (chk = d->showstr_point; isspace(*chk); chk++);
	    {
		if (!*chk) {
		    if (d->showstr_head) {
			free_mem(d->showstr_head);
			d->showstr_head = 0;
		    }
		    d->showstr_point = 0;
		}
	    }
	    return;
	}
    }
    return;
}


/* quick sex fixer */
void fix_sex(CHAR_DATA * ch)
{
    if (ch->sex < 0 || ch->sex > 2)
	ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

void
act(const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2,
    int type)
{
    /* to be compatible with older code */
    act_new(format, ch, arg1, arg2, type, POS_RESTING);
}

/*
 * The colour version of the act_new( ) function, -Lope
 */
void
act_new(const char *format, CHAR_DATA * ch, const void *arg1,
	const void *arg2, int type, int min_pos)
{
    static char *const he_she[] = { "it", "he", "she" };
    static char *const him_her[] = { "it", "him", "her" };
    static char *const his_her[] = { "its", "his", "her" };

    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
    const char *str;
    char *i = NULL;
    char *point;
    char *pbuff;
    char buffer[MAX_STRING_LENGTH * 2];
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    bool fColour = FALSE;

    /*
     * Discard null and zero-length messages.
     */
    if (!format || !*format)
	return;

    /* discard null rooms and chars */
    if (!ch || !ch->in_room)
	return;

    to = ch->in_room->people;
    if (type == TO_VICT) {
	if (!vch) {
	    bug("Act: null vch with TO_VICT.", 0);
	    return;
	}
	if (!vch->in_room)
	    return;

	to = vch->in_room->people;
    }
    for (; to; to = to->next_in_room) {
	if ((!IS_NPC(to) && to->desc == NULL)
	    || (IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT)
		&& to->desc == NULL) || to->position < min_pos)
	    continue;

	if ((type == TO_CHAR) && to != ch)
	    continue;
	if (type == TO_VICT && (to != vch || to == ch))
	    continue;
	if (type == TO_ROOM && to == ch)
	    continue;
	if (type == TO_NOTVICT && (to == ch || to == vch))
	    continue;

	point = buf;
	str = format;
	while (*str != '\0') {
	    if (*str != '$') {
		*point++ = *str++;
		continue;
	    }
	    fColour = TRUE;
	    ++str;
	    i = " <@@@> ";
	    if (!arg2 && *str >= 'A' && *str <= 'Z') {
		bug("Act: missing arg2 for code %d.", *str);
		i = " <@@@> ";
	    } else {
		switch (*str) {
		default:
		    bug("Act: bad code %d.", *str);
		    i = " <@@@> ";
		    break;
		case 't':
		    i = (char *) arg1;
		    break;
		case 'T':
		    i = (char *) arg2;
		    break;
		case 'n':
		    i = PERS(ch, to);
		    break;
		case 'N':
		    i = PERS(vch, to);
		    break;
		case 'e':
		    i = he_she[URANGE(0, ch->sex, 2)];
		    break;
		case 'E':
		    i = he_she[URANGE(0, vch->sex, 2)];
		    break;
		case 'm':
		    i = him_her[URANGE(0, ch->sex, 2)];
		    break;
		case 'M':
		    i = him_her[URANGE(0, vch->sex, 2)];
		    break;
		case 's':
		    i = his_her[URANGE(0, ch->sex, 2)];
		    break;
		case 'S':
		    i = his_her[URANGE(0, vch->sex, 2)];
		    break;

		case 'p':
		    i = can_see_obj(to, obj1)
			? obj1->short_descr : "something";
		    break;

		case 'P':
		    i = can_see_obj(to, obj2)
			? obj2->short_descr : "something";
		    break;

		case 'd':
		    if (arg2 == NULL || ((char *) arg2)[0] == '\0') {
			i = "door";
		    } else {
			one_argument((char *) arg2, fname);
			i = fname;
		    }
		    break;
		}
	    }

	    ++str;
	    while ((*point = *i) != '\0')
		++point, ++i;
	}
	*point++ = '\n';
	*point++ = '\r';
	*point = '\0';

	if (strncmp(buf, "{", 1)) {
	    buf[0] = UPPER(buf[0]);
	} else {
	    buf[2] = UPPER(buf[2]);
	}
	pbuff = buffer;
	colourconv(pbuff, buf, to);


	if (to->desc != NULL)
	    write_to_buffer(to->desc, buffer, 0);
	else if (MOBtrigger)
	    mp_act_trigger(buffer, to, ch, arg1, arg2, TRIG_ACT);

    }

    return;
}


void log_printf(char *fmt, ...)
{
    char buf[2 * MSL];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    log_string(buf);
}


int colour(char type, CHAR_DATA * ch, char *string)
{
    char code[20];
    char *p = '\0';

    char ccodes[16] = { 'x','b','c','g','m','r','w','y','B','C','G','M','R','W','Y','D' };

    if (type == '!')
      type = ccodes[number_range(0,15)];

    switch (type) {
      default:
			  sprintf(code, CLEAR);
		  break;
      case 'x':
			  sprintf(code, CLEAR);
			break;
		  case 'b':
			  sprintf(code, C_BLUE);
			break;
		  case 'c':
			  sprintf(code, C_CYAN);
			break;
		  case 'g':
			  sprintf(code, C_GREEN);
			break;
		  case 'm':
			  sprintf(code, C_MAGENTA);
			break;
		  case 'r':
			  sprintf(code, C_RED);
			break;
		  case 'w':
			  sprintf(code, C_WHITE);
			break;
		  case 'y':
			  sprintf(code, C_YELLOW);
			break;
		  case 'B':
			  sprintf(code, C_B_BLUE);
			break;
		  case 'C':
			  sprintf(code, C_B_CYAN);
			break;
		  case 'G':
			  sprintf(code, C_B_GREEN);
			break;
		  case 'M':
			  sprintf(code, C_B_MAGENTA);
			break;
		  case 'R':
			  sprintf(code, C_B_RED);
			break;
		  case 'W':
			  sprintf(code, C_B_WHITE);
			break;
		  case 'Y':
			  sprintf(code, C_B_YELLOW);
			break;
		  case 'D':
			  sprintf(code, C_D_GREY);
			break;
		  case '*':
			  sprintf(code, "%c", 007);
			break;
		  case 'u':
			  sprintf(code, "%c", 129);
			break;
		  case 's':
			  sprintf(code, "%c", 001);
			break;
		  case 'S':
		  	sprintf(code, "%c", 002);
			break;
		  case '/':
			  sprintf(code, "%c", 012);
			break;
		  case '{':
			  sprintf(code, "%c", '{');
			break;
    }

    p = code;
    while (*p != '\0') {
	*string = *p++;
	*++string = '\0';
    }

    return (strlen(code));
}

void colourconv(char *buffer, const char *txt, CHAR_DATA * ch)
{
    const char *point;
    int skip = 0;

    if (ch->desc && txt) {
	if (IS_SET(ch->act, PLR_COLOUR)) {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    skip = colour(*point, ch, buffer);
		    while (skip-- > 0)
			++buffer;
		    continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	    }
	    *buffer = '\0';
	} else {
	    for (point = txt; *point; point++) {
		if (*point == '{') {
		    point++;
		    continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	    }
	    *buffer = '\0';
	}
    }
    return;
}

/************** START OF HOT REBOOT **************/

#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/* This file holds the copyover data */

#define COPYOVER_FILE "copyover.data"

/* This is the executable file */

#define EXE_FILE	  "../src/rom"


/*  Copyover - Original idea: Fusion of MUD++
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
 *  http://pip.dknet.dk/~pip1773
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
void do_genesi(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to GENESIS, spell it out, you moron.\n\r",
		 ch);
    return;
}

void do_genesis(CHAR_DATA * ch, char *argument)
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next;
    char buf[MAX_STRING_LENGTH], buf2[100];
    extern int port, control;	/* db.c */
    bool color;
    color = FALSE;

    if (IS_NPC(ch))
	return;

    fp = fopen(COPYOVER_FILE, "w");

    if (!fp) {
	send_to_char("Genesis file not writeable, aborted.\n\r", ch);

	log_printf("Could not write to genesis file: %s", COPYOVER_FILE);

	perror("do_genesis:fopen");
	return;
    }
    sprintf(buf,
	"\n\n%s summons the powers of the three moons and a violent wind arises.."
	"You feel yourself imploding violently, and you scream your last breath as "
	"your molecules begin to scatter!\n\n ", ch->name);

    /* For each playing descriptor, save its state */
    for (d = descriptor_list; d; d = d_next) {
	CHAR_DATA *och = CH(d);
	d_next = d->next;	/* We delete from the list , so need to save this */


	if (!d->character || d->connected > CON_PLAYING) {	/* drop those logging on */
	    write_to_descriptor(d->descriptor,
				"\n\rSorry, we are rebooting. Come back in a few minutes.\n\r",
				0, color);
	    close_socket(d);	/* throw'em out */
	} else {
	    fprintf(fp, "%d %s %s\n", d->descriptor, och->name, d->host);

	    if (d->connected == CON_PLAYING && d->pString && d->character)
		interpret(d->character, "@");

	    if (och->level < 2)
		    och->level = 2;

      if (ch->arena)
        leave_arena(ch);

	    save_char_obj(och);

	    write_to_descriptor(d->descriptor, buf, 0, color);
	}
    }

    if (argument[0] == '\0' || str_prefix(argument, "nosave"))
	do_asave(NULL, "");	/* OLC */

	turf_save();
	cbank_save();
	persist_container( 123);

    fprintf(fp, "-1\n");
    fclose(fp);

    /* Close reserve and other always-open files and release other resources */

    fclose(fpReserve);

    /* exec - descriptors are inherited */

    sprintf(buf, "%d", port);
    sprintf(buf2, "%d", control);
    execl(EXE_FILE, "rom", buf, "copyover", buf2, (char *)
	  NULL);

    /* Failed - sucessful exec will not return */

    perror("do_genesis: execl");
    send_to_char("Genesis FAILED!\n\r", ch);

    /* Here you might want to reopen fpReserve */
    fpReserve = fopen(NULL_FILE, "r");
}

/* Recover from a copyover - load players */
void copyover_recover()
{
    DESCRIPTOR_DATA *d;
    FILE *fp;
    char name[100];
    char host[MSL];
    int desc;
    bool fOld;
    bool color;
    color = FALSE;

    log_printf("Genesis recovery initiated");

    fp = fopen(COPYOVER_FILE, "r");

    if (!fp) {			/* there are some descriptors open which will hang forever then ? */
	perror("genesis_recover:fopen");
	log_printf("Genesis file not found. Exitting.\n\r");
	exit(1);
    }
    unlink(COPYOVER_FILE);	/* In case something crashes - doesn't prevent reading  */

    for (;;) {
	fscanf(fp, "%d %s %s\n", &desc, name, host);
	if (desc == -1)
	    break;

	/* Write something, and check if it goes error-free */
	if (!write_to_descriptor
	    (desc,
	     "Please hold as your body is put back together...done!\n\r",
	     0, color)) {
	    close(desc);	/* Nope */
	    continue;
	}
	d = new_descriptor();
	d->descriptor = desc;

	d->host = str_dup(host);
	d->next = descriptor_list;
	descriptor_list = d;
	d->connected = CON_COPYOVER_RECOVER;	/* -15, so close_socket frees the char */


	/* Now, find the pfile */

	fOld = load_char_obj(d, name);

	if (!fOld) {		/* Player file not found?! */
	    write_to_descriptor(desc,
				"\n\rSomehow, your character was lost in the copyover. Sorry.\n\r",
				0, color);
	    close_socket(d);
	} else {		/* ok! */
	    write_to_descriptor(desc, "Have a nice day.\n\r", 0, color);

	    /* Just In Case */
	    if (!d->character->in_room)
		d->character->in_room = get_room_index(ROOM_VNUM_TEMPLE);

	    /* Insert in the char_list */
	    d->character->next = char_list;
	    char_list = d->character;

	    char_to_room(d->character, d->character->in_room);
	    do_look(d->character, "auto");
	    send_to_char("{x", d->character);
	    act("$n materializes!{x", d->character, NULL, NULL, TO_ROOM);
	    d->connected = CON_PLAYING;

	    if (d->character->pet != NULL) {
		char_to_room(d->character->pet, d->character->in_room);
		act("$n materializes before you!{x", d->character->pet,
		    NULL, NULL, TO_ROOM);
	    }
	}

    }


}

char *spritespeak(char *string, CHAR_DATA * ch)
{

    int pos;
    char buf[MSL];
    char buf2[MSL];

    if (ch->race != race_lookup("sprite"))
	    return string;

    strcpy(buf, string);

    for (pos = 0; buf[pos] != '\0'; pos++) {
	    if (dice(1, 100) > 50)
	      buf2[pos] = UPPER(buf[pos]);
	    else
	      buf2[pos] = buf[pos];
    }

    buf[0] = '\0';
    buf2[pos] = '\0';

    return str_dup(buf2);
}
