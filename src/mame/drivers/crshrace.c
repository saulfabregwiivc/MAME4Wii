/***************************************************************************

Crash Race       (c) 1993 Video System Co.

driver by Nicola Salmoria

Notes:
- Keep player1 button1&2 pressed while entering service mode to get an
  extended menu


Stephh's notes (based on the games M68000 code and some tests) :

0) all games

  - There seems to be preliminary support for 3 simulataneous players, but the
    game resets before the race starts if the 3 players don't play against each
    other ! I can't tell however if it's an ingame or an emulation bug.
    To test this, change CRSHRACE_3P_HACK to 1, set the "Reset on P.O.S.T. Error"
    Dip Switch to "No" (because of the ROMS patch), and set the "Maximum Players"
    Dip Switch to "3".

  - There are 2 buttons for each player (one for accel and one for brake),
    the 3rd one being for "debug" purpose (see notes below).
  - The "Difficulty" Dip Switch also determines the time to complete the race.
  - "Coin B" Dip Switches only has an effect if you set the "Coin Slot"
    Dip Switch to "Same".
    If you set it to "Individual", it will use the coinage from "Coin A".
  - COIN3 adds 1 credit only if you set the "Coin Slot" Dip Switch to "Same".
    If you set it to "Individual", it will add 1 credit to fake player 3,
    thus having no effect.

  - DSW 3 bit 0 used to be a "Max Players" Dip Switch (but it is now unused) :
      * when Off, 2 players cabinet
      * when On,  3 players cabinet

  - DSW 3 bits 1 to 3 used to be a "Coin C" Dip Switch (but they are now unused)
    which is in fact similar to the table for "Coin A" and "Coin B" :
       1   2   3      Coinage
      Off Off Off      1C_1C
      On  Off Off      2C_1C
      Off On  Off      3C_1C
      On  On  Off      1C_2C
      Off Off On       1C_3C
      On  Off On       1C_4C
      Off On  On       1C_5C
      On  On  On       1C_6C

  - DSW 3 bit 7 is tested only if an error has occured during P.O.S.T. :
      * when Off, the game is reset
      * when On,  don't bother with the error and continue

  - There are NO differences between Country code 0x0004 ("World") and 0x0005.
    Country code is stored at 0xfe1c9e and can have the following values :
      * 0000 : Japan
      * 0001 : USA & Canada
      * 0002 : Korea
      * 0003 : Hong Kong & Taiwan
      * 0004 : World
      * 0005 : ???

  - When in the "test mode" with the extended menu, pressing "P1 button 3"
    causes a "freeze"; press it again to unfreeze.
  - When in the "test mode" with the extended menu, pressing "P2 button 3"
    has an unknown effect (sound related ?), but sets bit 2 at 0xfe0019.

  - There are writes to 0xfff00c and 0xfff00d, but these addresses aren't mapped :
      * when "Flip Screen" Dip Switch is Off, 0x0001 is written to 0xfff00c.w
      * when "Flip Screen" Dip Switch is Off, 0xc001 is written to 0xfff00c.w
    I can't tell however what is the effect of these writes 8(


1) 'crshrace'

  - Even if there is code for it, there is NO possibility to select a 3 players
    game due to code at 0x003778 which "invalidates" the previous reading of DSW 3 :

    00363C: 13F8 F00B 00FE 1C85      move.b  $f00b.w, $fe1c85.l
    ...
    003650: 4639 00FE 1C85           not.b   $fe1c85.l
    ...
    003778: 51F9 00FE 1C85           sf      $fe1c85.l

  - When in the "test mode" with the extended menu, pressing "P1 start" +
    "P2 start" + the 3 buttons of the SAME player causes a reset of the game
    (code at 0x003182).
  - When in the "test play" menu of the "test mode", pressing "P1 button 1" +
    "P1 button 2" + "P2 button 1" + "P2 button 2" + "P2 button 3" returns
    to the "test mode" (code at 0x0040de).


2) 'crshrac2'

  - Even if there is code for it, there is NO possibility to select a 3 players
    game due to code at 0x003796 which "invalidates" the previous reading of DSW 3 :

    00365A: 13F8 F00B 00FE 1C85      move.b  $f00b.w, $fe1c85.l
    ...
    00366E: 4639 00FE 1C85           not.b   $fe1c85.l
    ...
    003796: 51F9 00FE 1C85           sf      $fe1c85.l

  - When in the "test mode" with the extended menu, pressing "P1 start" +
    "P2 start" + the 3 buttons of the SAME player causes a reset of the game
    (code at 0x0031a0).
  - When in the "test play" menu of the "test mode", pressing "P1 button 1" +
    "P1 button 2" + "P2 button 1" + "P2 button 2" + "P2 button 3" returns
    to the "test mode" (code at 0x0040fc).

  - I can't determine the effect of DSW 1 bit 4 8( All I can tell is that code
    at 0x00ea9c is called when initialising the race "parameters".


TODO:
- handle screen flip correctly
- sprite lag - I think it needs sprites to be delayed TWO frames
- is bg color in service mode right (blue)? Should it be black instead?
- handling of layer priority & enable might not be correct, though it should be
  enough to run this game.
- unknown writes to fff044/fff046. They look like two more scroll registers,
  but for what? The first starts at 0 when going over the start line and
  increases during the race

2008-08
Dip locations verified with Service Mode.

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "cpu/m68000/m68000.h"
#include "video/konamiic.h"
#include "sound/2610intf.h"
#include "crshrace.h"


#define CRSHRACE_3P_HACK	0


static READ16_HANDLER( extrarom1_r )
{
	UINT8 *rom = memory_region(space->machine, "user1");

	offset *= 2;

	return rom[offset] | (rom[offset+1] << 8);
}

static READ16_HANDLER( extrarom2_r )
{
	UINT8 *rom = memory_region(space->machine, "user2");

	offset *= 2;

	return rom[offset] | (rom[offset+1] << 8);
}

static WRITE8_HANDLER( crshrace_sh_bankswitch_w )
{
	UINT8 *rom = memory_region(space->machine, "audiocpu") + 0x10000;

	memory_set_bankptr(space->machine, 1,rom + (data & 0x03) * 0x8000);
}


static int pending_command;

static WRITE16_HANDLER( sound_command_w )
{
	if (ACCESSING_BITS_0_7)
	{
		pending_command = 1;
		soundlatch_w(space,offset,data & 0xff);
		cputag_set_input_line(space->machine, "audiocpu", INPUT_LINE_NMI, PULSE_LINE);
	}
}

static CUSTOM_INPUT( country_sndpending_r )
{
	return pending_command;
}

static WRITE8_HANDLER( pending_command_clear_w )
{
	pending_command = 0;
}



static ADDRESS_MAP_START( crshrace_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x300000, 0x3fffff) AM_READ(extrarom1_r)
	AM_RANGE(0x400000, 0x4fffff) AM_READ(extrarom2_r)
	AM_RANGE(0x500000, 0x5fffff) AM_READ(extrarom2_r)	/* mirror */
	AM_RANGE(0xa00000, 0xa0ffff) AM_RAM AM_BASE(&spriteram16_2) AM_SIZE(&spriteram_2_size)
	AM_RANGE(0xd00000, 0xd01fff) AM_RAM_WRITE(crshrace_videoram1_w) AM_BASE(&crshrace_videoram1)
	AM_RANGE(0xe00000, 0xe01fff) AM_RAM AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0xfe0000, 0xfeffff) AM_RAM
	AM_RANGE(0xffc000, 0xffc001) AM_WRITE(crshrace_roz_bank_w)
	AM_RANGE(0xffd000, 0xffdfff) AM_RAM_WRITE(crshrace_videoram2_w) AM_BASE(&crshrace_videoram2)
	AM_RANGE(0xffe000, 0xffefff) AM_RAM_WRITE(paletteram16_xGGGGGBBBBBRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0xfff000, 0xfff001) AM_READ_PORT("P1") AM_WRITE(crshrace_gfxctrl_w)
	AM_RANGE(0xfff002, 0xfff003) AM_READ_PORT("P2")
	AM_RANGE(0xfff004, 0xfff005) AM_READ_PORT("DSW0")
	AM_RANGE(0xfff006, 0xfff007) AM_READ_PORT("DSW2")
	AM_RANGE(0xfff008, 0xfff009) AM_WRITE(sound_command_w)
	AM_RANGE(0xfff00a, 0xfff00b) AM_READ_PORT("DSW1")
	AM_RANGE(0xfff00e, 0xfff00f) AM_READ_PORT("P3")
	AM_RANGE(0xfff020, 0xfff03f) AM_WRITE(SMH_RAM) AM_BASE(&K053936_0_ctrl)
	AM_RANGE(0xfff044, 0xfff047) AM_WRITE(SMH_RAM)	// ??? moves during race
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x77ff) AM_ROM
	AM_RANGE(0x7800, 0x7fff) AM_RAM
	AM_RANGE(0x8000, 0xffff) AM_ROMBANK(1)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_WRITE(crshrace_sh_bankswitch_w)
	AM_RANGE(0x04, 0x04) AM_READWRITE(soundlatch_r, pending_command_clear_w)
	AM_RANGE(0x08, 0x0b) AM_DEVREADWRITE("ym", ym2610_r, ym2610_w)
