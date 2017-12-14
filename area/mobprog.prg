#MOBPROGS
#1
mob delay 100
~
#2
IF ISPC $n
 IF ISGOOD $n
~
#400
say Eh?
emote unravels the scrolls and peers intently at them.
say Hah!  Boys, we got 'em!  Time to set sail!
mob echo The ship suddenly goes into a flurry of activity as they prepare to set sail.
say If ye be needin passage back to Midgaard, we pass by it and can drop you off at the Levee.  Just say ye want passage.
mob junk scroll
if LEVEL $n < 20
mob oload 499
drop bag
get bag
give 700 gold $n
mob echoat $n The captain gives you a hard pat on the shoulder.
endif
~
#401
mob echo The ship disembarks from the port and sails into the calm waters of the Dragon Sea.
mob echo Soon you find yourself disembarking from the ship.  The captain wishes you the best of luck and departs.
mob transfer all 3049
mob at 3049 mob echo $n disembarks from a ship.
~
#416
say Eh?
emote unravels the scrolls and peers intently at them.
say Hah!  Boys, time to set sail for new land!
if LEVEL $n >= 20
~
#600
mob echoat $n Diana looks at the chest and smiles at you.
mob echoaround $n Diana looks at the chest and smiles at $n.
say Thank you for delivering this to me.
if LEVEL $n >= 20
wave $n
mob junk chest
else
 if LEVEL $n < 20
  if RAND 15
  mob junk chest
  mob oload 680
  say Here is some gold for your troubles.  Farewell.
  drop purse
  get purse
  give 500 gold $n
  else
   if RAND 1
   mob junk chest
   say Please deliver a message for me.  The stockpiles of brass armor have replenished, and Ofcol is more than willing to exchange armor with Midgaard.
   mob oload 681
   give parchment $n
   mob cast regeneration $n
   mob cast refresh $n
   mob cast sanctuary $n
   say Safe journey to you.
   wave $n
   endif
  endif
 endif
endif
~
#1256
~
#1260
if NAME $n Gohan
  say Welcome Son Gohan.
  Hug Gohan
  Kiss Gohan
else
  say What the hell are you doing here?
  Glare $n
