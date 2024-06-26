/***************************************************************************

Crazy Climber memory map (preliminary)
as described by Lionel Theunissen (lionelth@ozemail.com.au)

Crazy Kong is very similar to Crazy Climber, there is an additional ROM at
5000-5fff and RAM is at 6000-6bff. Dip switches and input connections are
different as well.

Swimmer is similar but also different (e.g. it has two CPUs and two 8910,
graphics are 3bpp instead of 2)

0000h-4fffh ;20k program ROMs. ROM11=0000h
                               ROM10=1000h
                               ROM09=2000h
                               ROM08=3000h
                               ROM07=4000h

8000h-83ffh ;1k scratchpad RAM.
8800h-88ffh ;256 bytes Bigsprite RAM.
9000h-93ffh ;1k screen RAM.
9800h-981fh ;Column smooth scroll position. Corresponds to each char
             column.

9880h-989fh ;Sprite controls. 8 groups of 4 bytes:
  1st byte; code/attribute.
            Bits 0-5: sprite code.
            Bit    6: x invert.
            Bit    7: y invert.
  2nd byte ;color.
            Bits 0-3: colour. (palette scheme 0-15)
            Bit    4: 0=charset1, 1 =charset 2.
  3rd byte ;y position
  4th byte ;x position

98dc        bit 0  big sprite priority over sprites? (1 = less priority)
98ddh ;Bigsprite colour/attribute.
            Bit 0-2: Big sprite colour.
            bit 3  ??
            Bit   4: x invert.
            Bit   5: y invert.
98deh ;Bigsprite y position.
98dfh ;Bigsprite x position.

9c00h-9fffh ;1/2k colour RAM: Bits 0-3: colour. (palette scheme 0-15)
                              Bit    4: 0=charset1, 1=charset2.
                              Bit    5: (not used by CC)
                              Bit    6: x invert.
                              Bit    7: y invert. (not used by CC)

a000h ;RD: Player 1 controls.
            Bit 0: Left up
                1: Left down
                2: Left left
                3: Left right
                4: Right up
                5: Right down
                6: Right left
                7: Right right

a000h ;WR: Non Maskable interrupt.
            Bit 0: 0=NMI disable, 1=NMI enable.

a001h ;WR: Horizontal video direction (Crazy Kong sets it to 1).
            Bit 0: 0=Normal, 1=invert.

a002h ;WR: Vertical video direction (Crazy Kong sets it to 1).
            Bit 0: 0=Normal, 1=invert.

a004h ;WR: Sample trigger.
            Bit 0: 0=Trigger.

a800h ;RD: Player 2 controls (table model only).
            Bit 0: Left up
                1: Left down
                2: Left left
                3: Left right
                4: Right up
                5: Right down
                6: Right left
                7: Right right


a800h ;WR: Sample rate speed.
              Full byte value (0-255).

b000h ;RD: DIP switches.
            Bit 1,0: Number of climbers.
                     00=3, 01=4, 10=5, 11=6.
            Bit   2: Extra climber bonus.
                     0=30000, 1=50000.
            Bit   3: 1=Test Pattern
            Bit 5,4: Coins per credit.
                     00=1, 01=2, 10=3 11=4.
            Bit 7,6: Plays per credit.
                     00=1, 01=2, 10=3, 11=Freeplay.

b000h ;WR: Sample volume.
            Bits 0-5: Volume (0-31).

b800h ;RD: Machine switches.
            Bit 0: Coin 1.
            Bit 1: Coin 2.
            Bit 2: 1 Player start.
            Bit 3: 2 Player start.
            Bit 4: Upright/table select.
                   0=table, 1=upright.


I/O 8  ;AY-3-8910 Control Reg.
I/O 9  ;AY-3-8910 Data Write Reg.
I/O C  ;AY-3-8910 Data Read Reg.
        Port A of the 8910 selects the digital sample to play

Changes:
25 Jan 98 LBO
        * Added support for the real Swimmer bigsprite ROMs, courtesy of Gary Walton.
        * Increased the IRQs for the Swimmer audio CPU to 4 to make it more "jaunty".
          Not sure if this is accurate, but it should be closer.
3 Mar 98 LBO
        * Added alternate version of Swimmer.

TODO:
        * Verify timings of sound/music on Swimmer.


-------------------------------------------------------------------

    T.S. 17.12.2005:

    Yamato:
    -------
     Added temporary bg gradient (bad colors/offset).

     Gradient table are stored in two(?) ROMs.
     Each table is 256 bytes long: 128 for normal
     and 128 bytes for flipped screen.
     Color format is unknown - probably direct RGB
     mapping of 8 or 16 (both roms) bits. Also table
     selection source is unknown.

     TODO:
      - bg gradient color decode & table selection


 Top Roller:
 ----------
     It's made by the same developers as Yamato and use
     probably the same encrypted SEGA cpu as Yamato.

     lives - $6155

     TODO:

       - COINB DSW is missing
       - few issues in cocktail mode
       - wrong colors (fg text layer) - game sometimes ("round" text , lives) updates only even columns of cell attribs...

-------------------------------------------------------------------


 Top Roller
 Jaleco

 Hardware : Original Jaleco board no 8307-B/8307-A(redump)

 Main CPU : Encrypted Z80 (probably 315-5018)
 Sound : AY-3-8910

 ROMS CRC32 + positions :

 [9894374d]  d5
 [ef789f00]  f5
 [d45494ba]  h5
 [1cb48ea0]  k5
 [84139f46]  l5
 [e30c1dd8]  m5
 [904fffb6]  d3
 [94371cfb]  f3
 [8a8032a7]  h3
 [1e8914a6]  k3
 [b20a9fa2]  l3
 [7f989dc9]  p3
 [89327329]  a4 bottom board 89327329
 [7a945733]  c4 bottom board
 [5f2c2a78]  h4 bottom board  bad dump / [1d9e3325] (8307-A)
 [ce3afe26]  j4 bottom board

----

2008-07
Dip location verified from manual for: cclimber, guzzler, swimmer

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "machine/segacrpt.h"
#include "sound/ay8910.h"
#include "sound/samples.h"
#include "includes/cclimber.h"


#define MASTER_CLOCK			XTAL_18_432MHz

static UINT8 yamato_p0;
static UINT8 yamato_p1;
static UINT8 toprollr_rombank;


static WRITE8_HANDLER( swimmer_sh_soundlatch_w )
{
	soundlatch_w(space,offset,data);
	cputag_set_input_line_and_vector(space->machine, "audiocpu", 0, HOLD_LINE, 0xff);
}


static WRITE8_HANDLER( yamato_p0_w )
{
	yamato_p0 = data;
}

static WRITE8_HANDLER( yamato_p1_w )
{
	yamato_p1 = data;
}

static READ8_HANDLER( yamato_p0_r )
{
	return yamato_p0;
}

static READ8_HANDLER( yamato_p1_r )
{
	return yamato_p1;
}


static WRITE8_HANDLER(toprollr_rombank_w)
{
	toprollr_rombank &= ~(1 << offset);
	toprollr_rombank |= (data & 1) << offset;

	if (toprollr_rombank < 3)
		memory_set_bank(space->machine, 1, toprollr_rombank);
}


static TIMER_CALLBACK( disable_interrupts )
{
	cpu_interrupt_enable(cputag_get_cpu(machine, "maincpu"), 0);
}


static MACHINE_RESET( cclimber )
{
	/* Disable interrupts, River Patrol / Silver Land needs this */

	/* we must do this on a timer in order to have it take effect */
	/* otherwise, the reset process will override our changes */
	timer_call_after_resynch(machine, NULL, 0, disable_interrupts);

	toprollr_rombank = 0;
}



/* Note that River Patrol reads/writes to a000-a4f0. This is a bug in the code.
   The instruction at 0x0593 should say LD DE,$8000 */

static ADDRESS_MAP_START( cclimber_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x6bff) AM_RAM				/* Crazy Kong only */
	AM_RANGE(0x8000, 0x83ff) AM_RAM
	AM_RANGE(0x8800, 0x88ff) AM_RAM AM_BASE(&cclimber_bigsprite_videoram)
	AM_RANGE(0x8900, 0x8bff) AM_RAM				/* not used, but initialized */
	AM_RANGE(0x9000, 0x93ff) AM_MIRROR(0x0400) AM_RAM AM_BASE(&cclimber_videoram)
	/* 9800-9bff and 9c00-9fff share the same RAM, interleaved */
	/* (9800-981f for scroll, 9c20-9c3f for color RAM, and so on) */
	AM_RANGE(0x9800, 0x981f) AM_RAM AM_BASE(&cclimber_column_scroll)
	AM_RANGE(0x9880, 0x989f) AM_RAM AM_BASE(&cclimber_spriteram)
	AM_RANGE(0x98dc, 0x98df) AM_RAM AM_BASE(&cclimber_bigsprite_control)
	AM_RANGE(0x9800, 0x9bff) AM_RAM  /* not used, but initialized */
	AM_RANGE(0x9c00, 0x9fff) AM_RAM_WRITE(cclimber_colorram_w) AM_BASE(&cclimber_colorram)
	AM_RANGE(0xa000, 0xa000) AM_READ_PORT("P1") AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xa001, 0xa002) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_flip_screen)
	AM_RANGE(0xa003, 0xa003) AM_WRITE(interrupt_enable_w) //used by Crazy Kong Bootleg with alt levels and speed up
	AM_RANGE(0xa004, 0xa004) AM_WRITE(cclimber_sample_trigger_w)
	AM_RANGE(0xa800, 0xa800) AM_READ_PORT("P2") AM_WRITE(cclimber_sample_rate_w)
	AM_RANGE(0xb000, 0xb000) AM_READ_PORT("DSW") AM_WRITE(cclimber_sample_volume_w)
	AM_RANGE(0xb800, 0xb800) AM_READ_PORT("SYSTEM")
ADDRESS_MAP_END

static ADDRESS_MAP_START( cannonb_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x5045, 0x505f) AM_WRITENOP		/* do not errorlog this */
	AM_RANGE(0x0000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x6bff) AM_RAM
	AM_RANGE(0x8000, 0x83ff) AM_RAM
	AM_RANGE(0x8800, 0x88ff) AM_READWRITE(SMH_NOP, SMH_RAM) AM_BASE(&cclimber_bigsprite_videoram) /* must not return what's written (game will reset after coin insert if it returns 0xff)*/
//  AM_RANGE(0x8900, 0x8bff) AM_WRITE(SMH_RAM)  /* not used, but initialized */
	AM_RANGE(0x9000, 0x93ff) AM_MIRROR(0x0400) AM_RAM AM_BASE(&cclimber_videoram)
	/* 9800-9bff and 9c00-9fff share the same RAM, interleaved */
	/* (9800-981f for scroll, 9c20-9c3f for color RAM, and so on) */
	AM_RANGE(0x9800, 0x981f) AM_RAM AM_BASE(&cclimber_column_scroll)
	AM_RANGE(0x9880, 0x989f) AM_RAM AM_BASE(&cclimber_spriteram)
	AM_RANGE(0x98dc, 0x98df) AM_RAM AM_BASE(&cclimber_bigsprite_control)
	AM_RANGE(0x9800, 0x9bff) AM_RAM  /* not used, but initialized */
	AM_RANGE(0x9c00, 0x9fff) AM_RAM_WRITE(cclimber_colorram_w) AM_BASE(&cclimber_colorram)
	AM_RANGE(0xa000, 0xa000) AM_READ_PORT("P1") AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xa001, 0xa002) AM_WRITE(cannonb_flip_screen_w) AM_BASE(&cclimber_flip_screen)
	AM_RANGE(0xa004, 0xa004) AM_WRITE(cclimber_sample_trigger_w)
	AM_RANGE(0xa800, 0xa800) AM_READ_PORT("P2") AM_WRITE(cclimber_sample_rate_w)
	AM_RANGE(0xb000, 0xb000) AM_READ_PORT("DSW") AM_WRITE(cclimber_sample_volume_w)
	AM_RANGE(0xb800, 0xb800) AM_READ_PORT("SYSTEM")
ADDRESS_MAP_END

static ADDRESS_MAP_START( swimmer_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0x8800, 0x88ff) AM_MIRROR(0x0100) AM_RAM AM_BASE(&cclimber_bigsprite_videoram)
	AM_RANGE(0x9000, 0x93ff) AM_MIRROR(0x0400) AM_RAM AM_BASE(&cclimber_videoram)
	AM_RANGE(0x9800, 0x981f) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_column_scroll)
	AM_RANGE(0x9880, 0x989f) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_spriteram)
	AM_RANGE(0x98fc, 0x98ff) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_bigsprite_control)
	AM_RANGE(0x9c00, 0x9fff) AM_RAM_WRITE(cclimber_colorram_w) AM_BASE(&cclimber_colorram)
	AM_RANGE(0xa000, 0xa000) AM_READ_PORT("P2") AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xa001, 0xa002) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_flip_screen)
	AM_RANGE(0xa003, 0xa003) AM_WRITE(SMH_RAM) AM_BASE(&swimmer_side_background_enabled)
	AM_RANGE(0xa004, 0xa004) AM_WRITE(SMH_RAM) AM_BASE(&swimmer_palettebank)
	AM_RANGE(0xa800, 0xa800) AM_READ_PORT("P1") AM_WRITE(swimmer_sh_soundlatch_w)
	AM_RANGE(0xb000, 0xb000) AM_READ_PORT("DSW1")
	AM_RANGE(0xb800, 0xb800) AM_READ_PORT("DSW2") AM_WRITE(SMH_RAM) AM_BASE(&swimmer_background_color)
	AM_RANGE(0xb880, 0xb880) AM_READ_PORT("SYSTEM")
	AM_RANGE(0xc000, 0xc7ff) AM_RAM					/* ??? used by Guzzler */
	AM_RANGE(0xe000, 0xffff) AM_ROM					/* Guzzler only */
