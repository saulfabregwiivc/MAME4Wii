/*******************************************************************************

    Pro Soccer                      (c) 1983 Data East Corporation
    Pro Sport                       (c) 1983 Data East Corporation
    Boomer Rang'R / Genesis         (c) 1983 Data East Corporation
    Kamikaze Cabbie / Yellow Cab    (c) 1984 Data East Corporation
    Liberation                      (c) 1984 Data East Corporation

    Liberation was available on two pcbs - a dedicated twin pcb set and
    a version on the Genesis/Yellow Cab pcb that had an extra cpu pcb attached
    for the different protection.  The program is the same on both versions.

    Emulation by Bryan McPhail, mish@tendril.co.uk

*******************************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "sound/ay8910.h"

PALETTE_INIT( liberate );
VIDEO_UPDATE( prosoccr );
VIDEO_UPDATE( prosport );
VIDEO_UPDATE( liberate );
VIDEO_UPDATE( boomrang );
VIDEO_START( prosoccr );
VIDEO_START( prosport );
VIDEO_START( boomrang );
VIDEO_START( liberate );

static int deco16_bank;
static UINT8 *scratchram;
UINT8 *prosoccr_charram;
UINT8 *prosport_bg_vram;

WRITE8_HANDLER( deco16_io_w );
WRITE8_HANDLER( prosoccr_io_w );
WRITE8_HANDLER( prosport_io_w );
WRITE8_HANDLER( prosport_paletteram_w );
WRITE8_HANDLER( prosport_bg_vram_w );
WRITE8_HANDLER( liberate_videoram_w );
WRITE8_HANDLER( liberate_colorram_w );

/*************************************
 *
 *  Deco Initialization
 *
 *************************************/

static READ8_HANDLER( deco16_bank_r )
{
	const UINT8 *ROM = memory_region(space->machine, "user1");

	/* The tilemap bank can be swapped into main memory */
	if (deco16_bank)
		return ROM[offset];

	/* Else the handler falls through to read the usual address */
	if (offset<0x400) return colorram[offset];
	if (offset<0x800) return videoram[offset-0x400];
	if (offset<0x1000) return spriteram[offset-0x800];
	if (offset<0x2200) { logerror("%04x: Unmapped bank read %04x\n",cpu_get_pc(space->cpu),offset); return 0; }
	if (offset<0x2800) return scratchram[offset-0x2200];

	logerror("%04x: Unmapped bank read %04x\n",cpu_get_pc(space->cpu),offset);
	return 0;
}

static READ8_HANDLER( deco16_io_r )
{
	if (offset==0) return input_port_read(space->machine, "IN1"); /* Player 1 controls */
	if (offset==1) return input_port_read(space->machine, "IN2"); /* Player 2 controls */
	if (offset==2) return input_port_read(space->machine, "IN3"); /* Vblank, coins */
	if (offset==3) return input_port_read(space->machine, "DSW1"); /* Dip 1 */
	if (offset==4) return input_port_read(space->machine, "DSW2"); /* Dip 2 */

	logerror("%04x:  Read input %d\n",cpu_get_pc(space->cpu),offset);
	return 0xff;
}