Endif
~
#1261
if NAME $n Gohan
  emote sits down on {Wa {Ds{Wi{wl{Wk {Db{We{wd along with Gohan.
      massage Gohan
  say Had a hard day, baby?
  Mob echo Gohan says 'Much better now.'
else
  endif
~
#1300
mob echo {x
if align $n > -350
  if level $n < 50
    mob echoat     $n {DImpenetrable darkness shrouds you tight.{X
    mob echoaround $n {DImpenetrable darkness shrouds $n tight.{X
    mob transfer $n 1400
  else
    if rand 50
      if race $n reddrg
      or race $n grndrg
      or race $n bludrg
      or race $n blkdrg
      or race $n glddrg
        mob echo A {Dshadow {cguardian{X whispers 'pray I won't cut off your wings $n'{X
        mob echo A {Dshadow {cguardian{X whispers 'you will need them to fly to heaven soon..'{X
      else
        if race $n demon
        or race $n vampire
        or race $n wraith
          mob echo A {Dshadow {cguardian{X whispers 'prepare for the hell fires $n'{X
          mob echo A {Dshadow {cguardian{X whispers 'soon I'll feed upon your souless corpse..'{X
        else
          if race $n minotaur
          or race $n drow
          or race $n wolf
            mob echo A {Dshadow {cguardian{X whispers 'the time to die is at hand..'{X
          else
            if race $n sprite
              mob echo A {Dshadow {cguardian{X whispers 'welcome creature of light'{X
              mob echo A {Dshadow {cguardian{X whispers 'which you will never see again..'{X
            else
              if race $n chicken
                mob echo A {Dshadow {cguardian{X whispers 'I'm a very lucky guardian'{X
                mob echo A {Dshadow {cguardian{X whispers 'I'll be eating chicken soup tonight..'{X
              endif
            endif
          endif
        endif
      endif
    else
      if rand 33
        mob echo A {Dshadow {cguardian{X whispers 'death..'{X
        mob echo A {Dshadow {cguardian{X whispers 'is the ultimate adventure..'{X
      else
        if rand 50
          mob echo A {Dshadow {cguardian{X stares into your soul and screams inaudible.{X
        else
          mob echo A {Dshadow {cguardian{X steps out of the shadows blocking your path.{X
        endif
      endif
    endif
    if uses $i weapon
      mob echo {x
      mob echo {MS{Cp{Ma{Cr{Mk{Cs {Wof {MU{Cl{Mt{Cr{Ma {Cv{Mi{Co{Ml{Ce{Mt {Wburst into the air as a {DShadow {cGuardian {Wdraws its blade!{X
      mob echo {x
    endif
    mob kill $n
  endif
else
  if rand 33
    mob echo A {Dshadow {cguardian{X whispers 'greetings evil being'{X 
    mob echo A {Dshadow {cguardian{X whispers 'you may pass..'{X
  else
    if rand 50
      mob echo A {Dshadow {cguardian{X stares into your soul and nods in silent greeting.{X
    else
      mob echo A {Dshadow {cguardian{X fades into the shadows, letting you pass.{X
    endif
  endif
endif
~
#1807
if carries $i 'heart shabbath'
	,bellows in fury!
	if mobhere 1809
		say KILL $m!!
		mob force "blood lurcher" mob kill $n
		mob force "2.blood lurcher" mob kill $n
	endif
	mob kill $n
else
	mob oload 1826
	say To me my servants!
	mob at "blood lurcher" mob echo The Blood Lurcher disappears with a flash and a sizzle!
	mob transfer "blood lurcher"
	mob at "2.blood lurcher" mob echo The Blood Lurcher disappears with a flash and a sizzle!
	mob transfer "2.blood lurcher"
	if mobhere 1809
		mob echo A sizzling sound erupts behind you!
		mob force "blood lurcher" mob kill $n
		mob force "2.blood lurcher" mob kill $n
		say Now DIE mortal!
		mob kill $n
	else
		emote howls {R"My minions have been slain!  YOU WILL BURN FOR THAT!"{x
		mob kill $n
	endif
endif
~
#1808
if room $i == 1821
mob echo Shabbath N'or bellows in agony {R"K'zik mk'a'ri dur'osh!"{x
mob echo Shabbath N'or disappears in a blinding flash!
mob echo From the east, a tremendous crash echos out.
mob goto 1820
mob echo Shabbath N'or appears in a blinding flash, bellowing in fury!
endif
~
#1809
emote howls {r"You cannot slay me!  I will be BACK for you {R$q...{x
mob zecho A tremendous wailing rips through the air, and rocks fall from the ceiling.
mob echo Shabbath N'or falls with a massive thump, his carcass sparking. 
~
#1814
if race $n dwarf
say A dwarf!  Greetings cousin!
smile $n
else
if race $n pixie
point $n
say Look how tiny $e is!
else
say A giant for dinner!  Brothers, aid me!
mob kill $n
endif
~
#2100
say Thank you my child!  May your generosity be rewarded!
MOB ECHOAT $n The beggar reaches out with a filthy hand and touches your arm.
MOB ECHOAT $n A {Cblue glow{x surrounds you, filling you with a sudden warmth!
MOB ECHOAROUND $n The beggar reaches out and touches $n, surrounding $m with a blue light!
MOB CAST shield $n
~
#2101
say May the rest of your existence be {rcursed{x $n!
MOB CAST curse $n
MOB CAST blindness $n
MOB CAST 'energy drain' $n
MOB CAST plague $n
MOB CAST poison $n
MOB CAST lethargy $n
MOB CAST weaken $n
~
#2102
MOB ECHOAT $n Lanaska grabs your palm and peers intently at it.
MOB ECHOAROUND $n Lanaska grabs $n's palm and studies it intently.
if rand 50
 if rand 50
  say Ahh.. your palms shall overflow with gold, but over gold you shall have no dominion..
 else
  frown
  say oh my.. before this day is out, you shall be wearing a duck.
 endif
else
 if rand 50
  say I see your charity spilling forth to just reward!
  MOB ECHOAT $n Lanaska winks slyly at you.
  MOB ECHOAROUND $n Lanaska winks slyly at $n.
 else
  say You are a highly intelligent person and that shall lead to your success!
 endif
endif
~
#2103
say Oh child, surely you can spare more than THAT for a look into the future?
~
#2104
emote tosses a handful of runes to the ground and studies them.
if rand 50
 if rand 50
  say To be a rich man is not to fill your pockets with gold.
  say Before the week is through, you will understand.
  eg
 else
  gasp
  say Avoid the forsaken streets of Xeryl!  Preserve your soul!
 endif
else
 if rand 50
  say Search and search and search you will..
  say and you will find you carried the answer with you the entire time!
 else
  say To the sea you must go, for the waves are your destiny!
 endif
endif
~
#2105
emote scoffs.
say I am a caster of runes, not a house of charity!  Read the sign!
~
#2106
emote grins and picks up his large mallet..
if rand 50
 if rand 50
  MOB ECHOAT $n Phyllo pounds your skull with his mallet!
  MOB ECHOAROUND $n Phyllo pounds $n on the head with a mallet!
  say Now yoo is cool, calm an' ko-lek-tid.
 else
  MOB ECHOAT $n Phyllo smashes your skull with a mallet!
  MOB ECHOAROUND $n Phyllo smashes $n's skull with a mallet!
  say Yoo a gentul spirit and got goods persunul hi-gene!
 endif
else
 if rand 50
  MOB ECHOAT $n Phyllo whomps your head with his mallet.
  MOB ECHOAROUND $n Phyllo whomps $n over the head with his mallet.
  say Now yoo is assertive and forthrite!
 else
  MOB ECHOAT $n Phyllo beats you upside the head with a mallet!
  MOB ECHOAROUND $n Phyllo beats $n upside the head with a mallet!
  say You forceful persun-alitee wid lotsa n-ur-gee!
 endif
endif
~
#2107
say Bah! For THAT bit of change, I just thump you!
MOB ECHOAT $n Phyllo bashes you over the head.  Ow..
MOB ECHOAROUND $n Phyllo bashes $n over the head. Ow..
~
#2108
emote clucks madly and attacks the fence!  {RCluCk cLUcK ClUcK!{x
~
#2201
mob echoat $n {YYou violently rip the {YH{yydra{Y's head off!  Blood everywhere!{X
mob echoaround $n {Y$n violently rips the {YH{yydra{Y's head off!  Blood everywhere!{X
mob echo {X 
mob echo {YTwo heads grow from the {Rb{rl{Ro{ro{Rd {Rs{rp{Re{rw{Ri{rn{Rg {Rn{re{Rc{rk{Y, screeching their wrath at you!{X
mob mload 2246
mob force hydra2 mob kill $n
mob goto 1
~
#2202
mob echoat $n {YYou violently rip the {WH{Dydra{Y's head off!  Blood everywhere!{X
mob echoaround $n {Y$n violently rips one of the {WH{Dydra{Y's head off!  Blood everywhere!{X
mob echo {X 
mob echo {YTwo heads grow from the {Rb{rl{Ro{ro{Rd {Rs{rp{Re{rw{Ri{rn{Rg {Rn{re{Rc{rk{Y, screeching their wrath at you!{X
mob mload 2247
mob force hydra3 mob kill $n
mob goto 1
~
#2203
mob echoat $n {YYou violently rip the {RH{rydra{Y's head off!  Blood everywhere!{X
mob echoaround $n {Y$n violently rips one of the {RH{rydra{Y's head off!  Blood everywhere!{X
mob echo {X 
mob echo {YTwo heads grow from the {Rb{rl{Ro{ro{Rd {Rs{rp{Re{rw{Ri{rn{Rg {Rn{re{Rc{rk{Y, screeching their wrath at you!{X
mob mload 2248
mob force hydra4 mob kill $n
mob goto 1
~
#2204
mob echoat $n {YYou violently rip the {GH{gydra{Y's head off!  Blood everywhere!{X
mob echoaround $n {Y$n violently rips one of the {GH{gydra{Y's head off!  Blood everywhere!{X
mob echo {X 
mob echo {YTwo heads grow from the {Rb{rl{Ro{ro{Rd {Rs{rp{Re{rw{Ri{rn{Rg {Rn{re{Rc{rk{Y, screeching their wrath at you!{X
mob mload 2249
mob force hydra5 mob kill $n
mob goto 1
~
#2205
mob echoat $n {YYou violently rip the {CH{cydra{Y's head off!  Blood everywhere!{X
mob echoaround $n {Y$n violently rips one of the {CH{cydra{Y's head off!  Blood everywhere!{X
mob echo {X 
mob echo {YTwo heads grow from the {Rb{rl{Ro{ro{Rd {Rs{rp{Re{rw{Ri{rn{Rg {Rn{re{Rc{rk{Y, screeching their wrath at you!{X
mob mload 2250
mob force hydra6 mob kill $n
mob goto 1
~
#2206
mob echoat $n {YYou violently rip the {BH{bydra{Y's head off!  Blood everywhere!{X
mob echoaround $n {Y$n violently rips one of the {BH{bydra{Y's head off!  Blood everywhere!{X
mob echo {X 
mob echo {YTwo heads grow from the {Rb{rl{Ro{ro{Rd {Rs{rp{Re{rw{Ri{rn{Rg {Rn{re{Rc{rk{Y, screeching their wrath at you!{X
mob mload 2226
mob force hydra7 mob kill $n
mob goto 1
~
#2207
mob echoat $n {YYou violently rip the {MH{mydra{Y's heart out, finding its true weak spot!{X
mob echoaround $n {Y$n violently rips the Hydra{Y's heart out, finding its true weak spot!{X
mob at 1 mob purge

if rand 50
  if rand 50
    mob oload 2213
  else
    mob oload 2214
  endif
endif

mob oload 2234

if ispc $n
  if carries $n 2288
    if level $n < 181
      if rand 50
        mob oload 2215
        mob at 2209 drop note
      endif
    endif
  endif
endif
~
#2208
if money $n > 9999999
  mob echo {yThe Draconian Queen says 'Tis so good to hear the jingling of'{X
  mob echo {yThe Draconian Queen says 'your success $n'{X
  mob echoat $n The Draconian Queen eyes your bag of coins.{X
  mob echoaround $n The Draconian Queen eyes $n's bag of coins.{X
  mob echo {yThe Draconian Queen says 'Would you like to see my new earrings?'{X
else
  if rand 20
    mob echoat $n The Draconian Queen smirks as she looks down her nose at you.{X
    mob echoaround $n The Draconian Queen smirks as she looks down her nose at $n.{X
    mob echo {yThe Draconian Queen hisses 'If you weren't such a pauper $n,'{X
    mob echo {yThe Draconian Queen hisses 'I might let you see my earrings.'{X
  endif
endif
~
#2209
if money $n > 9999999
  mob echo The Draconian Queen sweeps her hair to the side and shows you 
  mob echo a pair of {YG{Wl{Yi{Wt{Yt{We{Yr{Wi{Yn{Wg {BS{ba{Bp{bp{Bh{bi{Br{be {WE{wa{Wr{wr{Wi{wn{Wg{ws.{X
  mob echo {yThe Draconian Queen says 'It fills me with such grief knowing'{X
  mob echo {yThe Draconian Queen says 'I have to part from them.'{X
endif
~
#2210
if money $n > 9999999
  if carries $n 2215
    frown $n
  else 
    mob echoat $n The Draconian Queen looks around nervously, being sure no one sees 
    mob echoat $n as she places a small note into your pocket.{X
    mob oload 2215
    give note $n
  endif
endif
~
#2211
if carries $n 2215
  mob at 1 drop 10000000 gold
  mob oload 2216
  give earring $n
  mob echo {yThe Draconian Queen says 'Thank you so much $n!'{X
  mob echo The Draconian Queen quickly leaves to plan her escape..{X
  mob goto 1
  mob purge gcash
endif
~
#2212
mob oload 2211 1 r
mob echo {/{WAs the last light of life in $I'{Ws eyes fades, you notice this treasury holds
mob echo {Wthe {Yr{yi{Yc{yh{Ye{ys{W of the {gDraconian Kingdom{W, just lying there for you to plunder.{x{/
~
#2400
MOB ECHO The security gate fizzles away.
MOB ECHOAT $n A magical force pushes you into the complex!
MOB ECHOAROUND $n $n is tossed into the complex by an unseen force!
MOB TRANSFER $n 2402
MOB AT 2402 MOB ECHOAROUND $n $n is tossed through the gate!
MOB ECHO The security gate snaps back into existence.
~
#3000
if mobexists zombie
	mob goto zombie
endif	
if affected zombie charm
	mob goto 3366
else
	mob purge zombie
	mob goto 3366
endif
~
#3001
if level $n < 20
say You looking for a job?
endif
~
#3002
IF POS $r sleeping
MOB FORCE ALL STAND
MOB ECHO The stray dog rushes up to the sleeping person and lets lose a huge golden shower, waking & spraying all in the vicinity.
ENDIF
~
#3003
mob echoat $n Stepping out the second-story window, you fall to the street below.{/
mob echoaround $n Stepping out the second-story window, $n falls to the street below.
mob trans $n 3024
mob echoaround $n Stepping out a second-story window, $n plummets to the cobbles at your feet.
~
#3004
mob echoat $q Stepping out the second-story window, you fall to the street below.
mob echoaround $q Stepping out the second-story window, $q falls to the street below.
mob trans $q 3024
~
#3005
mobecho The clerk shuffles through the sheets of paper on the counter.
~
#3012
if ISDELAY $i
 say I'll have a job for you in a few moments.
 else
 if LEVEL $n <= 20
   if RAND 80
   mob oload 3179
   emote pulls a document from under a stack of papers, causing all of them to fall on the ground.
   mutter
   say Bring this to the palace guard in New Thalos.  I'm sure the Sultan wil reward you nicely.
   give document $n
   mob DELAY 42
   ELSE
    if RAND 60
    mob oload 3180
    emote ducks under the counter, and, after fishing around for a bit, comes up with a small package.
    say This should have been delivered to King Tyerian awhile ago, but eh.
    emote shrugs, and says 'Off to Tyerian's Castle.'
    give package $n
    mob DELAY 42
    ELSE
     if RAND 40
     mob oload 3184
     emote shoves a few stacks of papers aside to reveal a small chest.
     say I trust you'll bring this to Marshall Diana of Ofcol....
     eye $n
     give chest $n
     mob DELAY 42
     ELSE
      if RAND 20
      mob oload 3186
      say Ah, this one is right in front of me.
      emote grabs some scrolls near the corner of the counter.
      give scrolls $n
      say Give this to one of the captains in Port of Aachen.  These need to be shipped out to...somewhere....
      mob DELAY 42
      ELSE 
       if RAND 0
       mob oload 3187
       emote grabs some random papers off of the counter.
       say These need to go to Werith.
       grin self
       give paper $n
       mob DELAY 42
       endif
      endif
     endif
    endif
   endif
  endif
endif
~
#3013
emote examines the document, then tosses it on a pile of similar-looking documents.
if level $n < 20
say Hmm, one of these days I'll take care of those.
say Thanks for the help.
mob oload 3183
drop bag
get bag
give 800 gold $n
mob junk response
else
 if LEVEL $n >= 20
 mutter
 say Hard to find good help these days.  Thanks for returning this.
 mob junk response
 endif
endif
~
#3014
pmote looks at $n curiously and askes 'What's this?'
emote reads the note and laughs.  'This oughta be fun.'
emote crumbles the paper into a wad and throws it in a trash can.
mob junk paper
if LEVEL $n < 20
say Thanks for your help.
mob oload 3183
mob oload 3183
drop bag
get bag
give 800 gold $n
else
 if LEVEL $n >= 20
 grumble
 endif
endif
~
#3015
if LEVEL $n < 20
cackle
say Boy, after all those sieges you'd think he'd mellow out.  But no, King Tyerian still has some spunk in him!
say Sorry if there was no payment.  I'll be sure you get something your next job.
endif
~
#3016
say Ah, that's good news.
mob echoat $n The clerk leans close to the bars and says softly, 'Just between you and me, if it wasn't for the Ofcol exchange, our cityguards would have a tough time against the city scum, if you catch my drift.'
chortle
mob junk parchment
if LEVEL $n < 20
mob oload 3183
drop bag
get bag
give 800 gold $n
say Thanks for your help.
endif
~
#3017
~
#3018
if LEVEL $n < 20
clasp $n
say Welcome.  I take it you're doing jobs for the clerk as well.
say If you need help with directions to areas, ask away.  Say the area and I'll provide directions.
endif
~
#3019
if LEVEL $n < 20
say Tyerian's Castle, eh?  Not a difficult path.  Head to Market Square, then keep going west until you find a place where you can go north, west, south, and east.  Head north and you're in.
say But before you go - when you're on that narrow trail to Tyerian's Castle, do NOT go southwards.
sigh
say Lost many runners there.  There's a swamp with vicious trolls and orcs to the south.  Be on your guard when you walk that path.
endif
~
#3020
if LEVEL $n < 20
say Ah, Port of Aachen.  That's a tricky path.
ponder
say Go to the south gate of Midgaard.  After you've gone there, head out, and quickest and least dangerous path to the Port is if you go immediately go all the way east, then keep heading south.
say The harbors are at the south end of the town.
say But beware of those mountain goblins.  Those things are quite aggressive and will attack you if they see you.
endif
~
#3021
if LEVEL $n < 20
say New Thalos.  That's a long haul.
say Head to Market Square, then keep going east.  It'll take quite some time before you hit the gates.
say If you're looking for the palace. you need to keep going east until you hit Center Marker Square.  The palace is on the northern path.  Look west for the palace guard.
endif
~
#3022
if LEVEL $n < 20
say Werith's Wayhouse?
grin $n
say A good place to have a good time.  Head to market square, then head east until you hit the Checkpoint.  Then take the southern path and you're there.
endif
~
#3023
if LEVEL $n < 20
say Ahh, Ofcol.  I haven't been there in a while.  Nice place.
say Head to the northern gate of Midgaard.  Then head north until you walking on the steep foothills.  Head east until you come across an intersection of three paths.  Take the north path, and you should be in Ofcol.
endif
~
#3024
if LEVEL $n < 20
mob echoat $n As you leave, the town clerk says, 'If you need directions, ask the runner.  The runner is usually at the Grunting Boar Inn, which after you get out of this town hall, head west to market square, then north to temple square and then head east.'
mob trans $n 3137
mob at 3137 mob echoaround $n $n arrives from the clerk's office.
else
 mob trans $n 3137
 mob at 3137 mob echoaround $n $n arrives from the clerk's office.
endif
~
#3025
emote mutters something about lots of work to be done and too few runners to do the job.
mob CANCEL
~
#3026
~
#3054
IF ISPC $n
say Welcome to my shoppe.  If you're looking for full costumes, take a look at the showcase and say which one you're interested in.
ENDIF
~
#3055
IF ISPC $n
say A Fluffy costume, eh?
pmote looks at $n curiously.
~
#3606
~
#3700
if LEVEL $n < 20
say Hello!
say If you need jobs, the town clerk offers many.  It helps you familiarize yourself with Alanthia and earns you good money.  If you're interested, say jobs.
say The town clerk will also help you with area directions.
endif
~
#3701
say You cannot kill me! Type "{Cflee{x"
~
#3702
if LEVEL $n < 20
say The clerk has many jobs for you.  Head to market square first, then head east twice and go south.  That should take you to the town hall, where the clerk works.
endif
~
#4200
~
#6700
if clan $n carnage
  bow $n
else  
  mob kill $n
endif
~
#6701
if name $n Madison
or name $n Anri
or name $n Mithrandir
or name $n SynDragon
or name $n Havok
  mob echo {wA {Wb{wlast of {BL{Wi{CG{BH{Wt{Cn{BI{CN{Bg{w crashes from the skies as {RC{ralis{Rto {wcracks her {Rf{rl{Ra{Ym{Ri{Yn{rg {Dwhip{x
  mob echo {D..{wthen a key spins in mid-air before it crashes to the ground.
  mob oload 6701 30 room
endif
~
#6702
if name $n Madison
or name $n AnrI
or name $n Mithrandir
or name $n SynDragon
or name $n Havok
  mob echoat $n A key {rs{Rco{rrc{Rhe{rs {wup from the pits of {DHell{w..then lays {Ws{wizzling before you.
  mob echoaround $n A key {rs{Rco{rrc{Rhe{rs {wup from the pits of {DHell{w..then lays {Ws{wizzling before $n.
  mob oload 6923 50 room
endif
~
#6703
mob cast nova $n
mob cast blind $n
mob cast blind $n
mob cast blind $n
~
#6750
if clan $n bilanx
  bow $n
  say Greetings Warrior.
else
  say You are on {WB{wi{Wl{wa{Wn{wx grounds.
  say Die Intrauder!
  mob kill $n
endif
~
#6751
mob cast 'cure blindness'
if clan $n bilanx
  mob echoat $n $I's Gladius {Ydoes {RUNSPEAKABLE {Ythings to {gyou! {W[{R1000{W]{w
  mob echoaround $n $I's Gladius {Ydoes {RUNSPEAKABLE {Ythings to {g$n!{w
  mob damage $n 1000 2000
endif
~
#6752
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
mob cast nova $n
~
#6753
if clan $n romana
mob transfer $n 6716
else
 mob transfer $n 3001
endif
~
#8000
giggle
if sex $n == 0
cringe $n
endif
if sex $n == 1
punch $n
endif
if sex $n == 2 
lick $n
endif
~
#8001
if level $n < 31
say Kneel, my pet..
mob echoat $n You kneel before your Mistress.
mob echoaround $n $n kneels before Marlina.
whip $n
else
say Kneel in my presence, slave...
emote looks down at $n and fingers her whip.
eg
endif
~
#8002
mob echoat $n $I says 'For a thousand coins of gold dear, we can have a great time..'
lick
~
#8003
mob echo $I mutters a word a disappears!
mob goto 8033
mob at $n mob echoaround $n A flash of white light envelops $n!
mob at $n mob echoat $n A blinding flash envelops you!
mob transfer $n 8033
mob echoat $n $I firmly kneads your flesh with strong fingers..
mob echoat $n $I takes your body to the brink of ecstasy..and leaves you there.
mob echoat $n $I smirks and disappears, whispering "Fool.."
mob goto 8002
mob echo $I appears in a flash, looking smug.
~
#8600
IF ISPC $n
  MOB ECHO The door opens and you see a resident with a basket of candy.
  IF WEARS $n 3212
    IF WEARS $n 3213
      IF WEARS $n 3214
        IF WEARS $n 3215
          IF WEARS $n 3216
            say Aww, don't you look so cute!  Here's some candy.
            IF CARRIES $n 8605
              MOB OLOAD 8604
              give snickers $n
              MOB FORCE $n put snickers bag
              MOB ECHOAT $n You hear a thump sound as the treat drops into your trick or treat bag.
              MOB ECHO $n The resident steps back inside and closes the door.
              MOB REMEMBER $n
            ELSE
              say Oh, you don't have a trick or treat bag for me to put it in.
              emote shrugs and closes the door.
            ENDIF
          ENDIF
        ENDIF
      ENDIF
    ENDIF
  ELSE
    IF WEARS $n 3217
      IF WEARS $n 3218
        IF WEARS $n 3219
          IF WEARS $n 3220
            say Aww, don't you look so cute!  Here's some candy.
            IF CARRIES $n 8605
              MOB OLOAD 8604
              give snickers $n
              MOB FORCE $n put snickers bag
              MOB ECHOAT $n You hear a thump sound as the treat drops into your trick or treat bag.
              MOB ECHO $n The resident steps back inside and closes the door.
              MOB REMEMBER $n
            ELSE
              say Oh, you don't have a trick or treat bag for me to put it in.
              emote shrugs and closes the door.
            ENDIF
          ENDIF
        ENDIF
      ENDIF
    ELSE
      IF WEARS $n 3195
        IF WEARS $n 3196
          IF WEARS $n 3197
            IF WEARS $n 3198
              IF WEARS $n 3199
                IF WEARS $n 3205
                  say Aww, don't you look so cute!  Here's some candy.
                  IF CARRIES $n 8605
                    MOB OLOAD 8604
                    give snickers $n
                    MOB FORCE $n put snickers bag
                    MOB ECHOAT $n You hear a thump sound as the treat drops into your trick or treat bag.
                    MOB ECHO $n The resident steps back inside and closes the door.
                    MOB REMEMBER $n
                  ELSE
                    say Oh, you don't have a trick or treat bag for me to put it in.
                    emote shrugs and closes the door.
                  ENDIF
                ENDIF
              ENDIF
            ENDIF
          ENDIF
        ENDIF
      ELSE
        IF WEARS $n 3189
          IF WEARS $n 3190
            IF WEARS $n 3191
              IF WEARS $n 3192
                IF WEARS $n 3193
                  IF WEARS $n 3194
                    say Aww, don't you look so cute!  Here's some candy.
                    IF CARRIES $n 8605
                      MOB OLOAD 8604
                      give snickers $n
                      MOB FORCE $n put snickers bag
                      MOB ECHOAT $n You hear a thump sound as the treat drops into your trick or treat bag.
                      MOB ECHO $n The resident steps back inside and closes the door.
                      MOB REMEMBER $n
                    ELSE
                      say Oh, you don't have a trick or treat bag for me to put it in.
                      emote shrugs and closes the door.
                    ENDIF
                  ENDIF
                ENDIF
              ENDIF
            ENDIF
          ENDIF
        ELSE
          IF WEARS $n 3207
            IF WEARS $n 3208
              IF WEARS $n 3209
                IF WEARS $n 3210
                  IF WEARS $n 3211
                    say Aww, don't you look so cute!  Here's some candy.
                    IF CARRIES $n 8605
                      MOB OLOAD 8604
                      give snickers $n
                      MOB FORCE $n put snickers bag
                      MOB ECHOAT $n You hear a thump sound as the treat drops into your trick or treat bag.
                      MOB ECHO $n The resident steps back inside and closes the door.
                      MOB REMEMBER $n
                    ELSE
                      say Oh, you don't have a trick or treat bag for me to put it in.
                      emote shrugs and closes the door.
                    ENDIF
                  ELSE
                    say You're not a trick or treater.  Go away!
                    mob echoat $n The annoyed resident slams the door in your face.
                    mob echoaround $n The annoyed resident slams the door in $n's face.
                  ENDIF
                ELSE
                  say You're not a trick or treater.  Go away!
                  mob echoat $n The annoyed resident slams the door in your face.
                  mob echoaround $n The annoyed resident slams the door in $n's face.
                ENDIF
              ELSE
                say You're not a trick or treater.  Go away!
                mob echoat $n The annoyed resident slams the door in your face.
                mob echoaround $n The annoyed resident slams the door in $n's face.
              ENDIF
            ELSE
              say You're not a trick or treater.  Go away!
              mob echoat $n The annoyed resident slams the door in your face.
              mob echoaround $n The annoyed resident slams the door in $n's face.
            ENDIF
          ELSE
            say You're not a trick or treater.  Go away!
            mob echoat $n The annoyed resident slams the door in your face.
            mob echoaround $n The annoyed resident slams the door in $n's face.
          ENDIF
        ENDIF
      ENDIF
    ENDIF
  ENDIF
ENDIF
~
#9505
peer $n
if level $n < 70
  emote pulls you aside and says in a low voice 'I know why you've come for this.  Don't involve yourself in Desslok's affairs.
  shrug
  say But I suppose it's your risk to take.
  mob oload 9596
  give doorknob $n
  else
  emote pulls you aside and says in a low voice 'I know why you've come for this.  Don't involve yourself in Desslok's affairs.
  say But I suppose you're experienced enough with danger, eh?
  mob oload 9596
  give doorknob $n
endif
~
#9508
if rand 60
  if rand 20
    $I barks loudly at you.
  else
    if rand 40
      $I cowers from you and runs away whimpering.
    else
      if rand 60
        $I jumps up and licks your face.
      else
        if rand 80
          $I runs around in circles, chasing its tail.
        else
          $I sees you and starts to snarl viciously.
      endif
    endif
  endif
endif
endif
~
#9550
mob junk document
emote says 'For the Sultan, I suppose.'  He walks near the gate and yells, 'I have a message for the Sultan!'
mob echo A young boy rushes to the gate.  The palace guard gives him the document, and he rushes back into the palace.
if LEVEL $n >= 20
say They've received the message.  You can go now.
else
 if LEVEL $n < 20
 mob echo The young man rushes towards the gate and hands the palace guard something, then rushes back to the palace.
  if RAND 15
  mob oload 9610
  drop sack
  get sack
  give 800 gold $n
  say Get going now, nothing more for you here.
  else
   if RAND 1
   mob oload 9611
   give response $n
   say Take this back to Midgaard.
   endif
  endif
 endif
endif
~
#9551
~
#9600
mob delay 1
~
#9601
eg
unlock down
open down
say Please, enter!
~
#9602
eg
unlock down
open down
say Please enter!
~
#11000
emote stares at you, his attention seeming far, far away....
if LEVEL $n >= 20
emote tosses it to one of his bodyguards, who gets a puzzled look on his face.
mob junk package
else
 if LEVEL $n < 20
  if RAND 15
  say Ah, the Midgaard messenger.
  mob oload 11042
  emote nudges the bodyguard.  He disappears, and returns with a few gems and gives it to King Tyerian.
  drop gem
  get gem
  give 650 gold $n
  say Escort $n to the way out.
  mob junk package
  mob echo A bodyguard motions you to follow.
  mob trans $n 11044
  mob at 11044 mob echoaround $n $n is escorted out of King Tyerian's house.
  else
   if RAND 1
   emote examines the package, and suddenly becomes very agitated.
   say Curses to that damned lazy town clerk!  I was supposed to get this package weeks ago!
   say Tell that sloth if he's ever late with a package again I'll have him removed from his job and have him work as a janitor!
   piss
   mob junk package
   mob echoaround $n The bodyguards grab $n by the arms and escort $n to the Royal Square.
   mob echoat $n The bodyguards grab you roughly by the arms and escort you to the Royal Square.
   mob trans $n 11044
   mob at 11044 mob echoaround $n $n is roughly escorted out of King Tyerian's house.
   endif
  endif
 endif
endif
~
#12800
~
#14000
emote puts down his rag and reads the paper.
if LEVEL $n >= 20
smile $n
mob junk paper
else
 if LEVEL $n < 20
  if RAND 15
  smile $n
  mob oload 14024
  drop pouch
  get pouch
  give 500 gold $n
  mob junk paper
  else
   if RAND 1
   frown
   mob junk paper
   mob oload 14025
   say I'm sorry, but we can't accommodate them this evening.
   emote flips the piece of paper and writes a few words on it.
   give paper $n
   endif
  endif
 endif
endif
~
#14004
if ispc $n
 emote studies you, then says 'hmm, got a minute?'
endif
~
#14005
say I've no time to squabble with details.  I need for you to find me an artifact that would allow me to create a door to the shadow realm.  Some mage in a city has it, but I don't know which city or who the mage is.
say Bring it to me and I'll make it worth your while.
~
#14006
if carries $n doorknob
  say I can sense the artifact!  Give it to me now!
endif
~
#14007
powertrip
mobecho Desslok holds the doorknob and mutters a few archaic words.
mobecho {DA large, shimmering suddenly appears!{x
mob oload 14023
say I promised you a handsome reward.  Find a weapon of true holiness to slay those who hold your riches.
bow $n
emote enters {D a shimmering portal.{x
mob purge desslok
~
#14100
if isnpc $n
  mob junk all
else
  if wears $i 14150
    if rand 20
      mob echoat $n {wThe {DM{wa{Dr{wb{Dl{we {wS{Wk{we{Wl{we{Wt{wo{Wn {wstabs its {RD{De{Rm{Do{Rn {DK{Rn{Di{Rf{De {wdeep into your chest!
      mob echoaround $n {wThe {DM{wa{Dr{wb{Dl{we {wS{Wk{we{Wl{we{Wt{wo{Wn {wstabs its {RD{De{Rm{Do{Rn {DK{Rn{Di{Rf{De {winto $n's chest!
      mob damage $n 100 500
    endif
  endif
endif
~
#14101
if name $n Madison
or name $n Alithra
or name $n Anavrin
  if ispc $n
    mob gtransfer $n 14146
    mob at $n mob echo You step through a wrinkle in space and time.
    mob echo $n steps through a wrinkle in space and time.
    break
  endif
endif
mob echoat $n {wThe {DM{wa{Dr{wb{Dl{we {wS{Wk{we{Wl{we{Wt{wo{Wn {wstabs its {RD{De{Rm{Do{Rn {DK{Rn{Di{Rf{De {wdeep into your chest!
mob echoaround $n {wThe {DM{wa{Dr{wb{Dl{we {wS{Wk{we{Wl{we{Wt{wo{Wn {wstabs its {RD{De{Rm{Do{Rn {DK{Rn{Di{Rf{De {winto $n's chest!
mob cast 'cure blindness'
mob damage $n 1000 2000
mob kill $n
~
#14110
mob junk all
if isnpc $n
  break
else
  if carries $n 14137
    break
  else
    if carries $n 14129
      mob oload 14137
    else
      if carries $n 14128
        mob oload 14129
      else
        mob oload 14128
      endif
    endif
  endif
  mob echo {x
  mob echo {GGurgling its last gasping breath, the creature starts to speak..{X
  mob echo {x
  mob echo {GThe creature hisses '{WFOOL!,,you may of killed me,,{G'{x
  mob echo {GThe creature hisses '{WBut the meek shall inherit the earth.{G'{x
  mob echo {GThe creature hisses '{WOnly the {DBlack Hole {Wcan save you now..{G'{x
  mob echo {x
endif
~
#14111
if name $n Tarc
  say Welcome home, My Lord.
  bow tarc
else
  if name $n Anri
    say Welcome home, My Lady.
    bow anri
  else
    say You are on Asmadi ground, prove yourself, or be removed
    say from the grounds!
  endif
endif
~
#14112
if name $n Tarc
or name $n Anri
or name $n Haldorn
or name $n Gikk
or name $n Claebrylla
  mob echoaround $n $n whispers 'Open the gates servant'
  if sex $n == 2
    say Yes Mistress
  else
    say Yes Master
  endif
  mob echoaround $n $I opens the gates for $n.
  mob echoat $n $I opens the gates for you.
  mob echoaround $n $I leaves east.
  mob echoaround $n $I closes the gates behind $n.
  mob echoat $n $I closes the gates behind you.
  mob gtransfer $n 14166
endif
~
#14113
mob junk all
mob oload 14165
if carries $n 14116
  break  
else
  if carries $n 14143
    mob oload 14116
  else
    if carries $n 14114
      mob oload 14143
    else
      mob oload 14114
    endif
  endif
endif
~
#14114
~
#14115
if name $n Vadakyn
or name $n Veshaun
or name $n Laurali
or name $n Sune
or name $n Darconix
or name $n Godac
or name $n Artanis
or name $n Renwold
or name $n Havok
or name $n Whisperwolf
   mob gtrans $n 14131
   mob echoaround $n $I ushers $n through the door into the hall{/and closes it tightly behind $m.
else
   mob echoaround $n $I chants a few words and $n disappears in a geyser of cold flame.
   mob echoat $n You are filled with dread as $I chants a few words and you suddenly find{/ yourself facing a quick and grisly death.
   mob trans $n 14172
endif
~
#14116
~
#14120
if name $n Alderon
  mob echo Alderon says 'give me a key you lazy trick'
  say I know you think I'm a sexy chick!
  pat $n
  mob oload 14158
  give key $n
  say Now stop losing your keys!
endif
~
#14130
if name $n Pulsar
or name $n Kiscu
or name $n Blackout
  mob gtransfer $n 14153
endif
~
#14177
if name $n Vadakyn
or name $n Veshaun
or name $n Artanis
or name $n Nalfeshnee
or name $n Laurali
or name $n Sune
or name $n Haldorn
or name $n Godac
or name $n Darconix
or name $n Havok
  mob echoaround $n {/$n commands 'Open the gates, loyal guard.'{/
  mob echoat $n {/'Open the gates, loyal guard,' you command.{/
  if sex $n == 2
    say Yes, my Lady. Welcome home.'{/{
  else
    say Yes, Lord. Welcome home.'{/{
  endif
  mob echoaround $n $I opens the gates for $n.
  mob echoat $n $I opens the gates for you.
  mob echoaround $n $I ushers $n north through the gates, and seals them tightly again.
  mob echoat $n $I ushers you north through the gates, closing them tightly behind you.{/{/
  mob gtransfer $n 14111
endif
~
#14178
if name $n Vadakyn
or name $n Veshaun
or name $n Artanis
or name $n Nalfeshnee
or name $n Laurali
or name $n Sune
or name $n Haldorn
if players == 1
mob load obj 14156
mob give shard $n
bow $n
endif
~
#14180
if name $n Krater
  mob echoaround $n says 'Hail Goddess of Beauty and Perfection.'
  mob echoaround $n says 'I beseech thy intervention and beg entrance into thy chamber'
  mob echoat $n You say 'Hail Goddess of Beauty and Perfection.'
  mob echoat $n You say 'I beseech thy intervention and beg entrance into thy chamber'
  mob echo {x
  mob echo $I utters some pretty magical words..
  mob echo {cOut of an {wo{Wr{wb {cof {Yl{yight {ca key falls down into $I{c's hand.{x
  mob oload 14180
  give divine $n
  mob echo {x
endif
~
#14181
if OBJHERE couch
   sleep couch
endif
~
#14183
~
#14189
emote turns to face you.{X{/
mob echoat $n {cLicourtrix {rp{Droclaim{rs {W'{cWelcome to the Altar of Dreams.  The altar awaits your sacrifice.{W'{X
~
#14190
if name $n Thassodar
or name $n Haldorn
or name $n Quix
or name $n Infinel
or name $n Khrima
or name $n Uraj
   mob echoat $n You step into the {DM{gi{Dra{gg{De.{x{/{/Welcome home, $n.{/
   mob gtrans $n 14191
endif
~
#14193
if name $n Darconix
or name $n Gaborn
   say Welcome home, Lord.
   kiss $n
else
if name $n Haldorn
or name $n Quix
   say Greetings, friend.
   curtsey $n
else
if money $n < 800000
   say No beggars here, $n! Come back when you're less destitute.
   mob gtrans $n 3001
else
if money $n >= 800000
   mob echo *Click*
   mob echoat $n $I grins at you evilly
   mob echoaround $n $I grins evilly at $n.
   say If you wish to leave here, you must answer my question correctly. Otherwise, you must give me a fee of 800000 gold coins to cover the cost of transporting you out.{/{/{
   say What is greater than God, more evil than the Devil? The rich need it but the poor have it.
endif
endif
endif
endif
~
#14194
if name $n Darconix
   mob trans $n 14197
else
   mob echoaround $n $n whispers the answer to $I.
   mob echoat $n You whisper the answer to $I.
   say Correct!
   mob echo *Click*
   mob echo $I unlocks the door.
   say You may leave in peace.
   mob gtrans $n 14194
endif
~
#14195
if ispc $n
   say Thank you for your contribution.
   say Good day!
   mob gtrans $n 2
   mob goto 14197
   drop 800000 gold
   mob goto 14195
endif
~
#14196
if name $n Darconix
or name $n Haldorn
or name $n Quix
   mob gtrans $n 1
endif
~
#14197
if ispc $n 
   mob damage everlast -900000 -900000
endif
~
#15801
~
#18200
mob echoat $n {x
mob echoat $n {rThe room begins to flash with crimson lightning as your form is transformed{x
mob echoat $n {r{rinto magical energy and transported to the begining of your journey.{x
mob echoat $n {x
mob transfer $n 18201
mob at $n mob echoat $n {x
mob at $n mob echoaround $n {rWith a clash of demonic thunder and lightning the form of $n begins to{x
mob at $n mob echoaround $n {r{rtake shape.{x
mob echo $n starts to mutter some ancient words.
mob echo {x
mob echo {rThe room begins to flash with crimson lightning as $n's form is{x
mob echo {r{rtransformed into magical energy and fades from view.{x
~
#18201
mob echoat $n {x
mob echoat $n {W$I . o O (Ah yes, $n, what can I do for you?){X
mob echoaround $n $I peers at $n intently, reading $s thoughts as well as $s inventory.
mob echoat $n $I peers at you intently, reading your thoughts as well as your inventory.
if carries $n 18205
  if carries $n 18206
    if carries $n 18207
      if carries $n 18208
        if carries $n 18209
          mob echo {x
          emote screams {WYOU INFIDEL!!!{X
          mob echoat $n $I raises $l hands to the sky and begings to call forth desctruction upon you.
          mob echoaround $n $I raises $l hands to the sky and begins to call forth destruction upon $n.
          mob echoat $n {x
          mob transfer $n 18334
          mob at 18334 mob echo {x
          mob at 18334 say Give me the five star points or face my wrath!
          break
        endif
      endif
    endif
  endif
endif
mob echo {x
mob echo $I turns $l back then mutters '{WYou are such a fool, $n.'{X
~
#18202
mob echo {x
if carries $i 18205
  if carries $i 18206
    if carries $i 18207
      if carries $i 18208
        if carries $i 18209
          mob echo $I speaks the magic that forces the pieces back together.
          mob oload 18300 1 R
          mob junk all
          mob at 18333 mob force god mob oload 18210
          mob at 18333 mob force god wear sack
          mob at 18333 mob force god mob oload 18204
          mob at 18333 mob force god put treasure sack
          say {x
          say Now there's one last thing that must be taken care of..
          eg $n
          mob echo {x
          mob kill $n
          break
        endif
      endif
    endif
  endif
endif
say Very well $n, now give me the next point.
~
#18203
~
#20300
smile $n
mob gtransfer $n 20301
mob gtransfer $i 20301
mob gtransfer $n 20302
mob gtransfer $i 20302
mob gtransfer $i 20300
~
#20700
if SEX $n == 1
say You are male.
else
if sex $n == 2
say You aren't a male.
endif
endif
~
#20721
if CLASS $n thief
  say I am good at thievery.  If you ever want any help with it, just ask me.
else
if CLASS $n warrior
  Say I'm a pretty skilled fighter so if you ever need help, just ask me.
else
if CLASS $n mage
  Say I'm not well versed in the ways of magic but I'm sure I can be of some help.
else
if CLASS $n cleric
  Say I do not have any divine powers but I will help in any way I can.
else
if CLASS $n paladin
  Say I'm a pretty skilled fighter so if you ever need help, just ask me.
else
if CLASS $n assassin
  Say Being a ninja has its high points.  I'm sure I can teach you a thing or two.
else
if CLASS $n dragoon
  Say I'm a pretty skilled fighter so if you ever need help, just ask me.  Don't know why you had decided to be a dragoon.  At least it's better than being a bard.
else
  say If you're ever interested, I'll help train you.  I'm skilled in a varity of skills.
endif
~
#21000
mob echo {/{W$I shivers violently as a shock ripples through its form.{/{/Eyes roll everywhere as $I loses cohesion and falls apart.{x{/
mob oload 21006
mob at 21057 drop eye
mob echo
mob goto 1
~
#21001
if wears $n 21007
or carries $n 21007
   mob echoat $n {WThe granite golem inspects your pass and waves you by.{x{/
   mob echoaround $n {WThe granite golem inspects $n's pass and waves $m by.{x{/
   mob echoaround $n $n leaves up.
   mob gtrans $n 21052
else
   mob echoat $n {WThe granite golem thumps the head of its club into the floor, blocking your way.{x
   mob echoaround $n {WThe granite golem thumps the head of its club into the floor, blocking $n's way.{x
   mob echo 
   emote rumbles 'Present your pass, please.'
endif
~
#21002
if wears $n 21008
   echoaround $n $n steps through the bare stone wall with a sound like pushing a boot into thick mud.
   echoat $n You step through the stone wall with a sound like pushing a boot into thick mud.
   mob gtrans $n 21055
   echoaround $n $n has arrived.
else
mob echoat $n The stone wall seems quite solid.
mob echoaround $n $n attempts to walk through the wall to the north, to no avail.
endif
~
#21003
emote bellows 'Greetings, $n. Welcome to the Vortex. Have a good ride!'
cackle
~
#21004
if ispc $n
or isnpc $n
mob echoat $n Gusts of wind pick up and hurl you further up the vortex.
mob echoaround $n Gusts of wind pick up and hurl $n further up the vortex.
mob gtrans $n 21002
endif
~
#21005
mob echoat $n A sharp gust of wind attempts to force you out of the whirlwind!
mob echoaround $n A sharp gust of wind attempts to force $n out of the whirlwind!
mob kill $n
~
#21006
if wears $n 21002
  if wears $n 21003
    if wears $n 21004
     mob oload 21007
     mob at $n give pass $n
     say Don't lose that one.
    endif
  endif
endif
~
#21007
if wears $n 21004
   mob call 21008 $n
else
   say Come back when you find your helmet, Guard.
endif
~
#21008
if wears $n 21003
   mob call 21009
else
   say I can't issue a pass to a guard without his shield!
endif
~
#21009
if wears $n 21002
   mob oload 21007
   mob give pass $n
   mob say Don't lose that.
endif
~
#21010
mob oload 21007
mob at $n give pass $n
say Don't lose that one, Soldier, or there will be consequences.
~
#21500
if clan $n Gaarde
mob transfer $n 21501
else
 say You do not belong here.  Begone!
endif
~
#21501
if clan $n gaarde
  say Off you go now.  Be safe.
  mob gtrans $n 3001
else
  say I'm sorry, I cannot help you with that.
endif
~
#21900
if clan $n ca'zultai
 mob transfer $n 21901
 else
 say You are not admitted in here
endif
~
#21903
IF clan $n ca'zultai
mob trans $n 3001
endif
~
#22500
If isevil $n
say Hello my friend how may I help you?
else
if isgood $n
shout Attack!!!! Attack!!!!
mob kill
else
if isneutral $n
say Wimp!!!!
mob kill
endif
endif
endif
~
#23000
~
#23201
if carries $n 23250
    say get ready!
    mob oload 23242
    mob oload 23243
    mob oload 23244
    mob oload 23245
    mob oload 23246
    give qp $n
    give qp $n
    give qp $n
    give qp $n
    give qp $n
    drop all
    sac all
    typo $n used Qpoints 
  else 
   say You do not belong here!
   mob transfer $n 3001
   typo $n tried to use QPOINTS
endif
~
#23202
if carries $n 23250
    say get ready!
    mob oload 23216
    mob oload 23216
    mob oload 23216
    mob oload 23216
    mob oload 23216
    give warp $n
    give warp $n
    give warp $n
    give warp $n
    give warp $n
    drop all
    sac all
  else 
   say You do not belong here!
   mob transfer $n 3001
   typo $n tried to use WARP
endif
~
#23203
if carries $n 23250
   say Welcome $n, what shall it be today?
  else
   say You do not belong here!
   mob transfer $n 3001
   typo $n was trespassing.
endif
~
#23204
if carries $n 23250
   mob oload 23200
   mob oload 23201
   mob oload 23202
   mob oload 23203
   mob oload 23204
   give cluepon $n
   give bless $n
   give bless $n
   give bless $n
   give moon $n
   type $n used blessing
  else
   say You do not belong here!
   mob transfer $n 3001
   typo $n Tried to use BLESSING
endif
~
#23205
if carries $n 23250
   mob oload 23206
   give card $n
  else
   say You do not belong here!
   mob transfer $n 3001
   typo $n tried to use enchant.
endif
~
#23206
if carries $n 23250
   mob oload 23209
   give card $n
  else
   say You do not belong here!
   mob transfer $n 3001
   typo $n tried to use restring.
endif
~
#23207
if carries $n 23250
   mob oload 23248
   give voucher $n
  else
   say You do not belong here!
   mob transfer $n 3001
   typo $n tried to use voucher
endif
~
#23208
if carries $n 23250
   say i offer these items
   say qpoints
   say warp pills
   say blessings and coupons
   say enchant cards
   say restring cards
   say voucher for gold
  else
   say You do not belong here!
   mob transfer $n 3001
endif
~
#25101
c
if clan $n Gaarde
  tell $n Off you go now, Be safe
  mob gtrans $n 3001
else
  say I'm sorry, but I cannot help you with that.
endif
~
#25612
emote looks serenly upon you as you come near.
say Salutations, {M$n.{X
emote flits to and fro, looking upon you with wisdom in her silver eyes.
emote whispers "How may I help you my child?"
 
~
#25614
say Greetings $n
say What sort of pixie magic can I find for you in my bag of tricks?
~
#25615
say Welcome, {Y$n{X.
say I would wager you want to leave my wondrous tree?
~
#25616
say Simply answer this riddle for me.
say "I never was, am always to be,
say No one ever saw me, nor ever will
say And yet I am the confidence of all
say To live and breathe on this terrestrial ball."
say What am I?
~
#25617
mob echoaround $n $n says 'tomorrow'
mob echoaround $n Very good, be gone with you then!
emote mutters a few words of {Mpixie {Ymagic{X
cackle
emote snatches a bolt of lightning from the heavens!
grin $n
say Fare thee well, foolish one.
mob echoaround $n The Pixie King throws the bolt at $n, knocking $s from the branch.
mob echoat $n The Pixie King throws the bolt at you knocking you from the branch.
mob echoaround $n $n lets out an ear piercing scream as $e plummets to $s doom.
mob damage $n 500 1000
mob trans $n 25603
~
#25618
mob echo {GWretching up his last breath, the wolf gags then vomits{/up a bone covered with rotting flesh and slime{X.
mob oload 25620
mob at 25683 drop bone
~
#25800
if level $n < 100
  say You are too young for my tastes.  BEGONE!!
  mob gtrans $n 3001
else
  say Ahhh, fresh meat.  You will make a fine meal.  Come let us feast.
  mob kill $n
endif
~
#25801
yawn
sleep bed
~
#25802
if HOUR >= 22
  say Goodness... its time to close.   Goodnight.
  emote tips his hat to you.
  up
  kiss Jynger
  say Time for bed love.
  north
  open north
  north
  yawn
  sleep bed
mob delay 1
else
  if HOUR < 21
    say Greetings and welcome to The Cloudy Ale.
    say Find my wife Jynger if you need a room.
  endif
endif
~
#25803
if ISDELAY $i
  mob echoat $n The Traveler says 'It is time to retire.'
else
  yawn
mob delay 1
endif
~
#25804
if HOUR < 7
  snore
else
  if HOUR >= 7
    wake
    yawn
    open south
    south
    south
    down
    say Well.. back to work for me.
  endif
endif
~
#25805
if ispc $n
  mob echoat $n Jynger says 'Hello.  Can I interest you in a room?'
  mob echoat $n Jynger smooths out the wrinkles in her clothing.
else
  grumble
endif
~
#25806
~
#26500
if ISDELAY $i
say Get moving soldier!
else
 say What the hell are you doing over here?  I told you to reinforce the other side!  MOVE IT SOLDIER!
mob delay 1
endif
~
#26501
say die mother fucker
mob kill $n
~
#26502
if HOUR <= 8
say Ah, I think it's before 8 o'clock.  Time to take care of my morning glory.
else
 if HOUR > 8
 say Ah, I think it's after 8 o'clock.  Time to pimp some bitches.
 south
 north
 east
 west
 say Damnit, no bitches.  I guess it's another whack off session.
 sigh
 endif
endif
~
#26503
IF ISDELAY $i
 IF HAS $n 26507
  say Hello there, captain.  Beautiful view from up here, eh?
  ENDIF
 ELSE
 IF HAS $n 26507
 MOB ECHOAT $q The lookout nods to the first officer, and salutes you as you   enter. He then heads to the crow's nest.
 MOB ECHOAROUND $q The lookout nods to the first officer, and salutes $q as $q   enters.  He then heads to the crow's nest.
 MOB GOTO 26516
 MOB DELAY 80
 ENDIF
ENDIF
~
#26504
IF ISPC $n
 IF ISDELAY $i
 say we're reloading as fast as we can, captain!
 ELSE
  say reloading, captain
  mob emote shouts orders to his men as they hurriedly reload the ballistae.
  mob DELAY 5
 ENDIF
ENDIF
~
#26505
say ballistae is loaded and ready to fire on your mark
~
#26506
IF ISPC $n
 IF ISDELAY $i
 say hold on captain, we're not done reloading!
 ELSE
  mob echo Swoosh!  The ballistae fires its large spear and recoils violently.
  IF RAND 50
  MOB at 26507 ECHO A ballistae tears through the hull, damaging you!
  mob echo The ballistae tears through the enemy ship!
  MOB AT 26507 damage all lethal
   ELSE
   mob echo The ballistae hits the ship, but no one is damaged.
  ENDIF
 ENDIF
ENDIF
~
#26507
~
#26508
~
#26510
IF ISPC $n
say Hello there.  You looking for a ship to buy?  You've come to the right place!
say I got a fine ship ready to go with a full compliment of weapons and crew.
say I'll sell her to you for 300,000 gold.
ENDIF
~
#26511
IF ISPC $n
beam
say Your ship is waiting out on the pier.  Your first officer should be say sitting outside on the docks.  Take good care of her!
MOB OLOAD 501
give deed $n
emote wishes you farewell and makes a mad dash to the bank!
MOB goto 1
MOB AT 482 MOB MLOAD 435
ENDIF
~
#26512
IF ISPC $n
emote takes a good look at the deed, and nods to himself.
emote says 'So, you're my new captain, eh,' he says, studying you.  He shrugs.
say I've had worse.  So, you ready to set sail?
MOB JUNK deed
MOB REMEMBER $n
ENDIF
~
#26513
IF ISPC $q
rub
say Where to?  Midgaard or New Thalos?
MOB MLOAD 436
MOB GOTO 1
ENDIF
~
#26514
IF ISPC $q
nod $q
say Just tell me when you're ready.
ENDIF
~
#26515
IF ISPC $q
 IF ISDELAY $i
 say Take it easy, we're going as fast as we can.
 ELSE
  say To Midgaard we go!  We'll be ready within 2 hours.  In the meantime
  say why don't you have a walk around in your ship and meet the rest of your crew?
  say I'll come get you when we're done.
  say Oh, you should wear this, or at least carry it around.  People will know who you are.
  MOB OLOAD 26507
  give insignia $q
  MOB ECHO The first officer steps into the ship and starts issuing orders.  Pretty soon the ship becomes a flurry of activity.
  MOB REMEMBER $n
  MOB GOTO 26509
  MOB DELAY 22
  ENDIF
ENDIF
~
#26516
IF ISPC $q
 IF ISDELAY $i
 say Take it easy, we're going as fast as we can.
 ELSE
  say To Thalos we go!  We'll be ready within 2 hours.  In the meantime
  say why don't you have a walk around in your ship and meet the rest of your crew?
  say I'll come get you when we're done.
  say Oh, you should wear this, or at least carry it around.  People will know who you are.
  MOB OLOAD 26507
  give insignia $q
  mob echo The first officer steps into the ship and starts issuing orders.  Pretty soon the ship becomes a flurry of activity.
  MOB GOTO 26509
  MOB DELAY 22
  MOB REMEMBER $n
 ENDIF
ENDIF
~
#26517
MOB AT $q MOB ECHOAT $q The first officer appears and escorts you to your ship.
MOB AT $q MOB ECHOAROUND $q $q's first officer appears, informing $q that the ship is ready and escorts $q to the ship.
MOB TRANS $q 26509 (middle of the deck of ship)
MOB ECHO You feel the ship jolt as it moves away from the dock and into the Dragon Sea.
MOB AT 482 ECHO You see one of the ships in the pier move out into the Dragon Sea.
MOB ECHOAT $q The first officer nudges you on the shoulder.
say We're all set and currently moving to our destination, captain.  I'll brief you on what to expect at the helm when you're ready.
MOB AT 26506 MOB MLOAD 26510
MOB GOTO 26506
MOB MLOAD 26512
MOB GOTO 1
~
#26518
IF HAS $n 26507
say Hello captain!  I'll brief you on what to expect on this voyage.
say We are sailing in the Dragon Sea, which have a lot of notorious pirates.
say Hopefully we won't encounter any, but our voyage will be long, so we probably will.
say If there is a ship in range, our lookout will alert us, and you'll tell us what to do.
say Say your order and I'll carry it out.
MOB DELAY 3
ENDIF
~
#26519
trigger - delay 100
say This ship is versatile and can handle most of the pirate ships out there.
say Are you new to this, or a bit rusty?
~
#26520
IF HAS $n 26507
say You can check in your captain's quarters for a small book that will
say get you up-to-speed on what a captain does.
MOB AT 26510 MOB OLOAD 26508
say I'll be around, making sure everything is ok.
MOB ECHOAT $q The first officer salutes you and heads on his way.
MOB ECHOAROUND $q The first officer salutes $q and heads on his way.
MOB GOTO 26514
MOB MLOAD 26516
MOB GOTO 1
~
#26521
IF HAS $n 26507
say Ah, good!  I'm tired of having inexperienced captains leading us to
say our graves.  I'll be around, making sure everything is ok.
MOB ECHOAT $q The first officer salutes you and heads on his way.
MOB ECHOAROUND $q The first officer salutes $q and heads on his way.
MOB GOTO 26514
MOB MLOAD 26516
MOB GOTO 1
ENDIF
~
#26522
IF ISNPC $q
say hey you're a mob!
say woowoo!
MOB ZECHO WOOWOO MOTHER FUCKERS!!!
MOB DELAY 3
ELSE
 say go away fatass
ENDIF
~
#26523
MOB ZECHO THE DELAY WORKED!  HOLY JESUS FUCK!!!
~
#26524
say If you choose to {Rmaintain course{x, we will just ignore the ship.  If we get in weapons
say range we will alert you and you will tell us to engage or evade.
say All in all, we love a good fight.
eg $q
MOB DELAY 5
~
#26525
MOB MLOAD 26516 (officer 7)
MOB AT 26516 MOB MLOAD 26510
MOB CALL 26503 $q
MOB GOTO 1
MOB PURGE
MOB PURGE self
~
#26526
~
#26527
~
#26528
~
#26529
~
#26530
~
#26531
~
#26532
~
#26533
~
#26534
~
#26535
~
#26536
~
#26537
~
#26538
~
#26539
~
#26540
~
#26541
say w00t!
mob pause 10000
say woo
~
#0

#$