ADDRESS_MAP_END

static ADDRESS_MAP_START( yamato_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x6fff) AM_RAM
	AM_RANGE(0x7000, 0x7fff) AM_ROM
	AM_RANGE(0x8800, 0x88ff) AM_RAM AM_BASE(&cclimber_bigsprite_videoram)
	AM_RANGE(0x8900, 0x8bff) AM_RAM				/* not used, but initialized */
	AM_RANGE(0x9000, 0x93ff) AM_MIRROR(0x0400) AM_RAM AM_BASE(&cclimber_videoram)
	/* 9800-9bff and 9c00-9fff share the same RAM, interleaved */
	/* (9800-981f for scroll, 9c20-9c3f for color RAM, and so on) */
	AM_RANGE(0x9800, 0x981f) AM_RAM AM_BASE(&cclimber_column_scroll)
	AM_RANGE(0x9880, 0x989f) AM_RAM AM_BASE(&cclimber_spriteram)
	AM_RANGE(0x98dc, 0x98df) AM_RAM AM_BASE(&cclimber_bigsprite_control)
	AM_RANGE(0x9800, 0x9bff) AM_RAM  /* not used, but initialized */
	AM_RANGE(0x9c00, 0x9fff) AM_RAM_WRITE(cclimber_colorram_w) AM_BASE(&cclimber_colorram)
	AM_RANGE(0xa000, 0xa000) AM_READ_PORT("P1") AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xa001, 0xa002) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_flip_screen)
	AM_RANGE(0xa800, 0xa800) AM_READ_PORT("P2")
	AM_RANGE(0xb000, 0xb000) AM_READ_PORT("DSW")
	AM_RANGE(0xb800, 0xb800) AM_READ_PORT("COIN")
	AM_RANGE(0xba00, 0xba00) AM_READ_PORT("START")	/* maybe a mirror of b800 */
ADDRESS_MAP_END

