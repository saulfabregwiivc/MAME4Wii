/***************************************************************************

Phoenix hardware games

driver by Richard Davies

Note:
    The discrete sound part of Phoenix requires a sample rate of at least 44100.
    This is because the frequencies are so high, that some sounds will be
    missed at a lower sample rate.

    pleiads is using another sound driver, audio\pleiads.c
 Andrew Scott (ascott@utkux.utcc.utk.edu)


To Do:


Survival:

- Check background visibile area.  When the background scrolls up, it
  currently shows below the top and bottom of the border of the play area.


Pleiads:

- Palette banking.  Controlled by 3 custom chips marked T-X, T-Y and T-Z.
  These chips are reponsible for the protection as well.

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/tms36xx.h"
#include "cpu/i8085/i8085.h"
#include "sound/ay8910.h"
#include "phoenix.h"


static ADDRESS_MAP_START( phoenix_memory_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x4fff) AM_READWRITE(SMH_BANK(1), phoenix_videoram_w)	/* 2 pages selected by bit 0 of the video register */
	AM_RANGE(0x5000, 0x53ff) AM_WRITE(phoenix_videoreg_w)
	AM_RANGE(0x5800, 0x5bff) AM_WRITE(phoenix_scroll_w)
	AM_RANGE(0x6000, 0x63ff) AM_DEVWRITE("discrete", phoenix_sound_control_a_w)
	AM_RANGE(0x6800, 0x6bff) AM_DEVWRITE("discrete", phoenix_sound_control_b_w)
	AM_RANGE(0x7000, 0x73ff) AM_READ_PORT("IN0")	 						/* IN0 or IN1 */
	AM_RANGE(0x7800, 0x7bff) AM_READ_PORT("DSW0")	 						/* DSW */
ADDRESS_MAP_END

static ADDRESS_MAP_START( pleiads_memory_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x4fff) AM_READWRITE(SMH_BANK(1), phoenix_videoram_w)	/* 2 pages selected by bit 0 of the video register */
	AM_RANGE(0x5000, 0x53ff) AM_WRITE(pleiads_videoreg_w)
	AM_RANGE(0x5800, 0x5bff) AM_WRITE(phoenix_scroll_w)
	AM_RANGE(0x6000, 0x63ff) AM_WRITE(pleiads_sound_control_a_w)
	AM_RANGE(0x6800, 0x6bff) AM_WRITE(pleiads_sound_control_b_w)
	AM_RANGE(0x7000, 0x73ff) AM_READ_PORT("IN0")	 						/* IN0 or IN1 + protection */
	AM_RANGE(0x7800, 0x7bff) AM_READ_PORT("DSW0") 							/* DSW */
ADDRESS_MAP_END

static ADDRESS_MAP_START( survival_memory_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x4fff) AM_READWRITE(SMH_BANK(1), phoenix_videoram_w)	/* 2 pages selected by bit 0 of the video register */
	AM_RANGE(0x5000, 0x53ff) AM_WRITE(phoenix_videoreg_w)
	AM_RANGE(0x5800, 0x5bff) AM_WRITE(phoenix_scroll_w)
	AM_RANGE(0x6800, 0x68ff) AM_DEVWRITE("ay", ay8910_address_w)
	AM_RANGE(0x6900, 0x69ff) AM_DEVREADWRITE("ay", ay8910_r, ay8910_data_w)
	AM_RANGE(0x7000, 0x73ff) AM_READ(survival_input_port_0_r)				/* IN0 or IN1 */
	AM_RANGE(0x7800, 0x7bff) AM_READ_PORT("DSW0") 							/* DSW */
ADDRESS_MAP_END



static INPUT_PORTS_START( phoenix )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(player_input_r, NULL)

	PORT_START("DSW0")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )            PORT_DIPLOCATION( "SW1:1,2" )
	PORT_DIPSETTING(	0x00, "3" )
	PORT_DIPSETTING(	0x01, "4" )
	PORT_DIPSETTING(	0x02, "5" )
	PORT_DIPSETTING(	0x03, "6" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION( "SW1:3,4" )
	PORT_DIPSETTING(	0x00, "3K 30K" )
	PORT_DIPSETTING(	0x04, "4K 40K" )
	PORT_DIPSETTING(	0x08, "5K 50K" )
	PORT_DIPSETTING(	0x0c, "6K 60K" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Coinage ) )          PORT_DIPLOCATION( "SW1:5" )
	PORT_DIPSETTING(	0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:6" )
	PORT_DIPSETTING(	0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:7" )
	PORT_DIPSETTING(	0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

	PORT_START("CAB")		/* fake port for non-memory mapped dip switch */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Cabinet ) )          PORT_DIPLOCATION( "SW1:!8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Cocktail ) )

	PORT_START("CTRL")		/* fake port for multiplexed controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_2WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL                  PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_COCKTAIL PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_2WAY PORT_COCKTAIL PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL                  PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( phoenixa )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("DSW0")
	/* Coinage is backwards from phoenix (Amstar) */
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Coinage ) )          PORT_DIPLOCATION( "SW1:5" )
	PORT_DIPSETTING(	0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x10, DEF_STR( 1C_1C ) )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( phoenixt )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:5" )
	PORT_DIPSETTING(	0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( phoenix3 )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:5" )
	PORT_DIPSETTING(	0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:6" )
	PORT_DIPSETTING(	0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Coinage ) )          PORT_DIPLOCATION( "SW1:7" )
	PORT_DIPSETTING(	0x40, "A - 2C/1C B - 1C/3C" )
	PORT_DIPSETTING(	0x00, "A - 1C/1C B - 1C/6C" )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( condor )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(player_input_r, NULL)

	PORT_START("DSW0")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )            PORT_DIPLOCATION( "SW1:1,2" )
	PORT_DIPSETTING(	0x00, "2" )
	PORT_DIPSETTING(	0x01, "3" )
	PORT_DIPSETTING(	0x02, "4" )
	PORT_DIPSETTING(	0x03, "5" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION( "SW1:3,4" )
	PORT_DIPSETTING(	0x00, "Every 6000" )
	PORT_DIPSETTING(	0x08, "Every 10000" )
	PORT_DIPSETTING(	0x04, "Every 14000" )
	PORT_DIPSETTING(	0x0c, "Every 18000" )
	PORT_DIPNAME( 0x70, 0x30, "Fuel Consumption" )          PORT_DIPLOCATION( "SW1:5,6,7" )
	PORT_DIPSETTING(	0x00, "Slowest" )
	PORT_DIPSETTING(	0x10, "Slower" )
	PORT_DIPSETTING(	0x20, "Slow" )
	PORT_DIPSETTING(	0x30, "Bit Slow" )
	PORT_DIPSETTING(	0x40, "Bit Fast" )
	PORT_DIPSETTING(	0x50, "Fast" )
	PORT_DIPSETTING(	0x60, "Faster" )
	PORT_DIPSETTING(	0x70, "Fastest" )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x00, DEF_STR( Coin_B ) )           PORT_DIPLOCATION( "SW2:1,2,3,4" )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_8C ) )
	PORT_DIPNAME( 0xf0, 0x00, DEF_STR( Coin_A ) )           PORT_DIPLOCATION( "SW2:5,6,7,8" )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 2C_4C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 2C_6C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_7C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 2C_8C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_8C ) )

	PORT_START("CAB")		/* fake port for non-memory mapped dip switch */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Cabinet ) )          PORT_DIPLOCATION( "SW1:!8" )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Cocktail ) )

	PORT_START("CTRL")		/* fake port for multiplexed controls */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_2WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  ) PORT_2WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL                  PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_COCKTAIL PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  ) PORT_2WAY PORT_COCKTAIL PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_COCKTAIL                  PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( falconz )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )            PORT_DIPLOCATION( "SW1:1,2" )
	PORT_DIPSETTING(	0x00, "2" )
	PORT_DIPSETTING(	0x01, "3" )
	PORT_DIPSETTING(	0x02, "4" )
	PORT_DIPSETTING(	0x03, "5" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION( "SW1:3,4" )
	PORT_DIPSETTING(	0x00, "3K 30K" )
	PORT_DIPSETTING(	0x04, "4K 40K" )
	PORT_DIPSETTING(	0x08, "5K 50K" )
	PORT_DIPSETTING(	0x0c, "6K 60K" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:5" )
	PORT_DIPSETTING(	0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:6" )
	PORT_DIPSETTING(	0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Coinage ) )          PORT_DIPLOCATION( "SW1:7" )
	PORT_DIPSETTING(	0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 1C_1C ) )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( nextfase )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )            PORT_DIPLOCATION( "SW1:1,2" )
	PORT_DIPSETTING(	0x00, "1" )
	PORT_DIPSETTING(	0x01, "2" )
	PORT_DIPSETTING(	0x02, "3" )
	PORT_DIPSETTING(	0x03, "4" )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION( "SW1:3,4" )
	PORT_DIPSETTING(	0x00, "3K 30K" )
	PORT_DIPSETTING(	0x04, "4K 40K" )
	PORT_DIPSETTING(	0x08, "5K 50K" )
	PORT_DIPSETTING(	0x0c, "6K 60K" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )          PORT_DIPLOCATION( "SW1:5" )
	PORT_DIPSETTING(	0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x60, 0x00, DEF_STR( Coinage ) )          PORT_DIPLOCATION( "SW1:6,7" )
	PORT_DIPSETTING(	0x00, "A - 1C/1C B - 1C/2C" )
	PORT_DIPSETTING(	0x20, "A - 2C/3C B - 1C/3C" )
	PORT_DIPSETTING(	0x40, "A - 1C/2C B - 1C/4C" )
	PORT_DIPSETTING(	0x60, "A - 2C/5C B - 1C/5C" )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( pleiads )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM(pleiads_protection_r, NULL)     /* Protection. See 0x0552 */

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )      PORT_DIPLOCATION( "SW1:7" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x40, DEF_STR( On ) )

	/* Based on various sources, no Button 2 was present in Pleiads */
	PORT_MODIFY("CTRL")		/* fake port for multiplexed controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_2WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL					PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_COCKTAIL PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_2WAY PORT_COCKTAIL	PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )									PORT_CONDITION("CAB",0x01,PORTCOND_EQUALS,0x01)
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( pleiadbl )
	PORT_INCLUDE( phoenix )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM(pleiads_protection_r, NULL)     /* Protection. See 0x0552 */

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )      PORT_DIPLOCATION( "SW1:7" )
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x40, DEF_STR( On ) )
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( pleiadce )
	PORT_INCLUDE( pleiadbl )

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION( "SW1:3,4" )
	PORT_DIPSETTING(	0x00, "7K 70K" )
	PORT_DIPSETTING(	0x04, "8K 80K" )
	PORT_DIPSETTING(	0x08, "9K 90K" )
  /*PORT_DIPSETTING(    0x0c, "INVALID" )                   Sets bonus to A000 */