ADDRESS_MAP_END


static INPUT_PORTS_START( crshrace )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)	// "Accel"
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)	// "Brake"
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_SERVICE2 )				// "Test"
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_SERVICE1 )

	PORT_START("P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)	// "Accel"
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)	// "Brake"
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW0")
	/* DSW2 : 0xfe1c84 = !(0xfff005) */
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Free_Play ) ) PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_SERVICE_DIPLOC( 0x0008, IP_ACTIVE_LOW, "SW2:4" )
	PORT_DIPUNUSED_DIPLOC( 0x0010, 0x0010, "SW2:5" )
	PORT_DIPUNUSED_DIPLOC( 0x0020, 0x0020, "SW2:6" )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:7,8")
	PORT_DIPSETTING(      0x0080, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x00c0, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	/* DSW1 : 0xfe1c83 = !(0xfff004) */
	PORT_DIPNAME( 0x0100, 0x0100, "Coin Slot" ) PORT_DIPLOCATION("SW1:1")
	PORT_DIPSETTING(      0x0100, "Same" )
	PORT_DIPSETTING(      0x0000, "Individual" )
	PORT_DIPNAME( 0x0e00, 0x0e00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:2,3,4") PORT_CONDITION("DSW0", 0x0100, PORTCOND_EQUALS, 0x0100)
	PORT_DIPSETTING(      0x0a00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x7000, 0x7000, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:5,6,7") PORT_CONDITION("DSW0", 0x0100, PORTCOND_EQUALS, 0x0100)
	PORT_DIPSETTING(      0x5000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x6000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x7000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x3000, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0e00, 0x0e00, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:2,3,4") PORT_CONDITION("DSW0", 0x0100, PORTCOND_NOTEQUALS, 0x0100)
	PORT_DIPSETTING(      0x0a00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPUNUSED_DIPLOC( 0x7000, 0x7000, "SW1:5,6,7") PORT_CONDITION("DSW0", 0x0100, PORTCOND_NOTEQUALS, 0x0100)
	PORT_DIPNAME( 0x8000, 0x8000, "2 to Start, 1 to Cont." ) PORT_DIPLOCATION("SW1:8")	// Other desc. was too long !
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("DSW1")
	/* DSW3 : 0xfe1c85 = !(0xfff00b) */
#if CRSHRACE_3P_HACK
	PORT_DIPNAME( 0x0001, 0x0001, "Maximum Players" ) PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(      0x0001, "2" )
	PORT_DIPSETTING(      0x0000, "3" )
	PORT_DIPNAME( 0x000e, 0x000e, "Coin C" ) PORT_DIPLOCATION("SW3:2,3,4")
	PORT_DIPSETTING(      0x000a, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x000e, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
#else
	PORT_DIPUNUSED_DIPLOC( 0x0001, 0x0001, "SW3:1" )
	PORT_DIPUNUSED_DIPLOC( 0x0002, 0x0002, "SW3:2" )
	PORT_DIPUNUSED_DIPLOC( 0x0004, 0x0004, "SW3:3" )
	PORT_DIPUNUSED_DIPLOC( 0x0008, 0x0008, "SW3:4" )
#endif
	PORT_DIPUNUSED_DIPLOC( 0x0010, 0x0010, "SW3:5" )
	PORT_DIPUNUSED_DIPLOC( 0x0020, 0x0020, "SW3:6" )
	PORT_DIPUNUSED_DIPLOC( 0x0040, 0x0040, "SW3:7" )
	PORT_DIPNAME( 0x0080, 0x0080, "Reset on P.O.S.T. Error" ) PORT_DIPLOCATION("SW3:8")	// Check code at 0x003812
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Yes ) )

	PORT_START("P3")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START3 )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x0f00, 0x0100, "Country" )
	PORT_DIPSETTING(      0x0100, DEF_STR( World ) )
	PORT_DIPSETTING(      0x0800, "USA & Canada" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Japan ) )
	PORT_DIPSETTING(      0x0200, "Korea" )
	PORT_DIPSETTING(      0x0400, "Hong Kong & Taiwan" )
/*
    the following are all the same and seem to act like the World setting, possibly
    with a slightly different attract sequence
    PORT_DIPSETTING(      0x0300, "5" )
    PORT_DIPSETTING(      0x0500, "5" )
    PORT_DIPSETTING(      0x0600, "5" )
    PORT_DIPSETTING(      0x0700, "5" )
    PORT_DIPSETTING(      0x0900, "5" )
    PORT_DIPSETTING(      0x0a00, "5" )
    PORT_DIPSETTING(      0x0b00, "5" )
    PORT_DIPSETTING(      0x0c00, "5" )
    PORT_DIPSETTING(      0x0d00, "5" )
    PORT_DIPSETTING(      0x0e00, "5" )
    PORT_DIPSETTING(      0x0f00, "5" )
*/
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(country_sndpending_r, NULL)	/* pending sound command */
INPUT_PORTS_END

/* Same as 'crshrace', but additional "unknown" Dip Switch (see notes) */
#if 0
static INPUT_PORTS_START( crshrac2 )
	PORT_INCLUDE( crshrace )

	PORT_MODIFY("DSW0")
	PORT_DIPUNKNOWN_DIPLOC( 0x0010, 0x0010, "SW2:5" )		// Check code at 0x00ea36
INPUT_PORTS_END
#endif

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64 },
	64*8
};