static WRITE8_HANDLER( deco16_bank_w )
{
	deco16_bank = data;

	if (deco16_bank)
		memory_install_read8_handler(cputag_get_address_space(space->machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x8000, 0x800f, 0, 0, deco16_io_r);
	else
		memory_install_read8_handler(cputag_get_address_space(space->machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x8000, 0x800f, 0, 0, (read8_space_func)SMH_BANK(1));
}

static UINT8 gfx_rom_readback;

static READ8_HANDLER( prosoccr_bank_r )
{
	const UINT8 *ROM = memory_region(space->machine, "user1");

	/* The tilemap bank can be swapped into main memory */
	if (deco16_bank)
		return ROM[offset];

	/* Else the handler falls through to read the usual address */
	if (offset<0x400) return colorram[offset];
	if (offset<0x800) return videoram[offset-0x400];
	if (offset<0xc00) return colorram[offset-0x800];
	if (offset<0x1000) return spriteram[offset-0xc00];
	if (offset<0x2200) { logerror("%04x: Unmapped bank read %04x\n",cpu_get_pc(space->cpu),offset); return 0; }
	if (offset<0x2800) return scratchram[offset-0x2200];

	logerror("%04x: Unmapped bank read %04x\n",cpu_get_pc(space->cpu),offset);
	return 0;
}

static READ8_HANDLER( prosoccr_charram_r )
{
	UINT8 *SRC_GFX = memory_region(space->machine, "shared_gfx");

	if(gfx_rom_readback)
	{
		switch(offset & 0x1800)
		{
			case 0x0000:
				return SRC_GFX[(offset & 0x7ff)+(0x0000)+0x0000];
			case 0x0800:
				return SRC_GFX[(offset & 0x7ff)+(0x0000)+0x2000];
			case 0x1000:
				return SRC_GFX[(offset & 0x7ff)+(0x0000)+0x4000];
		}
	}

	/* note: gfx_rom_readback == 1 never happens. */
	return prosoccr_charram[offset+gfx_rom_readback*0x1800];
}

static WRITE8_HANDLER( prosoccr_charram_w )
{
	UINT8 *FG_GFX = memory_region(space->machine, "fg_gfx");

	if(deco16_bank)
	{
		prosoccr_io_w(space,offset & 0xf,data);
	}
	else
	{
		/* note: gfx_rom_readback == 1 never happens. */
		prosoccr_charram[offset+gfx_rom_readback*0x1800] = data;

		switch(offset & 0x1800)
		{
			case 0x0000:
				FG_GFX[(offset & 0x7ff)+(0x0000)+0x0000] = data;
				//FG_GFX[(offset & 0x7ff)+(0x1800)+0x0000] = data;
				break;
			case 0x0800:
				FG_GFX[(offset & 0x7ff)+(0x0000)+0x2000] = data;
				//FG_GFX[(offset & 0x7ff)+(0x1800)+0x2000] = data;
				break;
			case 0x1000:
				FG_GFX[(offset & 0x7ff)+(0x0000)+0x4000] = data;
				//FG_GFX[(offset & 0x7ff)+(0x1800)+0x4000] = data;
				break;
		}
	}

	offset&=0x7ff;

	/* dirty char */
    gfx_element_mark_dirty(space->machine->gfx[0], offset >> 3);
//  gfx_element_mark_dirty(space->machine->gfx[0], (offset|0x1800) >> 3);
}

static WRITE8_HANDLER( prosoccr_char_bank_w )
{
	gfx_rom_readback = data & 1; //enable GFX rom read-back
	if(data & 0xfe)
		printf("%02x\n",data);
}

static WRITE8_HANDLER( prosoccr_io_bank_w )
{
	deco16_bank = data & 1;

	if (deco16_bank)
		memory_install_read8_handler(cputag_get_address_space(space->machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x8000, 0x800f, 0, 0, deco16_io_r);
	else
		memory_install_read8_handler(cputag_get_address_space(space->machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x8000, 0x800f, 0, 0, prosoccr_charram_r);

}

#if 0
static READ8_HANDLER( prosport_charram_r )
{
	UINT8 *FG_GFX = memory_region(space->machine, "progolf_fg_gfx");

	switch(offset & 0x1800)
	{
		case 0x0000:
			return FG_GFX[(offset & 0x7ff)+(0x0800)+0x0000];
			//FG_GFX[(offset & 0x7ff)+(0x1800)+0x0000] = data;
		case 0x0800:
			return FG_GFX[(offset & 0x7ff)+(0x0800)+0x2000];
			//FG_GFX[(offset & 0x7ff)+(0x1800)+0x2000] = data;
		case 0x1000:
			return FG_GFX[(offset & 0x7ff)+(0x0800)+0x4000];
			//FG_GFX[(offset & 0x7ff)+(0x1800)+0x4000] = data;
	}

	return 0;
}
#endif
#if 0
static WRITE8_HANDLER( prosport_charram_w )
{
	UINT8 *FG_GFX = memory_region(space->machine, "progolf_fg_gfx");

	switch(offset & 0x1800)
	{
		case 0x0000:
			FG_GFX[(offset & 0x7ff)+(0x0800)+0x0000] = data;
			//FG_GFX[(offset & 0x7ff)+(0x1800)+0x0000] = data;
			break;
		case 0x0800:
			FG_GFX[(offset & 0x7ff)+(0x0800)+0x2000] = data;
			//FG_GFX[(offset & 0x7ff)+(0x1800)+0x2000] = data;
			break;
		case 0x1000:
			FG_GFX[(offset & 0x7ff)+(0x0800)+0x4000] = data;
			//FG_GFX[(offset & 0x7ff)+(0x1800)+0x4000] = data;
			break;
	}

	offset&=0x7ff;

	/* dirty char */
    gfx_element_mark_dirty(space->machine->gfx[3], (offset+0x800) >> 3);
    gfx_element_mark_dirty(space->machine->gfx[3+4], (offset+0x800) >> 5);
}
#endif

/*************************************
 *
 *  Memory handlers
 *
 *************************************/
#if 0
static ADDRESS_MAP_START( prosport_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0200, 0x021f) AM_RAM_WRITE(prosport_paletteram_w) AM_BASE(&paletteram)
	AM_RANGE(0x0000, 0x03ff) AM_MIRROR(0x2000) AM_RAM
	AM_RANGE(0x0400, 0x07ff) AM_RAM_WRITE(prosport_bg_vram_w) AM_BASE(&prosport_bg_vram)
	AM_RANGE(0x0800, 0x1fff) AM_READWRITE(prosport_charram_r,prosport_charram_w) //0x1e00-0x1fff isn't charram!
 	AM_RANGE(0x2400, 0x2fff) AM_RAM
	AM_RANGE(0x3000, 0x33ff) AM_RAM_WRITE(liberate_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x3400, 0x37ff) AM_RAM_WRITE(liberate_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x3800, 0x3fff) AM_RAM AM_BASE(&spriteram)
	AM_RANGE(0x8000, 0x800f) AM_WRITE(prosport_io_w)
	AM_RANGE(0x8000, 0x800f) AM_ROMBANK(1)
	AM_RANGE(0x4000, 0xffff) AM_ROM
ADDRESS_MAP_END
#endif

static ADDRESS_MAP_START( liberate_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_RAM
	AM_RANGE(0x1000, 0x3fff) AM_ROM /* Mirror of main rom */
	AM_RANGE(0x4000, 0x7fff) AM_READ(deco16_bank_r)
	AM_RANGE(0x4000, 0x43ff) AM_WRITE(liberate_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x4400, 0x47ff) AM_WRITE(liberate_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x4800, 0x4fff) AM_WRITEONLY AM_BASE(&spriteram)
	AM_RANGE(0x6200, 0x67ff) AM_RAM AM_BASE(&scratchram)
	AM_RANGE(0x8000, 0x800f) AM_WRITE(deco16_io_w)
	AM_RANGE(0x8000, 0x800f) AM_ROMBANK(1)
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( prosoccr_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_RAM
	AM_RANGE(0x1000, 0x3fff) AM_ROM /* Mirror of main rom */
	AM_RANGE(0x4000, 0x7fff) AM_READ(prosoccr_bank_r)
	AM_RANGE(0x4000, 0x43ff) AM_MIRROR(0x800) AM_WRITE(liberate_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x4400, 0x47ff) AM_WRITE(liberate_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x4c00, 0x4fff) AM_WRITEONLY AM_BASE(&spriteram)
	AM_RANGE(0x6200, 0x67ff) AM_RAM AM_BASE(&scratchram)
	AM_RANGE(0x8000, 0x97ff) AM_READWRITE(prosoccr_charram_r,prosoccr_charram_w)
	AM_RANGE(0x9800, 0x9800) AM_WRITE(prosoccr_char_bank_w)
	AM_RANGE(0xa000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( deco16_io_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x00, 0x00) AM_READ_PORT("IN0") AM_WRITE(deco16_bank_w)
	AM_RANGE(0x01, 0x01) AM_READ_PORT("TILT")
ADDRESS_MAP_END

static ADDRESS_MAP_START( prosoccr_io_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x00, 0x00) AM_READ_PORT("IN0") AM_WRITE(prosoccr_io_bank_w)
	//AM_RANGE(0x01, 0x01) AM_READ_PORT("TILT")
ADDRESS_MAP_END

#if 0
static ADDRESS_MAP_START( liberatb_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00fe, 0x00fe) AM_READ_PORT("IN0")
	AM_RANGE(0x0000, 0x0fff) AM_RAM
	AM_RANGE(0x1000, 0x3fff) AM_ROM /* Mirror of main rom */
	AM_RANGE(0x4000, 0x7fff) AM_READ(deco16_bank_r)
	AM_RANGE(0x4000, 0x43ff) AM_WRITE(liberate_colorram_w) AM_BASE(&colorram)
	AM_RANGE(0x4400, 0x47ff) AM_WRITE(liberate_videoram_w) AM_BASE(&videoram)
	AM_RANGE(0x4800, 0x4fff) AM_WRITEONLY AM_BASE(&spriteram)
	AM_RANGE(0x6200, 0x67ff) AM_WRITEONLY AM_BASE(&scratchram)
	AM_RANGE(0xf000, 0xf00f) AM_WRITE(deco16_io_w)
	AM_RANGE(0xf000, 0xf000) AM_READ_PORT("IN1")
	AM_RANGE(0xf001, 0xf001) AM_READ_PORT("IN2")
	AM_RANGE(0xf002, 0xf002) AM_READ_PORT("IN3")
	AM_RANGE(0xf003, 0xf003) AM_READ_PORT("DSW1")
	AM_RANGE(0xf004, 0xf004) AM_READ_PORT("DSW2")
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END
#endif
/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( prosoccr_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x01ff) AM_RAM
	AM_RANGE(0x2000, 0x2000) AM_DEVWRITE("ay1", ay8910_data_w)
	AM_RANGE(0x4000, 0x4000) AM_DEVWRITE("ay1", ay8910_address_w)
	AM_RANGE(0x6000, 0x6000) AM_DEVWRITE("ay2", ay8910_data_w)
	AM_RANGE(0x8000, 0x8000) AM_DEVWRITE("ay2", ay8910_address_w)
	AM_RANGE(0xa000, 0xa000) AM_READ(soundlatch_r)
	AM_RANGE(0xc000, 0xc000) AM_WRITENOP //irq ack
	AM_RANGE(0xe000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( liberate_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x01ff) AM_RAM
	AM_RANGE(0x1000, 0x1000) AM_WRITENOP
	AM_RANGE(0x3000, 0x3000) AM_DEVWRITE("ay1", ay8910_data_w)
	AM_RANGE(0x4000, 0x4000) AM_DEVWRITE("ay1", ay8910_address_w)
	AM_RANGE(0x7000, 0x7000) AM_DEVWRITE("ay2", ay8910_data_w)
	AM_RANGE(0x8000, 0x8000) AM_DEVWRITE("ay2", ay8910_address_w)
	AM_RANGE(0xb000, 0xb000) AM_READ(soundlatch_r)
	AM_RANGE(0xc000, 0xffff) AM_ROM
ADDRESS_MAP_END

/*************************************
 *
 *  Generic port definitions
 *
 *************************************/

static INPUT_PORTS_START( generic_input )
	PORT_START("IN0")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_VBLANK )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK )

	PORT_START("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x00, "Manufacturer" )
	PORT_DIPSETTING(    0x00, "Data East USA" )
	PORT_DIPSETTING(    0x80, "Data East Corporation" )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("TILT")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0xfd, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END

/*************************************
 *
 *  Game-specific port definitions
 *
 *************************************/

static INPUT_PORTS_START( boomrang )
	PORT_INCLUDE( generic_input )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Infinite ) )
	PORT_DIPNAME( 0x0c, 0x0c, "Bonus" )
	PORT_DIPSETTING(    0x00, "20K Only" )
	PORT_DIPSETTING(    0x0c, "20K & Every 30K" )
	PORT_DIPSETTING(    0x08, "30K & Every 50K" )
	PORT_DIPSETTING(    0x04, "50K & Every 70K" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Invincibility" ) /* Manual calls it "Hit Check Of Boomer" */
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( kamikcab )
	PORT_INCLUDE( generic_input )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Infinite ) )
	PORT_DIPNAME( 0x0c, 0x0c, "Bonus" )
	PORT_DIPSETTING(    0x00, "20000" )
	PORT_DIPSETTING(    0x0c, "20000 Every 30000" )
	PORT_DIPSETTING(    0x08, "30000 Every 40000" )
	PORT_DIPSETTING(    0x04, "40000 Every 50000" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x80, "Invincibility (Cheat)" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_MODIFY("TILT")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( yellowcb )
	PORT_INCLUDE( kamikcab )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0xfb, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( liberate )
	PORT_INCLUDE( generic_input )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x80, 0x80, "Manufacturer" )
	PORT_DIPSETTING(    0x00, "(INVALID) Data East USA (Dual Assault)" )
	PORT_DIPSETTING(    0x80, "Data East Corporation (Liberation)" )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Infinite ) )
	PORT_DIPNAME( 0x0c, 0x0c, "Bonus" )
	PORT_DIPSETTING(    0x00, "20000" )
	PORT_DIPSETTING(    0x0c, "20000 30000" )
	PORT_DIPSETTING(    0x08, "30000 50000" )
	PORT_DIPSETTING(    0x04, "50000 70000" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) ) /* Difficulty? */
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) ) /* Difficulty? */
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( prosport )
	PORT_INCLUDE( generic_input )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Service" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Test Mode" )
	PORT_DIPSETTING(    0x00, "ROM Test" )
	PORT_DIPSETTING(    0x20, "Maping Test" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( dualaslt )
	PORT_INCLUDE( liberate )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x80, 0x00, "Manufacturer / Title" )
	PORT_DIPSETTING(    0x00, "Data East USA (Dual Assault)" )
	PORT_DIPSETTING(    0x80, "(INVALID) Data East Corporation (Liberation)" )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( liberatb )
	PORT_INCLUDE( liberate )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0xf9, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_MODIFY("TILT")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( prosoccr )
	PORT_INCLUDE( generic_input )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_B ) ) //maybe reversed coins actually?
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
 	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout sprites =
{
	16,16,
	RGN_FRAC(1,3),
	3,
 	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 16*8, 1+(16*8), 2+(16*8), 3+(16*8), 4+(16*8), 5+(16*8), 6+(16*8), 7+(16*8),
		0,1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 ,8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	16*16
};

