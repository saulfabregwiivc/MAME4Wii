#define RNG_DEBUG 0

/*
   Run and Gun / Slam Dunk
   (c) 1993 Konami

   Driver by R. Belmont.

   This hardware uses the 55673 sprite chip like PreGX and System GX, but in a 4 bit
   per pixel layout.  There is also an all-TTL front overlay tilemap and a rotating
   scaling background done with the PSAC2 ('936).

   Status: Front tilemap should be complete, sprites are mostly correct, controls
   should be fine.


   Change Log:

   (AT070703)
   drivers\rungun.c (this file)
     - mem maps, device settings, component communications, I/O's, sound...etc.

   video\rungun.c
     - general clean-up, clipping, alignment

   video\konamiic.c
     - missing sprites and priority

   Known Issues:
     - no dual monitor support
     - synchronization and other oddities (rungunu doesn't show attract mode)
     - swapped P12 and P34 controls in 4-player mode team selectet (real puzzler)
     - P3 and P4 coin chutes not working in 4-player mode
     - sprite palettes are not entirely right
     - ROZ update causes music to stutter
*/
#if 0

#include "driver.h"

#include "video/konamiic.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "machine/eeprom.h"
#include "sound/k054539.h"
#include "konamipt.h"

VIDEO_START( rng );
VIDEO_UPDATE( rng );
static MACHINE_RESET( rng );
READ16_HANDLER( rng_ttl_ram_r );
WRITE16_HANDLER( rng_ttl_ram_w );
WRITE16_HANDLER( rng_936_videoram_w );

extern UINT16 *rng_936_videoram;

static UINT16 *rng_sysreg;
static int init_eeprom_count;
static int rng_z80_control;
static int rng_sound_status;

static const eeprom_interface eeprom_intf =
{
	7,			/* address bits */
	8,			/* data bits */
	"011000",		/*  read command */
	"011100",		/* write command */
	"0100100000000",/* erase command */
	"0100000000000",/* lock command */
	"0100110000000" /* unlock command */
};

static NVRAM_HANDLER( rungun )
{
	if (read_or_write)
		eeprom_save(file);
	else
	{
		eeprom_init(machine, &eeprom_intf);

		if (file)
		{
			init_eeprom_count = 0;
			eeprom_load(file);
		}
		else
			init_eeprom_count = 10;
	}
}

static READ16_HANDLER( rng_sysregs_r )
{
	UINT16 data = 0;

	switch (offset)
	{
		case 0x00/2:
			if (input_port_read(space->machine, "DSW") & 0x20)
				return(input_port_read(space->machine, "P1") | input_port_read(space->machine, "P3")<<8);
			else
			{
				data = input_port_read(space->machine, "P1") & input_port_read(space->machine, "P3");
				return(data<<8 | data);
			}

		case 0x02/2:
			if (input_port_read(space->machine, "DSW") & 0x20)
				return(input_port_read(space->machine, "P2") | input_port_read(space->machine, "P4")<<8);
			else
			{
				data = input_port_read(space->machine, "P2") & input_port_read(space->machine, "P4");
				return(data<<8 | data);
			}

		case 0x04/2:
			/*
                bit0-7: coin mechs and services
                bit8 : freeze
                bit9 : joysticks layout(auto detect???)
            */
			return input_port_read(space->machine, "SYSTEM");

		case 0x06/2:
			if (ACCESSING_BITS_0_7)
			{
				data = input_port_read(space->machine, "DSW");

				if (init_eeprom_count)
				{
					init_eeprom_count--;
					data &= 0xf7;
				}
			}
			return((rng_sysreg[0x06/2] & 0xff00) | data);
	}

	return(rng_sysreg[offset]);
}