static const gfx_layout tilelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4,
			10*4, 11*4, 8*4, 9*4, 14*4, 15*4, 12*4, 13*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
			8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 1*4, 0*4, 3*4, 2*4, 5*4, 4*4, 7*4, 6*4,
			9*4, 8*4, 11*4, 10*4, 13*4, 12*4, 15*4, 14*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
			8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8
};

static GFXDECODE_START( crshrace )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,     0,  1 )
	GFXDECODE_ENTRY( "gfx2", 0, tilelayout,   256, 16 )
	GFXDECODE_ENTRY( "gfx3", 0, spritelayout, 512, 32 )
GFXDECODE_END



static void irqhandler(const device_config *device, int irq)
{
	cputag_set_input_line(device->machine, "audiocpu", 0, irq ? ASSERT_LINE : CLEAR_LINE);
}

static const ym2610_interface ym2610_config =
{
	irqhandler
};



static MACHINE_DRIVER_START( crshrace )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000,16000000)	/* 16 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(crshrace_map)
	MDRV_CPU_VBLANK_INT("screen", irq1_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80,4000000)	/* 4 MHz ??? */
	MDRV_CPU_PROGRAM_MAP(sound_map)
	MDRV_CPU_IO_MAP(sound_io_map)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)

	MDRV_GFXDECODE(crshrace)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(crshrace)
	MDRV_VIDEO_EOF(crshrace)
	MDRV_VIDEO_UPDATE(crshrace)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("ym", YM2610, 8000000)
	MDRV_SOUND_CONFIG(ym2610_config)
	MDRV_SOUND_ROUTE(0, "lspeaker",  0.25)
	MDRV_SOUND_ROUTE(0, "rspeaker", 0.25)
	MDRV_SOUND_ROUTE(1, "lspeaker",  1.0)
	MDRV_SOUND_ROUTE(2, "rspeaker", 1.0)
