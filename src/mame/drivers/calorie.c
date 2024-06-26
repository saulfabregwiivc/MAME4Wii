/*

Calorie-Kun
Sega, 1986

PCB Layout
----------

Top PCB


837-6077  171-5381
|--------------------------------------------|
|                    Z80             YM2149  |
|                                            |
|                    2016            YM2149  |
|                                            |
|10079    10077      10075           YM2149  |
|    10078    10076                          |
|                                           -|
|                                          |
|                    2114                   -|
|                                           1|
|                    2114                   8|
|                                           W|
|                    2114                   A|
|                                           Y|
|                                           -|
|                                          |
|                                    DSW2(8)-|
|           10082   10081   10080            |
|2016                                DSW1(8) |
|--------------------------------------------|

Notes:
      Z80 clock   : 3.000MHz
      YM2149 clock: 1.500MHz
      VSync       : 60Hz
      2016        : 2K x8 SRAM
      2114        : 1K x4 SRAM



Bottom PCB


837-6076  171-5380
|--------------------------------------------|
|                            12MHz           |
|                                            |
|                                            |
|                                            |
|                                            |
|                                            |
|                                            |
| 10074                               10071  |
|               NEC                          |
| 10073      D317-0004   4MHz         10070  |
|                                            |
| 10072                               10069  |
|                                            |
| 2016         2114      6148                |
|                                            |
| 2016         2114      6148                |
|                                            |
|                        6148                |
|                        6148                |
|--------------------------------------------|
Notes:
      317-0004 clock: 4.000MHz
      2016          : 2K x8 SRAM
      2114          : 1K x4 SRAM
      6148          : 1K x4 SRAM


 driver by David Haywood and Pierpaolo Prazzoli

*/
#if 0

#include "driver.h"
#include "cpu/z80/z80.h"
#include "machine/segacrpt.h"
#include "sound/ay8910.h"

static UINT8 *calorie_fg;
static UINT8  calorie_bg;
static UINT8 *calorie_sprites;

static tilemap *bg_tilemap,*fg_tilemap;

static TILE_GET_INFO( get_bg_tile_info )
{
	UINT8 *src = memory_region(machine, "user1");
	int bg_base = (calorie_bg & 0x0f) * 0x200;
	int code  = src[bg_base + tile_index] | (((src[bg_base + tile_index + 0x100]) & 0x10) << 4);
	int color = src[bg_base + tile_index + 0x100] & 0x0f;
	int flag  = src[bg_base + tile_index + 0x100] & 0x40 ? TILE_FLIPX : 0;

	SET_TILE_INFO(1, code, color, flag);
}

static TILE_GET_INFO( get_fg_tile_info )
{
	int code  = ((calorie_fg[tile_index + 0x400] & 0x30) << 4) | calorie_fg[tile_index];
	int color = calorie_fg[tile_index + 0x400] & 0x0f;

	SET_TILE_INFO(0, code, color, TILE_FLIPYX((calorie_fg[tile_index + 0x400] & 0xc0) >> 6));
}


static VIDEO_START( calorie )
{
	bg_tilemap = tilemap_create(machine, get_bg_tile_info,tilemap_scan_rows,16,16,16,16);
	fg_tilemap = tilemap_create(machine, get_fg_tile_info,tilemap_scan_rows,8, 8,32,32);

	tilemap_set_transparent_pen(fg_tilemap,0);
}

static VIDEO_UPDATE( calorie )
{
	int x;

	if (calorie_bg & 0x10)
	{
		tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
		tilemap_draw(bitmap,cliprect,fg_tilemap,0,0);
	}
	else
	{
		tilemap_draw(bitmap,cliprect,fg_tilemap,TILEMAP_DRAW_OPAQUE,0);
	}


	for (x = 0x400; x >= 0; x -= 4)
	{
		int xpos, ypos, tileno, color, flipx, flipy;

		tileno = calorie_sprites[x+0];
		color = calorie_sprites[x+1] & 0x0f;
		flipx = calorie_sprites[x+1] & 0x40;
		flipy = 0;
		ypos = 0xff - calorie_sprites[x+2];
		xpos = calorie_sprites[x+3];

		if(flip_screen_get(screen->machine))
		{
			if( calorie_sprites[x+1] & 0x10 )
				ypos = 0xff - ypos + 32;
			else
				ypos = 0xff - ypos + 16;

			xpos = 0xff - xpos - 16;
			flipx = !flipx;
			flipy = !flipy;
		}

		if( calorie_sprites[x+1] & 0x10 )
		{
			 /* 32x32 sprites */
			drawgfx_transpen(bitmap,cliprect,screen->machine->gfx[3],tileno | 0x40,color,flipx,flipy,xpos,ypos - 31,0);
		}
		else
		{
			/* 16x16 sprites */
			drawgfx_transpen(bitmap,cliprect,screen->machine->gfx[2],tileno,color,flipx,flipy,xpos,ypos - 15,0);
		}
	}
	return 0;
}