static WRITE16_HANDLER( rng_sysregs_w )
{
	COMBINE_DATA(rng_sysreg + offset);

	switch (offset)
	{
		case 0x08/2:
			/*
                bit0  : eeprom_write_bit
                bit1  : eeprom_set_cs_line
                bit2  : eeprom_set_clock_line
                bit3  : coin counter?
                bit7  : set before massive memory writes
                bit10 : IRQ5 ACK
            */
			if (ACCESSING_BITS_0_7)
			{
				eeprom_write_bit((data & 0x01) ? 1 : 0);
				eeprom_set_cs_line((data & 0x02) ? CLEAR_LINE : ASSERT_LINE);
				eeprom_set_clock_line((data & 0x04) ? ASSERT_LINE : CLEAR_LINE);
			}

			if (!(data & 0x40))
				cputag_set_input_line(space->machine, "maincpu", M68K_IRQ_5, CLEAR_LINE);
		break;

		case 0x0c/2:
			/*
                bit 0 : also enables IRQ???
                bit 1 : disable PSAC2 input?
                bit 2 : OBJCHA
                bit 3 : enable IRQ 5
            */
			K053246_set_OBJCHA_line((data & 0x04) ? ASSERT_LINE : CLEAR_LINE);
		break;
	}
}

static WRITE16_HANDLER( sound_cmd1_w )
{
	if (ACCESSING_BITS_8_15)
		soundlatch_w(space, 0, data >> 8);
}

static WRITE16_HANDLER( sound_cmd2_w )
{
	if (ACCESSING_BITS_8_15)
		soundlatch2_w(space, 0, data>>8);
}

static WRITE16_HANDLER( sound_irq_w )
{
	if (ACCESSING_BITS_8_15)
		cputag_set_input_line(space->machine, "soundcpu", 0, HOLD_LINE);
}

static READ16_HANDLER( sound_status_msb_r )
{
	if (ACCESSING_BITS_8_15)
		return(rng_sound_status << 8);

	return(0);
}

static INTERRUPT_GEN(rng_interrupt)
{
	if (rng_sysreg[0x0c/2] & 0x09)
		cpu_set_input_line(device, M68K_IRQ_5, ASSERT_LINE);
}

static ADDRESS_MAP_START( rungun_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x2fffff) AM_ROM											// main program + data
	AM_RANGE(0x300000, 0x3007ff) AM_RAM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x380000, 0x39ffff) AM_RAM											// work RAM
	AM_RANGE(0x400000, 0x43ffff) AM_READNOP	// AM_READ( K053936_0_rom_r )       // '936 ROM readback window
	AM_RANGE(0x480000, 0x48001f) AM_READWRITE(rng_sysregs_r,rng_sysregs_w) AM_BASE(&rng_sysreg)
	AM_RANGE(0x4c0000, 0x4c001f) AM_READ(K053252_word_r)						// CCU (for scanline and vblank polling)
	AM_RANGE(0x540000, 0x540001) AM_WRITE(sound_irq_w)
	AM_RANGE(0x58000c, 0x58000d) AM_WRITE(sound_cmd1_w)
	AM_RANGE(0x58000e, 0x58000f) AM_WRITE(sound_cmd2_w)
	AM_RANGE(0x580014, 0x580015) AM_READ(sound_status_msb_r)
	AM_RANGE(0x580000, 0x58001f) AM_RAM											// sound regs read/write fall-through
	AM_RANGE(0x5c0000, 0x5c000d) AM_READ(K053246_word_r)						// 246A ROM readback window
	AM_RANGE(0x5c0010, 0x5c001f) AM_WRITE(K053247_reg_word_w)
	AM_RANGE(0x600000, 0x600fff) AM_READWRITE(K053247_word_r,K053247_word_w)	// OBJ RAM
	AM_RANGE(0x601000, 0x601fff) AM_RAM											// communication? second monitor buffer?
	AM_RANGE(0x640000, 0x640007) AM_WRITE(K053246_word_w)						// '246A registers
	AM_RANGE(0x680000, 0x68001f) AM_WRITEONLY AM_BASE(&K053936_0_ctrl)			// '936 registers
	AM_RANGE(0x6c0000, 0x6cffff) AM_RAM_WRITE(rng_936_videoram_w) AM_BASE(&rng_936_videoram)	// PSAC2 ('936) RAM (34v + 35v)
	AM_RANGE(0x700000, 0x7007ff) AM_RAM AM_BASE(&K053936_0_linectrl)			// PSAC "Line RAM"
	AM_RANGE(0x740000, 0x741fff) AM_READWRITE(rng_ttl_ram_r,rng_ttl_ram_w)		// text plane RAM
	AM_RANGE(0x7c0000, 0x7c0001) AM_WRITENOP									// watchdog
