/***************************************************************************

The CPS1 system is made of 3 boards: A, B and C. The first two games also exist
a 2-board setups, where the C-board is integrated into the B-board.

There are several revisions of the A-board, but they are functionally equivalent
and interchangeable except for 89626A-4 DASH used by sf2 which has a 12MHz xtal
replacing the 10Hz one. Note that it's likely that from one point onwards Capcom
simply switched to make only 89626A-4 DASH board, that is all games after a
certain point should use the 12MHz xtal not just sf2.
Verified: Quiz and Dragons (US) comes with the DASH version (original pcb with sticker).

Known A-board revisions:
88617A-4
88617A-5
88617A-7
88617A-7b
89626A-4
89626A-4 DASH

NAME                                        Year  B-board #      B-board PALs      C-board #         CPS-B #           C-board PALs
------------------------------------------- ----  --------  ---------------------  ---------  -----------------------  -------------
Forgotten Worlds / Lost Worlds              1988  88618B-2  LWCHR            LWIO  None       CPS-B-01  DL-0411-10001  N/A
  (alt B-board revision)                          88621B-2  LW621            LWIO  None       CPS-B-01  DL-0411-10001  N/A
Ghouls 'n Ghosts (World / US)               1988  88620-B-2 DM620            LWIO  None       CPS-B-01  DL-0411-10001  N/A
  (alt B-board revision - Japan)                  88622B-2  DM22A            LWIO  88622-C-1  CPS-B-01  DL-0411-10001  None
  (alt B-board revision - Japan)                  91634B-2  DAM63B    BPRG1  IOB1  92631C-6   CPS-B-21  DL-0921-10014  C632  IOC1
Strider                                     1989  89624B-2  ST24M1           LWIO  88622-C-1  CPS-B-01  DL-0411-10001  None
  (alt B-board revision)                          89624B-2  ST24M2           LWIO  88622-C-2  CPS-B-01  DL-0411-10001  None
  (alt B-board revision)                          89624B-3  ST24B2           LWIO  88622-C-2  CPS-B-01  DL-0411-10001  None
  (alt B-board revision - Japan)                  ?         ST22B            ?
Dynasty Wars                                1989  89624B-3  TK24B?           ?     88622-C-2  CPS-B-02  DL-0411-10002  None
  (alt B-board revision - Japan)                  88622B-3  TK22B            LWIO
Willow                                      1989  89624B-3  WL24B            LWIO  88622-C-4  CPS-B-03  DL-0411-10003  None
  (alt B-board revision - Japan)                  88622B-3  WL22B            LWIO  88622-C-2  CPS-B-03  DL-0411-10003  None
UN Squadron / Area 88                       1989  89624B-3  AR24B            LWIO  88622-C-4  CPS-B-11  DL-0411-10004  None
  (alt B-board revision - Japan)                  88622B-2  AR22B            LWIO
  (alt B-board revision - Japan)                  88622B-3  AR22B            LWIO
Final Fight (first rev)                     1989  89624B-3  S224B            IOB1  88622-C-5  CPS-B-04  DL-0411-10005  None
Final Fight (US/Japan 900112)                                                      ?          CPS-B-04@ DL-0411-10001  None
Final Fight (US 900613)                           89624B-3                   IOB1  88622-C-5  CPS-B-05  DL-0411-10006  None
Final Fight (Japan 900305) (alt B-board revision) 88622B-2  S222B            LWIO  88622-C-5  CPS-B-02  DL-0411-10002  None
1941                                        1990  ?         YI24B            ?     88622-C-5  CPS-B-05  DL-0411-10006  ?
  (alt B-board revision - Japan)                  ?         YI22B?           ?
Mercs                                       1990  89624B-3  O224B            IOB1  90628-C-1  CPS-B-12  DL-0411-10007  C628
  (alt B-board revision - Japan)                  ?         O222B?           ?
  (alt C-board revision)                                                           90628-C-2
Magic Sword                                 1990  89624B-3  MS24B            IOB1  88622-C-5  CPS-B-13  DL-0411-10008  None
  (alt B-board revision - Japan)                  89625B-1  MS22B            IOB1
Chiki Chiki Boys / Mega Twins               1990  89624B-3  CK24B            IOB1  88622-C-5  CPS-B-14  DL-0411-10009  None
  (alt B-board revision - Japan)                  ?         CK22B?           ?
Nemo                                        1990  89624B-3  NM24B            IOB1  88622-C-5  CPS-B-15  DL-0411-10010  None
  (alt B-board revision - Japan)                  89625B-1  NM22B            ?
Carrier Air Wing / U.S. Navy                1990  89624B-3  CA24B            IOB1  88622-C-5  CPS-B-16  DL-0411-10011  None
  (alt B-board revision - Japan)                  89625B-1  CA22B            LWIO
Street Fighter II (910214)                  1991  90629B-2  STF29            IOB1  90632C-1   CPS-B-17  DL-0411-10012  C632
Street Fighter II (US 910206)                                                                 CPS-B-17  DL-0411-10012
Street Fighter II (US 910228)                                                ??               CPS-B-18  DL-0411-10013  C632B
Street Fighter II (Japan 910306)                                                              CPS-B-12  DL-0411-10007
Street Fighter II (US 910318)                                                                 CPS-B-05  DL-0411-10006
Street Fighter II (US 910411)                                                                 CPS-B-15  DL-0411-10010
Street Fighter II (World 910522)                                                              CPS-B-11  DL-0411-10004
Street Fighter II (US 910522)                                                                 CPS-B-14  DL-0411-10009
Street Fighter II (US 911101)                                                                 CPS-B-17  DL-0411-10012
Street Fighter II (Japan 911210)                                                              CPS-B-13  DL-0411-10008
Three Wonders*                              1991  89624B-3  RT24B            IOB1  90630C-4   CPS-B-21  DL-0921-10014        IOC1
  (alt B-board revision - Japan)                  89625B-1  RT22B            IOB1
  (alt B-board revision)                          91634B-2  RT63B?    BPRG1? IOB1
King of Dragons*                            1991  90629B-3  KD29B            IOB1  90631C-5   CPS-B-21  DL-0921-10014  C632  IOC1
Captain Commando*                           1991  91635B-2  CC63B     CCPRG  IOB1  90631C-5   CPS-B-21  DL-0921-10014  C632  IOC1
Knights of the Round*                       1991  91635B-2  KR63B     BPRG1  IOB1  90631C-5   CPS-B-21  DL-0921-10014  C632  IOC1
  (alt B-board revision)                          ?         KR29B?           ?
Street Fighter II' Champion Edition         1992  91635B-2  S9263B    BPRG1  IOB1  92631C-6   CPS-B-21  DL-0921-10014  C632  IOC1
  (alt B-board revision - Japan)                  91634B-2  S9263B    BPRG1  IOB1
Varth (World)                               1992  89624B-3  VA24B            IOB1  88622-C-5  CPS-B-04  DL-0411-10005  None
  (alt B-board revision - US)                     ?         VA63B?           ?
Varth (Japan)*                                    88622B-3  VA22B            LWIO  92641C-1   CPS-B-21  DL-0921-10014        IOC1
Capcom World 2*                             1992  ?         Q522B            IOB1  ?          CPS-B-21  DL-0921-10014        IOC1
Quiz and Dragons (US)*                      1992  89625B-1  QD22B            IOB1  92641C-1   CPS-B-21  DL-0921-10014        IOC1
Quiz and Dragons (Japan)                    1994  ?         ?                IOB1  ?          CPS-B-21  DL-0921-10014        IOC1
Warriors of Fate*                           1992  91634B-2  TK263B    BPRG1  IOB1  92641C-1   CPS-B-21  DL-0921-10014        IOC1
Street Fighter II Turbo Hyper Fighting      1992  91635B-2  S9263B    BPRG1  IOB1  92631C-6   CPS-B-21  DL-0921-10014  C632  IOC1
Cadillacs and Dinosaurs*                    1993  ?         CD63B     BPRG1  IOB1  92641C-1   CPS-B-21  DL-0921-10014        IOC1
Punisher*                                   1993  91635B-2  PS63B     BPRG1  IOB1  92641C-1   CPS-B-21  DL-0921-10014        IOC1
Saturday Night Slam Masters*                1993  ?         MB63B     BPRG1  IOB1  92641C-1   CPS-B-21  DL-0921-10014        IOC1
Muscle Bomber Duo*                          1993  ?         ?                      92641C-1   CPS-B-21  DL-0921-10014  ?
Quiz Tonosama no Yabou 2                    1995  ?         ?                      ?          ?
Pnickies                                    1994  ?         ?                      ?          CPS-B-21? DL-0921-10014? ?
Pang 3                                      1995  94916-10  CP1B1F,CP1B8K,CP1B9KA  92631C-6   CPS-B-21  DL-0921-10014  C632  IOC1
Megaman the Power Battle                    1995  91634B-2  RCM63B    BPRG1  IOB1  92631C-6   CPS-B-21  DL-0921-10014  C632  IOC1

@actually CPS-B-01, the original number was scratched out and "04" stamped over it
*denotes Suicide Battery

The C628/C632 PALs on some C-boards probably handle the extra inputs (6 buttons/third player).

You can set the suicide CPS-B-21 chips to their default layer register and priority bit values
if you pull pins 45 and 46 high (floating the pins seems to work, too). The default is the same
values as Street Fighter 2 CE/Turbo.



CPS-A Registers
---------------
0x00-0x01     OBJ RAM base (/256)
0x02-0x03     Scroll1 (8x8) RAM base (/256)
0x04-0x05     Scroll2 (16x16) RAM base (/256)
0x06-0x07     Scroll3 (32x32) RAM base (/256)
0x08-0x09     rowscroll RAM base (/256)
0x0a-0x0b     Palette base (/256) after this register is written to, the palette
              is copied from gfxram to the dedicated ram. The palette control
              register (see below) determines how the copy should happen.
              Tests on a msword pcb show that the minimum alignment for the palette
              is 0x400 bytes. The hardware seems to ignore bit 1, while when bit 0
              is set the palette doesn't seem to be copied. However, some games set
              bit 0 during boot (ghouls, strider, 1941) so it still isn't clear
              what bit 0 should actually do.
0x0c-0x0d     Scroll 1 X
0x0e-0x0f     Scroll 1 Y
0x10-0x11     Scroll 2 X
0x12-0x13     Scroll 2 Y
0x14-0x15     Scroll 3 X
0x16-0x17     Scroll 3 Y
0x18-0x19     Starfield 1 X
0x1a-0x1b     Starfield 1 Y
0x1c-0x1d     Starfield 2 X
0x1e-0x1f     Starfield 2 Y
0x20-0x21     start offset for the rowscroll matrix
0x22-0x23     video control. Usually 0x0e.
              bit 0 enables rowscroll on layer 2.
              bit 15 is flip screen.
              ghouls sets bit 14. Purpose unknown.
              1941 uses bits 1-3 by setting them to 0 on screen transitions,
              however it also uses the normal layer control register so there
              doesn't seem to be an obvious effect.

              Games known to use rowscroll:
              SF2
              Mega Twins (underwater, cave)
              Carrier Air Wing (hazy background at beginning of mission 8, put 07 at ff8501 to jump there)
              Magic Sword (fire on floor 3; screen distort after continue)
              Varth (title screen, end of stage 4)
              Captain Commando (end game sequence)

              Tests done on msword at the beginning of gameplay (many thanks to Corrado Tomaselli for these):
              3e  is the default value set by the game (not 0e like most games)
              3c  the last two rows of scroll1 are repeated on the whole screen
              3a  scroll2 is disabled
              36  scroll3 is disabled
              2e  no visible differences
              1e  no visible differences
              one might suspect that bits 4&5 should disable the star layers, but
              Strider sets this register to 0x0e so that's not possible.

              TODO:
              the scroll2/scroll3 disable bits are supported by the emulation,
              while the scroll1 weird effect is not (it doesn't seem to make a
              difference in any game).


CPS-B Registers
---------------
Unlike CPS-A registers, which are at fixed addresses, CPS-B registers move from game to game.
Following example strider

0x66-0x67   Layer control register
            bits 14-15 seem to be unused
                ghouls sets bits 15 in service mode when you press button 2 in
                the input test, with no apparent effect on the pcb.
                qtono2 sets them both at the game over screen.
            bits 6-13 (4 groups of 2 bits) select layer draw order
            bits 1-5 enable the three tilemap layers and the two starfield
                layers (the bit order changes from game to game).
                Only Forgotten Worlds and Strider use the starfield.
            bit 0 could be rowscroll related. It is set by captain commando,
                varth, mtwins, mssword, cawing while rowscroll is active. However
                kodj and sf2 do NOT set this bit while they are using rowscroll.
                Tests on the msword pcb show that even if this bit is not set,
                rowscroll still works. Therefore, the purpose of this bit is unclear.
0x68-0x69   Priority mask \   Tiles in the layer just below sprites can have
0x6a-0x6b   Priority mask |   four priority levels, each one associated with one
0x6c-0x6d   Priority mask |   of these masks. The masks indicate pens in the tile
0x6e-0x6f   Priority mask /   that have priority over sprites.
0x70-0x71   Palette control register. This indicates which palette
            pages to copy when the palette base register is written to.
            There is one CPS2 game (Slammasters II) setting this to 0x2f; all the other
            games normally set it to 0x3f, though in some cases different values are
            used during boot:
            ghouls  0x02 (and palette base is set to 9105; palette base is 9100 normally)
            strider 0x02 (and palette base is set to 9145; palette base is 9140 normally)
            1941    0x02 (and palette base is set to 9145; palette base is 9140 normally)
            unsquad 0x0f
            kod     0x0f
            mtwins  0x0f

            bit 0: copy page 0 (sprites)
            bit 1: copy page 1 (scroll1)
            bit 2: copy page 2 (scroll2)
            bit 3: copy page 3 (scroll3)
            bit 4: copy page 4 (stars1)
            bit 5: copy page 5 (stars2)

            An important quirk is that if the first bits are not set, page 0 in
            gfxram is not skipped but instead is copied to the first enabled page.
            For the other pages, if the bit is not set the gfxram page is skipped.
            Example: 0x0a
            bit 0 is not set so palette page 0 (sprites) is not updated
            bit 1 is set so palette page 1 (scroll1) is updated; since bit 0 was
            not set, it is taken from gfxram page 0
            bit 2 is not set so palette page 2 (scroll2) is not updated; gfxram
            page 1 is skipped
            bit 3 is set so palette page 3 (scroll3) is updated; it is taken from
            gfxram page 2

            bits 0-3 have been verified on a msword pcb, while bits 4-5 are only
            supposed.


A special note has to be made about tile/sprite codes. Even if all graphics are
stored together in the same ROMs, the hardware knows which part of the ROM space
is 8x8 tiles, 16x16 tiles, 16x16 spites, 32x32 tiles, and all games tested only
draw tiles if their code falls in the valid range. If a tile is out of range, it
is replaced by transparent pixels.
Ideally, this shouldn't be important as far as the emulation is concerned, since
games should only request tiles from valid ranges. In practice, many games contain
bugs which make them try to display out of range tiles. The masking applied by
the hardware therefore needs to be emulated properly, otherwise glitches appear.

There are various versions of the ROM board (B-board), so the implementation
details may vary, but in general the tile ranges are controlled by a PAL found
on the B-board (see the table at the top of this file).

The A-board passes 23 bits of address to the B-board when requesting gfx ROM data.
The B-board selects 64 bits of data, that is 16 4bpp pixels, and returns half of
them depending on a signal from the C board.
The 23 address bits are laid out this way (note that the top 3 bits select the
tile type; the purpose of the top bit is unknown):

sprite  000ccccccccccccccccyyyy
scroll1 001?ccccccccccccccccyyy
scroll2 010ccccccccccccccccyyyy
scroll3 011ccccccccccccccyyyyyx
stars   100000000sxxxxxyyyyyyyy (to be verified)

where
c is the tile code
y is the y position in the tile
x is the x position in the tile (only applicable to 32x32 tiles)

E.g. on the 89624B-3 board, used by cawing and others, the top 13 bits of the
address are passed to the 16L8B PAL @1A, which outputs the /OE signal for roms
1,3,5,7 (together), 2,4,6,8 (together), 10,12,14,16,20,22,24,26 (together) or
11,13,15,17,21,23,25,27 (together).
Note that in cawing only sockets 1,3,5,7 are populated, so effectively there is
only one bank.

The above would mean that
1) 8x8 and 16x16 tiles have a 16-bit tile code, while
   32x32 tiles have a 14-bit tile code
2) which ROM bank to use is determined by
   bits 15-7 of a 8x8 tile code,
   bits 15-6 of a 16x16 tile code,
   bits 13-4 of a 32x32 tile code

If the PAL decides that the tile code is out of range and doesn't output any /OE
signal, no ROM is read and pullup resistors force the result to all 1 (which
means a transparent tile).

Note that there are several known cases (nemo, cawing, 3wonders, varth, etc.) where
16x16 tiles are used for BOTH sprites and scroll2.

Ideally, the PALs should be dumped and loaded like the ROMs, and the mapping should
be derived from the loaded PALs at run time.
In practice, this is unlikely to happen, so we'll have to tolerate using hardcoded
maps for every game.


Known Bug List
==============
CPS2:
* CPS2 can do raster effects, certainly used by ssf2 (Cammy, DeeJay, T.Hawk levels),
  msh (lava level, early in attract mode) and maybe others (xmcotaj, vsavj).
  IRQ4 is some sort of scanline interrupt used for that purpose.

* Its unknown what CPS2_OBJ_BASE register (0x400000) does but it is not a object base
  register. The base is 0x7000 for all games even if 0x7080 is written to this register
  (checked on real HW). Maybe it sets the object bank used when cps2_objram_bank is set.

* Sprites are currently lagged by one frame to keep sync with backgrounds. This causes
  sprites to stay on screen one frame longer (visible in VSAV attract mode).

Marvel Vs. Capcom
* Sometimes corrupt gfx are displayed on the 32x32 layer as the screen flashes at the
  start of super combo moves. The problem seems to be due to tiles being fetched before
  the first 32x32 tile offset and results in data coming from 16x16 or 8x8 tiles instead.

CPS1:
SF2
* Missing chain in the foreground in Ken's level, and sign in Chun Li's level.
  Those graphics are in the backmost layer. Probably they are leftover from game
  development and aren't supposed to be visible.

3wonders
* writes to output ports 42, 44, 46.

qad
* layer enable mask incomplete


Unknown issues
==============

There are often some redundant high bits in the scroll layer's attributes.
I think that these are spare bits that the game uses to store additional
information, not used by the hardware.
The games seem to use them to mark platforms, kill zones and no-go areas.

***************************************************************************/