static const gfx_layout tiles1 =
{
	16,16,
	128,
	3,
	{ 4, 0, 0x4000*8+4 },
	{
		24,25,26,27, 16,17,18,19, 8,9,10,11, 0,1,2,3
	},
	{
		0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32
	},
	64*8
};

static const gfx_layout tiles2 =
{
	16,16,
	128,
	3,
	{ 0x2000*8+4, 0x2000*8+0, 0x4000*8 },
	{
		24,25,26,27, 16,17,18,19, 8,9,10,11, 0,1,2,3
	},
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	64*8
};

static const gfx_layout prosoccr_bg_gfx =
{
	16,16,
	RGN_FRAC(1,2),
	3,
	{ RGN_FRAC(0,2)+4, RGN_FRAC(0,2)+0, RGN_FRAC(1,2)+4 },
	{
		24,25,26,27, 16,17,18,19, 8,9,10,11, 0,1,2,3
	},
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	64*8
};

#if 0
static const gfx_layout pro_tiles =
{
	16,16,
	RGN_FRAC(1,2),
	3,
 	{ 4,0, RGN_FRAC(1,2)+4 },
	{ 384+0, 384+1, 384+2, 384+3,
	  256+0, 256+1, 256+2, 256+3,
	  128+0, 128+1, 128+2, 128+3,
	  0,1,2,3 },
	{ 0*8, 1*8, 2*8, 3*8,4*8,5*8,6*8,7*8,8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	512
};
#endif

static GFXDECODE_START( liberate )
	GFXDECODE_ENTRY( "gfx1", 0x00000, charlayout,  0, 4 )
	GFXDECODE_ENTRY( "gfx1", 0x00000, sprites,     0, 4 )
	GFXDECODE_ENTRY( "gfx2", 0x00000, tiles1,      0, 4 )
	GFXDECODE_ENTRY( "gfx2", 0x00000, tiles2,      0, 4 )
GFXDECODE_END

#if 0
static GFXDECODE_START( prosport )
	GFXDECODE_ENTRY( "prosport_fg_gfx",  0x00000, charlayout,  0, 4 )
	GFXDECODE_ENTRY( "protenns_fg_gfx",  0x00000, charlayout,  0, 4 )
	GFXDECODE_ENTRY( "probowl_fg_gfx",   0x00000, charlayout,  0, 4 )
	GFXDECODE_ENTRY( "progolf_fg_gfx",   0x00000, charlayout,  0, 4 )
	GFXDECODE_ENTRY( "prosport_fg_gfx",  0x00000, sprites,  0, 4 )
	GFXDECODE_ENTRY( "protenns_fg_gfx",  0x00000, sprites,  0, 4 )
	GFXDECODE_ENTRY( "probowl_fg_gfx",   0x00000, sprites,  0, 4 )
	GFXDECODE_ENTRY( "progolf_fg_gfx",   0x00000, sprites,  0, 4 )
	GFXDECODE_ENTRY( "gfx2", 0x00000, pro_tiles,   0, 4 ) //backgrounds
GFXDECODE_END
#endif

static GFXDECODE_START( prosoccr )
	GFXDECODE_ENTRY( "fg_gfx", 0x00000, charlayout,        0, 4 )
	GFXDECODE_ENTRY( "sp_gfx", 0x00000, sprites,           0, 4 )
	GFXDECODE_ENTRY( "bg_gfx", 0x00000, prosoccr_bg_gfx,   8, 2 )
GFXDECODE_END

/*************************************
 *
 *  Deco16 interrupts
 *
 *************************************/

static INTERRUPT_GEN( deco16_interrupt )
{
	static int latch = 0;
	int p = ~input_port_read(device->machine, "IN3");
	if ((p & 0x43) && !latch)
	{
		cpu_set_input_line(device, DECO16_IRQ_LINE, ASSERT_LINE);
		latch = 1;
	}
	else
	{
		if (!(p & 0x43))
			latch=0;
	}
}

#if 0
static INTERRUPT_GEN( prosport_interrupt )
{
	/* ??? */
	cpu_set_input_line(device, DECO16_IRQ_LINE, ASSERT_LINE);
}
#endif

/*************************************
 *
 *  Machine driver(s)
 *
 *************************************/

static MACHINE_DRIVER_START( liberate )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu",DECO16, 2000000)
	MDRV_CPU_PROGRAM_MAP(liberate_map)
	MDRV_CPU_IO_MAP(deco16_io_map)
	MDRV_CPU_VBLANK_INT("screen", deco16_interrupt)

	MDRV_CPU_ADD("audiocpu",M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(liberate_sound_map)
	MDRV_CPU_PERIODIC_INT(nmi_line_pulse,16*60) /* ??? */

	MDRV_QUANTUM_TIME(HZ(12000))

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 529ms Vblank duration?? */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(liberate)
	MDRV_PALETTE_LENGTH(33)
	MDRV_PALETTE_INIT(liberate)

	MDRV_VIDEO_START(liberate)
	MDRV_VIDEO_UPDATE(liberate)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD("ay2", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END

#if 0
static MACHINE_DRIVER_START( liberatb )
	MDRV_IMPORT_FROM(liberate)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_TYPE(M6502)
	MDRV_CPU_PROGRAM_MAP(liberatb_map)
	MDRV_CPU_IO_MAP(0)
	MDRV_CPU_VBLANK_INT("screen", deco16_interrupt)
MACHINE_DRIVER_END
#endif

static MACHINE_DRIVER_START( boomrang )
	MDRV_IMPORT_FROM(liberate)

	MDRV_VIDEO_START(boomrang)
	MDRV_VIDEO_UPDATE(boomrang)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( prosoccr )

	MDRV_IMPORT_FROM(liberate)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_CLOCK(10000000/8) //xtal is unknown?
	MDRV_CPU_PROGRAM_MAP(prosoccr_map)
	MDRV_CPU_IO_MAP(prosoccr_io_map)

	MDRV_CPU_MODIFY("audiocpu")
	MDRV_CPU_CLOCK(10000000/8) //xtal is 12 Mhz, divider is unknown
	MDRV_CPU_PROGRAM_MAP(prosoccr_sound_map)

	MDRV_QUANTUM_TIME(HZ(12000))

	MDRV_SCREEN_MODIFY("screen")
	MDRV_SCREEN_VISIBLE_AREA(1*8, 31*8-1, 0*8, 32*8-1)

	MDRV_GFXDECODE(prosoccr)

	MDRV_VIDEO_START(prosoccr)
	MDRV_VIDEO_UPDATE(prosoccr)
MACHINE_DRIVER_END

#if 0
static MACHINE_DRIVER_START( prosport )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", DECO16, 2000000)
	MDRV_CPU_PROGRAM_MAP(prosport_map)
	MDRV_CPU_IO_MAP(deco16_io_map)
	MDRV_CPU_VBLANK_INT("screen", deco16_interrupt)

	MDRV_CPU_ADD("audiocpu", M6502, 1500000/2)
	MDRV_CPU_PROGRAM_MAP(liberate_sound_map)
	MDRV_CPU_PERIODIC_INT(nmi_line_pulse,16*60) /* ??? */

//  MDRV_QUANTUM_TIME(HZ(12000))

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(1529) /* 529ms Vblank duration?? */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(prosport)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(prosport)
	MDRV_VIDEO_UPDATE(prosport)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay1", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)

	MDRV_SOUND_ADD("ay2", AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END
#endif

/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( prosoccr )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "am08.9e",  0xa000, 0x2000, CRC(73d45d0d) SHA1(07736286087478af404bd9c6b279d631a01cf4e2) )
	ROM_LOAD( "am09.10e", 0xc000, 0x2000, CRC(a7ee0b3a) SHA1(87e487f863bd90c5b979c2d3c4317869ba1d71d9) )
	ROM_LOAD( "am10.11e", 0xe000, 0x2000, CRC(5571bdb8) SHA1(a3740650453c9e4f78dcc7826eb112d0d9f65b22) )
//low reload??
	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "am06.10a", 0xe000, 0x2000, CRC(37a0c74f) SHA1(5757b9eaf5b1129ee2d03b0ab6c3b15c120cf43c) )

	ROM_REGION( 0x6000, "shared_gfx", 0 ) // gfxs for foreground and sprites
	ROM_LOAD( "am00.2b",  0x0000, 0x2000, CRC(f3c8b649) SHA1(d2d42484e80d9241dac77a78c68314f88e0cbe5d) )
	ROM_LOAD( "am01.5b",  0x2000, 0x2000, CRC(24785bda) SHA1(536bdda766b46771223f01e463fa4c61e0dd545c) )
	ROM_LOAD( "am02.7b",  0x4000, 0x2000, CRC(c5af58ea) SHA1(a73d537b88befb76d67cc17d241e78c572c5b737) )

	ROM_REGION( 0x6000, "fg_gfx", ROMREGION_ERASE00 )
	ROM_COPY( "shared_gfx", 0x0800, 0x0800, 0x1800 )
	ROM_COPY( "shared_gfx", 0x2800, 0x2800, 0x1800 )
	ROM_COPY( "shared_gfx", 0x4800, 0x4800, 0x1800 )

	ROM_REGION( 0x6000, "sp_gfx", 0 )
	ROM_COPY( "shared_gfx", 0x0000, 0x0000, 0x2000 )
	ROM_COPY( "shared_gfx", 0x2000, 0x2000, 0x2000 )
	ROM_COPY( "shared_gfx", 0x4000, 0x4000, 0x2000 )

	ROM_REGION( 0x4000, "bg_gfx", 0 )
	ROM_LOAD( "am03.10b", 0x0000, 0x2000, CRC(47dc31dc) SHA1(7f492477e30a0353251a43e7e726551c3861b63f) )
	ROM_LOAD( "am04.c10", 0x2000, 0x2000, CRC(e057d827) SHA1(81ca4351777de5c32f4cf65547287c8169ba1494) )

	ROM_REGION(0x04000, "user1", 0 )
	ROM_LOAD( "am05.d12", 0x0000, 0x2000, CRC(f63e5a73) SHA1(50e7a1a0eb3bf8df3264bcba441c5fbd7dec52f4) )
	ROM_LOAD( "am07.7e",  0x2000, 0x2000, CRC(55415fb5) SHA1(676feb07d4fbd76aae8349b46f7edc8f357f2ddf) )

	ROM_REGION( 64, "proms", 0 )
	ROM_LOAD( "k1",    0, 32,  CRC(ebdc8343) SHA1(c9ae04da662f40237de24f5f01e97051e99e8c15) ) /* Colour */
	ROM_LOAD( "e13",  32, 32,  CRC(6909a061) SHA1(b9b2c1a7fec46027bfcc2c744946e27681c82b40) ) /* Timing? */
ROM_END

#if 0
ROM_START( prosport )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "ic21ar09.bin", 0x4000, 0x2000,  CRC(4faa8d12) SHA1(326216eb67d54ecd01701e4677f62b5c11b6763e) )
	ROM_LOAD( "ic22ar10.bin", 0x6000, 0x2000,  CRC(389e405b) SHA1(263088e49ab14a0017b2ad130bd78afcd0f13a4b) )
	ROM_LOAD( "ic23ar11.bin", 0x8000, 0x2000,  CRC(c0bc7f2a) SHA1(15d806bb8e28215178dbac0157d75e3ead42f6e9) )
	ROM_LOAD( "ic24ar12.bin", 0xa000, 0x2000,  CRC(4acd3f0d) SHA1(8bce597e4ba12d3cafa997653947e3aa6180b6c0) )
	ROM_LOAD( "ic25ar13.bin", 0xc000, 0x2000,  CRC(2bdabdf3) SHA1(530cd84dc7fbfdd6805bc555c0e9a5fa2175bc59) )
	ROM_LOAD( "ic26ar14.bin", 0xe000, 0x2000,  CRC(10ccfddb) SHA1(6c2d3cfd7be7cb4d3a217b1a70273ded5bd7e126) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ic43ar16.bin", 0xc000, 0x2000,  CRC(113a4f89) SHA1(abbc7f5ad543f3500c0194100d236ac942e4739f) )
	ROM_LOAD( "ic42ar15.bin", 0xe000, 0x2000,  CRC(635425a6) SHA1(2b95c3252046462f8886a309d02ea3a15b693780) )

	ROM_REGION( 0x12000, "fg_gfx_src", 0 )
	ROM_LOAD( "ic52ar00.bin",   0x00000, 0x2000, CRC(1e16adde) SHA1(229f68a687cbc9ac0d393e4db49d91f646eea7a6) )
	ROM_LOAD( "ic53ar01.bin",   0x02000, 0x2000, CRC(4b7a6431) SHA1(a8a23dffc3bf9fb3b806985272822904578e460e) )
	ROM_LOAD( "ic54ar02.bin",   0x04000, 0x2000, CRC(039eba80) SHA1(bd15f707f4d5dded8dd3373de5cb2a8d91a731d6) )

	ROM_LOAD( "ic55ar03.bin",   0x06000, 0x2000, CRC(caecafcb) SHA1(74c0e5aad65c162b9e58c1c37ec481cf3aa99056) )
	ROM_LOAD( "ic56ar04.bin",   0x08000, 0x2000, CRC(d555835e) SHA1(4e3f1b6418aec948aaf27d05a4736995763dd1aa) )
	ROM_LOAD( "ic57ar05.bin",   0x0a000, 0x2000, CRC(9d05c4cc) SHA1(898e4971d850c5f26513c4aabd548a41fdcf2b4f) )

	ROM_LOAD( "ic58ar06.bin",   0x0c000, 0x2000, CRC(903ea834) SHA1(93fc69a2b460ed4cc8945f34a761b9841eba15a3) )
	ROM_LOAD( "ic59ar07.bin",   0x0e000, 0x2000, CRC(e6527838) SHA1(e40acbcfda7d73ce4c1faa1c05e17d21bfc7f0d4) )
	ROM_LOAD( "ic60ar08.bin",   0x10000, 0x2000, CRC(ff1e6b01) SHA1(4561b718be41c67d713f6d7f10decc4d2eed9acc) )

	/* 0 - Title Screen */
	ROM_REGION( 0x6000, "prosport_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x04000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0a000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x10000, 0x04000, 0x2000 )

	/* 4 - Pro Tennis*/
	ROM_REGION( 0x6000, "protenns_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x02000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x08000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0e000, 0x04000, 0x2000 )

	/* 8 - Pro Bowling */
	ROM_REGION( 0x6000, "probowl_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x00000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x06000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0c000, 0x04000, 0x2000 )

	/* c - Pro Golf */
	ROM_REGION( 0x6000, "progolf_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x04000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0a000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x10000, 0x04000, 0x2000 )
	/* the following is WRONG! */
	ROM_COPY( "fg_gfx_src", 0x05000, 0x01800, 0x0800 )
	ROM_COPY( "fg_gfx_src", 0x0b000, 0x03800, 0x0800 )
	ROM_COPY( "fg_gfx_src", 0x11000, 0x05800, 0x0800 )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "ic46ar18.bin",   0x00000, 0x1000, CRC(d23998d3) SHA1(4d3545a0e1df2eb7927ec6fa4a35abd21321016c) )
	ROM_LOAD( "ic45ar17.bin",   0x01000, 0x1000, CRC(5f1c621e) SHA1(29ce85d3d5da5ee16bb67644b0555ab9bce52d05) )

	ROM_REGION(0x02000, "user1", 0 )
	ROM_COPY( "gfx2", 0x0000, 0x0000, 0x2000 )
ROM_END
#endif
#if 0
ROM_START( prosporta )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "ar09.21", 0x4000, 0x2000,  CRC(c839b404) SHA1(58d9ffd851ea09a2ca43da37c03433d9b327e547) )
	ROM_LOAD( "ar10.22", 0x6000, 0x2000,  CRC(5b70af2c) SHA1(2e14afde4f6443754a420f14190eab5d2a2d99cd) )
	ROM_LOAD( "ar11.23", 0x8000, 0x2000,  CRC(af05edd7) SHA1(655e56773a734999b1c4031cade5545279dd20b9) )
	ROM_LOAD( "ar12.24", 0xa000, 0x2000,  CRC(4acaa986) SHA1(c8c1e60f2a401514e111d15e78ecd4bdb6898a34) )
	ROM_LOAD( "ar13.25", 0xc000, 0x2000,  CRC(736f19ac) SHA1(ecfe8c0cdcef32187165d7d4ef0013c57a6fd969) )
	ROM_LOAD( "ar14.26", 0xe000, 0x2000,  CRC(38521ba3) SHA1(65bae8c1c15b8376c9b29771149fda36ee2c27d0) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "ar16.43", 0xc000, 0x2000,  CRC(113a4f89) SHA1(abbc7f5ad543f3500c0194100d236ac942e4739f) )
	ROM_LOAD( "ar15.42", 0xe000, 0x2000,  CRC(635425a6) SHA1(2b95c3252046462f8886a309d02ea3a15b693780) )

	ROM_REGION( 0x12000, "fg_gfx_src", 0 )
	ROM_LOAD( "ar00.52",   0x00000, 0x2000, CRC(1e16adde) SHA1(229f68a687cbc9ac0d393e4db49d91f646eea7a6) )
	ROM_LOAD( "ar01.53",   0x02000, 0x2000, CRC(4b7a6431) SHA1(a8a23dffc3bf9fb3b806985272822904578e460e) )
	ROM_LOAD( "ar02.54",   0x04000, 0x2000, CRC(cb22c60a) SHA1(0c83c15781cedb17ade7d48a606d1c49e8b80819) )

	ROM_LOAD( "ar03.55",   0x06000, 0x2000, CRC(caecafcb) SHA1(74c0e5aad65c162b9e58c1c37ec481cf3aa99056) )
	ROM_LOAD( "ar04.56",   0x08000, 0x2000, CRC(d555835e) SHA1(4e3f1b6418aec948aaf27d05a4736995763dd1aa) )
	ROM_LOAD( "ar05.57",   0x0a000, 0x2000, CRC(55b9b846) SHA1(cd2536acd58aec01815de8f609e0faeadfa00c43) )

	ROM_LOAD( "ar06.58",   0x0c000, 0x2000, CRC(903ea834) SHA1(93fc69a2b460ed4cc8945f34a761b9841eba15a3) )
	ROM_LOAD( "ar07.59",   0x0e000, 0x2000, CRC(e6527838) SHA1(e40acbcfda7d73ce4c1faa1c05e17d21bfc7f0d4) )
	ROM_LOAD( "ar08.60",   0x10000, 0x2000, CRC(37a2178b) SHA1(28655d0ebe5813b4fc7eb6dae2a64575214ebc6a) )

	/* 0 - Title Screen */
	ROM_REGION( 0x6000, "prosport_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x04000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0a000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x10000, 0x04000, 0x2000 )

	/* 4 - Pro Tennis*/
	ROM_REGION( 0x6000, "protenns_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x02000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x08000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0e000, 0x04000, 0x2000 )

	/* 8 - Pro Bowling */
	ROM_REGION( 0x6000, "probowl_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x00000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x06000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0c000, 0x04000, 0x2000 )

	/* c - Pro Golf */
	ROM_REGION( 0x6000, "progolf_fg_gfx", 0 )
	ROM_COPY( "fg_gfx_src", 0x04000, 0x00000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x0a000, 0x02000, 0x2000 )
	ROM_COPY( "fg_gfx_src", 0x10000, 0x04000, 0x2000 )
	/* the following is WRONG! */
	ROM_COPY( "fg_gfx_src", 0x05000, 0x01800, 0x0800 )
	ROM_COPY( "fg_gfx_src", 0x0b000, 0x03800, 0x0800 )
	ROM_COPY( "fg_gfx_src", 0x11000, 0x05800, 0x0800 )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "ar18.46",   0x00000, 0x1000, CRC(d23998d3) SHA1(4d3545a0e1df2eb7927ec6fa4a35abd21321016c) )
	ROM_LOAD( "ar17.45",   0x01000, 0x1000, CRC(5f1c621e) SHA1(29ce85d3d5da5ee16bb67644b0555ab9bce52d05) )

	ROM_REGION(0x02000, "user1", 0 )
	ROM_COPY( "gfx2", 0x0000, 0x0000, 0x2000 )
ROM_END
#endif

ROM_START( boomrang )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "bp13.9k",  0x8000, 0x4000,  CRC(b70439b1) SHA1(a020e9f6a71f72dfa72b8b202b4a08cca5e26ee0) )
	ROM_RELOAD(           0x0000, 0x4000 )
	ROM_LOAD( "bp14.11k", 0xc000, 0x4000,  CRC(98050e13) SHA1(2d936f95dc818883f735f92e9399470320e32a65) )

	ROM_REGION(0x10000, "audiocpu", 0)
	ROM_LOAD( "bp11.11f", 0xc000, 0x4000,  CRC(d6106f00) SHA1(068117d68eaabceb2e5890caf3f1761d89434f6c) )

	ROM_REGION(0xc000, "gfx1", 0 )
	ROM_LOAD( "bp04.7b",  0x00000, 0x4000, CRC(5d4b12eb) SHA1(f46a40e8509717d20224a60c2121bdcd3f3eff5a) )
	ROM_LOAD( "bp06.10b", 0x04000, 0x4000, CRC(5a18296e) SHA1(435fcbf7418aa3bec0fc0e86a3c17d3f7dfb2666) )
	ROM_LOAD( "bp08.13b", 0x08000, 0x4000, CRC(4cdb30d9) SHA1(7584792834926ebb0388f552b2c930ee84631c77) )

	ROM_REGION( 0x8000, "gfx2", 0 )
	ROM_LOAD( "bp02.4b",  0x00000, 0x4000, CRC(f3c2b84f) SHA1(e5c8d631058b73652c522d76618097f7289c0523) )
	ROM_LOAD( "bp00.1b",  0x04000, 0x4000, CRC(3370cf6e) SHA1(60a94e40d960fac611e69ba96dfa78fe747574e6) )

	ROM_REGION(0x04000, "user1", 0 )
	ROM_LOAD( "bp10.10a", 0x0000, 0x4000,  CRC(dd18a96f) SHA1(76ffa1bcf6377588c0b9b72508748c1cf2a0b303) )

	ROM_REGION( 32, "proms", 0 )
	ROM_LOAD( "82s123.5l",  0, 32,  CRC(a71e19ff) SHA1(fc7bf69f7971bf763aeeb1d9eb0861470acbf5d8) )
ROM_END

#if 0
ROM_START( boomranga )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "bp12-2",  0x8000, 0x2000,  CRC(87fc2f0b) SHA1(a5142cb3ee2c85906df2b5eccf7495486b162ae2) )
	ROM_RELOAD(          0x0000, 0x2000 )
	ROM_LOAD( "bp13-2",  0xa000, 0x2000,  CRC(8e864764) SHA1(8faea69cb087b19de77589ea24d6f99ca0237deb) )
	ROM_RELOAD(          0x2000, 0x2000 )
	ROM_LOAD( "bp14-",  0x0c000, 0x2000, CRC(0a64018a) SHA1(318124619440af5b19bd4dc74ab3075ea03a6833) )
	ROM_LOAD( "bp15-",  0x0e000, 0x2000, CRC(d23a5c31) SHA1(e61fee651ee80b03bf5813ec0cebb022bd0285bf) )


	ROM_REGION(0x10000, "audiocpu", 0) /* same content, alt layout */
	ROM_LOAD( "bp11-",  0x0c000, 0x2000, CRC(bbafe1ff) SHA1(c786465d714d576d5bbbfea588d34850a317dd9d) )
	ROM_RELOAD(0xe000,0x2000)

	ROM_REGION(0xc000, "gfx1", 0 ) /* same content, alt rom layout */
	ROM_LOAD( "bp03-",  0x00000, 0x2000, CRC(33565e00) SHA1(5ddf179d222db7b3ad965ede2f25590d93dfdbf7) )
	ROM_LOAD( "bp04-",  0x02000, 0x2000, CRC(ab3ccae2) SHA1(d81d3bf38d8b9b05792c0b74e2573c23c01eef4a) )
	ROM_LOAD( "bp05-",  0x04000, 0x2000, CRC(8a8d25fa) SHA1(af9ae6a53f5ae2c97c8632e30d63830a82b3a2fd) )
	ROM_LOAD( "bp06-",  0x06000, 0x2000, CRC(aa64bacd) SHA1(6a5e58abcd3753f9766376b31b4d2098a172dc8d) )
	ROM_LOAD( "bp07-",  0x08000, 0x2000, CRC(6c7370aa) SHA1(e2c874a7f9205b6af9cd3336239839f659b93dae) )
	ROM_LOAD( "bp08-",  0x0a000, 0x2000, CRC(348bd0cc) SHA1(a99aa9bf8c707b2f2a9539a118b0b74e20e1e923) )

	ROM_REGION( 0x8000, "gfx2", 0 ) /* same content, alt rom layout */
	ROM_LOAD( "bp01-",  0x00000, 0x2000, CRC(b4000aff) SHA1(7706ddd4ab3ef07df0e4f41635725781f8b0c386) )
	ROM_LOAD( "bp02-",  0x02000, 0x2000, CRC(90044512) SHA1(4c0fd9c95796e4281ec2de8639990717f8216bec) )
	ROM_LOAD( "bp00-",  0x04000, 0x2000, CRC(e33564e5) SHA1(69563fea82716b440dbc2b5912078c70727aa560) )

	ROM_REGION(0x04000, "user1", 0 ) /* same content, alt rom layout */
	ROM_LOAD( "bp10-",  0x00000, 0x2000, CRC(cb3b0f60) SHA1(0fe8e11b1e24db7881a9f00cb700495e1e0daf84) )
	ROM_LOAD( "bp09-",  0x02000, 0x2000, CRC(a64ac71d) SHA1(0e6525aa037a11f83ca7e23572b488c6f8ddab63) )

	ROM_REGION( 32, "proms", 0 )
	ROM_LOAD( "ap-16.5l",  0, 32,  CRC(a71e19ff) SHA1(fc7bf69f7971bf763aeeb1d9eb0861470acbf5d8) )
ROM_END
#endif

ROM_START( kamikcab )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "bp11", 0x0c000, 0x4000, CRC(a69e5580) SHA1(554e45a3f5a91864b62a2439c2277cd18dbe45a7) )
	ROM_RELOAD(       0x00000, 0x4000 )

	ROM_REGION(0x10000, "audiocpu", 0)	/* 64K for CPU 2 */
	ROM_LOAD( "bp09", 0x0e000, 0x2000, CRC(16b13676) SHA1(f3cad959cbcde243db3ebc77a3692302a44beb09) )

	ROM_REGION(0xc000, "gfx1", 0 )
	ROM_LOAD( "bp04", 0x00000, 0x4000, CRC(b471542d) SHA1(aad323da7771c2ffdb04a60a4b4bbe032f5b1865) )
	ROM_LOAD( "bp06", 0x04000, 0x4000, CRC(4bf96d0d) SHA1(d7cd0e1da2d64e5b9318618b0ddd848ac405f28a) )
	ROM_LOAD( "bp08", 0x08000, 0x4000, CRC(b4756bed) SHA1(83c39ac5f4628f14a5f2ded6c9a9ed4874d2d8b6) )

	ROM_REGION(0x8000, "gfx2", 0 )
	ROM_LOAD( "bp02", 0x00000, 0x4000, CRC(77299e6e) SHA1(477a6f466f08fe99823dc55d246b4d732423663d) )
	ROM_LOAD( "bp00", 0x04000, 0x2000, CRC(c20ca7ca) SHA1(ca91af848ae38b296992bb21040ef22a325bbcdc) )

	ROM_REGION(0x4000, "user1", 0 )
	ROM_LOAD( "bp12", 0x00000, 0x4000, CRC(8c8f5d35) SHA1(5b908d92786dae76aaf84de14f8847ee8ee350a1) )

	ROM_REGION(32, "proms", 0 )
	ROM_LOAD( "bp15", 0, 32,  CRC(30d3acce) SHA1(be88d74250edc2920fc0f95cfdd93468ac9c640e) )
ROM_END

#if 0
ROM_START( yellowcbj )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "bp11.bin", 0xe000, 0x2000, CRC(63cdee83) SHA1(c4a5c3d7761336f4fc3fe83490b69812d70b2cde) )
	ROM_RELOAD(            0x2000, 0x2000 )
	ROM_LOAD( "bp10.bin", 0xc000, 0x2000, CRC(1024f2f1) SHA1(a3804df3c9ecfde9318ed121327ef095d7a9e1f0) )
	ROM_RELOAD(            0x0000, 0x2000 )

	ROM_REGION(0x10000, "audiocpu", 0)	/* 64K for CPU 2 */
	ROM_LOAD( "bp09", 0x0e000, 0x2000, CRC(16b13676) SHA1(f3cad959cbcde243db3ebc77a3692302a44beb09) )

	ROM_REGION(0xc000, "gfx1", 0 )
	ROM_LOAD( "bp03.bin", 0x0000, 0x2000, CRC(6761d767) SHA1(f5180cc31d3b4103c78b74e5de311565581d6db7) )
	ROM_LOAD( "bp04.bin", 0x2000, 0x2000, CRC(55517196) SHA1(f46899751e614aa35ec316caacb84e25193a56d6) )
	ROM_LOAD( "bp05.rom", 0x4000, 0x2000, CRC(33658fd9) SHA1(16c1d211b8fb46467906cd2cd32bcc9eac464573) )
	ROM_LOAD( "bp06.rom", 0x6000, 0x2000, CRC(fbc20f07) SHA1(7f7028638038f61dae6aca09d393adeea3ab1b2d) )
	ROM_LOAD( "bp07.rom", 0x8000, 0x2000, CRC(061f9e54) SHA1(2e7b7c98892702513e53b8dd2a2a0dc152d52016) )
	ROM_LOAD( "bp08.rom", 0xa000, 0x2000, CRC(2ace626d) SHA1(6f12eb2d6a01ac1285d15d15948124b85c6964b7) )

	ROM_REGION(0x8000, "gfx2", 0 )
	ROM_LOAD( "bp01.bin", 0x0000, 0x2000, CRC(8e92a253) SHA1(94063c5aae9ce36b47e51f55e7502d1b6c4334be) )
	ROM_LOAD( "bp02.bin", 0x2000, 0x2000, CRC(47ada1bb) SHA1(cc0a38d0cc220382c36e6268f47db332bdbb07a2) )
	ROM_LOAD( "bp00.bin", 0x4000, 0x2000, CRC(9ead0da1) SHA1(5e46527fa38d75efd266ca982c7cb6d88bb3ea49) )

	ROM_REGION(0x4000, "user1", 0 )
	ROM_LOAD( "bp12", 0x00000, 0x4000, CRC(8c8f5d35) SHA1(5b908d92786dae76aaf84de14f8847ee8ee350a1) )

	ROM_REGION(32, "proms", 0 )
	ROM_LOAD( "bp15", 0, 32,  CRC(30d3acce) SHA1(be88d74250edc2920fc0f95cfdd93468ac9c640e) )
ROM_END
#endif
#if 0
ROM_START( yellowcbb )
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "rom11.rom", 0xc000, 0x2000, CRC(af97d530) SHA1(b8b9bfcb2e9164daa115b91a533418a39c40c31d) )
	ROM_RELOAD(            0x0000, 0x2000 )
	ROM_LOAD( "rom10.rom", 0xe000, 0x2000, CRC(33c3e9b9) SHA1(7ea6602204c43a86842a0b0f7a0786913a6707d6) )
	ROM_RELOAD(            0x2000, 0x2000 )

	ROM_REGION(0x10000, "audiocpu", 0)	/* 64K for CPU 2 */
	ROM_LOAD( "bp09", 0x0e000, 0x2000, CRC(16b13676) SHA1(f3cad959cbcde243db3ebc77a3692302a44beb09) )

	ROM_REGION(0xc000, "gfx1", 0 )
	ROM_LOAD( "rom3.rom", 0x0000, 0x2000, CRC(6761d767) SHA1(f5180cc31d3b4103c78b74e5de311565581d6db7) )
	ROM_LOAD( "rom4.rom", 0x2000, 0x2000, CRC(55517196) SHA1(f46899751e614aa35ec316caacb84e25193a56d6) )
	ROM_LOAD( "rom5.rom", 0x4000, 0x2000, CRC(33658fd9) SHA1(16c1d211b8fb46467906cd2cd32bcc9eac464573) )
	ROM_LOAD( "rom6.rom", 0x6000, 0x2000, CRC(fbc20f07) SHA1(7f7028638038f61dae6aca09d393adeea3ab1b2d) )
	ROM_LOAD( "rom7.rom", 0x8000, 0x2000, CRC(061f9e54) SHA1(2e7b7c98892702513e53b8dd2a2a0dc152d52016) )
	ROM_LOAD( "rom8.rom", 0xa000, 0x2000, CRC(2ace626d) SHA1(6f12eb2d6a01ac1285d15d15948124b85c6964b7) )

	ROM_REGION(0x8000, "gfx2", 0 )
	ROM_LOAD( "rom1.rom", 0x0000, 0x2000, CRC(8e92a253) SHA1(94063c5aae9ce36b47e51f55e7502d1b6c4334be) )
	ROM_LOAD( "rom2.rom", 0x2000, 0x2000, CRC(47ada1bb) SHA1(cc0a38d0cc220382c36e6268f47db332bdbb07a2) )
	ROM_LOAD( "rom0.rom", 0x4000, 0x2000, CRC(9ead0da1) SHA1(5e46527fa38d75efd266ca982c7cb6d88bb3ea49) )

	ROM_REGION(0x4000, "user1", 0 )
	ROM_LOAD( "bp12", 0x00000, 0x4000, CRC(8c8f5d35) SHA1(5b908d92786dae76aaf84de14f8847ee8ee350a1) )

	ROM_REGION(32, "proms", 0 )
	ROM_LOAD( "bp15", 0, 32,  CRC(30d3acce) SHA1(be88d74250edc2920fc0f95cfdd93468ac9c640e) )
ROM_END
#endif

ROM_START( liberate )
	ROM_REGION( 0x10000, "maincpu", 0 )
 	ROM_LOAD( "bt12-2.bin", 0x8000, 0x4000, CRC(a0079ffd) SHA1(340398352500a33f01dca07dd9c86ad3a78f227e) )
	ROM_RELOAD(             0x0000, 0x4000 )
 	ROM_LOAD( "bt13-2.bin", 0xc000, 0x4000, CRC(19f8485c) SHA1(1e2a68e4cf6b96c53832f7d020f14a45de19967d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "bt11.bin",  0xe000, 0x2000,  CRC(b549ccaa) SHA1(e4c8350fea61ed85d21037cbd4c3c50f9a9de09f) )

	ROM_REGION( 0x12000, "gfx1", 0 )
	ROM_LOAD( "bt04.bin", 0x00000, 0x4000, CRC(96e48d72) SHA1(c31a58d6f1a3354b234849bf7ee013fe59bf908e) )	/* Chars/Sprites */
	ROM_LOAD( "bt03.bin", 0x04000, 0x2000, CRC(29ad1b59) SHA1(4d5a385ccad4cdebe87300ef08e1220bc9303673) )
	ROM_LOAD( "bt06.bin", 0x06000, 0x4000, CRC(7bed1497) SHA1(ba309f468d98269014b2a757b8e98496d7e29120) )
	ROM_LOAD( "bt05.bin", 0x0a000, 0x2000, CRC(a8896c20) SHA1(c21412c8a6b10719d324ce7ecb01ec4e9d803932) )
	ROM_LOAD( "bt08.bin", 0x0c000, 0x4000, CRC(828ef78d) SHA1(79076b5552e6aff032839f2daca952305c863a64) )
	ROM_LOAD( "bt07.bin", 0x10000, 0x2000, CRC(f919e8e2) SHA1(e9eafa10f024aa522947f6098480bddf1fbe960f) )

	ROM_REGION( 0x8000, "gfx2", 0 )
 	ROM_LOAD( "bt02.bin", 0x0000, 0x4000, CRC(7169f7bb) SHA1(06e45a15d7e878d0a6063c2fab55d065334935b2) )
 	ROM_LOAD( "bt00.bin", 0x4000, 0x2000, CRC(b744454d) SHA1(664619c3907c538f353d8ac04d66086dcfbd53d4) )
	/* On early revision bt02 is split as BT01-A (0x2000) BT02-A (0x2000) */

	ROM_REGION(0x4000, "user1", 0 )
	ROM_LOAD( "bt10.bin",  0x0000, 0x4000,  CRC(ee335397) SHA1(2d54f93d330357033b8ebc4bc052383c25156311) )

	ROM_REGION( 32, "proms", 0 )
	ROM_LOAD( "bt14.bin", 0x0000, 32,  CRC(20281d61) SHA1(905dd2744c148d50332fcad34a57dc573d41bb0a) )
ROM_END

#if 0
ROM_START( dualaslt )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "bt12",       0x8000, 0x4000, CRC(1434ee46) SHA1(c431982c25323787b8e2ac1b433fc0e81650fbf9) )
	ROM_RELOAD(             0x0000, 0x4000 )
	ROM_LOAD( "bt13",       0xc000, 0x4000, CRC(38e0ffa4) SHA1(c450960cdcfa9b2b136f96bc1e3a37995a37f60c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "bt11.bin",  0xe000, 0x2000,  CRC(b549ccaa) SHA1(e4c8350fea61ed85d21037cbd4c3c50f9a9de09f) )

	ROM_REGION( 0x12000, "gfx1", 0 )
	ROM_LOAD( "bt04-5",   0x00000, 0x4000, CRC(159a3e85) SHA1(e916ee7e96c7c64d9ef05ff410d0cbba4d1b8ad0) ) 	/* Chars/Sprites */
	ROM_LOAD( "bt03.bin", 0x04000, 0x2000, CRC(29ad1b59) SHA1(4d5a385ccad4cdebe87300ef08e1220bc9303673) )
	ROM_LOAD( "bt06-5",   0x06000, 0x4000, CRC(3b5a80c8) SHA1(8b55b18ab46a64381fc135e84ab82fc451ee722d) )
	ROM_LOAD( "bt05.bin", 0x0a000, 0x2000, CRC(a8896c20) SHA1(c21412c8a6b10719d324ce7ecb01ec4e9d803932) )
	ROM_LOAD( "bt08-5",   0x0c000, 0x4000, CRC(b0cebde8) SHA1(9ed418705f56bc43fd864dbfb412cd7062229006) )
	ROM_LOAD( "bt07.bin", 0x10000, 0x2000, CRC(f919e8e2) SHA1(e9eafa10f024aa522947f6098480bddf1fbe960f) )

	ROM_REGION( 0x8000, "gfx2", 0 )
	ROM_LOAD( "bt01",     0x0000, 0x2000, CRC(c0ddbeb5) SHA1(043c3ef2aa62a0e8b650d4daddb944e694078e01) )
	ROM_LOAD( "bt02a",    0x2000, 0x2000, CRC(846d9d24) SHA1(d0c395876601cf666dcf40a0f95e461095a98df4) )
 	ROM_LOAD( "bt00.bin", 0x4000, 0x2000, CRC(b744454d) SHA1(664619c3907c538f353d8ac04d66086dcfbd53d4) )

	ROM_REGION(0x4000, "user1", 0 )
	ROM_LOAD( "bt09",  0x0000, 0x2000, CRC(2ea31472) SHA1(2e3125b53755260a036dfc2940970eeb4c5c058b) )
	ROM_LOAD( "bt10a", 0x2000, 0x2000, CRC(69d9aa8d) SHA1(4587481d304bf34de56ab027668b22e716ee1da7) )

	ROM_REGION( 32, "proms", 0 )
	ROM_LOAD( "bt14.bin", 0x0000, 32,  CRC(20281d61) SHA1(905dd2744c148d50332fcad34a57dc573d41bb0a) )
ROM_END
#endif
#if 0
ROM_START( liberateb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "liber6.c17", 0x8000, 0x2000, CRC(c1811fe0) SHA1(1f857042ce00e489c2e73bb459b81a2461ea0b25) )
	ROM_RELOAD(             0x0000, 0x2000)
	ROM_LOAD( "liber4.c18", 0xa000, 0x2000, CRC(0e8db1ce) SHA1(bb7b77c31b3bb2c0d523f5cad4ef46d42a9dc857) )
	ROM_RELOAD(             0x2000, 0x2000)
	ROM_LOAD( "liber3.c20", 0xc000, 0x2000, CRC(16c423f3) SHA1(0cf3c46c9fc13eb0f61a3945d3db6ca2f9ab76fe) )
 	ROM_LOAD( "liber5.c19", 0xe000, 0x2000, CRC(7738c194) SHA1(54fb094150481640f40d8a2066e43dc647980cda) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "bt11.bin",  0xe000, 0x2000,  CRC(b549ccaa) SHA1(e4c8350fea61ed85d21037cbd4c3c50f9a9de09f) )

	ROM_REGION( 0x12000, "gfx1", 0 )
	ROM_LOAD( "bt04.bin", 0x00000, 0x4000, CRC(96e48d72) SHA1(c31a58d6f1a3354b234849bf7ee013fe59bf908e) )	/* Chars/Sprites */
	ROM_LOAD( "bt03.bin", 0x04000, 0x2000, CRC(29ad1b59) SHA1(4d5a385ccad4cdebe87300ef08e1220bc9303673) )
	ROM_LOAD( "bt06.bin", 0x06000, 0x4000, CRC(7bed1497) SHA1(ba309f468d98269014b2a757b8e98496d7e29120) )
	ROM_LOAD( "bt05.bin", 0x0a000, 0x2000, CRC(a8896c20) SHA1(c21412c8a6b10719d324ce7ecb01ec4e9d803932) )
	ROM_LOAD( "bt08.bin", 0x0c000, 0x4000, CRC(828ef78d) SHA1(79076b5552e6aff032839f2daca952305c863a64) )
	ROM_LOAD( "bt07.bin", 0x10000, 0x2000, CRC(f919e8e2) SHA1(e9eafa10f024aa522947f6098480bddf1fbe960f) )

	ROM_REGION( 0x8000, "gfx2", 0 )
 	ROM_LOAD( "bt02.bin", 0x0000, 0x4000, CRC(7169f7bb) SHA1(06e45a15d7e878d0a6063c2fab55d065334935b2) )
 	ROM_LOAD( "bt00.bin", 0x4000, 0x2000, CRC(b744454d) SHA1(664619c3907c538f353d8ac04d66086dcfbd53d4) )

	ROM_REGION(0x4000, "user1", 0 )
	ROM_LOAD( "bt10.bin",  0x0000, 0x4000,  CRC(ee335397) SHA1(2d54f93d330357033b8ebc4bc052383c25156311) )

	ROM_REGION( 32, "proms", 0 )
	ROM_LOAD( "bt14.bin", 0x0000, 32,  CRC(20281d61) SHA1(905dd2744c148d50332fcad34a57dc573d41bb0a) )
ROM_END
#endif
/*************************************
 *
 *  Game-specific driver inits
 *
 *************************************/

static void sound_cpu_decrypt(running_machine *machine)
{
	const address_space *space = cputag_get_address_space(machine, "audiocpu", ADDRESS_SPACE_PROGRAM);
	UINT8 *decrypted = auto_alloc_array(machine, UINT8, 0x4000);
	UINT8 *rom = memory_region(machine, "audiocpu");
	int i;

	/* Bit swapping on sound cpu - Opcodes only */
	for (i=0xc000; i<0x10000; i++)
		decrypted[i-0xc000]=((rom[i] & 0x20) << 1) | ((rom[i] & 0x40) >> 1) | (rom[i] & 0x9f);

	memory_set_decrypted_region(space, 0xc000, 0xffff, decrypted);
}

static DRIVER_INIT( prosport )
{
	UINT8 *RAM = memory_region(machine, "maincpu");
	int i;

	/* Main cpu has the nibbles swapped */
	for (i=0; i<0x10000; i++)
		RAM[i]=((RAM[i] & 0x0f) << 4) | ((RAM[i] & 0xf0) >> 4);

	sound_cpu_decrypt(machine);
}

#if 0
static DRIVER_INIT( yellowcb )
{
	DRIVER_INIT_CALL(prosport);

	memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xa000, 0xa000, 0, 0, "IN0");
}
#endif

static DRIVER_INIT( liberate )
{
	int A;
	const address_space *space = cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM);
	UINT8 *decrypted = auto_alloc_array(machine, UINT8, 0x10000);
	UINT8 *ROM = memory_region(machine, "maincpu");

	memory_set_decrypted_region(space, 0x0000, 0xffff, decrypted);

	/* Swap bits for opcodes only, not data */
	for (A = 0;A < 0x10000;A++) {
		decrypted[A] = (ROM[A] & 0xd7) | ((ROM[A] & 0x08) << 2) | ((ROM[A] & 0x20) >> 2);
		decrypted[A] = (decrypted[A] & 0xbb) | ((decrypted[A] & 0x04) << 4) | ((decrypted[A] & 0x40) >> 4);
		decrypted[A] = (decrypted[A] & 0x7d) | ((decrypted[A] & 0x02) << 6) | ((decrypted[A] & 0x80) >> 6);
	}

	memory_configure_bank_decrypted(machine, 1, 0, 1, decrypted + 0x8000, 0x10);

	sound_cpu_decrypt(machine);
}