#if RNG_DEBUG
	AM_RANGE(0x5c0010, 0x5c001f) AM_READ(K053247_reg_word_r)
	AM_RANGE(0x640000, 0x640007) AM_READ(K053246_reg_word_r)
#endif
ADDRESS_MAP_END


/**********************************************************************************/

static WRITE8_HANDLER( sound_status_w )
{
	rng_sound_status = data;
}

static WRITE8_HANDLER( z80ctrl_w )
{
	rng_z80_control = data;

	memory_set_bankptr(space->machine, 2, memory_region(space->machine, "soundcpu") + 0x10000 + (data & 0x07) * 0x4000);

	if (data & 0x10)
		cputag_set_input_line(space->machine, "soundcpu", INPUT_LINE_NMI, CLEAR_LINE);
}

static INTERRUPT_GEN(audio_interrupt)
{
	if (rng_z80_control & 0x80) return;

	cpu_set_input_line(device, INPUT_LINE_NMI, ASSERT_LINE);
}

/* sound (this should be split into audio/xexex.c or pregx.c or so someday) */

static ADDRESS_MAP_START( rungun_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(2)
	AM_RANGE(0xc000, 0xdfff) AM_RAM
	AM_RANGE(0xe000, 0xe22f) AM_DEVREADWRITE("konami1", k054539_r,k054539_w)
	AM_RANGE(0xe230, 0xe3ff) AM_RAM
	AM_RANGE(0xe400, 0xe62f) AM_DEVREADWRITE("konami2", k054539_r,k054539_w)
	AM_RANGE(0xe630, 0xe7ff) AM_RAM
	AM_RANGE(0xf000, 0xf000) AM_WRITE(sound_status_w)
	AM_RANGE(0xf002, 0xf002) AM_READ(soundlatch_r)
	AM_RANGE(0xf003, 0xf003) AM_READ(soundlatch2_r)
	AM_RANGE(0xf800, 0xf800) AM_WRITE(z80ctrl_w)
	AM_RANGE(0xfff0, 0xfff3) AM_WRITENOP
ADDRESS_MAP_END


static const k054539_interface k054539_config =
{
	"shared"
};

/**********************************************************************************/

static const gfx_layout bglayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4, 8*4,
	  9*4, 10*4, 11*4, 12*4, 13*4, 14*4, 15*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
			8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8
};

static GFXDECODE_START( rungun )
	GFXDECODE_ENTRY( "gfx1", 0, bglayout, 0x0000, 64 )
GFXDECODE_END

static MACHINE_DRIVER_START( rng )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 16000000)
	MDRV_CPU_PROGRAM_MAP(rungun_map)
	MDRV_CPU_VBLANK_INT("screen", rng_interrupt)

	MDRV_CPU_ADD("soundcpu", Z80, 10000000) // 8Mhz (10Mhz is much safer in self-test due to heavy sync)
	MDRV_CPU_PROGRAM_MAP(rungun_sound_map)
	MDRV_CPU_PERIODIC_INT(audio_interrupt, 480)

	MDRV_QUANTUM_TIME(HZ(6000)) // higher if sound stutters

	MDRV_GFXDECODE(rungun)

	MDRV_MACHINE_RESET(rng)
	MDRV_NVRAM_HANDLER(rungun)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_HAS_SHADOWS | VIDEO_HAS_HIGHLIGHTS | VIDEO_UPDATE_BEFORE_VBLANK)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(88, 88+384-1, 24, 24+224-1)

	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(rng)
	MDRV_VIDEO_UPDATE(rng)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("konami1", K054539, 48000)
	MDRV_SOUND_CONFIG(k054539_config)
	MDRV_SOUND_ROUTE(0, "lspeaker", 1.0)
	MDRV_SOUND_ROUTE(1, "rspeaker", 1.0)

	MDRV_SOUND_ADD("konami2", K054539, 48000)
	MDRV_SOUND_CONFIG(k054539_config)
	MDRV_SOUND_ROUTE(0, "lspeaker", 1.0)
	MDRV_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_DRIVER_END

