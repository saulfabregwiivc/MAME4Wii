/***************************************************************************

        Circus Charlie   GX380 (c) 1984 Konami

         Upper board PWB (B)3000148  - OSC 14.31818MHz
         Lower board PWB (A)3000161  - OSC 18432.00KHz


Based on drivers from Juno First emulator by Chris Hardy (chrish@kcbbs.gen.nz)

To enter service mode, keep 1&2 pressed on reset


'circusc2' ingame bug :
-----------------------

  "Test mode" displays 2, 3, 4 and 7 lives instead of 3, 4, 5 and 7
  due to code at 0xcb38 :

    CB38: B6 30 88         LDA   $1800
    CB3B: 43               COMA
    CB3C: 84 2B            ANDA  #$03
    CB3E: 81 8B            CMPA  #$03
    CB40: 25 20            BCS   $CB44
    CB42: 86 87            LDA   #$05
    CB44: 8B 20            ADDA  #$02

  In other sets, you have the following (code from 'circusc') :

    CB38: B6 30 88         LDA   $1800
    CB3B: 43               COMA
    CB3C: 84 2B            ANDA  #$03
    CB3E: 81 8B            CMPA  #$03
    CB40: 25 20            BCS   $CB44
    CB42: 86 86            LDA   #$04
    CB44: 8B 21            ADDA  #$03

  Ingame lives are correct though (same code for 'circusc' and 'circusc2') :

    6B93: 96 0D            LDA   $2F
    6B95: 84 81            ANDA  #$03
    6B97: 8B 2B            ADDA  #$03
    6B99: 81 8E            CMPA  #$06
    6B9B: 25 2A            BCS   $6B9F
    6B9D: 86 8F            LDA   #$07


***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "machine/konami1.h"
#include "cpu/m6809/m6809.h"
#include "sound/dac.h"
#include "sound/sn76496.h"
#include "sound/discrete.h"

extern UINT8 *circusc_spritebank;
extern UINT8 *circusc_scroll;
extern UINT8 *circusc_videoram,*circusc_colorram;

static UINT8 sn_latch;

WRITE8_HANDLER( circusc_videoram_w );
WRITE8_HANDLER( circusc_colorram_w );

VIDEO_START( circusc );
WRITE8_HANDLER( circusc_flipscreen_w );
PALETTE_INIT( circusc );
VIDEO_UPDATE( circusc );



static MACHINE_START( circusc )
{
	state_save_register_global(machine, sn_latch);
}

static MACHINE_RESET( circusc )
{
	sn_latch = 0;
}

static READ8_HANDLER( circusc_sh_timer_r )
{
	/* This port reads the output of a counter clocked from the CPU clock.
     * The CPU XTAL is 14.31818MHz divided by 4.  It then goes through 10
     * /2 stages to clock a 4 bit counter.  The output of the counter goes
     * to D1-D4.
     *
     * The following:
     * clock = cpu_get_total_cycles(space->cpu) >> 10;
     * return (clock & 0x0f) << 1;
     * Can be shortened to:
     */

	int clock;

	clock = cpu_get_total_cycles(space->cpu) >> 9;

	return clock & 0x1e;
}

static WRITE8_HANDLER( circusc_sh_irqtrigger_w )
{
	cputag_set_input_line_and_vector(space->machine, "audiocpu", 0, HOLD_LINE, 0xff);
}

static WRITE8_HANDLER( circusc_coin_counter_w )
{
	coin_counter_w(offset,data);
}

static WRITE8_HANDLER(circusc_sound_w)
{
	const device_config *device;
	//int c;

	switch (offset & 7)
	{
		/* CS2 */
		case 0:
			sn_latch = data;
			break;

		/* CS3 */
		case 1:
			device = devtag_get_device(space->machine, "sn1");
			sn76496_w(device, 0, sn_latch);
			break;

		/* CS4 */
		case 2:
			device = devtag_get_device(space->machine, "sn2");
			sn76496_w(device, 0, sn_latch);
			break;

		/* CS5 */
		case 3:
			device = devtag_get_device(space->machine, "dac");
			dac_w(device, 0, data);
			break;

		/* CS6 */
		case 4:
			device = devtag_get_device(space->machine, "fltdisc");
			discrete_sound_w(device, NODE_05, (offset & 0x20) >> 5);
			discrete_sound_w(device, NODE_06, (offset & 0x18) >> 3);
			discrete_sound_w(device, NODE_07, (offset & 0x40) >> 6);
			break;
	}
}