#include "driver.h"
#include "cps1.h"

#define VERBOSE 0

/********************************************************************

            Configuration table:

********************************************************************/

/* Game specific data */

#define GFXTYPE_SPRITES   (1<<0)
#define GFXTYPE_SCROLL1   (1<<1)
#define GFXTYPE_SCROLL2   (1<<2)
#define GFXTYPE_SCROLL3   (1<<3)
#define GFXTYPE_STARS     (1<<4)


struct gfx_range
{
	// start and end are as passed by the game (shift adjusted to be all
	// in the same scale a 8x8 tiles): they don't necessarily match the
	// position in ROM.
	int type;
	int start;
	int end;
	int bank;
};

struct CPS1config
{
	const char *name;             /* game driver name */

	/* Some games interrogate a couple of registers on bootup. */
	/* These are CPS1 board B self test checks. They wander from game to */
	/* game. */
	int cpsb_addr;        /* CPS board B test register address */
	int cpsb_value;       /* CPS board B test register expected value */

	/* some games use as a protection check the ability to do 16-bit multiplies */
	/* with a 32-bit result, by writing the factors to two ports and reading the */
	/* result from two other ports. */
	/* It looks like this feature was introduced with 3wonders (CPSB ID = 08xx) */
	int mult_factor1;
	int mult_factor2;
	int mult_result_lo;
	int mult_result_hi;

	/* unknown registers which might be related to the multiply protection */
	int unknown1;
	int unknown2;
	int unknown3;

	int layer_control;
	int priority[4];
	int palette_control;

	/* ideally, the layer enable masks should consist of only one bit, */
	/* but in many cases it is unknown which bit is which. */
	int layer_enable_mask[5];

	/* these depend on the B-board model and PAL */
	int bank_sizes[4];
	const struct gfx_range *bank_mapper;

	/* some C-boards have additional I/O for extra buttons/extra players */
	int in2_addr;
	int in3_addr;
	int out2_addr;

	int bootleg_kludge;
};

static const struct CPS1config *cps1_game_config;


#define __not_applicable__	-1,-1,-1,-1,-1,-1,-1

/*                     CPSB ID    multiply protection      unknown      ctrl     priority masks   palctrl    layer enable masks  */
#define CPS_B_01      -1, 0x0000,          __not_applicable__,          0x26,{0x28,0x2a,0x2c,0x2e},0x30, {0x02,0x04,0x08,0x30,0x30}
#define CPS_B_02     0x20,0x0002,          __not_applicable__,          0x2c,{0x2a,0x28,0x26,0x24},0x22, {0x02,0x04,0x08,0x00,0x00}
#define CPS_B_03      -1, 0x0000,          __not_applicable__,          0x30,{0x2e,0x2c,0x2a,0x28},0x26, {0x20,0x10,0x08,0x00,0x00}
#define CPS_B_04     0x20,0x0004,          __not_applicable__,          0x2e,{0x26,0x30,0x28,0x32},0x2a, {0x02,0x04,0x08,0x00,0x00}
#define CPS_B_05     0x20,0x0005,          __not_applicable__,          0x28,{0x2a,0x2c,0x2e,0x30},0x32, {0x02,0x08,0x20,0x14,0x14}
#define CPS_B_11     0x32,0x0401,          __not_applicable__,          0x26,{0x28,0x2a,0x2c,0x2e},0x30, {0x08,0x10,0x20,0x00,0x00}
#define CPS_B_12     0x20,0x0402,          __not_applicable__,          0x2c,{0x2a,0x28,0x26,0x24},0x22, {0x02,0x04,0x08,0x00,0x00}
#define CPS_B_13     0x2e,0x0403,          __not_applicable__,          0x22,{0x24,0x26,0x28,0x2a},0x2c, {0x20,0x02,0x04,0x00,0x00}
#define CPS_B_14     0x1e,0x0404,          __not_applicable__,          0x12,{0x14,0x16,0x18,0x1a},0x1c, {0x08,0x20,0x10,0x00,0x00}
#define CPS_B_15     0x0e,0x0405,          __not_applicable__,          0x02,{0x04,0x06,0x08,0x0a},0x0c, {0x04,0x02,0x20,0x00,0x00}
#define CPS_B_16     0x00,0x0406,          __not_applicable__,          0x0c,{0x0a,0x08,0x06,0x04},0x02, {0x10,0x0a,0x0a,0x00,0x00}
#define CPS_B_17     0x08,0x0407,          __not_applicable__,          0x14,{0x12,0x10,0x0e,0x0c},0x0a, {0x08,0x10,0x02,0x00,0x00}
#define CPS_B_18     0x10,0x0408,          __not_applicable__,          0x1c,{0x1a,0x18,0x16,0x14},0x12, {0x10,0x08,0x02,0x00,0x00}
#define CPS_B_21_DEF 0x32,  -1,   0x00,0x02,0x04,0x06, 0x08, -1,  -1,   0x26,{0x28,0x2a,0x2c,0x2e},0x30, {0x02,0x04,0x08,0x30,0x30}	// pang3 sets layer enable to 0x26 on startup
#define CPS_B_21_BT1 0x32,0x0800, 0x0e,0x0c,0x0a,0x08, 0x06,0x04,0x02,  0x28,{0x26,0x24,0x22,0x20},0x30, {0x20,0x04,0x08,0x12,0x12}
#define CPS_B_21_BT2  -1,   -1,   0x1e,0x1c,0x1a,0x18,  -1, 0x0c,0x0a,  0x20,{0x2e,0x2c,0x2a,0x28},0x30, {0x30,0x08,0x30,0x00,0x00}
#define CPS_B_21_BT3  -1,   -1,   0x06,0x04,0x02,0x00, 0x0e,0x0c,0x0a,  0x20,{0x2e,0x2c,0x2a,0x28},0x30, {0x20,0x12,0x12,0x00,0x00}
#define CPS_B_21_BT4  -1,   -1,   0x06,0x04,0x02,0x00, 0x1e,0x1c,0x1a,  0x28,{0x26,0x24,0x22,0x20},0x30, {0x20,0x10,0x02,0x00,0x00}
#define CPS_B_21_BT5 0x32,  -1,   0x0e,0x0c,0x0a,0x08, 0x1e,0x1c,0x1a,  0x20,{0x2e,0x2c,0x2a,0x28},0x30, {0x20,0x04,0x02,0x00,0x00}
#define CPS_B_21_BT6  -1,   -1,    -1,  -1,  -1,  -1,   -1,  -1,  -1,   0x20,{0x2e,0x2c,0x2a,0x28},0x30, {0x20,0x14,0x14,0x00,0x00}
#define CPS_B_21_BT7  -1,   -1,    -1,  -1,  -1,  -1,   -1,  -1,  -1,   0x2c,{ -1,  -1,  -1,  -1 },0x12, {0x14,0x02,0x14,0x00,0x00}
#define CPS_B_21_QS1  -1,   -1,    -1,  -1,  -1,  -1,   -1,  -1,  -1,   0x22,{0x24,0x26,0x28,0x2a},0x2c, {0x10,0x08,0x04,0x00,0x00}
#define CPS_B_21_QS2  -1,   -1,    -1,  -1,  -1,  -1,   -1, 0x2e,0x20,  0x0a,{0x0c,0x0e,0x00,0x02},0x04, {0x16,0x16,0x16,0x00,0x00}
#define CPS_B_21_QS3 0x0e,0x0c00,  -1,  -1,  -1,  -1,  0x2c, -1,  -1,   0x12,{0x14,0x16,0x08,0x0a},0x0c, {0x04,0x02,0x20,0x00,0x00}
#define CPS_B_21_QS4 0x2e,0x0c01,  -1,  -1,  -1,  -1,  0x1c,0x1e,0x08,  0x16,{0x00,0x02,0x28,0x2a},0x2c, {0x04,0x08,0x10,0x00,0x00}
#define CPS_B_21_QS5 0x1e,0x0c02,  -1,  -1,  -1,  -1,  0x0c, -1,  -1,   0x2a,{0x2c,0x2e,0x30,0x32},0x1c, {0x04,0x08,0x10,0x00,0x00}
#define HACK_B_1      -1,   -1,    -1,  -1,  -1,  -1,   -1,  -1,  -1,   0x14,{0x12,0x10,0x0e,0x0c},0x0a, {0x0e,0x0e,0x0e,0x30,0x30}

/*
CPS_B_21_DEF is CPS-B-21 at default settings (no battery)
CPS_B_21_BTx are various battery configurations
CPS_B_21_QSx are various battery configurations in Q-Sound games
*/


// LWCHR and LW621 are equivalent as far as the game is concerned, though the
// equations are different

#define mapper_LWCHR	{ 0x8000, 0x8000, 0, 0 }, mapper_LWCHR_table
static const struct gfx_range mapper_LWCHR_table[] =
{
	// verified from PAL dump (PAL16P8B @ 3A):
	// bank 0 = pin 19 (ROMs 1,5,8,12)
	// bank 1 = pin 16 (ROMs 2,6,9,13)
	// pin 12 and pin 14 are always enabled (except for stars)
	// note that allowed codes go up to 0x1ffff but physical ROM is half that size

	/* type            start    end      bank */
	{ GFXTYPE_SPRITES, 0x00000, 0x07fff, 0 },
	{ GFXTYPE_SCROLL1, 0x00000, 0x1ffff, 0 },

	{ GFXTYPE_STARS,   0x00000, 0x1ffff, 1 },
	{ GFXTYPE_SCROLL2, 0x00000, 0x1ffff, 1 },
	{ GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 1 },
	{ 0 }
};

#define mapper_LW621	{ 0x8000, 0x8000, 0, 0 }, mapper_LW621_table
static const struct gfx_range mapper_LW621_table[] =
{
	// verified from PAL dump (PAL @ 1A):
	// bank 0 = pin 18
	// bank 1 = pin 14
	// pins 19, 16, 17, and 12 give an alternate half-size mapping which would
	// allow to use smaller ROMs:
	// pin 19
	// 0 00000-03fff
	// pin 16
	// 0 04000-07fff
	// 1 00000-1ffff
	// pin 17
	// 2 00000-1ffff
	// 3 00000-1ffff
	// 4 00000-1ffff
	// pin 12
	// 3 00000-1ffff
	//
	// note that allowed codes go up to 0x1ffff but physical ROM is half that size

	/* type            start    end      bank */
	{ GFXTYPE_SPRITES, 0x00000, 0x07fff, 0 },
	{ GFXTYPE_SCROLL1, 0x00000, 0x1ffff, 0 },

	{ GFXTYPE_STARS,   0x00000, 0x1ffff, 1 },
	{ GFXTYPE_SCROLL2, 0x00000, 0x1ffff, 1 },
	{ GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 1 },
	{ 0 }
};


// DM620, DM22A and DAM63B are equivalent as far as the game is concerned, though
// the equations are quite different

