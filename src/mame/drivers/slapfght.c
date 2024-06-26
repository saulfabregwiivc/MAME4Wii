/***************************************************************************

Slap Fight driver by K.Wilkins Jan 1998

Slap Fight - Taito

The three drivers provided are identical, only the 1st CPU EPROM is different
which shows up in the boot message, one if Japanese domestic and the other
is English. The proms which MAY be the original slapfight ones currently
give a hardware error and fail to boot.

slapfigh - Arcade ROMs from Japan http://home.onestop.net/j_rom/
slapboot - Unknown source
slpboota - ROMS Dumped by KW 29/12/97 from unmarked Slap Fight board (bootleg?)


Slap Fight/Alcon
1986 Taito Corporation

Slap Fight and Alcon are the same PCBs exactly, with just 4 ROMs changed. The same MCU is
common to Slap Fight and Alcon.
Tiger Heli is almost exactly the same PCB but a few chips have different placings and there's
more EPROM locations on one of the boards (although they're not populated).
Also, Tiger Heli has a 22-way edge connector, while Alcon/Slap Fight is JAMMA.

PCB Layouts - Top Board
-----------------------

Alcon -        M6100186A
               860100714

Slap Fight -   M6100179A
               860090333


8606S MADE IN JAPAN
|--------------------------------------------------|
|VOL ROM21.12Q                 2148                |
|MB3730                        2148                |
|    ROM19.12P                 2148                |
|                              2148                |
|    ROM20.12M                         6264        |
|                                                  |
|                                      6264        |
|                                                  |
|J                 A77_12.8J A77_11.6J 6264        |
|A    AY3-8910                                     |
|M                 A77_10.8H A77_09.6H 6264        |
|M    DSW1  DSW2                                   |
|A                                       2148      |
|     AY3-8910                                     |
|                                          ROM16.1E|
|                                                  |
|  A77_02.12D                              ROM17.1C|
|                                                  |
|                                     ROM18.2B     |
|  Z80A   2016                                     |
|                                                  |
|-----|----------|--------------|----------|-------|
      |----------|              |----------|
Notes:
      AY3-8910 clock - 1.500MHz (36/24)
      Z80A clock - 3.000MHz (36/12)
      VSync - 57Hz
      HSync - 14.97kHz
      A77_02 - 2764 EPROM
      A77_09 to A77_12 - 27256 EPROM
      ROM18 - 18S030 PROM
      ROM16, ROM17, ROM19, ROM20, ROM21 - 82S129 PROM


Bottom Board
------------

8606M MADE IN JAPAN
|--------------------------------------------------|
|                        A77_13.6A                 |
|                                                  |
|    ROM14.2C                          ROM15.8B    |
|                                                  |
|                                                  |
|              2016      A77_04.6F                 |
|                                           36MHz  |
|              2016      A77_03.6G                 |
|                                                  |
|                                                  |
|                                                  |
|                                                  |
|                                                  |
|                        A77_08.6K                 |
|                                                  |
|                        A77_07.6M         2016    |
|     2016                                         |
|              2016      A77_06.6N       A77_01.8N |
|                                                  |
|              2016      A77_05.6P       A77_00.8P |
|                                           Z80B   |
|-----|----------|--------------|----------|-------|
      |----------|              |----------|
Notes:
      Z80B clock - 6.000MHz (36/6)
      ROM14, ROM15 - 82S129 PROM
      A77_00, A77_01 - 27256 EPROM (replace with A77_00-1, A77_01-1 on Alcon)
      A77_05 to A77_08 - 27256 EPROM
      A77_03, A77_04 - 2764 EPROM (replace with A77_03-1, A77_04-1 on Alcon)
      A77_13 - Motorola MC68705P5S Micro-Controller (protected). Clock 3.000MHz


PCB Details from slpboota boardset:

Upper PCB (Sound board)
---------
Z80A CPU
Toshiba TMM2016BP-10 (2KB SRAM)
sf_s05 (Fujitsu MBM2764-25 8KB EPROM) - Sound CPU Code

Yamaha YM2149F (Qty 2 - Pin compatible with AY-3-8190)
Hitachi SRAM - HM6464 (8KB - Qty 4)

sf_s01 (OKI M27256-N 32KB PROM)              Sprite Data (16x16 4bpp)
sf_s02 (OKI M27256-N 32KB PROM)              Sprite Data
sf_s03 (OKI M27256-N 32KB PROM)              Sprite Data
sf_s04 (OKI M27256-N 32KB PROM)              Sprite Data


Lower PCB
---------
Z80B CPU
12MHz Xtal
Toshiba TMM2016BP-10 (2KB SRAM - Total Qty 6 = 2+2+1+1)

sf_s10 (Fujitsu MBM2764-25 8KB EPROM)        Font/Character Data (8x8 2bpp)
sf_s11 (Fujitsu MBM2764-25 8KB EPROM)

sf_s06 (OKI M27256-N 32KB PROM)              Tile Data (8x8 4bpp)
sf_s07 (OKI M27256-N 32KB PROM)              Tile Data
sf_s08 (OKI M27256-N 32KB PROM)              Tile Data
sf_s09 (OKI M27256-N 32KB PROM)              Tile Data

sf_s16 (Fujitsu MBM2764-25 8KB EPROM)        Colour Tables (512B used?)

sf_sH  (OKI M27256-N 32KB PROM)              Level Maps ???

sf_s19 (NEC S27128 16KB EPROM)               CPU Code $0000-$3fff
sf_s20 (Mitsubishi M5L27128K 16KB EPROM)     CPU Code $4000-$7fff


Main CPU Memory Map
-------------------

$0000-$3fff    ROM (SF_S19)
$4000-$7fff    ROM (SF_S20)
$8000-$bfff    ROM (SF_SH) - This is a 32K ROM - Paged ????? How ????

$c000-$c7ff    2K RAM
$c800-$cfff    READ:Unknown H/W  WRITE:Unknown H/W (Upper PCB)
$d000-$d7ff    Background RAM1
$d800-$dfff    Background RAM2
$e000-$e7ff    Sprite RAM
$e800-$efff    READ:Unknown H/W  WRITE:Unknown H/W
$f000-$f7ff    READ:SF_S16       WRITE:Character RAM
$f800-$ffff    READ:Unknown H/W  WRITE:Attribute RAM

$c800-$cfff    Appears to be RAM BUT 1st 0x10 bytes are swapped with
               the sound CPU and visversa for READ OPERATIONS


Write I/O MAP
-------------
Addr    Address based write                     Data based write

$00     Reset sound CPU
$01     Clear sound CPU reset
$02
$03
$04
$05
$06     Clear/Disable Hardware interrupt
$07     Enable Hardware interrupt
$08     LOW Bank select for SF_SH               X axis character scroll reg
$09     HIGH Bank select for SF_SH              X axis pixel scroll reg
$0a
$0b
$0c     Select 1st set of sprites colors
$0d     Select 2nd set of sprites colors
$0e
$0f

Read I/O Map
------------

$00     Status regsiter - cycle 0xc7, 0x55, 0x00  (Thanks to Dave Spicer for the info)


Known Info
----------

2K Character RAM at write only address $f000-$f7fff looks to be organised
64x32 chars with the screen rotated thru 90 degrees clockwise. There
appears to be some kind of attribute(?) RAM above at $f800-$ffff organised
in the same manner.

From the look of data in the buffer it is arranged thus: 37x32 (HxW) which
would make the overall frame buffer 296x256.

Print function maybe around $09a2 based on info from log file.

$e000 looks like sprite ram, setup routines at $0008.


Sound System CPU Details
------------------------

Memory Map
$0000-$1fff  ROM(SF_S5)
$a080        AY-3-8910(PSG1) Register address
$a081        AY-3-8910(PSG1) Read register
$a082        AY-3-8910(PSG1) Write register
$a090        AY-3-8910(PSG2) Register address
$a091        AY-3-8910(PSG2) Read register
$a092        AY-3-8910(PSG2) Write register
$c800-$cfff  RAM(2K)

Strangely the RAM hardware registers seem to be overlaid at $c800
$00a6 routine here reads I/O ports and stores in, its not a straight
copy, the data is mangled before storage:
PSG1-E -> $c808
PSG1-F -> $c80b
PSG2-E -> $c809
PSG2-F -> $c80a - DIP Switch Bank 2 (Test mode is here)

-------------------------------GET STAR------------------------------------
        following info by Luca Elia (l.elia@tin.it)

                Interesting locations
                ---------------------

c803    credits
c806    used as a watchdog: main cpu reads then writes FF.
    If FF was read, jp 0000h. Sound cpu zeroes it.

c807(1p)    left    7           c809    DSW1(cpl'd)
c808(2p)    down    6           c80a    DSW2(cpl'd)
active_H    right   5           c80b    ip 1(cpl'd)
        up  4
        0   3
        0   2
        but2    1
        but1    0

c21d(main)  1p lives

Main cpu writes to unmapped ports 0e,0f,05,03 at startup.
Before playing, f1 is written to e802 and 00 to port 03.
If flip screen dsw is on, ff is written to e802 an 00 to port 02, instead.

                Interesting routines (main cpu)
                -------------------------------
4a3 wait A irq's
432 init the Ath sprite
569 reads a sequence from e803
607 prints the Ath string (FF terminated). String info is stored at
    65bc in the form of: attribute, dest. address, string address (5 bytes)
b73 checks lives. If zero, writes 0 to port 04 then jp 0000h.
    Before that, sets I to FF as a flag, for the startup ram check
    routine, to not alter the credit counter.
1523    put name in hi-scores?

-------------------------------Performan-----------------------------------
                 Interesting RAM locations (Main CPU).
                 -------------------------------------

$8056            Hero counter
$8057            Level counter
$8006 - $8035    High score table
$8609 - $860f    High score characters to display to screen for highest score


****************************************************************************

Stephh's notes (based on the games Z80 code and some tests) :

0) all games

  - All these games have a specific way of handling inputs :
      * when cabinet is set to "Upright", any player can use inputs
        from player 1 OR from player 2
      * when cabinet is set to "Cocktail", each player has its own inputs
    See Mantis bug ID 01746 attached files with Z80 explanations


1) 'tigerh' and clones

1a) 'tigerh'

  - US version
  - MCU dumped and emulated
  - Game inputs aren't working after heliport 4 (area 35);
    I can't tell at the moment if it's an ingame bug or not.
    See Mantis bug ID 01785 for more details.

1b) 'tigerhj'

  - Japan version
  - MCU dumped and emulated
  - Game inputs aren't working after heliport 4 (area 35);
    I can't tell at the moment if it's an ingame bug or not.
    See Mantis bug ID 01785 for more details.

1c) 'tigerhb1'

  - Bootleg based on 'tigerhj'
  - I can't tell if a specific MCU is required for this game :
      * MCU status doesn't seem to be required
        (see nop instructions at 0x10da, 0x10db, 0x10e5 and 0x10e6)
      * there is patched code at 0x0f9c which is called instead of
        standard call to 0x10cc, and there is no call to 0x0f88
      * there are writes to unknown ports 0x0e and 0x0f
      * hardware test is still computed at 0x109d but there is
        no delay to wait for correct value (0x83) to be returned
        (thus the need of specific read/write handlers)
  - Game inputs aren't working after heliport 4 (area 35);
    I can't tell at the moment if it's an ingame bug or not.
    See Mantis bug ID 01785 for more details.

1d) 'tigerhb2'

  - Bootleg based on 'tigerhj'
  - MCU reads/writes are patched.as well as ROM check
  - Game inputs are still working after heliport 4 (area 35);
    This is the ONLY set with this behaviour !
    See Mantis bug ID 01785 for more details.

1e) 'tigerhb3'

  - Bootleg based on 'tigerhj'
  - MCU reads/writes are patched.
  - There is extra code at 0x10e2 (where MCU status was checked)
    that prevents player to get any extra lives (even by collecting
    10 diamonds) when Dip Switch is set to "20k then every 80k"
    (see Mantis bug ID 00652 with Z80 explanations).
  - Game inputs aren't working after heliport 4 (area 35);
    I can't tell at the moment if it's an ingame bug or not.
    See Mantis bug ID 01785 for more details.


2) 'getstar' and clones

2a) 'getstar'

  - US version, licenced to Kitkorp - name "Guardian".
  - MCU missing and simulated.
  - Difficulty determines the number of energy bars you get.
  - Each hit removes 1 enegy bar.
  - According to the manual, default difficulty shall be set to "Hard".
  - You can only get ONE extra life.
  - I don't know if it's an ingame bug, but you always have to enter
    your initials after a game ends, even if you score less than 10th place.

2b) 'getstarj'

  - Japan version - name "Get Star".
  - MCU missing and simulated.
  - You always get 4 energy bars.
  - Difficulty determines how many energy bars you lose when you get hit.
  - I don't know what default difficulty shall be, so I set it to "Easy".
  - You can get multiple extra lives.

2c) 'gtstarb1'

  - Bootleg based on 'getstarj'
  - MCU reads/writes are patched, but this hasn't been done in the "test mode".
  - The game seems to have its own protection on startup (check code at 0x6d1a).

2d) 'gtstarb2'

  - Bootleg based on 'getstarj'
  - MCU reads/writes are patched, but this hasn't been done in the "test mode".
  - The game seems to have its own protection (check code at 0x0569 and 0x0ac6).
  - Patches are coded diffrently than in 'gtstarb1' and code isn't always perfect
    (lazy coding ?) which causes LOTS of ingames bugs or strange behaviours :
      * patched command 0x20 : NO continue play
      * patched command 0x21 : as soon as a player loses all his lives,
        the game is over (even if other player still has lives)
      * patched command 0x22 : difficulty is ALWAYS set to "Easy" at start
        so "Difficulty" Dip Switches have no effect when you are playing
        (it still has an effect in the "test mode" though)
      * patched command 0x23 : different lives settings
        (thus the need of a specific INPUT_PORTS definition for this game)
      * patched command 0x25 : right and down directions are swapped
        (thus the need of a specific INPUT_PORTS definition for this game)
      * patched command 0x2a : player flag (which determines if player 1 or
        player 2 is playing) is NOT updated, causing the following things :
          . current player score will ALWAYS be dusplayed under player 1
          . when cabinet is set to "Cocktail", player 2 uses player 1 inputs
      * patched command 0x38 : laser position does NOT change according to
        player position (it ALWATS starts from the middle of the screen)

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "deprecat.h"
#include "cpu/m6805/m6805.h"
#include "sound/ay8910.h"
#include "includes/slapfght.h"

int getstar_id;

#if 0
static ADDRESS_MAP_START( perfrman_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0x8800, 0x880f) AM_RAM AM_SHARE(1)
	AM_RANGE(0x8810, 0x8fff) AM_RAMBANK(1) /* Shared RAM with sound CPU */
	AM_RANGE(0x9000, 0x97ff) AM_RAM_WRITE(slapfight_videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x9800, 0x9fff) AM_RAM_WRITE(slapfight_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xa000, 0xa7ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
ADDRESS_MAP_END
#endif

static ADDRESS_MAP_START( tigerh_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
	AM_RANGE(0xc800, 0xc80f) AM_RAM AM_SHARE(1)
	AM_RANGE(0xc810, 0xcfff) AM_RAM
	AM_RANGE(0xd000, 0xd7ff) AM_RAM_WRITE(slapfight_videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0xd800, 0xdfff) AM_RAM_WRITE(slapfight_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xe800, 0xe800) AM_WRITEONLY AM_BASE(&slapfight_scrollx_lo)
	AM_RANGE(0xe801, 0xe801) AM_WRITEONLY AM_BASE(&slapfight_scrollx_hi)
	AM_RANGE(0xe802, 0xe802) AM_WRITEONLY AM_BASE(&slapfight_scrolly)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(slapfight_fixram_w) AM_BASE(&slapfight_videoram) AM_SIZE(&slapfight_videoram_size)
	AM_RANGE(0xf800, 0xffff) AM_RAM_WRITE(slapfight_fixcol_w) AM_BASE(&slapfight_colorram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( slapfght_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
	AM_RANGE(0xc800, 0xc80f) AM_RAM AM_SHARE(1)
	AM_RANGE(0xc810, 0xcfff) AM_RAM
	AM_RANGE(0xd000, 0xd7ff) AM_RAM_WRITE(slapfight_videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0xd800, 0xdfff) AM_RAM_WRITE(slapfight_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xe800, 0xe800) AM_WRITEONLY AM_BASE(&slapfight_scrollx_lo)
	AM_RANGE(0xe801, 0xe801) AM_WRITEONLY AM_BASE(&slapfight_scrollx_hi)
	AM_RANGE(0xe802, 0xe802) AM_WRITEONLY AM_BASE(&slapfight_scrolly)
//  AM_RANGE(0xe803, 0xe803) AM_READ(mcu_r) // MCU lives here
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(slapfight_fixram_w) AM_BASE(&slapfight_videoram) AM_SIZE(&slapfight_videoram_size)
	AM_RANGE(0xf800, 0xffff) AM_RAM_WRITE(slapfight_fixcol_w) AM_BASE(&slapfight_colorram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( slapbtuk_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
	AM_RANGE(0xc800, 0xc80f) AM_RAM AM_SHARE(1)
	AM_RANGE(0xc810, 0xcfff) AM_RAM
	AM_RANGE(0xd000, 0xd7ff) AM_RAM_WRITE(slapfight_videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0xd800, 0xdfff) AM_RAM_WRITE(slapfight_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xe800, 0xe800) AM_WRITEONLY AM_BASE(&slapfight_scrollx_hi)
	AM_RANGE(0xe802, 0xe802) AM_WRITEONLY AM_BASE(&slapfight_scrolly)
	AM_RANGE(0xe803, 0xe803) AM_WRITEONLY AM_BASE(&slapfight_scrollx_lo)
//  AM_RANGE(0xe803, 0xe803) AM_READ(getstar_e803_r)
	AM_RANGE(0xec00, 0xefff) AM_ROM // it reads a copy of the logo from here!
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(slapfight_fixram_w) AM_BASE(&slapfight_videoram) AM_SIZE(&slapfight_videoram_size)
	AM_RANGE(0xf800, 0xffff) AM_RAM_WRITE(slapfight_fixcol_w) AM_BASE(&slapfight_colorram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( slapfght_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READWRITE(slapfight_port_00_r, slapfight_port_00_w)	/* status register */
	AM_RANGE(0x01, 0x01) AM_WRITE(slapfight_port_01_w)
	AM_RANGE(0x02, 0x03) AM_WRITE(slapfight_flipscreen_w)
//  AM_RANGE(0x04, 0x04) AM_WRITE(getstar_port_04_w)
	AM_RANGE(0x06, 0x06) AM_WRITE(slapfight_port_06_w)
	AM_RANGE(0x07, 0x07) AM_WRITE(slapfight_port_07_w)
	AM_RANGE(0x08, 0x08) AM_WRITE(slapfight_port_08_w)	/* select bank 0 */
	AM_RANGE(0x09, 0x09) AM_WRITE(slapfight_port_09_w)	/* select bank 1 */
	AM_RANGE(0x0c, 0x0d) AM_WRITE(slapfight_palette_bank_w)
ADDRESS_MAP_END

static READ8_HANDLER(tigerh_status_r)
{
	return (slapfight_port_00_r(space,0)&0xf9)| ((tigerh_mcu_status_r(space,0)));
}

static ADDRESS_MAP_START( tigerh_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READWRITE(tigerh_status_r, slapfight_port_00_w)	/* status register */
	AM_RANGE(0x01, 0x01) AM_WRITE(slapfight_port_01_w)
	AM_RANGE(0x02, 0x03) AM_WRITE(slapfight_flipscreen_w)
	AM_RANGE(0x06, 0x06) AM_WRITE(slapfight_port_06_w)
	AM_RANGE(0x07, 0x07) AM_WRITE(slapfight_port_07_w)
ADDRESS_MAP_END

#if 0
static ADDRESS_MAP_START( tigerhb_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READWRITE(slapfight_port_00_r, slapfight_port_00_w)	/* status register */
	AM_RANGE(0x01, 0x01) AM_WRITE(slapfight_port_01_w)
	AM_RANGE(0x02, 0x03) AM_WRITE(slapfight_flipscreen_w)
	AM_RANGE(0x06, 0x06) AM_WRITE(slapfight_port_06_w)
	AM_RANGE(0x07, 0x07) AM_WRITE(slapfight_port_07_w)
ADDRESS_MAP_END
#endif

static ADDRESS_MAP_START( tigerh_m68705_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0x7ff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(tigerh_68705_portA_r,tigerh_68705_portA_w)
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(tigerh_68705_portB_r,tigerh_68705_portB_w)
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(tigerh_68705_portC_r,tigerh_68705_portC_w)
	AM_RANGE(0x0004, 0x0004) AM_WRITE(tigerh_68705_ddrA_w)
	AM_RANGE(0x0005, 0x0005) AM_WRITE(tigerh_68705_ddrB_w)
	AM_RANGE(0x0006, 0x0006) AM_WRITE(tigerh_68705_ddrC_w)
	AM_RANGE(0x0010, 0x007f) AM_RAM
	AM_RANGE(0x0080, 0x07ff) AM_ROM
ADDRESS_MAP_END

#if 0
static ADDRESS_MAP_START( perfrman_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x8800, 0x880f) AM_RAM AM_SHARE(1)
	AM_RANGE(0x8810, 0x8fff) AM_RAMBANK(1) /* Shared RAM with main CPU */
	AM_RANGE(0xa080, 0xa080) AM_DEVWRITE("ay1", ay8910_address_w)
	AM_RANGE(0xa081, 0xa081) AM_DEVREAD("ay1", ay8910_r)
	AM_RANGE(0xa082, 0xa082) AM_DEVWRITE("ay1", ay8910_data_w)
	AM_RANGE(0xa090, 0xa090) AM_DEVWRITE("ay2", ay8910_address_w)
	AM_RANGE(0xa091, 0xa091) AM_DEVREAD("ay2", ay8910_r)
	AM_RANGE(0xa092, 0xa092) AM_DEVWRITE("ay2", ay8910_data_w)
	AM_RANGE(0xa0e0, 0xa0e0) AM_WRITE(getstar_sh_intenable_w) /* maybe a0f0 also -LE */
//  AM_RANGE(0xa0f0, 0xa0f0) AM_WRITENOP
ADDRESS_MAP_END
#endif

static ADDRESS_MAP_START( slapfght_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0xa080, 0xa080) AM_DEVWRITE("ay1", ay8910_address_w)
	AM_RANGE(0xa081, 0xa081) AM_DEVREAD("ay1", ay8910_r)
	AM_RANGE(0xa082, 0xa082) AM_DEVWRITE("ay1", ay8910_data_w)
	AM_RANGE(0xa090, 0xa090) AM_DEVWRITE("ay2", ay8910_address_w)
	AM_RANGE(0xa091, 0xa091) AM_DEVREAD("ay2", ay8910_r)
	AM_RANGE(0xa092, 0xa092) AM_DEVWRITE("ay2", ay8910_data_w)
	AM_RANGE(0xa0e0, 0xa0e0) AM_WRITE(getstar_sh_intenable_w) /* maybe a0f0 also -LE */
//  AM_RANGE(0xa0f0, 0xa0f0) AM_WRITENOP
	AM_RANGE(0xc800, 0xc80f) AM_RAM AM_SHARE(1)
	AM_RANGE(0xc810, 0xcfff) AM_RAM
	AM_RANGE(0xd000, 0xffff) AM_RAM
ADDRESS_MAP_END



static INPUT_PORTS_START( slapfght_generic )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_8WAY PORT_COCKTAIL

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )          PORT_DIPLOCATION("SW1:8,7,6")
//  PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) )      PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Cabinet ) )          PORT_DIPLOCATION("SW1:4")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )      PORT_DIPLOCATION("SW1:3")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE_DIPLOC(0x40, IP_ACTIVE_LOW, "SW1:2" )
	PORT_DIPUNUSED_DIPLOC( 0x80, IP_ACTIVE_LOW, "SW1:1" )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )            PORT_DIPLOCATION("SW2:8,7")
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW2:6,5")
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	/* Filled by "Bonus Lives" settings */
	PORT_DIPUNUSED_DIPLOC( 0x10, IP_ACTIVE_LOW, "SW2:4" )
	PORT_DIPUNUSED_DIPLOC( 0x20, IP_ACTIVE_LOW, "SW2:3" )
	PORT_DIPUNUSED_DIPLOC( 0x40, IP_ACTIVE_LOW, "SW2:2" )
	PORT_DIPUNUSED_DIPLOC( 0x80, IP_ACTIVE_LOW, "SW2:1" )
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( perfrman )
	PORT_INCLUDE(slapfght_generic)

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x20, 0x20, "Intermissions" )             PORT_DIPLOCATION("SW1:3")     /* "Screen Test" in the "test mode" */
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0xf0, 0x70, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION("SW2:4,3,2,1")
	PORT_DIPSETTING(    0xb0, "20k, 120k, then every 100k" )
	PORT_DIPSETTING(    0xa0, "40k, 120k, then every 100k" )
	PORT_DIPSETTING(    0x90, "60k, 160k, then every 100k" )
	PORT_DIPSETTING(    0x80, "Every 100k" )
	PORT_DIPSETTING(    0x70, "20k, 220k, then every 200k" )
	PORT_DIPSETTING(    0x60, "40k, 240k, then every 200k" )
	PORT_DIPSETTING(    0x50, "60k, 260k, then every 200k" )
	PORT_DIPSETTING(    0x40, "Every 200k" )
	PORT_DIPSETTING(    0x30, "20k, 320k, then every 300k" )
	PORT_DIPSETTING(    0x20, "40k, 340k, then every 300k" )
	PORT_DIPSETTING(    0x10, "60k, 360k, then every 300k" )
	PORT_DIPSETTING(    0x00, "Every 300k" )
	PORT_DIPSETTING(    0xf0, "20k only" )
	PORT_DIPSETTING(    0xe0, "40k only" )
	PORT_DIPSETTING(    0xd0, "60k only" )
	PORT_DIPSETTING(    0xc0, DEF_STR( None ) )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( tigerh )
	PORT_INCLUDE(slapfght_generic)

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x80, 0x80, "Player Speed" )              PORT_DIPLOCATION("SW1:1")
	PORT_DIPSETTING(    0x80, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, "Fast" )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )            PORT_DIPLOCATION("SW2:8,7")
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW1:6,5")
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION("SW1:4")
	PORT_DIPSETTING(    0x10, "20k, 100k, then every 80k" )                               // see notes for 'tigerhb3'
	PORT_DIPSETTING(    0x00, "50k, 170k, then every 120k" )
	PORT_DIPUNUSED_DIPLOC( 0x20, IP_ACTIVE_LOW, "SW2:3" )
	PORT_DIPUNUSED_DIPLOC( 0x40, IP_ACTIVE_LOW, "SW2:2" )
	PORT_DIPUNUSED_DIPLOC( 0x80, IP_ACTIVE_LOW, "SW2:1" )
INPUT_PORTS_END


static INPUT_PORTS_START( slapfigh )
	PORT_INCLUDE(slapfght_generic)

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Screen Test" ) PORT_CODE(KEYCODE_F1) PORT_TOGGLE
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x02, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0c, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "30000 100000" )
	PORT_DIPSETTING(    0x10, "50000 200000" )
	PORT_DIPSETTING(    0x20, "50000" )
	PORT_DIPSETTING(    0x00, "100000" )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( getstar )
	PORT_INCLUDE(slapfght_generic)

	PORT_MODIFY("DSW2")
	/* harder default difficulty (confirmed by manual) */
	PORT_DIPNAME( 0x0c, 0x04, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW2:6,5")
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION("SW2:4,3")
	PORT_DIPSETTING(    0x30, "50k only" )
	PORT_DIPSETTING(    0x20, "100k only" )
	PORT_DIPSETTING(    0x10, "150k only" )
	PORT_DIPSETTING(    0x00, "200k only" )
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( getstarj )
	PORT_INCLUDE(getstar)

	/* easier default difficulty (to be confirmed) and different bonus lives settings */
	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW2:6,5")
	PORT_DIPSETTING(    0x0c, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x10, "30k, 130k, then every 100k" )
	PORT_DIPSETTING(    0x00, "50k, 200k, then every 150k" )
	PORT_DIPUNUSED_DIPLOC( 0x20, IP_ACTIVE_LOW, "SW2:3" )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( gtstarb2 )
	PORT_INCLUDE(getstarj)

	/* RIGHT and DOWN are swapped due to code at 0x0551 */
	PORT_MODIFY("IN0")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_8WAY PORT_COCKTAIL

	/* different lives settings due to code at 0x0e36 */
	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )            PORT_DIPLOCATION("SW2:8,7")
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPSETTING(    0x03, "240 (Cheat)")
	/* difficulty settings have no effect due to code at 0x0e7c */
	/* I don't mark the Dips as "unused" because they are still read in the "test mode" */
INPUT_PORTS_END
#endif

static const gfx_layout charlayout =
{
	8,8,			/* 8*8 characters */
	RGN_FRAC(1,2),	/* 1024 characters */
	2,				/* 2 bits per pixel */
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8     /* every char takes 8 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	8,8,			/* 8*8 tiles */
	RGN_FRAC(1,4),	/* 2048/4096 tiles */
	4,				/* 4 bits per pixel */
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8    /* every tile takes 8 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,			/* 16*16 sprites */
	RGN_FRAC(1,4),	/* 512/1024 sprites */
	4,				/* 4 bits per pixel */
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8,
			9, 10 ,11, 12, 13, 14, 15 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	32*8    /* every sprite takes 64 consecutive bytes */
};

#if 0
static const gfx_layout perfrman_charlayout =
{
	8,8,			/* 8*8 characters */
	RGN_FRAC(1,3),	/* 1024 characters */
	3,				/* 3 bits per pixel */
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8     /* every char takes 8 consecutive bytes */
};
#endif
#if 0
static const gfx_layout perfrman_spritelayout =
{
	16,16,			/* 16*16 sprites */
	RGN_FRAC(1,3),	/* 256 sprites */
	3,				/* 3 bits per pixel */
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8,
			9, 10 ,11, 12, 13, 14, 15 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	32*8
};
#endif
#if 0
static GFXDECODE_START( perfrman )
	GFXDECODE_ENTRY( "gfx1", 0, perfrman_charlayout,     0, 16 )
	GFXDECODE_ENTRY( "gfx2", 0, perfrman_spritelayout, 128, 16 )
GFXDECODE_END
#endif

static GFXDECODE_START( slapfght )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,   0,  64 )
	GFXDECODE_ENTRY( "gfx2", 0, tilelayout,   0,  16 )
	GFXDECODE_ENTRY( "gfx3", 0, spritelayout, 0,  16 )
GFXDECODE_END



static const ay8910_interface ay8910_interface_1 =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_INPUT_PORT("IN0"),
	DEVCB_INPUT_PORT("IN1"),
	DEVCB_NULL,
	DEVCB_NULL
};

static const ay8910_interface ay8910_interface_2 =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_INPUT_PORT("DSW1"),
	DEVCB_INPUT_PORT("DSW2"),
	DEVCB_NULL,
	DEVCB_NULL
};

static VIDEO_EOF( perfrman )
{
	const address_space *space = cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM);
	buffer_spriteram_w(space, 0, 0);
}

#if 0
static MACHINE_DRIVER_START( perfrman )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80,16000000/4)			/* 4MHz ???, 16MHz Oscillator */
	MDRV_CPU_PROGRAM_MAP(perfrman_map)
	MDRV_CPU_IO_MAP(slapfght_io_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80,16000000/8)			/* 2MHz ???, 16MHz Oscillator */
	MDRV_CPU_PROGRAM_MAP(perfrman_sound_map)
	MDRV_CPU_VBLANK_INT_HACK(getstar_interrupt,4)	/* music speed, verified */

	MDRV_QUANTUM_TIME(HZ(600))		/* 10 CPU slices per frame - enough for the sound CPU to read all commands */

	MDRV_MACHINE_RESET(slapfight)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(1*8, 34*8-1, 2*8, 32*8-1)

	MDRV_GFXDECODE(perfrman)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(perfrman)
	MDRV_VIDEO_EOF(perfrman)
	MDRV_VIDEO_UPDATE(perfrman)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 16000000/8)
	MDRV_SOUND_CONFIG(ay8910_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD("ay2", AY8910, 16000000/8)
	MDRV_SOUND_CONFIG(ay8910_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END
#endif
#if 0
static MACHINE_DRIVER_START( tigerhb )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, 6000000)
	MDRV_CPU_PROGRAM_MAP(tigerh_map)
	MDRV_CPU_IO_MAP(tigerhb_io_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80, 6000000)
	MDRV_CPU_PROGRAM_MAP(slapfght_sound_map)
	MDRV_CPU_VBLANK_INT_HACK(nmi_line_pulse,6)    /* ??? */

	MDRV_QUANTUM_TIME(HZ(600))	/* 10 CPU slices per frame - enough for the sound CPU to read all commands */

	MDRV_MACHINE_RESET(slapfight)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(1*8, 36*8-1, 2*8, 32*8-1)

	MDRV_GFXDECODE(slapfght)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(slapfight)
	MDRV_VIDEO_EOF(perfrman)
	MDRV_VIDEO_UPDATE(slapfight)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8910_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD("ay2", AY8910, 1500000)
	MDRV_SOUND_CONFIG(ay8910_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END
#endif

static MACHINE_DRIVER_START( tigerh )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, XTAL_36MHz/6) /* verified on pcb */
	MDRV_CPU_PROGRAM_MAP(tigerh_map)
	MDRV_CPU_IO_MAP(tigerh_io_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80, XTAL_36MHz/12) /* verified on pcb */
	MDRV_CPU_PROGRAM_MAP(slapfght_sound_map)
	MDRV_CPU_VBLANK_INT_HACK(nmi_line_pulse,6)    /* ??? */

	MDRV_CPU_ADD("mcu", M68705,XTAL_36MHz/12) /* verified on pcb */
	MDRV_CPU_PROGRAM_MAP(tigerh_m68705_map)

	MDRV_QUANTUM_TIME(HZ(600))	/* 10 CPU slices per frame - enough for the sound CPU to read all commands */

	MDRV_MACHINE_RESET(slapfight)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(1*8, 36*8-1, 2*8, 32*8-1)

	MDRV_GFXDECODE(slapfght)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(slapfight)
	MDRV_VIDEO_EOF(perfrman)
	MDRV_VIDEO_UPDATE(slapfight)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, XTAL_36MHz/24) /* verified on pcb */
	MDRV_SOUND_CONFIG(ay8910_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD("ay2", AY8910, XTAL_36MHz/24) /* verified on pcb */
	MDRV_SOUND_CONFIG(ay8910_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( slapfigh )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu",Z80, XTAL_36MHz/6) /* verified on pcb */
	MDRV_CPU_PROGRAM_MAP(slapfght_map)
	MDRV_CPU_IO_MAP(slapfght_io_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80, XTAL_36MHz/12) /* verified on pcb */
	MDRV_CPU_PROGRAM_MAP(slapfght_sound_map)
	MDRV_CPU_VBLANK_INT_HACK(getstar_interrupt, 3)

	MDRV_QUANTUM_TIME(HZ(600))	/* 10 CPU slices per frame - enough for the sound CPU to read all commands */

	MDRV_MACHINE_RESET(slapfight)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(1*8, 36*8-1, 2*8, 32*8-1)

	MDRV_GFXDECODE(slapfght)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)
	MDRV_VIDEO_START(slapfight)
	MDRV_VIDEO_EOF(perfrman)
	MDRV_VIDEO_UPDATE(slapfight)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, XTAL_36MHz/24) /* verified on pcb */
	MDRV_SOUND_CONFIG(ay8910_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD("ay2", AY8910, XTAL_36MHz/24) /* verified on pcb */
	MDRV_SOUND_CONFIG(ay8910_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END

/* identical to slapfigh_ but the scroll registers are located elsewhere in memory */
static MACHINE_DRIVER_START( slapbtuk )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(slapfigh)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(slapbtuk_map)
MACHINE_DRIVER_END

#if 0
ROM_START( perfrman )
	ROM_REGION( 0x10000, "maincpu", 0 )				 /* Main CPU code */
	ROM_LOAD( "ci07.0",    0x00000, 0x4000, CRC(7ad32eea) SHA1(e5b29793e9c8c5c9322ca2af468a9810a598c0ae) )
	ROM_LOAD( "ci08.1",    0x04000, 0x4000, CRC(90a02d5f) SHA1(9f2d2ce70a5bc96fc9d268e2b24533f73361225c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )				 /* Sound CPU code */
	ROM_LOAD( "ci06.4",    0x0000, 0x2000, CRC(df891ad0) SHA1(0d33e7d0562831382f48d1588ef20a1bc73be71a) )

	ROM_REGION( 0x6000, "gfx1", 0 ) /* Tiles */
	ROM_LOAD( "ci02.7",     0x0000, 0x2000, CRC(8efa960a) SHA1(d547ea23f2dd622500bf3f38cd9aca4e80aa27ca) )
	ROM_LOAD( "ci01.6",     0x2000, 0x2000, CRC(2e8e69df) SHA1(183c1868f0c94a2a82709f9c38020ee81c283051) )
	ROM_LOAD( "ci00.5",     0x4000, 0x2000, CRC(79e191f8) SHA1(3a755857dab147b73761aebfcf931dc3c87286a4) )

	ROM_REGION( 0x6000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "ci05.10",    0x0000, 0x2000, CRC(809a4ccc) SHA1(bca5a27abe205a65e1160d0c0c61e9831a949acc) )
	ROM_LOAD( "ci04.9",     0x2000, 0x2000, CRC(026f27b3) SHA1(a222d31368fa5117824f5a14a1e52f01326e1f63) )
	ROM_LOAD( "ci03.8",     0x4000, 0x2000, CRC(6410d9eb) SHA1(7e57de9255cbcacb4610cabb1364e2a4933ec12b) )

	ROM_REGION( 0x300, "proms", 0 )				 /* Color BPROMs */
	ROM_LOAD( "ci14.16",    0x000, 0x0100, CRC(515f8a3b) SHA1(a99d4c119f6c4c6cd1b3fd208eadfb69ef7e8e2d) )
	ROM_LOAD( "ci13.15",    0x100, 0x0100, CRC(a9a397eb) SHA1(a84cf23efa0cf3e97b8dd1fff868c85d7eda1253) )
	ROM_LOAD( "ci12.14",    0x200, 0x0100, CRC(67f86e3d) SHA1(b1240212ea91cf451dbd7c6e2bfccbac76568cf6) )

	ROM_REGION( 0x220, "user1", 0 )
	ROM_LOAD( "ci11.11",    0x000, 0x0100, CRC(d492e6c2) SHA1(5789adda3a63ef8656ebd012416fcf3f991241fe) )
	ROM_LOAD( "ci10.12",    0x100, 0x0100, CRC(59490887) SHA1(c894edecbcfc67972ad893cd7c8197d07862a20a) )
	ROM_LOAD( "ci09.13",    0x200, 0x0020, CRC(aa0ca5a5) SHA1(4c45be71658f40ebb05634febba5822f1a8a7f79) )
ROM_END
#endif
#if 0
ROM_START( perfrmanu )
	ROM_REGION( 0x10000, "maincpu", 0 )				 /* Main CPU code */
	ROM_LOAD( "ci07.0",    0x00000, 0x4000, CRC(7ad32eea) SHA1(e5b29793e9c8c5c9322ca2af468a9810a598c0ae) )
	ROM_LOAD( "ci108r5.1", 0x04000, 0x4000, CRC(9d373efa) SHA1(b1d87e033ee3c50cfc56db05891b00b7bc236733) )

	ROM_REGION( 0x10000, "audiocpu", 0 )				 /* Sound CPU code */
	ROM_LOAD( "ci06.4",    0x0000, 0x2000, CRC(df891ad0) SHA1(0d33e7d0562831382f48d1588ef20a1bc73be71a) )

	ROM_REGION( 0x6000, "gfx1", 0 ) /* Tiles */
	ROM_LOAD( "ci02.7",     0x0000, 0x2000, CRC(8efa960a) SHA1(d547ea23f2dd622500bf3f38cd9aca4e80aa27ca) )
	ROM_LOAD( "ci01.6",     0x2000, 0x2000, CRC(2e8e69df) SHA1(183c1868f0c94a2a82709f9c38020ee81c283051) )
	ROM_LOAD( "ci00.5",     0x4000, 0x2000, CRC(79e191f8) SHA1(3a755857dab147b73761aebfcf931dc3c87286a4) )

	ROM_REGION( 0x6000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "ci05.10",    0x0000, 0x2000, CRC(809a4ccc) SHA1(bca5a27abe205a65e1160d0c0c61e9831a949acc) )
	ROM_LOAD( "ci04.9",     0x2000, 0x2000, CRC(026f27b3) SHA1(a222d31368fa5117824f5a14a1e52f01326e1f63) )
	ROM_LOAD( "ci03.8",     0x4000, 0x2000, CRC(6410d9eb) SHA1(7e57de9255cbcacb4610cabb1364e2a4933ec12b) )

	ROM_REGION( 0x300, "proms", 0 )				 /* Color BPROMs */
	ROM_LOAD( "ci14.16",    0x000, 0x0100, CRC(515f8a3b) SHA1(a99d4c119f6c4c6cd1b3fd208eadfb69ef7e8e2d) )
	ROM_LOAD( "ci13.15",    0x100, 0x0100, CRC(a9a397eb) SHA1(a84cf23efa0cf3e97b8dd1fff868c85d7eda1253) )
	ROM_LOAD( "ci12.14",    0x200, 0x0100, CRC(67f86e3d) SHA1(b1240212ea91cf451dbd7c6e2bfccbac76568cf6) )

	ROM_REGION( 0x220, "user1", 0 )
	ROM_LOAD( "ci11.11",    0x000, 0x0100, CRC(d492e6c2) SHA1(5789adda3a63ef8656ebd012416fcf3f991241fe) )
	ROM_LOAD( "ci10.12",    0x100, 0x0100, CRC(59490887) SHA1(c894edecbcfc67972ad893cd7c8197d07862a20a) )
	ROM_LOAD( "ci09r1.13",  0x200, 0x0020, CRC(d9e92f6f) SHA1(7dc2939267b7d2b1eeeca906cc6151fab2cf1cc4) )
ROM_END
#endif

/*
Tiger Heli
1985 Taito Corporation

The Tiger Heli PCB is almost exactly the same PCB as Slap Fight/Alcon but a few chips
have different placings and there's more EPROM locations on one of the boards
(although they're not populated).
Also, Tiger Heli has a 22-way edge connector, while Alcon/Slap Fight is JAMMA.


PCB Layouts - Top Board
-----------------------

Tiger Heli -   M6100111A
               850011401

GX-511-A MADE IN JAPAN
|--------------------------------------------------|
|VOL ROM21.12Q                 2148                |
|MB3712                        2148                |
|    ROM19.12P                 2148                |
|                              2148                |
|    ROM20.12M                         6264        |
|                                                  |
|                                      6264        |
|                                                  |
|2                 A47_13.8J A47_12.6J 6264        |
|2    AY3-8910                                     |
|W                 A47_11.8H A47_10.6H 6264        |
|A    DSW1  DSW2                                   |
|Y                                       2148      |
|     AY3-8910                                     |
|                                   PAL    ROM16.1E|
|                                                  |
|  A47_03.12D                              ROM17.1C|
|                                                  |
|                                     ROM18.2B     |
|  Z80A   2016                                     |
|                                                  |
|-----|----------|--------------|----------|-------|
      |----------|              |----------|
Notes:
      AY3-8910 clock - 3.000MHz (36/12)
      Z80A clock - 3.000MHz (36/12)
      VSync - 57Hz
      HSync - 15.02kHz
      A47_03 - 2764 EPROM
      A47_10 to A47_13 - 27128 EPROM
      ROM18 - 18S030 PROM
      ROM16, ROM17, ROM19, ROM20, ROM21 - 82S129 PROM


Bottom Board
------------

GX-511-B MADE IN JAPAN
|--------------------------------------------------|
|                        A47_14.6A                 |
|                                                  |
|    ROM14.2C                          ROM15.8B    |
|                                                  |
|                                                  |
|              2016      A47_05.6F                 |
|                                           36MHz  |
|              2016      A47_04.6G                 |
|                                                  |
|                                                  |
|                                                  |
|                                                  |
|                                                  |
|                                        A47_02.8K |
|                                                  |
|              A47_09.4M A47_08.6M       X         |
|     2016                                         |
|              2016      A47_07.6N       A47_01.8N |
|                                                  |
|              2016      A47_06.6P       A47_00.8P |
|                                           Z80B   |
|-----|----------|--------------|----------|-------|
      |----------|              |----------|
Notes:
      Z80B clock - 6.000MHz (36/6)
      ROM14, ROM15 - 82S129 PROM
      A47_00 to A47_02 - 27128 EPROM
      A47_06 to A47_09 - 27128 EPROM
      A47_04, A47_05 - 2764 EPROM
      A47_14 - Motorola DA68237 15-00011-001 (MC68705P3-compatible) Micro-Controller (Not protected). Clock 3.000MHz
      X - Unpopulated socket
*/

ROM_START( tigerh )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "0.4",          0x00000, 0x4000, CRC(4be73246) SHA1(a6f6a36fa7e3d269b87b777c0975b210d8b53483) )
	ROM_LOAD( "1.4",          0x04000, 0x4000, CRC(aad04867) SHA1(5e9ff3c982afe104428e936ef417de2d238dc033) )
	ROM_LOAD( "2.4",          0x08000, 0x4000, CRC(4843f15c) SHA1(c0c145c9df9d6273171ac64fb7396e65a786f67c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "a47_03.bin",   0x0000,  0x2000, CRC(d105260f) SHA1(f6a0e393e29354bb37fb723828f3267d030a45ea) )

	ROM_REGION( 0x0800, "mcu", 0 )
	ROM_LOAD( "a47_14.6a",   0x0000, 0x0800, CRC(4042489f) SHA1(b977e0821b6b1aa5a0a0f349cd78150af1a231df) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "a47_05.bin",   0x00000, 0x2000, CRC(c5325b49) SHA1(6df9051e7545dcac4995340f80957510457aaf64) )  /* Chars */
	ROM_LOAD( "a47_04.bin",   0x02000, 0x2000, CRC(cd59628e) SHA1(7be6479f20eb51b79b93e6fd65ab219096d54984) )

	ROM_REGION( 0x10000, "gfx2", 0 )
	ROM_LOAD( "a47_09.bin",   0x00000, 0x4000, CRC(31fae8a8) SHA1(ef8c23776431f00a74b25c5800755b6fa8d585ec) )  /* Tiles */
	ROM_LOAD( "a47_08.bin",   0x04000, 0x4000, CRC(e539af2b) SHA1(0c8369a0fac1cbe40c07b51e16e8f8a9b8ed03b8) )
	ROM_LOAD( "a47_07.bin",   0x08000, 0x4000, CRC(02fdd429) SHA1(fa392f2e57cfb6af4c124e0c151a4652f83e5577) )
	ROM_LOAD( "a47_06.bin",   0x0c000, 0x4000, CRC(11fbcc8c) SHA1(b4fdb9ee00b749e1a54cfc0cdf55cc5e9bee3662) )

	ROM_REGION( 0x10000, "gfx3", 0 )
	ROM_LOAD( "a47_13.bin",   0x00000, 0x4000, CRC(739a7e7e) SHA1(5fee71d9e1540903a6cf7bcaab30acaa088d35ed) )  /* Sprites */
	ROM_LOAD( "a47_12.bin",   0x04000, 0x4000, CRC(c064ecdb) SHA1(fa8d712e2b2bda78b9375d96c93a4d7549c94075) )
	ROM_LOAD( "a47_11.bin",   0x08000, 0x4000, CRC(744fae9b) SHA1(b324350469c51043e1d90ce58808d966467435b9) )
	ROM_LOAD( "a47_10.bin",   0x0c000, 0x4000, CRC(e1cf844e) SHA1(eeb8eff09f96c693e147d155a8c0a87416d64603) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "82s129.12q",   0x0000,  0x0100, CRC(2c69350d) SHA1(658bf63c6d1e718f99494cd1c9346c3622913beb) )
	ROM_LOAD( "82s129.12m",   0x0100,  0x0100, CRC(7142e972) SHA1(4a854c2fdd006077aecb695832110ae6bf5819c1) )
	ROM_LOAD( "82s129.12n",   0x0200,  0x0100, CRC(25f273f2) SHA1(2c696745f42fa09b64295a39536aeba08ab58d67) )

	ROM_REGION( 0x0200, "plds", 0 )
	ROM_LOAD( "pal16r4a.2e", 0x0000, 0x0104, NO_DUMP ) /* PAL is read protected */
ROM_END

#if 0
ROM_START( tigerhj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "a47_00.bin",   0x00000, 0x4000, CRC(cbdbe3cc) SHA1(5badf76cdf4a7f0ae9e85ee602420ba5c128efef) )
	ROM_LOAD( "a47_01.bin",   0x04000, 0x4000, CRC(65df2152) SHA1(8e1516905a4af379cb0d0b9d42ff1cc3179c3589) )
	ROM_LOAD( "a47_02.bin",   0x08000, 0x4000, CRC(633d324b) SHA1(70a17d17ebe003bfb2246e92e925a343a92553e5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "a47_03.bin",   0x0000,  0x2000, CRC(d105260f) SHA1(f6a0e393e29354bb37fb723828f3267d030a45ea) )

	ROM_REGION( 0x0800, "mcu", 0 )
	ROM_LOAD( "a47_14.6a",   0x0000, 0x0800, CRC(4042489f) SHA1(b977e0821b6b1aa5a0a0f349cd78150af1a231df) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "a47_05.bin",   0x00000, 0x2000, CRC(c5325b49) SHA1(6df9051e7545dcac4995340f80957510457aaf64) )  /* Chars */
	ROM_LOAD( "a47_04.bin",   0x02000, 0x2000, CRC(cd59628e) SHA1(7be6479f20eb51b79b93e6fd65ab219096d54984) )

	ROM_REGION( 0x10000, "gfx2", 0 )
	ROM_LOAD( "a47_09.bin",   0x00000, 0x4000, CRC(31fae8a8) SHA1(ef8c23776431f00a74b25c5800755b6fa8d585ec) )  /* Tiles */
	ROM_LOAD( "a47_08.bin",   0x04000, 0x4000, CRC(e539af2b) SHA1(0c8369a0fac1cbe40c07b51e16e8f8a9b8ed03b8) )
	ROM_LOAD( "a47_07.bin",   0x08000, 0x4000, CRC(02fdd429) SHA1(fa392f2e57cfb6af4c124e0c151a4652f83e5577) )
	ROM_LOAD( "a47_06.bin",   0x0c000, 0x4000, CRC(11fbcc8c) SHA1(b4fdb9ee00b749e1a54cfc0cdf55cc5e9bee3662) )

	ROM_REGION( 0x10000, "gfx3", 0 )
	ROM_LOAD( "a47_13.bin",   0x00000, 0x4000, CRC(739a7e7e) SHA1(5fee71d9e1540903a6cf7bcaab30acaa088d35ed) )  /* Sprites */
	ROM_LOAD( "a47_12.bin",   0x04000, 0x4000, CRC(c064ecdb) SHA1(fa8d712e2b2bda78b9375d96c93a4d7549c94075) )
	ROM_LOAD( "a47_11.bin",   0x08000, 0x4000, CRC(744fae9b) SHA1(b324350469c51043e1d90ce58808d966467435b9) )
	ROM_LOAD( "a47_10.bin",   0x0c000, 0x4000, CRC(e1cf844e) SHA1(eeb8eff09f96c693e147d155a8c0a87416d64603) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "82s129.12q",   0x0000,  0x0100, CRC(2c69350d) SHA1(658bf63c6d1e718f99494cd1c9346c3622913beb) )
	ROM_LOAD( "82s129.12m",   0x0100,  0x0100, CRC(7142e972) SHA1(4a854c2fdd006077aecb695832110ae6bf5819c1) )
	ROM_LOAD( "82s129.12n",   0x0200,  0x0100, CRC(25f273f2) SHA1(2c696745f42fa09b64295a39536aeba08ab58d67) )
ROM_END
#endif
#if 0
ROM_START( tigerhb1 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "b0.5",         0x00000, 0x4000, CRC(6ae7e13c) SHA1(47ef34635f8648e883a850293d92a46e95976a50) )
	ROM_LOAD( "a47_01.bin",   0x04000, 0x4000, CRC(65df2152) SHA1(8e1516905a4af379cb0d0b9d42ff1cc3179c3589) )
	ROM_LOAD( "a47_02.bin",   0x08000, 0x4000, CRC(633d324b) SHA1(70a17d17ebe003bfb2246e92e925a343a92553e5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "a47_03.bin",   0x0000,  0x2000, CRC(d105260f) SHA1(f6a0e393e29354bb37fb723828f3267d030a45ea) )

	ROM_REGION( 0x0800, "cpu2", ROMREGION_ERASEFF ) // probably doesn't have an MCU
	/* is this the right mcu for this set? the mcu handling code in the roms seems patched and it doesn't work correctly */
//  ROM_LOAD( "a47_14.6a",   0x0000, 0x0800, CRC(4042489f) SHA1(b977e0821b6b1aa5a0a0f349cd78150af1a231df) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "a47_05.bin",   0x00000, 0x2000, CRC(c5325b49) SHA1(6df9051e7545dcac4995340f80957510457aaf64) )  /* Chars */
	ROM_LOAD( "a47_04.bin",   0x02000, 0x2000, CRC(cd59628e) SHA1(7be6479f20eb51b79b93e6fd65ab219096d54984) )

	ROM_REGION( 0x10000, "gfx2", 0 )
	ROM_LOAD( "a47_09.bin",   0x00000, 0x4000, CRC(31fae8a8) SHA1(ef8c23776431f00a74b25c5800755b6fa8d585ec) )  /* Tiles */
	ROM_LOAD( "a47_08.bin",   0x04000, 0x4000, CRC(e539af2b) SHA1(0c8369a0fac1cbe40c07b51e16e8f8a9b8ed03b8) )
	ROM_LOAD( "a47_07.bin",   0x08000, 0x4000, CRC(02fdd429) SHA1(fa392f2e57cfb6af4c124e0c151a4652f83e5577) )
	ROM_LOAD( "a47_06.bin",   0x0c000, 0x4000, CRC(11fbcc8c) SHA1(b4fdb9ee00b749e1a54cfc0cdf55cc5e9bee3662) )

	ROM_REGION( 0x10000, "gfx3", 0 )
	ROM_LOAD( "a47_13.bin",   0x00000, 0x4000, CRC(739a7e7e) SHA1(5fee71d9e1540903a6cf7bcaab30acaa088d35ed) )  /* Sprites */
	ROM_LOAD( "a47_12.bin",   0x04000, 0x4000, CRC(c064ecdb) SHA1(fa8d712e2b2bda78b9375d96c93a4d7549c94075) )
	ROM_LOAD( "a47_11.bin",   0x08000, 0x4000, CRC(744fae9b) SHA1(b324350469c51043e1d90ce58808d966467435b9) )
	ROM_LOAD( "a47_10.bin",   0x0c000, 0x4000, CRC(e1cf844e) SHA1(eeb8eff09f96c693e147d155a8c0a87416d64603) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "82s129.12q",   0x0000,  0x0100, CRC(2c69350d) SHA1(658bf63c6d1e718f99494cd1c9346c3622913beb) )
	ROM_LOAD( "82s129.12m",   0x0100,  0x0100, CRC(7142e972) SHA1(4a854c2fdd006077aecb695832110ae6bf5819c1) )
	ROM_LOAD( "82s129.12n",   0x0200,  0x0100, CRC(25f273f2) SHA1(2c696745f42fa09b64295a39536aeba08ab58d67) )
ROM_END
#endif
#if 0
ROM_START( tigerhb2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "rom00_09.bin", 0x00000, 0x4000, CRC(ef738c68) SHA1(c78c802d885b7f7c5e312ec079d52b8817590735) )
	ROM_LOAD( "a47_01.bin",   0x04000, 0x4000, CRC(65df2152) SHA1(8e1516905a4af379cb0d0b9d42ff1cc3179c3589) )
	ROM_LOAD( "rom02_07.bin", 0x08000, 0x4000, CRC(36e250b9) SHA1(79bd86bde81981e4d0dbee420bc0a10c80b5241e) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "a47_03.bin",   0x0000,  0x2000, CRC(d105260f) SHA1(f6a0e393e29354bb37fb723828f3267d030a45ea) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "a47_05.bin",   0x00000, 0x2000, CRC(c5325b49) SHA1(6df9051e7545dcac4995340f80957510457aaf64) )  /* Chars */
	ROM_LOAD( "a47_04.bin",   0x02000, 0x2000, CRC(cd59628e) SHA1(7be6479f20eb51b79b93e6fd65ab219096d54984) )

	ROM_REGION( 0x10000, "gfx2", 0 )
	ROM_LOAD( "a47_09.bin",   0x00000, 0x4000, CRC(31fae8a8) SHA1(ef8c23776431f00a74b25c5800755b6fa8d585ec) )  /* Tiles */
	ROM_LOAD( "a47_08.bin",   0x04000, 0x4000, CRC(e539af2b) SHA1(0c8369a0fac1cbe40c07b51e16e8f8a9b8ed03b8) )
	ROM_LOAD( "a47_07.bin",   0x08000, 0x4000, CRC(02fdd429) SHA1(fa392f2e57cfb6af4c124e0c151a4652f83e5577) )
	ROM_LOAD( "a47_06.bin",   0x0c000, 0x4000, CRC(11fbcc8c) SHA1(b4fdb9ee00b749e1a54cfc0cdf55cc5e9bee3662) )

	ROM_REGION( 0x10000, "gfx3", 0 )
	ROM_LOAD( "a47_13.bin",   0x00000, 0x4000, CRC(739a7e7e) SHA1(5fee71d9e1540903a6cf7bcaab30acaa088d35ed) )  /* Sprites */
	ROM_LOAD( "a47_12.bin",   0x04000, 0x4000, CRC(c064ecdb) SHA1(fa8d712e2b2bda78b9375d96c93a4d7549c94075) )
	ROM_LOAD( "a47_11.bin",   0x08000, 0x4000, CRC(744fae9b) SHA1(b324350469c51043e1d90ce58808d966467435b9) )
	ROM_LOAD( "a47_10.bin",   0x0c000, 0x4000, CRC(e1cf844e) SHA1(eeb8eff09f96c693e147d155a8c0a87416d64603) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "82s129.12q",   0x0000,  0x0100, CRC(2c69350d) SHA1(658bf63c6d1e718f99494cd1c9346c3622913beb) )
	ROM_LOAD( "82s129.12m",   0x0100,  0x0100, CRC(7142e972) SHA1(4a854c2fdd006077aecb695832110ae6bf5819c1) )
	ROM_LOAD( "82s129.12n",   0x0200,  0x0100, CRC(25f273f2) SHA1(2c696745f42fa09b64295a39536aeba08ab58d67) )
ROM_END
#endif
#if 0
ROM_START( tigerhb3 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "14",           0x00000, 0x4000, CRC(ca59dd73) SHA1(c07961fcc209ec10ace3830d79c8ccc1cfda9765) )
	ROM_LOAD( "13",           0x04000, 0x4000, CRC(38bd54db) SHA1(75e999f606c410d7481bc4d29c4b523d45847649) )
	ROM_LOAD( "a47_02.bin",   0x08000, 0x4000, CRC(633d324b) SHA1(70a17d17ebe003bfb2246e92e925a343a92553e5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "a47_03.bin",   0x0000,  0x2000, CRC(d105260f) SHA1(f6a0e393e29354bb37fb723828f3267d030a45ea) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "a47_05.bin",   0x00000, 0x2000, CRC(c5325b49) SHA1(6df9051e7545dcac4995340f80957510457aaf64) )  /* Chars */
	ROM_LOAD( "a47_04.bin",   0x02000, 0x2000, CRC(cd59628e) SHA1(7be6479f20eb51b79b93e6fd65ab219096d54984) )

	ROM_REGION( 0x10000, "gfx2", 0 )
	ROM_LOAD( "a47_09.bin",   0x00000, 0x4000, CRC(31fae8a8) SHA1(ef8c23776431f00a74b25c5800755b6fa8d585ec) )  /* Tiles */
	ROM_LOAD( "a47_08.bin",   0x04000, 0x4000, CRC(e539af2b) SHA1(0c8369a0fac1cbe40c07b51e16e8f8a9b8ed03b8) )
	ROM_LOAD( "a47_07.bin",   0x08000, 0x4000, CRC(02fdd429) SHA1(fa392f2e57cfb6af4c124e0c151a4652f83e5577) )
	ROM_LOAD( "a47_06.bin",   0x0c000, 0x4000, CRC(11fbcc8c) SHA1(b4fdb9ee00b749e1a54cfc0cdf55cc5e9bee3662) )

	ROM_REGION( 0x10000, "gfx3", 0 )
	ROM_LOAD( "a47_13.bin",   0x00000, 0x4000, CRC(739a7e7e) SHA1(5fee71d9e1540903a6cf7bcaab30acaa088d35ed) )  /* Sprites */
	ROM_LOAD( "a47_12.bin",   0x04000, 0x4000, CRC(c064ecdb) SHA1(fa8d712e2b2bda78b9375d96c93a4d7549c94075) )
	ROM_LOAD( "a47_11.bin",   0x08000, 0x4000, CRC(744fae9b) SHA1(b324350469c51043e1d90ce58808d966467435b9) )
	ROM_LOAD( "a47_10.bin",   0x0c000, 0x4000, CRC(e1cf844e) SHA1(eeb8eff09f96c693e147d155a8c0a87416d64603) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "82s129.12q",   0x0000,  0x0100, CRC(2c69350d) SHA1(658bf63c6d1e718f99494cd1c9346c3622913beb) )
	ROM_LOAD( "82s129.12m",   0x0100,  0x0100, CRC(7142e972) SHA1(4a854c2fdd006077aecb695832110ae6bf5819c1) )
	ROM_LOAD( "82s129.12n",   0x0200,  0x0100, CRC(25f273f2) SHA1(2c696745f42fa09b64295a39536aeba08ab58d67) )
ROM_END
#endif

ROM_START( slapfigh )
	ROM_REGION( 0x18000, "maincpu", 0 )
	ROM_LOAD( "sf_r19.bin",   0x00000, 0x8000, CRC(674c0e0f) SHA1(69fc17881c89cc5e82b0fefec49c4116054f9e3b) )
	ROM_LOAD( "sf_rh.bin",    0x10000, 0x8000, CRC(3c42e4a7) SHA1(8e4da1e6e73603e484ba4f5609ac9ea92999a526) )	/* banked at 8000 */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sf_r05.bin",   0x0000,  0x2000, CRC(87f4705a) SHA1(a90d5644ce268f3321047a4f96df96ac294d2f1b) )

	ROM_REGION( 0x0800, "cpu2", 0 )	/* 2k for the microcontroller */
	ROM_LOAD( "68705.bin",    0x0000,  0x0800, NO_DUMP )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "sf_r11.bin",   0x00000, 0x2000, CRC(2ac7b943) SHA1(d0c3560bb1f0c2647aeff807cb4b09450237b955) )  /* Chars */
	ROM_LOAD( "sf_r10.bin",   0x02000, 0x2000, CRC(33cadc93) SHA1(59ffc206c62a651d2ac0ef52f519dd56edf2c021) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD( "sf_r06.bin",   0x00000, 0x8000, CRC(b6358305) SHA1(c7bb4236a75ec6b88f011bc30f8fb9a718e2ca3e) )  /* Tiles */
	ROM_LOAD( "sf_r09.bin",   0x08000, 0x8000, CRC(e92d9d60) SHA1(2554617e0e6615ca8c85a49299a4a0e762478339) )
	ROM_LOAD( "sf_r08.bin",   0x10000, 0x8000, CRC(5faeeea3) SHA1(696fba24bcf1f3a7e914a4403854da5eededaf7f) )
	ROM_LOAD( "sf_r07.bin",   0x18000, 0x8000, CRC(974e2ea9) SHA1(3840550fc3a833828dad8f3e300d2ea583d69ce7) )

	ROM_REGION( 0x20000, "gfx3", 0 )
	ROM_LOAD( "sf_r03.bin",   0x00000, 0x8000, CRC(8545d397) SHA1(9a1fd5bfd8fb830b8e46643c08eef32ba968fc23) )  /* Sprites */
	ROM_LOAD( "sf_r01.bin",   0x08000, 0x8000, CRC(b1b7b925) SHA1(199b0b52bbeb384211171eca5c50a1c0ebf6826f) )
	ROM_LOAD( "sf_r04.bin",   0x10000, 0x8000, CRC(422d946b) SHA1(c251ef9597a11ec8de39be4fcbddaba84e649ef2) )
	ROM_LOAD( "sf_r02.bin",   0x18000, 0x8000, CRC(587113ae) SHA1(90abe961494a1af7c87693a419fbabf7a58a5dee) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "sf_col21.bin", 0x0000,  0x0100, CRC(a0efaf99) SHA1(5df01663480acad1f89abab8662d437617a66d1c) )
	ROM_LOAD( "sf_col20.bin", 0x0100,  0x0100, CRC(a56d57e5) SHA1(bfbd0db52b23fe1b4994e05103be3d412c1c013e) )
	ROM_LOAD( "sf_col19.bin", 0x0200,  0x0100, CRC(5cbf9fbf) SHA1(abfa58fa4e44ebc56f2e0fac9bcc36164c845fa3) )
ROM_END

/* dump labeled Alcon, but GFX in tile roms clearly read Slap Fight */
#if 0
ROM_START( slapfigha )
	ROM_REGION( 0x18000, "maincpu", 0 )
	ROM_LOAD( "a76-00.bin",   0x00000, 0x4000, CRC(ac22bb86) SHA1(3ecff006fc487d494f21adb7bff6f8c56eb5d707) )
	ROM_LOAD( "a76-01.bin",   0x04000, 0x4000, CRC(d6b4f02e) SHA1(37f840c444ba7dcc75810580c9da83289670d5cc) )
	ROM_LOAD( "a76-02.bin",   0x10000, 0x8000, CRC(9dd0971f) SHA1(92bd0b54635bf5c4118a53e0f897c65f5eb2984a) )	/* banked at 8000 */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "a76-03.bin",   0x0000,  0x2000, CRC(87f4705a) SHA1(a90d5644ce268f3321047a4f96df96ac294d2f1b) )

	ROM_REGION( 0x0800, "cpu2", 0 )	/* 2k for the microcontroller */
	ROM_LOAD( "68705.bin",    0x0000,  0x0800, NO_DUMP )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "a76-05.bin",   0x00000, 0x2000, CRC(be9a1bc5) SHA1(2fabfd42cd49db67654eac824c9852ed368a6e50) )  /* Chars */
	ROM_LOAD( "a76-04.bin",   0x02000, 0x2000, CRC(3519daa4) SHA1(ab77cc1bfe7c394d1a90a4c50d5d4a98158eb86d) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD( "a76-09.bin",   0x00000, 0x8000, CRC(b6358305) SHA1(c7bb4236a75ec6b88f011bc30f8fb9a718e2ca3e) )  /* Tiles */
	ROM_LOAD( "a76-08.bin",   0x08000, 0x8000, CRC(e92d9d60) SHA1(2554617e0e6615ca8c85a49299a4a0e762478339) )
	ROM_LOAD( "a76-07.bin",   0x10000, 0x8000, CRC(5faeeea3) SHA1(696fba24bcf1f3a7e914a4403854da5eededaf7f) )
	ROM_LOAD( "a76-06.bin",   0x18000, 0x8000, CRC(974e2ea9) SHA1(3840550fc3a833828dad8f3e300d2ea583d69ce7) )

	ROM_REGION( 0x20000, "gfx3", 0 )
	ROM_LOAD( "a76-13.bin",   0x00000, 0x8000, CRC(8545d397) SHA1(9a1fd5bfd8fb830b8e46643c08eef32ba968fc23) )  /* Sprites */
	ROM_LOAD( "a76-12.bin",   0x08000, 0x8000, CRC(b1b7b925) SHA1(199b0b52bbeb384211171eca5c50a1c0ebf6826f) )
	ROM_LOAD( "a76-11.bin",   0x10000, 0x8000, CRC(422d946b) SHA1(c251ef9597a11ec8de39be4fcbddaba84e649ef2) )
	ROM_LOAD( "a76-10.bin",   0x18000, 0x8000, CRC(587113ae) SHA1(90abe961494a1af7c87693a419fbabf7a58a5dee) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "sf_col21.bin", 0x0000,  0x0100, CRC(a0efaf99) SHA1(5df01663480acad1f89abab8662d437617a66d1c) )
	ROM_LOAD( "sf_col20.bin", 0x0100,  0x0100, CRC(a56d57e5) SHA1(bfbd0db52b23fe1b4994e05103be3d412c1c013e) )
	ROM_LOAD( "sf_col19.bin", 0x0200,  0x0100, CRC(5cbf9fbf) SHA1(abfa58fa4e44ebc56f2e0fac9bcc36164c845fa3) )
ROM_END
#endif
#if 0
ROM_START( alcon )
	ROM_REGION( 0x18000, "maincpu", 0 )
	ROM_LOAD( "00",           0x00000, 0x8000, CRC(2ba82d60) SHA1(b37659aa18a3f96a3cc7fa93db2439f36487b8c8) )
	ROM_LOAD( "01",           0x10000, 0x8000, CRC(18bb2f12) SHA1(7c16d4bbb8b5e22f227aff170e5e6326c5968968) )	/* banked at 8000 */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sf_r05.bin",   0x0000,  0x2000, CRC(87f4705a) SHA1(a90d5644ce268f3321047a4f96df96ac294d2f1b) )

	ROM_REGION( 0x0800, "cpu2", 0 )	/* 2k for the microcontroller */
	ROM_LOAD( "68705.bin",    0x0000,  0x0800, NO_DUMP )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "04",           0x00000, 0x2000, CRC(31003483) SHA1(7014ceb6313ac5a3d2dcb735643dfd8bfabaa185) )  /* Chars */
	ROM_LOAD( "03",           0x02000, 0x2000, CRC(404152c0) SHA1(d05bc9baa1f336475fffc2f19f1018e9f0547f10) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD( "sf_r06.bin",   0x00000, 0x8000, CRC(b6358305) SHA1(c7bb4236a75ec6b88f011bc30f8fb9a718e2ca3e) )  /* Tiles */
	ROM_LOAD( "sf_r09.bin",   0x08000, 0x8000, CRC(e92d9d60) SHA1(2554617e0e6615ca8c85a49299a4a0e762478339) )
	ROM_LOAD( "sf_r08.bin",   0x10000, 0x8000, CRC(5faeeea3) SHA1(696fba24bcf1f3a7e914a4403854da5eededaf7f) )
	ROM_LOAD( "sf_r07.bin",   0x18000, 0x8000, CRC(974e2ea9) SHA1(3840550fc3a833828dad8f3e300d2ea583d69ce7) )

	ROM_REGION( 0x20000, "gfx3", 0 )
	ROM_LOAD( "sf_r03.bin",   0x00000, 0x8000, CRC(8545d397) SHA1(9a1fd5bfd8fb830b8e46643c08eef32ba968fc23) )  /* Sprites */
	ROM_LOAD( "sf_r01.bin",   0x08000, 0x8000, CRC(b1b7b925) SHA1(199b0b52bbeb384211171eca5c50a1c0ebf6826f) )
	ROM_LOAD( "sf_r04.bin",   0x10000, 0x8000, CRC(422d946b) SHA1(c251ef9597a11ec8de39be4fcbddaba84e649ef2) )
	ROM_LOAD( "sf_r02.bin",   0x18000, 0x8000, CRC(587113ae) SHA1(90abe961494a1af7c87693a419fbabf7a58a5dee) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "sf_col21.bin", 0x0000,  0x0100, CRC(a0efaf99) SHA1(5df01663480acad1f89abab8662d437617a66d1c) )
	ROM_LOAD( "sf_col20.bin", 0x0100,  0x0100, CRC(a56d57e5) SHA1(bfbd0db52b23fe1b4994e05103be3d412c1c013e) )
	ROM_LOAD( "sf_col19.bin", 0x0200,  0x0100, CRC(5cbf9fbf) SHA1(abfa58fa4e44ebc56f2e0fac9bcc36164c845fa3) )
ROM_END
#endif
#if 0
ROM_START( slapbtjp )
	ROM_REGION( 0x18000, "maincpu", 0 )
	ROM_LOAD( "sf_r19jb.bin", 0x00000, 0x8000, CRC(9a7ac8b3) SHA1(01fbad9b4fc80f2406eff18db20e196e212d0c17) )
	ROM_LOAD( "sf_rh.bin",    0x10000, 0x8000, CRC(3c42e4a7) SHA1(8e4da1e6e73603e484ba4f5609ac9ea92999a526) )	/* banked at 8000 */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sf_r05.bin",   0x0000,  0x2000, CRC(87f4705a) SHA1(a90d5644ce268f3321047a4f96df96ac294d2f1b) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "sf_r11.bin",   0x00000, 0x2000, CRC(2ac7b943) SHA1(d0c3560bb1f0c2647aeff807cb4b09450237b955) )  /* Chars */
	ROM_LOAD( "sf_r10.bin",   0x02000, 0x2000, CRC(33cadc93) SHA1(59ffc206c62a651d2ac0ef52f519dd56edf2c021) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD( "sf_r06.bin",   0x00000, 0x8000, CRC(b6358305) SHA1(c7bb4236a75ec6b88f011bc30f8fb9a718e2ca3e) )  /* Tiles */
	ROM_LOAD( "sf_r09.bin",   0x08000, 0x8000, CRC(e92d9d60) SHA1(2554617e0e6615ca8c85a49299a4a0e762478339) )
	ROM_LOAD( "sf_r08.bin",   0x10000, 0x8000, CRC(5faeeea3) SHA1(696fba24bcf1f3a7e914a4403854da5eededaf7f) )
	ROM_LOAD( "sf_r07.bin",   0x18000, 0x8000, CRC(974e2ea9) SHA1(3840550fc3a833828dad8f3e300d2ea583d69ce7) )

	ROM_REGION( 0x20000, "gfx3", 0 )
	ROM_LOAD( "sf_r03.bin",   0x00000, 0x8000, CRC(8545d397) SHA1(9a1fd5bfd8fb830b8e46643c08eef32ba968fc23) )  /* Sprites */
	ROM_LOAD( "sf_r01.bin",   0x08000, 0x8000, CRC(b1b7b925) SHA1(199b0b52bbeb384211171eca5c50a1c0ebf6826f) )
	ROM_LOAD( "sf_r04.bin",   0x10000, 0x8000, CRC(422d946b) SHA1(c251ef9597a11ec8de39be4fcbddaba84e649ef2) )
	ROM_LOAD( "sf_r02.bin",   0x18000, 0x8000, CRC(587113ae) SHA1(90abe961494a1af7c87693a419fbabf7a58a5dee) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "sf_col21.bin", 0x0000,  0x0100, CRC(a0efaf99) SHA1(5df01663480acad1f89abab8662d437617a66d1c) )
	ROM_LOAD( "sf_col20.bin", 0x0100,  0x0100, CRC(a56d57e5) SHA1(bfbd0db52b23fe1b4994e05103be3d412c1c013e) )
	ROM_LOAD( "sf_col19.bin", 0x0200,  0x0100, CRC(5cbf9fbf) SHA1(abfa58fa4e44ebc56f2e0fac9bcc36164c845fa3) )
ROM_END
#endif
#if 0
ROM_START( slapbtuk )
	ROM_REGION( 0x18000, "maincpu", 0 )
	ROM_LOAD( "sf_r19eb.bin", 0x00000, 0x4000, CRC(2efe47af) SHA1(69ce3e83a0d8fa5ee4737c741d31cf32db6b9919) )
	ROM_LOAD( "sf_r20eb.bin", 0x04000, 0x4000, CRC(f42c7951) SHA1(d76e7a72f6ced67b550ba68cd42987f7111f5468) )
	ROM_LOAD( "sf_rh.bin",    0x10000, 0x8000, CRC(3c42e4a7) SHA1(8e4da1e6e73603e484ba4f5609ac9ea92999a526) )	/* banked at 8000 */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sf_r05.bin",   0x0000,  0x2000, CRC(87f4705a) SHA1(a90d5644ce268f3321047a4f96df96ac294d2f1b) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "sf_r11.bin",   0x00000, 0x2000, CRC(2ac7b943) SHA1(d0c3560bb1f0c2647aeff807cb4b09450237b955) )  /* Chars */
	ROM_LOAD( "sf_r10.bin",   0x02000, 0x2000, CRC(33cadc93) SHA1(59ffc206c62a651d2ac0ef52f519dd56edf2c021) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD( "sf_r06.bin",   0x00000, 0x8000, CRC(b6358305) SHA1(c7bb4236a75ec6b88f011bc30f8fb9a718e2ca3e) )  /* Tiles */
	ROM_LOAD( "sf_r09.bin",   0x08000, 0x8000, CRC(e92d9d60) SHA1(2554617e0e6615ca8c85a49299a4a0e762478339) )
	ROM_LOAD( "sf_r08.bin",   0x10000, 0x8000, CRC(5faeeea3) SHA1(696fba24bcf1f3a7e914a4403854da5eededaf7f) )
	ROM_LOAD( "sf_r07.bin",   0x18000, 0x8000, CRC(974e2ea9) SHA1(3840550fc3a833828dad8f3e300d2ea583d69ce7) )

	ROM_REGION( 0x20000, "gfx3", 0 )
	ROM_LOAD( "sf_r03.bin",   0x00000, 0x8000, CRC(8545d397) SHA1(9a1fd5bfd8fb830b8e46643c08eef32ba968fc23) )  /* Sprites */
	ROM_LOAD( "sf_r01.bin",   0x08000, 0x8000, CRC(b1b7b925) SHA1(199b0b52bbeb384211171eca5c50a1c0ebf6826f) )
	ROM_LOAD( "sf_r04.bin",   0x10000, 0x8000, CRC(422d946b) SHA1(c251ef9597a11ec8de39be4fcbddaba84e649ef2) )
	ROM_LOAD( "sf_r02.bin",   0x18000, 0x8000, CRC(587113ae) SHA1(90abe961494a1af7c87693a419fbabf7a58a5dee) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "sf_col21.bin", 0x0000,  0x0100, CRC(a0efaf99) SHA1(5df01663480acad1f89abab8662d437617a66d1c) )
	ROM_LOAD( "sf_col20.bin", 0x0100,  0x0100, CRC(a56d57e5) SHA1(bfbd0db52b23fe1b4994e05103be3d412c1c013e) )
	ROM_LOAD( "sf_col19.bin", 0x0200,  0x0100, CRC(5cbf9fbf) SHA1(abfa58fa4e44ebc56f2e0fac9bcc36164c845fa3) )
ROM_END
#endif
/* very similar to slapbtuk, is slapbtuk missing the logo rom? */
ROM_START( slapfgtr )
	ROM_REGION( 0x18000, "maincpu", 0 )
	ROM_LOAD( "k1-10.u90",    0x00000, 0x4000, CRC(2efe47af) SHA1(69ce3e83a0d8fa5ee4737c741d31cf32db6b9919) )
	ROM_LOAD( "k1-09.u89",    0x04000, 0x4000, CRC(17c187c5) SHA1(6e1fd651f56036d1c6c830de8479df25fc182c10) )
	ROM_LOAD( "k1-08.u88",    0x0e000, 0x2000, CRC(945af97f) SHA1(4d901be477b6101338eb1d86497e1bdc57f9c1b4) ) // contains another copy of the logo tilemap read at ec00!
	ROM_LOAD( "k1-07.u87",    0x10000, 0x8000, CRC(3c42e4a7) SHA1(8e4da1e6e73603e484ba4f5609ac9ea92999a526) )	/* banked at 8000 */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "k1-11.u89",    0x0000,  0x2000, CRC(87f4705a) SHA1(a90d5644ce268f3321047a4f96df96ac294d2f1b) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "k1-02.u57",    0x00000, 0x2000, CRC(2ac7b943) SHA1(d0c3560bb1f0c2647aeff807cb4b09450237b955) )  /* Chars */
	ROM_LOAD( "k1-03.u58",    0x02000, 0x2000, CRC(33cadc93) SHA1(59ffc206c62a651d2ac0ef52f519dd56edf2c021) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD( "k1-01.u49",    0x00000, 0x8000, CRC(b6358305) SHA1(c7bb4236a75ec6b88f011bc30f8fb9a718e2ca3e) )  /* Tiles */
	ROM_LOAD( "k1-04.u62",    0x08000, 0x8000, CRC(e92d9d60) SHA1(2554617e0e6615ca8c85a49299a4a0e762478339) )
	ROM_LOAD( "k1-05.u63",    0x10000, 0x8000, CRC(5faeeea3) SHA1(696fba24bcf1f3a7e914a4403854da5eededaf7f) )
	ROM_LOAD( "k1-06.u64",    0x18000, 0x8000, CRC(974e2ea9) SHA1(3840550fc3a833828dad8f3e300d2ea583d69ce7) )

	ROM_REGION( 0x20000, "gfx3", 0 )
	ROM_LOAD( "k1-15.u60",    0x00000, 0x8000, CRC(8545d397) SHA1(9a1fd5bfd8fb830b8e46643c08eef32ba968fc23) )  /* Sprites */
	ROM_LOAD( "k1-13.u50",    0x08000, 0x8000, CRC(b1b7b925) SHA1(199b0b52bbeb384211171eca5c50a1c0ebf6826f) )
	ROM_LOAD( "k1-14.u59",    0x10000, 0x8000, CRC(422d946b) SHA1(c251ef9597a11ec8de39be4fcbddaba84e649ef2) )
	ROM_LOAD( "k1-12.u49",    0x18000, 0x8000, CRC(587113ae) SHA1(90abe961494a1af7c87693a419fbabf7a58a5dee) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "sf_col21.bin", 0x0000,  0x0100, CRC(a0efaf99) SHA1(5df01663480acad1f89abab8662d437617a66d1c) )
	ROM_LOAD( "sf_col20.bin", 0x0100,  0x0100, CRC(a56d57e5) SHA1(bfbd0db52b23fe1b4994e05103be3d412c1c013e) )
	ROM_LOAD( "sf_col19.bin", 0x0200,  0x0100, CRC(5cbf9fbf) SHA1(abfa58fa4e44ebc56f2e0fac9bcc36164c845fa3) )
ROM_END

ROM_START( getstar )
	ROM_REGION( 0x18000, "maincpu", 0 )		/* Region 0 - main cpu code */
	ROM_LOAD( "rom0",         0x00000, 0x4000, CRC(6a8bdc6c) SHA1(c923bca539bd2eb9a34cb9c7a67a199e28bc081a) )
	ROM_LOAD( "rom1",         0x04000, 0x4000, CRC(ebe8db3c) SHA1(9046d6e63c33fc9cbd48b90dcbcc0badf1d3b9ba) )
	ROM_LOAD( "rom2",         0x10000, 0x8000, CRC(343e8415) SHA1(00b98055277a0ddfb7d0bda6537df10a4049533e) )

	ROM_REGION( 0x10000, "audiocpu", 0 )		/* Region 3 - sound cpu code */
	ROM_LOAD( "a68-03",       0x0000,  0x2000, CRC(18daa44c) SHA1(1a3d22a186c591321d1b836ee30d89fba4771122) )

	ROM_REGION( 0x0800, "cpu2", 0 )	/* 2k for the microcontroller */
	ROM_LOAD( "68705.bin",    0x0000,  0x0800, NO_DUMP )

	ROM_REGION( 0x04000, "gfx1", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_05-1",     0x00000, 0x2000, CRC(06f60107) SHA1(c5dcf0c7a5863ea960ee747d2d7ec7ac8bb7d3af) )  /* Chars */
	ROM_LOAD( "a68_04-1",     0x02000, 0x2000, CRC(1fc8f277) SHA1(59dc1a0fad23b1e98abca3d0b1685b9d2939b059) )

	ROM_REGION( 0x20000, "gfx2", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_09",       0x00000, 0x8000, CRC(a293cc2e) SHA1(a2c2598e92982d13b51cbb6efb4b963142233433) )  /* Tiles */
	ROM_LOAD( "a68_08",       0x08000, 0x8000, CRC(37662375) SHA1(46ba8a3f0b553d476ecf431d0d20556896b4ca43) )
	ROM_LOAD( "a68_07",       0x10000, 0x8000, CRC(cf1a964c) SHA1(e9223c8d4f3bdafed193a1ded63e377f16f45e17) )
	ROM_LOAD( "a68_06",       0x18000, 0x8000, CRC(05f9eb9a) SHA1(a71640a63b259799086d361ef293aa26cec46a0c) )

	ROM_REGION( 0x20000, "gfx3", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68-13",       0x00000, 0x8000, CRC(643fb282) SHA1(d904d3c27c2b56341929c5eed4ea97e948c53c34) )  /* Sprites */
	ROM_LOAD( "a68-12",       0x08000, 0x8000, CRC(11f74e32) SHA1(02d8b4cc679f45a02c4989f2b62cde91b7418235) )
	ROM_LOAD( "a68-11",       0x10000, 0x8000, CRC(f24158cf) SHA1(db4c6b68a488b0798ea5f793ac8ced283a8ecab2) )
	ROM_LOAD( "a68-10",       0x18000, 0x8000, CRC(83161ed0) SHA1(a6aa28f22f487dc3a2ec07935e6d42bcdd1eff81) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "rom21",        0x0000,  0x0100, CRC(d6360b4d) SHA1(3e64548c82a3378fc091e104cdc2b0c7e592fc44) )
	ROM_LOAD( "rom20",        0x0100,  0x0100, CRC(4ca01887) SHA1(2892c89d5e60f1d10593adffff55c1a9654e8209) )
	ROM_LOAD( "rom19",        0x0200,  0x0100, CRC(513224f0) SHA1(15b34612206138f6fc5f7478925b1fff2ed56aa8) )
ROM_END

#if 0
ROM_START( getstarj )
	ROM_REGION( 0x18000, "maincpu", 0 )		/* Region 0 - main cpu code */
	ROM_LOAD( "a68_00.bin",   0x00000, 0x4000, CRC(ad1a0143) SHA1(0d9adeb12bd4d5ad11e5bada0cd7498bc565c1db) )
	ROM_LOAD( "a68_01.bin",   0x04000, 0x4000, CRC(3426eb7c) SHA1(e91db45a650a1bfefd7c12c7553b647bc916c7c8) )
	ROM_LOAD( "a68_02.bin",   0x10000, 0x8000, CRC(3567da17) SHA1(29d698606d0bd30abfc3171d79bfad95b0de89fc) )

	ROM_REGION( 0x10000, "audiocpu", 0 )		/* Region 3 - sound cpu code */
	ROM_LOAD( "a68-03",       0x00000, 0x2000, CRC(18daa44c) SHA1(1a3d22a186c591321d1b836ee30d89fba4771122) )

	ROM_REGION( 0x0800, "cpu2", 0 )	/* 2k for the microcontroller */
	ROM_LOAD( "68705.bin",    0x0000,  0x0800, NO_DUMP )

	ROM_REGION( 0x04000, "gfx1", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_05.bin",   0x00000, 0x2000, CRC(e3d409e7) SHA1(0b6be4767f110729f4dd1a472ef8d9a0c718b684) )  /* Chars */
	ROM_LOAD( "a68_04.bin",   0x02000, 0x2000, CRC(6e5ac9d4) SHA1(74f90b7a1ceb3b1c2fd92dff100d92dea0155530) )

	ROM_REGION( 0x20000, "gfx2", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_09",       0x00000, 0x8000, CRC(a293cc2e) SHA1(a2c2598e92982d13b51cbb6efb4b963142233433) )  /* Tiles */
	ROM_LOAD( "a68_08",       0x08000, 0x8000, CRC(37662375) SHA1(46ba8a3f0b553d476ecf431d0d20556896b4ca43) )
	ROM_LOAD( "a68_07",       0x10000, 0x8000, CRC(cf1a964c) SHA1(e9223c8d4f3bdafed193a1ded63e377f16f45e17) )
	ROM_LOAD( "a68_06",       0x18000, 0x8000, CRC(05f9eb9a) SHA1(a71640a63b259799086d361ef293aa26cec46a0c) )

	ROM_REGION( 0x20000, "gfx3", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68-13",       0x00000, 0x8000, CRC(643fb282) SHA1(d904d3c27c2b56341929c5eed4ea97e948c53c34) )  /* Sprites */
	ROM_LOAD( "a68-12",       0x08000, 0x8000, CRC(11f74e32) SHA1(02d8b4cc679f45a02c4989f2b62cde91b7418235) )
	ROM_LOAD( "a68-11",       0x10000, 0x8000, CRC(f24158cf) SHA1(db4c6b68a488b0798ea5f793ac8ced283a8ecab2) )
	ROM_LOAD( "a68-10",       0x18000, 0x8000, CRC(83161ed0) SHA1(a6aa28f22f487dc3a2ec07935e6d42bcdd1eff81) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "rom21",        0x0000,  0x0100, CRC(d6360b4d) SHA1(3e64548c82a3378fc091e104cdc2b0c7e592fc44) )
	ROM_LOAD( "rom20",        0x0100,  0x0100, CRC(4ca01887) SHA1(2892c89d5e60f1d10593adffff55c1a9654e8209) )
	ROM_LOAD( "rom19",        0x0200,  0x0100, CRC(513224f0) SHA1(15b34612206138f6fc5f7478925b1fff2ed56aa8) )
ROM_END
#endif
#if 0
ROM_START( gtstarb1 )
	ROM_REGION( 0x18000, "maincpu", 0 )		/* Region 0 - main cpu code */
	ROM_LOAD( "gs_rb_1.bin",  0x00000, 0x4000, CRC(9afad7e0) SHA1(6b2e82a6b7fcbfed5f4d250959ecc571fdf0cbc2) )
	ROM_LOAD( "gs_rb_2.bin",  0x04000, 0x4000, CRC(5feb0a60) SHA1(b1300055180ddf6ca96475eb3a27a17722273fc6) )
	ROM_LOAD( "gs_rb_3.bin",  0x10000, 0x8000, CRC(e3cfb1ba) SHA1(bd21655c82a14e18ff9df4539c4d0bb2484c73f1) )

	ROM_REGION( 0x10000, "audiocpu", 0 )		/* Region 3 - sound cpu code */
	ROM_LOAD( "a68-03",       0x00000, 0x2000, CRC(18daa44c) SHA1(1a3d22a186c591321d1b836ee30d89fba4771122) )

	ROM_REGION( 0x04000, "gfx1", 0 )	/* Region 1 - temporary for gfx */
	/* these roms were in the set, but they're corrupt */
//  ROM_LOAD( "gs_rb_8.bin",  0x00000, 0x2000, CRC(a30aaf04) SHA1(2509554c3851a68eaec1cadc01f4d69c7aa2c09d) )  /* Chars */
//  ROM_LOAD( "gs_rb_7.bin",  0x02000, 0x2000, CRC(f47a93c6) SHA1(441fee1fb195bb2583d220f30dfcff617a31742a) )
	/* use the original roms instead */
	ROM_LOAD( "a68_05.bin",   0x00000, 0x2000, CRC(e3d409e7) SHA1(0b6be4767f110729f4dd1a472ef8d9a0c718b684) )  /* Chars */
	ROM_LOAD( "a68_04.bin",   0x02000, 0x2000, CRC(6e5ac9d4) SHA1(74f90b7a1ceb3b1c2fd92dff100d92dea0155530) )


	ROM_REGION( 0x20000, "gfx2", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_09",       0x00000, 0x8000, CRC(a293cc2e) SHA1(a2c2598e92982d13b51cbb6efb4b963142233433) )  /* Tiles */
	ROM_LOAD( "a68_08",       0x08000, 0x8000, CRC(37662375) SHA1(46ba8a3f0b553d476ecf431d0d20556896b4ca43) )
	ROM_LOAD( "a68_07",       0x10000, 0x8000, CRC(cf1a964c) SHA1(e9223c8d4f3bdafed193a1ded63e377f16f45e17) )
	ROM_LOAD( "a68_06",       0x18000, 0x8000, CRC(05f9eb9a) SHA1(a71640a63b259799086d361ef293aa26cec46a0c) )

	ROM_REGION( 0x20000, "gfx3", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68-13",       0x00000, 0x8000, CRC(643fb282) SHA1(d904d3c27c2b56341929c5eed4ea97e948c53c34) )  /* Sprites */
	ROM_LOAD( "a68-12",       0x08000, 0x8000, CRC(11f74e32) SHA1(02d8b4cc679f45a02c4989f2b62cde91b7418235) )
	ROM_LOAD( "a68-11",       0x10000, 0x8000, CRC(f24158cf) SHA1(db4c6b68a488b0798ea5f793ac8ced283a8ecab2) )
	ROM_LOAD( "a68-10",       0x18000, 0x8000, CRC(83161ed0) SHA1(a6aa28f22f487dc3a2ec07935e6d42bcdd1eff81) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "rom21",        0x0000,  0x0100, CRC(d6360b4d) SHA1(3e64548c82a3378fc091e104cdc2b0c7e592fc44) )
	ROM_LOAD( "rom20",        0x0100,  0x0100, CRC(4ca01887) SHA1(2892c89d5e60f1d10593adffff55c1a9654e8209) )
	ROM_LOAD( "rom19",        0x0200,  0x0100, CRC(513224f0) SHA1(15b34612206138f6fc5f7478925b1fff2ed56aa8) )
ROM_END
#endif
#if 0
ROM_START( gtstarb2 )
	ROM_REGION( 0x18000, "maincpu", 0 )		/* Region 0 - main cpu code */
	ROM_LOAD( "gs_14.rom",    0x00000, 0x4000, CRC(1a57a920) SHA1(b1e9d5b29c0e3632eec3ad1ee51bf3392e4b816d) )
	ROM_LOAD( "gs_13.rom",    0x04000, 0x4000, CRC(805f8e77) SHA1(c3ad6eae842d2d10f716998d5a803038fa7b338f) )
	ROM_LOAD( "a68_02.bin",   0x10000, 0x8000, CRC(3567da17) SHA1(29d698606d0bd30abfc3171d79bfad95b0de89fc) )

	ROM_REGION( 0x10000, "audiocpu", 0 )		/* Region 3 - sound cpu code */
	ROM_LOAD( "a68-03",       0x0000,  0x2000, CRC(18daa44c) SHA1(1a3d22a186c591321d1b836ee30d89fba4771122) )

	ROM_REGION( 0x04000, "gfx1", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_05.bin",   0x00000, 0x2000, CRC(e3d409e7) SHA1(0b6be4767f110729f4dd1a472ef8d9a0c718b684) )  /* Chars */
	ROM_LOAD( "a68_04.bin",   0x02000, 0x2000, CRC(6e5ac9d4) SHA1(74f90b7a1ceb3b1c2fd92dff100d92dea0155530) )

	ROM_REGION( 0x20000, "gfx2", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68_09",       0x00000, 0x8000, CRC(a293cc2e) SHA1(a2c2598e92982d13b51cbb6efb4b963142233433) )  /* Tiles */
	ROM_LOAD( "a68_08",       0x08000, 0x8000, CRC(37662375) SHA1(46ba8a3f0b553d476ecf431d0d20556896b4ca43) )
	ROM_LOAD( "a68_07",       0x10000, 0x8000, CRC(cf1a964c) SHA1(e9223c8d4f3bdafed193a1ded63e377f16f45e17) )
	ROM_LOAD( "a68_06",       0x18000, 0x8000, CRC(05f9eb9a) SHA1(a71640a63b259799086d361ef293aa26cec46a0c) )

	ROM_REGION( 0x20000, "gfx3", 0 )	/* Region 1 - temporary for gfx */
	ROM_LOAD( "a68-13",       0x00000, 0x8000, CRC(643fb282) SHA1(d904d3c27c2b56341929c5eed4ea97e948c53c34) )  /* Sprites */
	ROM_LOAD( "a68-12",       0x08000, 0x8000, CRC(11f74e32) SHA1(02d8b4cc679f45a02c4989f2b62cde91b7418235) )
	ROM_LOAD( "a68-11",       0x10000, 0x8000, CRC(f24158cf) SHA1(db4c6b68a488b0798ea5f793ac8ced283a8ecab2) )
	ROM_LOAD( "a68-10",       0x18000, 0x8000, CRC(83161ed0) SHA1(a6aa28f22f487dc3a2ec07935e6d42bcdd1eff81) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "rom21",        0x0000,  0x0100, CRC(d6360b4d) SHA1(3e64548c82a3378fc091e104cdc2b0c7e592fc44) )
	ROM_LOAD( "rom20",        0x0100,  0x0100, CRC(4ca01887) SHA1(2892c89d5e60f1d10593adffff55c1a9654e8209) )
	ROM_LOAD( "rom19",        0x0200,  0x0100, CRC(513224f0) SHA1(15b34612206138f6fc5f7478925b1fff2ed56aa8) )
ROM_END
#endif

static DRIVER_INIT( tigerh )
{
	memory_install_readwrite8_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe803, 0xe803, 0, 0, tigerh_mcu_r, tigerh_mcu_w  );
}

#if 0
static DRIVER_INIT( tigerhb )
{
	memory_install_readwrite8_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe803, 0xe803, 0, 0, tigerhb_e803_r, tigerhb_e803_w  );
}
#endif
#if 0
static READ8_HANDLER( gtstarb1_port_0_read )
{
	/* The bootleg has it's own 'protection' on startup ?
        6D1A: 06 04         ld   b,$04
        6D1C: DB 00         in   a,($00)
        6D1E: E6 06         and  $06
        6D20: 20 FA         jr   nz,$6D1C
        6D22: DB 00         in   a,($00)
        6D24: E6 06         and  $06
        6D26: FE 06         cp   $06
        6D28: 20 F8         jr   nz,$6D22
        6D2A: DB 00         in   a,($00)
        6D2C: E6 06         and  $06
        6D2E: FE 02         cp   $02
        6D30: 20 F8         jr   nz,$6D2A
        6D32: DB 00         in   a,($00)
        6D34: E6 06         and  $06
        6D36: FE 04         cp   $04
        6D38: 20 F8         jr   nz,$6D32
        6D3A: 10 E0         djnz $6D1C
    */
	if (cpu_get_pc(space->cpu) == 0x6d1e) return 0;
	if (cpu_get_pc(space->cpu) == 0x6d24) return 6;
	if (cpu_get_pc(space->cpu) == 0x6d2c) return 2;
	if (cpu_get_pc(space->cpu) == 0x6d34) return 4;

	/* The bootleg hangs in the "test mode" before diplaying (wrong) lives settings :
        6AD4: DB 00         in   a,($00)
        6AD6: CB 4F         bit  1,a
        6AD8: 28 FA         jr   z,$6AD4
        6ADA: 3E 23         ld   a,$23
        6ADC: CD 52 11      call $1152
        6ADF: 32 03 E8      ld   ($E803),a
        6AE2: DB 00         in   a,($00)
        6AE4: CB 4F         bit  1,a
        6AE6: 28 FA         jr   z,$6AE2
        6AE8: 3A 0A C8      ld   a,($C80A)
        6AEB: E6 03         and  $03
        6AED: CD 52 11      call $1152
        6AF0: 32 03 E8      ld   ($E803),a
        6AF3: DB 00         in   a,($00)
        6AF5: CB 57         bit  2,a
        6AF7: 20 FA         jr   nz,$6AF3
       This seems to be what used to be the MCU status.
    */
	if (cpu_get_pc(space->cpu) == 0x6ad6) return 2; /* bit 1 must be ON */
	if (cpu_get_pc(space->cpu) == 0x6ae4) return 2; /* bit 1 must be ON */
	if (cpu_get_pc(space->cpu) == 0x6af5) return 0; /* bit 2 must be OFF */

	logerror("Port Read PC=%04x\n",cpu_get_pc(space->cpu));

	return 0;
}
#endif

static void getstar_init( running_machine *machine )
{
	memory_install_readwrite8_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe803, 0xe803, 0, 0, getstar_e803_r, getstar_e803_w  );
}

static DRIVER_INIT( getstar )
{
	getstar_id = GETSTAR;
	getstar_init(machine);
}

#if 0
static DRIVER_INIT( getstarj )
{
	getstar_id = GETSTARJ;
	getstar_init(machine);
}
#endif
#if 0
static DRIVER_INIT( gtstarb1 )
{
	UINT8 *ROM = memory_region(machine, "maincpu");

	getstar_id = GTSTARB1;
	getstar_init(machine);

	/* specific handlers for this bootleg */
	memory_install_read8_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_IO), 0x0, 0x0, 0, 0, gtstarb1_port_0_read );
	/* requires this or it gets stuck with 'rom test' on screen */
	/* it is possible the program roms are slighly corrupt like the gfx roms, or
       that the bootleg simply shouldn't execute the code due to the modified roms */
	/* TODO: find & fix the cause of the following happening. */
	ROM[0x6d56] = 0xc3; //jp instead of jp z
}
#endif
#if 0
static DRIVER_INIT( gtstarb2 )
{
	getstar_id = GTSTARB2;
	getstar_init(machine);
}
#endif

static int slapfigh_prot_pos;

static READ8_HANDLER( slapfigh_mcu_r )
{
	/* pass initial checks */
	static const int protvalues[] = { 0xc7, 0x55, -1 };

	if ((cpu_get_pc(space->cpu)==0x1369) || // slapfigh
		(cpu_get_pc(space->cpu)==0x136d)) // slapfiga
	{
		int retdat = protvalues[slapfigh_prot_pos];
		if (retdat == -1)
		{
			slapfigh_prot_pos = 0;
			retdat = protvalues[slapfigh_prot_pos];
		}

		slapfigh_prot_pos++;
		return retdat;
	}
	logerror("MCU Read PC=%04x\n",cpu_get_pc(space->cpu));
	return 0;
}

static DRIVER_INIT( slapfigh )
{
	slapfigh_prot_pos = 0;
	memory_install_read8_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe803, 0xe803, 0, 0, slapfigh_mcu_r );
//  memory_install_write8_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe803, 0xe803, 0, 0, getstar_mcu_w  );
}


/*   ( YEAR  NAME      PARENT    MACHINE   INPUT     INIT    MONITOR  COMPANY    FULLNAME     FLAGS ) */
//GAME( 1985, perfrman, 0,        perfrman, perfrman, 0,        ROT270, "[Toaplan] Data East Corporation", "Performan (Japan)", 0 )
//GAME( 1985, perfrmanu,perfrman, perfrman, perfrman, 0,        ROT270, "[Toaplan] Data East USA",         "Performan (US)", 0 )
GAME( 1985, tigerh,   0,        tigerh,   tigerh,   tigerh,   ROT270, "Taito America Corp.", "Tiger Heli (US)", GAME_NO_COCKTAIL )
//GAME( 1985, tigerhj,  tigerh,   tigerh,   tigerh,   tigerh,   ROT270, "Taito",   "Tiger Heli (Japan)", GAME_NO_COCKTAIL )
//GAME( 1985, tigerhb1, tigerh,   tigerhb,  tigerh,   tigerhb,  ROT270, "bootleg", "Tiger Heli (bootleg set 1)", GAME_NO_COCKTAIL )
//GAME( 1985, tigerhb2, tigerh, 	tigerhb,  tigerh,   0,        ROT270, "bootleg", "Tiger Heli (bootleg set 2)", GAME_NO_COCKTAIL )
//GAME( 1985, tigerhb3, tigerh,	tigerhb,  tigerh,   0,        ROT270, "bootleg", "Tiger Heli (bootleg set 3)", GAME_NO_COCKTAIL )
GAME( 1986, slapfigh, 0,        slapfigh, slapfigh, slapfigh, ROT270, "Taito",   "Slap Fight (set 1)", GAME_NOT_WORKING | GAME_NO_COCKTAIL )
//GAME( 1986, slapfigha,slapfigh, slapfigh, slapfigh, slapfigh, ROT270, "Taito",   "Slap Fight (set 2)", GAME_NOT_WORKING | GAME_NO_COCKTAIL )
//GAME( 1986, alcon,    slapfigh, slapfigh, slapfigh, slapfigh, ROT270, "Taito America Corp.", "Alcon", GAME_NOT_WORKING | GAME_NO_COCKTAIL )
//GAME( 1986, slapbtjp, slapfigh, slapfigh, slapfigh, 0,        ROT270, "bootleg", "Slap Fight (Japan bootleg)", GAME_NO_COCKTAIL )
//GAME( 1986, slapbtuk, slapfigh, slapbtuk, slapfigh, 0,        ROT270, "bootleg", "Slap Fight (English bootleg)", GAME_NO_COCKTAIL )
GAME( 1986, slapfgtr, slapfigh, slapbtuk, slapfigh, 0,        ROT270, "bootleg", "Slap Fight (bootleg)", GAME_NO_COCKTAIL ) // PCB labeled 'slap fighter'
GAME( 1986, getstar,  0,        slapfigh, getstar,  getstar,  ROT0,   "Taito America Corporation (Kitkorp license)", "Guardian (US)", GAME_NO_COCKTAIL )
//GAME( 1986, getstarj, getstar,  slapfigh, getstarj, getstarj, ROT0,   "Taito",   "Get Star (Japan)", GAME_NO_COCKTAIL )
//GAME( 1986, gtstarb1, getstar,  slapfigh, getstarj, gtstarb1, ROT0,   "bootleg", "Get Star (bootleg set 1)", GAME_NO_COCKTAIL )
//GAME( 1986, gtstarb2, getstar,  slapfigh, gtstarb2, gtstarb2, ROT0,   "bootleg", "Get Star (bootleg set 2)", GAME_NO_COCKTAIL )