static ADDRESS_MAP_START( circusc_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0000) AM_MIRROR(0x03f8) AM_WRITE(circusc_flipscreen_w)		/* FLIP */
	AM_RANGE(0x0001, 0x0001) AM_MIRROR(0x03f8) AM_WRITE(interrupt_enable_w)			/* INTST */
//  AM_RANGE(0x0002, 0x0002) AM_MIRROR(0x03f8) AM_WRITENOP                          /* MUT - not used /*
	AM_RANGE(0x0003, 0x0004) AM_MIRROR(0x03f8) AM_WRITE(circusc_coin_counter_w)		/* COIN1, COIN2 */
	AM_RANGE(0x0005, 0x0005) AM_MIRROR(0x03f8) AM_WRITE(SMH_RAM) AM_BASE(&circusc_spritebank) /* OBJ CHENG */
	AM_RANGE(0x0400, 0x0400) AM_MIRROR(0x03ff) AM_WRITE(watchdog_reset_w)			/* WDOG */
	AM_RANGE(0x0800, 0x0800) AM_MIRROR(0x03ff) AM_WRITE(soundlatch_w)				/* SOUND DATA */
	AM_RANGE(0x0c00, 0x0c00) AM_MIRROR(0x03ff) AM_WRITE(circusc_sh_irqtrigger_w)	/* SOUND-ON causes interrupt on audio CPU */
	AM_RANGE(0x1000, 0x1000) AM_MIRROR(0x03fc) AM_READ_PORT("SYSTEM")
	AM_RANGE(0x1001, 0x1001) AM_MIRROR(0x03fc) AM_READ_PORT("P1")
	AM_RANGE(0x1002, 0x1002) AM_MIRROR(0x03fc) AM_READ_PORT("P2")
	AM_RANGE(0x1003, 0x1003) AM_MIRROR(0x03fc) AM_READNOP              /* unpopulated DIPSW 3*/
	AM_RANGE(0x1400, 0x1400) AM_MIRROR(0x03ff) AM_READ_PORT("DSW1")
	AM_RANGE(0x1800, 0x1800) AM_MIRROR(0x03ff) AM_READ_PORT("DSW2")
	AM_RANGE(0x1c00, 0x1c00) AM_MIRROR(0x03ff) AM_WRITE(SMH_RAM) AM_BASE(&circusc_scroll) /* VGAP */
	AM_RANGE(0x2000, 0x2fff) AM_RAM
	AM_RANGE(0x3000, 0x33ff) AM_RAM_WRITE(circusc_colorram_w) AM_BASE(&circusc_colorram) /* colorram */
	AM_RANGE(0x3400, 0x37ff) AM_RAM_WRITE(circusc_videoram_w) AM_BASE(&circusc_videoram) /* videoram */
	AM_RANGE(0x3800, 0x38ff) AM_RAM AM_BASE(&spriteram_2) /* spriteram2 */
	AM_RANGE(0x3900, 0x39ff) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size) /* spriteram */
	AM_RANGE(0x3a00, 0x3fff) AM_RAM
	AM_RANGE(0x6000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x43ff) AM_MIRROR(0x1c00) AM_RAM
	AM_RANGE(0x6000, 0x6000) AM_MIRROR(0x1fff) AM_READ(soundlatch_r)		/* CS0 */
	AM_RANGE(0x8000, 0x8000) AM_MIRROR(0x1fff) AM_READ(circusc_sh_timer_r)	/* CS1 */
	AM_RANGE(0xa000, 0xa07f) AM_MIRROR(0x1f80) AM_WRITE(circusc_sound_w)	/* CS2 - CS6 */