INPUT_PORTS_END
#endif
#if 0
static INPUT_PORTS_START( capitol )
	PORT_INCLUDE( pleiads )

	/* Capitol has no Button 2 as Pleiads, but there is no protection */
	PORT_MODIFY("IN0")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( survival )
	PORT_START("IN0")      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP  )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT    )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  )

	PORT_START("IN1")		/* IN1 */
	PORT_BIT( 0x07, IP_ACTIVE_LOW, IPT_SPECIAL )	/* comes from IN0 0-2 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN     ) PORT_COCKTAIL

    PORT_START("DSW0")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(	0x03, "2" )
	PORT_DIPSETTING(	0x02, "3" )
	PORT_DIPSETTING(	0x01, "4" )
	PORT_DIPSETTING(	0x00, "5" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(	0x0c, "25000" )
	PORT_DIPSETTING(	0x08, "35000" )
	PORT_DIPSETTING(	0x04, "45000" )
	PORT_DIPSETTING(	0x00, "55000" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(	0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Coinage ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(	0x20, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	0x40, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x60, DEF_STR( 1C_1C ) )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_VBLANK )

	PORT_START("CAB")		/* fake port for non-memory mapped dip switch */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Cocktail ) )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	256,	/* 256 characters */
	2,	/* 2 bits per pixel */
	{ 256*8*8, 0 }, /* the two bitplanes are separated */
	{ 7, 6, 5, 4, 3, 2, 1, 0 }, /* pretty straightforward layout */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8 /* every char takes 8 consecutive bytes */
};

static GFXDECODE_START( phoenix )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 0, 32 )
	GFXDECODE_ENTRY( "gfx2", 0, charlayout, 0, 32 )
GFXDECODE_END

static GFXDECODE_START( pleiads )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 0, 64 )
	GFXDECODE_ENTRY( "gfx2", 0, charlayout, 0, 64 )
GFXDECODE_END


static const tms36xx_interface phoenix_tms36xx_interface =
{
	MM6221AA,	/* TMS36xx subtype(s) */
	{0.50,0,0,1.05,0,0}, /* decay times of voices */
    0.21       /* tune speed (time between beats) */
};

static const tms36xx_interface pleiads_tms36xx_interface =
{
	TMS3615,	/* TMS36xx subtype(s) */
	/*
     * Decay times of the voices; NOTE: it's unknown if
     * the the TMS3615 mixes more than one voice internally.
     * A wav taken from Pop Flamer sounds like there
     * are at least no 'odd' harmonics (5 1/3' and 2 2/3')
     */
	{0.33,0.33,0,0.33,0,0.33}
};

static const ay8910_interface survival_ay8910_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_NULL,
	DEVCB_HANDLER(survival_protection_r),
	DEVCB_NULL,
	DEVCB_NULL
};



static MACHINE_RESET( phoenix )
{
	memory_set_bankptr(machine, 1, memory_region(machine, "maincpu") + 0x4000);
}