static ADDRESS_MAP_START( toprollr_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_READ(SMH_BANK(1))
	AM_RANGE(0x6000, 0x6bff) AM_RAM
	AM_RANGE(0x8800, 0x88ff) AM_RAM AM_BASE(&cclimber_bigsprite_videoram)
	AM_RANGE(0x8c00, 0x8fff) AM_RAM AM_BASE(&toprollr_bg_videoram)
	AM_RANGE(0x9000, 0x93ff) AM_RAM AM_BASE(&cclimber_videoram)
	AM_RANGE(0x9400, 0x97ff) AM_RAM AM_BASE(&toprollr_bg_coloram)
	AM_RANGE(0x9800, 0x987f) AM_RAM /* unused ? */
	AM_RANGE(0x9880, 0x995f) AM_RAM AM_BASE(&cclimber_spriteram)
	AM_RANGE(0x99dc, 0x99df) AM_RAM AM_BASE(&cclimber_bigsprite_control)
	AM_RANGE(0x9c00, 0x9fff) AM_RAM AM_BASE(&cclimber_colorram)
	AM_RANGE(0xa000, 0xa000) AM_READ_PORT("P1") AM_WRITE(interrupt_enable_w)
	AM_RANGE(0xa001, 0xa002) AM_WRITE(SMH_RAM) AM_BASE(&cclimber_flip_screen)
	AM_RANGE(0xa004, 0xa004) AM_WRITE(cclimber_sample_trigger_w)
	AM_RANGE(0xa005, 0xa006) AM_WRITE(toprollr_rombank_w)
	AM_RANGE(0xa800, 0xa800) AM_READ_PORT("P2") AM_WRITE(cclimber_sample_rate_w)
	AM_RANGE(0xb000, 0xb000) AM_READ_PORT("DSW") AM_WRITE(cclimber_sample_volume_w)
	AM_RANGE(0xb800, 0xb800) AM_READ_PORT("SYSTEM")
	AM_RANGE(0xc000, 0xffff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( cclimber_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x08, 0x09) AM_DEVWRITE("ay", ay8910_address_data_w)
	AM_RANGE(0x0c, 0x0c) AM_DEVREAD("ay", ay8910_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( yamato_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_WRITE(yamato_p0_w)	/* ??? */
	AM_RANGE(0x01, 0x01) AM_WRITE(yamato_p1_w)	/* ??? */
ADDRESS_MAP_END


static ADDRESS_MAP_START( swimmer_audio_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_ROM
	AM_RANGE(0x2000, 0x23ff) AM_RAM
	AM_RANGE(0x3000, 0x3000) AM_READ(soundlatch_r)
	AM_RANGE(0x4000, 0x4001) AM_RAM				/* ??? */
ADDRESS_MAP_END

static ADDRESS_MAP_START( yamato_audio_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_ROM
	AM_RANGE(0x5000, 0x53ff) AM_RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( swimmer_audio_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVWRITE("ay1", ay8910_data_address_w)
	AM_RANGE(0x80, 0x81) AM_DEVWRITE("ay2", ay8910_data_address_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( yamato_audio_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVWRITE("ay1", ay8910_address_data_w)
	AM_RANGE(0x02, 0x03) AM_DEVWRITE("ay2", ay8910_address_data_w)
	AM_RANGE(0x04, 0x04) AM_READ(yamato_p0_r)	/* ??? */
	AM_RANGE(0x08, 0x08) AM_READ(yamato_p1_r)	/* ??? */
ADDRESS_MAP_END


static INPUT_PORTS_START( cclimber )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_LEFT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_RIGHT ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_RIGHT ) PORT_8WAY PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW:!1,!2")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "SW:!3" )		// Look code at 0x03c4 : 0x8076 is never tested !
	PORT_DIPNAME( 0x08, 0x00, "Rack Test (Cheat)" ) PORT_CODE(KEYCODE_F1) PORT_DIPLOCATION("SW:!4")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW:!5,!6")
	PORT_DIPSETTING(    0x30, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW:!7,!8")	// Also "Bonus Life" due to code at 0x03d4
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )								// Bonus life : 30000 points
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ) )								// Bonus life : 50000 points
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )								// Bonus life : 30000 points
	PORT_DIPSETTING(    0xc0, DEF_STR( Free_Play ) )							// Bonus life : 50000 points

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

/* Same as 'cclimber' but correct "Bonus Life" Dip Switch */
#if 0
static INPUT_PORTS_START( cclimbrj )
	PORT_INCLUDE( cclimber )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW:!3")
	PORT_DIPSETTING(    0x00, "30000" )
	PORT_DIPSETTING(    0x04, "50000" )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( ckong )
	PORT_START("P1")
	PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY

	PORT_START("P2")
	PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "7000" )
	PORT_DIPSETTING(    0x04, "10000" )
	PORT_DIPSETTING(    0x08, "15000" )
	PORT_DIPSETTING(    0x0c, "20000" )
	PORT_DIPNAME( 0x70, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

/* Similar to normal Crazy Kong except for the lives per game */
#if 0
static INPUT_PORTS_START( ckongb )
	PORT_INCLUDE( ckong )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x01, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x03, "4" )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( cannonb )
	PORT_INCLUDE( ckong )

	PORT_MODIFY("P1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY

	PORT_MODIFY("P2")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, "Display" )
	PORT_DIPSETTING(    0x03, "Scores and Progession Bars" )
	PORT_DIPSETTING(    0x01, "Scores only" )
	PORT_DIPSETTING(    0x02, "Progession Bars only" )
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x10, "5" )
	PORT_DIPSETTING(    0x18, "6" )
	PORT_DIPUNUSED( 0x20, IP_ACTIVE_LOW )
	PORT_DIPUNUSED( 0x40, IP_ACTIVE_LOW )
	PORT_DIPUNUSED( 0x80, IP_ACTIVE_LOW )

	PORT_MODIFY("SYSTEM")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME( "Start" )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 ) PORT_NAME( "Select" )
INPUT_PORTS_END

static INPUT_PORTS_START( rpatrol )
	PORT_START("P1")	/* P2 controls but we use cclimber tags */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x3e, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_2WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_COCKTAIL

	PORT_START("P2")	/* P1 controls but we use cclimber tags */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x3e, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_2WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_2WAY

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x08, "5" )
	PORT_DIPSETTING(    0x0c, "6" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x00, "Unknown 1" )  /* Probably unused */
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "Unknown 2" )  /* Probably unused */
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Memory Test" )
	PORT_DIPSETTING(    0x00, "Retry on Error" )
	PORT_DIPSETTING(    0x80, "Stop on Error" )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( swimmer )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
  	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
  	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW A:!1,!2")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "Infinite (Cheat)")
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW A:!3,!4")
	PORT_DIPSETTING(    0x00, "10000" )
	PORT_DIPSETTING(    0x04, "20000" )
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x0c, DEF_STR( None ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW A:!5,!6")
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW A:!7,!8")
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
  	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_6C ) )

	PORT_START("DSW2")
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW B:!1")
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW B:!2")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW B:!3,!4")
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )		// labeled this way for similarities with 'swimmerb'
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )		// labeled this way for similarities with 'swimmerb'
	PORT_DIPSETTING(    0x80, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Hardest ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

/* Same as 'swimmer' but different "Difficulty" Dip Switch */
#if 0
static INPUT_PORTS_START( swimmerb )
	PORT_INCLUDE( swimmer )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW B:!3")
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )	// SW B:!4
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( guzzler )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW A:!1,!2")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "Infinite (Cheat)")
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW A:!3,!4")
	PORT_DIPSETTING(    0x04, "20K, every 50K" )
	PORT_DIPSETTING(    0x00, "30K, every 100K" )
	PORT_DIPSETTING(    0x08, "30K only" )
	PORT_DIPSETTING(    0x0c, DEF_STR( None ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW A:!5,!6")
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW A:!7,!8")
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_6C ) )

	PORT_START("DSW2")
	PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW B:!1")
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x00, "High Score Names" ) PORT_DIPLOCATION("SW B:!2")
	PORT_DIPSETTING(    0x20, "3 Letters" )
	PORT_DIPSETTING(    0x00, "10 Letters" )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW B:!3,!4")
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Hardest ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( yamato )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x1c, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "Every 30000" )
	PORT_DIPSETTING(    0x20, "Every 50000" )
	PORT_DIPNAME( 0x40, 0x00, "Speed" )
	PORT_DIPSETTING(    0x00, "Slow" )
	PORT_DIPSETTING(    0x40, "Fast" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )

	PORT_START("COIN")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN2 )	/* set 1 only */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN3 )	/* set 1 only */
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("START")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( toprollr )
	PORT_START("P1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x1c, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "Every 30000" )
	PORT_DIPSETTING(    0x20, "Every 50000" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN3 )
INPUT_PORTS_END



static const gfx_layout cclimber_charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ 0, RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout cclimber_spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	2,
	{ 0, RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8
};

static const gfx_layout cannonb_charlayout =
{
	8,8,
	512,
	2,
	{ 0, 1024*8*8 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout cannonb_spritelayout =
{
	16,16,
	64,
	2,
	{ 0, 256*16*16 },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8
};

static const gfx_layout swimmer_charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ 0, RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },	     /* characters are upside down */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8     /* every char takes 8 consecutive bytes */
};

static const gfx_layout swimmer_spritelayout =
{
	16,16,
	RGN_FRAC(1,3),
	3,
	{ 0, RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	32*8
};

static GFXDECODE_START( cclimber )
	GFXDECODE_ENTRY( "gfx1", 0x0000, cclimber_charlayout,      0, 16 ) /* characters */
	GFXDECODE_ENTRY( "gfx1", 0x0000, cclimber_spritelayout,    0, 16 ) /* sprites */
	GFXDECODE_ENTRY( "gfx2", 0x0000, cclimber_charlayout,   16*4,  8 ) /* big sprites */
GFXDECODE_END

static GFXDECODE_START( cannonb )
	GFXDECODE_ENTRY( "gfx1", 0x0000, cannonb_charlayout,       0, 16 ) /* characters */
	GFXDECODE_ENTRY( "gfx1", 0x1000, cannonb_spritelayout,     0, 16 ) /* sprites */
	GFXDECODE_ENTRY( "gfx2", 0x0000, cclimber_charlayout,   16*4,  8 ) /* big sprites */
GFXDECODE_END

static GFXDECODE_START( swimmer )
	GFXDECODE_ENTRY( "gfx1", 0x0000, swimmer_charlayout,       0, 32 ) /* characters */
	GFXDECODE_ENTRY( "gfx1", 0x0000, swimmer_spritelayout,     0, 32 ) /* sprites */
	GFXDECODE_ENTRY( "gfx2", 0x0000, swimmer_charlayout,    32*8,  4 ) /* big sprites */
GFXDECODE_END

static GFXDECODE_START( toprollr )
	GFXDECODE_ENTRY( "gfx1", 0x0000, cclimber_charlayout,      0, 16 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, cclimber_spritelayout,    0, 16 )
	GFXDECODE_ENTRY( "gfx2", 0x0000, cclimber_charlayout,   16*4,  8 ) /* big sprites */
	GFXDECODE_ENTRY( "gfx3", 0x0000, cclimber_charlayout,   24*4, 16 )
GFXDECODE_END



static MACHINE_DRIVER_START( root )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, MASTER_CLOCK/3/2)	/* 3.072 MHz */
	MDRV_CPU_PROGRAM_MAP(cclimber_map)
	MDRV_CPU_IO_MAP(cclimber_portmap)
	MDRV_CPU_VBLANK_INT("screen", nmi_line_pulse)

	MDRV_MACHINE_RESET(cclimber)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(cclimber)
	MDRV_PALETTE_LENGTH(16*4+8*4)

	MDRV_PALETTE_INIT(cclimber)
	MDRV_VIDEO_START(cclimber)
	MDRV_VIDEO_UPDATE(cclimber)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( cclimber )

	MDRV_IMPORT_FROM(root)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay", AY8910, MASTER_CLOCK/3/2/2)
	MDRV_SOUND_CONFIG(cclimber_ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD("samples", SAMPLES, 0)
	MDRV_SOUND_CONFIG(cclimber_samples_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( cannonb )

	MDRV_IMPORT_FROM(cclimber)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(cannonb_map)

	/* video hardware */
	MDRV_GFXDECODE(cannonb)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( yamato )

	MDRV_IMPORT_FROM(root)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(yamato_map)
	MDRV_CPU_IO_MAP(yamato_portmap)

	MDRV_CPU_ADD("audiocpu", Z80, 3072000) /* 3.072 MHz ? */
	MDRV_CPU_PROGRAM_MAP(yamato_audio_map)
	MDRV_CPU_IO_MAP(yamato_audio_portmap)

	/* video hardware */
	MDRV_PALETTE_LENGTH(16*4+8*4+256)
	MDRV_PALETTE_INIT(yamato)
	MDRV_VIDEO_UPDATE(yamato)

	/* audio hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 1536000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD("ay2", AY8910, 1536000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( toprollr )

	MDRV_IMPORT_FROM(cclimber)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(toprollr_map)

	/* video hardware */
	MDRV_GFXDECODE(toprollr)
	MDRV_PALETTE_LENGTH(32*5)
	MDRV_PALETTE_INIT(toprollr)

	MDRV_VIDEO_START(toprollr)
	MDRV_VIDEO_UPDATE(toprollr)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( swimmer )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, 3072000)	/* 3.072 MHz */
	MDRV_CPU_PROGRAM_MAP(swimmer_map)
	MDRV_CPU_VBLANK_INT("screen", nmi_line_pulse)

	MDRV_CPU_ADD("audiocpu", Z80,4000000/2)
	MDRV_CPU_PROGRAM_MAP(swimmer_audio_map)
	MDRV_CPU_IO_MAP(swimmer_audio_portmap)
	MDRV_CPU_PERIODIC_INT(nmi_line_pulse, (double)4000000/16384) /* IRQs are triggered by the main CPU */

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(swimmer)
	MDRV_PALETTE_LENGTH(32*8+4*8+1)

	MDRV_PALETTE_INIT(swimmer)
	MDRV_VIDEO_START(swimmer)
	MDRV_VIDEO_UPDATE(swimmer)

	/* audio hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("ay1", AY8910, 4000000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MDRV_SOUND_ADD("ay2", AY8910, 4000000/2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( cclimber )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cc11",         0x0000, 0x1000, CRC(217ec4ff) SHA1(334604c3a051d57440a9d0bfc34b809418ef1d2d) )
	ROM_LOAD( "cc10",         0x1000, 0x1000, CRC(b3c26cef) SHA1(f52cb5482c12a9c5fb56e2e2aec7cab0ed23e5a5) )
	ROM_LOAD( "cc09",         0x2000, 0x1000, CRC(6db0879c) SHA1(c0ba1976c1dcd6edadd78073173a26851ae8dd4f) )
	ROM_LOAD( "cc08",         0x3000, 0x1000, CRC(f48c5fe3) SHA1(79072bbbf37387998ffd031afe8eb569a16fa9bd) )
	ROM_LOAD( "cc07",         0x4000, 0x1000, CRC(3e873baf) SHA1(8870dc5948cdd3c8d2fe9e54a20cf6c311c94e53) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "cc06",         0x0000, 0x0800, CRC(481b64cc) SHA1(3f35c545fc784ed4f969aba2d7be6e13a5ae32b7) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "cc05",         0x1000, 0x0800, CRC(2c33b760) SHA1(2edea8fe13376fbd51a5586d97aba3b30d78e94b) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "cc04",         0x2000, 0x0800, CRC(332347cb) SHA1(4115ca32af73f1791635b7d9e093bf77088a8222) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "cc03",         0x3000, 0x0800, CRC(4e4b3658) SHA1(0d39a8cb5cd6cf06008be60707f9b277a8a32a2d) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "cc02",         0x0000, 0x0800, CRC(14f3ecc9) SHA1(a1b5121abfbe8f07580eb3fa6384352d239a3d75) )
	ROM_LOAD( "cc01",         0x0800, 0x0800, CRC(21c0f9fb) SHA1(44fad56d302a439257216ddac9fd62b3666589f1) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "cclimber.pr1", 0x0000, 0x0020, CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a) )
	ROM_LOAD( "cclimber.pr2", 0x0020, 0x0020, CRC(ab1940fa) SHA1(8d98e05cbaa6f55770c12e0a9a8ed9c73cc54423) )
	ROM_LOAD( "cclimber.pr3", 0x0040, 0x0020, CRC(71317756) SHA1(1195f0a037e379cc1a3c0314cb746f5cd2bffe50) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13",         0x0000, 0x1000, CRC(e0042f75) SHA1(86cb31b110742a0f7ae33052c88f42d00deb5468) )
	ROM_LOAD( "cc12",         0x1000, 0x1000, CRC(5da13aaa) SHA1(b2d41e69435d09c456648a10e33f5e1fbb0bc64c) )
ROM_END

#if 0
ROM_START( cclimberj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cc11j.bin",    0x0000, 0x1000, CRC(89783959) SHA1(948fa88fcb9e3797b9c10934d36cf6a55cb590fe) )
	ROM_LOAD( "cc10j.bin",    0x1000, 0x1000, CRC(14eda506) SHA1(4bc55b4c4ec197952b05ad32584f15f0383cc2df) )
	ROM_LOAD( "cc09j.bin",    0x2000, 0x1000, CRC(26489069) SHA1(9be4d4a22dd334e619416e6c846a05003c0d687e) )
	ROM_LOAD( "cc08j.bin",    0x3000, 0x1000, CRC(b33c96f8) SHA1(3974f4a60f37bed9e4faee7dafb565f553b9c201) )
	ROM_LOAD( "cc07j.bin",    0x4000, 0x1000, CRC(fbc9626c) SHA1(32be2d06321b2943718d0bec77ec9ebb806e4b93) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "cc06",         0x0000, 0x0800, CRC(481b64cc) SHA1(3f35c545fc784ed4f969aba2d7be6e13a5ae32b7) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "cc05",         0x1000, 0x0800, CRC(2c33b760) SHA1(2edea8fe13376fbd51a5586d97aba3b30d78e94b) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "cc04",         0x2000, 0x0800, CRC(332347cb) SHA1(4115ca32af73f1791635b7d9e093bf77088a8222) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "cc03",         0x3000, 0x0800, CRC(4e4b3658) SHA1(0d39a8cb5cd6cf06008be60707f9b277a8a32a2d) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "cc02",         0x0000, 0x0800, CRC(14f3ecc9) SHA1(a1b5121abfbe8f07580eb3fa6384352d239a3d75) )
	ROM_LOAD( "cc01",         0x0800, 0x0800, CRC(21c0f9fb) SHA1(44fad56d302a439257216ddac9fd62b3666589f1) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "cclimber.pr1", 0x0000, 0x0020, CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a) )
	ROM_LOAD( "cclimber.pr2", 0x0020, 0x0020, CRC(ab1940fa) SHA1(8d98e05cbaa6f55770c12e0a9a8ed9c73cc54423) )
	ROM_LOAD( "cclimber.pr3", 0x0040, 0x0020, CRC(71317756) SHA1(1195f0a037e379cc1a3c0314cb746f5cd2bffe50) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( ccboot )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "m11.bin",      0x0000, 0x1000, CRC(5efbe180) SHA1(e0c24f21d563da075eb5019d0e76cb01c2598c7a) )
	ROM_LOAD( "m10.bin",      0x1000, 0x1000, CRC(be2748c7) SHA1(ae66bc4e5e02bf9944a3ee4b0d2dec073f732260) )
	ROM_LOAD( "cc09j.bin",    0x2000, 0x1000, CRC(26489069) SHA1(9be4d4a22dd334e619416e6c846a05003c0d687e) )
	ROM_LOAD( "m08.bin",      0x3000, 0x1000, CRC(e3c542d6) SHA1(645cc4c94d1b1601c0083b156de67ec47fe2449f) )
	ROM_LOAD( "cc07j.bin",    0x4000, 0x1000, CRC(fbc9626c) SHA1(32be2d06321b2943718d0bec77ec9ebb806e4b93) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "cc06",         0x0000, 0x0800, CRC(481b64cc) SHA1(3f35c545fc784ed4f969aba2d7be6e13a5ae32b7) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "m05.bin",      0x1000, 0x0800, CRC(056af36b) SHA1(756a295bbf7ede201b2e4cb106ce67a127e008de) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "m04.bin",      0x2000, 0x0800, CRC(6fb80538) SHA1(6ba5add5c0190e79191b3fa749a1b94e766e3950) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "m03.bin",      0x3800, 0x0800, CRC(67127253) SHA1(e27556ed74e73644a2578ce6645c312d64f484c6) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "m02.bin",      0x0000, 0x0800, CRC(7f4877de) SHA1(c9aa9ff1b6cf907917fedfbd419b15ac337cf7bb) )
	ROM_LOAD( "m01.bin",      0x0800, 0x0800, CRC(49fab908) SHA1(9665d6e26f390afcbf0ed9fe8fea9be94fbb3a84) )

	ROM_REGION( 0x0160, "proms", 0 )
	ROM_LOAD( "cclimber.pr1", 0x0000, 0x0020, CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a) )
	ROM_LOAD( "cclimber.pr2", 0x0020, 0x0020, CRC(ab1940fa) SHA1(8d98e05cbaa6f55770c12e0a9a8ed9c73cc54423) )
	ROM_LOAD( "cclimber.pr3", 0x0040, 0x0020, CRC(71317756) SHA1(1195f0a037e379cc1a3c0314cb746f5cd2bffe50) )
	ROM_LOAD( "ccboot.prm",   0x0060, 0x0100, CRC(9e11550d) SHA1(b8cba8e16e10e23fba1f11551102ab77b680bdf0) )	/* decryption table (not used) */

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( ccboot2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "11.4k",        0x0000, 0x1000, CRC(b2b17e24) SHA1(1242d64242b3a6fe099457d155ebc508e5482818) )
	ROM_LOAD( "10.4j",        0x1000, 0x1000, CRC(8382bc0f) SHA1(2390ee2ec08a074c7bc4b9c7750b979a1d3a8a67) )
	ROM_LOAD( "cc09j.bin",    0x2000, 0x1000, CRC(26489069) SHA1(9be4d4a22dd334e619416e6c846a05003c0d687e) )
	ROM_LOAD( "m08.bin",      0x3000, 0x1000, CRC(e3c542d6) SHA1(645cc4c94d1b1601c0083b156de67ec47fe2449f) )
	ROM_LOAD( "cc07j.bin",    0x4000, 0x1000, CRC(fbc9626c) SHA1(32be2d06321b2943718d0bec77ec9ebb806e4b93) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "cc06",         0x0000, 0x0800, CRC(481b64cc) SHA1(3f35c545fc784ed4f969aba2d7be6e13a5ae32b7) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "cc05",         0x1000, 0x0800, CRC(2c33b760) SHA1(2edea8fe13376fbd51a5586d97aba3b30d78e94b) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "cc04",         0x2000, 0x0800, CRC(332347cb) SHA1(4115ca32af73f1791635b7d9e093bf77088a8222) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "cc03",         0x3000, 0x0800, CRC(4e4b3658) SHA1(0d39a8cb5cd6cf06008be60707f9b277a8a32a2d) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "cc02",         0x0000, 0x0800, CRC(14f3ecc9) SHA1(a1b5121abfbe8f07580eb3fa6384352d239a3d75) )
	ROM_LOAD( "cc01",         0x0800, 0x0800, CRC(21c0f9fb) SHA1(44fad56d302a439257216ddac9fd62b3666589f1) )

	ROM_REGION( 0x0160, "proms", 0 )
	ROM_LOAD( "cclimber.pr1", 0x0000, 0x0020, CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a) )
	ROM_LOAD( "cclimber.pr2", 0x0020, 0x0020, CRC(ab1940fa) SHA1(8d98e05cbaa6f55770c12e0a9a8ed9c73cc54423) )
	ROM_LOAD( "cclimber.pr3", 0x0040, 0x0020, CRC(71317756) SHA1(1195f0a037e379cc1a3c0314cb746f5cd2bffe50) )
	ROM_LOAD( "ccboot.prm",   0x0060, 0x0100, CRC(9e11550d) SHA1(b8cba8e16e10e23fba1f11551102ab77b680bdf0) )	/* decryption table (not used) */

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif

ROM_START( ckong )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d05-07.bin",   0x0000, 0x1000, CRC(b27df032) SHA1(57f9be139c610405e3c2fddd7093dfb1277e450e) )
	ROM_LOAD( "f05-08.bin",   0x1000, 0x1000, CRC(5dc1aaba) SHA1(42b9e5946ffce7c156d114bde68f37c2c34853c4) )
	ROM_LOAD( "h05-09.bin",   0x2000, 0x1000, CRC(c9054c94) SHA1(1aa08d2501ee620759fd5c111e12f6d432c25294) )
	ROM_LOAD( "k05-10.bin",   0x3000, 0x1000, CRC(069c4797) SHA1(03be185e6914ec7f3770ce3da4eb49cdb97adc85) )
	ROM_LOAD( "l05-11.bin",   0x4000, 0x1000, CRC(ae159192) SHA1(d467256a3a366e246243e7828ff4a45d4c146e2c) )
	ROM_LOAD( "n05-12.bin",   0x5000, 0x1000, CRC(966bc9ab) SHA1(4434fc620169ffea1b1f227b61674e1daf79b54b) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "n11-06.bin",   0x0000, 0x1000, CRC(2dcedd12) SHA1(dfdcfc21bcba7c8e148ee54daae511ca78c58e70) )
	ROM_LOAD( "l11-05.bin",   0x1000, 0x1000, CRC(fa7cbd91) SHA1(0208d2ebc59f3600005476b6987472685bc99d67) )
	ROM_LOAD( "k11-04.bin",   0x2000, 0x1000, CRC(3375b3bd) SHA1(a00b3c31cff123aab6ac0833aabfdd663302971a) )
	ROM_LOAD( "h11-03.bin",   0x3000, 0x1000, CRC(5655cc11) SHA1(5195e9b2a60c54280b48b32ee8248090904dbc51) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-02.bin",   0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-01.bin",   0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END

#if 0
ROM_START( ckonga )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d05-07.bin",   0x0000, 0x1000, CRC(b27df032) SHA1(57f9be139c610405e3c2fddd7093dfb1277e450e) )
	ROM_LOAD( "f05-08.bin",   0x1000, 0x1000, CRC(5dc1aaba) SHA1(42b9e5946ffce7c156d114bde68f37c2c34853c4) )
	ROM_LOAD( "h05-09.bin",   0x2000, 0x1000, CRC(c9054c94) SHA1(1aa08d2501ee620759fd5c111e12f6d432c25294) )
	ROM_LOAD( "10.dat",       0x3000, 0x1000, CRC(c3beb501) SHA1(14f49c45fc7c91799034c5a51fca310f0a66b1d7) )
	ROM_LOAD( "l05-11.bin",   0x4000, 0x1000, CRC(ae159192) SHA1(d467256a3a366e246243e7828ff4a45d4c146e2c) )
	ROM_LOAD( "n05-12.bin",   0x5000, 0x1000, CRC(966bc9ab) SHA1(4434fc620169ffea1b1f227b61674e1daf79b54b) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "n11-06.bin",   0x0000, 0x1000, CRC(2dcedd12) SHA1(dfdcfc21bcba7c8e148ee54daae511ca78c58e70) )
	ROM_LOAD( "l11-05.bin",   0x1000, 0x1000, CRC(fa7cbd91) SHA1(0208d2ebc59f3600005476b6987472685bc99d67) )
	ROM_LOAD( "k11-04.bin",   0x2000, 0x1000, CRC(3375b3bd) SHA1(a00b3c31cff123aab6ac0833aabfdd663302971a) )
	ROM_LOAD( "h11-03.bin",   0x3000, 0x1000, CRC(5655cc11) SHA1(5195e9b2a60c54280b48b32ee8248090904dbc51) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-02.bin",   0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-01.bin",   0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( ckong2j )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d05-07.bin",   0x0000, 0x1000, CRC(b27df032) SHA1(57f9be139c610405e3c2fddd7093dfb1277e450e) )
	ROM_LOAD( "f05-08.bin",   0x1000, 0x1000, CRC(5dc1aaba) SHA1(42b9e5946ffce7c156d114bde68f37c2c34853c4) )
	ROM_LOAD( "h05-09.bin",   0x2000, 0x1000, CRC(c9054c94) SHA1(1aa08d2501ee620759fd5c111e12f6d432c25294) )
	ROM_LOAD( "10.dat",       0x3000, 0x1000, CRC(c3beb501) SHA1(14f49c45fc7c91799034c5a51fca310f0a66b1d7) )
	ROM_LOAD( "11.5l",        0x4000, 0x1000, CRC(4164eb4d) SHA1(ec95f913820375c3f6dd24776b4d3fd04163f5de) )
	ROM_LOAD( "n05-12.bin",   0x5000, 0x1000, CRC(966bc9ab) SHA1(4434fc620169ffea1b1f227b61674e1daf79b54b) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "n11-06.bin",   0x0000, 0x1000, CRC(2dcedd12) SHA1(dfdcfc21bcba7c8e148ee54daae511ca78c58e70) )
	ROM_LOAD( "l11-05.bin",   0x1000, 0x1000, CRC(fa7cbd91) SHA1(0208d2ebc59f3600005476b6987472685bc99d67) )
	ROM_LOAD( "k11-04.bin",   0x2000, 0x1000, CRC(3375b3bd) SHA1(a00b3c31cff123aab6ac0833aabfdd663302971a) )
	ROM_LOAD( "h11-03.bin",   0x3000, 0x1000, CRC(5655cc11) SHA1(5195e9b2a60c54280b48b32ee8248090904dbc51) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-02.bin",   0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-01.bin",   0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( ckongjeu )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d05-07.bin",   0x0000, 0x1000, CRC(b27df032) SHA1(57f9be139c610405e3c2fddd7093dfb1277e450e) )
	ROM_LOAD( "f05-08.bin",   0x1000, 0x1000, CRC(5dc1aaba) SHA1(42b9e5946ffce7c156d114bde68f37c2c34853c4) )
	ROM_LOAD( "h05-09.bin",   0x2000, 0x1000, CRC(c9054c94) SHA1(1aa08d2501ee620759fd5c111e12f6d432c25294) )
	ROM_LOAD( "ckjeu10.dat",  0x3000, 0x1000, CRC(7e6eeec4) SHA1(98b283ea22bedc46710a24e65cfae48b87a57605) )
	ROM_LOAD( "l05-11.bin",   0x4000, 0x1000, CRC(ae159192) SHA1(d467256a3a366e246243e7828ff4a45d4c146e2c) )
	ROM_LOAD( "ckjeu12.dat",  0x5000, 0x1000, CRC(0532f270) SHA1(a73680bd7939097bd821fb6834e8763cf1572b55) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "n11-06.bin",   0x0000, 0x1000, CRC(2dcedd12) SHA1(dfdcfc21bcba7c8e148ee54daae511ca78c58e70) )
	ROM_LOAD( "l11-05.bin",   0x1000, 0x1000, CRC(fa7cbd91) SHA1(0208d2ebc59f3600005476b6987472685bc99d67) )
	ROM_LOAD( "k11-04.bin",   0x2000, 0x1000, CRC(3375b3bd) SHA1(a00b3c31cff123aab6ac0833aabfdd663302971a) )
	ROM_LOAD( "h11-03.bin",   0x3000, 0x1000, CRC(5655cc11) SHA1(5195e9b2a60c54280b48b32ee8248090904dbc51) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-02.bin",   0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-01.bin",   0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( ckongo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "o55a-1",       0x0000, 0x1000, CRC(8bfb4623) SHA1(1b8e12d1f337756bbfa9c3d736db7513d571c1b3) )
	ROM_LOAD( "o55a-2",       0x1000, 0x1000, CRC(9ae8089b) SHA1(e50864bb77dce24ba6d10c4fc16ccaa593962442) )
	ROM_LOAD( "o55a-3",       0x2000, 0x1000, CRC(e82b33c8) SHA1(27befba696cd1a9453fb49e8e4ddd46eab41b30d) )
	ROM_LOAD( "o55a-4",       0x3000, 0x1000, CRC(f038f941) SHA1(02be92ef3bf8d36c9916b40109c738965a652a76) )
	ROM_LOAD( "o55a-5",       0x4000, 0x1000, CRC(5182db06) SHA1(f3e981dc3744aff7756f8e0bfd4d92583a02417d) )
	/* no ROM at 5000 */

	ROM_REGION( 0x4000, "gfx1", 0 )
	/* same as ckong but with halves switched */
	ROM_LOAD( "o50b-1",       0x0000, 0x0800, CRC(cae9e2bf) SHA1(bc62d98840b8b5b296de0f1379baabb1b4d25df0) )
	ROM_CONTINUE(             0x1000, 0x0800 )
	ROM_LOAD( "o50b-2",       0x0800, 0x0800, CRC(fba82114) SHA1(36b7c124edf73b01681f5d63867fafa38a31abbf) )
	ROM_CONTINUE(             0x1800, 0x0800 )
	ROM_LOAD( "o50b-3",       0x2000, 0x0800, CRC(1714764b) SHA1(b025fcc03d45b1ec29be7e292622745544ba891d) )
	ROM_CONTINUE(             0x3000, 0x0800 )
	ROM_LOAD( "o50b-4",       0x2800, 0x0800, CRC(b7008b57) SHA1(9328ff79947dbebdc3e2dd8bcc362667b8201476) )
	ROM_CONTINUE(             0x3800, 0x0800 )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-02.bin",   0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-01.bin",   0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( ckongalc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ck7.bin",      0x0000, 0x1000, CRC(2171cac3) SHA1(7b18bfe44c32fb64b675bbbe2136344522c79b09) )
	ROM_LOAD( "ck8.bin",      0x1000, 0x1000, CRC(88b83ff7) SHA1(4afc494cc264aaa4614da6aed02ce062d9c20850) )
	ROM_LOAD( "ck9.bin",      0x2000, 0x1000, CRC(cff2af47) SHA1(1757428cefad13855a623162101ec01c04006c94) )
	ROM_LOAD( "ck10.bin",     0x3000, 0x1000, CRC(520fa4de) SHA1(6edbaf727756cd33bde94492d72654aa12dbd7e1) )
	ROM_LOAD( "ck11.bin",     0x4000, 0x1000, CRC(327dcadf) SHA1(17b2d3b9e2a82b5278a01cc972cb49705d113127) )
	/* no ROM at 5000 */

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "ck6.bin",      0x0000, 0x1000, CRC(a8916dc8) SHA1(472520aae3837e6026f2a7577d3b2aff371a316c) )
	ROM_LOAD( "ck5.bin",      0x1000, 0x1000, CRC(cd3b5dde) SHA1(2319a2be04d70989b01f4fc703756ba6e1c1f388) )
	ROM_LOAD( "ck4.bin",      0x2000, 0x1000, CRC(b62a0367) SHA1(8c285cbc714d7e6589bd63b3cef7c841ed1c2a4e) )
	ROM_LOAD( "ck3.bin",      0x3000, 0x1000, CRC(61122c5e) SHA1(978b6dbec35f3adc651fddf332db17625099a92e) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "ck2.bin",      0x0000, 0x0800, CRC(f67c80f1) SHA1(d1fbcce1b6242f810e106ff50812636e3168ebc1) )
	ROM_LOAD( "ck1.bin",      0x0800, 0x0800, CRC(80eb517d) SHA1(fef4111f656c58b28e7eac5aa5b5cc7e07ccb2fd) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "ck6v.bin",     0x0000, 0x0020, CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a) )
	ROM_LOAD( "ck6u.bin",     0x0020, 0x0020, CRC(ab1940fa) SHA1(8d98e05cbaa6f55770c12e0a9a8ed9c73cc54423) )
	ROM_LOAD( "ck6t.bin",     0x0040, 0x0020, CRC(b4e827a5) SHA1(31a5a5ad54417a474d22bb16c473415d99a2b6f1) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "ck12.bin",     0x1000, 0x1000, CRC(2eb23b60) SHA1(c9e7dc584562aceb374193655fbacb7df6c9c731) )
ROM_END
#endif
#if 0
ROM_START( bigkong )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dk01f7_2532.d5",   0x0000, 0x1000, CRC(4c9102f1) SHA1(845b48fa1e6ad82dc797520f7ff7daffc1a47c39) )
	ROM_LOAD( "dk02f8_2532.f5",   0x1000, 0x1000, CRC(1683e9ae) SHA1(4690c8be70c0cc0e7d78d2ff205eed3f4ead7278) )
	ROM_LOAD( "dk03f9_2532.h5",   0x2000, 0x1000, CRC(073eea32) SHA1(de7889df04f8a279a0864748298e5ccdce0578f1) )
	ROM_LOAD( "dk04f10_2532.k5",   0x3000, 0x1000, CRC(0aab0334) SHA1(be4b5c121538dc3a82797475f3bb15918eb6d817) )
	ROM_LOAD( "dk05f11_2532.l5",   0x4000, 0x1000, CRC(45be1c6a) SHA1(3d45da4ab21586148a3608d085aa4c401bd257fe) )
	ROM_LOAD( "n05-12.bin",   0x5000, 0x1000, CRC(966bc9ab) SHA1(4434fc620169ffea1b1f227b61674e1daf79b54b) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "n11-06.bin",   0x0000, 0x1000, CRC(2dcedd12) SHA1(dfdcfc21bcba7c8e148ee54daae511ca78c58e70) )
	ROM_LOAD( "l11-05.bin",   0x1000, 0x1000, CRC(fa7cbd91) SHA1(0208d2ebc59f3600005476b6987472685bc99d67) )
	ROM_LOAD( "k11-04.bin",   0x2000, 0x1000, CRC(3375b3bd) SHA1(a00b3c31cff123aab6ac0833aabfdd663302971a) )
	ROM_LOAD( "h11-03.bin",   0x3000, 0x1000, CRC(5655cc11) SHA1(5195e9b2a60c54280b48b32ee8248090904dbc51) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-02.bin",   0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-01.bin",   0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cc12j.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif
#if 0
ROM_START( monkeyd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ck7.bin",      0x0000, 0x1000, CRC(2171cac3) SHA1(7b18bfe44c32fb64b675bbbe2136344522c79b09) )
	ROM_LOAD( "ck8.bin",      0x1000, 0x1000, CRC(88b83ff7) SHA1(4afc494cc264aaa4614da6aed02ce062d9c20850) )
	ROM_LOAD( "ck9.bin",      0x2000, 0x1000, CRC(cff2af47) SHA1(1757428cefad13855a623162101ec01c04006c94) )
	ROM_LOAD( "ck10.bin",     0x3000, 0x1000, CRC(520fa4de) SHA1(6edbaf727756cd33bde94492d72654aa12dbd7e1) )
	ROM_LOAD( "md5l.bin",     0x4000, 0x1000, CRC(d1db1bb0) SHA1(fe7d700c7f9eca9c389be3717ebebf3e7dc63aa2) )
	/* no ROM at 5000 */

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "ck6.bin",      0x0000, 0x1000, CRC(a8916dc8) SHA1(472520aae3837e6026f2a7577d3b2aff371a316c) )
	ROM_LOAD( "ck5.bin",      0x1000, 0x1000, CRC(cd3b5dde) SHA1(2319a2be04d70989b01f4fc703756ba6e1c1f388) )
	ROM_LOAD( "ck4.bin",      0x2000, 0x1000, CRC(b62a0367) SHA1(8c285cbc714d7e6589bd63b3cef7c841ed1c2a4e) )
	ROM_LOAD( "ck3.bin",      0x3000, 0x1000, CRC(61122c5e) SHA1(978b6dbec35f3adc651fddf332db17625099a92e) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "ck2.bin",      0x0000, 0x0800, CRC(f67c80f1) SHA1(d1fbcce1b6242f810e106ff50812636e3168ebc1) )
	ROM_LOAD( "ck1.bin",      0x0800, 0x0800, CRC(80eb517d) SHA1(fef4111f656c58b28e7eac5aa5b5cc7e07ccb2fd) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "ck6v.bin",     0x0000, 0x0020, BAD_DUMP CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a)  )
	ROM_LOAD( "ck6u.bin",     0x0020, 0x0020, BAD_DUMP CRC(ab1940fa) SHA1(8d98e05cbaa6f55770c12e0a9a8ed9c73cc54423)  )
	ROM_LOAD( "ck6t.bin",     0x0040, 0x0020, BAD_DUMP CRC(b4e827a5) SHA1(31a5a5ad54417a474d22bb16c473415d99a2b6f1)  )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cc13j.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "ck12.bin",     0x1000, 0x1000, CRC(2eb23b60) SHA1(c9e7dc584562aceb374193655fbacb7df6c9c731) )
ROM_END
#endif
#if 0
ROM_START( ckongb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d05-7.rom",    0x0000, 0x1000, CRC(5d96ee9a) SHA1(f531d251fd3469edd3b5b5e7c26ff9cef7006ce8) )
	ROM_LOAD( "f05-8.rom",    0x1000, 0x1000, CRC(74a8435b) SHA1(465ad96009d3ba939eee13ba0d5fd6d9dec118bc) )
	ROM_LOAD( "h05-9.rom",    0x2000, 0x1000, CRC(e06ca575) SHA1(cd5a32fac614902e136e522ac188616c72d65571) )
	ROM_LOAD( "k05-10.rom",   0x3000, 0x1000, CRC(46d83a11) SHA1(de840994104bfc633a3640610966f087fbc3d749) )
	ROM_LOAD( "l05-11.rom",   0x4000, 0x1000, CRC(07c30f3d) SHA1(9b72f8a76c64ab22f8b8c1bd8e457c10b86d95a1) )
	ROM_LOAD( "n05-12.rom",   0x5000, 0x1000, CRC(151de90a) SHA1(5d063c4fe6767727d051815120d692818a30ee81) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "n11-6.bin",    0x0000, 0x1000, CRC(2dcedd12) SHA1(dfdcfc21bcba7c8e148ee54daae511ca78c58e70) )
	ROM_LOAD( "l11-5.bin",    0x1000, 0x1000, CRC(fa7cbd91) SHA1(0208d2ebc59f3600005476b6987472685bc99d67) )
	ROM_LOAD( "k11-4.bin",    0x2000, 0x1000, CRC(3375b3bd) SHA1(a00b3c31cff123aab6ac0833aabfdd663302971a) )
	ROM_LOAD( "h11-3.bin",    0x3000, 0x1000, CRC(5655cc11) SHA1(5195e9b2a60c54280b48b32ee8248090904dbc51) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "c11-2.bin",    0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "a11-1.bin",    0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )  /* samples */
	ROM_LOAD( "s05-14.bin",   0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "r05-13.bin",   0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif

/* This set came from a 'Silver Land' board with Silver Land GFX roms, however, the program roms are nearly
   the same as River Patrol but appear to have an original ORCA copyright

   I think the board was a half-converted board as 'Water Gage' and 'Bon Voyage' don't really fit the theme
   of Silver Land so I'm loading the River Patrol GFX instead as they fit better
*/
ROM_START( rpatrol )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sci1.bin",       0x0000, 0x1000, CRC(33b01c90) SHA1(9c8da6dd963bfb0544ef99b8fdedcf86c32cdb6b) )
	ROM_LOAD( "sci2.bin",       0x1000, 0x1000, CRC(03f53340) SHA1(35336945f4b634fc4c7791ac9c9e6643c8cd8006) )
	ROM_LOAD( "sci3.bin",       0x2000, 0x1000, CRC(8fa300df) SHA1(5c3ba1ef6c1ce8df437b4fa464293208630b5e8d) )
	ROM_LOAD( "sci4.bin",       0x3000, 0x1000, CRC(74a8f1f4) SHA1(6bbc4944e4b31425a6b82f370b6760e5a4b36f56) )
	ROM_LOAD( "sci5.bin",       0x4000, 0x1000, CRC(d7ef6c87) SHA1(38e3b44b355907824919acc4f5064dcb98ebb1d0) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "rp6.6n",       0x0000, 0x0800, CRC(19f18e9e) SHA1(a5500ac36bcda772f3ba79d9e9d37b1eec7bfd13) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "rp7.6l",       0x1000, 0x0800, CRC(07f2070d) SHA1(39df286fda9e48eba6e770fe23a603b5e10d88b6) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "rp8.6k",       0x2000, 0x0800, CRC(008738c7) SHA1(a66d9daf31b0d9cf087b591c74f0aaee3d7607b5) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "rp9.6h",       0x3000, 0x0800, CRC(ea5aafca) SHA1(d8f8fe270680ae261d63bd4702107961cd904699) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "rp10.6a",      0x0000, 0x0800, CRC(59747c31) SHA1(92acf07489f3e17f0c1769a0df15b6ddb117830f) )
	ROM_LOAD( "rp11.6c",      0x0800, 0x0800, CRC(065651a5) SHA1(5c2f9b44d8819d2f792525c06b5c341fe07329c0) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "bprom1.9n",    0x0000, 0x0020, CRC(f9a2383b) SHA1(4d88c177740efdb27708474c9ee0fcdca5a78c36) )
	ROM_LOAD( "bprom2.9p",    0x0020, 0x0020, CRC(1743bd26) SHA1(9bb50f6e24a7ac3c9ddf3923e57c5532603009e5) )
	ROM_LOAD( "bprom3.9c",    0x0040, 0x0020, CRC(ee03bc96) SHA1(45e33e750a536a904f30136d84dd7993d97e8e54) )

	// these are the GFX Roms found on the board, from Silver Land, but IMO not correct for this program
#if 0
	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "sci46.bin",    0x0000, 0x0800, CRC(affb804f) SHA1(9fc77804690e91773787e06f3329accef075f9f3) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "sci45.bin",    0x1000, 0x0800, CRC(ad4642e5) SHA1(f4de2d9ed0e69c002be07f47247e95167a3ffffb) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "sci44.bin",    0x2000, 0x0800, CRC(e487579d) SHA1(aed59f15dbc904d73e19d914ccd0a86fda859085) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "sci43.bin",    0x3000, 0x0800, CRC(59125a1a) SHA1(37638fb690d6b4f11585f6a13586271c2f0e3743) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "sci42.bin",         0x0000, 0x0800, CRC(c8d32b8e) SHA1(7d655d243ed13cf2537f3fdfde5bf34229f7cb84) )
	ROM_LOAD( "sci41.bin",         0x0800, 0x0800, CRC(ee333daf) SHA1(b02998dccec9a4f841838874221caabae8380fcc) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "mb7051.1v",    0x0000, 0x0020, CRC(1d2343b1) SHA1(294f22178af4532abf767c1ffe2dc831bbe683bf) )
	ROM_LOAD( "mb7051.1u",    0x0020, 0x0020, CRC(c174753c) SHA1(303bfb1f470b525ccaeafa81a38a4bc3a7de5dbb) )
	ROM_LOAD( "mb7051.1t",    0x0040, 0x0020, CRC(04a1be01) SHA1(9c270c04d374d46752ec99bd4e79fed1e2896bc0) )
#endif
	/* no samples */
ROM_END

#if 0
ROM_START( rpatrolb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "rp1.4l",       0x0000, 0x1000, CRC(bfd7ae7a) SHA1(a06d1cc2674ed40d0bfa67dd6d724964c1e40600) )
	ROM_LOAD( "rp2.4j",       0x1000, 0x1000, CRC(03f53340) SHA1(35336945f4b634fc4c7791ac9c9e6643c8cd8006) )
	ROM_LOAD( "rp3.4f",       0x2000, 0x1000, CRC(8fa300df) SHA1(5c3ba1ef6c1ce8df437b4fa464293208630b5e8d) )
	ROM_LOAD( "rp4.4e",       0x3000, 0x1000, CRC(74a8f1f4) SHA1(6bbc4944e4b31425a6b82f370b6760e5a4b36f56) )
	ROM_LOAD( "rp5.4c",       0x4000, 0x1000, CRC(d7ef6c87) SHA1(38e3b44b355907824919acc4f5064dcb98ebb1d0) )
	/* no ROM at 5000 */

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "rp6.6n",       0x0000, 0x0800, CRC(19f18e9e) SHA1(a5500ac36bcda772f3ba79d9e9d37b1eec7bfd13) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "rp7.6l",       0x1000, 0x0800, CRC(07f2070d) SHA1(39df286fda9e48eba6e770fe23a603b5e10d88b6) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "rp8.6k",       0x2000, 0x0800, CRC(008738c7) SHA1(a66d9daf31b0d9cf087b591c74f0aaee3d7607b5) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "rp9.6h",       0x3000, 0x0800, CRC(ea5aafca) SHA1(d8f8fe270680ae261d63bd4702107961cd904699) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "rp10.6a",      0x0000, 0x0800, CRC(59747c31) SHA1(92acf07489f3e17f0c1769a0df15b6ddb117830f) )
	ROM_LOAD( "rp11.6c",      0x0800, 0x0800, CRC(065651a5) SHA1(5c2f9b44d8819d2f792525c06b5c341fe07329c0) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "bprom1.9n",    0x0000, 0x0020, CRC(f9a2383b) SHA1(4d88c177740efdb27708474c9ee0fcdca5a78c36) )
	ROM_LOAD( "bprom2.9p",    0x0020, 0x0020, CRC(1743bd26) SHA1(9bb50f6e24a7ac3c9ddf3923e57c5532603009e5) )
	ROM_LOAD( "bprom3.9c",    0x0040, 0x0020, CRC(ee03bc96) SHA1(45e33e750a536a904f30136d84dd7993d97e8e54) )

	/* no samples */
ROM_END
#endif
#if 0
ROM_START( silvland )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "7.2r",         0x0000, 0x1000, CRC(57e6be62) SHA1(c1d47970f8209256c9ccd6512b921dec6c276998) )
	ROM_LOAD( "8.1n",         0x1000, 0x1000, CRC(bbb2b287) SHA1(93cd4ebe238c189c80be8b8ab1ec2649256dd6ea) )
	ROM_LOAD( "rp3.4f",       0x2000, 0x1000, CRC(8fa300df) SHA1(5c3ba1ef6c1ce8df437b4fa464293208630b5e8d) )
	ROM_LOAD( "10.2n",        0x3000, 0x1000, CRC(5536a65d) SHA1(0bf2b9ea76fd6fd8c0475bf6f49a42f1c96d3906) )
	ROM_LOAD( "11.1r",        0x4000, 0x1000, CRC(6f23f66f) SHA1(3ca8075c28956ec473ccb0e9f05e9ad8669f743d) )
	ROM_LOAD( "12.2k",        0x5000, 0x1000, CRC(26f1537c) SHA1(0468352d49edec3a52e32612856735b78e11079b) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "6.6n",         0x0000, 0x0800, CRC(affb804f) SHA1(9fc77804690e91773787e06f3329accef075f9f3) )
	/* 0x0800-0x0fff - empty */
	ROM_LOAD( "5.6l",         0x1000, 0x0800, CRC(ad4642e5) SHA1(f4de2d9ed0e69c002be07f47247e95167a3ffffb) )
	/* 0x1800-0xffff - empty */
	ROM_LOAD( "4.6k",         0x2000, 0x0800, CRC(e487579d) SHA1(aed59f15dbc904d73e19d914ccd0a86fda859085) )
	/* 0x2800-0x2fff - empty */
	ROM_LOAD( "3.6h",         0x3000, 0x0800, CRC(59125a1a) SHA1(37638fb690d6b4f11585f6a13586271c2f0e3743) )
	/* 0x3800-0x3fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "2.6c",         0x0000, 0x0800, CRC(c8d32b8e) SHA1(7d655d243ed13cf2537f3fdfde5bf34229f7cb84) )
	ROM_LOAD( "1.6a",         0x0800, 0x0800, CRC(ee333daf) SHA1(b02998dccec9a4f841838874221caabae8380fcc) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "mb7051.1v",    0x0000, 0x0020, CRC(1d2343b1) SHA1(294f22178af4532abf767c1ffe2dc831bbe683bf) )
	ROM_LOAD( "mb7051.1u",    0x0020, 0x0020, CRC(c174753c) SHA1(303bfb1f470b525ccaeafa81a38a4bc3a7de5dbb) )
	ROM_LOAD( "mb7051.1t",    0x0040, 0x0020, CRC(04a1be01) SHA1(9c270c04d374d46752ec99bd4e79fed1e2896bc0) )

	/* no samples */
ROM_END
#endif
/*This dump is a mess.  11n and 11k seem to be bad dumps, the second half should probably be sprite data
  Comparing to set 2 11l and 11h are unnecessary, and are actually from Le Bagnard(set1), as is 5m.
  5n ID'd as unknown, but it also is from bagnard with some patches.*/
#if 0
ROM_START( cannonb )
	ROM_REGION( 0x11000, "maincpu", 0 )
	ROM_LOAD( "canballs.5d", 0x10000, 0x1000, CRC(43ad0d16) SHA1(682f1ee15e41bb5a161287536bb97704c0d3be9c) ) /* only this one ROM is encrypted */
	ROM_LOAD( "canballs.5f",  0x1000, 0x1000, CRC(3e0dacdd) SHA1(cdd3684a6962f2fb582b8a415383c06a5e5059dd) )
	ROM_LOAD( "canballs.5h",  0x2000, 0x1000, CRC(e18a836b) SHA1(19b90a55db82914c5db18486e05d9f59aba1b442) )
	ROM_LOAD( "canballs.5k",  0x3000, 0x0800, CRC(6ed3cbf4) SHA1(070ba61dc97df6be8004f7e052a4cef836234888) )
	ROM_LOAD( "canballs.5m",  0x4000, 0x1000, CRC(4f0088ab) SHA1(a8009f5b8517ba4d84fbc483b199f2514f24eae8) )
	ROM_LOAD( "canballs.5n",  0x5000, 0x1000, CRC(91570033) SHA1(7cd7fe9541da36c3919324bc65e6db1d1ca635e0) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "canballs.11n", 0x0000, 0x1000, CRC(a95b8e03) SHA1(e78125023e1af6de292292b875b45401b2173ca9) )
	ROM_LOAD( "canballs.11l", 0x1000, 0x1000, CRC(060b044c) SHA1(3121f07adb661663a2303085eea1b662968f8f98) )
	ROM_LOAD( "canballs.11k", 0x2000, 0x1000, CRC(dbbe8263) SHA1(efe4bba25a03261bc8309e6d83d5600def875b0c) )
	ROM_LOAD( "canballs.11h", 0x3000, 0x1000, CRC(8043bc1a) SHA1(bd2f3dfe26cf8d987d9ecaa41eac4bdc4e16a692) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "canballs.11c", 0x0000, 0x0800, CRC(d1352c31) SHA1(da726a63a8be830d695afeddc1717749af8c9d47) )
	ROM_LOAD( "canballs.11a", 0x0800, 0x0800, CRC(a7a2fdbd) SHA1(529865f8bbfbdbbf34ac39c70ef17e6d5bd0f845) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "prom.v6",      0x0000, 0x0020, CRC(b3fc1505) SHA1(5b94adde0428a26b815c7eb9b3f3716470d349c7) )
	ROM_LOAD( "prom.u6",      0x0020, 0x0020, CRC(26aada9e) SHA1(f59645e606ea4f0dd0fc4ea47dd03f526c534941) )
	ROM_LOAD( "prom.t6",      0x0040, 0x0020, CRC(676b3166) SHA1(29b9434cd34d43ea5664e436e2a24b54f8d88aac) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "canballs.5s",  0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "canballs.5p",  0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END
#endif

ROM_START( cannonb2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cb1.bin",   0x0000, 0x1000, CRC(7a3cba7c) SHA1(08b8b356fdbe642e80d42b5ab4164a1bd6ad93ba) )
	ROM_LOAD( "cb2.bin",   0x1000, 0x1000, CRC(58ef3118) SHA1(51ae36c21147e99d4060034520f6eebf3210937c) )
	ROM_LOAD( "cb3.bin",   0x2000, 0x1000, CRC(e18a836b) SHA1(19b90a55db82914c5db18486e05d9f59aba1b442) )
	ROM_LOAD( "cb4.bin",   0x3000, 0x1000, CRC(696ebdb0) SHA1(0bff115e4710199641722ca12af4e16dc5b0ec13) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "cb10.bin",   0x0000, 0x0800, CRC(602a6c2d) SHA1(788f83bcb0667d8a42c209f3d51708d496be58df) )
	/* 0x0800-0x0fff - empty */
	ROM_CONTINUE(           0x1000, 0x0800 )
	/* 0x1800-0x0fff - empty */
	ROM_LOAD( "cb9.bin",    0x2000, 0x0800, CRC(2d036026) SHA1(b6eada3e67edd7db59d9ca823b798cd20f0afca9) )
	/* 0x2800-0x0fff - empty */
	ROM_CONTINUE(           0x3000, 0x0800 )
	/* 0x3800-0x0fff - empty */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "cb7.bin",   0x0000, 0x0800, CRC(80eb517d) SHA1(fef4111f656c58b28e7eac5aa5b5cc7e07ccb2fd) )
	ROM_LOAD( "cb8.bin",   0x0800, 0x0800, CRC(f67c80f1) SHA1(d1fbcce1b6242f810e106ff50812636e3168ebc1) )

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "v6.bin",      0x0000, 0x0020, CRC(751c3325) SHA1(edce2bc883996c1d72dc6c1c9f62799b162d415a) )
	ROM_LOAD( "u6.bin",      0x0020, 0x0020, CRC(c0539747) SHA1(1bc70057b59b8cb11299fb6b0d84a46da6c0a025) )
	ROM_LOAD( "t6.bin",      0x0040, 0x0020, CRC(b4e827a5) SHA1(31a5a5ad54417a474d22bb16c473415d99a2b6f1) )

	ROM_REGION( 0x2000, "samples", 0 )	/* samples */
	ROM_LOAD( "cb6.bin",    0x0000, 0x1000, CRC(5f0bcdfb) SHA1(7f79bf6de117348f606696ed7ea1937bbf926612) )
	ROM_LOAD( "cb5.bin",    0x1000, 0x1000, CRC(9003ffbd) SHA1(fd016056aabc23957643f37230f03842294f795e) )
ROM_END

ROM_START( swimmer )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sw1",          0x0000, 0x1000, CRC(f12481e7) SHA1(4e8ee509043fd57ec1579594f0b2c543f270bead) )
	ROM_LOAD( "sw2",          0x1000, 0x1000, CRC(a0b6fdd2) SHA1(7d3603de6c282224869824c7572868fc85599ea2) )
	ROM_LOAD( "sw3",          0x2000, 0x1000, CRC(ec93d7de) SHA1(e225c6b98eb3c32825c1cc1fcf69dec7e340460c) )
	ROM_LOAD( "sw4",          0x3000, 0x1000, CRC(0107927d) SHA1(419aeca37c7604f71f49e3dee36f477eee0ba53a) )
	ROM_LOAD( "sw5",          0x4000, 0x1000, CRC(ebd8a92c) SHA1(65401f8d39250f6ec61841e58ce4c21ddfe99842) )
	ROM_LOAD( "sw6",          0x5000, 0x1000, CRC(f8539821) SHA1(82f43ecbbb0a3771632eb26e10bc5453d74b65b1) )
	ROM_LOAD( "sw7",          0x6000, 0x1000, CRC(37efb64e) SHA1(0ed4d678895c17b37df605990acd096c538e3675) )
	ROM_LOAD( "sw8",          0x7000, 0x1000, CRC(33d6001e) SHA1(749b746d018e74e364fd6974e4522c8a18915774) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sw12.4k",      0x0000, 0x1000, CRC(2eee9bcb) SHA1(ceafdf750a8af0c1c9abbbf437c3e9d9ae09f72b) )

	ROM_REGION( 0x3000, "gfx1", 0 )
	ROM_LOAD( "sw15.18k",     0x0000, 0x1000, CRC(4f3608cb) SHA1(cebcad69c4ad5dacc0bf597fdaed6f8950ffdfe1) )  /* chars */
	ROM_LOAD( "sw14.18l",     0x1000, 0x1000, CRC(7181c8b4) SHA1(b22fa0ebac884002cf6f5651e4366f30d0ab09f5) )
	ROM_LOAD( "sw13.18m",     0x2000, 0x1000, CRC(2eb1af5c) SHA1(0105d03adfc5ce9ca478e678a1e1d8bae7c516e0) )

	ROM_REGION( 0x3000, "gfx2", 0 )
	ROM_LOAD( "sw23.6c",      0x0000, 0x0800, CRC(9ca67e24) SHA1(86f561abc1a1c6b0c29c6017246d805c5a48b999) )  /* bigsprite data */
	ROM_RELOAD(               0x0800, 0x0800 )	/* Guzzler has larger ROMs */
	ROM_LOAD( "sw22.5c",      0x1000, 0x0800, CRC(02c10992) SHA1(8c383fdcd83aa9997e5802a58419b9d993a9b38d) )
	ROM_RELOAD(               0x1800, 0x0800 )	/* Guzzler has larger ROMs */
	ROM_LOAD( "sw21.4c",      0x2000, 0x0800, CRC(7f4993c1) SHA1(a5884b3af707109e810cf1f38bee3cb642e619f6) )
	ROM_RELOAD(               0x2800, 0x0800 )	/* Guzzler has larger ROMs */

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "24s10.13b",    0x0000, 0x100, CRC(8e35b97d) SHA1(2e2c254574660e01b9983f795a2adb5b9911d7f0) )
	ROM_LOAD( "24s10.13a",    0x0100, 0x100, CRC(c5f24909) SHA1(27f2c967d440f6387841aa3f7b116c64bb812af1) )
	ROM_LOAD( "18s030.12c",   0x0200, 0x020, CRC(3b2deb3a) SHA1(bb7b5c662454f5b355cc59cbdf8879e4664bed1d) )
ROM_END

#if 0
ROM_START( swimmera )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "swa1",         0x0000, 0x1000, CRC(42c2b6c5) SHA1(13688e1ee08308b13ead5af7b4f65043dae4e40f) )
	ROM_LOAD( "swa2",         0x1000, 0x1000, CRC(49bac195) SHA1(a5d2cc2cdd10003f69014c4799f5f59e47a44260) )
	ROM_LOAD( "swa3",         0x2000, 0x1000, CRC(a6d8cb01) SHA1(80ab0ffaee6e0edf19b767229865722c2af6112c) )
	ROM_LOAD( "swa4",         0x3000, 0x1000, CRC(7be75182) SHA1(4fe7bc6382ea7311be1225fb0715aa2ff4ec084c) )
	ROM_LOAD( "swa5",         0x4000, 0x1000, CRC(78f79573) SHA1(6124fae47b3fa2e5965dffdfe9cbeb96acf08314) )
	ROM_LOAD( "swa6",         0x5000, 0x1000, CRC(fda9b311) SHA1(d9c914ad27f5988d0d4da5c942fb12bb5728cdfb) )
	ROM_LOAD( "swa7",         0x6000, 0x1000, CRC(7090e5ee) SHA1(d1e0ca38c3d1e4a7b7efa3696e47fb36ad3f8aa0) )
	ROM_LOAD( "swa8",         0x7000, 0x1000, CRC(ab86efa9) SHA1(5b5a80ae285c7e9f4c51e646116edf789d4dba39) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sw12.4k",      0x0000, 0x1000, CRC(2eee9bcb) SHA1(ceafdf750a8af0c1c9abbbf437c3e9d9ae09f72b) )

	ROM_REGION( 0x3000, "gfx1", 0 )
	ROM_LOAD( "sw15.18k",     0x0000, 0x1000, CRC(4f3608cb) SHA1(cebcad69c4ad5dacc0bf597fdaed6f8950ffdfe1) )  /* chars */
	ROM_LOAD( "sw14.18l",     0x1000, 0x1000, CRC(7181c8b4) SHA1(b22fa0ebac884002cf6f5651e4366f30d0ab09f5) )
	ROM_LOAD( "sw13.18m",     0x2000, 0x1000, CRC(2eb1af5c) SHA1(0105d03adfc5ce9ca478e678a1e1d8bae7c516e0) )

	ROM_REGION( 0x3000, "gfx2", 0 )
	ROM_LOAD( "sw23.6c",      0x0000, 0x0800, CRC(9ca67e24) SHA1(86f561abc1a1c6b0c29c6017246d805c5a48b999) )  /* bigsprite data */
	ROM_RELOAD(               0x0800, 0x0800 )	/* Guzzler has larger ROMs */
	ROM_LOAD( "sw22.5c",      0x1000, 0x0800, CRC(02c10992) SHA1(8c383fdcd83aa9997e5802a58419b9d993a9b38d) )
	ROM_RELOAD(               0x1800, 0x0800 )	/* Guzzler has larger ROMs */
	ROM_LOAD( "sw21.4c",      0x2000, 0x0800, CRC(7f4993c1) SHA1(a5884b3af707109e810cf1f38bee3cb642e619f6) )
	ROM_RELOAD(               0x2800, 0x0800 )	/* Guzzler has larger ROMs */

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "24s10.13b",    0x0000, 0x100, CRC(8e35b97d) SHA1(2e2c254574660e01b9983f795a2adb5b9911d7f0) )
	ROM_LOAD( "24s10.13a",    0x0100, 0x100, CRC(c5f24909) SHA1(27f2c967d440f6387841aa3f7b116c64bb812af1) )
	ROM_LOAD( "18s030.12c",   0x0200, 0x020, CRC(3b2deb3a) SHA1(bb7b5c662454f5b355cc59cbdf8879e4664bed1d) )
ROM_END
#endif
#if 0
ROM_START( swimmerb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sw1.9l",       0x0000, 0x1000, CRC(b045be08) SHA1(52187e1daebec521a98157f22960637393e40e62) )
	ROM_LOAD( "sw2.9k",       0x1000, 0x1000, CRC(163d65e5) SHA1(b505f05af96f241285f0f7082ed03fa07bbde7de) )
	ROM_LOAD( "sw3.9j",       0x2000, 0x1000, CRC(631d74e9) SHA1(b6adba9445264de80f5daf33dad1c90b23617648) )
	ROM_LOAD( "sw4.9f",       0x3000, 0x1000, CRC(d62634db) SHA1(c6d0d2cf7a3a19fac1752a30189f31eb3df8fa42) )
	ROM_LOAD( "sw5.9e",       0x4000, 0x1000, CRC(922d5d87) SHA1(e5f111d82a072e59c00b759eaada195f1fc06532) )
	ROM_LOAD( "sw6.9d",       0x5000, 0x1000, CRC(85478209) SHA1(df3c79ca25229fef2fe0f48d3c173e389628a68d) )
	ROM_LOAD( "sw7.9c",       0x6000, 0x1000, CRC(88266f2e) SHA1(4ad15f9ba7b45a6c1c3637f8d0fd8be9c04b495f) )
	ROM_LOAD( "sw8.9a",       0x7000, 0x1000, CRC(191a16e4) SHA1(75d3f49e2f4ea04d3a7cc88662c023768bf48365) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sw12.4k",      0x0000, 0x1000, CRC(2eee9bcb) SHA1(ceafdf750a8af0c1c9abbbf437c3e9d9ae09f72b) )

	ROM_REGION( 0x3000, "gfx1", 0 )
	ROM_LOAD( "sw15.18k",     0x0000, 0x1000, CRC(4f3608cb) SHA1(cebcad69c4ad5dacc0bf597fdaed6f8950ffdfe1) )  /* chars */
	ROM_LOAD( "sw14.18l",     0x1000, 0x1000, CRC(7181c8b4) SHA1(b22fa0ebac884002cf6f5651e4366f30d0ab09f5) )
	ROM_LOAD( "sw13.18m",     0x2000, 0x1000, CRC(2eb1af5c) SHA1(0105d03adfc5ce9ca478e678a1e1d8bae7c516e0) )

	ROM_REGION( 0x3000, "gfx2", 0 )
	ROM_LOAD( "sw23.6c",      0x0000, 0x0800, CRC(9ca67e24) SHA1(86f561abc1a1c6b0c29c6017246d805c5a48b999) )  /* bigsprite data */
	ROM_RELOAD(               0x0800, 0x0800 )	/* Guzzler has larger ROMs */
	ROM_LOAD( "sw22.5c",      0x1000, 0x0800, CRC(02c10992) SHA1(8c383fdcd83aa9997e5802a58419b9d993a9b38d) )
	ROM_RELOAD(               0x1800, 0x0800 )	/* Guzzler has larger ROMs */
	ROM_LOAD( "sw21.4c",      0x2000, 0x0800, CRC(7f4993c1) SHA1(a5884b3af707109e810cf1f38bee3cb642e619f6) )
	ROM_RELOAD(               0x2800, 0x0800 )	/* Guzzler has larger ROMs */

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "24s10.13b",    0x0000, 0x100, CRC(8e35b97d) SHA1(2e2c254574660e01b9983f795a2adb5b9911d7f0) )
	ROM_LOAD( "24s10.13a",    0x0100, 0x100, CRC(c5f24909) SHA1(27f2c967d440f6387841aa3f7b116c64bb812af1) )
	ROM_LOAD( "18s030.12c",   0x0200, 0x020, CRC(3b2deb3a) SHA1(bb7b5c662454f5b355cc59cbdf8879e4664bed1d) )
ROM_END
#endif

ROM_START( guzzler )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "guzz-01.bin",  0x0000, 0x2000, CRC(58aaa1e9) SHA1(4ea9c85670a0d71483ac79564093043762a24b2c) )
	ROM_LOAD( "guzz-02.bin",  0x2000, 0x2000, CRC(f80ceb17) SHA1(eedff7355fb5aa18b82f0a3e39bba5521c359791) )
	ROM_LOAD( "guzz-03.bin",  0x4000, 0x2000, CRC(e63c65a2) SHA1(e2b888911330690faa3a041e1a17d838b46e6bbd) )
	ROM_LOAD( "guzz-04.bin",  0x6000, 0x2000, CRC(45be42f5) SHA1(578943afdb6ceca34ca7c19c2fd1164ca3aa57bd) )
	ROM_LOAD( "guzz-16.bin",  0xe000, 0x2000, CRC(61ee00b7) SHA1(ea8516c8dfb2de32a8034f94c7d0c086e3596740) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "guzz-12.bin",  0x0000, 0x1000, CRC(f3754d9e) SHA1(bb30832aba4e82ab0ecce40fc1223d9771ff7dd2) )

	ROM_REGION( 0x3000, "gfx1", 0 )
	ROM_LOAD( "guzz-13.bin",  0x0000, 0x1000, CRC(afc464e2) SHA1(61730b5e5add24ba3d4e8903c5d71cf4df9b77e0) )   /* chars */
	ROM_LOAD( "guzz-14.bin",  0x1000, 0x1000, CRC(acbdfe1f) SHA1(ab7abe4bb321fc7dc4e73acab4b1a7133e6bcf20) )
	ROM_LOAD( "guzz-15.bin",  0x2000, 0x1000, CRC(66978c05) SHA1(2c8d5545f8b1d3cd7cd63448f8064fd3712d6fee) )

	ROM_REGION( 0x3000, "gfx2", 0 )
	ROM_LOAD( "guzz-11.bin",  0x0000, 0x1000, CRC(ec2e9d86) SHA1(2fc631229e78db68777e74a03f98f660f324a885) )   /* big sprite */
	ROM_LOAD( "guzz-10.bin",  0x1000, 0x1000, CRC(bd3f0bf7) SHA1(c57aff05812801c22104a4afc8a8a6bca33dda96) )
	ROM_LOAD( "guzz-09.bin",  0x2000, 0x1000, CRC(18927579) SHA1(414676193ef1f6ce79a4cba73e4d017312f766f4) )

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "guzzler.003",  0x0000, 0x100, CRC(f86930c1) SHA1(58efc8cbef05e1612d12e2f0babddf15571d42bb) )
	ROM_LOAD( "guzzler.002",  0x0100, 0x100, CRC(b566ea9e) SHA1(345078af6a339fbe6cd966046acd9d04c8926b5c) )
	ROM_LOAD( "guzzler.001",  0x0200, 0x020, CRC(69089495) SHA1(96b067b22be14536bac748f8d61e5587a8a04e92) )