static WRITE8_HANDLER( calorie_fg_w )
{
	calorie_fg[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap,offset & 0x3ff);
}

static WRITE8_HANDLER( calorie_bg_w )
{
	if((calorie_bg & ~0x10) != (data & ~0x10))
	{
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}

	calorie_bg = data;
}

static WRITE8_HANDLER( calorie_flipscreen_w )
{
	flip_screen_set(space->machine, data & 1);
}

static READ8_HANDLER( calorie_soundlatch_r )
{
	UINT8 latch = soundlatch_r(space,0);
	soundlatch_clear_w(space,0,0);
	return latch;
}

static WRITE8_HANDLER( bogus_w )
{
	popmessage("written to 3rd sound chip: data = %02X port = %02X", data, offset);
}

static ADDRESS_MAP_START( calorie_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xcfff) AM_RAM
	AM_RANGE(0xd000, 0xd7ff) AM_RAM_WRITE(calorie_fg_w) AM_BASE(&calorie_fg)
	AM_RANGE(0xd800, 0xdbff) AM_RAM AM_BASE(&calorie_sprites)
	AM_RANGE(0xdc00, 0xdcff) AM_RAM_WRITE(paletteram_xxxxBBBBGGGGRRRR_le_w) AM_BASE(&paletteram)
	AM_RANGE(0xde00, 0xde00) AM_WRITE(calorie_bg_w)
	AM_RANGE(0xf000, 0xf000) AM_READ_PORT("P1")
	AM_RANGE(0xf001, 0xf001) AM_READ_PORT("P2")
	AM_RANGE(0xf002, 0xf002) AM_READ_PORT("SYSTEM")
	AM_RANGE(0xf004, 0xf004) AM_READ_PORT("DSW1") AM_WRITE(calorie_flipscreen_w)
	AM_RANGE(0xf005, 0xf005) AM_READ_PORT("DSW2")
	AM_RANGE(0xf800, 0xf800) AM_WRITE(soundlatch_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( calorie_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0xc000, 0xc000) AM_READ(calorie_soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( calorie_sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x01) AM_DEVWRITE("ay1", ay8910_address_data_w)
	AM_RANGE(0x01, 0x01) AM_DEVREAD("ay1", ay8910_r)
	AM_RANGE(0x10, 0x11) AM_DEVWRITE("ay2", ay8910_address_data_w)
	AM_RANGE(0x11, 0x11) AM_DEVREAD("ay2", ay8910_r)
	// 3rd ?
	AM_RANGE(0x00, 0xff) AM_WRITE(bogus_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( calorie )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_COCKTAIL PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_COCKTAIL PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:3,4")
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0x80, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW1:7,8")
	PORT_DIPSETTING(    0xc0, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPSETTING(    0x80, "5" )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPSETTING(    0x01, "20,000 Only" )
	PORT_DIPSETTING(    0x03, "20,000 and 60,000" ) /* No listed value for 0x02 (Off / On) */
	PORT_DIPNAME( 0x04, 0x00, "Number of Bombs" ) PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPNAME( 0x08, 0x00, "Difficulty - Mogura Nian" ) PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x30, 0x00, "Difficulty - Select of Mogura" ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Hardest ) )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW2:7" )
	PORT_DIPNAME( 0x80, 0x00, "Infinite Lives" ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3),RGN_FRAC(1,3),RGN_FRAC(2,3) },
	{ 0, 1,2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout tiles16x16_layout =
{
	16,16,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3),RGN_FRAC(1,3),RGN_FRAC(2,3) },
	{ 0, 1,2, 3, 4, 5, 6, 7, 8*8+0,8*8+1,8*8+2,8*8+3,8*8+4,8*8+5,8*8+6,8*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 },
	8*32
};