#define mapper_DM620	{ 0x8000, 0x2000, 0x2000, 0 }, mapper_DM620_table
static const struct gfx_range mapper_DM620_table[] =
{
	// verified from PAL dump (PAL16P8B @ 2A):
	// bank 0 = pin 19 (ROMs  5,6,7,8)
	// bank 1 = pin 16 (ROMs  9,11,13,15,18,20,22,24)
	// bank 2 = pin 14 (ROMs 10,12,14,16,19,21,23,25)
	// pin 12 is never enabled
	// note that bank 0 is enabled whenever banks 1 or 2 are not enabled,
	// which would make it highly redundant, so I'm relying on the table
	// to be scanned top to bottom and using a catch-all clause at the end.

	/* type            start   end     bank */
	{ GFXTYPE_SCROLL3, 0x8000, 0xbfff, 1 },

	{ GFXTYPE_SPRITES, 0x2000, 0x3fff, 2 },

	{ GFXTYPE_STARS | GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 0 },
	{ 0 }
};

#define mapper_DM22A	{ 0x4000, 0x4000, 0x2000, 0x2000 }, mapper_DM22A_table
static const struct gfx_range mapper_DM22A_table[] =
{
	// verified from PAL dump
	// bank 0 = pin 19
	// bank 1 = pin 16
	// bank 2 = pin 14
	// bank 3 = pin 12

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x00000, 0x01fff, 0 },
	{ GFXTYPE_SCROLL1, 0x02000, 0x03fff, 0 },

	{ GFXTYPE_SCROLL2, 0x04000, 0x07fff, 1 },

	{ GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 2 },

	{ GFXTYPE_SPRITES, 0x02000, 0x03fff, 3 },
	{ 0 }
};

#define mapper_DAM63B	{ 0x8000, 0x8000, 0, 0 }, mapper_DAM63B_table
static const struct gfx_range mapper_DAM63B_table[] =
{
	// verified from PAL dump:
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// bank1 = pin 17 (ROMs 5,7) & pin 16 (ROMs 6,8)
	// pins 12,13,14,15 are always enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x00000, 0x01fff, 0 },
	{ GFXTYPE_SCROLL1, 0x02000, 0x02fff, 0 },
	{ GFXTYPE_SCROLL2, 0x04000, 0x07fff, 0 },

	{ GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 1 },
	{ GFXTYPE_SPRITES, 0x02000, 0x03fff, 1 },
	{ 0 }
};


// ST24M1 and ST22B are equivalent except for the stars range which is
// different. This has no practical effect.

#define mapper_ST24M1	{ 0x8000, 0x8000, 0, 0 }, mapper_ST24M1_table
static const struct gfx_range mapper_ST24M1_table[] =
{
	// verified from PAL dump
	// bank 0 = pin 19 (ROMs 2,4,6,8)
	// bank 1 = pin 16 (ROMs 1,3,5,7)
	// pin 12 and pin 14 are never enabled

	/* type            start    end      bank */
	{ GFXTYPE_STARS,   0x00000, 0x003ff, 0 },
	{ GFXTYPE_SPRITES, 0x00000, 0x04fff, 0 },
	{ GFXTYPE_SCROLL2, 0x04000, 0x07fff, 0 },

	{ GFXTYPE_SCROLL3, 0x00000, 0x07fff, 1 },
	{ GFXTYPE_SCROLL1, 0x07000, 0x07fff, 1 },
	{ 0 }
};

#define mapper_ST22B	{ 0x4000, 0x4000, 0x4000, 0x4000 }, mapper_ST22B_table
static const struct gfx_range mapper_ST22B_table[] =
{
	// verified from PAL dump
	// bank 0 = pin 19 (ROMs 1,5, 9,13,17,24,32,38)
	// bank 1 = pin 16 (ROMs 2,6,10,14,18,25,33,39)
	// bank 2 = pin 14 (ROMs 3,7,11,15,19,21,26,28)
	// bank 3 = pin 12 (ROMS 4,8,12,16,20,22,27,29)

	/* type            start    end      bank */
	{ GFXTYPE_STARS,   0x00000, 0x1ffff, 0 },
	{ GFXTYPE_SPRITES, 0x00000, 0x03fff, 0 },

	{ GFXTYPE_SPRITES, 0x04000, 0x04fff, 1 },
	{ GFXTYPE_SCROLL2, 0x04000, 0x07fff, 1 },

	{ GFXTYPE_SCROLL3, 0x00000, 0x03fff, 2 },

	{ GFXTYPE_SCROLL3, 0x04000, 0x07fff, 3 },
	{ GFXTYPE_SCROLL1, 0x07000, 0x07fff, 3 },
	{ 0 }
};


#define mapper_TK22B	{ 0x4000, 0x4000, 0x4000, 0x4000 }, mapper_TK22B_table
static const struct gfx_range mapper_TK22B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 1,5, 9,13,17,24,32,38)
	// bank 1 = pin 16 (ROMs 2,6,10,14,18,25,33,39)
	// bank 2 = pin 14 (ROMs 3,7,11,15,19,21,26,28)
	// bank 3 = pin 12 (ROMS 4,8,12,16,20,22,27,29)

	/* type            start  end      bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x3fff, 0 },

	{ GFXTYPE_SPRITES, 0x4000, 0x5fff, 1 },
	{ GFXTYPE_SCROLL1, 0x6000, 0x7fff, 1 },

	{ GFXTYPE_SCROLL3, 0x0000, 0x3fff, 2 },

	{ GFXTYPE_SCROLL2, 0x4000, 0x7fff, 3 },
	{ 0 }
};


#define mapper_WL24B	{ 0x8000, 0x8000, 0, 0 }, mapper_WL24B_table
static const struct gfx_range mapper_WL24B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// bank 1 = pin 12 (ROMs 10,12,14,16,20,22,24,26)
	// pin 14 and pin 19 are never enabled

	/* type            start  end      bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x4fff, 0 },
	{ GFXTYPE_SCROLL3, 0x5000, 0x6fff, 0 },
	{ GFXTYPE_SCROLL1, 0x7000, 0x7fff, 0 },

	{ GFXTYPE_SCROLL2, 0x0000, 0x3fff, 1 },
	{ 0 }
};


#define mapper_S224B	{ 0x8000, 0, 0, 0 }, mapper_S224B_table
static const struct gfx_range mapper_S224B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// pin 12 & pin 14 give an alternate half-size mapping which would allow to
	// populate the 8-bit ROM sockets instead of the 16-bit ones:
	// pin 12
	// 0 00000 - 03fff
	// pin 14
	// 0 04000 - 043ff
	// 1 04400 - 04bff
	// 2 06000 - 07fff
	// 3 04c00 - 05fff
	// pin 19 is never enabled

	/* type            start  end      bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x43ff, 0 },
	{ GFXTYPE_SCROLL1, 0x4400, 0x4bff, 0 },
	{ GFXTYPE_SCROLL3, 0x4c00, 0x5fff, 0 },
	{ GFXTYPE_SCROLL2, 0x6000, 0x7fff, 0 },
	{ 0 }
};


#define mapper_YI24B	{ 0x8000, 0, 0, 0 }, mapper_YI24B_table
static const struct gfx_range mapper_YI24B_table[] =
{
	// verified from JED:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// pin 12 & pin 14 give an alternate half-size mapping which would allow to
	// populate the 8-bit ROM sockets instead of the 16-bit ones:
	// pin 12
	// 0 0000-1fff
	// 3 2000-3fff
	// pin 14
	// 1 4000-47ff
	// 2 4800-7fff
	// pin 19 is never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x1fff, 0 },
	{ GFXTYPE_SCROLL3, 0x2000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL1, 0x4000, 0x47ff, 0 },
	{ GFXTYPE_SCROLL2, 0x4800, 0x7fff, 0 },
	{ 0 }
};


// AR24B and AR22B are equivalent, but since we could dump both PALs we are
// documenting both.

#define mapper_AR24B	{ 0x8000, 0, 0, 0 }, mapper_AR24B_table
static const struct gfx_range mapper_AR24B_table[] =
{
	// verified from JED:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// pin 12 & pin 14 give an alternate half-size mapping which would allow to
	// populate the 8-bit ROM sockets instead of the 16-bit ones:
	// pin 12
	// 0 0000-2fff
	// 1 3000-3fff
	// pin 14
	// 2 4000-5fff
	// 3 6000-7fff
	// pin 19 is never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL1, 0x3000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL2, 0x4000, 0x5fff, 0 },
	{ GFXTYPE_SCROLL3, 0x6000, 0x7fff, 0 },
	{ 0 }
};

#define mapper_AR22B	{ 0x4000, 0x4000, 0, 0 }, mapper_AR22B_table
static const struct gfx_range mapper_AR22B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 1,5, 9,13,17,24,32,38)
	// bank 1 = pin 16 (ROMs 2,6,10,14,18,25,33,39)
	// pins 12 and 14 are tristated

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL1, 0x3000, 0x3fff, 0 },

	{ GFXTYPE_SCROLL2, 0x4000, 0x5fff, 1 },
	{ GFXTYPE_SCROLL3, 0x6000, 0x7fff, 1 },
	{ 0 }
};


#define mapper_O224B	{ 0x8000, 0x4000, 0, 0 }, mapper_O224B_table
static const struct gfx_range mapper_O224B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 2,4,6,8)
	// bank 1 = pin 12 (ROMs 10,12,14,16,20,22,24,26)
	// pin 16 & pin 14 appear to be an alternate half-size mapping for bank 0
	// but scroll1 is missing:
	// pin 16
	// 2 00c00 - 03bff
	// 3 03c00 - 03fff
	// pin 14
	// 3 04000 - 04bff
	// 0 04c00 - 07fff

	/* type            start   end     bank */
	{ GFXTYPE_SCROLL1, 0x0000, 0x0bff, 0 },
	{ GFXTYPE_SCROLL2, 0x0c00, 0x3bff, 0 },
	{ GFXTYPE_SCROLL3, 0x3c00, 0x4bff, 0 },
	{ GFXTYPE_SPRITES, 0x4c00, 0x7fff, 0 },

	{ GFXTYPE_SPRITES, 0x8000, 0xa7ff, 1 },
	{ GFXTYPE_SCROLL2, 0xa800, 0xb7ff, 1 },
	{ GFXTYPE_SCROLL3, 0xb800, 0xbfff, 1 },
	{ 0 }
};


#define mapper_MS24B	{ 0x8000, 0, 0, 0 }, mapper_MS24B_table
static const struct gfx_range mapper_MS24B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// pin 14 duplicates pin 16 allowing to populate the 8-bit ROM sockets
	// instead of the 16-bit ones.
	// pin 12 is enabled only for sprites:
	// 0 0000-3fff
	// pin 19 is never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL1, 0x4000, 0x4fff, 0 },
	{ GFXTYPE_SCROLL2, 0x5000, 0x6fff, 0 },
	{ GFXTYPE_SCROLL3, 0x7000, 0x7fff, 0 },
	{ 0 }
};


#define mapper_CK24B	{ 0x8000, 0, 0, 0 }, mapper_CK24B_table
static const struct gfx_range mapper_CK24B_table[] =
{
	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL1, 0x3000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL2, 0x4000, 0x6fff, 0 },
	{ GFXTYPE_SCROLL3, 0x7000, 0x7fff, 0 },
	{ 0 }
};


#define mapper_NM24B	{ 0x8000, 0, 0, 0 }, mapper_NM24B_table
static const struct gfx_range mapper_NM24B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// pin 12 & pin 14 give an alternate half-size mapping which would allow to
	// populate the 8-bit ROM sockets instead of the 16-bit ones:
	// pin 12
	// 0 00000 - 03fff
	// 2 00000 - 03fff
	// pin 14
	// 1 04000 - 047ff
	// 0 04800 - 067ff
	// 2 04800 - 067ff
	// 3 06800 - 07fff
	// pin 19 is never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL2, 0x0000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL1, 0x4000, 0x47ff, 0 },
	{ GFXTYPE_SPRITES, 0x4800, 0x67ff, 0 },
	{ GFXTYPE_SCROLL2, 0x4800, 0x67ff, 0 },
	{ GFXTYPE_SCROLL3, 0x6800, 0x7fff, 0 },
	{ 0 }
};


// CA24B and CA22B are equivalent, but since we could dump both PALs we are
// documenting both.

#define mapper_CA24B	{ 0x8000, 0, 0, 0 }, mapper_CA24B_table
static const struct gfx_range mapper_CA24B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// pin 12 & pin 14 give an alternate half-size mapping which would allow to
	// populate the 8-bit ROM sockets instead of the 16-bit ones:
	// pin 12
	// 0 0000-2fff
	// 2 0000-2fff
	// 3 3000-3fff
	// pin 14
	// 3 4000-4fff
	// 1 5000-57ff
	// 0 5800-7fff
	// 2 5800-7fff
	// pin 19 is never enabled (actually it is always enabled when PAL pin 1 is 1, purpose unknown)

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL2, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL3, 0x3000, 0x4fff, 0 },
	{ GFXTYPE_SCROLL1, 0x5000, 0x57ff, 0 },
	{ GFXTYPE_SPRITES, 0x5800, 0x7fff, 0 },
	{ GFXTYPE_SCROLL2, 0x5800, 0x7fff, 0 },
	{ 0 }
};

#define mapper_CA22B	{ 0x4000, 0x4000, 0, 0 }, mapper_CA22B_table
static const struct gfx_range mapper_CA22B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 1,5, 9,13,17,24,32,38)
	// bank 1 = pin 16 (ROMs 2,6,10,14,18,25,33,39)
	// pin 12 and pin 14 are never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL2, 0x0000, 0x2fff, 0 },
	{ GFXTYPE_SCROLL3, 0x3000, 0x3fff, 0 },

	{ GFXTYPE_SCROLL3, 0x4000, 0x4fff, 1 },
	{ GFXTYPE_SCROLL1, 0x5000, 0x57ff, 1 },
	{ GFXTYPE_SPRITES, 0x5800, 0x7fff, 1 },
	{ GFXTYPE_SCROLL2, 0x5800, 0x7fff, 1 },
	{ 0 }
};


#define mapper_STF29	{ 0x8000, 0x8000, 0x8000, 0 }, mapper_STF29_table
static const struct gfx_range mapper_STF29_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 5,6,7,8)
	// bank 1 = pin 14 (ROMs 14,15,16,17)
	// bank 2 = pin 12 (ROMS 24,25,26,27)

	/* type            start    end      bank */
	{ GFXTYPE_SPRITES, 0x00000, 0x07fff, 0 },

	{ GFXTYPE_SPRITES, 0x08000, 0x0ffff, 1 },

	{ GFXTYPE_SPRITES, 0x10000, 0x11fff, 2 },
	{ GFXTYPE_SCROLL3, 0x02000, 0x03fff, 2 },
	{ GFXTYPE_SCROLL1, 0x04000, 0x04fff, 2 },
	{ GFXTYPE_SCROLL2, 0x05000, 0x07fff, 2 },
	{ 0 }
};



// RT24B and RT22B are equivalent, but since we could dump both PALs we are
// documenting both.