static MACHINE_DRIVER_START( phoenix )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", 8085A, CPU_CLOCK)	/* 2.75 MHz */
	MDRV_CPU_PROGRAM_MAP(phoenix_memory_map)

	MDRV_MACHINE_RESET(phoenix)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(PIXEL_CLOCK, HTOTAL, HBEND, HBSTART, VTOTAL, VBEND, VBSTART)

	MDRV_GFXDECODE(phoenix)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(phoenix)
	MDRV_VIDEO_START(phoenix)
	MDRV_VIDEO_UPDATE(phoenix)

	MDRV_SOUND_START(phoenix)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("tms",  TMS36XX, 372)
	MDRV_SOUND_CONFIG(phoenix_tms36xx_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.5)

	MDRV_SOUND_ADD("cust", PHOENIX, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.4)

	MDRV_SOUND_ADD("discrete", DISCRETE, 120000)
	MDRV_SOUND_CONFIG_DISCRETE(phoenix)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.6)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( pleiads )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(phoenix)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(pleiads_memory_map)

	/* video hardware */
	MDRV_GFXDECODE(pleiads)

	MDRV_PALETTE_INIT(pleiads)

	/* sound hardware */
	MDRV_SOUND_REPLACE("tms", TMS36XX, 247)
	MDRV_SOUND_CONFIG(pleiads_tms36xx_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	MDRV_SOUND_REPLACE("cust", PLEIADS, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)

	MDRV_DEVICE_REMOVE("discrete")
MACHINE_DRIVER_END


/* Same as Phoenix, but uses an AY8910 and an extra visible line (column) */

static I8085_CONFIG( survival_i8085_config )
{
	DEVCB_NULL,							/* STATUS changed callback */
	DEVCB_NULL,							/* INTE changed callback */
	DEVCB_LINE(survival_sid_callback),	/* SID changed callback (8085A only) */
	DEVCB_NULL							/* SOD changed callback (8085A only) */
};

static MACHINE_DRIVER_START( survival )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", 8085A, CPU_CLOCK)	/* 5.50 MHz */
	MDRV_CPU_CONFIG(survival_i8085_config)
	MDRV_CPU_PROGRAM_MAP(survival_memory_map)

	MDRV_MACHINE_RESET(phoenix)

	/* video hardware */

	/* schematics fairly identical to phoenix, however the interesting
     * page is missing
     */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(PIXEL_CLOCK, HTOTAL, HBEND, HBSTART, VTOTAL, VBEND, VBSTART)

	MDRV_GFXDECODE(phoenix)
	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(survival)
	MDRV_VIDEO_START(phoenix)
	MDRV_VIDEO_UPDATE(phoenix)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	/* FIXME: check clock */
	MDRV_SOUND_ADD("ay", AY8910, 11000000/4)
	MDRV_SOUND_CONFIG(survival_ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


/* Uses a Z80 */
#if 0
static MACHINE_DRIVER_START( condor )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(phoenix)
	/* FIXME: Verify clock. This is most likely 11MHz/2 */
	MDRV_CPU_REPLACE("maincpu", Z80, 11000000/4)	/* 2.75 MHz??? */
MACHINE_DRIVER_END
#endif

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( phoenix )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ic45",         0x0000, 0x0800, CRC(9f68086b) SHA1(fc3cef299bf03bf0586c4047c6b96ca666846220) )
	ROM_LOAD( "ic46",         0x0800, 0x0800, CRC(273a4a82) SHA1(6f3019a074e73ff50ceb92f655fcf15659f34919) )
	ROM_LOAD( "ic47",         0x1000, 0x0800, CRC(3d4284b9) SHA1(6e69f8f0d537fe89140cd95d2398531d7e93d102) )
	ROM_LOAD( "ic48",         0x1800, 0x0800, CRC(cb5d9915) SHA1(49bcf55a5721cfcc02c3b811a4b601e35ea576db) )
	ROM_LOAD( "h5-ic49.5a",   0x2000, 0x0800, CRC(a105e4e7) SHA1(b35142a91b6b7fdf7535202671793393c9f4685f) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "h7-ic51.7a",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "h8-ic52.8a",   0x3800, 0x0800, CRC(aff8e9c5) SHA1(e4164f85ec12d4d9bcbffba27ab1f51b3599f6d0) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "b1-ic39.3b",   0x0000, 0x0800, CRC(53413e8f) SHA1(d772358505b973b10da840d204afb210c0c746ec) )
	ROM_LOAD( "b2-ic40.4b",   0x0800, 0x0800, CRC(0be2ba91) SHA1(af9243ee23377b632b9b7d0b84d341d06bf22480) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END

#if 0
ROM_START( phoenixa )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "1-ic45.1a",    0x0000, 0x0800, CRC(c7a9b499) SHA1(cda61de47956b3603ff6e48556ce528b5f45deab) )
	ROM_LOAD( "2-ic46.2a",    0x0800, 0x0800, CRC(d0e6ae1b) SHA1(63c6df8365dcb8befa338e8479482e34a4259abf) )
	ROM_LOAD( "3-ic47.3a",    0x1000, 0x0800, CRC(64bf463a) SHA1(6cd876e80b85fbac6374ea1f26620c026ba9e99a) )
	ROM_LOAD( "4-ic48.4a",    0x1800, 0x0800, CRC(1b20fe62) SHA1(87d2da6b9bde9049825245ca4b994fc84543e8b9) )
	ROM_LOAD( "h5-ic49.5a",   0x2000, 0x0800, CRC(a105e4e7) SHA1(b35142a91b6b7fdf7535202671793393c9f4685f) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "h7-ic51.7a",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "h8-ic52.8a",   0x3800, 0x0800, CRC(aff8e9c5) SHA1(e4164f85ec12d4d9bcbffba27ab1f51b3599f6d0) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "b1-ic39.3b",   0x0000, 0x0800, CRC(53413e8f) SHA1(d772358505b973b10da840d204afb210c0c746ec) )
	ROM_LOAD( "b2-ic40.4b",   0x0800, 0x0800, CRC(0be2ba91) SHA1(af9243ee23377b632b9b7d0b84d341d06bf22480) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( phoenixb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "1-ic45.1a",    0x0000, 0x0800, CRC(c7a9b499) SHA1(cda61de47956b3603ff6e48556ce528b5f45deab) )
	ROM_LOAD( "2-ic46.2a",    0x0800, 0x0800, CRC(d0e6ae1b) SHA1(63c6df8365dcb8befa338e8479482e34a4259abf) )
	ROM_LOAD( "3-ic47.3a",    0x1000, 0x0800, CRC(64bf463a) SHA1(6cd876e80b85fbac6374ea1f26620c026ba9e99a) )
	ROM_LOAD( "4-ic48.4a",    0x1800, 0x0800, CRC(1b20fe62) SHA1(87d2da6b9bde9049825245ca4b994fc84543e8b9) )
	ROM_LOAD( "phoenixc.49",  0x2000, 0x0800, CRC(1a1ce0d0) SHA1(c2825eef5d461e16ca2172daff94b3751be2f4dc) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "h7-ic51.7a",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "h8-ic52.8a",   0x3800, 0x0800, CRC(aff8e9c5) SHA1(e4164f85ec12d4d9bcbffba27ab1f51b3599f6d0) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "phoenixc.39",  0x0000, 0x0800, CRC(bb0525ed) SHA1(86db1c7584fb3846bfd47535e1585eeb7fbbb1fe) )
	ROM_LOAD( "phoenixc.40",  0x0800, 0x0800, CRC(4178aa4f) SHA1(5350f8f62cc7c223c38008bc83140b7a19147d81) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( phoenixt )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "phoenix.45",   0x0000, 0x0800, CRC(5b8c55a8) SHA1(839c1ca9766f730ec3accd48db70f6429a9c3362) )
	ROM_LOAD( "phoenix.46",   0x0800, 0x0800, CRC(dbc942fa) SHA1(9fe224e6ced407289dfa571468259a021d942b7d) )
	ROM_LOAD( "phoenix.47",   0x1000, 0x0800, CRC(cbbb8839) SHA1(b7f449374cac111081559e39646f973e7e99fd64) )
	ROM_LOAD( "phoenix.48",   0x1800, 0x0800, CRC(cb65eff8) SHA1(63e674d680972d3744d66b943e8546f3b77ee6d4) )
	ROM_LOAD( "phoenix.49",   0x2000, 0x0800, CRC(c8a5d6d6) SHA1(ef6ade323544e3edd4101609138ecf35e8cb9577) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "h7-ic51.7a",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "phoenix.52",   0x3800, 0x0800, CRC(b9915263) SHA1(f61396077b23364b5b26f62c6923394d23a37eb3) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "b1-ic39.3b",   0x0000, 0x0800, CRC(53413e8f) SHA1(d772358505b973b10da840d204afb210c0c746ec) )
	ROM_LOAD( "b2-ic40.4b",   0x0800, 0x0800, CRC(0be2ba91) SHA1(af9243ee23377b632b9b7d0b84d341d06bf22480) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( phoenixj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "pn01.45",   0x0000, 0x0800, CRC(5b8c55a8) SHA1(839c1ca9766f730ec3accd48db70f6429a9c3362) )
	ROM_LOAD( "pn02.46",   0x0800, 0x0800, CRC(dbc942fa) SHA1(9fe224e6ced407289dfa571468259a021d942b7d) )
	ROM_LOAD( "pn03.47",   0x1000, 0x0800, CRC(cbbb8839) SHA1(b7f449374cac111081559e39646f973e7e99fd64) )
	ROM_LOAD( "pn04.48",   0x1800, 0x0800, CRC(dd41f22b) SHA1(cb3748e18a99e35d99b5f18cee2a9796dd5646f9) )
	ROM_LOAD( "pn05.49",   0x2000, 0x0800, CRC(1a1ce0d0) SHA1(c2825eef5d461e16ca2172daff94b3751be2f4dc) )
	ROM_LOAD( "pn06.50",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "pn07.51",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "pn08.52",   0x3800, 0x0800, CRC(b9915263) SHA1(f61396077b23364b5b26f62c6923394d23a37eb3) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "pn11.23",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "pn12.24",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "pn09.39",   0x0000, 0x0800, CRC(53413e8f) SHA1(d772358505b973b10da840d204afb210c0c746ec) )
	ROM_LOAD( "pn10.40",   0x0800, 0x0800, CRC(0be2ba91) SHA1(af9243ee23377b632b9b7d0b84d341d06bf22480) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "pn14.40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "pn13.41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( phoenix3 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "phoenix3.45",  0x0000, 0x0800, CRC(a362cda0) SHA1(5ab38afaf92179c965533326574c773f6a63dbbb) )
	ROM_LOAD( "phoenix3.46",  0x0800, 0x0800, CRC(5748f486) SHA1(49e6fd836d26ec24105e95227b24cf668e8a470a) )
	ROM_LOAD( "phoenix.47",   0x1000, 0x0800, CRC(cbbb8839) SHA1(b7f449374cac111081559e39646f973e7e99fd64) )
	ROM_LOAD( "phoenix3.48",  0x1800, 0x0800, CRC(b5d97a4d) SHA1(d5d92c5e34431b2ded47e58608c459cc8cdd7937) )
	ROM_LOAD( "h5-ic49.5a",   0x2000, 0x0800, CRC(a105e4e7) SHA1(b35142a91b6b7fdf7535202671793393c9f4685f) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "h7-ic51.7a",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "phoenix3.52",  0x3800, 0x0800, CRC(d2c5c984) SHA1(a9432f9aff8a2f5ca1d347443efc008a177d8ae0) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "b1-ic39.3b",   0x0000, 0x0800, CRC(53413e8f) SHA1(d772358505b973b10da840d204afb210c0c746ec) )
	ROM_LOAD( "b2-ic40.4b",   0x0800, 0x0800, CRC(0be2ba91) SHA1(af9243ee23377b632b9b7d0b84d341d06bf22480) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( phoenixc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "phoenix.45",   0x0000, 0x0800, CRC(5b8c55a8) SHA1(839c1ca9766f730ec3accd48db70f6429a9c3362) )
	ROM_LOAD( "phoenix.46",   0x0800, 0x0800, CRC(dbc942fa) SHA1(9fe224e6ced407289dfa571468259a021d942b7d) )
	ROM_LOAD( "phoenix.47",   0x1000, 0x0800, CRC(cbbb8839) SHA1(b7f449374cac111081559e39646f973e7e99fd64) )
	ROM_LOAD( "phoenixc.48",  0x1800, 0x0800, CRC(5ae0b215) SHA1(f6dd86806fb9c467aaa63edf0cb4dbed9645e7c0) )
	ROM_LOAD( "phoenixc.49",  0x2000, 0x0800, CRC(1a1ce0d0) SHA1(c2825eef5d461e16ca2172daff94b3751be2f4dc) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "h7-ic51.7a",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "phoenixc.52",  0x3800, 0x0800, CRC(8424d7c4) SHA1(1b5fa7d8be9e8750a4148dfefc17e96c86ed084d) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "phoenixc.39",  0x0000, 0x0800, CRC(bb0525ed) SHA1(86db1c7584fb3846bfd47535e1585eeb7fbbb1fe) )
	ROM_LOAD( "phoenixc.40",  0x0800, 0x0800, CRC(4178aa4f) SHA1(5350f8f62cc7c223c38008bc83140b7a19147d81) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( condor )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cond01c.bin",  0x0000, 0x0800, CRC(c0f73929) SHA1(3cecf8341a5674165d2cae9b22ea5db26a9597de) )
	ROM_LOAD( "cond02c.bin",  0x0800, 0x0800, CRC(440d56e8) SHA1(b3147d5416cec8c00c7df40b878b826434121737) )
	ROM_LOAD( "cond03c.bin",  0x1000, 0x0800, CRC(750b059b) SHA1(6fbaa2ef4c7eef6f731a73b2d33a02fff21b318a) )
	ROM_LOAD( "cond04c.bin",  0x1800, 0x0800, CRC(ca55e1dd) SHA1(f3d8de56e54ec8651ab95af90ed122096c076c65) )
	ROM_LOAD( "cond05c.bin",  0x2000, 0x0800, CRC(1ff3a982) SHA1(66fb39e7abdf7a9c6e2eb01d41cfe9429781d6aa) )
	ROM_LOAD( "cond06c.bin",  0x2800, 0x0800, CRC(8c83bff7) SHA1(3dfb090d7e3a9ae8da882b06e166c48555eaf77c) )
	ROM_LOAD( "cond07c.bin",  0x3000, 0x0800, CRC(805ec2e8) SHA1(7e56fc9990eb99512078e2b1e2874fb33b0aa05c) )
	ROM_LOAD( "cond08c.bin",  0x3800, 0x0800, CRC(1edebb45) SHA1(2fdf061ee600e27a6ed512ea61a8d78307a7fb8a) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "cond09c.bin",  0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "cond10c.bin",  0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "cond11c.bin",  0x0000, 0x0800, CRC(53c52eb0) SHA1(19624ca359996b77d3c65ef78a7af90eeb092377) )
	ROM_LOAD( "cond12c.bin",  0x0800, 0x0800, CRC(eba42f0f) SHA1(378282cb2c4e10c23179ae3c605ae7bf691150f6) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( falcon )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "falcon.45",    0x0000, 0x0800, CRC(80382b6c) SHA1(47e24f04b5dd8aa8258ce324a0e4ef68a75dc168) )
	ROM_LOAD( "falcon.46",    0x0800, 0x0800, CRC(6a13193b) SHA1(760347695f1abc92cfe19ea7085e5aaf2dced383) )
	ROM_LOAD( "phoenix.47",   0x1000, 0x0800, CRC(cbbb8839) SHA1(b7f449374cac111081559e39646f973e7e99fd64) )
	ROM_LOAD( "falcon.48",    0x1800, 0x0800, CRC(084e9766) SHA1(844b83041c3cf60c51a045029624296f205847ab) )
	ROM_LOAD( "phoenixc.49",  0x2000, 0x0800, CRC(1a1ce0d0) SHA1(c2825eef5d461e16ca2172daff94b3751be2f4dc) )
	ROM_LOAD( "h6-ic50.6a",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "falcon.51",    0x3000, 0x0800, CRC(6e82e400) SHA1(22e97f74ca7010bba4263ea844cb7b2c6da09ab7) )
	ROM_LOAD( "h8-ic52.8a",   0x3800, 0x0800, CRC(aff8e9c5) SHA1(e4164f85ec12d4d9bcbffba27ab1f51b3599f6d0) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "b1-ic39.3b",   0x0000, 0x0800, CRC(53413e8f) SHA1(d772358505b973b10da840d204afb210c0c746ec) )
	ROM_LOAD( "b2-ic40.4b",   0x0800, 0x0800, CRC(0be2ba91) SHA1(af9243ee23377b632b9b7d0b84d341d06bf22480) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( vautour )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "vautor01.1e",  0x0000, 0x0800, CRC(cd2807ee) SHA1(79b9769f212d25b9ccb5124e2aa632c964c14a0b) )
	ROM_LOAD( "phoenix.46",   0x0800, 0x0800, CRC(dbc942fa) SHA1(9fe224e6ced407289dfa571468259a021d942b7d) )
	ROM_LOAD( "phoenix.47",   0x1000, 0x0800, CRC(cbbb8839) SHA1(b7f449374cac111081559e39646f973e7e99fd64) )
	ROM_LOAD( "vautor04.1j",  0x1800, 0x0800, CRC(106262eb) SHA1(1e52ca66ea3542d86f2604f5aadc854ffe22fd89) )
	ROM_LOAD( "phoenixc.49",  0x2000, 0x0800, CRC(1a1ce0d0) SHA1(c2825eef5d461e16ca2172daff94b3751be2f4dc) )
	ROM_LOAD( "vautor06.1h",  0x2800, 0x0800, CRC(c90e3287) SHA1(696014738d3b87acb10175b021d9fd4885904a9f) )
	ROM_LOAD( "vautor07.1m",  0x3000, 0x0800, CRC(079ac364) SHA1(55b17c069b191cd1752e78068ef683b33c094e56) )
	ROM_LOAD( "vautor08.1n",  0x3800, 0x0800, CRC(1dbd937a) SHA1(efed9adad3d639c893b33071fd86c64800a7a32f) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "vautor12.2h",  0x0000, 0x0800, CRC(8eff75c9) SHA1(d38a0e0c02ba680984dd8748a3c45ac55f81f127) )
	ROM_LOAD( "vautor11.2j",  0x0800, 0x0800, CRC(369e7476) SHA1(599d2fc3b298060d746e95c20a089ad37f685d5b) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( falconz )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "f45.bin",      0x0000, 0x0800, CRC(9158b43b) SHA1(222cbcfb3f95d09bb90148813541c2613d8b7e1c) )
	ROM_LOAD( "f46.bin",      0x0800, 0x0800, CRC(22ddb600) SHA1(9606d11722261990c34b788baae5dc7516ba52d6) )
	ROM_LOAD( "f47.bin",      0x1000, 0x0800, CRC(cb2838d9) SHA1(332e339475b17d17eeb43a524c5cb3bf9818837a) )
	ROM_LOAD( "f48.bin",      0x1800, 0x0800, CRC(552cf57a) SHA1(d9d36495f0cb25c1648e0807c8d37daf49bbf0d4) )
	ROM_LOAD( "f49.bin",      0x2000, 0x0800, CRC(1ff3a982) SHA1(66fb39e7abdf7a9c6e2eb01d41cfe9429781d6aa) )
	ROM_LOAD( "f50.bin",      0x2800, 0x0800, CRC(8c83bff7) SHA1(3dfb090d7e3a9ae8da882b06e166c48555eaf77c) )
	ROM_LOAD( "f51.bin",      0x3000, 0x0800, CRC(805ec2e8) SHA1(7e56fc9990eb99512078e2b1e2874fb33b0aa05c) )
	ROM_LOAD( "f52.bin",      0x3800, 0x0800, CRC(33f3af63) SHA1(f2e2ebdec205360a6fa8ce4bc8cdf15b82b02728) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "f39.bin",      0x0000, 0x0800, CRC(53c52eb0) SHA1(19624ca359996b77d3c65ef78a7af90eeb092377) )
	ROM_LOAD( "f40.bin",      0x0800, 0x0800, CRC(eba42f0f) SHA1(378282cb2c4e10c23179ae3c605ae7bf691150f6) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( vautourz )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "vautour1.bin", 0x0000, 0x0800, CRC(a600f6a4) SHA1(f1aea5ec71da0d0664fb90b87602fe4c4eed2dbe) )
	ROM_LOAD( "vautour2.bin", 0x0800, 0x0800, CRC(3699b11a) SHA1(7122685cbfcd75898eaa68f8c5bf87c11df59a3b) )
	ROM_LOAD( "vautour3.bin", 0x1000, 0x0800, CRC(750b059b) SHA1(6fbaa2ef4c7eef6f731a73b2d33a02fff21b318a) )
	ROM_LOAD( "vautour4.bin", 0x1800, 0x0800, CRC(01a4bfde) SHA1(a740b8f43a226eb585ea538d41228a98392872d6) )
	ROM_LOAD( "vautour5.bin", 0x2000, 0x0800, CRC(1ff3a982) SHA1(66fb39e7abdf7a9c6e2eb01d41cfe9429781d6aa) )
	ROM_LOAD( "vautour6.bin", 0x2800, 0x0800, CRC(8c83bff7) SHA1(3dfb090d7e3a9ae8da882b06e166c48555eaf77c) )
	ROM_LOAD( "vautour7.bin", 0x3000, 0x0800, CRC(805ec2e8) SHA1(7e56fc9990eb99512078e2b1e2874fb33b0aa05c) )
	ROM_LOAD( "vautour8.bin", 0x3800, 0x0800, CRC(1edebb45) SHA1(2fdf061ee600e27a6ed512ea61a8d78307a7fb8a) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "vautor12.2h",  0x0000, 0x0800, CRC(8eff75c9) SHA1(d38a0e0c02ba680984dd8748a3c45ac55f81f127) )
	ROM_LOAD( "vautor11.2j",  0x0800, 0x0800, CRC(369e7476) SHA1(599d2fc3b298060d746e95c20a089ad37f685d5b) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( fenix )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "0.1e",         0x0000, 0x0800, NO_DUMP )	// socket at location '1e' is empty.
	ROM_LOAD( "1.1f",         0x0800, 0x0800, CRC(3699b11a) SHA1(7122685cbfcd75898eaa68f8c5bf87c11df59a3b) )	// 1.1f
	ROM_LOAD( "2.1h",         0x1000, 0x0800, CRC(750b059b) SHA1(6fbaa2ef4c7eef6f731a73b2d33a02fff21b318a) )	// 2.1h
	ROM_LOAD( "3.1j",         0x1800, 0x0800, CRC(61b8a41b) SHA1(6dc7b23cee607042183ac13a0ecf408d88057513) )	//          96.386719%
	ROM_LOAD( "4.1k",         0x2000, 0x0800, CRC(1ff3a982) SHA1(66fb39e7abdf7a9c6e2eb01d41cfe9429781d6aa) )	// 4.1k
	ROM_LOAD( "5.1l",         0x2800, 0x0800, CRC(a210fe51) SHA1(0487d5bc835549cf2bfb8f26f665019490f643b7) )	//          82.812500%
	ROM_LOAD( "6.1m",         0x3000, 0x0800, CRC(805ec2e8) SHA1(7e56fc9990eb99512078e2b1e2874fb33b0aa05c) )	// 6.1m
	ROM_LOAD( "7.1n",         0x3800, 0x0800, CRC(1edebb45) SHA1(2fdf061ee600e27a6ed512ea61a8d78307a7fb8a) )	// 7.1n

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "9.2h",         0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )	// 9.2h
	ROM_LOAD( "8.2j",         0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )	// 8.2j

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "11.3h",        0x0000, 0x0800, CRC(8eff75c9) SHA1(d38a0e0c02ba680984dd8748a3c45ac55f81f127) )	// 11.3h
	ROM_LOAD( "10.3j",        0x0800, 0x0800, CRC(369e7476) SHA1(599d2fc3b298060d746e95c20a089ad37f685d5b) )	// 10.3j

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( griffon )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "griffon0.a5",  0x0000, 0x0800, CRC(c0f73929) SHA1(3cecf8341a5674165d2cae9b22ea5db26a9597de) )
	ROM_LOAD( "griffon1.a6",  0x0800, 0x0800, CRC(3cc33e4a) SHA1(45d16334f245cc185e18f63062e08627e9bd06bb) )
	ROM_LOAD( "griffon2.a7",  0x1000, 0x0800, CRC(750b059b) SHA1(6fbaa2ef4c7eef6f731a73b2d33a02fff21b318a) )
	ROM_LOAD( "griffon3.a8",  0x1800, 0x0800, CRC(3a6188eb) SHA1(f343d7a6dc836d118621e9a143799e33658a3005) )
	ROM_LOAD( "griffon4.a9",  0x2000, 0x0800, CRC(87a45ceb) SHA1(0788dd57eac3047e34a50e730df37059616abc1a) )
	ROM_LOAD( "griffon5.a10", 0x2800, 0x0800, CRC(8c83bff7) SHA1(3dfb090d7e3a9ae8da882b06e166c48555eaf77c) )
	ROM_LOAD( "griffon6.a11", 0x3000, 0x0800, CRC(805ec2e8) SHA1(7e56fc9990eb99512078e2b1e2874fb33b0aa05c) )
	ROM_LOAD( "griffon7.a12", 0x3800, 0x0800, CRC(55e68cb1) SHA1(b19de884fb3c988599772a76c0c5229e76241e6d) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.3d",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "ic24.4d",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "griffon.d7",   0x0000, 0x0800, CRC(53c52eb0) SHA1(19624ca359996b77d3c65ef78a7af90eeb092377) )
	ROM_LOAD( "griffon.d8",   0x0800, 0x0800, CRC(eba42f0f) SHA1(378282cb2c4e10c23179ae3c605ae7bf691150f6) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( nextfase )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "nf01.bin",   0x0000, 0x0800, CRC(b31ce820) SHA1(dfdb17995a14b66d2571c2c8de481d2792f9ce6a) )
	ROM_LOAD( "nf02.bin",   0x0800, 0x0800, CRC(891d21e1) SHA1(bea01962c0706c00eae42920bb2b3bfdb7e80d89) )
	ROM_LOAD( "nf03.bin",   0x1000, 0x0800, CRC(2ab7389d) SHA1(c0bc0c235cae4a8e880237196ea1718f8c1d0123) )
	ROM_LOAD( "nf04.bin",   0x1800, 0x0800, CRC(590d3c36) SHA1(89e87f207cdb9a7f5624170c09626ef85ede3969) )
	ROM_LOAD( "nf05.bin",   0x2000, 0x0800, CRC(3527f247) SHA1(0cccbc3e15d7603deaec845581983bfbcc4d4560) )
	ROM_LOAD( "nf06.bin",   0x2800, 0x0800, CRC(ac5e9ec1) SHA1(0402e5241d99759d804291998efd43f37ce99917) )
	ROM_LOAD( "nf07.bin",   0x3000, 0x0800, CRC(2eab35b4) SHA1(849bf8273317cc869bdd67e50c68399ee8ece81d) )
	ROM_LOAD( "nf08.bin",   0x3800, 0x0800, CRC(04c2323f) SHA1(4d820464f57e4f59acc46ea3264dba3cb9c501a1) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "nf11.bin",      0x0000, 0x0800, CRC(3c7e623f) SHA1(e7ff5fc371664af44785c079e92eeb2d8530187b) )
	ROM_LOAD( "nf12.bin",      0x0800, 0x0800, CRC(59916d3b) SHA1(71aec70a8e096ed1f0c2297b3ae7dca1b8ecc38d) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "nf09.bin",   0x0000, 0x0800, CRC(bacbfa88) SHA1(bf378a729726db01448f2cc4820f06e17659d674) )
	ROM_LOAD( "nf10.bin",   0x0800, 0x0800, CRC(3143a9ee) SHA1(371bb314dc9e4ec6ed469eb81391061296c547ec) )

	/* Proms were missing from this dump, these might not be correct */
	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "mmi6301.ic40",   0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )  /* palette low bits */
	ROM_LOAD( "mmi6301.ic41",   0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )  /* palette high bits */
ROM_END
#endif

ROM_START( pleiads )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ic47.r1",      0x0000, 0x0800, CRC(960212c8) SHA1(52a3232e99920805ce9e195b8a6338ae7044dd18) )
	ROM_LOAD( "ic48.r2",      0x0800, 0x0800, CRC(b254217c) SHA1(312a33cca09d5d2d18992f28eb051230a90db6e3) )
	ROM_LOAD( "ic47.bin",     0x1000, 0x0800, CRC(87e700bb) SHA1(0f352b5461da957c564920fd1da83bc81f41ffb9) ) /* IC 49 on real board */
	ROM_LOAD( "ic48.bin",     0x1800, 0x0800, CRC(2d5198d0) SHA1(6bfdc6c965199c5d4d687fe35dda057ec38cd8e0) ) /* IC 50 on real board */
	ROM_LOAD( "ic51.r5",      0x2000, 0x0800, CRC(49c629bc) SHA1(fd7937d0c114c8d9c1efaa9918ae3df2af41f032) )
	ROM_LOAD( "ic50.bin",     0x2800, 0x0800, CRC(f1a8a00d) SHA1(5c183e3a73fa882ffec3cb9219fb5619e625591a) ) /* IC 52 on real board */
	ROM_LOAD( "ic53.r7",      0x3000, 0x0800, CRC(b5f07fbc) SHA1(2ae687c84732942e69ad4dfb7a4ac1b97b77487a) )
	ROM_LOAD( "ic52.bin",     0x3800, 0x0800, CRC(b1b5a8a6) SHA1(7e4ef298c8ddefc7dc0cbf94a9c9f36a4b807ba0) ) /* IC 54 on real board */

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.bin",     0x0000, 0x0800, CRC(4e30f9e7) SHA1(da023a94725dc40107cd97e4decfd4dc0f9f00ee) ) /* IC 45 on real board */
	ROM_LOAD( "ic24.bin",     0x0800, 0x0800, CRC(5188fc29) SHA1(421dedc674c6dde7abf01412df035a8eb8e6db9b) ) /* IC 44 on real board */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "ic39.bin",     0x0000, 0x0800, CRC(85866607) SHA1(cd240bd056f761b2f9e2142049434f02cae3e315) ) /* IC 27 on real board */
	ROM_LOAD( "ic40.bin",     0x0800, 0x0800, CRC(a841d511) SHA1(8349008ab1d8ef08775b54170c37deb1d391fffc) ) /* IC 26 on real board */

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "7611-5.26",    0x0000, 0x0100, CRC(7a1bcb1e) SHA1(bdfab316ea26e2063879e7aa78b6ae2b55eb95c8) )   /* palette low bits */
	ROM_LOAD( "7611-5.33",    0x0100, 0x0100, CRC(e38eeb83) SHA1(252880d80425b2e697146e76efdc6cb9f3ba0378) )   /* palette high bits */
ROM_END

#if 0
ROM_START( pleiadbl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ic45.bin",     0x0000, 0x0800, CRC(93fc2958) SHA1(d8723c4f4376e035e655f69352c1765fdbf4a602) )
	ROM_LOAD( "ic46.bin",     0x0800, 0x0800, CRC(e2b5b8cd) SHA1(514ab2b24fc1d6d1fd64e74470b601ba9a11f36f) )
	ROM_LOAD( "ic47.bin",     0x1000, 0x0800, CRC(87e700bb) SHA1(0f352b5461da957c564920fd1da83bc81f41ffb9) )
	ROM_LOAD( "ic48.bin",     0x1800, 0x0800, CRC(2d5198d0) SHA1(6bfdc6c965199c5d4d687fe35dda057ec38cd8e0) )
	ROM_LOAD( "ic49.bin",     0x2000, 0x0800, CRC(9dc73e63) SHA1(8a2de6666fecead7071285125b16641b50249adc) )
	ROM_LOAD( "ic50.bin",     0x2800, 0x0800, CRC(f1a8a00d) SHA1(5c183e3a73fa882ffec3cb9219fb5619e625591a) )
	ROM_LOAD( "ic51.bin",     0x3000, 0x0800, CRC(6f56f317) SHA1(d7e6b0b1c58b741de3504640bcc23e86d1a134a0) )
	ROM_LOAD( "ic52.bin",     0x3800, 0x0800, CRC(b1b5a8a6) SHA1(7e4ef298c8ddefc7dc0cbf94a9c9f36a4b807ba0) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "ic23.bin",     0x0000, 0x0800, CRC(4e30f9e7) SHA1(da023a94725dc40107cd97e4decfd4dc0f9f00ee) )
	ROM_LOAD( "ic24.bin",     0x0800, 0x0800, CRC(5188fc29) SHA1(421dedc674c6dde7abf01412df035a8eb8e6db9b) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "ic39.bin",     0x0000, 0x0800, CRC(85866607) SHA1(cd240bd056f761b2f9e2142049434f02cae3e315) )
	ROM_LOAD( "ic40.bin",     0x0800, 0x0800, CRC(a841d511) SHA1(8349008ab1d8ef08775b54170c37deb1d391fffc) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "7611-5.26",    0x0000, 0x0100, CRC(7a1bcb1e) SHA1(bdfab316ea26e2063879e7aa78b6ae2b55eb95c8) )   /* palette low bits */
	ROM_LOAD( "7611-5.33",    0x0100, 0x0100, CRC(e38eeb83) SHA1(252880d80425b2e697146e76efdc6cb9f3ba0378) )   /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( pleiadce )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "pleiades.47",  0x0000, 0x0800, CRC(711e2ba0) SHA1(62d9108b9066d3e2b99c712daf2b9412704970cc) )
	ROM_LOAD( "pleiades.48",  0x0800, 0x0800, CRC(93a36943) SHA1(7cb4a9e8b60e28415df8401373ff4e595eaab7f5) )
	ROM_LOAD( "ic47.bin",     0x1000, 0x0800, CRC(87e700bb) SHA1(0f352b5461da957c564920fd1da83bc81f41ffb9) )
	ROM_LOAD( "pleiades.50",  0x1800, 0x0800, CRC(5a9beba0) SHA1(e9cf03c88d8db2a7cf97877a103cfdd1fe3f459e) )
	ROM_LOAD( "pleiades.51",  0x2000, 0x0800, CRC(1d828719) SHA1(54857a3de9f4c9c5f18b0d46cf428b4171f839e9) )
	ROM_LOAD( "ic50.bin",     0x2800, 0x0800, CRC(f1a8a00d) SHA1(5c183e3a73fa882ffec3cb9219fb5619e625591a) )
	ROM_LOAD( "pleiades.53",  0x3000, 0x0800, CRC(037b319c) SHA1(2ff7a7777a63326e2abca2d1881df33a8e3f8561) )
	ROM_LOAD( "pleiades.54",  0x3800, 0x0800, CRC(ca264c7c) SHA1(3a6adfaa935a1a11cb62e73b9f43b228b711c2da) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "pleiades.45",  0x0000, 0x0800, CRC(8dbd3785) SHA1(700cb9eb8ea64be99d843910cebcd29d601ab2e9) )
	ROM_LOAD( "pleiades.44",  0x0800, 0x0800, CRC(0db3e436) SHA1(cd1825775b0a10df66d2ccc01cb4b6a9a3d2141a) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "ic39.bin",     0x0000, 0x0800, CRC(85866607) SHA1(cd240bd056f761b2f9e2142049434f02cae3e315) )
	ROM_LOAD( "ic40.bin",     0x0800, 0x0800, CRC(a841d511) SHA1(8349008ab1d8ef08775b54170c37deb1d391fffc) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "7611-5.26",    0x0000, 0x0100, CRC(7a1bcb1e) SHA1(bdfab316ea26e2063879e7aa78b6ae2b55eb95c8) )   /* palette low bits */
	ROM_LOAD( "7611-5.33",    0x0100, 0x0100, CRC(e38eeb83) SHA1(252880d80425b2e697146e76efdc6cb9f3ba0378) )   /* palette high bits */
ROM_END
#endif
#if 0
ROM_START( capitol )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cp1.45",       0x0000, 0x0800, CRC(0922905b) SHA1(501342b0162bba43570b1cbefa1ada6302a54017) )
	ROM_LOAD( "cp2.46",       0x0800, 0x0800, CRC(4f168f45) SHA1(8d268dad54a2cf7081f22a29a3e025174ae462e7) )
	ROM_LOAD( "cp3.47",       0x1000, 0x0800, CRC(3975e0b0) SHA1(d509398aa8b90c7c451d9e60bc1ca6488b1563a9) )
	ROM_LOAD( "cp4.48",       0x1800, 0x0800, CRC(da49caa8) SHA1(274c8dcabb9f43a2cbf9682b849ffd2bc8cf6496) )
	ROM_LOAD( "cp5.49",       0x2000, 0x0800, CRC(38e4362b) SHA1(c5aeb8b7d49b3da760904a16b57604e782cf29fc) )
	ROM_LOAD( "cp6.50",       0x2800, 0x0800, CRC(aaf798eb) SHA1(660774db4195aaa499569804a2304e969f168cdf) )
	ROM_LOAD( "cp7.51",       0x3000, 0x0800, CRC(eaadf14c) SHA1(753a46317e98b1ae63f88f5c3e70ff1c7ec04286) )
	ROM_LOAD( "cp8.52",       0x3800, 0x0800, CRC(d3fe2af4) SHA1(f0c9bfc17ba6f55fbe95136da40a3de775aa46d2) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "cp11.23",      0x0000, 0x0800, CRC(9b0bbb8d) SHA1(cde7c0140e773fe28e97e36486d4e048710f6004) )
	ROM_LOAD( "cp12.24",      0x0800, 0x0800, CRC(39949e66) SHA1(21a204f22f04c5808538b21d49ebc6b7cb7625e8) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "cp9.39",       0x0000, 0x0800, CRC(04f7d19a) SHA1(9b8e55ad0e374d4e7538e82c7b3f081b3bb98124) )
	ROM_LOAD( "cp10.40",      0x0800, 0x0800, CRC(4807408f) SHA1(4aa81e934a65e9986b194e9a9bab99f6c85ff7a5) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "ic40.prm",     0x0000, 0x0100, CRC(79350b25) SHA1(57411be4c1d89677f7919ae295446da90612c8a8) )
	ROM_LOAD( "ic41.prm",     0x0100, 0x0100, CRC(e176b768) SHA1(e2184dd495ed579f10b6da0b78379e02d7a6229f) )
ROM_END
#endif

ROM_START( survival )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "g959-32a.u45", 0x0000, 0x0800, CRC(0bc53541) SHA1(0d1bcf226b89d0cfe0864aab8126b276273a23c2) )
	ROM_LOAD( "g959-33a.u46", 0x0800, 0x0800, CRC(726e9428) SHA1(515c6278ece9bf39827b9c886a1a900e274bd272) )
	ROM_LOAD( "g959-34a.u47", 0x1000, 0x0800, CRC(78f166ff) SHA1(ad079372067319dcfad4a00e437d34a999ff9161) )
	ROM_LOAD( "g959-35a.u48", 0x1800, 0x0800, CRC(59dbe099) SHA1(b7a60302510c72d61b397f7d0f615cfe1762ef5f) )
	ROM_LOAD( "g959-36a.u49", 0x2000, 0x0800, CRC(bd5e586e) SHA1(8e53e5de7bf73cb56fbd6c18017f09cbbbb17769) )
	ROM_LOAD( "g959-37a.u50", 0x2800, 0x0800, CRC(b2de1094) SHA1(c8b35b9dd57bd50f837087f10c50f2af04cea823) )
	ROM_LOAD( "g959-38a.u51", 0x3000, 0x0800, CRC(131c4440) SHA1(c2610d8fbf63110767037c384b6776cfe981da4c) )
	ROM_LOAD( "g959-39a.u52", 0x3800, 0x0800, CRC(213bc910) SHA1(a03b3ca8185e929898cc32ea2d5e944c4c897d0d) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "g959-42.u23",  0x0000, 0x0800, CRC(3d1ce38d) SHA1(48b94027467ba360c08a7f56bb75474e6859381f) )
	ROM_LOAD( "g959-43.u24",  0x0800, 0x0800, CRC(cd150da9) SHA1(642264c0eed34ae2b9f1156d261786361f296d1a) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "g959-40.u39",  0x0000, 0x0800, CRC(41dee996) SHA1(55a792504b2ffc1236eb3f427dee38893e1ca5b7) )
	ROM_LOAD( "g959-41.u40",  0x0800, 0x0800, CRC(a255d6dc) SHA1(1b2f635f4392d0df1cbd527dcf6cf662b2a1014e) )

	ROM_REGION( 0x0200, "proms", 0 )
	ROM_LOAD( "clr.u40",      0x0000, 0x0100, CRC(b3e20669) SHA1(4f01c5d74fc8abe748dd88e4513edf52b977ee32) )   /* palette low bits */
	ROM_LOAD( "clr.u41",      0x0100, 0x0100, CRC(abddf69a) SHA1(e22c380a94fb491bec95c4f4c2d4f072839c09cf) )   /* palette high bits */
ROM_END

#if 0
static DRIVER_INIT( condor )
{
	/* additional inputs for coinage */
	memory_install_read_port_handler(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x5000, 0x5000, 0, 0, "DSW1");
}
#endif


GAME( 1980, phoenix,  0,        phoenix,  phoenix,  0,        ROT90, "Amstar", "Phoenix (Amstar)", GAME_SUPPORTS_SAVE )
//GAME( 1980, phoenixa, phoenix,  phoenix,  phoenixa, 0,        ROT90, "Amstar (Centuri license)", "Phoenix (Centuri, set 1)", GAME_SUPPORTS_SAVE )
//GAME( 1980, phoenixb, phoenix,  phoenix,  phoenixa, 0,        ROT90, "Amstar (Centuri license)", "Phoenix (Centuri, set 2)", GAME_SUPPORTS_SAVE )
//GAME( 1980, phoenixt, phoenix,  phoenix,  phoenixt, 0,        ROT90, "Taito", "Phoenix (Taito)", GAME_SUPPORTS_SAVE )
//GAME( 1980, phoenixj, phoenix,  phoenix,  phoenixt, 0,        ROT90, "Taito", "Phoenix (Taito Japan Ver.)", GAME_SUPPORTS_SAVE )
//GAME( 1980, phoenix3, phoenix,  phoenix,  phoenix3, 0,        ROT90, "bootleg", "Phoenix (T.P.N.)", GAME_SUPPORTS_SAVE )
//GAME( 1981, phoenixc, phoenix,  phoenix,  phoenixt, 0,        ROT90, "bootleg?", "Phoenix (IRECSA, G.G.I Corp)", GAME_SUPPORTS_SAVE )
//GAME( 1981, condor,   phoenix,  condor,   condor,   condor,   ROT90, "Sidam (bootleg)",       "Condor (bootleg of Phoenix)", GAME_SUPPORTS_SAVE )
// the following 2 were common bootlegs in england & france respectively
//GAME( 1980, falcon,   phoenix,  phoenix,  phoenixt, 0,        ROT90, "bootleg",               "Falcon (bootleg of Phoenix) (8085A CPU)", GAME_SUPPORTS_SAVE )
//GAME( 1980, vautour,  phoenix,  phoenix,  phoenixt, 0,        ROT90, "Jeutel (bootleg)",      "Vautour (bootleg of Phoenix) (8085A CPU)", GAME_SUPPORTS_SAVE )
//GAME( 1980, falconz,  phoenix,  condor,   falconz,  0,        ROT90, "bootleg",               "Falcon (bootleg of Phoenix) (Z80 CPU)", GAME_SUPPORTS_SAVE )
//GAME( 1980, vautourz, phoenix,  condor,   condor,   condor,   ROT90, "bootleg",               "Vautour (bootleg of Phoenix) (Z80 CPU)", GAME_SUPPORTS_SAVE )
// fenix is an italian bootleg based on vautourz
//GAME( 1980, fenix,    phoenix,  condor,   condor,   condor,   ROT90, "bootleg",               "Fenix (bootleg of Phoenix)", GAME_NOT_WORKING | GAME_SUPPORTS_SAVE )
//GAME( 1980, griffon,  phoenix,  condor,   condor,   condor,   ROT90, "Videotron (bootleg)",   "Griffon (bootleg of Phoenix)", GAME_SUPPORTS_SAVE )
// nextfase is a spanish bootleg
//GAME( 1981, nextfase, phoenix,  phoenix,  nextfase, 0,        ROT90, "Petaco S.A. (bootleg)", "Next Fase (bootleg of Phoenix)", GAME_SUPPORTS_SAVE )

GAME( 1981, pleiads,  0,        pleiads,  pleiads,  0,        ROT90, "Tehkan", "Pleiads (Tehkan)", GAME_IMPERFECT_COLORS )
//GAME( 1981, pleiadbl, pleiads,  pleiads,  pleiadbl, 0,        ROT90, "bootleg", "Pleiads (bootleg)", GAME_IMPERFECT_COLORS )
//GAME( 1981, pleiadce, pleiads,  pleiads,  pleiadce, 0,        ROT90, "Tehkan (Centuri license)", "Pleiads (Centuri)", GAME_IMPERFECT_COLORS )
//GAME( 1981, capitol,  pleiads,  phoenix,  capitol,  0,        ROT90, "Universal Video Spiel", "Capitol", GAME_IMPERFECT_COLORS )

GAME( 1982, survival, 0,        survival, survival, 0,        ROT90, "Rock-ola", "Survival", GAME_IMPERFECT_COLORS )