static const gfx_layout tiles32x32_layout =
{
	32,32,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3),RGN_FRAC(1,3),RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7,
			32*8+0, 32*8+1, 32*8+2, 32*8+3, 32*8+4, 32*8+5, 32*8+6, 32*8+7,
			40*8+0, 40*8+1, 40*8+2, 40*8+3, 40*8+4, 40*8+5, 40*8+6, 40*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8,
			64*8, 65*8, 66*8, 67*8, 68*8, 69*8, 70*8, 71*8,
			80*8, 81*8, 82*8, 83*8, 84*8, 85*8, 86*8, 87*8 },
	128*8
};

static GFXDECODE_START( calorie )
	GFXDECODE_ENTRY( "gfx2", 0, tiles8x8_layout,   0, 16 )
	GFXDECODE_ENTRY( "gfx3", 0, tiles16x16_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx1", 0, tiles16x16_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx1", 0, tiles32x32_layout, 0, 16 )
GFXDECODE_END

static MACHINE_DRIVER_START( calorie )
	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80,4000000)		 /* 4 MHz */
	MDRV_CPU_PROGRAM_MAP(calorie_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_CPU_ADD("audiocpu", Z80,3000000)		 /* 3 MHz */
	MDRV_CPU_PROGRAM_MAP(calorie_sound_map)
	MDRV_CPU_IO_MAP(calorie_sound_io_map)
	MDRV_CPU_PERIODIC_INT(irq0_line_hold, 64)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 256-1, 16, 256-16-1)

	MDRV_GFXDECODE(calorie)
	MDRV_PALETTE_LENGTH(0x100)

	MDRV_VIDEO_START(calorie)
	MDRV_VIDEO_UPDATE(calorie)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.8)  /* YM2149 really */

	MDRV_SOUND_ADD("ay2", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.8)  /* YM2149 really */

	MDRV_SOUND_ADD("ay3", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.8)  /* YM2149 really */
MACHINE_DRIVER_END


ROM_START( calorie )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "epr10072.1j", 0x00000, 0x4000, CRC(ade792c1) SHA1(6ea5afb00a87037d502c17adda7e4060d12680d7) )
	ROM_LOAD( "epr10073.1k", 0x04000, 0x4000, CRC(b53e109f) SHA1(a41c5affe917232e7adf40d7c15cff778b197e90) )
	ROM_LOAD( "epr10074.1m", 0x08000, 0x4000, CRC(a08da685) SHA1(69f9cfebc771312dbb1726350c2d9e9e8c46353f) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "epr10075.4d", 0x0000, 0x4000, CRC(ca547036) SHA1(584a65482f2b92a4c08c37560450d6db68a56c7b) )

	ROM_REGION( 0x2000, "user1", 0 ) /* background tilemaps */
	ROM_LOAD( "epr10079.8d", 0x0000, 0x2000, CRC(3c61a42c) SHA1(68ea6b5d2f3c6a9e5308c08dde20424f20021a73) )

	ROM_REGION( 0xc000, "gfx1", 0 ) /* sprites */
	ROM_LOAD( "epr10071.7m", 0x0000, 0x4000, CRC(5f55527a) SHA1(ec1ba8f95ac47a0c783e117ef4af6fe0ab5925b5) )
	ROM_LOAD( "epr10070.7k", 0x4000, 0x4000, CRC(97f35a23) SHA1(869553a334e1b3ba900a8b9c9eaf25fbc6ab31dd) )
	ROM_LOAD( "epr10069.7j", 0x8000, 0x4000, CRC(c0c3deaf) SHA1(8bf2e2146b794a330a079dd080f0586500964b1a) )

	ROM_REGION( 0x6000, "gfx2", 0 ) /* tiles  */
	ROM_LOAD( "epr10082.5r", 0x0000, 0x2000, CRC(5984ea44) SHA1(010011b5b8dfa593c6fc7d2366f8cf82fcc8c978) )
	ROM_LOAD( "epr10081.4r", 0x2000, 0x2000, CRC(e2d45dd8) SHA1(5e11089680b574ea4cbf64510e51b0a945f79174) )
	ROM_LOAD( "epr10080.3r", 0x4000, 0x2000, CRC(42edfcfe) SHA1(feba7b1daffcad24d4c24f55ab5466f8cebf31ad) )

	ROM_REGION( 0xc000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "epr10078.7d", 0x0000, 0x4000, CRC(5b8eecce) SHA1(e7eee82081939b361edcbb9587b072b4b9a162f9) )
	ROM_LOAD( "epr10077.6d", 0x4000, 0x4000, CRC(01bcb609) SHA1(5d01fa75f214d34483284aaaef985ab92a606505) )
	ROM_LOAD( "epr10076.5d", 0x8000, 0x4000, CRC(b1529782) SHA1(8e0e92aae4c8dd8720414372aa767054cc316a0f) )