ROM_END

ROM_START( yamato )
	ROM_REGION( 2*0x10000, "maincpu", 0 )
	ROM_LOAD( "2.5de",        0x0000, 0x2000, CRC(20895096) SHA1(af76786e3c519e710899f143d46c53087e9817c7) )
	ROM_LOAD( "3.5f",         0x2000, 0x2000, CRC(57a696f9) SHA1(28ea80fb100ac92295fc3eb318617d7cb014408d) )
	ROM_LOAD( "4.5jh",        0x4000, 0x2000, CRC(59a468e8) SHA1(a79cdee6efefd87a356cc8d710f8050bc12e07c3) )
	/* hole at 6000-6fff */
	ROM_LOAD( "11.5a",        0x7000, 0x1000, CRC(35987485) SHA1(1f0cb545bbd52982cbf801bc1dd2c4087af2f5f7) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1.5v",         0x0000, 0x0800, CRC(3aad9e3c) SHA1(37b0414b265397881bb45b166ecab85880d1358d) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "10.11k",       0x0000, 0x2000, CRC(161121f5) SHA1(017c5c6b773b0ae1d0be52e4bac90b699ea196dd) )
	ROM_LOAD( "9.11h",        0x2000, 0x2000, CRC(56e84cc4) SHA1(c48e0e5460376d6b34173c42a27907ef12218182) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "8.11c",        0x0000, 0x1000, CRC(28024d9a) SHA1(c871c4d74be72a8bfea99e89d43f91922f4b734b) )
	ROM_LOAD( "7.11a",        0x1000, 0x1000, CRC(4a179790) SHA1(7fb6b033de939ff8bd13055c073311dca2c1a6fe) )

	ROM_REGION( 0x2000, "user1", 0 )
	ROM_LOAD( "5.5lm",        0x0000, 0x1000, CRC(7761ad24) SHA1(98878b19addd142d35718080eece05eaaee0388d) )	/* ?? */
	ROM_LOAD( "6.5n",         0x1000, 0x1000, CRC(da48444c) SHA1(a43e672ce262eb817fb4e5715ef4fb304a6a2815) )

	ROM_REGION( 0x00a0, "proms", 0 )
	ROM_LOAD( "1.bpr",        0x0000, 0x0020, CRC(ef2053ab) SHA1(2006cbf003f90a8e75f39047a88a3bba85d78e80) )
	ROM_LOAD( "2.bpr",        0x0020, 0x0020, CRC(2281d39f) SHA1(e9b568bdacf7ab611801cf42ea5c7624f5440ef6) )
	ROM_LOAD( "3.bpr",        0x0040, 0x0020, CRC(9e6341e3) SHA1(2e7a4d3c1f40d6089735734b9d9de2ca57fb73c7) )
	ROM_LOAD( "4.bpr",        0x0060, 0x0020, CRC(1c97dc0b) SHA1(fe8e0a91172abdd2d14b199da144306a9b944372) )
	ROM_LOAD( "5.bpr",        0x0080, 0x0020, CRC(edd6c05f) SHA1(b95db8aaf74fe175d1179f0d85f79242b16f5fb4) )
ROM_END

#if 0
ROM_START( yamato2 )
	ROM_REGION( 2*0x10000, "maincpu", 0 )
	ROM_LOAD( "2-2.5de",      0x0000, 0x2000, CRC(93da1d52) SHA1(21b72856ebbd969e4e075b52719e6acdbd1bc4c5) )
	ROM_LOAD( "3-2.5f",       0x2000, 0x2000, CRC(31e73821) SHA1(e582c9fcea1b29d43f65b6aa67e1895c38d2736c) )
	ROM_LOAD( "4-2.5jh",      0x4000, 0x2000, CRC(fd7bcfc3) SHA1(5037170cb3a9824794e90d74def92b0b25d45caa) )
	/* hole at 6000-6fff */
	/* 7000-7fff not present here */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1.5v",         0x0000, 0x0800, CRC(3aad9e3c) SHA1(37b0414b265397881bb45b166ecab85880d1358d) )

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "10.11k",       0x0000, 0x2000, CRC(161121f5) SHA1(017c5c6b773b0ae1d0be52e4bac90b699ea196dd) )
	ROM_LOAD( "9.11h",        0x2000, 0x2000, CRC(56e84cc4) SHA1(c48e0e5460376d6b34173c42a27907ef12218182) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "8.11c",        0x0000, 0x1000, CRC(28024d9a) SHA1(c871c4d74be72a8bfea99e89d43f91922f4b734b) )
	ROM_LOAD( "7.11a",        0x1000, 0x1000, CRC(4a179790) SHA1(7fb6b033de939ff8bd13055c073311dca2c1a6fe) )

	ROM_REGION( 0x2000, "user1", 0 )
	ROM_LOAD( "5.5lm",        0x0000, 0x1000, CRC(7761ad24) SHA1(98878b19addd142d35718080eece05eaaee0388d) )	/* ?? */
	ROM_LOAD( "6.5n",         0x1000, 0x1000, CRC(da48444c) SHA1(a43e672ce262eb817fb4e5715ef4fb304a6a2815) )

	ROM_REGION( 0x00a0, "proms", 0 )
	ROM_LOAD( "1.bpr",        0x0000, 0x0020, CRC(ef2053ab) SHA1(2006cbf003f90a8e75f39047a88a3bba85d78e80) )
	ROM_LOAD( "2.bpr",        0x0020, 0x0020, CRC(2281d39f) SHA1(e9b568bdacf7ab611801cf42ea5c7624f5440ef6) )
	ROM_LOAD( "3.bpr",        0x0040, 0x0020, CRC(9e6341e3) SHA1(2e7a4d3c1f40d6089735734b9d9de2ca57fb73c7) )
	ROM_LOAD( "4.bpr",        0x0060, 0x0020, CRC(1c97dc0b) SHA1(fe8e0a91172abdd2d14b199da144306a9b944372) )
	ROM_LOAD( "5.bpr",        0x0080, 0x0020, CRC(edd6c05f) SHA1(b95db8aaf74fe175d1179f0d85f79242b16f5fb4) )