ADDRESS_MAP_END



static INPUT_PORTS_START( circusc )
	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* SW7 of 8 on unpopulated DIPSW 3 */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )	/* 1P UP - unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )	/* 1P DOWN - unused */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )	/* 1P SHOOT2 - unused */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )	/* 2P UP - unused */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )	/* 2P DOWN - unused */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )	/* 2P SHOOT2 - unused */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )	PORT_DIPLOCATION("SW1:1,2,3,4")
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )	PORT_DIPLOCATION("SW1:5,6,7,8")
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )		PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )		PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Bonus_Life ) )	PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x08, "20k 90k 70k+" )
	PORT_DIPSETTING(    0x00, "30k 110k 80k+" )
	PORT_DIPUNUSED_DIPLOC( 0x10, 0x00, "SW2:5" )
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )	PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(    0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )	PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	512,    /* 512 characters */
	4,      /* 4 bits per pixel */
	{ 0, 1, 2, 3 }, /* the four bitplanes are packed in one nibble */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8    /* every char takes 8 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,  /* 16*16 sprites */
	384,    /* 384 sprites */
	4,      /* 4 bits per pixel */
	{ 0, 1, 2, 3 },        /* the bitplanes are packed */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			8*4, 9*4, 10*4, 11*4, 12*4, 13*4, 14*4, 15*4 },
	{ 0*4*16, 1*4*16, 2*4*16, 3*4*16, 4*4*16, 5*4*16, 6*4*16, 7*4*16,
			8*4*16, 9*4*16, 10*4*16, 11*4*16, 12*4*16, 13*4*16, 14*4*16, 15*4*16 },
	32*4*8    /* every sprite takes 128 consecutive bytes */
};

static GFXDECODE_START( circusc )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,       0, 16 )
	GFXDECODE_ENTRY( "gfx2", 0, spritelayout, 16*16, 16 )
GFXDECODE_END

static const discrete_mixer_desc circusc_mixer_desc =
	{DISC_MIXER_IS_RESISTOR,
		{RES_K(2.2), RES_K(2.2), RES_K(10)},
		{0,0,0},	// no variable resistors
		{0,0,0},  // no node capacitors
		0, RES_K(1),
		CAP_U(0.1),
		CAP_U(0.47),
		0, 1};

static DISCRETE_SOUND_START( circusc )

	DISCRETE_INPUTX_STREAM(NODE_01, 0, 1.0, 0)
	DISCRETE_INPUTX_STREAM(NODE_02, 1, 1.0, 0)
	DISCRETE_INPUTX_STREAM(NODE_03, 2, 2.0, 0) // DAC 0..32767, multiply by 2

	DISCRETE_INPUT_DATA(NODE_05)
	DISCRETE_INPUT_DATA(NODE_06)
	DISCRETE_INPUT_DATA(NODE_07)

	DISCRETE_RCFILTER_SW(NODE_10, 1, NODE_01, NODE_05, 1000, CAP_U(0.47), 0, 0, 0)
	DISCRETE_RCFILTER_SW(NODE_11, 1, NODE_02, NODE_06, 1000, CAP_U(0.047), CAP_U(0.47), 0, 0)
	DISCRETE_RCFILTER_SW(NODE_12, 1, NODE_03, NODE_07, 1000, CAP_U(0.47), 0, 0, 0)

	DISCRETE_MIXER3(NODE_20, 1, NODE_10, NODE_11, NODE_12, &circusc_mixer_desc)

	DISCRETE_OUTPUT(NODE_20, 10.0 )

DISCRETE_SOUND_END

