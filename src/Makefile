CC      = gcc
C_FLAGS = -O -pipe -ggdb -Wall 
L_FLAGS = -lcrypt

O_FILES = act.comm.o	\
		act.enter.o		\
		act.info.o		\
		act.move.o		\
		act.obj.o		\
		act.wiz.o		\
		alias.o			\
	    arena.o 		\
	    attacks.o 		\
	    attack_stacks.o	\
		ban.o			\
		bank.o			\
		bit.o			\
		chargen.o		\
		classes.o		\
		comm.o			\
		const.o			\
		containers.o	\
		db.o			\
		db2.o			\
		drunk.o			\
		dump.o			\
		effects.o		\
		fight.o			\
		flags.o			\
		guide.o			\
		handler.o		\
		healer.o		\
		hunger.o		\
		ignore.o		\
		interp.o		\
		lookup.o		\
		magic.o			\
		mem.o			\
		mob.cmds.o		\
		mob.memory.o	\
		mob.prog.o		\
		music.o			\
		new.spell.effects.o	\
		new_skills.o	\
		note.o			\
		olc.o			\
		olc.aedit.o		\
		olc.del.o		\
		olc.hedit.o		\
		olc.medit.o		\
    	olc.mpcode.o	\
		olc.oedit.o		\
		olc.raedit.o	\
		olc.redit.o		\
		olc.save.o		\
		olc.util.o		\
		races.o			\
		recycle.o		\
		remort.o		\
		repair.o		\
		save.o			\
		scan.o			\
		score.o			\
		skill.defs.o	\
		skills.o		\
		special.o		\
		spells.o		\
		string.o		\
		tables.o		\
		turf.o			\
		update.o		\
		utils.o			\
		vortex.o

rom: $(O_FILES)
	@ rm -f rom
	$(CC) -Ddebug -o rom $(O_FILES) $(L_FLAGS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean: 
	@rm -f *~ *.o rom

distclean : rom
	@rm -f *~ *.o