ROM_END

ROM_START( calorieb )
	ROM_REGION( 0x1c000, "maincpu", 0 )
	ROM_LOAD( "12.bin",      0x10000, 0x4000, CRC(cf5fa69e) SHA1(520d5652e93a672a1fc147295fbd63b873967885) )
	ROM_CONTINUE(            0x00000, 0x4000 )
	ROM_LOAD( "13.bin",      0x14000, 0x4000, CRC(52e7263f) SHA1(4d684c9e3f08ddb18b0b3b982aba82d3c809a633) )
	ROM_CONTINUE(            0x04000, 0x4000 )
	ROM_LOAD( "epr10074.1m", 0x08000, 0x4000, CRC(a08da685) SHA1(69f9cfebc771312dbb1726350c2d9e9e8c46353f) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "epr10075.4d", 0x0000, 0x4000, CRC(ca547036) SHA1(584a65482f2b92a4c08c37560450d6db68a56c7b) )

	ROM_REGION( 0x2000, "user1", 0 ) /* background tilemaps */
	ROM_LOAD( "epr10079.8d", 0x0000, 0x2000, CRC(3c61a42c) SHA1(68ea6b5d2f3c6a9e5308c08dde20424f20021a73) )

	ROM_REGION( 0xc000, "gfx1", 0 ) /* sprites */
	ROM_LOAD( "epr10071.7m", 0x0000, 0x4000, CRC(5f55527a) SHA1(ec1ba8f95ac47a0c783e117ef4af6fe0ab5925b5) )
	ROM_LOAD( "epr10070.7k", 0x4000, 0x4000, CRC(97f35a23) SHA1(869553a334e1b3ba900a8b9c9eaf25fbc6ab31dd) )
	ROM_LOAD( "epr10069.7j", 0x8000, 0x4000, CRC(c0c3deaf) SHA1(8bf2e2146b794a330a079dd080f0586500964b1a) )

	ROM_REGION( 0x6000, "gfx2", 0 ) /* tiles  */
	ROM_LOAD( "epr10082.5r", 0x0000, 0x2000, CRC(5984ea44) SHA1(010011b5b8dfa593c6fc7d2366f8cf82fcc8c978) )
	ROM_LOAD( "epr10081.4r", 0x2000, 0x2000, CRC(e2d45dd8) SHA1(5e11089680b574ea4cbf64510e51b0a945f79174) )
	ROM_LOAD( "epr10080.3r", 0x4000, 0x2000, CRC(42edfcfe) SHA1(feba7b1daffcad24d4c24f55ab5466f8cebf31ad) )

	ROM_REGION( 0xc000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "epr10078.7d", 0x0000, 0x4000, CRC(5b8eecce) SHA1(e7eee82081939b361edcbb9587b072b4b9a162f9) )
	ROM_LOAD( "epr10077.6d", 0x4000, 0x4000, CRC(01bcb609) SHA1(5d01fa75f214d34483284aaaef985ab92a606505) )
	ROM_LOAD( "epr10076.5d", 0x8000, 0x4000, CRC(b1529782) SHA1(8e0e92aae4c8dd8720414372aa767054cc316a0f) )
ROM_END


static DRIVER_INIT( calorie )
{
	calorie_decode(machine, "maincpu");
}

static DRIVER_INIT( calorieb )
{
	const address_space *space = cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM);
	memory_set_decrypted_region(space, 0x0000, 0x7fff, memory_region(machine, "maincpu") + 0x10000);
}
#endif


/* Note: the bootleg is identical to the original once decrypted */
//GAME( 1986, calorie,  0,       calorie, calorie, calorie,  ROT0, "Sega",    "Calorie Kun vs Moguranian", 0 )
//GAME( 1986, calorieb, calorie, calorie, calorie, calorieb, ROT0, "bootleg", "Calorie Kun vs Moguranian (bootleg)", 0 )