static MACHINE_DRIVER_START( circusc )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M6809, 2048000)        /* 2 MHz */
	MDRV_CPU_PROGRAM_MAP(circusc_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)
	MDRV_WATCHDOG_VBLANK_INIT(8)

	MDRV_CPU_ADD("audiocpu", Z80,14318180/4)     /* Z80 Clock is derived from a 14.31818 MHz crystal */
	MDRV_CPU_PROGRAM_MAP(sound_map)

	MDRV_MACHINE_START(circusc)
	MDRV_MACHINE_RESET(circusc)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(circusc)
	MDRV_PALETTE_LENGTH(16*16+16*16)

	MDRV_PALETTE_INIT(circusc)
	MDRV_VIDEO_START(circusc)
	MDRV_VIDEO_UPDATE(circusc)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("sn1", SN76496, 14318180/8)
	MDRV_SOUND_ROUTE_EX(0, "fltdisc", 1.0, 0)

	MDRV_SOUND_ADD("sn2", SN76496, 14318180/8)
	MDRV_SOUND_ROUTE_EX(0, "fltdisc", 1.0, 1)

	MDRV_SOUND_ADD("dac", DAC, 0)
	MDRV_SOUND_ROUTE_EX(0, "fltdisc", 1.0, 2)

	MDRV_SOUND_ADD("fltdisc", DISCRETE, 0)

	MDRV_SOUND_CONFIG_DISCRETE(circusc)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( circusc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "380_s05.3h",   0x6000, 0x2000, CRC(48feafcf) SHA1(0e5bd350fa5fee42569eb0c4accf7512d645b792) )
	ROM_LOAD( "380_r04.4h",   0x8000, 0x2000, CRC(c283b887) SHA1(458c398911453d558003f49c298b0d593c941c11) )
	ROM_LOAD( "380_r03.5h",   0xa000, 0x2000, CRC(e90c0e86) SHA1(03211f0cc90b6e356989c5e2a41b70f4ff2ead83) )
	ROM_LOAD( "380_q02.6h",   0xc000, 0x2000, CRC(4d847dc6) SHA1(a1f65e73c4e5abff1b0970bad32a128173245561) )
	ROM_LOAD( "380_q01.7h",   0xe000, 0x2000, CRC(18c20adf) SHA1(2f40e1a109d129bb127a8b98e27817988cd08c8b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "380_l14.5c",   0x0000, 0x2000, CRC(607df0fb) SHA1(67103d61994fd3a1e2de7cf9487e4f763234b18e) )
	ROM_LOAD( "380_l15.7c",   0x2000, 0x2000, CRC(a6ad30e1) SHA1(14f305717edcc2471e763b262960a0b96eef3530) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "380_j12.4a",   0x0000, 0x2000, CRC(56e5b408) SHA1(73b9e3d46dfe9e39b390c634df153648a0906876) )
	ROM_LOAD( "380_j13.5a",   0x2000, 0x2000, CRC(5aca0193) SHA1(4d0b0a773c385b7f1dcf024760d0437f47e78fbe) )

	ROM_REGION( 0x0c000, "gfx2", 0 )
	ROM_LOAD( "380_j06.11e",  0x0000, 0x2000, CRC(df0405c6) SHA1(70a50dcc86dfbdaa9c2af613105aae7f90747804) )
	ROM_LOAD( "380_j07.12e",  0x2000, 0x2000, CRC(23dfe3a6) SHA1(2ad7cbcbdbb434dc43e9c94cd00df9e57ac097f5) )
	ROM_LOAD( "380_j08.13e",  0x4000, 0x2000, CRC(3ba95390) SHA1(b22ad7cfda392894208eb4b39505f38bfe4c4342) )
	ROM_LOAD( "380_j09.14e",  0x6000, 0x2000, CRC(a9fba85a) SHA1(1a649ec667d377ffab26b4694be790b3a2742f30) )
	ROM_LOAD( "380_j10.15e",  0x8000, 0x2000, CRC(0532347e) SHA1(4c02b75a62993cce60d2cb87b81c7738abbc9a0d) )
	ROM_LOAD( "380_j11.16e",  0xa000, 0x2000, CRC(e1725d24) SHA1(d315588e6cc2f4263be621d2d8603c8215a90046) )

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "380_j18.2a",   0x0000, 0x020, CRC(10dd4eaa) SHA1(599acd25f36445221c553510a5de23ddba5ecc15) ) /* palette */
	ROM_LOAD( "380_j17.7b",   0x0020, 0x100, CRC(13989357) SHA1(0d61d468f6d3e1570fd18d236ec8cab92db4ed5c) ) /* character lookup table */
	ROM_LOAD( "380_j16.10c",  0x0120, 0x100, CRC(c244f2aa) SHA1(86df21c8e0b1ed51a0a4bd33dbb33f6efdea7d39) ) /* sprite lookup table */