ROM_END
#endif

ROM_START( toprollr )
	ROM_REGION( 0x10000*2, "maincpu", 0 )
	ROM_LOAD( "10.k3", 0xc000, 0x2000, CRC(1e8914a6) SHA1(ec17f185f890d04ce75a5d8edf8b32da60e7a8d8) )
	ROM_LOAD( "11.l3", 0xe000, 0x2000, CRC(b20a9fa2) SHA1(accd3296447eca002b0808e7b02832f5e35407e8) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "16.j4", 0x0000, 0x2000, CRC(ce3afe26) SHA1(7de00720f091537c64cc0fec687c061de3a8b1a3) )
	ROM_LOAD( "15.h4", 0x2000, 0x2000, CRC(1d9e3325) SHA1(e7f6863aa2ba2aeec40cfcc5cf6c69e947c185b5) )

	ROM_REGION( 0x04000, "gfx2", 0 )
	ROM_LOAD( "14.c4", 0x0000, 0x2000, CRC(7a945733) SHA1(14187ba303aecf0a812c425c34d8edda3deaa2b5) )
	ROM_LOAD( "13.a4", 0x2000, 0x2000, CRC(89327329) SHA1(555331a3136aa8c5bb35b97dd54bc59da067be57) )

	ROM_REGION( 0x2000, "gfx3", 0 )
	ROM_LOAD( "6.m5",  0x0000, 0x1000, CRC(e30c1dd8) SHA1(1777bf98625153c9b191020860e4e1839b46b998) )
	ROM_LOAD( "5.l5",  0x1000, 0x1000, CRC(84139f46) SHA1(976f781fb279dd540778708174b942a263f16443) )

	ROM_REGION( 0x12000, "user1", 0 )
	ROM_LOAD( "2.f5",	0x00000, 0x02000, CRC(ef789f00) SHA1(424d69584d391ee7b9ad5db7ee6ced97d69897d4) )
	ROM_LOAD( "8.f3",	0x02000, 0x02000, CRC(94371cfb) SHA1(cb501c36b213c995a4048b3a96c85848c556cd05) )
	ROM_LOAD( "4.k5",	0x04000, 0x02000, CRC(1cb48ea0) SHA1(fdc75075112042ec84a7d1b3e5b5a6db1d1cb871) )
	ROM_COPY( "user1", 0x04000, 0x0a000, 0x02000 )
	ROM_COPY( "user1", 0x04000, 0x10000, 0x02000 )
	ROM_LOAD( "3.h5",	0x06000, 0x02000, CRC(d45494ba) SHA1(6e235b34f9457acadad6d4e27799978bc2e3db08) )
	ROM_LOAD( "9.h3",	0x08000, 0x02000, CRC(8a8032a7) SHA1(d6642d72645c613c21f65bbbe1560d0437d41f43) )
	ROM_LOAD( "1.d5",	0x0c000, 0x02000, CRC(9894374d) SHA1(173de4abbc3fb5d522aa6d6d5caf8e4d54f2a598) )
	ROM_LOAD( "7.d3",	0x0e000, 0x02000, CRC(904fffb6) SHA1(5528bc2a4d2fe8672428fd4725644265f0d57ded) )

	ROM_REGION( 0x2000, "samples", 0 )
	ROM_LOAD( "12.p3",  0x0000, 0x2000, CRC(7f989dc9) SHA1(3b4d18cbb992872b3cf8f5eaf5381ed3a9468cc1) )

	ROM_REGION( 0x01a0, "proms", 0 )
	ROM_LOAD( "prom.p2",  0x0000, 0x0020, CRC(42e828fa) SHA1(81250b1f7c3956b3902324adbbaf3b5989e854ee) ) //08-0f sprites + fg (wrong?)
	ROM_LOAD( "prom.r2",  0x0020, 0x0020, CRC(99b87eed) SHA1(06c3164d681fe4aff0338c0dad1a921f7fe7369d) ) //10-17 sprites
	ROM_LOAD( "prom.a1",  0x0040, 0x0020, CRC(7d626d6c) SHA1(7c7202d0ec5bf0381e7104eef53afa5fa4596a29) ) //00-07 big sprites
	ROM_LOAD( "prom.p9",  0x0060, 0x0020, CRC(eb399c02) SHA1(bf3d6c6dd982cb54446cf8a010b7adb949514bdb) ) //18-1f bg
	ROM_LOAD( "prom.n9",  0x0080, 0x0020, CRC(fb03ea99) SHA1(4dcef86106cef713dfcbd965072bfa8fe4b68e15) ) //20-27 bg
	ROM_LOAD( "prom.s9",  0x00a0, 0x0100, CRC(abf4c5fb) SHA1(a953f14642d4b72328293b36bc3c65b13491ffff) ) //unknown prom (filled with 2 bit vals)