/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1983, prosoccr,  0,        prosoccr,  prosoccr, prosport, ROT270, "Data East Corporation", "Pro Soccer", 0 )
//GAME( 1983, prosport,  0,        prosport,  prosport, prosport, ROT270, "Data East Corporation", "Pro. Sports", GAME_NO_COCKTAIL | GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
//GAME( 1983, prosporta, prosport, prosport,  prosport, prosport, ROT270, "Data East Corporation", "Pro. Sports (alternate)", GAME_NO_COCKTAIL | GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_SOUND )
GAME( 1983, boomrang,  0,        boomrang,  boomrang, prosport, ROT270, "Data East Corporation", "Boomer Rang'r / Genesis (set 1)", 0 )
//GAME( 1983, boomranga, boomrang, boomrang,  boomrang, prosport, ROT270, "Data East Corporation", "Boomer Rang'r / Genesis (set 2)", 0 )
GAME( 1984, kamikcab,  0,        boomrang,  kamikcab, prosport, ROT270, "Data East Corporation", "Kamikaze Cabbie", 0 )
//GAME( 1984, yellowcbj, kamikcab, boomrang,  yellowcb, yellowcb, ROT270, "Data East Corporation", "Yellow Cab (Japan)", 0 )
//GAME( 1984, yellowcbb, kamikcab, boomrang,  yellowcb, yellowcb, ROT270, "bootleg",               "Yellow Cab (bootleg)", 0 )
GAME( 1984, liberate,  0,        liberate,  liberate, liberate, ROT270, "Data East Corporation", "Liberation", 0 )
//GAME( 1984, dualaslt,  liberate, liberate,  dualaslt, liberate, ROT270, "Data East USA",         "Dual Assault", 0 )
//GAME( 1984, liberateb, liberate, liberatb,  liberatb, prosport, ROT270, "bootleg",               "Liberation (bootleg)", 0 )