ROM_END

#if 0
ROM_START( circusc2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "380_unk.3h",   0x6000, 0x2000, CRC(87df9f5e) SHA1(d759ff6200546c562aecee749dc9941bbbdb9918) ) /* Actual Konami revision unknown, possibly "Q" or "T" */
	ROM_LOAD( "380_r04.4h",   0x8000, 0x2000, CRC(c283b887) SHA1(458c398911453d558003f49c298b0d593c941c11) )
	ROM_LOAD( "380_r03.5h",   0xa000, 0x2000, CRC(e90c0e86) SHA1(03211f0cc90b6e356989c5e2a41b70f4ff2ead83) )
	ROM_LOAD( "380_r02.6h",   0xc000, 0x2000, CRC(2d434c6f) SHA1(2c794f24422db7671d1bc85cef308ab4a62d523d) )
	ROM_LOAD( "380_q01.7h",   0xe000, 0x2000, CRC(18c20adf) SHA1(2f40e1a109d129bb127a8b98e27817988cd08c8b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "380_l14.5c",   0x0000, 0x2000, CRC(607df0fb) SHA1(67103d61994fd3a1e2de7cf9487e4f763234b18e) )
	ROM_LOAD( "380_l15.7c",   0x2000, 0x2000, CRC(a6ad30e1) SHA1(14f305717edcc2471e763b262960a0b96eef3530) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "380_j12.4a",   0x0000, 0x2000, CRC(56e5b408) SHA1(73b9e3d46dfe9e39b390c634df153648a0906876) )
	ROM_LOAD( "380_j13.5a",   0x2000, 0x2000, CRC(5aca0193) SHA1(4d0b0a773c385b7f1dcf024760d0437f47e78fbe) )

	ROM_REGION( 0x0c000, "gfx2", 0 )
	ROM_LOAD( "380_j06.11e",  0x0000, 0x2000, CRC(df0405c6) SHA1(70a50dcc86dfbdaa9c2af613105aae7f90747804) )
	ROM_LOAD( "380_j07.12e",  0x2000, 0x2000, CRC(23dfe3a6) SHA1(2ad7cbcbdbb434dc43e9c94cd00df9e57ac097f5) )
	ROM_LOAD( "380_j08.13e",  0x4000, 0x2000, CRC(3ba95390) SHA1(b22ad7cfda392894208eb4b39505f38bfe4c4342) )
	ROM_LOAD( "380_j09.14e",  0x6000, 0x2000, CRC(a9fba85a) SHA1(1a649ec667d377ffab26b4694be790b3a2742f30) )
	ROM_LOAD( "380_j10.15e",  0x8000, 0x2000, CRC(0532347e) SHA1(4c02b75a62993cce60d2cb87b81c7738abbc9a0d) )
	ROM_LOAD( "380_j11.16e",  0xa000, 0x2000, CRC(e1725d24) SHA1(d315588e6cc2f4263be621d2d8603c8215a90046) )

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "380_j18.2a",   0x0000, 0x020, CRC(10dd4eaa) SHA1(599acd25f36445221c553510a5de23ddba5ecc15) ) /* palette */
	ROM_LOAD( "380_j17.7b",   0x0020, 0x100, CRC(13989357) SHA1(0d61d468f6d3e1570fd18d236ec8cab92db4ed5c) ) /* character lookup table */
	ROM_LOAD( "380_j16.10c",  0x0120, 0x100, CRC(c244f2aa) SHA1(86df21c8e0b1ed51a0a4bd33dbb33f6efdea7d39) ) /* sprite lookup table */