MACHINE_DRIVER_END


ROM_START( crshrace )
	ROM_REGION( 0x80000, "maincpu", 0 )	/* 68000 code */
	ROM_LOAD16_WORD_SWAP( "1",            0x000000, 0x80000, CRC(21e34fb7) SHA1(be47b4a9bce2d6ce0a127dffe032c61547b2a3c0) )

	ROM_REGION( 0x100000, "user1", 0 )	/* extra ROM */
	ROM_LOAD( "w21",          0x000000, 0x100000, CRC(a5df7325) SHA1(614095a086164af5b5e73245744411187d81deec) )

	ROM_REGION( 0x100000, "user2", 0 )	/* extra ROM */
	ROM_LOAD( "w22",          0x000000, 0x100000, CRC(fc9d666d) SHA1(45aafcce82b668f93e51b5e4d092b1d0077e5192) )

	ROM_REGION( 0x30000, "audiocpu", 0 )	/* 64k for the audio CPU + banks */
	ROM_LOAD( "2",            0x00000, 0x20000, CRC(e70a900f) SHA1(edfe5df2dab5a7dccebe1a6f978144bcd516ab03) )
	ROM_RELOAD(               0x10000, 0x20000 )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD( "h895",         0x000000, 0x100000, CRC(36ad93c3) SHA1(f68f229dd1a1f8bfd3b8f73b6627f5f00f809d34) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "w18",          0x000000, 0x100000, CRC(b15df90d) SHA1(56e38e6c40a02553b6b8c5282aa8f16b20779ebf) )
	ROM_LOAD( "w19",          0x100000, 0x100000, CRC(28326b93) SHA1(997e9b250b984b012ce1d165add59c741fb18171) )
	ROM_LOAD( "w20",          0x200000, 0x100000, CRC(d4056ad1) SHA1(4b45b14aa0766d7aef72f060e1cd28d67690d5fe) )
	/* 300000-3fffff empty */

	ROM_REGION( 0x400000, "gfx3", 0 )
	ROM_LOAD( "h897",         0x000000, 0x200000, CRC(e3230128) SHA1(758c65f113481cf25bf0359deecd6736a7c9ee7e) )
	ROM_LOAD( "h896",         0x200000, 0x200000, CRC(fff60233) SHA1(56b4b708883a80761dc5f9184780477d72b80351) )

	ROM_REGION( 0x100000, "ym.deltat", 0 ) /* sound samples */
	ROM_LOAD( "h894",         0x000000, 0x100000, CRC(d53300c1) SHA1(4c3ff7d3156791cb960c28845a5f1906605bce55) )

	ROM_REGION( 0x100000, "ym", 0 ) /* sound samples */
	ROM_LOAD( "h893",         0x000000, 0x100000, CRC(32513b63) SHA1(c4ede4aaa2611cedb53d47448422a1926acf3052) )