ROM_END


static DRIVER_INIT( yamato )
{
	yamato_decode(machine,"maincpu");
}


static DRIVER_INIT( toprollr )
{
	toprollr_decode(machine, "maincpu", "user1");
}


GAME( 1980, cclimber, 0,        cclimber, cclimber, cclimber, ROT0,   "Nichibutsu", "Crazy Climber (US)", 0 )
//GAME( 1980, cclimberj,cclimber, cclimber, cclimbrj, cclimbrj, ROT0,   "Nichibutsu", "Crazy Climber (Japan)", 0 )
//GAME( 1980, ccboot,   cclimber, cclimber, cclimber, cclimbrj, ROT0,   "bootleg", "Crazy Climber (bootleg set 1)", 0 )
//GAME( 1980, ccboot2,  cclimber, cclimber, cclimber, cclimbrj, ROT0,   "bootleg", "Crazy Climber (bootleg set 2)", 0 )
GAME( 1981, ckong,    0,        cclimber, ckong,    0,        ROT270, "Falcon", "Crazy Kong Part II (set 1)", 0 )
//GAME( 1981, ckonga,   ckong,    cclimber, ckong,    0,        ROT270, "Falcon", "Crazy Kong Part II (set 2)", 0 )
//GAME( 1981, ckong2j,  ckong,    cclimber, ckong,    0,        ROT270, "Falcon", "Crazy Kong Part II (Japan)", 0 )
//GAME( 1981, ckongjeu, ckong,    cclimber, ckong,    0,        ROT270, "bootleg", "Crazy Kong Part II (Jeutel bootleg)", 0 )
//GAME( 1981, ckongo,   ckong,    cclimber, ckong,    0,        ROT270, "bootleg", "Crazy Kong (Orca bootleg)", 0 )
//GAME( 1981, ckongalc, ckong,    cclimber, ckong,    0,        ROT270, "bootleg", "Crazy Kong (Alca bootleg)", 0 )
//GAME( 1981, bigkong,  ckong,    cclimber, ckong,    0,        ROT270, "bootleg", "Big Kong", 0 )
//GAME( 1981, monkeyd,  ckong,    cclimber, ckong,    0,        ROT270, "bootleg", "Monkey Donkey", 0 )
//GAME( 198?, ckongb,   ckong,    cclimber, ckongb,   ckongb,   ROT270, "bootleg", "Crazy Kong Part II (alternative levels)", 0 )
GAME( 1981, rpatrol,  0,        cclimber, rpatrol,  0,        ROT0,   "Orca", "River Patrol (Orca)", 0 )
//GAME( 1981, rpatrolb, rpatrol,  cclimber, rpatrol,  0,        ROT0,   "bootleg", "River Patrol (bootleg)", 0 )
//GAME( 1981, silvland, rpatrol,  cclimber, rpatrol,  0,        ROT0,   "Falcon", "Silver Land", 0 )
//GAME( 1985, cannonb,  cannonbp, cannonb,  cannonb,  cannonb,  ROT90,  "Soft", "Cannon Ball (bootleg on Crazy Climber hardware, set 1)" , GAME_IMPERFECT_GRAPHICS )
GAME( 1985, cannonb2, cannonbp, cannonb,  cannonb,  cannonb2, ROT90,  "TV Game Gruenberg", "Cannon Ball (bootleg on Crazy Climber hardware, set 2)", 0 )
GAME( 1982, swimmer,  0,        swimmer,  swimmer,  0,        ROT0,   "Tehkan", "Swimmer (set 1)", 0 )
//GAME( 1982, swimmera, swimmer,  swimmer,  swimmer,  0,        ROT0,   "Tehkan", "Swimmer (set 2)", 0 )
//GAME( 1982, swimmerb, swimmer,  swimmer,  swimmerb, 0,        ROT0,   "Tehkan", "Swimmer (set 3)", 0 )
GAME( 1983, guzzler,  0,        swimmer,  guzzler,  0,        ROT90,  "Tehkan", "Guzzler", 0 )
GAME( 1983, yamato,   0,        yamato,   yamato,   yamato,   ROT90,  "Sega", "Yamato (US)", GAME_IMPERFECT_GRAPHICS )
//GAME( 1983, yamato2,  yamato,   yamato,   yamato,   yamato,   ROT90,  "Sega", "Yamato (World?)", GAME_IMPERFECT_GRAPHICS )
GAME( 1983, toprollr, 0,        toprollr, toprollr, toprollr, ROT90,  "Jaleco", "Top Roller", GAME_IMPERFECT_COLORS )