ROM_END
#endif
#if 0
ROM_START( circusc3 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "380_r05.3h",   0x6000, 0x2000, CRC(ed52c60f) SHA1(aa9dc6a57e29895be521ac6a146de56a7beef957) )
	ROM_LOAD( "380_n04.4h",   0x8000, 0x2000, CRC(fcc99e33) SHA1(da140a849ac22419e8890414b8984aa264f7e3c7) )
	ROM_LOAD( "380_n03.5h",   0xa000, 0x2000, CRC(5ef5b3b5) SHA1(b058600c915a0d6653eaa5fc87ecee44a38eed00) )
	ROM_LOAD( "380_n02.6h",   0xc000, 0x2000, CRC(a5a5e796) SHA1(a41700b272ff4198447ed75138d65ec3a759d221) )
	ROM_LOAD( "380_n01.7h",   0xe000, 0x2000, CRC(70d26721) SHA1(eb71cb0da26991a3628150f45f1389c2f2ef90fc) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "380_l14.5c",   0x0000, 0x2000, CRC(607df0fb) SHA1(67103d61994fd3a1e2de7cf9487e4f763234b18e) )
	ROM_LOAD( "380_l15.7c",   0x2000, 0x2000, CRC(a6ad30e1) SHA1(14f305717edcc2471e763b262960a0b96eef3530) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "380_j12.4a",   0x0000, 0x2000, CRC(56e5b408) SHA1(73b9e3d46dfe9e39b390c634df153648a0906876) )
	ROM_LOAD( "380_j13.5a",   0x2000, 0x2000, CRC(5aca0193) SHA1(4d0b0a773c385b7f1dcf024760d0437f47e78fbe) )

	ROM_REGION( 0x0c000, "gfx2", 0 )
	ROM_LOAD( "380_j06.11e",  0x0000, 0x2000, CRC(df0405c6) SHA1(70a50dcc86dfbdaa9c2af613105aae7f90747804) )
	ROM_LOAD( "380_j07.12e",  0x2000, 0x2000, CRC(23dfe3a6) SHA1(2ad7cbcbdbb434dc43e9c94cd00df9e57ac097f5) )
	ROM_LOAD( "380_j08.13e",  0x4000, 0x2000, CRC(3ba95390) SHA1(b22ad7cfda392894208eb4b39505f38bfe4c4342) )
	ROM_LOAD( "380_j09.14e",  0x6000, 0x2000, CRC(a9fba85a) SHA1(1a649ec667d377ffab26b4694be790b3a2742f30) )
	ROM_LOAD( "380_j10.15e",  0x8000, 0x2000, CRC(0532347e) SHA1(4c02b75a62993cce60d2cb87b81c7738abbc9a0d) )
	ROM_LOAD( "380_j11.16e",  0xa000, 0x2000, CRC(e1725d24) SHA1(d315588e6cc2f4263be621d2d8603c8215a90046) )

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "380_j18.2a",   0x0000, 0x020, CRC(10dd4eaa) SHA1(599acd25f36445221c553510a5de23ddba5ecc15) ) /* palette */
	ROM_LOAD( "380_j17.7b",   0x0020, 0x100, CRC(13989357) SHA1(0d61d468f6d3e1570fd18d236ec8cab92db4ed5c) ) /* character lookup table */
	ROM_LOAD( "380_j16.10c",  0x0120, 0x100, CRC(c244f2aa) SHA1(86df21c8e0b1ed51a0a4bd33dbb33f6efdea7d39) ) /* sprite lookup table */
