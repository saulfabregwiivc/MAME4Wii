/***************************************************************************

Dottori Kun (Head On's mini game)
(c)1990 SEGA

Driver by Takahiro Nogi (nogi@kt.rim.or.jp) 1999/12/15 -


CPU   : Z-80 (4MHz)
SOUND : (none)

14479.MPR  ; PRG (FIRST VER)
14479A.MPR ; PRG (NEW VER)

* This game is only for the test of cabinet
* BackRaster = WHITE on the FIRST version.
* BackRaster = BLACK on the NEW version.
* On the NEW version, push COIN-SW as TEST MODE.
* 0000-3FFF:ROM 8000-85FF:VRAM(128x96) 8600-87FF:WORK-RAM

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"

/*---------------  External Function Prototypes  ---------------*/

VIDEO_UPDATE( dotrikun );
VIDEO_START( dotrikun );
WRITE8_HANDLER( dotrikun_color_w );

/*--------------------  Address / I/O Maps  --------------------*/

static ADDRESS_MAP_START( dotrikun_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM AM_BASE(&videoram) AM_SIZE(&videoram_size)
ADDRESS_MAP_END

static ADDRESS_MAP_START( io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READ_PORT("INPUTS") AM_WRITE(dotrikun_color_w)
ADDRESS_MAP_END

/*--------------------------  Inputs  --------------------------*/

static INPUT_PORTS_START( dotrikun )
	PORT_START("INPUTS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )
INPUT_PORTS_END

/*-------------------------  Machine  --------------------------*/

static MACHINE_DRIVER_START( dotrikun )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, 4000000)		 /* 4 MHz */
	MDRV_CPU_PROGRAM_MAP(dotrikun_map)
	MDRV_CPU_IO_MAP(io_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	/* video hardware */
	MDRV_VIDEO_UPDATE(dotrikun)
	MDRV_VIDEO_START(dotrikun)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 256-1, 0, 192-1)

	/* sound hardware */
MACHINE_DRIVER_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( dotrikun )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "14479a.mpr",	0x0000, 0x4000, CRC(b77a50db) SHA1(2a5d812d39f0f58f5c3e1b46f80aca75aa225115) )
ROM_END

#if 0
ROM_START( dotrikun2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "14479.mpr",	0x0000, 0x4000, CRC(a6aa7fa5) SHA1(4dbea33fb3541fdacf2195355751078a33bb30d5) )
ROM_END
#endif


GAME( 1990, dotrikun, 0,        dotrikun, dotrikun, 0, ROT0, "Sega", "Dottori Kun (new version)", GAME_SUPPORTS_SAVE | GAME_NO_SOUND )
//GAME( 1990, dotrikun2,dotrikun, dotrikun, dotrikun, 0, ROT0, "Sega", "Dottori Kun (old version)", GAME_SUPPORTS_SAVE | GAME_NO_SOUND )