ROM_END

#if 0
ROM_START( crshrace2 )
	ROM_REGION( 0x80000, "maincpu", 0 )	/* 68000 code */
	ROM_LOAD16_WORD_SWAP( "01-ic10.bin",  0x000000, 0x80000, CRC(b284aacd) SHA1(f0ef279cdec30eb32e8aa8cdd51e289b70f2d6f5) )

	ROM_REGION( 0x100000, "user1", 0 )	/* extra ROM */
	ROM_LOAD( "w21",          0x000000, 0x100000, CRC(a5df7325) SHA1(614095a086164af5b5e73245744411187d81deec) )	// IC14.BIN

	ROM_REGION( 0x100000, "user2", 0 )	/* extra ROM */
	ROM_LOAD( "w22",          0x000000, 0x100000, CRC(fc9d666d) SHA1(45aafcce82b668f93e51b5e4d092b1d0077e5192) )	// IC13.BIN

	ROM_REGION( 0x30000, "audiocpu", 0 )	/* 64k for the audio CPU + banks */
	ROM_LOAD( "2",            0x00000, 0x20000, CRC(e70a900f) SHA1(edfe5df2dab5a7dccebe1a6f978144bcd516ab03) )	// 02-IC58.BIN
	ROM_RELOAD(               0x10000, 0x20000 )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD( "h895",         0x000000, 0x100000, CRC(36ad93c3) SHA1(f68f229dd1a1f8bfd3b8f73b6627f5f00f809d34) )	// IC50.BIN

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "w18",          0x000000, 0x100000, CRC(b15df90d) SHA1(56e38e6c40a02553b6b8c5282aa8f16b20779ebf) )	// ROM-A.BIN
	ROM_LOAD( "w19",          0x100000, 0x100000, CRC(28326b93) SHA1(997e9b250b984b012ce1d165add59c741fb18171) )	// ROM-B.BIN
	ROM_LOAD( "w20",          0x200000, 0x100000, CRC(d4056ad1) SHA1(4b45b14aa0766d7aef72f060e1cd28d67690d5fe) )	// ROM-C.BIN
	/* 300000-3fffff empty */

	ROM_REGION( 0x400000, "gfx3", 0 )
	ROM_LOAD( "h897",         0x000000, 0x200000, CRC(e3230128) SHA1(758c65f113481cf25bf0359deecd6736a7c9ee7e) )	// IC29.BIN
	ROM_LOAD( "h896",         0x200000, 0x200000, CRC(fff60233) SHA1(56b4b708883a80761dc5f9184780477d72b80351) )	// IC75.BIN

	ROM_REGION( 0x100000, "ym.deltat", 0 ) /* sound samples */
	ROM_LOAD( "h894",         0x000000, 0x100000, CRC(d53300c1) SHA1(4c3ff7d3156791cb960c28845a5f1906605bce55) )	// IC73.BIN

	ROM_REGION( 0x100000, "ym", 0 ) /* sound samples */
	ROM_LOAD( "h893",         0x000000, 0x100000, CRC(32513b63) SHA1(c4ede4aaa2611cedb53d47448422a1926acf3052) )	// IC69.BIN
ROM_END
#endif

#ifdef UNUSED_FUNCTION
void crshrace_patch_code(UINT16 offset)
{
	/* A hack which shows 3 player mode in code which is disabled */
	UINT16 *RAM = (UINT16 *)memory_region(machine, "maincpu");
	RAM[(offset + 0)/2] = 0x4e71;
	RAM[(offset + 2)/2] = 0x4e71;
	RAM[(offset + 4)/2] = 0x4e71;
}
#endif


static DRIVER_INIT( crshrace )
{
	#if CRSHRACE_3P_HACK
	crshrace_patch_code(0x003778);
	#endif
}

#if 0
static DRIVER_INIT( crshrac2 )
{
	#if CRSHRACE_3P_HACK
	crshrace_patch_code(0x003796);
	#endif
}
#endif

GAME( 1993, crshrace, 0,        crshrace, crshrace, crshrace, ROT270, "Video System Co.", "Lethal Crash Race (set 1)", GAME_NO_COCKTAIL )
//GAME( 1993, crshrace2,crshrace, crshrace, crshrac2, crshrac2, ROT270, "Video System Co.", "Lethal Crash Race (set 2)", GAME_NO_COCKTAIL )