ROM_END
#endif
#if 0
ROM_START( circuscc ) /* Version U */
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "380_u05.3h",   0x6000, 0x2000, CRC(964c035a) SHA1(bd69bb755be327d04fc95cd33115663b33b33ed3) )
	ROM_LOAD( "380_p04.4h",   0x8000, 0x2000, CRC(dd0c0ee7) SHA1(e56e48f6f251430b7ce0e2cc59cfd00b5c760b9c) )
	ROM_LOAD( "380_p03.5h",   0xa000, 0x2000, CRC(190247af) SHA1(f2128fb5e6c16791493af1c77628b610b86d4677) )
	ROM_LOAD( "380_p02.6h",   0xc000, 0x2000, CRC(7e63725e) SHA1(f731f15956c6e7a0a4e8225513f8b9e6017c7a17) )
	ROM_LOAD( "380_p01.7h",   0xe000, 0x2000, CRC(eedaa5b2) SHA1(0c606ca4d092c3dc290c30b1a73f94e3b348e8fd) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "380_l14.5c",   0x0000, 0x2000, CRC(607df0fb) SHA1(67103d61994fd3a1e2de7cf9487e4f763234b18e) )
	ROM_LOAD( "380_l15.7c",   0x2000, 0x2000, CRC(a6ad30e1) SHA1(14f305717edcc2471e763b262960a0b96eef3530) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "380_j12.4a",   0x0000, 0x2000, CRC(56e5b408) SHA1(73b9e3d46dfe9e39b390c634df153648a0906876) )
	ROM_LOAD( "380_j13.5a",   0x2000, 0x2000, CRC(5aca0193) SHA1(4d0b0a773c385b7f1dcf024760d0437f47e78fbe) )

	ROM_REGION( 0x0c000, "gfx2", 0 )
	ROM_LOAD( "380_j06.11e",  0x0000, 0x2000, CRC(df0405c6) SHA1(70a50dcc86dfbdaa9c2af613105aae7f90747804) )
	ROM_LOAD( "380_j07.12e",  0x2000, 0x2000, CRC(23dfe3a6) SHA1(2ad7cbcbdbb434dc43e9c94cd00df9e57ac097f5) )
	ROM_LOAD( "380_j08.13e",  0x4000, 0x2000, CRC(3ba95390) SHA1(b22ad7cfda392894208eb4b39505f38bfe4c4342) )
	ROM_LOAD( "380_j09.14e",  0x6000, 0x2000, CRC(a9fba85a) SHA1(1a649ec667d377ffab26b4694be790b3a2742f30) )
	ROM_LOAD( "380_j10.15e",  0x8000, 0x2000, CRC(0532347e) SHA1(4c02b75a62993cce60d2cb87b81c7738abbc9a0d) )
	ROM_LOAD( "380_j11.16e",  0xa000, 0x2000, CRC(e1725d24) SHA1(d315588e6cc2f4263be621d2d8603c8215a90046) )

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "380_j18.2a",   0x0000, 0x020, CRC(10dd4eaa) SHA1(599acd25f36445221c553510a5de23ddba5ecc15) ) /* palette */
	ROM_LOAD( "380_j17.7b",   0x0020, 0x100, CRC(13989357) SHA1(0d61d468f6d3e1570fd18d236ec8cab92db4ed5c) ) /* character lookup table */
	ROM_LOAD( "380_j16.10c",  0x0120, 0x100, CRC(c244f2aa) SHA1(86df21c8e0b1ed51a0a4bd33dbb33f6efdea7d39) ) /* sprite lookup table */