#define mapper_RT24B	{ 0x8000, 0x8000, 0, 0 }, mapper_RT24B_table
static const struct gfx_range mapper_RT24B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 16 (ROMs 1,3,5,7)
	// bank 1 = pin 19 (ROMs 2,4,6,8)
	// pin 12 & pin 14 are never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x53ff, 0 },
	{ GFXTYPE_SCROLL1, 0x5400, 0x6fff, 0 },
	{ GFXTYPE_SCROLL3, 0x7000, 0x7fff, 0 },

	{ GFXTYPE_SCROLL3, 0x0000, 0x3fff, 1 },
	{ GFXTYPE_SCROLL2, 0x2800, 0x7fff, 1 },
	{ GFXTYPE_SPRITES, 0x5400, 0x7fff, 1 },
	{ 0 }
};

#define mapper_RT22B	{ 0x4000, 0x4000, 0x4000, 0x4000 }, mapper_RT22B_table
static const struct gfx_range mapper_RT22B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 1,5, 9,13,17,24,32,38)
	// bank 1 = pin 16 (ROMs 2,6,10,14,18,25,33,39)
	// bank 2 = pin 14 (ROMs 3,7,11,15,19,21,26,28)
	// bank 3 = pin 12 (ROMS 4,8,12,16,20,22,27,29)

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x3fff, 0 },

	{ GFXTYPE_SPRITES, 0x4000, 0x53ff, 1 },
	{ GFXTYPE_SCROLL1, 0x5400, 0x6fff, 1 },
	{ GFXTYPE_SCROLL3, 0x7000, 0x7fff, 1 },

	{ GFXTYPE_SCROLL3, 0x0000, 0x3fff, 2 },
	{ GFXTYPE_SCROLL2, 0x2800, 0x3fff, 2 },

	{ GFXTYPE_SCROLL2, 0x4000, 0x7fff, 3 },
	{ GFXTYPE_SPRITES, 0x5400, 0x7fff, 3 },
	{ 0 }
};


#define mapper_KD29B	{ 0x8000, 0x8000, 0, 0 }, mapper_KD29B_table
static const struct gfx_range mapper_KD29B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 1,2,3,4)
	// bank 1 = pin 14 (ROMs 10,11,12,13)
	// pin 12 is never enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x7fff, 0 },

	{ GFXTYPE_SPRITES, 0x8000, 0x8fff, 1 },
	{ GFXTYPE_SCROLL2, 0x9000, 0xbfff, 1 },
	{ GFXTYPE_SCROLL1, 0xc000, 0xd7ff, 1 },
	{ GFXTYPE_SCROLL3, 0xd800, 0xffff, 1 },
	{ 0 }
};


#define mapper_CC63B	{ 0x8000, 0x8000, 0, 0 }, mapper_CC63B_table
static const struct gfx_range mapper_CC63B_table[] =
{
	// verified from PAL dump:
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// bank1 = pin 17 (ROMs 5,7) & pin 16 (ROMs 6,8)
	// pins 12,13,14,15 are always enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x7fff, 0 },
	{ GFXTYPE_SCROLL2, 0x0000, 0x7fff, 0 },

	{ GFXTYPE_SPRITES, 0x8000, 0xffff, 1 },
	{ GFXTYPE_SCROLL1, 0x8000, 0xffff, 1 },
	{ GFXTYPE_SCROLL2, 0x8000, 0xffff, 1 },
	{ GFXTYPE_SCROLL3, 0x8000, 0xffff, 1 },
	{ 0 }
};


#define mapper_KR63B	{ 0x8000, 0x8000, 0, 0 }, mapper_KR63B_table
static const struct gfx_range mapper_KR63B_table[] =
{
	// verified from PAL dump:
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// bank1 = pin 17 (ROMs 5,7) & pin 16 (ROMs 6,8)
	// pins 12,13,14,15 are always enabled

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x7fff, 0 },
	{ GFXTYPE_SCROLL2, 0x0000, 0x7fff, 0 },

	{ GFXTYPE_SCROLL1, 0x8000, 0x9fff, 1 },
	{ GFXTYPE_SPRITES, 0x8000, 0xcfff, 1 },
	{ GFXTYPE_SCROLL2, 0x8000, 0xcfff, 1 },
	{ GFXTYPE_SCROLL3, 0xd000, 0xffff, 1 },
	{ 0 }
};


#define mapper_S9263B	{ 0x8000, 0x8000, 0x8000, 0 }, mapper_S9263B_table
static const struct gfx_range mapper_S9263B_table[] =
{
	// verified from PAL dump:
	// FIXME there is some problem with this dump since pin 14 is never enabled
	// instead of being the same as pin 15 as expected
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// bank1 = pin 17 (ROMs 5,7) & pin 16 (ROMs 6,8)
	// bank2 = pin 15 (ROMs 10,12) & pin 14 (ROMs 11,13)
	// pins 12 and 13 are the same as 14 and 15

	/* type            start    end      bank */
	{ GFXTYPE_SPRITES, 0x00000, 0x07fff, 0 },

	{ GFXTYPE_SPRITES, 0x08000, 0x0ffff, 1 },

	{ GFXTYPE_SPRITES, 0x10000, 0x11fff, 2 },
	{ GFXTYPE_SCROLL3, 0x02000, 0x03fff, 2 },
	{ GFXTYPE_SCROLL1, 0x04000, 0x04fff, 2 },
	{ GFXTYPE_SCROLL2, 0x05000, 0x07fff, 2 },
	{ 0 }
};


// VA22B and VA63B are equivalent, but since we could dump both PALs we are
// documenting both.

#define mapper_VA22B	{ 0x4000, 0x4000, 0, 0 }, mapper_VA22B_table
static const struct gfx_range mapper_VA22B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19 (ROMs 1,5, 9,13,17,24,32,38)
	// bank 1 = pin 16 (ROMs 2,6,10,14,18,25,33,39)
	// pin 12 and pin 14 are never enabled

	/* type                                                                  start    end      bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x03fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x04000, 0x07fff, 1 },
	{ 0 }
};

#define mapper_VA63B	{ 0x8000, 0, 0, 0 }, mapper_VA63B_table
static const struct gfx_range mapper_VA63B_table[] =
{
	// verified from PAL dump (PAL # uncertain):
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// pins 12,13,14,15,16,17 are never enabled

	/* type                                                                  start    end      bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x07fff, 0 },
	{ 0 }
};


#define mapper_Q522B	{ 0x8000, 0, 0, 0 }, mapper_Q522B_table
static const struct gfx_range mapper_Q522B_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x0000, 0x6fff, 0 },
	{ GFXTYPE_SCROLL3,                   0x7000, 0x77ff, 0 },
	{ GFXTYPE_SCROLL1,                   0x7800, 0x7fff, 0 },
	{ 0 }
};


#define mapper_TK263B	{ 0x8000, 0x8000, 0, 0 }, mapper_TK263B_table
static const struct gfx_range mapper_TK263B_table[] =
{
	// verified from PAL dump:
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// bank1 = pin 17 (ROMs 5,7) & pin 16 (ROMs 6,8)
	// pins 12,13,14,15 are always enabled

	/* type                                                                  start    end      bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x07fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x08000, 0x0ffff, 1 },
	{ 0 }
};


#define mapper_CD63B	{ 0x8000, 0x8000, 0, 0 }, mapper_CD63B_table
static const struct gfx_range mapper_CD63B_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SCROLL1,                   0x0000, 0x0fff, 0 },
	{ GFXTYPE_SPRITES,                   0x1000, 0x7fff, 0 },

	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x8000, 0xdfff, 1 },
	{ GFXTYPE_SCROLL3,                   0xe000, 0xffff, 1 },
	{ 0 }
};


#define mapper_PS63B	{ 0x8000, 0x8000, 0, 0 }, mapper_PS63B_table
static const struct gfx_range mapper_PS63B_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SCROLL1,                   0x0000, 0x0fff, 0 },
	{ GFXTYPE_SPRITES,                   0x1000, 0x7fff, 0 },

	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x8000, 0xdbff, 1 },
	{ GFXTYPE_SCROLL3,                   0xdc00, 0xffff, 1 },
	{ 0 }
};


#define mapper_MB63B	{ 0x8000, 0x8000, 0x8000, 0 }, mapper_MB63B_table
static const struct gfx_range mapper_MB63B_table[] =
{
	/* type                              start    end      bank */
	{ GFXTYPE_SCROLL1,                   0x00000, 0x00fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x01000, 0x07fff, 0 },

	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x08000, 0x0ffff, 1 },

	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x10000, 0x167ff, 2 },
	{ GFXTYPE_SCROLL3,                   0x16800, 0x17fff, 2 },
	{ 0 }
};


#define mapper_QD22B	{ 0x4000, 0, 0, 0 }, mapper_QD22B_table
static const struct gfx_range mapper_QD22B_table[] =
{
	// verified from PAL dump:
	// bank 0 = pin 19

	/* type            start   end     bank */
	{ GFXTYPE_SPRITES, 0x0000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL1, 0x0000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL2, 0x0000, 0x3fff, 0 },
	{ GFXTYPE_SCROLL3, 0x0000, 0x3fff, 0 },
	{ 0 }
};


#define mapper_qadj	{ 0x8000, 0, 0, 0 }, mapper_qadj_table
static const struct gfx_range mapper_qadj_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SCROLL1,                   0x0000, 0x07ff, 0 },
	{ GFXTYPE_SCROLL3,                   0x0800, 0x1fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x2000, 0x7fff, 0 },
	{ 0 }
};


#define mapper_qtono2	{ 0x8000, 0x8000, 0, 0 }, mapper_qtono2_table
static const struct gfx_range mapper_qtono2_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SCROLL1,                   0x0000, 0x0fff, 0 },
	{ GFXTYPE_SCROLL3,                   0x1000, 0x3fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x4000, 0x7fff, 0 },

	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x8000, 0xffff, 1 },
	{ 0 }
};


#define mapper_RCM63B	{ 0x8000, 0x8000, 0x8000, 0x8000 }, mapper_RCM63B_table
static const struct gfx_range mapper_RCM63B_table[] =
{
	// verified from PAL dump:
	// bank0 = pin 19 (ROMs 1,3) & pin 18 (ROMs 2,4)
	// bank1 = pin 17 (ROMs 5,7) & pin 16 (ROMs 6,8)
	// bank0 = pin 15 (ROMs 10,12) & pin 14 (ROMs 11,13)
	// bank1 = pin 13 (ROMs 14,16) & pin 12 (ROMs 15,17)

	/* type                                                                  start    end      bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x07fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x08000, 0x0ffff, 1 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x10000, 0x17fff, 2 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x18000, 0x1ffff, 3 },
	{ 0 }
};


#define mapper_pnickj	{ 0x8000, 0, 0, 0 }, mapper_pnickj_table
static const struct gfx_range mapper_pnickj_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SCROLL1,                   0x0000, 0x0fff, 0 },
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x1000, 0x5fff, 0 },
	{ GFXTYPE_SCROLL3,                   0x6000, 0x7fff, 0 },
	{ 0 }
};


#define mapper_pang3	{ 0x8000, 0x8000, 0, 0 }, mapper_pang3_table
static const struct gfx_range mapper_pang3_table[] =
{
	/* type                              start   end     bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x0000, 0x7fff, 0 },

	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL2, 0x8000, 0x9fff, 1 },
	{ GFXTYPE_SCROLL1,                   0xa000, 0xbfff, 1 },
	{ GFXTYPE_SCROLL3,                   0xc000, 0xffff, 1 },
	{ 0 }
};


#ifdef MESS

#define mapper_sfzch	{ 0x20000, 0, 0, 0 }, mapper_sfzch_table
static const struct gfx_range mapper_sfzch_table[] =
{
	/* type                                                                  start    end      bank */
	{ GFXTYPE_SPRITES | GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 0 },
	{ 0 }
};

#endif



/*
  I don't know if CPS2 ROM boards use PALs as well; since all games seem to be
  well behaved, I'll just assume that there is no strong checking of gfx type.
  (sprites are not listed here because they are addressed linearly by the CPS2
  sprite code)
 */
#define mapper_cps2	{ 0x20000, 0x20000, 0, 0 }, mapper_cps2_table
static const struct gfx_range mapper_cps2_table[] =
{
	/* type                                                start    end      bank */
	{ GFXTYPE_SCROLL1 | GFXTYPE_SCROLL2 | GFXTYPE_SCROLL3, 0x00000, 0x1ffff, 1 },	// 20000-3ffff physical
	{ 0 }
};