static INPUT_PORTS_START( rng )
	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE4 )
	PORT_DIPNAME( 0x0100, 0x0000, "Freeze" )
	PORT_DIPSETTING( 0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x0100, DEF_STR( On ) )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(eeprom_bit_r, NULL)	/* EEPROM data */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SPECIAL )	/* EEPROM ready (always 1) */
	PORT_SERVICE_NO_TOGGLE( 0x08, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x10, 0x00, "Monitors" )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPNAME( 0x20, 0x00, "Number of players" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPNAME( 0x40, 0x00, "Sound Output" )
	PORT_DIPSETTING(    0x40, DEF_STR( Mono ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Stereo ) )
	PORT_DIPNAME( 0x04, 0x04, "Bit2 (Unknown)" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Bit7 (Unknown)" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("P1")
	KONAMI8_B123_START(1)

	PORT_START("P2")
	KONAMI8_B123_START(2)

	PORT_START("P3")
	KONAMI8_B123_START(3)

	PORT_START("P4")
	KONAMI8_B123_START(4)
INPUT_PORTS_END


ROM_START( rungun )
	/* main program Europe Version AA  1993, 10.8 */
	ROM_REGION( 0x300000, "maincpu", 0)
	ROM_LOAD16_BYTE( "247eaa03.bin", 0x000000, 0x80000, CRC(f5c91ec0) SHA1(298926ea30472fa8d2c0578dfeaf9a93509747ef) )
	ROM_LOAD16_BYTE( "247eaa04.bin", 0x000001, 0x80000, CRC(0e62471f) SHA1(2861b7a4e78ff371358d318a1b13a6488c0ac364) )

	/* data (Guru 1 megabyte redump) */
	ROM_LOAD16_BYTE( "247b01.23n", 0x200000, 0x80000, CRC(2d774f27) SHA1(c48de9cb9daba25603b8278e672f269807aa0b20) )
	ROM_CONTINUE(                  0x100000, 0x80000)
	ROM_LOAD16_BYTE( "247b02.21n", 0x200001, 0x80000, CRC(d088c9de) SHA1(19d7ad4120f7cfed9cae862bb0c799fdad7ab15c) )
	ROM_CONTINUE(                  0x100001, 0x80000)

	/* sound program */
	ROM_REGION( 0x030000, "soundcpu", 0 )
	ROM_LOAD("247a05",  0x000000, 0x20000, CRC(64e85430) SHA1(542919c3be257c8f118fc21d3835d7b6426a22ed) )
	ROM_RELOAD(         0x010000, 0x20000 )

	/* '936 tiles */
	ROM_REGION( 0x400000, "gfx1", 0)
	ROM_LOAD( "247-a13", 0x000000, 0x200000, CRC(cc194089) SHA1(b5af94f5f583d282ac1499b371bbaac8b2fedc03) )

	/* sprites */
	ROM_REGION( 0x800000, "gfx2", 0)
	ROM_LOAD64_WORD( "247-a11", 0x000000, 0x200000, CRC(c3f60854) SHA1(cbee7178ab9e5aa6a5aeed0511e370e29001fb01) )	// 5y
	ROM_LOAD64_WORD( "247-a08", 0x000002, 0x200000, CRC(3e315eef) SHA1(898bc4d5ad244e5f91cbc87820b5d0be99ef6662) )	// 2u
	ROM_LOAD64_WORD( "247-a09", 0x000004, 0x200000, CRC(5ca7bc06) SHA1(83c793c68227399f93bd1ed167dc9ed2aaac4167) )	// 2y
	ROM_LOAD64_WORD( "247-a10", 0x000006, 0x200000, CRC(a5ccd243) SHA1(860b88ade1a69f8b6c5b8206424814b386343571) )	// 5u

	/* TTL text plane ("fix layer") */
	ROM_REGION( 0x20000, "gfx3", 0)
	ROM_LOAD( "247-a12", 0x000000, 0x20000, CRC(57a8d26e) SHA1(0431d10b76d77c26a1f6f2b55d9dbcfa959e1cd0) )

	/* sound data */
	ROM_REGION( 0x400000, "shared", 0)
	ROM_LOAD( "247-a06", 0x000000, 0x200000, CRC(b8b2a67e) SHA1(a873d32f4b178c714743664fa53c0dca29cb3ce4) )
	ROM_LOAD( "247-a07", 0x200000, 0x200000, CRC(0108142d) SHA1(4dc6a36d976dad9c0da5a5b1f01f2eb3b369c99d) )
ROM_END

ROM_START( runguna )
	/* main program Europe Version AA 1993, 10.4 */
	ROM_REGION( 0x300000, "maincpu", 0)
	ROM_LOAD16_BYTE( "247eaa03.rom", 0x000000, 0x80000, CRC(fec3e1d6) SHA1(cd89dc32ad06308134d277f343a7e8b5fe381f69) )
	ROM_LOAD16_BYTE( "247eaa04.rom", 0x000001, 0x80000, CRC(1b556af9) SHA1(c8351ebd595307d561d089c66cd6ed7f6111d996) )

	/* data (Guru 1 megabyte redump) */
	ROM_LOAD16_BYTE( "247b01.23n", 0x200000, 0x80000, CRC(2d774f27) SHA1(c48de9cb9daba25603b8278e672f269807aa0b20) )
	ROM_CONTINUE(                  0x100000, 0x80000)
	ROM_LOAD16_BYTE( "247b02.21n", 0x200001, 0x80000, CRC(d088c9de) SHA1(19d7ad4120f7cfed9cae862bb0c799fdad7ab15c) )
	ROM_CONTINUE(                  0x100001, 0x80000)

	/* sound program */
	ROM_REGION( 0x030000, "soundcpu", 0 )
	ROM_LOAD("1.13g",  0x000000, 0x20000, CRC(c0b35df9) SHA1(a0c73d993eb32bd0cd192351b5f86794efd91949) )
	ROM_RELOAD(         0x010000, 0x20000 )

	/* '936 tiles */
	ROM_REGION( 0x400000, "gfx1", 0)
	ROM_LOAD( "247-a13", 0x000000, 0x200000, CRC(cc194089) SHA1(b5af94f5f583d282ac1499b371bbaac8b2fedc03) )

	/* sprites */
	ROM_REGION( 0x800000, "gfx2", 0)
	ROM_LOAD64_WORD( "247-a11", 0x000000, 0x200000, CRC(c3f60854) SHA1(cbee7178ab9e5aa6a5aeed0511e370e29001fb01) )	// 5y
	ROM_LOAD64_WORD( "247-a08", 0x000002, 0x200000, CRC(3e315eef) SHA1(898bc4d5ad244e5f91cbc87820b5d0be99ef6662) )	// 2u
	ROM_LOAD64_WORD( "247-a09", 0x000004, 0x200000, CRC(5ca7bc06) SHA1(83c793c68227399f93bd1ed167dc9ed2aaac4167) )	// 2y
	ROM_LOAD64_WORD( "247-a10", 0x000006, 0x200000, CRC(a5ccd243) SHA1(860b88ade1a69f8b6c5b8206424814b386343571) )	// 5u

	/* TTL text plane ("fix layer") */
	ROM_REGION( 0x20000, "gfx3", 0)
	ROM_LOAD( "247-a12", 0x000000, 0x20000, CRC(57a8d26e) SHA1(0431d10b76d77c26a1f6f2b55d9dbcfa959e1cd0) )

	/* sound data */
	ROM_REGION( 0x400000, "shared", 0)
	ROM_LOAD( "247-a06", 0x000000, 0x200000, CRC(b8b2a67e) SHA1(a873d32f4b178c714743664fa53c0dca29cb3ce4) )
	ROM_LOAD( "247-a07", 0x200000, 0x200000, CRC(0108142d) SHA1(4dc6a36d976dad9c0da5a5b1f01f2eb3b369c99d) )
ROM_END

ROM_START( rungunu )
	/* main program US Version AB 1993 10.12 */
	ROM_REGION( 0x300000, "maincpu", 0)
	ROM_LOAD16_BYTE( "247uab03.bin", 0x000000, 0x80000, CRC(f259fd11) SHA1(60381a3fa7f78022dcb3e2f3d13ea32a10e4e36e) )
	ROM_LOAD16_BYTE( "247uab04.bin", 0x000001, 0x80000, CRC(b918cf5a) SHA1(4314c611ef600ec081f409c78218de1639f8b463) )

	/* data */
	ROM_LOAD16_BYTE( "247a01", 0x100000, 0x80000, CRC(8341cf7d) SHA1(372c147c4a5d54aed2a16b0ed258247e65dda563) )
	ROM_LOAD16_BYTE( "247a02", 0x100001, 0x80000, CRC(f5ef3f45) SHA1(2e1d8f672c130dbfac4365dc1301b47beee10161) )

	/* sound program */
	ROM_REGION( 0x030000, "soundcpu", 0 )
	ROM_LOAD("247a05", 0x000000, 0x20000, CRC(64e85430) SHA1(542919c3be257c8f118fc21d3835d7b6426a22ed) )
	ROM_RELOAD(        0x010000, 0x20000 )

	/* '936 tiles */
	ROM_REGION( 0x400000, "gfx1", 0)
	ROM_LOAD( "247a13", 0x000000, 0x200000, CRC(c5a8ef29) SHA1(23938b8093bc0b9eef91f6d38127ca7acbdc06a6) )

	/* sprites */
	ROM_REGION( 0x800000, "gfx2", 0)
	ROM_LOAD64_WORD( "247-a11", 0x000000, 0x200000, CRC(c3f60854) SHA1(cbee7178ab9e5aa6a5aeed0511e370e29001fb01) )	// 5y
	ROM_LOAD64_WORD( "247-a08", 0x000002, 0x200000, CRC(3e315eef) SHA1(898bc4d5ad244e5f91cbc87820b5d0be99ef6662) )	// 2u
	ROM_LOAD64_WORD( "247-a09", 0x000004, 0x200000, CRC(5ca7bc06) SHA1(83c793c68227399f93bd1ed167dc9ed2aaac4167) )	// 2y
	ROM_LOAD64_WORD( "247-a10", 0x000006, 0x200000, CRC(a5ccd243) SHA1(860b88ade1a69f8b6c5b8206424814b386343571) )	// 5u

	/* TTL text plane ("fix layer") */
	ROM_REGION( 0x20000, "gfx3", 0)
	ROM_LOAD( "247-a12", 0x000000, 0x20000, CRC(57a8d26e) SHA1(0431d10b76d77c26a1f6f2b55d9dbcfa959e1cd0) )

	/* sound data */
	ROM_REGION( 0x400000, "shared", 0)
	ROM_LOAD( "247-a06", 0x000000, 0x200000, CRC(b8b2a67e) SHA1(a873d32f4b178c714743664fa53c0dca29cb3ce4) )
	ROM_LOAD( "247-a07", 0x200000, 0x200000, CRC(0108142d) SHA1(4dc6a36d976dad9c0da5a5b1f01f2eb3b369c99d) )
ROM_END

ROM_START( rungunua )
	/* main program US Version BA 1993 10.8 */
	ROM_REGION( 0x300000, "maincpu", 0)
	ROM_LOAD16_BYTE( "247uba03.bin", 0x000000, 0x80000, CRC(c24d7500) SHA1(38e6ae9fc00bf8f85549be4733992336c46fe1f3) )
	ROM_LOAD16_BYTE( "247uba04.bin", 0x000001, 0x80000, CRC(3f255a4a) SHA1(3a4d50ecec8546933ad8dabe21682ba0951eaad0) )

	/* data (Guru 1 megabyte redump) */
	ROM_LOAD16_BYTE( "247b01.23n", 0x200000, 0x80000, CRC(2d774f27) SHA1(c48de9cb9daba25603b8278e672f269807aa0b20) )
	ROM_CONTINUE(                  0x100000, 0x80000)
	ROM_LOAD16_BYTE( "247b02.21n", 0x200001, 0x80000, CRC(d088c9de) SHA1(19d7ad4120f7cfed9cae862bb0c799fdad7ab15c) )
	ROM_CONTINUE(                  0x100001, 0x80000)

	/* sound program */
	ROM_REGION( 0x030000, "soundcpu", 0 )
	ROM_LOAD("247a05", 0x000000, 0x20000, CRC(64e85430) SHA1(542919c3be257c8f118fc21d3835d7b6426a22ed) )
	ROM_RELOAD(        0x010000, 0x20000 )

	/* '936 tiles */
	ROM_REGION( 0x400000, "gfx1", 0)
	ROM_LOAD( "247a13", 0x000000, 0x200000, CRC(c5a8ef29) SHA1(23938b8093bc0b9eef91f6d38127ca7acbdc06a6) )

	/* sprites */
	ROM_REGION( 0x800000, "gfx2", 0)
	ROM_LOAD64_WORD( "247-a11", 0x000000, 0x200000, CRC(c3f60854) SHA1(cbee7178ab9e5aa6a5aeed0511e370e29001fb01) )	// 5y
	ROM_LOAD64_WORD( "247-a08", 0x000002, 0x200000, CRC(3e315eef) SHA1(898bc4d5ad244e5f91cbc87820b5d0be99ef6662) )	// 2u
	ROM_LOAD64_WORD( "247-a09", 0x000004, 0x200000, CRC(5ca7bc06) SHA1(83c793c68227399f93bd1ed167dc9ed2aaac4167) )	// 2y
	ROM_LOAD64_WORD( "247-a10", 0x000006, 0x200000, CRC(a5ccd243) SHA1(860b88ade1a69f8b6c5b8206424814b386343571) )	// 5u

	/* TTL text plane ("fix layer") */
	ROM_REGION( 0x20000, "gfx3", 0)
	ROM_LOAD( "247-a12", 0x000000, 0x20000, CRC(57a8d26e) SHA1(0431d10b76d77c26a1f6f2b55d9dbcfa959e1cd0) )

	/* sound data */
	ROM_REGION( 0x400000, "shared", 0)
	ROM_LOAD( "247-a06", 0x000000, 0x200000, CRC(b8b2a67e) SHA1(a873d32f4b178c714743664fa53c0dca29cb3ce4) )
	ROM_LOAD( "247-a07", 0x200000, 0x200000, CRC(0108142d) SHA1(4dc6a36d976dad9c0da5a5b1f01f2eb3b369c99d) )
ROM_END

ROM_START( slmdunkj )
	/* main program Japan Version AA 1993 10.8 */
	ROM_REGION( 0x300000, "maincpu", 0)
	ROM_LOAD16_BYTE( "247jaa03.bin", 0x000000, 0x20000, CRC(87572078) SHA1(cfa784eb40ed8b3bda9d57abb6022bbe92056206) )
	ROM_LOAD16_BYTE( "247jaa04.bin", 0x000001, 0x20000, CRC(aa105e00) SHA1(617ac14535048b6e0da43cc98c4b67c8e306bef1) )

	/* data (Guru 1 megabyte redump) */
	ROM_LOAD16_BYTE( "247b01.23n", 0x200000, 0x80000, CRC(2d774f27) SHA1(c48de9cb9daba25603b8278e672f269807aa0b20) )
	ROM_CONTINUE(                  0x100000, 0x80000)
	ROM_LOAD16_BYTE( "247b02.21n", 0x200001, 0x80000, CRC(d088c9de) SHA1(19d7ad4120f7cfed9cae862bb0c799fdad7ab15c) )
	ROM_CONTINUE(                  0x100001, 0x80000)

	/* sound program */
	ROM_REGION( 0x030000, "soundcpu", 0 )
	ROM_LOAD("247a05",  0x000000, 0x20000, CRC(64e85430) SHA1(542919c3be257c8f118fc21d3835d7b6426a22ed) )
	ROM_RELOAD(         0x010000, 0x20000 )

	/* '936 tiles */
	ROM_REGION( 0x400000, "gfx1", 0)
	ROM_LOAD( "247-a13", 0x000000, 0x200000, CRC(cc194089) SHA1(b5af94f5f583d282ac1499b371bbaac8b2fedc03) )

	/* sprites */
	ROM_REGION( 0x800000, "gfx2", 0)
	ROM_LOAD64_WORD( "247-a11", 0x000000, 0x200000, CRC(c3f60854) SHA1(cbee7178ab9e5aa6a5aeed0511e370e29001fb01) )	// 5y
	ROM_LOAD64_WORD( "247-a08", 0x000002, 0x200000, CRC(3e315eef) SHA1(898bc4d5ad244e5f91cbc87820b5d0be99ef6662) )	// 2u
	ROM_LOAD64_WORD( "247-a09", 0x000004, 0x200000, CRC(5ca7bc06) SHA1(83c793c68227399f93bd1ed167dc9ed2aaac4167) )	// 2y
	ROM_LOAD64_WORD( "247-a10", 0x000006, 0x200000, CRC(a5ccd243) SHA1(860b88ade1a69f8b6c5b8206424814b386343571) )	// 5u

	/* TTL text plane ("fix layer") */
	ROM_REGION( 0x20000, "gfx3", 0)
	ROM_LOAD( "247-a12", 0x000000, 0x20000, CRC(57a8d26e) SHA1(0431d10b76d77c26a1f6f2b55d9dbcfa959e1cd0) )

	/* sound data */
	ROM_REGION( 0x400000, "shared", 0)
	ROM_LOAD( "247-a06", 0x000000, 0x200000, CRC(b8b2a67e) SHA1(a873d32f4b178c714743664fa53c0dca29cb3ce4) )
	ROM_LOAD( "247-a07", 0x200000, 0x200000, CRC(0108142d) SHA1(4dc6a36d976dad9c0da5a5b1f01f2eb3b369c99d) )
ROM_END

static DRIVER_INIT( rng )
{
}

static MACHINE_RESET( rng )
{
	k054539_init_flags(devtag_get_device(machine, "konami1"), K054539_REVERSE_STEREO);

	memset(rng_sysreg, 0, 0x20);

	init_eeprom_count = 0;
	rng_z80_control = 0;
	rng_sound_status = 0;
}
#endif

//GAME( 1993, rungun,   0,      rng, rng, rng, ROT0, "Konami", "Run and Gun (ver EAA 1993 10.8)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_SOUND )
//GAME( 1993, runguna,  rungun, rng, rng, rng, ROT0, "Konami", "Run and Gun (ver EAA 1993 10.4)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_SOUND )
//GAME( 1993, rungunu,  rungun, rng, rng, rng, ROT0, "Konami", "Run and Gun (ver UAB 1993 10.12)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_SOUND )
//GAME( 1993, rungunua, rungun, rng, rng, rng, ROT0, "Konami", "Run and Gun (ver UBA 1993 10.8)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_SOUND )
//GAME( 1993, slmdunkj, rungun, rng, rng, rng, ROT0, "Konami", "Slam Dunk (ver JAA 1993 10.8)", GAME_IMPERFECT_GRAPHICS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_SOUND )