ROM_END
#endif
#if 0
ROM_START( circusce ) /* Version P */
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "380_p05.3h",   0x6000, 0x2000, CRC(7ca74494) SHA1(326e081490e413b0638ec77de184b128fb2afd14) )
	ROM_LOAD( "380_p04.4h",   0x8000, 0x2000, CRC(dd0c0ee7) SHA1(e56e48f6f251430b7ce0e2cc59cfd00b5c760b9c) )
	ROM_LOAD( "380_p03.5h",   0xa000, 0x2000, CRC(190247af) SHA1(f2128fb5e6c16791493af1c77628b610b86d4677) )
	ROM_LOAD( "380_p02.6h",   0xc000, 0x2000, CRC(7e63725e) SHA1(f731f15956c6e7a0a4e8225513f8b9e6017c7a17) )
	ROM_LOAD( "380_p01.7h",   0xe000, 0x2000, CRC(eedaa5b2) SHA1(0c606ca4d092c3dc290c30b1a73f94e3b348e8fd) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "380_l14.5c",   0x0000, 0x2000, CRC(607df0fb) SHA1(67103d61994fd3a1e2de7cf9487e4f763234b18e) )
	ROM_LOAD( "380_l15.7c",   0x2000, 0x2000, CRC(a6ad30e1) SHA1(14f305717edcc2471e763b262960a0b96eef3530) )

	ROM_REGION( 0x04000, "gfx1", 0 )
	ROM_LOAD( "380_j12.4a",   0x0000, 0x2000, CRC(56e5b408) SHA1(73b9e3d46dfe9e39b390c634df153648a0906876) )
	ROM_LOAD( "380_j13.5a",   0x2000, 0x2000, CRC(5aca0193) SHA1(4d0b0a773c385b7f1dcf024760d0437f47e78fbe) )

	ROM_REGION( 0x0c000, "gfx2", 0 )
	ROM_LOAD( "380_j06.11e",  0x0000, 0x2000, CRC(df0405c6) SHA1(70a50dcc86dfbdaa9c2af613105aae7f90747804) )
	ROM_LOAD( "380_j07.12e",  0x2000, 0x2000, CRC(23dfe3a6) SHA1(2ad7cbcbdbb434dc43e9c94cd00df9e57ac097f5) )
	ROM_LOAD( "380_j08.13e",  0x4000, 0x2000, CRC(3ba95390) SHA1(b22ad7cfda392894208eb4b39505f38bfe4c4342) )
	ROM_LOAD( "380_j09.14e",  0x6000, 0x2000, CRC(a9fba85a) SHA1(1a649ec667d377ffab26b4694be790b3a2742f30) )
	ROM_LOAD( "380_j10.15e",  0x8000, 0x2000, CRC(0532347e) SHA1(4c02b75a62993cce60d2cb87b81c7738abbc9a0d) )
	ROM_LOAD( "380_j11.16e",  0xa000, 0x2000, CRC(e1725d24) SHA1(d315588e6cc2f4263be621d2d8603c8215a90046) )

	ROM_REGION( 0x0220, "proms", 0 )
	ROM_LOAD( "380_j18.2a",   0x0000, 0x020, CRC(10dd4eaa) SHA1(599acd25f36445221c553510a5de23ddba5ecc15) ) /* palette */
	ROM_LOAD( "380_j17.7b",   0x0020, 0x100, CRC(13989357) SHA1(0d61d468f6d3e1570fd18d236ec8cab92db4ed5c) ) /* character lookup table */
	ROM_LOAD( "380_j16.10c",  0x0120, 0x100, CRC(c244f2aa) SHA1(86df21c8e0b1ed51a0a4bd33dbb33f6efdea7d39) ) /* sprite lookup table */
ROM_END
#endif

static DRIVER_INIT( circusc )
{
	konami1_decode(machine, "maincpu");
}


GAME( 1984, circusc,  0,       circusc, circusc, circusc, ROT90, "Konami", "Circus Charlie (level select, set 1)", GAME_SUPPORTS_SAVE )
//GAME( 1984, circusc2, circusc, circusc, circusc, circusc, ROT90, "Konami", "Circus Charlie (level select, set 2)", GAME_SUPPORTS_SAVE )
//GAME( 1984, circusc3, circusc, circusc, circusc, circusc, ROT90, "Konami", "Circus Charlie (no level select)", GAME_SUPPORTS_SAVE )
//GAME( 1984, circuscc, circusc, circusc, circusc, circusc, ROT90, "Konami (Centuri license)", "Circus Charlie (Centuri)", GAME_SUPPORTS_SAVE )
//GAME( 1984, circusce, circusc, circusc, circusc, circusc, ROT90, "Konami (Centuri license)", "Circus Charlie (Centuri, earlier)", GAME_SUPPORTS_SAVE )