static const struct CPS1config cps1_config_table[]=
{
	/* name       CPSB         gfx mapper   in2  in3  out2   kludge */
	{"forgottn", CPS_B_01,     mapper_LW621 },
	{"forgottnu",CPS_B_01,     mapper_LWCHR },
	{"lostwrld", CPS_B_01,     mapper_LWCHR },
	{"lostwrldo",CPS_B_01,     mapper_LWCHR },
	{"ghouls",   CPS_B_01,     mapper_DM620 },
	{"ghoulsu",  CPS_B_01,     mapper_DM620 },
	{"daimakai", CPS_B_01,     mapper_DM22A },	// equivalent to DM620
	{"daimakair",CPS_B_21_DEF, mapper_DAM63B },	// equivalent to DM620, also CPS_B_21_DEF is equivalent to CPS_B_01
	{"strider",  CPS_B_01,     mapper_ST24M1 },
	{"striderua",CPS_B_01,     mapper_ST24M1 },
	{"striderj", CPS_B_01,     mapper_ST22B },	// equivalent to ST24M1
	{"striderjr",CPS_B_01,     mapper_ST24M1 },
	{"dynwar",   CPS_B_02,     mapper_TK22B },
	{"dynwaru",  CPS_B_02,     mapper_TK22B },
	{"dynwarj",  CPS_B_02,     mapper_TK22B },
	{"willow",   CPS_B_03,     mapper_WL24B },
	{"willowj",  CPS_B_03,     mapper_WL24B },
	{"willowje", CPS_B_03,     mapper_WL24B },
	{"ffight",   CPS_B_04,     mapper_S224B },
	{"ffightu",  CPS_B_04,     mapper_S224B },
	{"ffightua", CPS_B_01,     mapper_S224B },
	{"ffightub", CPS_B_05,     mapper_S224B },
	{"ffightj",  CPS_B_04,     mapper_S224B },
	{"ffightj1", CPS_B_02,     mapper_S224B },
	{"ffightj2", CPS_B_01,     mapper_S224B },
	{"1941",     CPS_B_05,     mapper_YI24B },
	{"1941j",    CPS_B_05,     mapper_YI24B },
	{"unsquad",  CPS_B_11,     mapper_AR24B },	/* CPSB ID not checked, but it's the same as sf2eg */
	{"area88",   CPS_B_11,     mapper_AR22B },	// equivalent to AR24B
	{"mercs",    CPS_B_12,     mapper_O224B,  0x36, 0, 0x34 },
	{"mercsu",   CPS_B_12,     mapper_O224B,  0x36, 0, 0x34 },
	{"mercsua",  CPS_B_12,     mapper_O224B,  0x36, 0, 0x34 },
	{"mercsj",   CPS_B_12,     mapper_O224B,  0x36, 0, 0x34 },
	{"msword",   CPS_B_13,     mapper_MS24B },	/* CPSB ID not checked, but it's the same as sf2j */
	{"mswordr1", CPS_B_13,     mapper_MS24B },	/* CPSB ID not checked, but it's the same as sf2j */
	{"mswordu",  CPS_B_13,     mapper_MS24B },	/* CPSB ID not checked, but it's the same as sf2j */
	{"mswordj",  CPS_B_13,     mapper_MS24B },	/* CPSB ID not checked, but it's the same as sf2j */
	{"mtwins",   CPS_B_14,     mapper_CK24B },
	{"chikij",   CPS_B_14,     mapper_CK24B },
	{"nemo",     CPS_B_15,     mapper_NM24B },
	{"nemoj",    CPS_B_15,     mapper_NM24B },
	{"cawing",   CPS_B_16,     mapper_CA24B },
	{"cawingr1", CPS_B_16,     mapper_CA24B },
	{"cawingu",  CPS_B_16,     mapper_CA24B },
	{"cawingj",  CPS_B_16,     mapper_CA22B },	// equivalent to CA24B
	{"sf2",      CPS_B_11,     mapper_STF29,  0x36 },
	{"sf2eb",    CPS_B_17,     mapper_STF29,  0x36 },
	{"sf2ebbl",  CPS_B_17,     mapper_STF29,  0x36, 0, 0, 1  },
	{"sf2ua",    CPS_B_17,     mapper_STF29,  0x36 },
	{"sf2ub",    CPS_B_17,     mapper_STF29,  0x36 },
	{"sf2ud",    CPS_B_05,     mapper_STF29,  0x36 },
	{"sf2ue",    CPS_B_18,     mapper_STF29,  0x3c },
	{"sf2uf",    CPS_B_15,     mapper_STF29,  0x36 },
	{"sf2ui",    CPS_B_14,     mapper_STF29,  0x36 },
	{"sf2uk",    CPS_B_17,     mapper_STF29,  0x36 }, // check CPS_B
	{"sf2j",     CPS_B_13,     mapper_STF29,  0x36 },
	{"sf2ja",    CPS_B_17,     mapper_STF29,  0x36 },
	{"sf2jc",    CPS_B_12,     mapper_STF29,  0x36 },
	{"sf2qp1",   CPS_B_17,     mapper_STF29,  0x36 },
	/* from here onwards the CPS-B board has suicide battery and multiply protection */
	{"3wonders", CPS_B_21_BT1, mapper_RT24B },
	{"3wondersu",CPS_B_21_BT1, mapper_RT24B },
	{"wonder3",  CPS_B_21_BT1, mapper_RT22B },	// equivalent to RT24B
	{"3wondersh",CPS_B_02    , mapper_RT24B },	/* Not 100% sure of the CPS B-ID */
	{"kod",      CPS_B_21_BT2, mapper_KD29B,  0x36, 0, 0x34 },
	{"kodu",     CPS_B_21_BT2, mapper_KD29B,  0x36, 0, 0x34 },
	{"kodj",     CPS_B_21_BT2, mapper_KD29B,  0x36, 0, 0x34 },
	{"kodb",     CPS_B_21_BT2, mapper_KD29B,  0x36, 0, 0x34 },	/* bootleg, doesn't use multiply protection */
	{"captcomm", CPS_B_21_BT3, mapper_CC63B,  0x36, 0x38, 0x34 },
	{"captcommu",CPS_B_21_BT3, mapper_CC63B,  0x36, 0x38, 0x34 },
	{"captcommj",CPS_B_21_BT3, mapper_CC63B,  0x36, 0x38, 0x34 },
	{"captcommb",CPS_B_21_BT3, mapper_CC63B,  0x36, 0x38, 0x34 },
	{"knights",  CPS_B_21_BT4, mapper_KR63B,  0x36, 0, 0x34 },
	{"knightsu", CPS_B_21_BT4, mapper_KR63B,  0x36, 0, 0x34 },
	{"knightsj", CPS_B_21_BT4, mapper_KR63B,  0x36, 0, 0x34 },
	{"knightsb", CPS_B_21_BT4, mapper_KR63B,  0x36, 0, 0x34 },
	{"sf2ce",    CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2ceua",  CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2ceub",  CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2ceuc",  CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2cej",   CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2rb",    CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2rb2",   CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2rb3",   CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2red",   CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2v004",  CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2acc",   CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2accp2", CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2dkot2", CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2m1",    CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2m2",    CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2m3",    CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2m4",    HACK_B_1,     mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2m5",    CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2m6",    CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2m7",    CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2yyc",   CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2koryu", CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"sf2mdt",   CPS_B_21_DEF, mapper_S9263B, 0x36, 0, 0, 1 },
	{"varth",    CPS_B_04,     mapper_VA63B },	/* CPSB test has been patched out (60=0008) register is also written to, possibly leftover from development */
	{"varthr1",  CPS_B_04,     mapper_VA63B },	/* CPSB test has been patched out (60=0008) register is also written to, possibly leftover from development */
	{"varthu",   CPS_B_04,     mapper_VA63B },	/* CPSB test has been patched out (60=0008) register is also written to, possibly leftover from development */
	{"varthj",   CPS_B_21_BT5, mapper_VA22B },	/* CPSB test has been patched out (72=0001) register is also written to, possibly leftover from development */
	{"cworld2j", CPS_B_21_BT6, mapper_Q522B,  0x36, 0, 0x34 },	/* (ports 36, 34 probably leftover input code from another game) */
	{"wof",      CPS_B_21_DEF, mapper_TK263B },	/* bootleg? */
	{"wofa",     CPS_B_21_DEF, mapper_TK263B },	/* bootleg? */
	{"wofu",     CPS_B_21_QS1, mapper_TK263B },
	{"wofj",     CPS_B_21_QS1, mapper_TK263B },
	{"wofhfh",   CPS_B_21_DEF, mapper_TK263B, 0x36 }, /* Chinese bootleg */
	{"dino",     CPS_B_21_QS2, mapper_CD63B },	/* layer enable never used */
	{"dinou",    CPS_B_21_QS2, mapper_CD63B },	/* layer enable never used */
	{"dinoj",    CPS_B_21_QS2, mapper_CD63B },	/* layer enable never used */
	{"dinopic",  CPS_B_21_QS2, mapper_CD63B },	/* layer enable never used */
	{"dinopic2", CPS_B_21_QS2, mapper_CD63B },	/* layer enable never used */
	{"punisher", CPS_B_21_QS3, mapper_PS63B },
	{"punisheru",CPS_B_21_QS3, mapper_PS63B },
	{"punisherj",CPS_B_21_QS3, mapper_PS63B },
	{"punipic",  CPS_B_21_QS3, mapper_PS63B },
	{"punipic2", CPS_B_21_QS3, mapper_PS63B },
	{"punipic3", CPS_B_21_QS3, mapper_PS63B },
	{"slammast", CPS_B_21_QS4, mapper_MB63B },
	{"slammastu",CPS_B_21_QS4, mapper_MB63B },
	{"mbomberj", CPS_B_21_QS4, mapper_MB63B },
	{"mbombrd",  CPS_B_21_QS5, mapper_MB63B },
	{"mbombrdj", CPS_B_21_QS5, mapper_MB63B },
	{"sf2hf",    CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2t",     CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"sf2tj",    CPS_B_21_DEF, mapper_S9263B, 0x36 },
	{"qad",      CPS_B_21_BT7, mapper_QD22B,  0x36 },	/* TODO: layer enable (port 36 probably leftover input code from another game) */
	{"qadj",     CPS_B_21_DEF, mapper_qadj,   0x36, 0x38, 0x34 },	/* (ports 36, 38, 34 probably leftover input code from another game) */
	{"qtono2",   CPS_B_21_DEF, mapper_qtono2, 0x36, 0x38, 0x34 },	/* (ports 36, 38, 34 probably leftover input code from another game) */
	{"megaman",  CPS_B_21_DEF, mapper_RCM63B },
	{"rockmanj", CPS_B_21_DEF, mapper_RCM63B },
	{"pnickj",   CPS_B_21_DEF, mapper_pnickj },
	{"pang3",    CPS_B_21_DEF, mapper_pang3 },	/* EEPROM port is among the CPS registers (handled by DRIVER_INIT) */
	{"pang3j",   CPS_B_21_DEF, mapper_pang3 },	/* EEPROM port is among the CPS registers (handled by DRIVER_INIT) */
	#ifdef MESS
	{"sfzch",    CPS_B_21_DEF, mapper_sfzch },
	#endif

    /* CPS2 games */
	{"cps2",     CPS_B_21_DEF, mapper_cps2 },

	{0}		/* End of table */
};



/* Public variables */
UINT16 *cps1_gfxram;
UINT16 *cps1_cps_a_regs;
UINT16 *cps1_cps_b_regs;

size_t cps1_gfxram_size;

/* Offset of each palette entry */
#define cps1_palette_entries (32*6)  /* Number colour schemes in palette */

static const int cps1_scroll_size =0x4000;	/* scroll1, scroll2, scroll3 */
static const int cps1_obj_size    =0x0800;
static const int cps1_other_size  =0x0800;
static const int cps1_palette_align=0x0400;	/* minimum alignment is a single palette page (512 colors). Verified on pcb. */
//static const int cps1_palette_size=cps1_palette_entries*32; /* Size of palette RAM */

static UINT16 *cps1_scroll1;
static UINT16 *cps1_scroll2;
static UINT16 *cps1_scroll3;
static UINT16 *cps1_obj;
static UINT16 *cps1_buffered_obj;
UINT16 *cps1_other;

/* Working variables */
static int cps1_last_sprite_offset;     /* Offset of the last sprite */
static int cps1_stars_enabled[2];          /* Layer enabled [Y/N] */

tilemap *cps1_bg_tilemap[3];


int cps1_scroll1x, cps1_scroll1y;
int cps1_scroll2x, cps1_scroll2y;
int cps1_scroll3x, cps1_scroll3y;
static int stars1x, stars1y, stars2x, stars2y;


/* CPS-A registers */
#define CPS1_OBJ_BASE           (0x00/2)    /* Base address of objects */
#define CPS1_SCROLL1_BASE       (0x02/2)    /* Base address of scroll 1 */
#define CPS1_SCROLL2_BASE       (0x04/2)    /* Base address of scroll 2 */
#define CPS1_SCROLL3_BASE       (0x06/2)    /* Base address of scroll 3 */
#define CPS1_OTHER_BASE         (0x08/2)    /* Base address of other video */
#define CPS1_PALETTE_BASE       (0x0a/2)    /* Base address of palette */
#define CPS1_SCROLL1_SCROLLX    (0x0c/2)    /* Scroll 1 X */
#define CPS1_SCROLL1_SCROLLY    (0x0e/2)    /* Scroll 1 Y */
#define CPS1_SCROLL2_SCROLLX    (0x10/2)    /* Scroll 2 X */
#define CPS1_SCROLL2_SCROLLY    (0x12/2)    /* Scroll 2 Y */
#define CPS1_SCROLL3_SCROLLX    (0x14/2)    /* Scroll 3 X */
#define CPS1_SCROLL3_SCROLLY    (0x16/2)    /* Scroll 3 Y */
#define CPS1_STARS1_SCROLLX     (0x18/2)    /* Stars 1 X */
#define CPS1_STARS1_SCROLLY     (0x1a/2)    /* Stars 1 Y */
#define CPS1_STARS2_SCROLLX     (0x1c/2)    /* Stars 2 X */
#define CPS1_STARS2_SCROLLY     (0x1e/2)    /* Stars 2 Y */
#define CPS1_ROWSCROLL_OFFS     (0x20/2)    /* base of row scroll offsets in other RAM */
#define CPS1_VIDEOCONTROL       (0x22/2)    /* flip screen, rowscroll enable */


/*
CPS1 VIDEO RENDERER

*/
/* first 0x4000 of gfx ROM are used, but 0x0000-0x1fff is == 0x2000-0x3fff */
static const int stars_rom_size = 0x2000;

/* PSL: CPS2 support */
static const int cps2_obj_size    =0x2000;
UINT16 *cps2_objram1,*cps2_objram2;
UINT16 *cps2_output;

size_t cps2_output_size;
static UINT16 *cps2_buffered_obj;
static int pri_ctrl;				/* Sprite layer priorities */
static int cps2_objram_bank;
static int cps2_last_sprite_offset;     /* Offset of the last sprite */

#define CPS2_OBJ_BASE	0x00	/* Unknown (not base address of objects). Could be bass address of bank used when object swap bit set? */
#define CPS2_OBJ_UK1	0x02	/* Unknown (nearly always 0x807d, or 0x808e when screen flipped) */
#define CPS2_OBJ_PRI	0x04	/* Layers priorities */
#define CPS2_OBJ_UK2	0x06	/* Unknown (usually 0x0000, 0x1101 in ssf2, 0x0001 in 19XX) */
#define CPS2_OBJ_XOFFS	0x08	/* X offset (usually 0x0040) */
#define CPS2_OBJ_YOFFS	0x0a	/* Y offset (always 0x0010) */


static int cps_version;
int cps1_scanline1;
int cps1_scanline2;
int cps1_scancalls;


static void cps1_build_palette(running_machine *machine, const UINT16* const palette_base);


static MACHINE_RESET( cps )
{
	const char *gamename = machine->gamedrv->name;
	const struct CPS1config *pCFG=&cps1_config_table[0];

	while(pCFG->name)
	{
		if (strcmp(pCFG->name, gamename) == 0)
		{
			break;
		}
		pCFG++;
	}
	cps1_game_config=pCFG;

    if (!cps1_game_config->name)
    {
        gamename="cps2";
        pCFG=&cps1_config_table[0];
        while(pCFG->name)
        {
            if (strcmp(pCFG->name, gamename) == 0)
            {
                break;
            }
            pCFG++;
        }
        cps1_game_config=pCFG;
   }

	if (strcmp(gamename, "sf2rb" )==0)
	{
		/* Patch out protection check */
		UINT16 *rom = (UINT16 *)memory_region(machine, "maincpu");
		rom[0xe5464/2] = 0x6012;
	}
	if (strcmp(gamename, "sf2rb2" )==0)
	{
		/* Patch out protection check */
		UINT16 *rom = (UINT16 *)memory_region(machine, "maincpu");
		rom[0xe5332/2] = 0x6014;
	}

#if 0
	if (strcmp(gamename, "sf2accp2" )==0)
	{
		/* Patch out a odd branch which would be incorrectly interpreted
           by the cpu core as a 32-bit branch. This branch would make the
           game crash (address error, since it would branch to an odd address)
           if location 180ca6 (outside ROM space) isn't 0. Protection check? */
		UINT16 *rom = (UINT16 *)memory_region(machine, "maincpu");
		rom[0x11756/2] = 0x4e71;
	}
	else if (strcmp(gamename, "ghouls" )==0)
	{
		/* Patch out self-test... it takes forever */
		UINT16 *rom = (UINT16 *)memory_region(machine, "maincpu");
		rom[0x61964/2] = 0x4ef9;
		rom[0x61966/2] = 0x0000;
		rom[0x61968/2] = 0x0400;
	}
#endif
}


INLINE UINT16 *cps1_base(int offset,int boundary)
{
	int base = cps1_cps_a_regs[offset]*256;

	/*
    The scroll RAM must start on a 0x4000 boundary.
    Some games do not do this.
    For example:
       Captain commando     - continue screen will not display
       Muscle bomber games  - will animate garbage during gameplay
    Mask out the irrelevant bits.
    */
	base &= ~(boundary-1);
 	return &cps1_gfxram[(base&0x3ffff)/2];
}



WRITE16_HANDLER( cps1_cps_a_w )
{
	data = COMBINE_DATA(&cps1_cps_a_regs[offset]);

	/*
    The main CPU writes the palette to gfxram, and the CPS-B custom copies it
    to the real palette RAM, which is separated from gfxram.
    This is done ONLY after the palette base register is written to. It is not
    known what the exact timing should be, how long it should take and when it
    should happen. We are assuming that the copy happens immediately, since it
    fixes glitches in the ghouls intro, but it might happen at next vblank.
    */
	if (offset == CPS1_PALETTE_BASE)
		cps1_build_palette(space->machine, cps1_base(CPS1_PALETTE_BASE,cps1_palette_align));

	// pzloop2 write to register 24 on startup. This is probably just a bug.
	if (offset == 0x24/2 && cps_version == 2)
		return;

	if (offset > CPS1_VIDEOCONTROL)
		popmessage("write to CPS-A register %02x contact MAMEDEV", offset*2);
}


READ16_HANDLER( cps1_cps_b_r )
{
	/* Some games interrogate a couple of registers on bootup. */
	/* These are CPS1 board B self test checks. They wander from game to */
	/* game. */
	if (offset == cps1_game_config->cpsb_addr/2)
		return cps1_game_config->cpsb_value;

	/* some games use as a protection check the ability to do 16-bit multiplications */
	/* with a 32-bit result, by writing the factors to two ports and reading the */
	/* result from two other ports. */
	if (offset == cps1_game_config->mult_result_lo/2)
		return (cps1_cps_b_regs[cps1_game_config->mult_factor1/2] *
				cps1_cps_b_regs[cps1_game_config->mult_factor2/2]) & 0xffff;
	if (offset == cps1_game_config->mult_result_hi/2)
		return (cps1_cps_b_regs[cps1_game_config->mult_factor1/2] *
				cps1_cps_b_regs[cps1_game_config->mult_factor2/2]) >> 16;

	if (offset == cps1_game_config->in2_addr/2)	/* Extra input ports (on C-board) */
		return input_port_read(space->machine, "IN2");

	if (offset == cps1_game_config->in3_addr/2)	/* Player 4 controls (on C-board) ("Captain Commando") */
		return input_port_read(space->machine, "IN3");

	if (cps_version == 2)
	{
		if (offset == 0x10/2)
		{
			// UNKNOWN--only mmatrix appears to read this, and I'm not sure if the result is actuallyused
			return cps1_cps_b_regs[0x10/2];
		}
		if (offset == 0x12/2)
		{
			return cps1_cps_b_regs[0x12/2];
		}
	}

	popmessage("CPS-B read port %02x contact MAMEDEV", offset*2);

	return 0xffff;
}


WRITE16_HANDLER( cps1_cps_b_w )
{
	data = COMBINE_DATA(&cps1_cps_b_regs[offset]);

	if (cps_version == 2)
	{
		/* To mark scanlines for raster effects */
		if (offset == 0x0e/2)
		{
			// UNKNOWN
			return;
		}
		if (offset == 0x10/2)
		{
			cps1_scanline1 = (data & 0x1ff);
			return;
		}
		if (offset == 0x12/2)
		{
			cps1_scanline2 = (data & 0x1ff);
			return;
		}
	}


	// additional outputs on C-board
	if (offset == cps1_game_config->out2_addr/2)
	{
		if (ACCESSING_BITS_0_7)
		{
			if (cps1_game_config->cpsb_value == 0x0402)	// Mercs (CN2 connector)
			{
				coin_lockout_w(2,~data & 0x01);
				set_led_status(0,data & 0x02);
				set_led_status(1,data & 0x04);
				set_led_status(2,data & 0x08);
			}
			else	// kod, captcomm, knights
			{
				coin_lockout_w(2,~data & 0x02);
				coin_lockout_w(3,~data & 0x08);
			}
		}
	}


	if (offset != cps1_game_config->cpsb_addr/2 &&	// only varth writes here
			offset != cps1_game_config->mult_factor1/2 &&
			offset != cps1_game_config->mult_factor2/2 &&
			offset != cps1_game_config->layer_control/2 &&
			offset != cps1_game_config->unknown1/2 &&
			offset != cps1_game_config->unknown2/2 &&
			offset != cps1_game_config->unknown3/2 &&
			offset != cps1_game_config->priority[0]/2 &&
			offset != cps1_game_config->priority[1]/2 &&
			offset != cps1_game_config->priority[2]/2 &&
			offset != cps1_game_config->priority[3]/2 &&
			offset != cps1_game_config->palette_control/2 &&
			offset != cps1_game_config->out2_addr/2 &&
			!cps1_game_config->bootleg_kludge)
		popmessage("CPS-B write %04x to port %02x contact MAMEDEV", data, offset*2);
}



INLINE int cps2_port(int offset)
{
    return cps2_output[offset/2];
}




static void cps1_gfx_decode(running_machine *machine)
{
	int size=memory_region_length(machine, "gfx");
	int i,j,gfxsize;
	UINT8 *cps1_gfx = memory_region(machine, "gfx");


	gfxsize=size/4;

	for (i = 0;i < gfxsize;i++)
	{
		UINT32 src = cps1_gfx[4*i] + (cps1_gfx[4*i+1]<<8) + (cps1_gfx[4*i+2]<<16) + (cps1_gfx[4*i+3]<<24);
		UINT32 dwval = 0;

		for (j = 0;j < 8;j++)
		{
			int n = 0;
			UINT32 mask = (0x80808080 >> j) & src;

			if (mask & 0x000000ff) n |= 1;
			if (mask & 0x0000ff00) n |= 2;
			if (mask & 0x00ff0000) n |= 4;
			if (mask & 0xff000000) n |= 8;

			dwval |= n << (j * 4);
		}
		cps1_gfx[4*i  ] = dwval>>0;
		cps1_gfx[4*i+1] = dwval>>8;
		cps1_gfx[4*i+2] = dwval>>16;
		cps1_gfx[4*i+3] = dwval>>24;
	}
}

static void unshuffle(UINT64 *buf,int len)
{
	int i;
	UINT64 t;

	if (len == 2) return;

	assert(len % 4 == 0);   /* must not happen */

	len /= 2;

	unshuffle(buf,len);
	unshuffle(buf + len,len);

	for (i = 0;i < len/2;i++)
	{
		t = buf[len/2 + i];
		buf[len/2 + i] = buf[len + i];
		buf[len + i] = t;
	}
}

static void cps2_gfx_decode(running_machine *machine)
{
	const int banksize=0x200000;
	int size=memory_region_length(machine, "gfx");
	int i;

	for (i = 0;i < size;i += banksize)
		unshuffle((UINT64 *)(memory_region(machine, "gfx") + i),banksize/8);

	cps1_gfx_decode(machine);
}


DRIVER_INIT( cps1 )
{
	cps1_gfx_decode(machine);
}



DRIVER_INIT( cps2_video )
{
	cps2_gfx_decode(machine);

	cps1_scanline1 = 262;
	cps1_scanline2 = 262;
	cps1_scancalls = 0;
}


void cps1_get_video_base(void )
{
	int layercontrol, videocontrol, scroll1xoff, scroll2xoff, scroll3xoff;

	/* Re-calculate the VIDEO RAM base */
	if (cps1_scroll1 != cps1_base(CPS1_SCROLL1_BASE,cps1_scroll_size))
	{
		cps1_scroll1 = cps1_base(CPS1_SCROLL1_BASE,cps1_scroll_size);
		tilemap_mark_all_tiles_dirty(cps1_bg_tilemap[0]);
	}
	if (cps1_scroll2 != cps1_base(CPS1_SCROLL2_BASE,cps1_scroll_size))
	{
		cps1_scroll2 = cps1_base(CPS1_SCROLL2_BASE,cps1_scroll_size);
		tilemap_mark_all_tiles_dirty(cps1_bg_tilemap[1]);
	}
	if (cps1_scroll3 != cps1_base(CPS1_SCROLL3_BASE,cps1_scroll_size))
	{
		cps1_scroll3 = cps1_base(CPS1_SCROLL3_BASE,cps1_scroll_size);
		tilemap_mark_all_tiles_dirty(cps1_bg_tilemap[2]);
	}

	/* Some of the sf2 hacks use only sprite port 0x9100 and the scroll layers are offset */
	if (cps1_game_config->bootleg_kludge == 1)
	{
		cps1_cps_a_regs[CPS1_OBJ_BASE] = 0x9100;
		cps1_obj = cps1_base(CPS1_OBJ_BASE, cps1_obj_size);
		scroll1xoff = -0x0c;
		scroll2xoff = -0x0e;
		scroll3xoff = -0x10;
	}
	else
	{
		cps1_obj = cps1_base(CPS1_OBJ_BASE, cps1_obj_size);
		scroll1xoff = 0;
		scroll2xoff = 0;
		scroll3xoff = 0;
	}
	cps1_other = cps1_base(CPS1_OTHER_BASE,cps1_other_size);

	/* Get scroll values */
	cps1_scroll1x = cps1_cps_a_regs[CPS1_SCROLL1_SCROLLX] + scroll1xoff;
	cps1_scroll1y = cps1_cps_a_regs[CPS1_SCROLL1_SCROLLY];
	cps1_scroll2x = cps1_cps_a_regs[CPS1_SCROLL2_SCROLLX] + scroll2xoff;
	cps1_scroll2y = cps1_cps_a_regs[CPS1_SCROLL2_SCROLLY];
	cps1_scroll3x = cps1_cps_a_regs[CPS1_SCROLL3_SCROLLX] + scroll3xoff;
	cps1_scroll3y = cps1_cps_a_regs[CPS1_SCROLL3_SCROLLY];
	stars1x = cps1_cps_a_regs[CPS1_STARS1_SCROLLX];
	stars1y = cps1_cps_a_regs[CPS1_STARS1_SCROLLY];
	stars2x = cps1_cps_a_regs[CPS1_STARS2_SCROLLX];
	stars2y = cps1_cps_a_regs[CPS1_STARS2_SCROLLY];

	/* Get layer enable bits */
	layercontrol = cps1_cps_b_regs[cps1_game_config->layer_control/2];
	videocontrol = cps1_cps_a_regs[CPS1_VIDEOCONTROL];
	tilemap_set_enable(cps1_bg_tilemap[0],layercontrol & cps1_game_config->layer_enable_mask[0]);
	tilemap_set_enable(cps1_bg_tilemap[1],(layercontrol & cps1_game_config->layer_enable_mask[1]) && (videocontrol & 4));
	tilemap_set_enable(cps1_bg_tilemap[2],(layercontrol & cps1_game_config->layer_enable_mask[2]) && (videocontrol & 8));
	cps1_stars_enabled[0] = layercontrol & cps1_game_config->layer_enable_mask[3];
	cps1_stars_enabled[1] = layercontrol & cps1_game_config->layer_enable_mask[4];


{
	int enablemask;

	enablemask = 0;
	if (cps1_game_config->layer_enable_mask[0] == cps1_game_config->layer_enable_mask[1])
		enablemask = cps1_game_config->layer_enable_mask[0];
	if (cps1_game_config->layer_enable_mask[0] == cps1_game_config->layer_enable_mask[2])
		enablemask = cps1_game_config->layer_enable_mask[0];
	if (cps1_game_config->layer_enable_mask[1] == cps1_game_config->layer_enable_mask[2])
		enablemask = cps1_game_config->layer_enable_mask[1];
	if (enablemask)
	{
		if (((layercontrol & enablemask) && (layercontrol & enablemask) != enablemask))
			popmessage("layer %02x contact MAMEDEV",layercontrol&0xc03f);
	}

	enablemask = cps1_game_config->layer_enable_mask[0] | cps1_game_config->layer_enable_mask[1]
			| cps1_game_config->layer_enable_mask[2]
			| cps1_game_config->layer_enable_mask[3] | cps1_game_config->layer_enable_mask[4];
	if (((layercontrol & ~enablemask) & 0x003e) != 0)
		popmessage("layer %02x contact MAMEDEV",layercontrol&0xc03f);
}

}


WRITE16_HANDLER( cps1_gfxram_w )
{
	int page = (offset >> 7) & 0x3c0;
	COMBINE_DATA(&cps1_gfxram[offset]);

	if (page == (cps1_cps_a_regs[CPS1_SCROLL1_BASE] & 0x3c0))
		tilemap_mark_tile_dirty(cps1_bg_tilemap[0],offset/2 & 0x0fff);
	if (page == (cps1_cps_a_regs[CPS1_SCROLL2_BASE] & 0x3c0))
		tilemap_mark_tile_dirty(cps1_bg_tilemap[1],offset/2 & 0x0fff);
	if (page == (cps1_cps_a_regs[CPS1_SCROLL3_BASE] & 0x3c0))
		tilemap_mark_tile_dirty(cps1_bg_tilemap[2],offset/2 & 0x0fff);
}



static int gfxrom_bank_mapper(running_machine *machine, int type, int code)
{
	const struct gfx_range *range = cps1_game_config->bank_mapper;
	int shift = 0;

	assert(range);

	switch (type)
	{
		case GFXTYPE_SPRITES: shift = 1; break;
		case GFXTYPE_SCROLL1: shift = 0; break;
		case GFXTYPE_SCROLL2: shift = 1; break;
		case GFXTYPE_SCROLL3: shift = 3; break;
	}

	code <<= shift;

	while (range->type)
	{
		if (code >= range->start && code <= range->end)
		{
			if (range->type & type)
			{
				int base = 0;
				int i;

				for (i = 0; i < range->bank; ++i)
					base += cps1_game_config->bank_sizes[i];

				return (base + (code & (cps1_game_config->bank_sizes[range->bank] - 1))) >> shift;
			}
		}

		++range;
	}

#ifdef MAME_DEBUG
//  popmessage("tile %02x/%04x out of range", type,code>>shift);
#endif

	return -1;
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

static TILEMAP_MAPPER( tilemap0_scan )
{
	/* logical (col,row) -> memory offset */
	return (row & 0x1f) + ((col & 0x3f) << 5) + ((row & 0x20) << 6);
}

static TILEMAP_MAPPER( tilemap1_scan )
{
	/* logical (col,row) -> memory offset */
	return (row & 0x0f) + ((col & 0x3f) << 4) + ((row & 0x30) << 6);
}

static TILEMAP_MAPPER( tilemap2_scan )
{
	/* logical (col,row) -> memory offset */
	return (row & 0x07) + ((col & 0x3f) << 3) + ((row & 0x38) << 6);
}

static UINT8 empty_tile8x8[8*8];
static UINT8 empty_tile[32*32/2];

static TILE_GET_INFO( get_tile0_info )
{
	int code = cps1_scroll1[2*tile_index];
	int attr = cps1_scroll1[2*tile_index+1];
	int gfxset;

	code = gfxrom_bank_mapper(machine, GFXTYPE_SCROLL1, code);

	/* allows us to reproduce a problem seen with a ffight board where USA and Japanese
         roms have been mixed to be reproduced (ffightua) -- it looks like each column
         should alternate between the left and right side of the 16x16 tiles */
	gfxset = (tile_index & 0x20) >> 5;

	SET_TILE_INFO(
			gfxset,
			code,
			(attr & 0x1f) + 0x20,
			TILE_FLIPYX((attr & 0x60) >> 5));
	tileinfo->group = (attr & 0x0180) >> 7;

	// for out of range tiles, switch to fully transparent data
	// (but still call SET_TILE_INFO, otherwise problems might occur on boot e.g. unsquad)
	if (code == -1)
		tileinfo->pen_data = empty_tile8x8;
}

static TILE_GET_INFO( get_tile1_info )
{
	int code = cps1_scroll2[2*tile_index];
	int attr = cps1_scroll2[2*tile_index+1];

	code = gfxrom_bank_mapper(machine, GFXTYPE_SCROLL2, code);

	SET_TILE_INFO(
			2,
			code,
			(attr & 0x1f) + 0x40,
			TILE_FLIPYX((attr & 0x60) >> 5));
	tileinfo->group = (attr & 0x0180) >> 7;

	// for out of range tiles, switch to fully transparent data
	if (code == -1)
		tileinfo->pen_data = empty_tile;
}

static TILE_GET_INFO( get_tile2_info )
{
	int code = cps1_scroll3[2*tile_index] & 0x3fff;
	int attr = cps1_scroll3[2*tile_index+1];

	code = gfxrom_bank_mapper(machine, GFXTYPE_SCROLL3, code);

	SET_TILE_INFO(
			3,
			code,
			(attr & 0x1f) + 0x60,
			TILE_FLIPYX((attr & 0x60) >> 5));
	tileinfo->group = (attr & 0x0180) >> 7;

	// for out of range tiles, switch to fully transparent data
	// (but still call SET_TILE_INFO, otherwise problems might occur on boot e.g. unsquad)
	if (code == -1)
		tileinfo->pen_data = empty_tile;
}



static void cps1_update_transmasks(void)
{
	int i;

	for (i = 0;i < 4;i++)
	{
		int mask;

		/* Get transparency registers */
		if (cps1_game_config->priority[i] != -1)
			mask = cps1_cps_b_regs[cps1_game_config->priority[i]/2] ^ 0xffff;
		else mask = 0xffff;	/* completely transparent if priority masks not defined (qad) */

		tilemap_set_transmask(cps1_bg_tilemap[0],i,mask,0x8000);
		tilemap_set_transmask(cps1_bg_tilemap[1],i,mask,0x8000);
		tilemap_set_transmask(cps1_bg_tilemap[2],i,mask,0x8000);
	}
}

static VIDEO_START( cps )
{
	int i;

	MACHINE_RESET_CALL(cps);

	cps1_bg_tilemap[0] = tilemap_create(machine, get_tile0_info,tilemap0_scan, 8, 8,64,64);
	cps1_bg_tilemap[1] = tilemap_create(machine, get_tile1_info,tilemap1_scan,16,16,64,64);
	cps1_bg_tilemap[2] = tilemap_create(machine, get_tile2_info,tilemap2_scan,32,32,64,64);

	/* front masks will change at runtime to handle sprite occluding */
	cps1_update_transmasks();

	memset(empty_tile8x8,0x0f,sizeof(empty_tile8x8));
	memset(empty_tile,0xff,sizeof(empty_tile));	// 16x16 and 32x32 use packed graphics, 8x8 does not

	for (i = 0;i < cps1_palette_entries*16;i++)
	{
		palette_set_color(machine,i,MAKE_RGB(0,0,0));
	}

    cps1_buffered_obj = auto_alloc_array_clear(machine, UINT16, cps1_obj_size/2);

	if (cps_version==2)
	{
		cps2_buffered_obj = auto_alloc_array_clear(machine, UINT16, cps2_obj_size/2);
	}

	memset(cps1_gfxram, 0, cps1_gfxram_size);   /* Clear GFX RAM */
	memset(cps1_cps_a_regs, 0, 0x40);   /* Clear CPS-A registers */
	memset(cps1_cps_b_regs, 0, 0x40);   /* Clear CPS-B registers */

	if (cps_version == 2)
	{
		memset(cps2_objram1, 0, cps2_obj_size);
		memset(cps2_objram2, 0, cps2_obj_size);
	}

	/* Put in some defaults */
	cps1_cps_a_regs[CPS1_OBJ_BASE]     = 0x9200;
	cps1_cps_a_regs[CPS1_SCROLL1_BASE] = 0x9000;
	cps1_cps_a_regs[CPS1_SCROLL2_BASE] = 0x9040;
	cps1_cps_a_regs[CPS1_SCROLL3_BASE] = 0x9080;
	cps1_cps_a_regs[CPS1_OTHER_BASE]   = 0x9100;

	assert_always(cps1_game_config, "cps1_game_config hasn't been set up yet");

	/* Set up old base */
	cps1_get_video_base();   /* Calculate base pointers */
	cps1_get_video_base();   /* Calculate old base pointers */
}

VIDEO_START( cps1 )
{
	cps_version=1;
	VIDEO_START_CALL(cps);
}

VIDEO_START( cps2 )
{
	cps_version=2;
	VIDEO_START_CALL(cps);
}

/***************************************************************************

  Build palette from palette RAM

  12 bit RGB with a 4 bit brightness value.

***************************************************************************/

static void cps1_build_palette(running_machine *machine, const UINT16* const palette_base)
{
	int offset, page;
	const UINT16 *palette_ram = palette_base;

	int ctrl = cps1_cps_b_regs[cps1_game_config->palette_control/2];

	/*
    The palette is copied only for pages that are enabled in the ctrl
    register. Note that if the first palette pages are skipped, all
    the following pages are scaled down.
    */
	for (page = 0; page < 6; ++page)
	{
		if (BIT(ctrl,page))
		{
			for (offset = 0; offset < 0x200; ++offset)
			{
				int palette = *(palette_ram++);
				int r, g, b, bright;

				// from my understanding of the schematics, when the 'brightness'
				// component is set to 0 it should reduce brightness to 1/3

				bright = 0x0f + ((palette>>12)<<1);

				r = ((palette>>8)&0x0f) * 0x11 * bright / 0x2d;
				g = ((palette>>4)&0x0f) * 0x11 * bright / 0x2d;
				b = ((palette>>0)&0x0f) * 0x11 * bright / 0x2d;

				palette_set_color (machine, 0x200*page + offset, MAKE_RGB(r, g, b));
			}
		}
		else
		{
			// skip page in gfxram, but only if we have already copied at least one page
			if (palette_ram != palette_base)
				palette_ram += 0x200;
		}
	}
}



/***************************************************************************

                                Sprites
                                =======

  Sprites are represented by a number of 8 byte values

  xx xx yy yy nn nn aa aa

  where xxxx = x position
        yyyy = y position
        nnnn = tile number
        aaaa = attribute word
                    0x0001  colour
                    0x0002  colour
                    0x0004  colour
                    0x0008  colour
                    0x0010  colour
                    0x0020  X Flip
                    0x0040  Y Flip
                    0x0080  X & Y offset toggle (used in Marvel vs. Capcom.)
                    0x0100  X block size (in sprites)
                    0x0200  X block size
                    0x0400  X block size
                    0x0800  X block size
                    0x1000  Y block size (in sprites)
                    0x2000  Y block size
                    0x4000  Y block size
                    0x8000  Y block size

  The end of the table (may) be marked by an attribute value of 0xff00.

***************************************************************************/

static void cps1_find_last_sprite(void)    /* Find the offset of last sprite */
{
    int offset=0;
	/* Locate the end of table marker */
    while (offset < cps1_obj_size/2)
	{
        int colour=cps1_buffered_obj[offset+3];
		if ((colour & 0xff00) == 0xff00)
		{
			/* Marker found. This is the last sprite. */
            cps1_last_sprite_offset=offset-4;
			return;
		}
        offset+=4;
	}
	/* Sprites must use full sprite RAM */
    cps1_last_sprite_offset=cps1_obj_size/2-4;
}


static void cps1_render_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
#define DRAWSPRITE(CODE,COLOR,FLIPX,FLIPY,SX,SY)					\
{																	\
	if (flip_screen_get(machine))											\
		pdrawgfx_transpen(bitmap,\
				cliprect,machine->gfx[2],							\
				CODE,												\
				COLOR,												\
				!(FLIPX),!(FLIPY),									\
				511-16-(SX),255-16-(SY),							machine->priority_bitmap,0x02,15);					\
	else															\
		pdrawgfx_transpen(bitmap,\
				cliprect,machine->gfx[2],							\
				CODE,												\
				COLOR,												\
				FLIPX,FLIPY,										\
				SX,SY,												machine->priority_bitmap,0x02,15);					\
}


	int i, baseadd;
	UINT16 *base=cps1_buffered_obj;

	/* some sf2 hacks draw the sprites in reverse order */
	if (cps1_game_config->bootleg_kludge == 1)
	{
		base += cps1_last_sprite_offset;
		baseadd = -4;
	}
	else
	{
		baseadd = 4;
	}

	for (i=cps1_last_sprite_offset; i>=0; i-=4)
	{
		int x=*(base+0);
		int y=*(base+1);
		int code  =*(base+2);
		int colour=*(base+3);
		int col=colour&0x1f;

//      x-=0x20;
//      y+=0x20;

		code = gfxrom_bank_mapper(machine, GFXTYPE_SPRITES, code);

		if (code != -1)
		{
			if (colour & 0xff00 )
			{
				/* handle blocked sprites */
				int nx=(colour & 0x0f00) >> 8;
				int ny=(colour & 0xf000) >> 12;
				int nxs,nys,sx,sy;
				nx++;
				ny++;

				if (colour & 0x40)
				{
					/* Y flip */
					if (colour &0x20)
					{
						for (nys=0; nys<ny; nys++)
						{
							for (nxs=0; nxs<nx; nxs++)
							{
								sx = (x+nxs*16) & 0x1ff;
								sy = (y+nys*16) & 0x1ff;

								DRAWSPRITE(
//                                      code+(nx-1)-nxs+0x10*(ny-1-nys),
										(code & ~0xf) + ((code + (nx-1) - nxs) & 0xf) + 0x10*(ny-1-nys),
										(col&0x1f),
										1,1,
										sx,sy);
							}
						}
					}
					else
					{
						for (nys=0; nys<ny; nys++)
						{
							for (nxs=0; nxs<nx; nxs++)
							{
								sx = (x+nxs*16) & 0x1ff;
								sy = (y+nys*16) & 0x1ff;

								DRAWSPRITE(
//                                      code+nxs+0x10*(ny-1-nys),
										(code & ~0xf) + ((code + nxs) & 0xf) + 0x10*(ny-1-nys),
										(col&0x1f),
										0,1,
										sx,sy);
							}
						}
					}
				}
				else
				{
					if (colour &0x20)
					{
						for (nys=0; nys<ny; nys++)
						{
							for (nxs=0; nxs<nx; nxs++)
							{
								sx = (x+nxs*16) & 0x1ff;
								sy = (y+nys*16) & 0x1ff;

								DRAWSPRITE(
//                                      code+(nx-1)-nxs+0x10*nys,
										(code & ~0xf) + ((code + (nx-1) - nxs) & 0xf) + 0x10*nys,
										(col&0x1f),
										1,0,
										sx,sy);
							}
						}
					}
					else
					{
						for (nys=0; nys<ny; nys++)
						{
							for (nxs=0; nxs<nx; nxs++)
							{
								sx = (x+nxs*16) & 0x1ff;
								sy = (y+nys*16) & 0x1ff;

								DRAWSPRITE(
//                                      code+nxs+0x10*nys,
										(code & ~0xf) + ((code + nxs) & 0xf) + 0x10*nys,	// fix 00406: qadj: When playing as the ninja, there is one broekn frame in his animation loop when walking.
										(col&0x1f),
										0,0,
										sx,sy);
							}
						}
					}
				}
			}
			else
			{
				/* Simple case... 1 sprite */
						DRAWSPRITE(
						code,
						(col&0x1f),
						colour&0x20,colour&0x40,
						x & 0x1ff,y & 0x1ff);
			}
		}
		base += baseadd;
	}
#undef DRAWSPRITE
}




WRITE16_HANDLER( cps2_objram_bank_w )
{
	if (ACCESSING_BITS_0_7)
	{
		cps2_objram_bank = data & 1;
	}
}

READ16_HANDLER( cps2_objram1_r )
{
	if (cps2_objram_bank & 1)
		return cps2_objram2[offset];
	else
		return cps2_objram1[offset];
}

READ16_HANDLER( cps2_objram2_r )
{
	if (cps2_objram_bank & 1)
		return cps2_objram1[offset];
	else
		return cps2_objram2[offset];
}

WRITE16_HANDLER( cps2_objram1_w )
{
	if (cps2_objram_bank & 1)
		COMBINE_DATA(&cps2_objram2[offset]);
	else
		COMBINE_DATA(&cps2_objram1[offset]);
}

WRITE16_HANDLER( cps2_objram2_w )
{
	if (cps2_objram_bank & 1)
		COMBINE_DATA(&cps2_objram1[offset]);
	else
		COMBINE_DATA(&cps2_objram2[offset]);
}

static UINT16 *cps2_objbase(void)
{
	int baseptr;
	baseptr = 0x7000;

	if (cps2_objram_bank & 1) baseptr ^= 0x0080;

//popmessage("%04x %d",cps2_port(CPS2_OBJ_BASE),cps2_objram_bank&1);

	if (baseptr == 0x7000)
		return cps2_objram1;
	else //if (baseptr == 0x7080)
		return cps2_objram2;
}


static void cps2_find_last_sprite(void)    /* Find the offset of last sprite */
{
	int offset=0;
	UINT16 *base=cps2_buffered_obj;

	/* Locate the end of table marker */
	while (offset < cps2_obj_size/2)
	{
		if (base[offset+1]>=0x8000
				|| base[offset+3]>=0xff00)
		{
			/* Marker found. This is the last sprite. */
			cps2_last_sprite_offset=offset-4;
			return;
		}

		offset+=4;
	}
	/* Sprites must use full sprite RAM */
	cps2_last_sprite_offset=cps2_obj_size/2-4;
#undef DRAWSPRITE
}

static void cps2_render_sprites(running_machine *machine, bitmap_t *bitmap,const rectangle *cliprect,int *primasks)
{
#define DRAWSPRITE(CODE,COLOR,FLIPX,FLIPY,SX,SY)									\
{																					\
	if (flip_screen_get(machine))															\
		pdrawgfx_transpen(bitmap,\
				cliprect,machine->gfx[2],											\
				CODE,																\
				COLOR,																\
				!(FLIPX),!(FLIPY),													\
				511-16-(SX),255-16-(SY),											machine->priority_bitmap,primasks[priority],15);					\
	else																			\
		pdrawgfx_transpen(bitmap,\
				cliprect,machine->gfx[2],											\
				CODE,																\
				COLOR,																\
				FLIPX,FLIPY,														\
				SX,SY,																machine->priority_bitmap,primasks[priority],15);					\
}

	int i;
	UINT16 *base=cps2_buffered_obj;
	int xoffs = 64-cps2_port(CPS2_OBJ_XOFFS);
	int yoffs = 16-cps2_port(CPS2_OBJ_YOFFS);

#ifdef MAME_DEBUG
	if (input_code_pressed(machine, KEYCODE_Z) && input_code_pressed(machine, KEYCODE_R))
	{
		return;
	}
#endif

	for (i=cps2_last_sprite_offset; i>=0; i-=4)
	{
		int x=base[i+0];
		int y=base[i+1];
		int priority=(x>>13)&0x07;
		int code  = base[i+2]+((y & 0x6000) <<3);
		int colour= base[i+3];
		int col=colour&0x1f;

		if(colour & 0x80)
		{
			x += cps2_port(CPS2_OBJ_XOFFS);  /* fix the offset of some games */
			y += cps2_port(CPS2_OBJ_YOFFS);  /* like Marvel vs. Capcom ending credits */
		}

		if (colour & 0xff00 )
		{
			/* handle blocked sprites */
			int nx=(colour & 0x0f00) >> 8;
			int ny=(colour & 0xf000) >> 12;
			int nxs,nys,sx,sy;
			nx++;
			ny++;

			if (colour & 0x40)
			{
				/* Y flip */
				if (colour &0x20)
				{
					for (nys=0; nys<ny; nys++)
					{
						for (nxs=0; nxs<nx; nxs++)
						{
							sx = (x+nxs*16+xoffs) & 0x3ff;
							sy = (y+nys*16+yoffs) & 0x3ff;
							DRAWSPRITE(
									code+(nx-1)-nxs+0x10*(ny-1-nys),
									(col&0x1f),
									1,1,
									sx,sy);
						}
					}
				}
				else
				{
					for (nys=0; nys<ny; nys++)
					{
						for (nxs=0; nxs<nx; nxs++)
						{
							sx = (x+nxs*16+xoffs) & 0x3ff;
							sy = (y+nys*16+yoffs) & 0x3ff;

							DRAWSPRITE(
									code+nxs+0x10*(ny-1-nys),
									(col&0x1f),
									0,1,
									sx,sy);
						}
					}
				}
			}
			else
			{
				if (colour &0x20)
				{
					for (nys=0; nys<ny; nys++)
					{
						for (nxs=0; nxs<nx; nxs++)
						{
							sx = (x+nxs*16+xoffs) & 0x3ff;
							sy = (y+nys*16+yoffs) & 0x3ff;

							DRAWSPRITE(
									code+(nx-1)-nxs+0x10*nys,
									(col&0x1f),
									1,0,
									sx,sy);
						}
					}
				}
				else
				{
					for (nys=0; nys<ny; nys++)
					{
						for (nxs=0; nxs<nx; nxs++)
						{
							sx = (x+nxs*16+xoffs) & 0x3ff;
							sy = (y+nys*16+yoffs) & 0x3ff;

							DRAWSPRITE(
//                                      code+nxs+0x10*nys,
									(code & ~0xf) + ((code + nxs) & 0xf) + 0x10*nys,	//  pgear fix
									(col&0x1f),
									0,0,
									sx,sy);
						}
					}
				}
			}
		}
		else
		{
			/* Simple case... 1 sprite */
			DRAWSPRITE(
					code,
					(col&0x1f),
					colour&0x20,colour&0x40,
					(x+xoffs) & 0x3ff,(y+yoffs) & 0x3ff);
		}
	}
}




static void cps1_render_stars(const device_config *screen, bitmap_t *bitmap,const rectangle *cliprect)
{
	int offs;
	UINT8 *stars_rom = memory_region(screen->machine, "stars");

	if (!stars_rom && (cps1_stars_enabled[0] || cps1_stars_enabled[1]))
	{
#ifdef MAME_DEBUG
//      popmessage("stars enabled but no stars ROM");
#endif
		return;
	}

	if (cps1_stars_enabled[0])
	{
		for (offs = 0;offs < stars_rom_size/2;offs++)
		{
			int col = stars_rom[8*offs+4];
			if (col != 0x0f)
			{
				int sx = (offs / 256) * 32;
				int sy = (offs % 256);
				sx = (sx - stars2x + (col & 0x1f)) & 0x1ff;
				sy = (sy - stars2y) & 0xff;
				if (flip_screen_get(screen->machine))
				{
					sx = 511 - sx;
					sy = 255 - sy;
				}

				col = ((col & 0xe0) >> 1) + (video_screen_get_frame_number(screen)/16 & 0x0f);

				if (sx >= cliprect->min_x && sx <= cliprect->max_x &&
					sy >= cliprect->min_y && sy <= cliprect->max_y)
					*BITMAP_ADDR16(bitmap, sy, sx) = 0xa00 + col;
			}
		}
	}

	if (cps1_stars_enabled[1])
	{
		for (offs = 0;offs < stars_rom_size/2;offs++)
		{
			int col = stars_rom[8*offs];
			if (col != 0x0f)
			{
				int sx = (offs / 256) * 32;
				int sy = (offs % 256);
				sx = (sx - stars1x + (col & 0x1f)) & 0x1ff;
				sy = (sy - stars1y) & 0xff;
				if (flip_screen_get(screen->machine))
				{
					sx = 511 - sx;
					sy = 255 - sy;
				}

				col = ((col & 0xe0) >> 1) + (video_screen_get_frame_number(screen)/16 & 0x0f);

				if (sx >= cliprect->min_x && sx <= cliprect->max_x &&
					sy >= cliprect->min_y && sy <= cliprect->max_y)
					*BITMAP_ADDR16(bitmap, sy, sx) = 0x800 + col;
			}
		}
	}
}


static void cps1_render_layer(running_machine *machine,bitmap_t *bitmap,const rectangle *cliprect,int layer,int primask)
{
	switch (layer)
	{
		case 0:
			cps1_render_sprites(machine,bitmap,cliprect);
			break;
		case 1:
		case 2:
		case 3:
			tilemap_draw(bitmap,cliprect,cps1_bg_tilemap[layer-1],TILEMAP_DRAW_LAYER1,primask);
			break;
	}
}

static void cps1_render_high_layer(bitmap_t *bitmap, const rectangle *cliprect, int layer)
{
	switch (layer)
	{
		case 0:
			/* there are no high priority sprites */
			break;
		case 1:
		case 2:
		case 3:
			tilemap_draw(NULL,cliprect,cps1_bg_tilemap[layer-1],TILEMAP_DRAW_LAYER0,1);
			break;
	}
}


/***************************************************************************

    Refresh screen

***************************************************************************/

VIDEO_UPDATE( cps1 )
{
	int layercontrol,l0,l1,l2,l3;
	int videocontrol = cps1_cps_a_regs[CPS1_VIDEOCONTROL];


	flip_screen_set(screen->machine, videocontrol & 0x8000);

	layercontrol = cps1_cps_b_regs[cps1_game_config->layer_control/2];

	/* Get video memory base registers */
	cps1_get_video_base();

	/* Find the offset of the last sprite in the sprite table */
	cps1_find_last_sprite();
	if (cps_version == 2)
	{
		cps2_find_last_sprite();
	}

	cps1_update_transmasks();

	tilemap_set_scrollx(cps1_bg_tilemap[0],0,cps1_scroll1x);
	tilemap_set_scrolly(cps1_bg_tilemap[0],0,cps1_scroll1y);
	if (videocontrol & 0x01)	/* linescroll enable */
	{
		int scrly=-cps1_scroll2y;
		int i;
		int otheroffs;

		tilemap_set_scroll_rows(cps1_bg_tilemap[1],1024);

		otheroffs = cps1_cps_a_regs[CPS1_ROWSCROLL_OFFS];

		for (i = 0;i < 256;i++)
			tilemap_set_scrollx(cps1_bg_tilemap[1],(i - scrly) & 0x3ff,cps1_scroll2x + cps1_other[(i + otheroffs) & 0x3ff]);
	}
	else
	{
		tilemap_set_scroll_rows(cps1_bg_tilemap[1],1);
		tilemap_set_scrollx(cps1_bg_tilemap[1],0,cps1_scroll2x);
	}
	tilemap_set_scrolly(cps1_bg_tilemap[1],0,cps1_scroll2y);
	tilemap_set_scrollx(cps1_bg_tilemap[2],0,cps1_scroll3x);
	tilemap_set_scrolly(cps1_bg_tilemap[2],0,cps1_scroll3y);


	/* Blank screen */
	if (cps_version == 1)
	{
		// CPS1 games use pen 0xbff as background color; this is used in 3wonders,
		// mtwins (explosion during attract), mercs (intermission).
		bitmap_fill(bitmap,cliprect,0xbff);
	}
	else
	{
		// CPS2 apparently always force the background to black. Several games would
		// should a blue screen during boot if we used the same code as CPS1.
		// Maybe Capcom changed the background handling due to the problems that
		// it caused on several monitors (because the background extended into the
		// blanking area instead of going black, causing the monitor to clip).
		bitmap_fill(bitmap,cliprect,get_black_pen(screen->machine));
	}

	cps1_render_stars(screen, bitmap,cliprect);

	/* Draw layers (0 = sprites, 1-3 = tilemaps) */
	l0 = (layercontrol >> 0x06) & 03;
	l1 = (layercontrol >> 0x08) & 03;
	l2 = (layercontrol >> 0x0a) & 03;
	l3 = (layercontrol >> 0x0c) & 03;
	bitmap_fill(screen->machine->priority_bitmap,cliprect,0);

	if (cps_version == 1)
	{
		cps1_render_layer(screen->machine,bitmap,cliprect,l0,0);
		if (l1 == 0) cps1_render_high_layer(bitmap,cliprect,l0); /* prepare mask for sprites */
		cps1_render_layer(screen->machine,bitmap,cliprect,l1,0);
		if (l2 == 0) cps1_render_high_layer(bitmap,cliprect,l1); /* prepare mask for sprites */
		cps1_render_layer(screen->machine,bitmap,cliprect,l2,0);
		if (l3 == 0) cps1_render_high_layer(bitmap,cliprect,l2); /* prepare mask for sprites */
		cps1_render_layer(screen->machine,bitmap,cliprect,l3,0);
	}
	else
	{
		int l0pri,l1pri,l2pri,l3pri;
		int primasks[8],i;
		l0pri = (pri_ctrl >> 4*l0) & 0x0f;
		l1pri = (pri_ctrl >> 4*l1) & 0x0f;
		l2pri = (pri_ctrl >> 4*l2) & 0x0f;
		l3pri = (pri_ctrl >> 4*l3) & 0x0f;

#if 0
if (	(cps2_port(CPS2_OBJ_BASE) != 0x7080 && cps2_port(CPS2_OBJ_BASE) != 0x7000) ||
		cps2_port(CPS2_OBJ_UK1) != 0x807d ||
		(cps2_port(CPS2_OBJ_UK2) != 0x0000 && cps2_port(CPS2_OBJ_UK2) != 0x1101 && cps2_port(CPS2_OBJ_UK2) != 0x0001) ||
	popmessage("base %04x uk1 %04x uk2 %04x",
			cps2_port(CPS2_OBJ_BASE),
			cps2_port(CPS2_OBJ_UK1),
			cps2_port(CPS2_OBJ_UK2));

if (0 && input_code_pressed(screen->machine, KEYCODE_Z))
	popmessage("order: %d (%d) %d (%d) %d (%d) %d (%d)",l0,l0pri,l1,l1pri,l2,l2pri,l3,l3pri);
#endif

		/* take out the CPS1 sprites layer */
		if (l0 == 0) { l0 = l1; l1 = 0; l0pri = l1pri; }
		if (l1 == 0) { l1 = l2; l2 = 0; l1pri = l2pri; }
		if (l2 == 0) { l2 = l3; l3 = 0; l2pri = l3pri; }

		{
			int mask0 = 0xaa;
			int mask1 = 0xcc;
			if(l0pri>l1pri) mask0 &= ~0x88;
			if(l0pri>l2pri) mask0 &= ~0xa0;
			if(l1pri>l2pri) mask1 &= ~0xc0;

			primasks[0] = 0xff;
			for (i = 1;i < 8;i++)
			{
				if (i <= l0pri && i <= l1pri && i <= l2pri)
				{
					primasks[i] = 0xfe;
					continue;
				}
				primasks[i] = 0;
				if (i <= l0pri) primasks[i] |= mask0;
				if (i <= l1pri) primasks[i] |= mask1;
				if (i <= l2pri) primasks[i] |= 0xf0;
			}
		}

		cps1_render_layer(screen->machine,bitmap,cliprect,l0,1);
		cps1_render_layer(screen->machine,bitmap,cliprect,l1,2);
		cps1_render_layer(screen->machine,bitmap,cliprect,l2,4);
		cps2_render_sprites(screen->machine,bitmap,cliprect,primasks);
	}

	return 0;
}

VIDEO_EOF( cps1 )
{
	/* Get video memory base registers */
	cps1_get_video_base();

	if (cps_version == 1)
	{
		/* CPS1 sprites have to be delayed one frame */
		memcpy(cps1_buffered_obj, cps1_obj, cps1_obj_size);
	}
}

void cps2_set_sprite_priorities(void)
{
	pri_ctrl = cps2_port(CPS2_OBJ_PRI);
}

void cps2_objram_latch(void)
{
	cps2_set_sprite_priorities();
	memcpy(cps2_buffered_obj, cps2_objbase(), cps2_obj_size);
}
