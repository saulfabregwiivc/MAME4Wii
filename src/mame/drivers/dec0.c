/***************************************************************************

  Data East 16 bit games - Bryan McPhail, mish@tendril.co.uk

  This file contains drivers for:

    * Heavy Barrel                            (USA set)
    * Heavy Barrel                            (World set)
    * Bad Dudes vs. Dragonninja               (USA set)
    * Dragonninja                             (Japanese version of above)
    * Birdie Try                              (Japanese set)
    * Robocop                                 (World bootleg rom set)
    * Robocop                                 (World rev 3)
    * Robocop                                 (USA rev 1)
    * Robocop                                 (USA rev 0)
    * Hippodrome                              (USA set)
    * Fighting Fantasy                        (Japanese version of above)
    * Sly Spy                                 (USA rev 3)
    * Sly Spy                                 (USA rev 2)
    * Secret Agent                            (World set)
    * Midnight Resistance                     (World set)
    * Midnight Resistance                     (USA set)
    * Midnight Resistance                     (Japanese set)
    * Boulderdash                             (World set)

    Heavy Barrel, Bad Dudes, Robocop, Birdie Try & Hippodrome use the 'MEC-M1'
motherboard and varying game boards.  Sly Spy, Midnight Resistance and
Boulderdash use the same graphics chips but are different pcbs.

    There are Secret Agent (bootleg) and Robocop (bootleg) sets to add.

    Thanks to Gouky & Richard Bush for information along the way, especially
    Gouky's patch for Bad Dudes & YM3812 information!
    Thanks to JC Alexander for fix to Robocop ending!

    All games' Dip Switches (except Boulder Dash) have been verified against
Original Service Manuals and Service Mode (when available).


ToDo:
  Hook up the 68705 in Midnight Resistance (bootleg)

***************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "cpu/m6502/m6502.h"
#include "cpu/h6280/h6280.h"
#include "cpu/z80/z80.h"
#include "dec0.h"
#include "sound/2203intf.h"
#include "sound/3812intf.h"
#include "sound/okim6295.h"
#include "sound/msm5205.h"


UINT16 *dec0_ram;
UINT8 *robocop_shared_ram;
#if 0
static UINT8 automat_adpcm_byte;
static int automat_msm5205_vclk_toggle;
#endif
/******************************************************************************/

static WRITE16_HANDLER( dec0_control_w )
{
	switch (offset << 1)
	{
		case 0: /* Playfield & Sprite priority */
			dec0_priority_w(space, 0, data, mem_mask);
			break;

		case 2: /* DMA flag */
			dec0_update_sprites_w(space, 0, 0, mem_mask);
			break;

		case 4: /* 6502 sound cpu */
			if (ACCESSING_BITS_0_7)
			{
				soundlatch_w(space, 0, data & 0xff);
				cputag_set_input_line(space->machine, "audiocpu", INPUT_LINE_NMI, PULSE_LINE);
			}
			break;

		case 6: /* Intel 8751 microcontroller - Bad Dudes, Heavy Barrel, Birdy Try only */
			dec0_i8751_write(space->machine, data);
			break;

		case 8: /* Interrupt ack (VBL - IRQ 6) */
			break;

		case 0xa: /* Mix Psel(?). */
 			logerror("CPU #0 PC %06x: warning - write %02x to unmapped memory address %06x\n",cpu_get_pc(space->cpu),data,0x30c010+(offset<<1));
			break;

		case 0xc: /* Cblk - coin blockout.  Seems to be unused by the games */
			break;

		case 0xe: /* Reset Intel 8751? - not sure, all the games write here at startup */
			dec0_i8751_reset();
 			logerror("CPU #0 PC %06x: warning - write %02x to unmapped memory address %06x\n",cpu_get_pc(space->cpu),data,0x30c010+(offset<<1));
			break;

		default:
			logerror("CPU #0 PC %06x: warning - write %02x to unmapped memory address %06x\n",cpu_get_pc(space->cpu),data,0x30c010+(offset<<1));
			break;
	}
}

#if 0
static WRITE16_HANDLER( automat_control_w )
{
	switch (offset << 1)
	{
		case 0xe: /* 6502 sound cpu */
			if (ACCESSING_BITS_0_7)
			{
				soundlatch_w(space, 0, data & 0xff);
				cputag_set_input_line(space->machine, "audiocpu", 0, HOLD_LINE);
			}
			break;

		case 12: /* DMA flag */
			dec0_update_sprites_w(space, 0, 0, mem_mask);
			break;
#if 0
		case 8: /* Interrupt ack (VBL - IRQ 6) */
			break;

		case 0xa: /* Mix Psel(?). */
			logerror("CPU #0 PC %06x: warning - write %02x to unmapped memory address %06x\n",cpu_get_pc(space->cpu),data,0x30c010+(offset<<1));
			break;

		case 0xc: /* Cblk - coin blockout.  Seems to be unused by the games */
			break;
#endif

		default:
			logerror("CPU #0 PC %06x: warning - write %02x to unmapped memory address %06x\n",cpu_get_pc(space->cpu),data,0x30c010+(offset<<1));
			break;
	}
}
#endif

static WRITE16_HANDLER( slyspy_control_w )
{
    switch (offset << 1)
	{
    	case 0:
			if (ACCESSING_BITS_0_7)
			{
				soundlatch_w(space, 0, data & 0xff);
				cputag_set_input_line(space->machine, "audiocpu", INPUT_LINE_NMI, PULSE_LINE);
			}
			break;
		case 2:
			dec0_priority_w(space, 0, data, mem_mask);
			break;
    }
}

static WRITE16_HANDLER( midres_sound_w )
{
	if (ACCESSING_BITS_0_7)
	{
		soundlatch_w(space, 0, data & 0xff);
		cputag_set_input_line(space->machine, "audiocpu", INPUT_LINE_NMI, PULSE_LINE);
	}
}

/******************************************************************************/

static ADDRESS_MAP_START( dec0_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x05ffff) AM_ROM
	AM_RANGE(0x240000, 0x240007) AM_WRITE(dec0_pf1_control_0_w)								/* text layer */
	AM_RANGE(0x240010, 0x240017) AM_WRITE(dec0_pf1_control_1_w)
 	AM_RANGE(0x242000, 0x24207f) AM_WRITEONLY AM_BASE(&dec0_pf1_colscroll)
	AM_RANGE(0x242400, 0x2427ff) AM_WRITEONLY AM_BASE(&dec0_pf1_rowscroll)
	AM_RANGE(0x242800, 0x243fff) AM_RAM														/* Robocop only */
	AM_RANGE(0x244000, 0x245fff) AM_RAM_WRITE(dec0_pf1_data_w) AM_BASE(&dec0_pf1_data)
	AM_RANGE(0x246000, 0x246007) AM_WRITE(dec0_pf2_control_0_w)								/* first tile layer */
	AM_RANGE(0x246010, 0x246017) AM_WRITE(dec0_pf2_control_1_w)
	AM_RANGE(0x248000, 0x24807f) AM_WRITEONLY AM_BASE(&dec0_pf2_colscroll)
	AM_RANGE(0x248400, 0x2487ff) AM_WRITEONLY AM_BASE(&dec0_pf2_rowscroll)
	AM_RANGE(0x24a000, 0x24a7ff) AM_RAM_WRITE(dec0_pf2_data_w) AM_BASE(&dec0_pf2_data)
	AM_RANGE(0x24c000, 0x24c007) AM_WRITE(dec0_pf3_control_0_w)								/* second tile layer */
	AM_RANGE(0x24c010, 0x24c017) AM_WRITE(dec0_pf3_control_1_w)
	AM_RANGE(0x24c800, 0x24c87f) AM_RAM AM_BASE(&dec0_pf3_colscroll)
	AM_RANGE(0x24cc00, 0x24cfff) AM_WRITEONLY AM_BASE(&dec0_pf3_rowscroll)
	AM_RANGE(0x24d000, 0x24d7ff) AM_RAM_WRITE(dec0_pf3_data_w) AM_BASE(&dec0_pf3_data)
	AM_RANGE(0x300000, 0x30001f) AM_READ(dec0_rotary_r)
	AM_RANGE(0x30c000, 0x30c00b) AM_READ(dec0_controls_r)
	AM_RANGE(0x30c010, 0x30c01f) AM_WRITE(dec0_control_w)									/* Priority, sound, etc. */
	AM_RANGE(0x310000, 0x3107ff) AM_RAM_WRITE(dec0_paletteram_rg_w) AM_BASE(&paletteram16)
	AM_RANGE(0x314000, 0x3147ff) AM_RAM_WRITE(dec0_paletteram_b_w) AM_BASE(&paletteram16_2)
	AM_RANGE(0xff8000, 0xffbfff) AM_RAM AM_BASE(&dec0_ram) 									/* Main ram */
	AM_RANGE(0xffc000, 0xffc7ff) AM_RAM AM_BASE(&spriteram16)								/* Sprites */
ADDRESS_MAP_END

static ADDRESS_MAP_START( robocop_sub_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x1f0000, 0x1f1fff) AM_RAM									/* Main ram */
	AM_RANGE(0x1f2000, 0x1f3fff) AM_RAM AM_BASE(&robocop_shared_ram)	/* Shared ram */
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(h6280_irq_status_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( hippodrm_sub_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x180000, 0x1800ff) AM_READWRITE(hippodrm_shared_r, hippodrm_shared_w)
	AM_RANGE(0x1a0000, 0x1a001f) AM_WRITE(dec0_pf3_control_8bit_w)
	AM_RANGE(0x1a1000, 0x1a17ff) AM_READWRITE(dec0_pf3_data_8bit_r, dec0_pf3_data_8bit_w)
	AM_RANGE(0x1d0000, 0x1d00ff) AM_READWRITE(hippodrm_prot_r, hippodrm_prot_w)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_RAMBANK(8) /* Main ram */
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(h6280_irq_status_w)
	AM_RANGE(0x1ff402, 0x1ff403) AM_READ_PORT("VBLANK")
ADDRESS_MAP_END

static ADDRESS_MAP_START( slyspy_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x05ffff) AM_ROM

	/* These locations aren't real!  They are just there so memory is allocated */
	AM_RANGE(0x200000, 0x2007ff) AM_WRITENOP AM_BASE(&dec0_pf2_data)
	AM_RANGE(0x202000, 0x203fff) AM_WRITENOP AM_BASE(&dec0_pf1_data)
	AM_RANGE(0x232000, 0x23207f) AM_WRITENOP AM_BASE(&dec0_pf2_colscroll)
	AM_RANGE(0x232400, 0x2327ff) AM_WRITENOP AM_BASE(&dec0_pf2_rowscroll)
	AM_RANGE(0x23c000, 0x23c07f) AM_WRITENOP AM_BASE(&dec0_pf1_colscroll)
	AM_RANGE(0x23c400, 0x23c7ff) AM_WRITENOP AM_BASE(&dec0_pf1_rowscroll)

	AM_RANGE(0x244000, 0x244001) AM_READ(slyspy_state_r) AM_WRITENOP /* protection */

	/* The location of p1 & pf2 can change between these according to protection */
	AM_RANGE(0x240000, 0x241fff) AM_WRITE(slyspy_240000_w)
	AM_RANGE(0x242000, 0x243fff) AM_WRITE(slyspy_242000_w)
	AM_RANGE(0x246000, 0x247fff) AM_WRITE(slyspy_246000_w)
	AM_RANGE(0x248000, 0x249fff) AM_WRITE(slyspy_248000_w)
	AM_RANGE(0x24a000, 0x24a001) AM_WRITE(slyspy_state_w) /* Protection */
	AM_RANGE(0x24c000, 0x24dfff) AM_WRITE(slyspy_24c000_w)
	AM_RANGE(0x24e000, 0x24ffff) AM_WRITE(slyspy_24e000_w)

	/* Pf3 is unaffected by protection */
	AM_RANGE(0x300000, 0x300007) AM_WRITE(dec0_pf3_control_0_w)
	AM_RANGE(0x300010, 0x300017) AM_WRITE(dec0_pf3_control_1_w)
	AM_RANGE(0x300800, 0x30087f) AM_WRITEONLY AM_BASE(&dec0_pf3_colscroll)
	AM_RANGE(0x300c00, 0x300fff) AM_WRITEONLY AM_BASE(&dec0_pf3_rowscroll)
	AM_RANGE(0x301000, 0x3017ff) AM_WRITE(dec0_pf3_data_w) AM_BASE(&dec0_pf3_data)

	AM_RANGE(0x304000, 0x307fff) AM_RAM AM_BASE(&dec0_ram) 	/* Sly spy main ram */
	AM_RANGE(0x308000, 0x3087ff) AM_RAM AM_BASE(&spriteram16)	/* Sprites */
	AM_RANGE(0x310000, 0x3107ff) AM_RAM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x314000, 0x314003) AM_WRITE(slyspy_control_w)
	AM_RANGE(0x314008, 0x31400f) AM_READ(slyspy_controls_r)
	AM_RANGE(0x31c000, 0x31c00f) AM_READ(slyspy_protection_r) AM_WRITENOP
ADDRESS_MAP_END

static ADDRESS_MAP_START( midres_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x100000, 0x103fff) AM_RAM AM_BASE(&dec0_ram)
	AM_RANGE(0x120000, 0x1207ff) AM_RAM AM_BASE(&spriteram16)
	AM_RANGE(0x140000, 0x1407ff) AM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x160000, 0x160001) AM_WRITE(dec0_priority_w)
	AM_RANGE(0x180000, 0x18000f) AM_READ(midres_controls_r)
	AM_RANGE(0x180008, 0x18000f) AM_WRITENOP /* ?? watchdog ?? */
	AM_RANGE(0x1a0000, 0x1a0001) AM_WRITE(midres_sound_w)

	AM_RANGE(0x200000, 0x200007) AM_WRITE(dec0_pf2_control_0_w)
	AM_RANGE(0x200010, 0x200017) AM_WRITE(dec0_pf2_control_1_w)
	AM_RANGE(0x220000, 0x2207ff) AM_WRITE(dec0_pf2_data_w) AM_BASE(&dec0_pf2_data)
	AM_RANGE(0x220800, 0x220fff) AM_WRITE(dec0_pf2_data_w)	/* mirror address used in end sequence */
	AM_RANGE(0x240000, 0x24007f) AM_RAM AM_BASE(&dec0_pf2_colscroll)
	AM_RANGE(0x240400, 0x2407ff) AM_RAM AM_BASE(&dec0_pf2_rowscroll)

	AM_RANGE(0x280000, 0x280007) AM_WRITE(dec0_pf3_control_0_w)
	AM_RANGE(0x280010, 0x280017) AM_WRITE(dec0_pf3_control_1_w)
	AM_RANGE(0x2a0000, 0x2a07ff) AM_WRITE(dec0_pf3_data_w) AM_BASE(&dec0_pf3_data)
	AM_RANGE(0x2c0000, 0x2c007f) AM_RAM AM_BASE(&dec0_pf3_colscroll)
	AM_RANGE(0x2c0400, 0x2c07ff) AM_RAM AM_BASE(&dec0_pf3_rowscroll)

	AM_RANGE(0x300000, 0x300007) AM_WRITE(dec0_pf1_control_0_w)
	AM_RANGE(0x300010, 0x300017) AM_WRITE(dec0_pf1_control_1_w)
	AM_RANGE(0x320000, 0x321fff) AM_WRITE(dec0_pf1_data_w) AM_BASE(&dec0_pf1_data)
	AM_RANGE(0x340000, 0x34007f) AM_RAM AM_BASE(&dec0_pf1_colscroll)
	AM_RANGE(0x340400, 0x3407ff) AM_RAM AM_BASE(&dec0_pf1_rowscroll)

	AM_RANGE(0x320000, 0x321fff) AM_RAM
ADDRESS_MAP_END

/******************************************************************************/

static ADDRESS_MAP_START( dec0_s_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x05ff) AM_RAM
	AM_RANGE(0x0800, 0x0801) AM_DEVWRITE("ym1", ym2203_w)
	AM_RANGE(0x1000, 0x1001) AM_DEVWRITE("ym2", ym3812_w)
	AM_RANGE(0x3000, 0x3000) AM_READ(soundlatch_r)
	AM_RANGE(0x3800, 0x3800) AM_DEVREADWRITE("oki", okim6295_r, okim6295_w)
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

/* Physical memory map (21 bits) */
static ADDRESS_MAP_START( slyspy_s_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x090000, 0x090001) AM_DEVWRITE("ym2", ym3812_w)
	AM_RANGE(0x0a0000, 0x0a0001) AM_READNOP /* Protection counter */
	AM_RANGE(0x0b0000, 0x0b0001) AM_DEVWRITE("ym1", ym2203_w)
	AM_RANGE(0x0e0000, 0x0e0001) AM_DEVREADWRITE("oki", okim6295_r, okim6295_w)
	AM_RANGE(0x0f0000, 0x0f0001) AM_READ(soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_RAMBANK(8)
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(h6280_irq_status_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( midres_s_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x108000, 0x108001) AM_DEVWRITE("ym2", ym3812_w)
	AM_RANGE(0x118000, 0x118001) AM_DEVWRITE("ym1", ym2203_w)
	AM_RANGE(0x130000, 0x130001) AM_DEVREADWRITE("oki", okim6295_r, okim6295_w)
	AM_RANGE(0x138000, 0x138001) AM_READ(soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_RAMBANK(8)
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(h6280_irq_status_w)
ADDRESS_MAP_END

#if 0
static ADDRESS_MAP_START( secretab_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x05ffff) AM_ROM
	AM_RANGE(0x240000, 0x240007) AM_WRITE(dec0_pf2_control_0_w)
	AM_RANGE(0x240010, 0x240017) AM_WRITE(dec0_pf2_control_1_w)
	AM_RANGE(0x246000, 0x247fff) AM_RAM_WRITE(dec0_pf2_data_w) AM_BASE(&dec0_pf2_data)
//  AM_RANGE(0x240000, 0x24007f) AM_RAM AM_BASE(&dec0_pf2_colscroll)
//  AM_RANGE(0x240400, 0x2407ff) AM_RAM AM_BASE(&dec0_pf2_rowscroll)

//  AM_RANGE(0x200000, 0x300007) AM_WRITE(dec0_pf1_control_0_w)
//  AM_RANGE(0x300010, 0x300017) AM_WRITE(dec0_pf1_control_1_w)
	AM_RANGE(0x24e000, 0x24ffff) AM_RAM_WRITE(dec0_pf1_data_w) AM_BASE(&dec0_pf1_data)
//  AM_RANGE(0x340000, 0x34007f) AM_RAM AM_BASE(&dec0_pf1_colscroll)
//  AM_RANGE(0x340400, 0x3407ff) AM_RAM AM_BASE(&dec0_pf1_rowscroll)

	AM_RANGE(0x314008, 0x31400f) AM_READ(slyspy_controls_r)
//  AM_RANGE(0x314000, 0x314003) AM_WRITE(slyspy_control_w)

	AM_RANGE(0x300000, 0x300007) AM_WRITE(dec0_pf3_control_0_w)
	AM_RANGE(0x300010, 0x300017) AM_WRITE(dec0_pf3_control_1_w)
	AM_RANGE(0x300800, 0x30087f) AM_RAM AM_BASE(&dec0_pf3_colscroll)
	AM_RANGE(0x300c00, 0x300fff) AM_RAM AM_BASE(&dec0_pf3_rowscroll)
	AM_RANGE(0x301000, 0x3017ff) AM_RAM_WRITE(dec0_pf3_data_w) AM_BASE(&dec0_pf3_data)
	AM_RANGE(0x301800, 0x307fff) AM_RAM AM_BASE(&dec0_ram) /* Sly spy main ram */
	AM_RANGE(0x310000, 0x3107ff) AM_RAM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0xb08000, 0xb087ff) AM_RAM AM_BASE(&spriteram16) /* Sprites */
ADDRESS_MAP_END
#endif
#if 0
static ADDRESS_MAP_START( automat_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x05ffff) AM_ROM

	AM_RANGE(0x240000, 0x240007) AM_WRITE(dec0_pf1_control_0_w)			/* text layer */
	AM_RANGE(0x240010, 0x240017) AM_WRITE(dec0_pf1_control_1_w)
	AM_RANGE(0x242000, 0x24207f) AM_WRITEONLY AM_BASE(&dec0_pf1_colscroll)
	AM_RANGE(0x242400, 0x2427ff) AM_WRITEONLY AM_BASE(&dec0_pf1_rowscroll)

	AM_RANGE(0x242800, 0x243fff) AM_RAM 								/* Robocop only */
	AM_RANGE(0x244000, 0x245fff) AM_RAM_WRITE(dec0_pf1_data_w) AM_BASE(&dec0_pf1_data)

	AM_RANGE(0x246000, 0x246007) AM_WRITE(dec0_pf2_control_0_w)			/* first tile layer */
	AM_RANGE(0x246010, 0x246017) AM_WRITE(dec0_pf2_control_1_w)
	AM_RANGE(0x248000, 0x24807f) AM_WRITEONLY AM_BASE(&dec0_pf2_colscroll)
	AM_RANGE(0x248400, 0x2487ff) AM_WRITEONLY AM_BASE(&dec0_pf2_rowscroll)

	AM_RANGE(0x24a000, 0x24a7ff) AM_RAM_WRITE(dec0_pf2_data_w) AM_BASE(&dec0_pf2_data)
	AM_RANGE(0x24c000, 0x24c007) AM_WRITE(dec0_pf3_control_0_w)			/* second tile layer */
	AM_RANGE(0x24c010, 0x24c017) AM_WRITE(dec0_pf3_control_1_w)
	AM_RANGE(0x24c800, 0x24c87f) AM_RAM AM_BASE(&dec0_pf3_colscroll)
	AM_RANGE(0x24cc00, 0x24cfff) AM_WRITEONLY AM_BASE(&dec0_pf3_rowscroll)
	AM_RANGE(0x24d000, 0x24d7ff) AM_RAM_WRITE(dec0_pf3_data_w) AM_BASE(&dec0_pf3_data)
	AM_RANGE(0x300000, 0x30001f) AM_READ(dec0_rotary_r)
	AM_RANGE(0x30c000, 0x30c00b) AM_READ(dec0_controls_r)
	AM_RANGE(0x30c000, 0x30c01f) AM_WRITE(automat_control_w)			/* Priority, sound, etc. */
	AM_RANGE(0x310000, 0x3107ff) AM_RAM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x314000, 0x3147ff) AM_RAM
	AM_RANGE(0x400008, 0x400009) AM_WRITE(dec0_priority_w) 				// NEW
	AM_RANGE(0xff8000, 0xffbfff) AM_RAM AM_BASE(&dec0_ram) 				/* Main ram */
	AM_RANGE(0xffc000, 0xffc7ff) AM_RAM AM_BASE(&spriteram16) 			/* Sprites */
ADDRESS_MAP_END
#endif
#if 0
static WRITE8_HANDLER( automat_adpcm_w )
{
	automat_adpcm_byte = data;
}
#endif
#if 0
static ADDRESS_MAP_START( automat_s_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0xc000, 0xc7ff) AM_RAM
//  AM_RANGE(0xc800, 0xc800) AM_WRITE(ym2203_control_port_0_w)
//  AM_RANGE(0xc801, 0xc801) AM_WRITE(ym2203_write_port_0_w)
	AM_RANGE(0xd800, 0xd800) AM_READ(soundlatch_r)
//  AM_RANGE(0xd000, 0xd000) AM_WRITE(ym2203_control_port_1_w)
//  AM_RANGE(0xd001, 0xd001) AM_WRITE(ym2203_write_port_1_w)
	AM_RANGE(0xf000, 0xf000) AM_WRITE(automat_adpcm_w)
	AM_RANGE(0x0000, 0xffff) AM_ROM
ADDRESS_MAP_END
#endif

/******************************************************************************/

static INPUT_PORTS_START( dec0 )
	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) /* Button 3 - only in Service Mode */
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 ) /* Button 4 - only in Service Mode */
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL /* Button 3 - only in Service Mode */
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL /* Button 4 - only in Service Mode */

	PORT_START("SYSTEM")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON5 ) /* Button 5 - only in Service Mode */
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_COCKTAIL /* Button 5 - only in Service Mode */
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_VBLANK )
INPUT_PORTS_END

static INPUT_PORTS_START( dec1 )
	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) /* Button 3 - only in Service Mode */
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL /* Button 3 - only in Service Mode */
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("SYSTEM")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

#define DEC0_COIN_SETTING \
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2")\
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(      0x0001, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_2C ) ) \
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:3,4")\
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(      0x0004, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x000c, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(      0x0008, DEF_STR( 1C_2C ) )

static INPUT_PORTS_START( hbarrel )
	PORT_INCLUDE( dec0 )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(      0x0000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:3,4")
	PORT_DIPSETTING(      0x0000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_3C ) )
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x0080, 0x0080, "SW1:8" )	// Always OFF

	PORT_DIPNAME( 0x0300, 0x0100, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0200, "5" )
	PORT_DIPSETTING(      0x0100, "1" )
	PORT_DIPSETTING(      0x0000, "Infinite (Cheat)")
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(      0x3000, "30k 80k 160k" )
	PORT_DIPSETTING(      0x2000, "50k 120k 190k" )
	PORT_DIPSETTING(      0x1000, "100k 200k 300k" )
	PORT_DIPSETTING(      0x0000, "150k 300k 450k" )
	PORT_DIPNAME( 0x4000, 0x0000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(      0x4000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Yes ) )
	PORT_DIPUNUSED_DIPLOC( 0x8000, 0x8000, "SW1:8" )	// Always OFF

	PORT_START("AN0")	/* player 1 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0x0f, 0x00, IPT_POSITIONAL ) PORT_POSITIONS(12) PORT_WRAPS PORT_SENSITIVITY(15) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_Z) PORT_CODE_INC(KEYCODE_X) PORT_REVERSE PORT_FULL_TURN_COUNT(12)

	PORT_START("AN1")	/* player 2 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0x0f, 0x00, IPT_POSITIONAL ) PORT_POSITIONS(12) PORT_WRAPS PORT_SENSITIVITY(15) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_N) PORT_CODE_INC(KEYCODE_M) PORT_PLAYER(2) PORT_REVERSE PORT_FULL_TURN_COUNT(12)
INPUT_PORTS_END

#if 0
static INPUT_PORTS_START( birdtry )
	PORT_INCLUDE( dec0 )

	PORT_START("DSW")
	DEC0_COIN_SETTING
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x0080, 0x0080, "SW1:8" )	// Always OFF

	PORT_DIPNAME( 0x0300, 0x0300, "Difficulty (Extend)" ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, "Difficulty (Course)" ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, "Timer" ) PORT_DIPLOCATION("SW2:6")
	PORT_DIPSETTING(      0x2000, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0000, "Fast" )
	PORT_DIPNAME( 0xc000, 0x0000, "Control Panel Type" ) PORT_DIPLOCATION("SW2:7,8")
	PORT_DIPSETTING(      0xc000, "Type A - Cocktail" )
	PORT_DIPSETTING(      0x8000, "Type B - Cocktail 2" )
	PORT_DIPSETTING(      0x4000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, "Type C - Upright" )

	PORT_START("AN0")	/* player 1 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0x0f, 0x00, IPT_POSITIONAL ) PORT_POSITIONS(12) PORT_WRAPS PORT_SENSITIVITY(15) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_Z) PORT_CODE_INC(KEYCODE_X) PORT_REVERSE PORT_FULL_TURN_COUNT(12)

	PORT_START("AN1")	/* player 2 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0x0f, 0x00, IPT_POSITIONAL ) PORT_POSITIONS(12) PORT_WRAPS PORT_SENSITIVITY(15) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_N) PORT_CODE_INC(KEYCODE_M) PORT_PLAYER(2) PORT_REVERSE PORT_FULL_TURN_COUNT(12)
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( baddudes )
	PORT_INCLUDE( dec0 )

	PORT_START("DSW")
	DEC0_COIN_SETTING
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x0080, 0x0080, "SW1:8" )		// Always OFF

	PORT_DIPNAME( 0x0300, 0x0200, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0200, "5" )
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0100, "1" )
	PORT_DIPSETTING(      0x0000, "Infinite (Cheat)")
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(      0x1000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Yes ) )
	PORT_DIPUNUSED_DIPLOC( 0x2000, 0x2000, "SW2:6" )	// Always OFF
	PORT_DIPUNUSED_DIPLOC( 0x4000, 0x4000, "SW2:7" )	// Always OFF
	PORT_DIPUNUSED_DIPLOC( 0x8000, 0x8000, "SW2:8" )	// Always OFF

	PORT_START("AN0")	/* player 1 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* unused */

	PORT_START("AN1")	/* player 2 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* unused */
INPUT_PORTS_END

static INPUT_PORTS_START( robocop )
	PORT_INCLUDE( dec0 )

	PORT_START("DSW")
	DEC0_COIN_SETTING
	PORT_DIPUNUSED_DIPLOC( 0x0010, 0x0010, "SW1:5" )	// Always OFF
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(      0x0000, DEF_STR( Upright ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Cocktail ) )

	PORT_DIPNAME( 0x0300, 0x0300, "Player Energy" ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0100, DEF_STR( Low ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( High ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Very_High ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x2000, 0x2000, "Bonus Stage Energy" ) PORT_DIPLOCATION("SW2:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Low ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( High ) )
	PORT_DIPNAME( 0x4000, 0x4000, "Brink Time" )
	PORT_DIPSETTING(      0x4000, DEF_STR( Normal ) ) PORT_DIPLOCATION("SW2:7")	/* not mentioned in manual */
	PORT_DIPSETTING(      0x0000, "Less" )
	PORT_DIPUNUSED_DIPLOC( 0x8000, 0x8000, "SW2:8" )	// Always OFF
INPUT_PORTS_END

static INPUT_PORTS_START( hippodrm )
	PORT_INCLUDE( dec0 )

	PORT_START("DSW")
	DEC0_COIN_SETTING
	PORT_DIPUNUSED_DIPLOC( 0x0010, 0x0010, "SW1:5" )	// Always OFF
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x0080, 0x0080, "SW1:8" )	// Always OFF

	PORT_DIPNAME( 0x0300, 0x0000, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0100, "1" )
	PORT_DIPSETTING(      0x0300, "2" )
	PORT_DIPSETTING(      0x0200, "3" )
	PORT_DIPSETTING(      0x0000, "5" )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x3000, 0x3000, "Player & Enemy Energy" ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(      0x3000, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Low ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Very_Low ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( High ) )
	PORT_DIPNAME( 0x4000, 0x4000, "Enemy Power Decrease on Continue" ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(      0x4000, "2 Dots" )	// 2 Dots less
	PORT_DIPSETTING(      0x0000, "3 Dots" )	// 3 Dots less
	PORT_DIPUNUSED_DIPLOC( 0x8000, 0x8000, "SW2:8" )	// Always OFF

	PORT_START("VBLANK")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_VBLANK )
INPUT_PORTS_END

static INPUT_PORTS_START( slyspy )
	PORT_INCLUDE( dec1 )
	/* if you set VBLANK as ACTIVE_LOW, you obtain screwed up colors */

	PORT_START("DSW")
	DEC0_COIN_SETTING
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(      0x0000, DEF_STR( Upright ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Cocktail ) )

	PORT_DIPNAME( 0x0300, 0x0300, "Energy" ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0200, "Low - 8 bars" )
	PORT_DIPSETTING(      0x0300, "Medium - 10 bars" )
	PORT_DIPSETTING(      0x0100, "High - 12 bars" )
	PORT_DIPSETTING(      0x0000, "Very High - 14 bars" )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")	/* not mentioned in manual */
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Yes ) )
	PORT_DIPUNUSED_DIPLOC( 0x2000, 0x2000, "SW2:6" )	// Always OFF
	PORT_DIPUNUSED_DIPLOC( 0x4000, 0x4000, "SW2:7" )	// Always OFF
	PORT_DIPUNUSED_DIPLOC( 0x8000, 0x8000, "SW2:8" )	// Always OFF
INPUT_PORTS_END

static INPUT_PORTS_START( midres )
	PORT_INCLUDE( dec1 )

	PORT_START("DSW")
	DEC0_COIN_SETTING
	PORT_DIPUNUSED_DIPLOC( 0x0010, 0x0010, "SW1:5" )	// Always OFF
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x0080, 0x0080, "SW1:8" )	// Always OFF

	PORT_DIPNAME( 0x0300, 0x0100, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0200, "4" )
	PORT_DIPSETTING(      0x0100, "5" )
	PORT_DIPSETTING(      0x0000, "Infinite (Cheat)")
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )  PORT_DIPLOCATION("SW2:5")	/* manual mentions Extra Lives - Default OFF */
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )  PORT_DIPLOCATION("SW2:6")	/* manual mentions Extra Lives - Default OFF */
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x0000, DEF_STR( Allow_Continue ) )  PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(      0x4000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Yes ) )
	PORT_DIPUNUSED_DIPLOC( 0x8000, 0x8000, "SW2:8" )	// Always OFF

	PORT_START("AN0")	/* player 1 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0x0f, 0x00, IPT_POSITIONAL ) PORT_POSITIONS(12) PORT_WRAPS PORT_SENSITIVITY(15) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_Z) PORT_CODE_INC(KEYCODE_X) PORT_REVERSE PORT_FULL_TURN_COUNT(12)

	PORT_START("AN1")	/* player 2 12-way rotary control - converted in controls_r() */
	PORT_BIT( 0x0f, 0x00, IPT_POSITIONAL ) PORT_POSITIONS(12) PORT_WRAPS PORT_SENSITIVITY(15) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_N) PORT_CODE_INC(KEYCODE_M) PORT_PLAYER(2) PORT_REVERSE PORT_FULL_TURN_COUNT(12)
INPUT_PORTS_END

static INPUT_PORTS_START( bouldash )
	PORT_INCLUDE( dec1 )

	PORT_MODIFY("SYSTEM")
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_VBLANK )		/* extremely slow palette fades with ACTIVE_HIGH */

	PORT_START("DSW")
	/* Different Coinage. Just a few combinations from manual, the rest was figured out */
	PORT_DIPNAME( 0x0007, 0x0007, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0005, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0038, 0x0038, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:4,5,6")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0038, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0028, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0018, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(      0x0000, DEF_STR( Upright ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Cocktail ) )

	PORT_DIPNAME( 0x0300, 0x0100, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0200, "4" )
	PORT_DIPSETTING(      0x0100, "5" )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPUNUSED_DIPLOC( 0x1000, 0x1000, "SW2:5" )	// Always OFF
	PORT_DIPNAME( 0x2000, 0x2000, "Game Change Mode" ) PORT_DIPLOCATION("SW2:6")
	PORT_DIPSETTING(      0x2000, "Part 1" )
	PORT_DIPSETTING(      0x0000, "Part 2" )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x8000, 0x0000, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout charlayout =
{
	8,8,	/* 8*8 chars */
	RGN_FRAC(1,4),
	4,		/* 4 bits per pixel  */
	{ RGN_FRAC(0,4), RGN_FRAC(2,4), RGN_FRAC(1,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8	/* every char takes 8 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(1,4), RGN_FRAC(3,4), RGN_FRAC(0,4), RGN_FRAC(2,4) },
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*16
};

#if 0
static const gfx_layout automat_tilelayout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(1,4), RGN_FRAC(3,4), RGN_FRAC(0,4), RGN_FRAC(2,4) },
	{ 16*8+7, 16*8+6,16*8+5,16*8+4,16*8+3,16*8+2,16*8+1,16*8+0,7,6,5,4,3,2,1,0},
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*16
};
#endif
#if 0
static const gfx_layout automat_tilelayout3 =
{
	16,16,
	0x800,
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(2,4), RGN_FRAC(1,4), RGN_FRAC(3,4) },
	{ 0,1,2,3,4,5,6,7,0x8000*8+0, 0x8000*8+1, 0x8000*8+2, 0x8000*8+3, 0x8000*8+4, 0x8000*8+5, 0x8000*8+6, 0x8000*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 0x4000*8+0*8, 0x4000*8+1*8, 0x4000*8+2*8, 0x4000*8+3*8, 0x4000*8+4*8, 0x4000*8+5*8, 0x4000*8+6*8, 0x4000*8+7*8 },
	8*8
};
#endif
#if 0
static const gfx_layout automat_tilelayout2 =
{
	16,16,
	0x400, // RGN_FRAC(1,16) causes divide by zero?!
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(2,4), RGN_FRAC(1,4), RGN_FRAC(3,4) },
	{ 0,1,2,3,4,5,6,7, 0x4000*8+0, 0x4000*8+1, 0x4000*8+2, 0x4000*8+3, 0x4000*8+4, 0x4000*8+5, 0x4000*8+6, 0x4000*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 0x2000*8+0*8, 0x2000*8+1*8, 0x2000*8+2*8, 0x2000*8+3*8, 0x2000*8+4*8, 0x2000*8+5*8, 0x2000*8+6*8, 0x2000*8+7*8 },
	8*8
};
#endif

static GFXDECODE_START( dec0 )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,   0, 16 )	/* Characters 8x8 */
	GFXDECODE_ENTRY( "gfx2", 0, tilelayout, 512, 16 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx3", 0, tilelayout, 768, 16 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx4", 0, tilelayout, 256, 16 )	/* Sprites 16x16 */
GFXDECODE_END

#if 0
static GFXDECODE_START( automat )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,   0, 16 )	/* Characters 8x8 */
	GFXDECODE_ENTRY( "gfx2", 0, automat_tilelayout3, 512, 16 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx3", 0, automat_tilelayout2, 768, 16 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx4", 0, automat_tilelayout, 256, 16 )	/* Sprites 16x16 */
GFXDECODE_END
#endif

static GFXDECODE_START( midres )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 256, 16 )	/* Characters 8x8 */
	GFXDECODE_ENTRY( "gfx2", 0, tilelayout, 512, 16 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx3", 0, tilelayout, 768, 16 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx4", 0, tilelayout,   0, 16 )	/* Sprites 16x16 */
GFXDECODE_END

/* TODO: clean up this. */
#if 0
static const gfx_layout secretab_charlayout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};
#endif
#if 0
static const gfx_layout secretab_tilelayout =
{
	16,16,
	0x400, // RGN_FRAC(1,16) causes divide by zero?!
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0,1,2,3,4,5,6,7, 0x4000*8+0, 0x4000*8+1, 0x4000*8+2, 0x4000*8+3, 0x4000*8+4, 0x4000*8+5, 0x4000*8+6, 0x4000*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 0x2000*8+0*8, 0x2000*8+1*8, 0x2000*8+2*8, 0x2000*8+3*8, 0x2000*8+4*8, 0x2000*8+5*8, 0x2000*8+6*8, 0x2000*8+7*8 },
	8*8
};
#endif
#if 0
static const gfx_layout secretab_tilelayout2 =
{
	16,16,
	0x800,
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0,1,2,3,4,5,6,7,0x8000*8+0, 0x8000*8+1, 0x8000*8+2, 0x8000*8+3, 0x8000*8+4, 0x8000*8+5, 0x8000*8+6, 0x8000*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 0x4000*8+0*8, 0x4000*8+1*8, 0x4000*8+2*8, 0x4000*8+3*8, 0x4000*8+4*8, 0x4000*8+5*8, 0x4000*8+6*8, 0x4000*8+7*8 },
	8*8
};
#endif
#if 0
static const gfx_layout secretab_tilelayout3 =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4), RGN_FRAC(1,4), RGN_FRAC(2,4), RGN_FRAC(3,4) },
	{ 0,1,2,3,4,5,6,7,16*8+0, 16*8+1,16*8+2,16*8+3,16*8+4,16*8+5,16*8+6,16*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*16
};
#endif
#if 0
static GFXDECODE_START( secretab )
	GFXDECODE_ENTRY( "gfx1", 0, secretab_charlayout,     0x000, 0x10 )
	GFXDECODE_ENTRY( "gfx2", 0, secretab_tilelayout,     0x200, 0x10 )
	GFXDECODE_ENTRY( "gfx3", 0, secretab_tilelayout,     0x300, 0x10 )
	GFXDECODE_ENTRY( "gfx4", 0, secretab_tilelayout3,    0x100, 0x10 )
GFXDECODE_END
#endif
/******************************************************************************/

static void sound_irq(const device_config *device, int linestate)
{
	cputag_set_input_line(device->machine, "audiocpu", 0, linestate); /* IRQ */
}

static void sound_irq2(const device_config *device, int linestate)
{
	cputag_set_input_line(device->machine, "audiocpu", 1, linestate); /* IRQ2 */
}

static const ym3812_interface ym3812_config =
{
	sound_irq
};

static const ym3812_interface ym3812b_interface =
{
	sound_irq2
};

/******************************************************************************/
#if 0
static void automat_vclk_cb(const device_config *device)
{
	if (automat_msm5205_vclk_toggle == 0)
	{
		msm5205_data_w(device, automat_adpcm_byte & 0xf);
	}
	else
	{
		msm5205_data_w(device, automat_adpcm_byte >> 4);
		cputag_set_input_line(device->machine, "maincpu", INPUT_LINE_NMI, PULSE_LINE);
	}

	automat_msm5205_vclk_toggle ^= 1;
}
#endif
#if 0
static const msm5205_interface msm5205_config =
{
	automat_vclk_cb,
	MSM5205_S48_4B
};
#endif
#if 0
static MACHINE_DRIVER_START( automat )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(automat_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	MDRV_CPU_ADD("audiocpu", Z80, 3000000)// ?
	MDRV_CPU_PROGRAM_MAP(automat_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(automat)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(robocop)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("msm", MSM5205, 384000/2)
	MDRV_SOUND_CONFIG(msm5205_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END
#endif

static MACHINE_DRIVER_START( hbarrel )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(dec0_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL, level 5 interrupts from i8751 */

	MDRV_CPU_ADD("audiocpu", M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dec0_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(hbarrel)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( baddudes )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(dec0_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL, level 5 interrupts from i8751 */

	MDRV_CPU_ADD("audiocpu", M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dec0_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(baddudes)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END

#if 0
static MACHINE_DRIVER_START( birdtry )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(dec0_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL, level 5 interrupts from i8751 */

	MDRV_CPU_ADD("audiocpu", M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dec0_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(birdtry)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END
#endif

static MACHINE_DRIVER_START( robocop )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(dec0_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	MDRV_CPU_ADD("audiocpu", M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dec0_s_map)

	MDRV_CPU_ADD("sub", H6280,21477200/16) /* 21.4772MHz clock */
	MDRV_CPU_PROGRAM_MAP(robocop_sub_map)

	MDRV_QUANTUM_TIME(HZ(3000))	/* Interleave between HuC6280 & 68000 */

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(robocop)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END

#if 0
static MACHINE_DRIVER_START( robocopb )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(dec0_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	MDRV_CPU_ADD("audiocpu", M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dec0_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(robocop)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END
#endif

static MACHINE_DRIVER_START( hippodrm )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, 10000000)
	MDRV_CPU_PROGRAM_MAP(dec0_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	MDRV_CPU_ADD("audiocpu", M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dec0_s_map)

	MDRV_CPU_ADD("sub", H6280,21477200/16) /* 21.4772MHz clock */
	MDRV_CPU_PROGRAM_MAP(hippodrm_sub_map)

	MDRV_QUANTUM_TIME(HZ(300))	/* Interleave between H6280 & 68000 */

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0)
	MDRV_VIDEO_UPDATE(hippodrm)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 1500000)
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( slyspy )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, XTAL_20MHz/2) /* verified on pcb (20MHZ OSC) 68000P12 running at 10Mhz */
	MDRV_CPU_PROGRAM_MAP(slyspy_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	MDRV_CPU_ADD("audiocpu", H6280, XTAL_12MHz/2/3) /* verified on pcb (6Mhz is XIN on pin 10 of H6280, verified on pcb */
	MDRV_CPU_PROGRAM_MAP(slyspy_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(dec0)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0_nodma)
	MDRV_VIDEO_UPDATE(slyspy)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, XTAL_12MHz/8) /* verified on pcb */
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, XTAL_12MHz/4) /* verified on pcb */
	MDRV_SOUND_CONFIG(ym3812b_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, XTAL_12MHz/12) /* verified on pcb */
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) /* verified on pcb */
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END

#if 0
static MACHINE_DRIVER_START( secretab )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, XTAL_20MHz/2) /* verified on pcb (20MHZ OSC) 68000P12 running at 10Mhz */
	MDRV_CPU_PROGRAM_MAP(secretab_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	/* z80 */
//  MDRV_CPU_ADD("audiocpu", H6280, XTAL_12MHz/2/3) /* verified on pcb (6Mhz is XIN on pin 10 of H6280, verified on pcb */
//  MDRV_CPU_PROGRAM_MAP(slyspy_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(secretab)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0_nodma)
	MDRV_VIDEO_UPDATE(robocop)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, XTAL_12MHz/8) /* verified on pcb */
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, XTAL_12MHz/4) /* verified on pcb */
	MDRV_SOUND_CONFIG(ym3812b_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, XTAL_12MHz/12) /* verified on pcb */
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) /* verified on pcb */
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)
MACHINE_DRIVER_END
#endif

static MACHINE_DRIVER_START( midres )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M68000, XTAL_20MHz/2) /* verified on pcb (20MHZ OSC) 68000P12 running at 10Mhz */
	MDRV_CPU_PROGRAM_MAP(midres_map)
	MDRV_CPU_VBLANK_INT("screen", irq6_line_hold)/* VBL */

	MDRV_CPU_ADD("audiocpu", H6280, XTAL_24MHz/4/3) /* verified on pcb (6Mhz is XIN on pin 10 of H6280, verified on pcb */
	MDRV_CPU_PROGRAM_MAP(midres_s_map)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57.41)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529) /* 57.41 Hz, 529us Vblank */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(midres)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(dec0_nodma)
	MDRV_VIDEO_UPDATE(midres)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, XTAL_24MHz/16) /* verified on pcb */
	MDRV_SOUND_ROUTE(0, "mono", 0.90)
	MDRV_SOUND_ROUTE(1, "mono", 0.90)
	MDRV_SOUND_ROUTE(2, "mono", 0.90)
	MDRV_SOUND_ROUTE(3, "mono", 0.35)

	MDRV_SOUND_ADD("ym2", YM3812, XTAL_24MHz/8) /* verified on pcb */
	MDRV_SOUND_CONFIG(ym3812b_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MDRV_SOUND_ADD("oki", OKIM6295, XTAL_1MHz) /* verified on pcb (1mhz crystal) */
	MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // pin 7 verified on pcb
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.40)
MACHINE_DRIVER_END

/******************************************************************************/

ROM_START( hbarrel )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 6*64k for 68000 code */
	ROM_LOAD16_BYTE( "hb04.bin",     0x00000, 0x10000, CRC(4877b09e) SHA1(30c653b2f59fece881d088b675192ff2599adbe3) )
	ROM_LOAD16_BYTE( "hb01.bin",     0x00001, 0x10000, CRC(8b41c219) SHA1(5155095f459c29bd1fa5b3e8e2555db20a3bcfbc) )
	ROM_LOAD16_BYTE( "hb05.bin",     0x20000, 0x10000, CRC(2087d570) SHA1(625a33c2f4feed56f636d318531d0996cdee9194) )
	ROM_LOAD16_BYTE( "hb02.bin",     0x20001, 0x10000, CRC(815536ae) SHA1(684f67dc92f2a3bd77effce68c50e4013e054d31) )
	ROM_LOAD16_BYTE( "hb06.bin",     0x40000, 0x10000, CRC(da4e3fbc) SHA1(afc054eb5ee1d64d69fd8134d62e7c2d90f775c8) )
	ROM_LOAD16_BYTE( "hb03.bin",     0x40001, 0x10000, CRC(7fed7c46) SHA1(697742a18a0b01acadb0bbddc54331ab7e097bd8) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "hb07.bin",     0x8000, 0x8000, CRC(a127f0f7) SHA1(2cf962410936ac336e384dda2bf434a297bc940f) )

	ROM_REGION( 0x1000, "mcu", 0 )	/* i8751 microcontroller */
	ROM_LOAD( "i8751",     0x0000, 0x1000, NO_DUMP )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "hb25.bin",     0x00000, 0x10000, CRC(8649762c) SHA1(84d3d82d4d011c54271ef7a0dc5857a34b61cf8a) )
	ROM_LOAD( "hb26.bin",     0x10000, 0x10000, CRC(f8189bbd) SHA1(b4445f50e8771af6ba4fcbc34018f6ecd379779a) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "hb18.bin",     0x00000, 0x10000, CRC(ef664373) SHA1(d66a8c685c44cc8583527297d7ea7778f0d9c8db) )
	ROM_LOAD( "hb17.bin",     0x10000, 0x10000, CRC(a4f186ac) SHA1(ee422f8479c1f21bb62d040567a9748b646e6f9f) )
	ROM_LOAD( "hb20.bin",     0x20000, 0x10000, CRC(2fc13be0) SHA1(cce46b91104c0ac4038e98131fe957e0ed2f1a88) )
	ROM_LOAD( "hb19.bin",     0x30000, 0x10000, CRC(d6b47869) SHA1(eaef6ed5505395b1b829d6a126363031ad4e851a) )
	ROM_LOAD( "hb22.bin",     0x40000, 0x10000, CRC(50d6a1ad) SHA1(e7b464f34d6f3796823de6fdcbfd79416f71a119) )
	ROM_LOAD( "hb21.bin",     0x50000, 0x10000, CRC(f01d75c5) SHA1(959f9e2461db5f08b7ab12cc3b43f33be69318c9) )
	ROM_LOAD( "hb24.bin",     0x60000, 0x10000, CRC(ae377361) SHA1(a9aa520044f5b5037a495402ef128d3d8522b20f) )
	ROM_LOAD( "hb23.bin",     0x70000, 0x10000, CRC(bbdaf771) SHA1(7b29d6d606319337562b0431b6290df15cde17e2) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "hb29.bin",     0x00000, 0x10000, CRC(5514b296) SHA1(d258134a95bb223db139780b8e7377cccbe01af0) )
	ROM_LOAD( "hb30.bin",     0x10000, 0x10000, CRC(5855e8ef) SHA1(0f09143fed7c354231a4f343d0371424d8436877) )
	ROM_LOAD( "hb27.bin",     0x20000, 0x10000, CRC(99db7b9c) SHA1(2faeb287d685c8ea72c21658777f62ff9e194a69) )
	ROM_LOAD( "hb28.bin",     0x30000, 0x10000, CRC(33ce2b1a) SHA1(ef150dd5bc22368857ba27da18a17c161bb807a4) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "hb15.bin",     0x00000, 0x10000, CRC(21816707) SHA1(859a70dfc7d8c01124a035dcd5ea554af5f4e871) )
	ROM_LOAD( "hb16.bin",     0x10000, 0x10000, CRC(a5684574) SHA1(2dfe429cd6e110645ab976dd3a2b27d54ad91e89) )
	ROM_LOAD( "hb11.bin",     0x20000, 0x10000, CRC(5c768315) SHA1(00905e59dec90bf51f1d8e2482f54ede0895d142) )
	ROM_LOAD( "hb12.bin",     0x30000, 0x10000, CRC(8b64d7a4) SHA1(4d880d97a8eabd9b0a50cba3357df4f70afdf909) )
	ROM_LOAD( "hb13.bin",     0x40000, 0x10000, CRC(56e3ed65) SHA1(e7e4a53a7a18c81af8e395a33bcd82a41482c0da) )
	ROM_LOAD( "hb14.bin",     0x50000, 0x10000, CRC(bedfe7f3) SHA1(9db9c632fbf5a98d2d21bb960cc7111f6f9410fc) )
	ROM_LOAD( "hb09.bin",     0x60000, 0x10000, CRC(26240ea0) SHA1(25732986d787afd99a045ce4587f1079f84e675b) )
	ROM_LOAD( "hb10.bin",     0x70000, 0x10000, CRC(47d95447) SHA1(d2ffe96a19cfcbddee0df07dad89bd83cba801fa) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "hb08.bin",     0x0000, 0x10000, CRC(645c5b68) SHA1(096ca5d7b5df752df6d2c856b3f94b29eea7c3de) )
ROM_END

#if 0
ROM_START( hbarrelw )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 6*64k for 68000 code */
	ROM_LOAD16_BYTE( "hb_ec04.rom",  0x00000, 0x10000, CRC(d01bc3db) SHA1(53c9b78ce12ab577111fd96ef793b0fc4131bec3) )
	ROM_LOAD16_BYTE( "hb_ec01.rom",  0x00001, 0x10000, CRC(6756f8ae) SHA1(4edea085dedab46995b07d134b0974e365c32bfe) )
	ROM_LOAD16_BYTE( "hb05.bin",     0x20000, 0x10000, CRC(2087d570) SHA1(625a33c2f4feed56f636d318531d0996cdee9194) )
	ROM_LOAD16_BYTE( "hb02.bin",     0x20001, 0x10000, CRC(815536ae) SHA1(684f67dc92f2a3bd77effce68c50e4013e054d31) )
	ROM_LOAD16_BYTE( "hb_ec06.rom",  0x40000, 0x10000, CRC(61ec20d8) SHA1(9cd87fb896e746dc7745c59396cf5b06a9c6fae1) )
	ROM_LOAD16_BYTE( "hb_ec03.rom",  0x40001, 0x10000, CRC(720c6b13) SHA1(2af04de911f759b20ecec3aaf96238545c6cc987) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "hb_ec07.rom",  0x8000, 0x8000, CRC(16a5a1aa) SHA1(27eb8c09be6b1be502bda9ae9c9ff860d2560d46) )

	ROM_REGION( 0x1000, "mcu", 0 )	/* i8751 microcontroller */
	ROM_LOAD( "i8751",     0x0000, 0x1000, NO_DUMP )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "hb_ec25.rom",  0x00000, 0x10000, CRC(2e5732a2) SHA1(b730ce11db1876b81d2b7cde0f129bd6fbfeb771) )
	ROM_LOAD( "hb_ec26.rom",  0x10000, 0x10000, CRC(161a2c4d) SHA1(fbfa97ecc8b4d540d38f811ebb6272b348ed37e5) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "hb18.bin",     0x00000, 0x10000, CRC(ef664373) SHA1(d66a8c685c44cc8583527297d7ea7778f0d9c8db) )
	ROM_LOAD( "hb17.bin",     0x10000, 0x10000, CRC(a4f186ac) SHA1(ee422f8479c1f21bb62d040567a9748b646e6f9f) )
	ROM_LOAD( "hb20.bin",     0x20000, 0x10000, CRC(2fc13be0) SHA1(cce46b91104c0ac4038e98131fe957e0ed2f1a88) )
	ROM_LOAD( "hb19.bin",     0x30000, 0x10000, CRC(d6b47869) SHA1(eaef6ed5505395b1b829d6a126363031ad4e851a) )
	ROM_LOAD( "hb22.bin",     0x40000, 0x10000, CRC(50d6a1ad) SHA1(e7b464f34d6f3796823de6fdcbfd79416f71a119) )
	ROM_LOAD( "hb21.bin",     0x50000, 0x10000, CRC(f01d75c5) SHA1(959f9e2461db5f08b7ab12cc3b43f33be69318c9) )
	ROM_LOAD( "hb24.bin",     0x60000, 0x10000, CRC(ae377361) SHA1(a9aa520044f5b5037a495402ef128d3d8522b20f) )
	ROM_LOAD( "hb23.bin",     0x70000, 0x10000, CRC(bbdaf771) SHA1(7b29d6d606319337562b0431b6290df15cde17e2) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "hb29.bin",     0x00000, 0x10000, CRC(5514b296) SHA1(d258134a95bb223db139780b8e7377cccbe01af0) )
	ROM_LOAD( "hb30.bin",     0x10000, 0x10000, CRC(5855e8ef) SHA1(0f09143fed7c354231a4f343d0371424d8436877) )
	ROM_LOAD( "hb27.bin",     0x20000, 0x10000, CRC(99db7b9c) SHA1(2faeb287d685c8ea72c21658777f62ff9e194a69) )
	ROM_LOAD( "hb28.bin",     0x30000, 0x10000, CRC(33ce2b1a) SHA1(ef150dd5bc22368857ba27da18a17c161bb807a4) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "hb15.bin",     0x00000, 0x10000, CRC(21816707) SHA1(859a70dfc7d8c01124a035dcd5ea554af5f4e871) )
	ROM_LOAD( "hb16.bin",     0x10000, 0x10000, CRC(a5684574) SHA1(2dfe429cd6e110645ab976dd3a2b27d54ad91e89) )
	ROM_LOAD( "hb11.bin",     0x20000, 0x10000, CRC(5c768315) SHA1(00905e59dec90bf51f1d8e2482f54ede0895d142) )
	ROM_LOAD( "hb12.bin",     0x30000, 0x10000, CRC(8b64d7a4) SHA1(4d880d97a8eabd9b0a50cba3357df4f70afdf909) )
	ROM_LOAD( "hb13.bin",     0x40000, 0x10000, CRC(56e3ed65) SHA1(e7e4a53a7a18c81af8e395a33bcd82a41482c0da) )
	ROM_LOAD( "hb14.bin",     0x50000, 0x10000, CRC(bedfe7f3) SHA1(9db9c632fbf5a98d2d21bb960cc7111f6f9410fc) )
	ROM_LOAD( "hb09.bin",     0x60000, 0x10000, CRC(26240ea0) SHA1(25732986d787afd99a045ce4587f1079f84e675b) )
	ROM_LOAD( "hb10.bin",     0x70000, 0x10000, CRC(47d95447) SHA1(d2ffe96a19cfcbddee0df07dad89bd83cba801fa) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "hb_ec08.rom",  0x0000, 0x10000, CRC(2159a609) SHA1(cae503e446c7164a44b59886680f554a4cb1eef2) )
ROM_END
#endif

ROM_START( baddudes )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 6*64k for 68000 code, middle 0x20000 unused */
	ROM_LOAD16_BYTE( "baddudes.4",   0x00000, 0x10000, CRC(4bf158a7) SHA1(e034f64cec3e8596a2d86dd83462592178f19611) )
	ROM_LOAD16_BYTE( "baddudes.1",   0x00001, 0x10000, CRC(74f5110c) SHA1(9b8ff24e69505846a1406f5ab82b855b84a5cdf2) )
	ROM_LOAD16_BYTE( "baddudes.6",   0x40000, 0x10000, CRC(3ff8da57) SHA1(eea8125a3eac33d76d22e72b69633eaae138efe5) )
	ROM_LOAD16_BYTE( "baddudes.3",   0x40001, 0x10000, CRC(f8f2bd94) SHA1(622c66fea00cabb2cce16bf621b07d38a660708d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "baddudes.7",   0x8000, 0x8000, CRC(9fb1ef4b) SHA1(f4dd0773be93c2ad8b0faacd12939c531b5aa130) )

	ROM_REGION( 0x1000, "mcu", 0 )	/* i8751 microcontroller */
	ROM_LOAD( "i8751",     0x0000, 0x1000, NO_DUMP )

	ROM_REGION( 0x10000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "baddudes.25",  0x00000, 0x08000, CRC(bcf59a69) SHA1(486727e19c12ea55b47e2ef773d0d0471cf50083) )
	ROM_LOAD( "baddudes.26",  0x08000, 0x08000, CRC(9aff67b8) SHA1(18c3972a9f17a48897463f48be0d723ea0cf5aba) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "baddudes.18",  0x00000, 0x10000, CRC(05cfc3e5) SHA1(a0163921c77dc9706463a402c3dd45ec4341cd21) )
	ROM_LOAD( "baddudes.20",  0x10000, 0x10000, CRC(e11e988f) SHA1(0c59f0d8d1abe414c7e1ebd49d454179fed2cd00) )
	ROM_LOAD( "baddudes.22",  0x20000, 0x10000, CRC(b893d880) SHA1(99e228174677f2e3e96154f77bfa9bf0f1c0a6a5) )
	ROM_LOAD( "baddudes.24",  0x30000, 0x10000, CRC(6f226dda) SHA1(65ebb16a292c57d49c135fce7ed7537146226eb5) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "baddudes.30",  0x08000, 0x08000, CRC(982da0d1) SHA1(d819a587905624d793988f2ea726783da527d9f2) )
	ROM_CONTINUE(             0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "baddudes.28",  0x18000, 0x08000, CRC(f01ebb3b) SHA1(1686690cb0c87d9e687b2abb4896cf285ab8378f) )
	ROM_CONTINUE(             0x10000, 0x08000 )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "baddudes.15",  0x00000, 0x10000, CRC(a38a7d30) SHA1(5cb1fb97605829fc733c79a7e169fa52adc6863b) )
	ROM_LOAD( "baddudes.16",  0x10000, 0x08000, CRC(17e42633) SHA1(405f5296a741901677cca978a1b287d894eb1e54) )
	ROM_LOAD( "baddudes.11",  0x20000, 0x10000, CRC(3a77326c) SHA1(4de81752329cde6210a9c250a9f8ebe3dad9fe92) )
	ROM_LOAD( "baddudes.12",  0x30000, 0x08000, CRC(fea2a134) SHA1(525dd5f48993db1fe1e3c095442884178f75e8e0) )
	ROM_LOAD( "baddudes.13",  0x40000, 0x10000, CRC(e5ae2751) SHA1(4e4a3c68b11e9b0c8da70121b23296128063d4e9) )
	ROM_LOAD( "baddudes.14",  0x50000, 0x08000, CRC(e83c760a) SHA1(d08db381658b8b3288c5eaa9048a906126e0f712) )
	ROM_LOAD( "baddudes.9",   0x60000, 0x10000, CRC(6901e628) SHA1(1162c8cee20450780774cad54a9af40ebf0f0826) )
	ROM_LOAD( "baddudes.10",  0x70000, 0x08000, CRC(eeee8a1a) SHA1(2bf8378ff38f6a7c7cbd4cbd489de25cb1f0fe71) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "baddudes.8",   0x0000, 0x10000, CRC(3c87463e) SHA1(f17c98507b562e91e9b27599614b3249fe68ff7a) )
ROM_END

#if 0
ROM_START( drgninja )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 6*64k for 68000 code, middle 0x20000 unused */
	ROM_LOAD16_BYTE( "drgninja.04",  0x00000, 0x10000, CRC(41b8b3f8) SHA1(0ab143b9f7a5f857cfd2053c24fa5213ce7641e4) )
	ROM_LOAD16_BYTE( "drgninja.01",  0x00001, 0x10000, CRC(e08e6885) SHA1(641eaf4ef6c8bfbc39611f5f81765f7915ae9d9f) )
	ROM_LOAD16_BYTE( "drgninja.06",  0x40000, 0x10000, CRC(2b81faf7) SHA1(6d10c29f5ee06856843d83e77ba24c2b6e00a9cb) )
	ROM_LOAD16_BYTE( "drgninja.03",  0x40001, 0x10000, CRC(c52c2e9d) SHA1(399f2b7df9d558c8f33bf1a7c8048c62e0f54cec) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "drgninja.07",  0x8000, 0x8000, CRC(001d2f51) SHA1(f186671f0450ccf9201577a5caf0efc490c6645e) )

	ROM_REGION( 0x1000, "mcu", 0 )	/* i8751 microcontroller */
	ROM_LOAD( "i8751",     0x0000, 0x1000, NO_DUMP )

	ROM_REGION( 0x10000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "drgninja.25",  0x00000, 0x08000, CRC(6791bc20) SHA1(7240b2688cda04ee9ea331472a84fbffc85b8e90) )
	ROM_LOAD( "drgninja.26",  0x08000, 0x08000, CRC(5d75fc8f) SHA1(92947dd78bfe8067fb5f645fa1ef212e48b69c70) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "baddudes.18",  0x00000, 0x10000, CRC(05cfc3e5) SHA1(a0163921c77dc9706463a402c3dd45ec4341cd21) )
	ROM_LOAD( "baddudes.20",  0x10000, 0x10000, CRC(e11e988f) SHA1(0c59f0d8d1abe414c7e1ebd49d454179fed2cd00) )
	ROM_LOAD( "baddudes.22",  0x20000, 0x10000, CRC(b893d880) SHA1(99e228174677f2e3e96154f77bfa9bf0f1c0a6a5) )
	ROM_LOAD( "baddudes.24",  0x30000, 0x10000, CRC(6f226dda) SHA1(65ebb16a292c57d49c135fce7ed7537146226eb5) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "drgninja.30",  0x08000, 0x08000, CRC(2438e67e) SHA1(5f143aeb83606a2c64d0b31bfee38156d231dcc9) )
	ROM_CONTINUE(             0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "drgninja.28",  0x18000, 0x08000, CRC(5c692ab3) SHA1(4c58ff50833f869575f1a15c776fbf1429944fab) )
	ROM_CONTINUE(             0x10000, 0x08000 )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "drgninja.15",  0x00000, 0x10000, CRC(5617d67f) SHA1(8f684de27ae79c4d35720706cdd2733af0e0a9cc) )
	ROM_LOAD( "baddudes.16",  0x10000, 0x08000, CRC(17e42633) SHA1(405f5296a741901677cca978a1b287d894eb1e54) )
	ROM_LOAD( "drgninja.11",  0x20000, 0x10000, CRC(ba83e8d8) SHA1(63092a5d0da0c9228a72a83b43a67a47b1388724) )
	ROM_LOAD( "baddudes.12",  0x30000, 0x08000, CRC(fea2a134) SHA1(525dd5f48993db1fe1e3c095442884178f75e8e0) )
	ROM_LOAD( "drgninja.13",  0x40000, 0x10000, CRC(fd91e08e) SHA1(8998f020791c8830e963096dc7b8fcb430d041d4) )
	ROM_LOAD( "baddudes.14",  0x50000, 0x08000, CRC(e83c760a) SHA1(d08db381658b8b3288c5eaa9048a906126e0f712) )
	ROM_LOAD( "baddudes.9",   0x60000, 0x10000, CRC(6901e628) SHA1(1162c8cee20450780774cad54a9af40ebf0f0826) )
	ROM_LOAD( "baddudes.10",  0x70000, 0x08000, CRC(eeee8a1a) SHA1(2bf8378ff38f6a7c7cbd4cbd489de25cb1f0fe71) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "baddudes.8",   0x0000, 0x10000, CRC(3c87463e) SHA1(f17c98507b562e91e9b27599614b3249fe68ff7a) )
ROM_END
#endif
#if 0
ROM_START( birdtry )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 6*64k for 68000 code */
	ROM_LOAD16_BYTE( "ek-04.bin",     0x00000, 0x10000, CRC(5f0f4686) SHA1(5eea74f5626339ebd50e623029f21f1cd0f93135) )
	ROM_LOAD16_BYTE( "ek-01.bin",     0x00001, 0x10000, CRC(47f470db) SHA1(8fcb043d02e1c04c8517781715da4dd4ee3bb8fb) )
	ROM_LOAD16_BYTE( "ek-05.bin",     0x20000, 0x10000, CRC(b508cffd) SHA1(c1861a2420d99e19d889881f9164fe4ff667a1be) )
	ROM_LOAD16_BYTE( "ek-02.bin",     0x20001, 0x10000, CRC(0195d989) SHA1(cff48d57b2085263e12413ae19757cdcc7028282) )
	ROM_LOAD16_BYTE( "ek-06.bin",     0x40000, 0x10000, CRC(301d57d8) SHA1(64fd77aa2fbb235c86f0f84603e5272b4f4bba85) )
	ROM_LOAD16_BYTE( "ek-03.bin",     0x40001, 0x10000, CRC(73b0acc5) SHA1(76b79c9f02de2e53093ded66a1639b40cd2640e8) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ek-07.bin",     0x8000, 0x8000, CRC(236549bc) SHA1(1f664a277b3451b7905638abdf98c7e428b2e935) )

	ROM_REGION( 0x1000, "mcu", 0 )	/* i8751 microcontroller */
	ROM_LOAD( "i8751",     0x0000, 0x1000, NO_DUMP )

	ROM_REGION( 0x10000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ek-25.bin",     0x00000, 0x08000, CRC(4df134ad) SHA1(f2cfa7e3fc4a2ac40897c2600c901ff75237e081) )
	ROM_LOAD( "ek-26.bin",     0x08000, 0x08000, CRC(a00d3e8e) SHA1(3ac8511d55a684a5b2bc05d8d520169447a66840) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ek-18.bin",     0x00000, 0x10000, CRC(9886fb70) SHA1(d36c41bfe217affab7f9deec64ff3f12e3efa28c) )
	ROM_LOAD( "ek-17.bin",     0x10000, 0x10000, CRC(bed91bf7) SHA1(f0ffc557a4c216a5a2e180b4c2366e7b49630064) )
	ROM_LOAD( "ek-20.bin",     0x20000, 0x10000, CRC(45d53965) SHA1(d54d33cc82e099bcb511de8ee26cdcc64a0b8f1d) )
	ROM_LOAD( "ek-19.bin",     0x30000, 0x10000, CRC(c2949dd2) SHA1(d4317f8e0d9957feda54ee6d05aafb3f74f243d1) )
	ROM_LOAD( "ek-22.bin",     0x40000, 0x10000, CRC(7f2cc80a) SHA1(f2539515fcf0b6dc90134d399baf779c50b19c0d) )
	ROM_LOAD( "ek-21.bin",     0x50000, 0x10000, CRC(281bc793) SHA1(836fc2900b7197c886c23d9eeb1a80aed85c4d13) )
	ROM_LOAD( "ek-24.bin",     0x60000, 0x10000, CRC(2244cc75) SHA1(67c9868927319abe80a932203e8ac6595ae455b3) )
	ROM_LOAD( "ek-23.bin",     0x70000, 0x10000, CRC(d0ed0116) SHA1(a35e64ecac57585b83e830a1bf90a402c931f071) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASEFF ) /* tiles */
	/* This game doesn't have the extra playfield chip, so no roms */

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ek-15.bin",     0x00000, 0x10000, CRC(a6a041a3) SHA1(3b8d18d5821e6d354ed97a4f547f1b2bee8674f5) )
	ROM_LOAD( "ek-16.bin",     0x10000, 0x08000, CRC(784f62b0) SHA1(b68b234a5f469149d481645290a3251667bdab27) )
	ROM_LOAD( "ek-11.bin",     0x20000, 0x10000, CRC(9224a6b9) SHA1(547c22db1728a85035a682eb54ce654a98a4ba3d) )
	ROM_LOAD( "ek-12.bin",     0x30000, 0x08000, CRC(12deecfa) SHA1(22e33ccc6623957533028f720e9a746f36217ded) )
	ROM_LOAD( "ek-13.bin",     0x40000, 0x10000, CRC(1f023459) SHA1(e502edb4078168df4677a6d3aa43770eb8e49caa) )
	ROM_LOAD( "ek-14.bin",     0x50000, 0x08000, CRC(57d54943) SHA1(9639fad61919652c1564b24926845d228d016ca0) )
	ROM_LOAD( "ek-09.bin",     0x60000, 0x10000, CRC(6d2d488a) SHA1(40b21a4bc8a4641a6f80d7579e32fe9d69eb42f1) )
	ROM_LOAD( "ek-10.bin",     0x70000, 0x08000, CRC(580ba206) SHA1(8e57e4ef8c732b85e494bd6ec5da6566f27540e6) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ek-08.bin",     0x0000, 0x10000, CRC(be3db6cb) SHA1(4e8b8e0bef3a3f36d7e641e27b5f48c8fe9a8b7f) )
ROM_END
#endif

ROM_START( robocop )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ep05-4.11c", 0x00000, 0x10000, CRC(29c35379) SHA1(a352c2d0dff843c1e0b5cade506a8b33c2d781f1) )
	ROM_LOAD16_BYTE( "ep01-4.11b", 0x00001, 0x10000, CRC(77507c69) SHA1(843b678b4a297d6d99ea7d797dedde33e5003119) )
	ROM_LOAD16_BYTE( "ep04-3", 0x20000, 0x10000, CRC(39181778) SHA1(f91b63e541ef547d34d144c80bc0344b6acf8de0) )
	ROM_LOAD16_BYTE( "ep00-3", 0x20001, 0x10000, CRC(e128541f) SHA1(c123b6ba282b552890319d97348015361264fa3b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ep03-3", 0x08000, 0x08000, CRC(5b164b24) SHA1(b217a2ac8b26aebd208631a13030487ed27d232e) )

	ROM_REGION( 0x200000, "sub", 0 )	/* HuC6280 CPU */
	ROM_LOAD( "en_24_mb7124e.a2", 0x01e00, 0x0200, CRC(b8e2ca98) SHA1(bd1e193c544dc17a665aa6c4d3b844775ed08b43) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ep23", 0x00000, 0x10000, CRC(a77e4ab1) SHA1(d06cc847192b6c7f642e4ff7128e298d0aa034b2) )
	ROM_LOAD( "ep22", 0x10000, 0x10000, CRC(9fbd6903) SHA1(9ac6ac8a18c23e915e8ae3782867d10c0bd65778) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ep20", 0x00000, 0x10000, CRC(1d8d38b8) SHA1(9add6349f8a578fb86b678cef921d6ec0cfccdad) )
	ROM_LOAD( "ep21", 0x10000, 0x10000, CRC(187929b2) SHA1(deca1f0a52584769caee1d2302617aa957c56a71) )
	ROM_LOAD( "ep18", 0x20000, 0x10000, CRC(b6580b5e) SHA1(ee216d8db89b8cb7a51a4e19bf6f17788547156b) )
	ROM_LOAD( "ep19", 0x30000, 0x10000, CRC(9bad01c7) SHA1(947c7f9d0facaea13a924274adde0e996be7b999) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ep14", 0x00000, 0x08000, CRC(ca56ceda) SHA1(edbaa29fc166cddf071ff5e59cfcfb7eeb127d68) )
	ROM_LOAD( "ep15", 0x08000, 0x08000, CRC(a945269c) SHA1(de0b387e8699298f7682d6d7ca803a209888f7a1) )
	ROM_LOAD( "ep16", 0x10000, 0x08000, CRC(e7fa4d58) SHA1(32e3f649b4f112a4e6be00068473b82c627bc8d1) )
	ROM_LOAD( "ep17", 0x18000, 0x08000, CRC(84aae89d) SHA1(037520bd0f291f862c2211a6f35b2a8a54f10b2a) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ep07", 0x00000, 0x10000, CRC(495d75cf) SHA1(0ffe677d53b7675073902e9bd40e4150f2cdfb1a) )
	ROM_LOAD( "ep06", 0x10000, 0x08000, CRC(a2ae32e2) SHA1(4e8182205563da9d50a831c65951645e278b03e6) )
	ROM_LOAD( "ep11", 0x20000, 0x10000, CRC(62fa425a) SHA1(be88c1a6436df8a456c405822e28c472e3e79a69) )
	ROM_LOAD( "ep10", 0x30000, 0x08000, CRC(cce3bd95) SHA1(00bbb197824d970b0e404167ca4ae53e1955ad94) )
	ROM_LOAD( "ep09", 0x40000, 0x10000, CRC(11bed656) SHA1(6a7d984a32982d9aef8ea7d8a720925036e7046e) )
	ROM_LOAD( "ep08", 0x50000, 0x08000, CRC(c45c7b4c) SHA1(70e3e475fe767eefa4cc1d6ca052271a099ff7a8) )
	ROM_LOAD( "ep13", 0x60000, 0x10000, CRC(8fca9f28) SHA1(cac85bf2b66e49e22c33c85bdb5712feef6aae7e) )
	ROM_LOAD( "ep12", 0x70000, 0x08000, CRC(3cd1d0c3) SHA1(ca3546cf51ebb10dfa4e78954f0212e8fcdb3d57) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ep02", 0x00000, 0x10000, CRC(711ce46f) SHA1(939a8545e53776ff2180d2c7e63bc997689c088e) )

/*
MB7116E at 12C on CPU board - Removing this causes all sprites to disappear

MB7124E at 12A on ROM Board (although the board
is screened as MB7130) - Removing this causes the
display to disappear, although coining up makes
the correct text appear (and sound to play),
intro graphics appear on starting a game, then
nothing, it won't even play blind (although the
coin up sound still plays on coin insert)
*/
	ROM_REGION( 0xc00, "proms", 0 )	/* PROMs */
	ROM_LOAD( "mb7116e.12c", 0x000, 0x400, CRC(c288a256) SHA1(cb6813507cfa6fa0b74f4692247fd1d4466c659b) )
	ROM_LOAD( "mb7122e.17e", 0x400, 0x800, CRC(64764ecf) SHA1(9385a8a2fae8b8f6310194331a77d97eca7917bd) )
ROM_END

#if 0
ROM_START( robocopw )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ep05-3", 0x00000, 0x10000, CRC(ba69bf84) SHA1(a9d4d94d1b936d43a610cfe02cc03bdeddb81ac6) )
	ROM_LOAD16_BYTE( "ep01-3", 0x00001, 0x10000, CRC(2a9f6e2c) SHA1(74aeb5be36619d90034d4a8139c3d043fe8d33c2) )
	ROM_LOAD16_BYTE( "ep04-3", 0x20000, 0x10000, CRC(39181778) SHA1(f91b63e541ef547d34d144c80bc0344b6acf8de0) )
	ROM_LOAD16_BYTE( "ep00-3", 0x20001, 0x10000, CRC(e128541f) SHA1(c123b6ba282b552890319d97348015361264fa3b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ep03-3", 0x08000, 0x08000, CRC(5b164b24) SHA1(b217a2ac8b26aebd208631a13030487ed27d232e) )

	ROM_REGION( 0x200000, "sub", 0 )	/* HuC6280 CPU */
	ROM_LOAD( "en_24.a2", 0x01e00, 0x0200, CRC(b8e2ca98) SHA1(bd1e193c544dc17a665aa6c4d3b844775ed08b43) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ep23", 0x00000, 0x10000, CRC(a77e4ab1) SHA1(d06cc847192b6c7f642e4ff7128e298d0aa034b2) )
	ROM_LOAD( "ep22", 0x10000, 0x10000, CRC(9fbd6903) SHA1(9ac6ac8a18c23e915e8ae3782867d10c0bd65778) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ep20", 0x00000, 0x10000, CRC(1d8d38b8) SHA1(9add6349f8a578fb86b678cef921d6ec0cfccdad) )
	ROM_LOAD( "ep21", 0x10000, 0x10000, CRC(187929b2) SHA1(deca1f0a52584769caee1d2302617aa957c56a71) )
	ROM_LOAD( "ep18", 0x20000, 0x10000, CRC(b6580b5e) SHA1(ee216d8db89b8cb7a51a4e19bf6f17788547156b) )
	ROM_LOAD( "ep19", 0x30000, 0x10000, CRC(9bad01c7) SHA1(947c7f9d0facaea13a924274adde0e996be7b999) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ep14", 0x00000, 0x08000, CRC(ca56ceda) SHA1(edbaa29fc166cddf071ff5e59cfcfb7eeb127d68) )
	ROM_LOAD( "ep15", 0x08000, 0x08000, CRC(a945269c) SHA1(de0b387e8699298f7682d6d7ca803a209888f7a1) )
	ROM_LOAD( "ep16", 0x10000, 0x08000, CRC(e7fa4d58) SHA1(32e3f649b4f112a4e6be00068473b82c627bc8d1) )
	ROM_LOAD( "ep17", 0x18000, 0x08000, CRC(84aae89d) SHA1(037520bd0f291f862c2211a6f35b2a8a54f10b2a) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ep07", 0x00000, 0x10000, CRC(495d75cf) SHA1(0ffe677d53b7675073902e9bd40e4150f2cdfb1a) )
	ROM_LOAD( "ep06", 0x10000, 0x08000, CRC(a2ae32e2) SHA1(4e8182205563da9d50a831c65951645e278b03e6) )
	ROM_LOAD( "ep11", 0x20000, 0x10000, CRC(62fa425a) SHA1(be88c1a6436df8a456c405822e28c472e3e79a69) )
	ROM_LOAD( "ep10", 0x30000, 0x08000, CRC(cce3bd95) SHA1(00bbb197824d970b0e404167ca4ae53e1955ad94) )
	ROM_LOAD( "ep09", 0x40000, 0x10000, CRC(11bed656) SHA1(6a7d984a32982d9aef8ea7d8a720925036e7046e) )
	ROM_LOAD( "ep08", 0x50000, 0x08000, CRC(c45c7b4c) SHA1(70e3e475fe767eefa4cc1d6ca052271a099ff7a8) )
	ROM_LOAD( "ep13", 0x60000, 0x10000, CRC(8fca9f28) SHA1(cac85bf2b66e49e22c33c85bdb5712feef6aae7e) )
	ROM_LOAD( "ep12", 0x70000, 0x08000, CRC(3cd1d0c3) SHA1(ca3546cf51ebb10dfa4e78954f0212e8fcdb3d57) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ep02", 0x00000, 0x10000, CRC(711ce46f) SHA1(939a8545e53776ff2180d2c7e63bc997689c088e) )
ROM_END
#endif
#if 0
ROM_START( robocopj )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "em05-1.c11", 0x00000, 0x10000, CRC(954ea8f4) SHA1(2efc6c6bf856bcd86aca439bf85ec9a5c2f89612) )
	ROM_LOAD16_BYTE( "em01-1.b12", 0x00001, 0x10000, CRC(1b87b622) SHA1(4b17e6377a77e8529b038529c12fdb2bd8a5af25) )
	ROM_LOAD16_BYTE( "ep04-3", 0x20000, 0x10000, CRC(39181778) SHA1(f91b63e541ef547d34d144c80bc0344b6acf8de0) )
	ROM_LOAD16_BYTE( "ep00-3", 0x20001, 0x10000, CRC(e128541f) SHA1(c123b6ba282b552890319d97348015361264fa3b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ep03-3", 0x08000, 0x08000, CRC(5b164b24) SHA1(b217a2ac8b26aebd208631a13030487ed27d232e) )

	ROM_REGION( 0x200000, "sub", 0 )	/* HuC6280 CPU */
	ROM_LOAD( "en_24.a2", 0x01e00, 0x0200, CRC(b8e2ca98) SHA1(bd1e193c544dc17a665aa6c4d3b844775ed08b43) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ep23", 0x00000, 0x10000, CRC(a77e4ab1) SHA1(d06cc847192b6c7f642e4ff7128e298d0aa034b2) )
	ROM_LOAD( "ep22", 0x10000, 0x10000, CRC(9fbd6903) SHA1(9ac6ac8a18c23e915e8ae3782867d10c0bd65778) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ep20", 0x00000, 0x10000, CRC(1d8d38b8) SHA1(9add6349f8a578fb86b678cef921d6ec0cfccdad) )
	ROM_LOAD( "ep21", 0x10000, 0x10000, CRC(187929b2) SHA1(deca1f0a52584769caee1d2302617aa957c56a71) )
	ROM_LOAD( "ep18", 0x20000, 0x10000, CRC(b6580b5e) SHA1(ee216d8db89b8cb7a51a4e19bf6f17788547156b) )
	ROM_LOAD( "ep19", 0x30000, 0x10000, CRC(9bad01c7) SHA1(947c7f9d0facaea13a924274adde0e996be7b999) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ep14", 0x00000, 0x08000, CRC(ca56ceda) SHA1(edbaa29fc166cddf071ff5e59cfcfb7eeb127d68) )
	ROM_LOAD( "ep15", 0x08000, 0x08000, CRC(a945269c) SHA1(de0b387e8699298f7682d6d7ca803a209888f7a1) )
	ROM_LOAD( "ep16", 0x10000, 0x08000, CRC(e7fa4d58) SHA1(32e3f649b4f112a4e6be00068473b82c627bc8d1) )
	ROM_LOAD( "ep17", 0x18000, 0x08000, CRC(84aae89d) SHA1(037520bd0f291f862c2211a6f35b2a8a54f10b2a) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ep07", 0x00000, 0x10000, CRC(495d75cf) SHA1(0ffe677d53b7675073902e9bd40e4150f2cdfb1a) )
	ROM_LOAD( "ep06", 0x10000, 0x08000, CRC(a2ae32e2) SHA1(4e8182205563da9d50a831c65951645e278b03e6) )
	ROM_LOAD( "ep11", 0x20000, 0x10000, CRC(62fa425a) SHA1(be88c1a6436df8a456c405822e28c472e3e79a69) )
	ROM_LOAD( "ep10", 0x30000, 0x08000, CRC(cce3bd95) SHA1(00bbb197824d970b0e404167ca4ae53e1955ad94) )
	ROM_LOAD( "ep09", 0x40000, 0x10000, CRC(11bed656) SHA1(6a7d984a32982d9aef8ea7d8a720925036e7046e) )
	ROM_LOAD( "ep08", 0x50000, 0x08000, CRC(c45c7b4c) SHA1(70e3e475fe767eefa4cc1d6ca052271a099ff7a8) )
	ROM_LOAD( "ep13", 0x60000, 0x10000, CRC(8fca9f28) SHA1(cac85bf2b66e49e22c33c85bdb5712feef6aae7e) )
	ROM_LOAD( "ep12", 0x70000, 0x08000, CRC(3cd1d0c3) SHA1(ca3546cf51ebb10dfa4e78954f0212e8fcdb3d57) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ep02", 0x00000, 0x10000, CRC(711ce46f) SHA1(939a8545e53776ff2180d2c7e63bc997689c088e) )
ROM_END
#endif
#if 0
ROM_START( robocopu )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ep05-1", 0x00000, 0x10000, CRC(8de5cb3d) SHA1(66eb87aa11697d0abdd0c265aaa2048ca3c80c18) )
	ROM_LOAD16_BYTE( "ep01-1", 0x00001, 0x10000, CRC(b3c6bc02) SHA1(380dd241bebfdbdc93450b6cf562bccf8e3b8e27) )
	ROM_LOAD16_BYTE( "ep04", 0x20000, 0x10000, CRC(c38b9d18) SHA1(683bc4ce8dac62ab9ce79679ad44dc9542b814c8) )
	ROM_LOAD16_BYTE( "ep00", 0x20001, 0x10000, CRC(374c91aa) SHA1(d8bccc12278b754fe303eb75204b38126d401c3d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ep03", 0x08000, 0x08000, CRC(1089eab8) SHA1(088c570b12b681f6751d7ae48560726464bcb79e) )

	ROM_REGION( 0x200000, "sub", 0 )	/* HuC6280 CPU */
	ROM_LOAD( "en_24.a2", 0x01e00, 0x0200, CRC(b8e2ca98) SHA1(bd1e193c544dc17a665aa6c4d3b844775ed08b43) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ep23", 0x00000, 0x10000, CRC(a77e4ab1) SHA1(d06cc847192b6c7f642e4ff7128e298d0aa034b2) )
	ROM_LOAD( "ep22", 0x10000, 0x10000, CRC(9fbd6903) SHA1(9ac6ac8a18c23e915e8ae3782867d10c0bd65778) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ep20", 0x00000, 0x10000, CRC(1d8d38b8) SHA1(9add6349f8a578fb86b678cef921d6ec0cfccdad) )
	ROM_LOAD( "ep21", 0x10000, 0x10000, CRC(187929b2) SHA1(deca1f0a52584769caee1d2302617aa957c56a71) )
	ROM_LOAD( "ep18", 0x20000, 0x10000, CRC(b6580b5e) SHA1(ee216d8db89b8cb7a51a4e19bf6f17788547156b) )
	ROM_LOAD( "ep19", 0x30000, 0x10000, CRC(9bad01c7) SHA1(947c7f9d0facaea13a924274adde0e996be7b999) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ep14", 0x00000, 0x08000, CRC(ca56ceda) SHA1(edbaa29fc166cddf071ff5e59cfcfb7eeb127d68) )
	ROM_LOAD( "ep15", 0x08000, 0x08000, CRC(a945269c) SHA1(de0b387e8699298f7682d6d7ca803a209888f7a1) )
	ROM_LOAD( "ep16", 0x10000, 0x08000, CRC(e7fa4d58) SHA1(32e3f649b4f112a4e6be00068473b82c627bc8d1) )
	ROM_LOAD( "ep17", 0x18000, 0x08000, CRC(84aae89d) SHA1(037520bd0f291f862c2211a6f35b2a8a54f10b2a) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ep07", 0x00000, 0x10000, CRC(495d75cf) SHA1(0ffe677d53b7675073902e9bd40e4150f2cdfb1a) )
	ROM_LOAD( "ep06", 0x10000, 0x08000, CRC(a2ae32e2) SHA1(4e8182205563da9d50a831c65951645e278b03e6) )
	ROM_LOAD( "ep11", 0x20000, 0x10000, CRC(62fa425a) SHA1(be88c1a6436df8a456c405822e28c472e3e79a69) )
	ROM_LOAD( "ep10", 0x30000, 0x08000, CRC(cce3bd95) SHA1(00bbb197824d970b0e404167ca4ae53e1955ad94) )
	ROM_LOAD( "ep09", 0x40000, 0x10000, CRC(11bed656) SHA1(6a7d984a32982d9aef8ea7d8a720925036e7046e) )
	ROM_LOAD( "ep08", 0x50000, 0x08000, CRC(c45c7b4c) SHA1(70e3e475fe767eefa4cc1d6ca052271a099ff7a8) )
	ROM_LOAD( "ep13", 0x60000, 0x10000, CRC(8fca9f28) SHA1(cac85bf2b66e49e22c33c85bdb5712feef6aae7e) )
	ROM_LOAD( "ep12", 0x70000, 0x08000, CRC(3cd1d0c3) SHA1(ca3546cf51ebb10dfa4e78954f0212e8fcdb3d57) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ep02", 0x00000, 0x10000, CRC(711ce46f) SHA1(939a8545e53776ff2180d2c7e63bc997689c088e) )
ROM_END
#endif
#if 0
ROM_START( robocopu0 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "ep05", 0x00000, 0x10000, CRC(c465bdd8) SHA1(a1d5435ea2664ac38db84577b97ba74304e09473) )
	ROM_LOAD16_BYTE( "ep01", 0x00001, 0x10000, CRC(1352d36e) SHA1(7bfdce66020b6c9465b768bac2ba7c9fe458242e) )
	ROM_LOAD16_BYTE( "ep04", 0x20000, 0x10000, CRC(c38b9d18) SHA1(683bc4ce8dac62ab9ce79679ad44dc9542b814c8) )
	ROM_LOAD16_BYTE( "ep00", 0x20001, 0x10000, CRC(374c91aa) SHA1(d8bccc12278b754fe303eb75204b38126d401c3d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ep03", 0x08000, 0x08000, CRC(1089eab8) SHA1(088c570b12b681f6751d7ae48560726464bcb79e) )

	ROM_REGION( 0x200000, "sub", 0 )	/* HuC6280 CPU */
	ROM_LOAD( "en_24.a2", 0x01e00, 0x0200, CRC(b8e2ca98) SHA1(bd1e193c544dc17a665aa6c4d3b844775ed08b43) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ep23", 0x00000, 0x10000, CRC(a77e4ab1) SHA1(d06cc847192b6c7f642e4ff7128e298d0aa034b2) )
	ROM_LOAD( "ep22", 0x10000, 0x10000, CRC(9fbd6903) SHA1(9ac6ac8a18c23e915e8ae3782867d10c0bd65778) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ep20", 0x00000, 0x10000, CRC(1d8d38b8) SHA1(9add6349f8a578fb86b678cef921d6ec0cfccdad) )
	ROM_LOAD( "ep21", 0x10000, 0x10000, CRC(187929b2) SHA1(deca1f0a52584769caee1d2302617aa957c56a71) )
	ROM_LOAD( "ep18", 0x20000, 0x10000, CRC(b6580b5e) SHA1(ee216d8db89b8cb7a51a4e19bf6f17788547156b) )
	ROM_LOAD( "ep19", 0x30000, 0x10000, CRC(9bad01c7) SHA1(947c7f9d0facaea13a924274adde0e996be7b999) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ep14", 0x00000, 0x08000, CRC(ca56ceda) SHA1(edbaa29fc166cddf071ff5e59cfcfb7eeb127d68) )
	ROM_LOAD( "ep15", 0x08000, 0x08000, CRC(a945269c) SHA1(de0b387e8699298f7682d6d7ca803a209888f7a1) )
	ROM_LOAD( "ep16", 0x10000, 0x08000, CRC(e7fa4d58) SHA1(32e3f649b4f112a4e6be00068473b82c627bc8d1) )
	ROM_LOAD( "ep17", 0x18000, 0x08000, CRC(84aae89d) SHA1(037520bd0f291f862c2211a6f35b2a8a54f10b2a) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ep07", 0x00000, 0x10000, CRC(495d75cf) SHA1(0ffe677d53b7675073902e9bd40e4150f2cdfb1a) )
	ROM_LOAD( "ep06", 0x10000, 0x08000, CRC(a2ae32e2) SHA1(4e8182205563da9d50a831c65951645e278b03e6) )
	ROM_LOAD( "ep11", 0x20000, 0x10000, CRC(62fa425a) SHA1(be88c1a6436df8a456c405822e28c472e3e79a69) )
	ROM_LOAD( "ep10", 0x30000, 0x08000, CRC(cce3bd95) SHA1(00bbb197824d970b0e404167ca4ae53e1955ad94) )
	ROM_LOAD( "ep09", 0x40000, 0x10000, CRC(11bed656) SHA1(6a7d984a32982d9aef8ea7d8a720925036e7046e) )
	ROM_LOAD( "ep08", 0x50000, 0x08000, CRC(c45c7b4c) SHA1(70e3e475fe767eefa4cc1d6ca052271a099ff7a8) )
	ROM_LOAD( "ep13", 0x60000, 0x10000, CRC(8fca9f28) SHA1(cac85bf2b66e49e22c33c85bdb5712feef6aae7e) )
	ROM_LOAD( "ep12", 0x70000, 0x08000, CRC(3cd1d0c3) SHA1(ca3546cf51ebb10dfa4e78954f0212e8fcdb3d57) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ep02", 0x00000, 0x10000, CRC(711ce46f) SHA1(939a8545e53776ff2180d2c7e63bc997689c088e) )
ROM_END
#endif
#if 0
ROM_START( robocopb )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "robop_05.rom", 0x00000, 0x10000, CRC(bcef3e9b) SHA1(0ca099ea7428f877036e6e2a6daddfd9145ed9bb) )
	ROM_LOAD16_BYTE( "robop_01.rom", 0x00001, 0x10000, CRC(c9803685) SHA1(13b3b0ebee24b4453685616e9a204b4ca6fb0053) )
	ROM_LOAD16_BYTE( "robop_04.rom", 0x20000, 0x10000, CRC(9d7b79e0) SHA1(e0d901b9b3cd62f7c947da04f7447ebfa88bf44a) )
	ROM_LOAD16_BYTE( "robop_00.rom", 0x20001, 0x10000, CRC(80ba64ab) SHA1(0688f1b483a265c7324f546d38a4a5ac5b1b9214) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 Sound */
	ROM_LOAD( "ep03-3", 0x08000, 0x08000, CRC(5b164b24) SHA1(b217a2ac8b26aebd208631a13030487ed27d232e) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ep23", 0x00000, 0x10000, CRC(a77e4ab1) SHA1(d06cc847192b6c7f642e4ff7128e298d0aa034b2) )
	ROM_LOAD( "ep22", 0x10000, 0x10000, CRC(9fbd6903) SHA1(9ac6ac8a18c23e915e8ae3782867d10c0bd65778) )

	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ep20", 0x00000, 0x10000, CRC(1d8d38b8) SHA1(9add6349f8a578fb86b678cef921d6ec0cfccdad) )
	ROM_LOAD( "ep21", 0x10000, 0x10000, CRC(187929b2) SHA1(deca1f0a52584769caee1d2302617aa957c56a71) )
	ROM_LOAD( "ep18", 0x20000, 0x10000, CRC(b6580b5e) SHA1(ee216d8db89b8cb7a51a4e19bf6f17788547156b) )
	ROM_LOAD( "ep19", 0x30000, 0x10000, CRC(9bad01c7) SHA1(947c7f9d0facaea13a924274adde0e996be7b999) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ep14", 0x00000, 0x08000, CRC(ca56ceda) SHA1(edbaa29fc166cddf071ff5e59cfcfb7eeb127d68) )
	ROM_LOAD( "ep15", 0x08000, 0x08000, CRC(a945269c) SHA1(de0b387e8699298f7682d6d7ca803a209888f7a1) )
	ROM_LOAD( "ep16", 0x10000, 0x08000, CRC(e7fa4d58) SHA1(32e3f649b4f112a4e6be00068473b82c627bc8d1) )
	ROM_LOAD( "ep17", 0x18000, 0x08000, CRC(84aae89d) SHA1(037520bd0f291f862c2211a6f35b2a8a54f10b2a) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ep07", 0x00000, 0x10000, CRC(495d75cf) SHA1(0ffe677d53b7675073902e9bd40e4150f2cdfb1a) )
	ROM_LOAD( "ep06", 0x10000, 0x08000, CRC(a2ae32e2) SHA1(4e8182205563da9d50a831c65951645e278b03e6) )
	ROM_LOAD( "ep11", 0x20000, 0x10000, CRC(62fa425a) SHA1(be88c1a6436df8a456c405822e28c472e3e79a69) )
	ROM_LOAD( "ep10", 0x30000, 0x08000, CRC(cce3bd95) SHA1(00bbb197824d970b0e404167ca4ae53e1955ad94) )
	ROM_LOAD( "ep09", 0x40000, 0x10000, CRC(11bed656) SHA1(6a7d984a32982d9aef8ea7d8a720925036e7046e) )
	ROM_LOAD( "ep08", 0x50000, 0x08000, CRC(c45c7b4c) SHA1(70e3e475fe767eefa4cc1d6ca052271a099ff7a8) )
	ROM_LOAD( "ep13", 0x60000, 0x10000, CRC(8fca9f28) SHA1(cac85bf2b66e49e22c33c85bdb5712feef6aae7e) )
	ROM_LOAD( "ep12", 0x70000, 0x08000, CRC(3cd1d0c3) SHA1(ca3546cf51ebb10dfa4e78954f0212e8fcdb3d57) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ep02", 0x00000, 0x10000, CRC(711ce46f) SHA1(939a8545e53776ff2180d2c7e63bc997689c088e) )
ROM_END
#endif
/*

AUTOMAT (bootleg ROBOCOP)
^^^^^^^^^^^^^^^^^^^^^^^^^

Dumped by Andrew Welburn on the evening of a day
of big snow! 02/02/09

http://www.andys-arcade.com

*************************************************
**Do not seperate this text file from the roms.**
*************************************************

Take a look at the photos in the archive, the roms
should be failry explanatory, and you should be
able to pick out the chips it uses.

The most striking thing about this bootleg apart
from the obviously changed title screen is that
the music/melody is not right, they've copied the
digital sound effects, but appear to have ripepd
the music and circuit design from an earlier
capcom game, i can't work out whcih one, but
what an odd thing to do!

you can see a youtube video of it runnign here:
http://uk.youtube.com/watch?v=Y-KvbKtqzaQ

Rom 21 is full of 0's... i cleaned and re-dumped
it numerous times, but i just got 0's everytime.
It contains some of the the graphics for enemies
on the opening stage at the very least.

enjoy..

*/
#if 0
ROM_START( automat )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "5.bin", 0x00000, 0x10000, CRC(fb6faa74) SHA1(0af03c06193b5ba1422571b9504a7f655c608d94) )
	ROM_LOAD16_BYTE( "2.bin", 0x00001, 0x10000, CRC(7ecf8309) SHA1(59dd50bcb528ece42a67154bcc4f432770420986) )
	ROM_LOAD16_BYTE( "4.bin", 0x20000, 0x10000, CRC(9d7b79e0) SHA1(e0d901b9b3cd62f7c947da04f7447ebfa88bf44a) )
	ROM_LOAD16_BYTE( "3.bin", 0x20001, 0x10000, CRC(e655f9c3) SHA1(d5e99d542303d009277ccfc245f877e4e28603c9) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Z80 Sound */
	ROM_LOAD( "1.bin", 0x00000, 0x10000, CRC(72ea6024) SHA1(debd30219879ec01f43cc116a6cfa17209940ecc) )

	ROM_REGION( 0x40000, "gfxload1", 0 ) /* chars */
	ROM_LOAD( "8.bin",  0x00000, 0x10000, CRC(dcfffc7a) SHA1(e250626473917d397381210ef536efbc93c46474) ) // y?
	ROM_LOAD( "7.bin",  0x10000, 0x10000, CRC(40218082) SHA1(6a5c83d20fe110d642d5730c52e2796655fb66b4) ) // y
	ROM_LOAD( "10.bin", 0x20000, 0x10000, CRC(957da6dd) SHA1(53490d80ef108e93f13440de13b58761b89a419a) ) // y
	ROM_LOAD( "12.bin", 0x30000, 0x10000, CRC(00cd0990) SHA1(3fc498fcee2110001e376f5ee38d7dd361bd3ee3) ) // y

	/* copy out the chars */
	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_COPY( "gfxload1", 0x00000, 0x00000, 0x8000 )
	ROM_COPY( "gfxload1", 0x10000, 0x08000, 0x8000 )
	ROM_COPY( "gfxload1", 0x20000, 0x10000, 0x8000 )
	ROM_COPY( "gfxload1", 0x30000, 0x18000, 0x8000 )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_COPY( "gfxload1", 0x08000, 0x00000, 0x8000 )
	ROM_COPY( "gfxload1", 0x18000, 0x08000, 0x8000 )
	ROM_COPY( "gfxload1", 0x28000, 0x10000, 0x8000 )
	ROM_COPY( "gfxload1", 0x38000, 0x18000, 0x8000 )

	// we have to rearrange this with ROM_CONTINUE due to the way gfxdecode works */
	ROM_REGION( 0x40000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "9.bin",  0x00000, 0x2000, CRC(ccf91ce0) SHA1(c976eddcea48da6e7fbd28a4d5c48706d61cabfb) )
	ROM_CONTINUE(       0x04000, 0x2000 )
	ROM_CONTINUE(       0x08000, 0x2000 )
	ROM_CONTINUE(       0x0c000, 0x2000 )
	ROM_CONTINUE(       0x02000, 0x2000 )
	ROM_CONTINUE(       0x06000, 0x2000 )
	ROM_CONTINUE(       0x0a000, 0x2000 )
	ROM_CONTINUE(       0x0e000, 0x2000 )
	ROM_LOAD( "6.bin",  0x10000, 0x2000, CRC(5a557765) SHA1(f081323dad532fae6ec5d2875ffb1c394ac0bcf9) )
	ROM_CONTINUE(       0x14000, 0x2000 )
	ROM_CONTINUE(       0x18000, 0x2000 )
	ROM_CONTINUE(       0x1c000, 0x2000 )
	ROM_CONTINUE(       0x12000, 0x2000 )
	ROM_CONTINUE(       0x16000, 0x2000 )
	ROM_CONTINUE(       0x1a000, 0x2000 )
	ROM_CONTINUE(       0x1e000, 0x2000 )
	ROM_LOAD( "11.bin", 0x20000, 0x2000, CRC(8b196ab7) SHA1(030dc19f464db072c8dbbf043ae9334aa58510d0) )
	ROM_CONTINUE(       0x24000, 0x2000 )
	ROM_CONTINUE(       0x28000, 0x2000 )
	ROM_CONTINUE(       0x2c000, 0x2000 )
	ROM_CONTINUE(       0x22000, 0x2000 )
	ROM_CONTINUE(       0x26000, 0x2000 )
	ROM_CONTINUE(       0x2a000, 0x2000 )
	ROM_CONTINUE(       0x2e000, 0x2000 )
	ROM_LOAD( "13.bin", 0x30000, 0x2000, CRC(7f12ed0e) SHA1(9340611b85f9866d086970ed5e9c0c704616c330) )
	ROM_CONTINUE(       0x34000, 0x2000 )
	ROM_CONTINUE(       0x38000, 0x2000 )
	ROM_CONTINUE(       0x3c000, 0x2000 )
	ROM_CONTINUE(       0x32000, 0x2000 )
	ROM_CONTINUE(       0x36000, 0x2000 )
	ROM_CONTINUE(       0x3a000, 0x2000 )
	ROM_CONTINUE(       0x3e000, 0x2000 )

	// the sprite data is the same as robocop, but with the bits in each byte reversed
	// 21.bin was repaired with this knowledge as the chip was faulty
	ROM_REGION( 0x80000, "gfx4", ROMREGION_INVERT) /* sprites */
	ROM_LOAD( "16.bin", 0x00000, 0x10000, CRC(e42e8675) SHA1(5b964477de8278ea330ffc2366e5fc7e10122ef8) )
	ROM_LOAD( "17.bin", 0x10000, 0x08000, CRC(9a414c56) SHA1(017eb5a238e24cd6de50afd029c239993fc61a21) )
	ROM_LOAD( "20.bin", 0x20000, 0x10000, CRC(7c62a2a1) SHA1(43a40355cdcbb17506f9634e8f12673287e79bd7) )
	ROM_LOAD( "21.bin", 0x30000, 0x08000, CRC(ae59dccd) SHA1(e4ec6e9441bd7882a14768a7b7d8e79a7781f436) )
	ROM_LOAD( "14.bin", 0x40000, 0x10000, CRC(674ad6dc) SHA1(63982b8106f771e9e79cd8dbad42cfd4aad6f16f) )
	ROM_LOAD( "15.bin", 0x50000, 0x08000, CRC(5e7dd1aa) SHA1(822232a7389708dd5fee4a874a8832e22e7a0a26) )
	ROM_LOAD( "18.bin", 0x60000, 0x10000, CRC(751e34aa) SHA1(066730a26606a74b9295fc483cb0063c32dc9a14) )
	ROM_LOAD( "19.bin", 0x70000, 0x08000, CRC(118e7fc7) SHA1(fa6d8eef9da873579e19a9bf982643e061b8ca26) )
ROM_END
#endif

ROM_START( hippodrm )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "ew02",         0x00000, 0x10000, CRC(df0d7dc6) SHA1(a60197ad6f19f730e05cf6a3be9181f28d425344) )
	ROM_LOAD16_BYTE( "ew01",         0x00001, 0x10000, CRC(d5670aa7) SHA1(ea8bdff63176c2657746c2c438298685e1f44eae) )
	ROM_LOAD16_BYTE( "ew05",         0x20000, 0x10000, CRC(c76d65ec) SHA1(620990acaf2fd7f3fbfe7135a17ac0195feb8330) )
	ROM_LOAD16_BYTE( "ew00",         0x20001, 0x10000, CRC(e9b427a6) SHA1(b334992846771739d31756724138b82f897dfad5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 sound */
	ROM_LOAD( "ew04",         0x8000, 0x8000, CRC(9871b98d) SHA1(2b6c46bc2b10a28946d6ad8251e1a156a0b99947) )

	ROM_REGION( 0x10000, "sub", 0 ) /* HuC6280 CPU */
	ROM_LOAD( "ew08",         0x00000, 0x10000, CRC(53010534) SHA1(8b996e48414bacd009e05ff49848884ecf15d967) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ew14",         0x00000, 0x10000, CRC(71ca593d) SHA1(05ab9403c4010a21dcaa169f4c59d19c4169d9cd) )
	ROM_LOAD( "ew13",         0x10000, 0x10000, CRC(86be5fa7) SHA1(71c31ca2e92fb39a5486e80150919e13d5617855) )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ew19",         0x00000, 0x08000, CRC(6b80d7a3) SHA1(323162e7e0ce16f6244d8d98fdb2396ffef87e82) )
	ROM_LOAD( "ew18",         0x08000, 0x08000, CRC(78d3d764) SHA1(e8f77a23bd4f4d268bec7c0153fb957acd07cdee) )
	ROM_LOAD( "ew20",         0x10000, 0x08000, CRC(ce9f5de3) SHA1(b8af33f52ca3579a45b41395751697a58931f9d6) )
	ROM_LOAD( "ew21",         0x18000, 0x08000, CRC(487a7ba2) SHA1(7d52cc1517def8426355e8281440ec5e617d1121) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ew24",         0x00000, 0x08000, CRC(4e1bc2a4) SHA1(d7d4c42fd932722436f1847929088e46d03184bd) )
	ROM_LOAD( "ew25",         0x08000, 0x08000, CRC(9eb47dfb) SHA1(bb1e8a3a47f447f3a983ea51943d3081d56ad9a4) )
	ROM_LOAD( "ew23",         0x10000, 0x08000, CRC(9ecf479e) SHA1(a8d4c1490f12e1b15d53a2a97147920dcb638378) )
	ROM_LOAD( "ew22",         0x18000, 0x08000, CRC(e55669aa) SHA1(2a9b0e85bb81ff87a108e08b28e19b7b469463e4) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ew15",         0x00000, 0x10000, CRC(95423914) SHA1(e9e7a6bdf5aa717dc04a751709632f31762886fb) )
	ROM_LOAD( "ew16",         0x10000, 0x10000, CRC(96233177) SHA1(929a1b7fb65ab33277719b84517ff57da563f875) )
	ROM_LOAD( "ew10",         0x20000, 0x10000, CRC(4c25dfe8) SHA1(e4334de96698cd0112a8926dea131e748b6a84fc) )
	ROM_LOAD( "ew11",         0x30000, 0x10000, CRC(f2e007fc) SHA1(da30ad3725b9bc4a07dbb1afa05f145c3574c84c) )
	ROM_LOAD( "ew06",         0x40000, 0x10000, CRC(e4bb8199) SHA1(49b5b45c7cd9c44f53d83ee2a156d9e9f8a53960) )
	ROM_LOAD( "ew07",         0x50000, 0x10000, CRC(470b6989) SHA1(16b292d8a3a54048bf29f0b4f41bb6ca049b347c) )
	ROM_LOAD( "ew17",         0x60000, 0x10000, CRC(8c97c757) SHA1(36fd807da9e144dfb29c8252e9450cc37ca2604f) )
	ROM_LOAD( "ew12",         0x70000, 0x10000, CRC(a2d244bc) SHA1(ff2391efc480f36a302650691f8a7a620b86d99a) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ew03",         0x0000, 0x10000, CRC(b606924d) SHA1(b759fcec10b333465cf5cd1b30987bf2d62186b2) )
ROM_END

#if 0
ROM_START( ffantasy )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "ff-02-2.bin",  0x00000, 0x10000, CRC(29fc22a7) SHA1(73cbd47c34bee22c16a69cfc6037a60dc30effe8) )
	ROM_LOAD16_BYTE( "ff-01-2.bin",  0x00001, 0x10000, CRC(9f617cb4) SHA1(447ea4e57dd6b23aaf48e5e14c7893277730c7d9) )
	ROM_LOAD16_BYTE( "ew05",         0x20000, 0x10000, CRC(c76d65ec) SHA1(620990acaf2fd7f3fbfe7135a17ac0195feb8330) )
	ROM_LOAD16_BYTE( "ew00",         0x20001, 0x10000, CRC(e9b427a6) SHA1(b334992846771739d31756724138b82f897dfad5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 sound */
	ROM_LOAD( "ew04",         0x8000, 0x8000, CRC(9871b98d) SHA1(2b6c46bc2b10a28946d6ad8251e1a156a0b99947) )

	ROM_REGION( 0x10000, "sub", 0 ) /* HuC6280 CPU */
	ROM_LOAD( "ew08",         0x00000, 0x10000, CRC(53010534) SHA1(8b996e48414bacd009e05ff49848884ecf15d967) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ev14",         0x00000, 0x10000, CRC(686f72c1) SHA1(41d4fc1208d779f3428990a96586f6a555c28562) )
	ROM_LOAD( "ev13",         0x10000, 0x10000, CRC(b787dcc9) SHA1(7fce9d2040bcb2483419ea1cafed538bb8aba4f9) )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ew19",         0x00000, 0x08000, CRC(6b80d7a3) SHA1(323162e7e0ce16f6244d8d98fdb2396ffef87e82) )
	ROM_LOAD( "ew18",         0x08000, 0x08000, CRC(78d3d764) SHA1(e8f77a23bd4f4d268bec7c0153fb957acd07cdee) )
	ROM_LOAD( "ew20",         0x10000, 0x08000, CRC(ce9f5de3) SHA1(b8af33f52ca3579a45b41395751697a58931f9d6) )
	ROM_LOAD( "ew21",         0x18000, 0x08000, CRC(487a7ba2) SHA1(7d52cc1517def8426355e8281440ec5e617d1121) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ew24",         0x00000, 0x08000, CRC(4e1bc2a4) SHA1(d7d4c42fd932722436f1847929088e46d03184bd) )
	ROM_LOAD( "ew25",         0x08000, 0x08000, CRC(9eb47dfb) SHA1(bb1e8a3a47f447f3a983ea51943d3081d56ad9a4) )
	ROM_LOAD( "ew23",         0x10000, 0x08000, CRC(9ecf479e) SHA1(a8d4c1490f12e1b15d53a2a97147920dcb638378) )
	ROM_LOAD( "ew22",         0x18000, 0x08000, CRC(e55669aa) SHA1(2a9b0e85bb81ff87a108e08b28e19b7b469463e4) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ev15",         0x00000, 0x10000, CRC(1d80f797) SHA1(1b6878155367350ff826593ea73bda5b893c1823) )
	ROM_LOAD( "ew16",         0x10000, 0x10000, CRC(96233177) SHA1(929a1b7fb65ab33277719b84517ff57da563f875) )
	ROM_LOAD( "ev10",         0x20000, 0x10000, CRC(c4e7116b) SHA1(1e665ba150e08ceb1c0d5f7b7e777f3d60997811) )
	ROM_LOAD( "ew11",         0x30000, 0x10000, CRC(f2e007fc) SHA1(da30ad3725b9bc4a07dbb1afa05f145c3574c84c) )
	ROM_LOAD( "ev06",         0x40000, 0x10000, CRC(6c794f1a) SHA1(ab7996917bea99850aef5a0890485dd27778cd99) )
	ROM_LOAD( "ew07",         0x50000, 0x10000, CRC(470b6989) SHA1(16b292d8a3a54048bf29f0b4f41bb6ca049b347c) )
	ROM_LOAD( "ev17",         0x60000, 0x10000, CRC(045509d4) SHA1(ebbd71de8e8492ff6321e3ede0d98d9ed462de01) )
	ROM_LOAD( "ew12",         0x70000, 0x10000, CRC(a2d244bc) SHA1(ff2391efc480f36a302650691f8a7a620b86d99a) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ew03",         0x0000, 0x10000, CRC(b606924d) SHA1(b759fcec10b333465cf5cd1b30987bf2d62186b2) )
ROM_END
#endif
#if 0
ROM_START( ffantasya )
	ROM_REGION( 0x60000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "ev02",         0x00000, 0x10000, CRC(797a7860) SHA1(aaab24c99e96b393d2bda435f18b0dc4003cdf09) )
	ROM_LOAD16_BYTE( "ev01",         0x00001, 0x10000, CRC(0f17184d) SHA1(c1bcd6347df9bee2d2d9ca29b22af9235493871c) )
	ROM_LOAD16_BYTE( "ew05",         0x20000, 0x10000, CRC(c76d65ec) SHA1(620990acaf2fd7f3fbfe7135a17ac0195feb8330) )
	ROM_LOAD16_BYTE( "ew00",         0x20001, 0x10000, CRC(e9b427a6) SHA1(b334992846771739d31756724138b82f897dfad5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 sound */
	ROM_LOAD( "ew04",         0x8000, 0x8000, CRC(9871b98d) SHA1(2b6c46bc2b10a28946d6ad8251e1a156a0b99947) )

	ROM_REGION( 0x10000, "sub", 0 ) /* HuC6280 CPU */
	ROM_LOAD( "ew08",         0x00000, 0x10000, CRC(53010534) SHA1(8b996e48414bacd009e05ff49848884ecf15d967) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "ev14",         0x00000, 0x10000, CRC(686f72c1) SHA1(41d4fc1208d779f3428990a96586f6a555c28562) )
	ROM_LOAD( "ev13",         0x10000, 0x10000, CRC(b787dcc9) SHA1(7fce9d2040bcb2483419ea1cafed538bb8aba4f9) )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "ew19",         0x00000, 0x08000, CRC(6b80d7a3) SHA1(323162e7e0ce16f6244d8d98fdb2396ffef87e82) )
	ROM_LOAD( "ew18",         0x08000, 0x08000, CRC(78d3d764) SHA1(e8f77a23bd4f4d268bec7c0153fb957acd07cdee) )
	ROM_LOAD( "ew20",         0x10000, 0x08000, CRC(ce9f5de3) SHA1(b8af33f52ca3579a45b41395751697a58931f9d6) )
	ROM_LOAD( "ew21",         0x18000, 0x08000, CRC(487a7ba2) SHA1(7d52cc1517def8426355e8281440ec5e617d1121) )

	ROM_REGION( 0x20000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "ew24",         0x00000, 0x08000, CRC(4e1bc2a4) SHA1(d7d4c42fd932722436f1847929088e46d03184bd) )
	ROM_LOAD( "ew25",         0x08000, 0x08000, CRC(9eb47dfb) SHA1(bb1e8a3a47f447f3a983ea51943d3081d56ad9a4) )
	ROM_LOAD( "ew23",         0x10000, 0x08000, CRC(9ecf479e) SHA1(a8d4c1490f12e1b15d53a2a97147920dcb638378) )
	ROM_LOAD( "ew22",         0x18000, 0x08000, CRC(e55669aa) SHA1(2a9b0e85bb81ff87a108e08b28e19b7b469463e4) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "ev15",         0x00000, 0x10000, CRC(1d80f797) SHA1(1b6878155367350ff826593ea73bda5b893c1823) )
	ROM_LOAD( "ew16",         0x10000, 0x10000, CRC(96233177) SHA1(929a1b7fb65ab33277719b84517ff57da563f875) )
	ROM_LOAD( "ev10",         0x20000, 0x10000, CRC(c4e7116b) SHA1(1e665ba150e08ceb1c0d5f7b7e777f3d60997811) )
	ROM_LOAD( "ew11",         0x30000, 0x10000, CRC(f2e007fc) SHA1(da30ad3725b9bc4a07dbb1afa05f145c3574c84c) )
	ROM_LOAD( "ev06",         0x40000, 0x10000, CRC(6c794f1a) SHA1(ab7996917bea99850aef5a0890485dd27778cd99) )
	ROM_LOAD( "ew07",         0x50000, 0x10000, CRC(470b6989) SHA1(16b292d8a3a54048bf29f0b4f41bb6ca049b347c) )
	ROM_LOAD( "ev17",         0x60000, 0x10000, CRC(045509d4) SHA1(ebbd71de8e8492ff6321e3ede0d98d9ed462de01) )
	ROM_LOAD( "ew12",         0x70000, 0x10000, CRC(a2d244bc) SHA1(ff2391efc480f36a302650691f8a7a620b86d99a) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "ew03",         0x0000, 0x10000, CRC(b606924d) SHA1(b759fcec10b333465cf5cd1b30987bf2d62186b2) )
ROM_END
#endif

ROM_START( slyspy )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fa14-3.17l",   0x00000, 0x10000, CRC(54353a84) SHA1(899559f17705a8222fd56e9304e9b802eac8f6db) )
	ROM_LOAD16_BYTE( "fa12-2.9l",    0x00001, 0x10000, CRC(1b534294) SHA1(cf7badea6604c47d9f3ff8a0ef326e09de1974a0) )
	ROM_LOAD16_BYTE( "fa15.19l",     0x20000, 0x10000, CRC(04a79266) SHA1(69d256ffb1c89721f8b1e929c581f187e047b977) )
	ROM_LOAD16_BYTE( "fa13.11l",     0x20001, 0x10000, CRC(641cc4b3) SHA1(ce0ccd14d201f411cfc02ec988b2ad4fcb0d8f5d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fa10.5h",      0x00000, 0x10000, CRC(dfd2ff25) SHA1(3dcd6d50b92b49daae4b51581abe9c95f764e848) )

	ROM_REGION( 0x10000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fa05.11a",     0x04000, 0x04000, CRC(09802924) SHA1(d9bc5fe7f053afa15cd39400aae993866d1b0226) )
	ROM_CONTINUE(             0x00000, 0x04000 )	/* the two halves are swapped */
	ROM_LOAD( "fa04.9a",      0x0c000, 0x04000, CRC(ec25b895) SHA1(8c1d2b9a2487fd7114d37fe9dc271183c4cc1613) )
	ROM_CONTINUE(             0x08000, 0x04000 )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fa07.17a",     0x00000, 0x10000, CRC(e932268b) SHA1(ee8ed29affa951e725cf19a5f56d3beac24420c9) )
	ROM_LOAD( "fa06.15a",     0x10000, 0x10000, CRC(c4dd38c0) SHA1(267dbbdd5df6b13662cd307c5c95fdf643d64f45) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fa09.22a",     0x00000, 0x20000, CRC(1395e9be) SHA1(60693ac6236ffe1e0933d81771cfad32e14514c3) )
	ROM_LOAD( "fa08.21a",     0x20000, 0x20000, CRC(4d7464db) SHA1(82e2a3c3d78447985968220d52c7c1f1ff625d83) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fa01.4a",      0x00000, 0x20000, CRC(99b0cd92) SHA1(2729e874730391b5fa93e9a28142c02c00eb5068) )
	ROM_LOAD( "fa03.7a",      0x20000, 0x20000, CRC(0e7ea74d) SHA1(22078a2856933af2d31750a4a506b993fe309e9a) )
	ROM_LOAD( "fa00.2a",      0x40000, 0x20000, CRC(f7df3fd7) SHA1(ed9e4649e0b1fcca61cf4d159b3f8a35f06102ce) )
	ROM_LOAD( "fa02.5a",      0x60000, 0x20000, CRC(84e8da9d) SHA1(41da6042f80ea3562aa350f4f466b16db29e2aca) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fa11.11k",     0x00000, 0x20000, CRC(4e547bad) SHA1(655eda4d00f8846957ed40dcbf750fba3ce19f4e) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "mb7114h.21k",  0x0000, 0x0100, CRC(ad26e8d4) SHA1(827337aeb8904429a1c050279240ae38aa6ce064) )	/* Priority (not used) */

	ROM_REGION( 0x0800, "plds", 0 )
	ROM_LOAD( "pal16l8b-ta-1.bin", 0x0000, 0x0104, CRC(79a87527) SHA1(3c6ad20d5a7c41c020e671d462c0b1e4a5dda7f8) )
	ROM_LOAD( "pal16r4a-ta-2.bin", 0x0200, 0x0104, CRC(eca31311) SHA1(a87b2721e13767f7448236d0bbb3355583fe88bb) )
	ROM_LOAD( "pal16l8a-ta-3.bin", 0x0400, 0x0104, CRC(6c324919) SHA1(83bba4634d7ab7c4ad3083c063804fd1e7c9c10b) )
	ROM_LOAD( "pal16l8a-ta-4.bin", 0x0600, 0x0104, CRC(116177fa) SHA1(f63802578b6f743e2d3a64e4805488d44361dcb2) )
ROM_END

#if 0
ROM_START( slyspy2 )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fa14-2.bin",   0x00000, 0x10000, CRC(0e431e39) SHA1(ab4774966ad113e4d7004d14bfd72330d4a93a43) )
	ROM_LOAD16_BYTE( "fa12-2.9l",    0x00001, 0x10000, CRC(1b534294) SHA1(cf7badea6604c47d9f3ff8a0ef326e09de1974a0) )
	ROM_LOAD16_BYTE( "fa15.19l",     0x20000, 0x10000, CRC(04a79266) SHA1(69d256ffb1c89721f8b1e929c581f187e047b977) )
	ROM_LOAD16_BYTE( "fa13.11l",     0x20001, 0x10000, CRC(641cc4b3) SHA1(ce0ccd14d201f411cfc02ec988b2ad4fcb0d8f5d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fa10.5h",      0x00000, 0x10000, CRC(dfd2ff25) SHA1(3dcd6d50b92b49daae4b51581abe9c95f764e848) )

	ROM_REGION( 0x10000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fa05.11a",     0x04000, 0x04000, CRC(09802924) SHA1(d9bc5fe7f053afa15cd39400aae993866d1b0226) )
	ROM_CONTINUE(             0x00000, 0x04000 )	/* the two halves are swapped */
	ROM_LOAD( "fa04.9a",      0x0c000, 0x04000, CRC(ec25b895) SHA1(8c1d2b9a2487fd7114d37fe9dc271183c4cc1613) )
	ROM_CONTINUE(             0x08000, 0x04000 )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fa07.17a",     0x00000, 0x10000, CRC(e932268b) SHA1(ee8ed29affa951e725cf19a5f56d3beac24420c9) )
	ROM_LOAD( "fa06.15a",     0x10000, 0x10000, CRC(c4dd38c0) SHA1(267dbbdd5df6b13662cd307c5c95fdf643d64f45) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fa09.22a",     0x00000, 0x20000, CRC(1395e9be) SHA1(60693ac6236ffe1e0933d81771cfad32e14514c3) )
	ROM_LOAD( "fa08.21a",     0x20000, 0x20000, CRC(4d7464db) SHA1(82e2a3c3d78447985968220d52c7c1f1ff625d83) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fa01.4a",      0x00000, 0x20000, CRC(99b0cd92) SHA1(2729e874730391b5fa93e9a28142c02c00eb5068) )
	ROM_LOAD( "fa03.7a",      0x20000, 0x20000, CRC(0e7ea74d) SHA1(22078a2856933af2d31750a4a506b993fe309e9a) )
	ROM_LOAD( "fa00.2a",      0x40000, 0x20000, CRC(f7df3fd7) SHA1(ed9e4649e0b1fcca61cf4d159b3f8a35f06102ce) )
	ROM_LOAD( "fa02.5a",      0x60000, 0x20000, CRC(84e8da9d) SHA1(41da6042f80ea3562aa350f4f466b16db29e2aca) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fa11.11k",     0x00000, 0x20000, CRC(4e547bad) SHA1(655eda4d00f8846957ed40dcbf750fba3ce19f4e) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "mb7114h.21k",  0x0000, 0x0100, CRC(ad26e8d4) SHA1(827337aeb8904429a1c050279240ae38aa6ce064) )	/* Priority (not used) */

	ROM_REGION( 0x0800, "plds", 0 )
	ROM_LOAD( "pal16l8b-ta-1.bin", 0x0000, 0x0104, CRC(79a87527) SHA1(3c6ad20d5a7c41c020e671d462c0b1e4a5dda7f8) )
	ROM_LOAD( "pal16r4a-ta-2.bin", 0x0200, 0x0104, CRC(eca31311) SHA1(a87b2721e13767f7448236d0bbb3355583fe88bb) )
	ROM_LOAD( "pal16l8a-ta-3.bin", 0x0400, 0x0104, CRC(6c324919) SHA1(83bba4634d7ab7c4ad3083c063804fd1e7c9c10b) )
	ROM_LOAD( "pal16l8a-ta-4.bin", 0x0600, 0x0104, CRC(116177fa) SHA1(f63802578b6f743e2d3a64e4805488d44361dcb2) )
ROM_END
#endif
#if 0
ROM_START( secretag )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fb14-3.17l",   0x00000, 0x10000, CRC(9be6ac90) SHA1(1c78af9da63add7c77c8d2ce24924505481381b1) )
	ROM_LOAD16_BYTE( "fb12-3.9l",   0x00001, 0x10000, CRC(28904b6b) SHA1(c3fd42c3ba5b19c3483df3ac9e44016570762de7) )
	ROM_LOAD16_BYTE( "fb15.19l",   0x20000, 0x10000, CRC(106bb26c) SHA1(e5d05124b6dfc54e41dcf40916633caaa9a19823) )
	ROM_LOAD16_BYTE( "fb13.11l",   0x20001, 0x10000, CRC(90523413) SHA1(7ea65525f2d7c577255aa01260acc5f43d136b3c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fa10.5h",      0x00000, 0x10000, CRC(dfd2ff25) SHA1(3dcd6d50b92b49daae4b51581abe9c95f764e848) )

	ROM_REGION( 0x10000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fa05.11a",     0x04000, 0x04000, CRC(09802924) SHA1(d9bc5fe7f053afa15cd39400aae993866d1b0226) )
	ROM_CONTINUE(             0x00000, 0x04000 )	/* the two halves are swapped */
	ROM_LOAD( "fa04.9a",      0x0c000, 0x04000, CRC(ec25b895) SHA1(8c1d2b9a2487fd7114d37fe9dc271183c4cc1613) )
	ROM_CONTINUE(             0x08000, 0x04000 )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fa07.17a",     0x00000, 0x10000, CRC(e932268b) SHA1(ee8ed29affa951e725cf19a5f56d3beac24420c9) )
	ROM_LOAD( "fa06.15a",     0x10000, 0x10000, CRC(c4dd38c0) SHA1(267dbbdd5df6b13662cd307c5c95fdf643d64f45) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fa09.22a",     0x00000, 0x20000, CRC(1395e9be) SHA1(60693ac6236ffe1e0933d81771cfad32e14514c3) )
	ROM_LOAD( "fa08.21a",     0x20000, 0x20000, CRC(4d7464db) SHA1(82e2a3c3d78447985968220d52c7c1f1ff625d83) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fa01.4a",      0x00000, 0x20000, CRC(99b0cd92) SHA1(2729e874730391b5fa93e9a28142c02c00eb5068) )
	ROM_LOAD( "fa03.7a",      0x20000, 0x20000, CRC(0e7ea74d) SHA1(22078a2856933af2d31750a4a506b993fe309e9a) )
	ROM_LOAD( "fa00.2a",      0x40000, 0x20000, CRC(f7df3fd7) SHA1(ed9e4649e0b1fcca61cf4d159b3f8a35f06102ce) )
	ROM_LOAD( "fa02.5a",      0x60000, 0x20000, CRC(84e8da9d) SHA1(41da6042f80ea3562aa350f4f466b16db29e2aca) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fa11.11k",     0x00000, 0x20000, CRC(4e547bad) SHA1(655eda4d00f8846957ed40dcbf750fba3ce19f4e) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "mb7114h.21k",  0x0000, 0x0100, CRC(ad26e8d4) SHA1(827337aeb8904429a1c050279240ae38aa6ce064) )	/* Priority (not used) */
ROM_END
#endif
/*

secret agent - deco (clone)

(snd)
1 x z80
1 x ym2203c
1 x ym3812
sa_01 and sa_02

(prg)
1 x 68000
from sa_03 to sa_06

(gfx)
from sa_07 to sa_14
from sa_15 to sa_22

[dump.it]

*/
#if 0
ROM_START( secretab )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "sa_05.bin",    0x00000, 0x10000, CRC(54869474) SHA1(88c1894d1b6d8dd3d37e97d566aafef9c9409d6e) )
	ROM_LOAD16_BYTE( "sa_03.bin",    0x00001, 0x10000, CRC(36ab1874) SHA1(baa47c466ab13ac792761531f77ee8e639d19203) )
	ROM_LOAD16_BYTE( "sa_06.bin",    0x20000, 0x10000, CRC(8e691f23) SHA1(eb08c9539b699af124fcf87be07a33d2d5a71ada) )
	ROM_LOAD16_BYTE( "sa_04.bin",    0x20001, 0x10000, CRC(c838b205) SHA1(8c7a453ec7a00d4f5bbf9fadba6d551909647ed8) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "sa_01.bin",      0x00000, 0x10000, CRC(9fdc503b) SHA1(7b258e0734ca88a7d3f574d75116f0fe3b628898) )

	ROM_REGION( 0x40000, "charset", 0 )
	ROM_LOAD( "sa_08.bin", 0x00000, 0x10000,CRC(4806b951) SHA1(a2fa5b8587132747067d7d64ccfd14129a34ef58) )
	ROM_LOAD( "sa_10.bin", 0x10000, 0x10000,CRC(843c4679) SHA1(871f3e77aa7e628e924a40d06ddec700487e23fb) )
	ROM_LOAD( "sa_12.bin", 0x20000, 0x10000,CRC(f9e2cd5f) SHA1(f2c3f6e763c6f80307e9daee533d316b05cd02c5) )
	ROM_LOAD( "sa_14.bin", 0x30000, 0x10000,CRC(3dac9128) SHA1(f3a2068e90973c1f04f1bbaa209111e3f9669ee0) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_COPY( "charset", 0x00000, 0x00000, 0x8000 )
	ROM_COPY( "charset", 0x10000, 0x08000, 0x8000 )
	ROM_COPY( "charset", 0x20000, 0x10000, 0x8000 )
	ROM_COPY( "charset", 0x30000, 0x18000, 0x8000 )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_COPY( "charset", 0x08000, 0x00000, 0x8000 )
	ROM_COPY( "charset", 0x18000, 0x08000, 0x8000 )
	ROM_COPY( "charset", 0x28000, 0x10000, 0x8000 )
	ROM_COPY( "charset", 0x38000, 0x18000, 0x8000 )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "sa_09.bin",      0x00000, 0x10000,CRC(9e412267) SHA1(482cd6e772fa21f15db66c27acf85e8f97f7c5a5) )
	ROM_LOAD( "sa_11.bin",      0x10000, 0x10000,CRC(e87650db) SHA1(381352428b12fd4a8cd13270009ff7602aa41a0b) )
	ROM_LOAD( "sa_07.bin",      0x20000, 0x10000,CRC(6ad2e575) SHA1(b6b159cb36e222fe62fc10271602226f027440e4) )
	ROM_LOAD( "sa_13.bin",      0x30000, 0x10000,CRC(e8601057) SHA1(fd73a36fb84049154248d250ffea68b1ee39a43f) )

	ROM_REGION( 0x80000, "gfx4", ROMREGION_INVERT ) /* sprites */
	ROM_LOAD( "sa_15.bin",      0x10000, 0x10000,CRC(54fcbc39) SHA1(293a6799193b01424c3eac86cf90cc023aa771db) )
	ROM_LOAD( "sa_16.bin",      0x00000, 0x10000,CRC(ff72b838) SHA1(fdc48ecdd2225fc69472313f34973f6add8fb558) )
	ROM_LOAD( "sa_17.bin",      0x30000, 0x10000,CRC(f61972c8) SHA1(fa9ddca3473091b4879171d8f3b302e8f2b45149) )
	ROM_LOAD( "sa_18.bin",      0x20000, 0x10000,CRC(4f989f00) SHA1(ae7ae6e62e6a516ae3c8ebbeb5e39887c1961add) )
	ROM_LOAD( "sa_19.bin",      0x50000, 0x10000,CRC(d29bc22e) SHA1(ce0935d09f7e94fa32247c86e14a74b73514b29e) )
	ROM_LOAD( "sa_20.bin",      0x40000, 0x10000,CRC(447e4f0b) SHA1(97db103e505a6e11eb9bdb3622e4aa3b796a9714) )
	ROM_LOAD( "sa_21.bin",      0x70000, 0x10000,CRC(dc6a38df) SHA1(9043df911389d3f085299f2f2202cab356473a32) )
	ROM_LOAD( "sa_22.bin",      0x60000, 0x10000,CRC(d234cae5) SHA1(0cd07bf087a4da19a5da29785385de9eee52d0fb) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "sa_02.bin",      0x00000, 0x10000,CRC(439eb5a9) SHA1(8d6baad8a1e89279ef0a378941d3d9b49a606864) ) // both halves identical
ROM_END
#endif

ROM_START( midres )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fk_14.rom",    0x00000, 0x20000, CRC(de7522df) SHA1(b627a4bf2f2308ff16e55a9e49ba4eb9bd637d90) )
	ROM_LOAD16_BYTE( "fk_12.rom",    0x00001, 0x20000, CRC(3494b8c9) SHA1(6cb3f1421fe71d329c65c0a9056bcfae7229a37b) )
	ROM_LOAD16_BYTE( "fl15",         0x40000, 0x20000, CRC(1328354e) SHA1(2780a524718f351350e0fbc92a9a7ce9bdfc315e) )
	ROM_LOAD16_BYTE( "fl13",         0x40001, 0x20000, CRC(e3b3955e) SHA1(10ff430b14c1dbcce81b13251bac124ef4f9f1d9) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fl16",              0x00000, 0x10000, CRC(66360bdf) SHA1(76ecaeb396118bb2fe6c0151bb0705a3a878f7a5) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fk_05.rom",         0x08000, 0x08000, CRC(3cdb7453) SHA1(d4b7fbf4726a375b4478922db6d936274bfa963c) )
	ROM_CONTINUE(                  0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "fk_04.rom",         0x18000, 0x08000, CRC(325ba20c) SHA1(fecd6254cf8c3b18496039fe18ded13c2ae47ff4) )
	ROM_CONTINUE(                  0x10000, 0x08000 )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fl09",              0x00000, 0x20000, CRC(907d5910) SHA1(6f4963724987bf44007988d117a1f7276cf270d8) )
	ROM_LOAD( "fl08",              0x20000, 0x20000, CRC(a936c03c) SHA1(293e69874ce9b2dfb1d605c9f988fa736b12bbcf) )
	ROM_LOAD( "fl07",              0x40000, 0x20000, CRC(2068c45c) SHA1(943ed767a462ee39a42cd15f02d06c8a2e4556b3) )
	ROM_LOAD( "fl06",              0x60000, 0x20000, CRC(b7241ab9) SHA1(3e83f9285ff4c476f1287bf73b514eace482dccc) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fl11",              0x00000, 0x20000, CRC(b86b73b4) SHA1(dd0e61d60574e537aa1b7f35ffdfd08434ec8208) )
	ROM_LOAD( "fl10",              0x20000, 0x20000, CRC(92245b29) SHA1(3289842bbd4bd7858846b234f08ea5737c11536d) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fl01",              0x00000, 0x20000, CRC(2c8b35a7) SHA1(9ab1c2f014a24837ee99c4db000291f7e55aeb12) )
	ROM_LOAD( "fl03",              0x20000, 0x20000, CRC(1eefed3c) SHA1(be0ce3db211587086ae3ee8df85b7c56f831c623) )
	ROM_LOAD( "fl00",              0x40000, 0x20000, CRC(756fb801) SHA1(35510c4ddf9258d87fdee0d3a64a8de0ebd1967d) )
	ROM_LOAD( "fl02",              0x60000, 0x20000, CRC(54d2c120) SHA1(84f93bcd41d5bda8cfb39c4947fff025f53b143d) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fl17",              0x00000, 0x20000, CRC(9029965d) SHA1(9b28dc38e86f24fa89d7971b141c9bdddc662c99) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "7114.prm",          0x0000, 0x0100, CRC(eb539ffb) SHA1(6a8c9112f289f63e8c88320c9df698b559632c3d) )	/* Priority (not used) */

	ROM_REGION( 0x0c00, "plds", 0 )
	ROM_LOAD( "pal16r4a-1.bin", 0x0000, 0x0104, CRC(d28fb8e0) SHA1(73cd73a075bd3ba3b3e50f3b71a4aaecce37115f) )
	ROM_LOAD( "pal16l8b-2.bin", 0x0200, 0x0104, CRC(bcb591e3) SHA1(d3ebc2a19108c9db355d3ba1512ab4cf0d9fad76) )
	ROM_LOAD( "pal16l8a-3.bin", 0x0400, 0x0104, CRC(e12972ac) SHA1(6b178c936068d9017a1444f437aea7e2ab1c6ca9) )
	ROM_LOAD( "pal16l8a-4.bin", 0x0600, 0x0104, CRC(c6437e49) SHA1(0d89855378ab5f45d55f6aa175a63458b3da52a3) )
	ROM_LOAD( "pal16l8b-5.bin", 0x0800, 0x0104, CRC(e9ee3a67) SHA1(5299f44f1141fcd57b0559b91ec7adb51b36c5c4) )
	ROM_LOAD( "pal16l8a-6.bin", 0x0a00, 0x0104, CRC(23b17abe) SHA1(ca6c47f4df63d84401ccb29d0a0e3633b09d708a) )
ROM_END

#if 0
ROM_START( midresu )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fl14",         0x00000, 0x20000, CRC(2f9507a2) SHA1(bfa3449c2f8d706ec9eebb41c0f089229cd30537) )
	ROM_LOAD16_BYTE( "fl12",         0x00001, 0x20000, CRC(3815ad9f) SHA1(04b05ca68a2526ef6b16a1bbbf91c36300070c6c) )
	ROM_LOAD16_BYTE( "fl15",         0x40000, 0x20000, CRC(1328354e) SHA1(2780a524718f351350e0fbc92a9a7ce9bdfc315e) )
	ROM_LOAD16_BYTE( "fl13",         0x40001, 0x20000, CRC(e3b3955e) SHA1(10ff430b14c1dbcce81b13251bac124ef4f9f1d9) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fl16",              0x00000, 0x10000, CRC(66360bdf) SHA1(76ecaeb396118bb2fe6c0151bb0705a3a878f7a5) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fl05",              0x08000, 0x08000, CRC(d75aba06) SHA1(cb3b969db3dd8e0c5c3729482f7461cde3a961f3) )
	ROM_CONTINUE(                  0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "fl04",              0x18000, 0x08000, CRC(8f5bbb79) SHA1(cb10f68787606111ba5e9967bf0b0cd21269a902) )
	ROM_CONTINUE(                  0x10000, 0x08000 )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fl09",              0x00000, 0x20000, CRC(907d5910) SHA1(6f4963724987bf44007988d117a1f7276cf270d8) )
	ROM_LOAD( "fl08",              0x20000, 0x20000, CRC(a936c03c) SHA1(293e69874ce9b2dfb1d605c9f988fa736b12bbcf) )
	ROM_LOAD( "fl07",              0x40000, 0x20000, CRC(2068c45c) SHA1(943ed767a462ee39a42cd15f02d06c8a2e4556b3) )
	ROM_LOAD( "fl06",              0x60000, 0x20000, CRC(b7241ab9) SHA1(3e83f9285ff4c476f1287bf73b514eace482dccc) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fl11",              0x00000, 0x20000, CRC(b86b73b4) SHA1(dd0e61d60574e537aa1b7f35ffdfd08434ec8208) )
	ROM_LOAD( "fl10",              0x20000, 0x20000, CRC(92245b29) SHA1(3289842bbd4bd7858846b234f08ea5737c11536d) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fl01",              0x00000, 0x20000, CRC(2c8b35a7) SHA1(9ab1c2f014a24837ee99c4db000291f7e55aeb12) )
	ROM_LOAD( "fl03",              0x20000, 0x20000, CRC(1eefed3c) SHA1(be0ce3db211587086ae3ee8df85b7c56f831c623) )
	ROM_LOAD( "fl00",              0x40000, 0x20000, CRC(756fb801) SHA1(35510c4ddf9258d87fdee0d3a64a8de0ebd1967d) )
	ROM_LOAD( "fl02",              0x60000, 0x20000, CRC(54d2c120) SHA1(84f93bcd41d5bda8cfb39c4947fff025f53b143d) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fl17",              0x00000, 0x20000, CRC(9029965d) SHA1(9b28dc38e86f24fa89d7971b141c9bdddc662c99) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "7114.prm",          0x0000, 0x0100, CRC(eb539ffb) SHA1(6a8c9112f289f63e8c88320c9df698b559632c3d) )	/* Priority (not used) */

	ROM_REGION( 0x0c00, "plds", 0 )
	ROM_LOAD( "pal16r4a-1.bin", 0x0000, 0x0104, CRC(d28fb8e0) SHA1(73cd73a075bd3ba3b3e50f3b71a4aaecce37115f) )
	ROM_LOAD( "pal16l8b-2.bin", 0x0200, 0x0104, CRC(bcb591e3) SHA1(d3ebc2a19108c9db355d3ba1512ab4cf0d9fad76) )
	ROM_LOAD( "pal16l8a-3.bin", 0x0400, 0x0104, CRC(e12972ac) SHA1(6b178c936068d9017a1444f437aea7e2ab1c6ca9) )
	ROM_LOAD( "pal16l8a-4.bin", 0x0600, 0x0104, CRC(c6437e49) SHA1(0d89855378ab5f45d55f6aa175a63458b3da52a3) )
	ROM_LOAD( "pal16l8b-5.bin", 0x0800, 0x0104, CRC(e9ee3a67) SHA1(5299f44f1141fcd57b0559b91ec7adb51b36c5c4) )
	ROM_LOAD( "pal16l8a-6.bin", 0x0a00, 0x0104, CRC(23b17abe) SHA1(ca6c47f4df63d84401ccb29d0a0e3633b09d708a) )
ROM_END
#endif
#if 0
ROM_START( midresj )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fh14",         0x00000, 0x20000, CRC(6d632a51) SHA1(38f9e8fe01ec9105c1ec83d70a5f5b2c754865ca) )
	ROM_LOAD16_BYTE( "fh12",         0x00001, 0x20000, CRC(45143384) SHA1(5733439d6598a02dc0ae74b41d34b6afadd39330) )
	ROM_LOAD16_BYTE( "fl15",         0x40000, 0x20000, CRC(1328354e) SHA1(2780a524718f351350e0fbc92a9a7ce9bdfc315e) )
	ROM_LOAD16_BYTE( "fl13",         0x40001, 0x20000, CRC(e3b3955e) SHA1(10ff430b14c1dbcce81b13251bac124ef4f9f1d9) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fh16",              0x00000, 0x10000, CRC(00736f32) SHA1(292f98b5579314c866247dd0ea1346c6e160b304) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fk_05.rom",         0x08000, 0x08000, CRC(3cdb7453) SHA1(d4b7fbf4726a375b4478922db6d936274bfa963c) )
	ROM_CONTINUE(                  0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "fk_04.rom",         0x18000, 0x08000, CRC(325ba20c) SHA1(fecd6254cf8c3b18496039fe18ded13c2ae47ff4) )
	ROM_CONTINUE(                  0x10000, 0x08000 )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fl09",              0x00000, 0x20000, CRC(907d5910) SHA1(6f4963724987bf44007988d117a1f7276cf270d8) )
	ROM_LOAD( "fl08",              0x20000, 0x20000, CRC(a936c03c) SHA1(293e69874ce9b2dfb1d605c9f988fa736b12bbcf) )
	ROM_LOAD( "fl07",              0x40000, 0x20000, CRC(2068c45c) SHA1(943ed767a462ee39a42cd15f02d06c8a2e4556b3) )
	ROM_LOAD( "fl06",              0x60000, 0x20000, CRC(b7241ab9) SHA1(3e83f9285ff4c476f1287bf73b514eace482dccc) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fl11",              0x00000, 0x20000, CRC(b86b73b4) SHA1(dd0e61d60574e537aa1b7f35ffdfd08434ec8208) )
	ROM_LOAD( "fl10",              0x20000, 0x20000, CRC(92245b29) SHA1(3289842bbd4bd7858846b234f08ea5737c11536d) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fl01",              0x00000, 0x20000, CRC(2c8b35a7) SHA1(9ab1c2f014a24837ee99c4db000291f7e55aeb12) )
	ROM_LOAD( "fl03",              0x20000, 0x20000, CRC(1eefed3c) SHA1(be0ce3db211587086ae3ee8df85b7c56f831c623) )
	ROM_LOAD( "fl00",              0x40000, 0x20000, CRC(756fb801) SHA1(35510c4ddf9258d87fdee0d3a64a8de0ebd1967d) )
	ROM_LOAD( "fl02",              0x60000, 0x20000, CRC(54d2c120) SHA1(84f93bcd41d5bda8cfb39c4947fff025f53b143d) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fh17",              0x00000, 0x20000, CRC(c7b0a24e) SHA1(8a068d7838bbdfb200c7104deb0cd5647336117a) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "7114.prm",          0x0000, 0x0100, CRC(eb539ffb) SHA1(6a8c9112f289f63e8c88320c9df698b559632c3d) )	/* Priority (not used) */

	ROM_REGION( 0x0c00, "plds", 0 )
	ROM_LOAD( "pal16r4a-1.bin", 0x0000, 0x0104, CRC(d28fb8e0) SHA1(73cd73a075bd3ba3b3e50f3b71a4aaecce37115f) )
	ROM_LOAD( "pal16l8b-2.bin", 0x0200, 0x0104, CRC(bcb591e3) SHA1(d3ebc2a19108c9db355d3ba1512ab4cf0d9fad76) )
	ROM_LOAD( "pal16l8a-3.bin", 0x0400, 0x0104, CRC(e12972ac) SHA1(6b178c936068d9017a1444f437aea7e2ab1c6ca9) )
	ROM_LOAD( "pal16l8a-4.bin", 0x0600, 0x0104, CRC(c6437e49) SHA1(0d89855378ab5f45d55f6aa175a63458b3da52a3) )
	ROM_LOAD( "pal16l8b-5.bin", 0x0800, 0x0104, CRC(e9ee3a67) SHA1(5299f44f1141fcd57b0559b91ec7adb51b36c5c4) )
	ROM_LOAD( "pal16l8a-6.bin", 0x0a00, 0x0104, CRC(23b17abe) SHA1(ca6c47f4df63d84401ccb29d0a0e3633b09d708a) )
ROM_END
#endif

/*
Midnight Resistance bootleg

Really nasty piece of junk, 2 huge boards.
There's also Bad Dudes running on the same
h/w (have seen one a few years ago)

basic components......
68000 @8MHz
6502
68705R3
YM2203
YM3812
M6295
XTALs 16MHz & 12MHz
2x 8 position DSWs

rom10 is missing (another PCB was found and dumped)
*/
#if 0
ROM_START( midresb )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "14.bin",         0x00000, 0x10000, CRC(d9c0f06f) SHA1(23cdc3e0613fed4e3e35094884b716d2507d59c8) )
	ROM_LOAD16_BYTE( "13.bin",         0x20000, 0x10000, CRC(d1bb2cd6) SHA1(6d4afd8dd8c4c3e90de199358da27108286637e2) )
	ROM_LOAD16_BYTE( "11.bin",         0x00001, 0x10000, CRC(1909081a) SHA1(a8cfa551b55830f3cc32e52c9a855ca525e1ab3f) )
	ROM_LOAD16_BYTE( "10.bin",         0x20001, 0x10000, CRC(42ccdd0d) SHA1(ef17cc984a8d57e9c52877f4e9b78e9976f99033) )
	ROM_LOAD16_BYTE( "12.bin",         0x40000, 0x10000, CRC(1e85a68d) SHA1(9ff778d023523302f408d80f1cbd3a7c49c044b0) )
	ROM_LOAD16_BYTE( "9.bin",          0x40001, 0x10000, CRC(1587bc2a) SHA1(0ca2abccfc52b0071b0741e1498c34d765fe38da) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 6502 sound */ // same as hippodrome / fighting fantasy...
	ROM_LOAD( "15.bin",         0x8000, 0x8000, CRC(9871b98d) SHA1(2b6c46bc2b10a28946d6ad8251e1a156a0b99947) )

	ROM_REGION( 0x10000, "cpu2", 0 )	/* 68705 MCU */
	ROM_LOAD( "68705r3.bin",              0x00000, 0x1000, CRC(ad5b1c13) SHA1(3616dc5969323a54e3e171d169f76250ae4e711a) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "23.bin",             0x08000, 0x08000, CRC(d75aba06) SHA1(cb3b969db3dd8e0c5c3729482f7461cde3a961f3) )
	ROM_CONTINUE(                   0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "24.bin",             0x18000, 0x08000, CRC(8f5bbb79) SHA1(cb10f68787606111ba5e9967bf0b0cd21269a902) )
	ROM_CONTINUE(                   0x10000, 0x08000 )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "28.bin",             0x00000, 0x10000, CRC(4490ed48) SHA1(e825c6290c65b9e3fa38f961a2174836ec2324d9) )
	ROM_LOAD( "19.bin",             0x10000, 0x10000, CRC(0f94f5c1) SHA1(235f1d8a09c0bfc51454a16c41489eb45ea29e0b) )
	ROM_LOAD( "26.bin",             0x20000, 0x10000, CRC(d4994050) SHA1(1d78ad702325013c3fd0889622b969af76d749ee) )
	ROM_LOAD( "18.bin",             0x30000, 0x10000, CRC(dc85368b) SHA1(4c5b04de63e1b58d5d8615eb561fbb90d1e16011) )
	ROM_LOAD( "27.bin",             0x40000, 0x10000, CRC(06f7ac18) SHA1(402a2c05ef7bea5afaff417bc1a1e2ba24e52eaa) )
	ROM_LOAD( "20.bin",             0x50000, 0x10000, CRC(d2679020) SHA1(5ca10cd55e8a4aa670645528f6eff33fabf0c4d7) )
	ROM_LOAD( "25.bin",             0x60000, 0x10000, CRC(323cce90) SHA1(8f5c5d0cc2bc2ded75bb4d0683f2611585b5affc) )
	ROM_LOAD( "17.bin",             0x70000, 0x10000, CRC(7c94e1b4) SHA1(5b2d036f13c9ec85b46d601d8d925cfa14d204c3) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "22.bin",             0x00000, 0x10000, CRC(68d50336) SHA1(89a1b2398796ec2392f003e1c77ba914ea90c8c2) )
	ROM_LOAD( "30.bin",             0x10000, 0x10000, CRC(efe22953) SHA1(2f4b6090c2fcd45381746ccc14c8ad8948aa096b) )
	ROM_LOAD( "21.bin",             0x20000, 0x10000, CRC(3311d7b0) SHA1(d9812cd9d8b5bd38a78c4c3a92aa2a90d78525a3) )
	ROM_LOAD( "29.bin",             0x30000, 0x10000, CRC(9210b713) SHA1(1db2775359d946221b99047fb648114a908690a9) )

	ROM_REGION( 0x80000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "8.bin",             0x00000, 0x10000, CRC(3f499acb) SHA1(1a22cfeed0497ddc2d571114d9f246b3ae18ede9) )
	ROM_LOAD( "4.bin",             0x10000, 0x10000, CRC(5e7a6800) SHA1(8dd5c9005b6804a30627644053f14e4477fe0074) )
	ROM_LOAD( "6.bin",             0x20000, 0x10000, CRC(897ba6e4) SHA1(70fd9cba3922751cb317770d6effdc2fb94c1324) )
	ROM_LOAD( "2.bin",             0x30000, 0x10000, CRC(9fefb810) SHA1(863a81540261e78de5c612dea807ba29b12054d4) )
	ROM_LOAD( "7.bin",             0x40000, 0x10000, CRC(ebafe720) SHA1(b9f76d2f1b59f1d028e6156b831c5c8ada033641) )
	ROM_LOAD( "3.bin",             0x50000, 0x10000, CRC(87aab3c2) SHA1(fc5e96505f392b95a397e412f193f9aee74d58f5) )
	ROM_LOAD( "5.bin",             0x60000, 0x10000, CRC(fd0bd8d3) SHA1(d6b19869ddc2a8ed4f38ba9d613b71853f2d13c0) )
	ROM_LOAD( "1.bin",             0x70000, 0x10000, CRC(fc46d5ed) SHA1(20ddf3f67f0dfb222ad8d3fd464b892ec9c9e4f5) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "16.bin",            0x00000, 0x10000, CRC(ccf24b52) SHA1(39b2663c548b30684197284cb8e7a6ca803330c9) )
ROM_END
#endif

ROM_START( bouldash )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fw-15-2.17l",   0x00000, 0x10000, CRC(ca19a967) SHA1(b9dc2b1323f19b6239e550ed020943bf13de8db0) )
	ROM_LOAD16_BYTE( "fw-12-2.9l",    0x00001, 0x10000, CRC(242bdc2a) SHA1(9f2d7a5af94fae4ce4e61a2f3881a7aa10cdef3f) )
	ROM_LOAD16_BYTE( "fw-16-2.19l",   0x20000, 0x10000, CRC(b7217265) SHA1(7ffc71fffb82b1299c6d5d0d1e9e1550977d258a) )
	ROM_LOAD16_BYTE( "fw-13-2.11l",   0x20001, 0x10000, CRC(19209ef4) SHA1(36d7eb242f9558ee760b6cc69e7cf8f32d01070f) )
	ROM_LOAD16_BYTE( "fw-17-2.20l",   0x40000, 0x10000, CRC(78a632a1) SHA1(6b7b82bf59cca10ac5a71b910a218a09c5014ff6) )
	ROM_LOAD16_BYTE( "fw-14-2.13l",   0x40001, 0x10000, CRC(69b6112d) SHA1(3a8e34ae858946fc72b9ed4f932b9af64b081051) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fn-10",      0x00000, 0x10000, CRC(c74106e7) SHA1(72213454c0ec78aa7d6843bd81d14b388ef7a48f) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fn-04",        0x08000, 0x08000, CRC(40f5a760) SHA1(0d08b816714c08d0848dd25882a09d0a57fcc71b) )
	ROM_CONTINUE(             0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "fn-05",        0x18000, 0x08000, CRC(824f2168) SHA1(32272a35e5faeebe41ece91fb902251707c9114b) )
	ROM_CONTINUE(             0x10000, 0x08000 )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fn-07",        0x00000, 0x10000, CRC(eac6a3b3) SHA1(359df7335d11134ae149675080169cb53cafc19c) )
	ROM_LOAD( "fn-06",        0x10000, 0x10000, CRC(3feee292) SHA1(d0dc75afffff268e0b3b817fbc060d52418a2ca7) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fn-09",        0x00000, 0x20000, CRC(c2b27bd2) SHA1(8452d4442af476a35d3dfc4bd4df0a7d84a3dd7c) )
	ROM_LOAD( "fn-08",        0x20000, 0x20000, CRC(5ac97178) SHA1(7d246cb17986033ae2c219f7409e3b91be0dd259) )

	ROM_REGION( 0x40000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fn-01",        0x00000, 0x10000, CRC(9333121b) SHA1(826ed261b1af41dd5468b2244767593d48577618) )
	ROM_LOAD( "fn-03",        0x10000, 0x10000, CRC(254ba60f) SHA1(71ab5cd48ee34da1d2dd951bb243a26d7a1171ae) )
	ROM_LOAD( "fn-00",        0x20000, 0x10000, CRC(ec18d098) SHA1(3cd1a27de295a177e81c14b9e9bbfcf5793aade2) )
	ROM_LOAD( "fn-02",        0x30000, 0x10000, CRC(4f060cba) SHA1(4063183e699bb8b6059d56f4e2fec5fa0b037c23) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fn-11",      0x00000, 0x10000, CRC(990fd8d9) SHA1(a37bd96ecd75c610d98df3320f53ae4e2b7fdefd) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "ta-16.21k",          0x0000, 0x0100, CRC(ad26e8d4) SHA1(827337aeb8904429a1c050279240ae38aa6ce064) )	/* Priority (not used) */
ROM_END

#if 0
ROM_START( bouldashj )
	ROM_REGION( 0x60000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fn-15-1.17l",  0x00000, 0x10000, CRC(d3ef20f8) SHA1(87a32a3865bec086afee5d97c0691087a41f4870) )
	ROM_LOAD16_BYTE( "fn-12-1.9l",   0x00001, 0x10000, CRC(f4a10b45) SHA1(12c42d8abc7b21fbdef4f02d588a800cef222754) )
	ROM_LOAD16_BYTE( "fn-16-.19l",   0x20000, 0x10000, CRC(fd1806a5) SHA1(fdbc8e8048d0935ee69b2b8023b5afdfe6fd9095) )
	ROM_LOAD16_BYTE( "fn-13-.11l",   0x20001, 0x10000, CRC(d24d3681) SHA1(3f822592f7db4ba10852a57ea03fbc84271d2f77) )
	ROM_LOAD16_BYTE( "fn-17-.20l",   0x40000, 0x10000, CRC(28d48a37) SHA1(7c5ddc35e7b29e5f89073ba88cd4048699f57e55) )
	ROM_LOAD16_BYTE( "fn-14-.13l",   0x40001, 0x10000, CRC(8c61c682) SHA1(4ff2b5fc61b7887775901c968c872a2853ea6dbc) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* Sound CPU */
	ROM_LOAD( "fn-10",      0x00000, 0x10000, CRC(c74106e7) SHA1(72213454c0ec78aa7d6843bd81d14b388ef7a48f) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* chars */
	ROM_LOAD( "fn-04",        0x08000, 0x08000, CRC(40f5a760) SHA1(0d08b816714c08d0848dd25882a09d0a57fcc71b) )
	ROM_CONTINUE(             0x00000, 0x08000 )	/* the two halves are swapped */
	ROM_LOAD( "fn-05",        0x18000, 0x08000, CRC(824f2168) SHA1(32272a35e5faeebe41ece91fb902251707c9114b) )
	ROM_CONTINUE(             0x10000, 0x08000 )

	ROM_REGION( 0x20000, "gfx2", 0 ) /* tiles */
	ROM_LOAD( "fn-07",        0x00000, 0x10000, CRC(eac6a3b3) SHA1(359df7335d11134ae149675080169cb53cafc19c) )
	ROM_LOAD( "fn-06",        0x10000, 0x10000, CRC(3feee292) SHA1(d0dc75afffff268e0b3b817fbc060d52418a2ca7) )

	ROM_REGION( 0x40000, "gfx3", 0 ) /* tiles */
	ROM_LOAD( "fn-09",        0x00000, 0x20000, CRC(c2b27bd2) SHA1(8452d4442af476a35d3dfc4bd4df0a7d84a3dd7c) )
	ROM_LOAD( "fn-08",        0x20000, 0x20000, CRC(5ac97178) SHA1(7d246cb17986033ae2c219f7409e3b91be0dd259) )

	ROM_REGION( 0x40000, "gfx4", 0 ) /* sprites */
	ROM_LOAD( "fn-01",        0x00000, 0x10000, CRC(9333121b) SHA1(826ed261b1af41dd5468b2244767593d48577618) )
	ROM_LOAD( "fn-03",        0x10000, 0x10000, CRC(254ba60f) SHA1(71ab5cd48ee34da1d2dd951bb243a26d7a1171ae) )
	ROM_LOAD( "fn-00",        0x20000, 0x10000, CRC(ec18d098) SHA1(3cd1a27de295a177e81c14b9e9bbfcf5793aade2) )
	ROM_LOAD( "fn-02",        0x30000, 0x10000, CRC(4f060cba) SHA1(4063183e699bb8b6059d56f4e2fec5fa0b037c23) )

	ROM_REGION( 0x40000, "oki", 0 )	/* ADPCM samples */
	ROM_LOAD( "fn-11",      0x00000, 0x10000, CRC(990fd8d9) SHA1(a37bd96ecd75c610d98df3320f53ae4e2b7fdefd) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "ta-16.21k",          0x0000, 0x0100, CRC(ad26e8d4) SHA1(827337aeb8904429a1c050279240ae38aa6ce064) )	/* Priority (not used) */
ROM_END
#endif

// helper function
#if 0
static void dump_to_file(running_machine* machine, UINT8* ROM, int offset, int size)
{
	{
		FILE *fp;
		char filename[256];
		sprintf(filename,"%s_%08x_%08x", machine->gamedrv->name, offset, size);
		fp=fopen(filename, "w+b");
		if (fp)
		{
			fwrite(ROM+offset, size, 1, fp);
			fclose(fp);
		}
	}
}


static DRIVER_INIT( convert_robocop_gfx4_to_automat )
{
	UINT8* R = memory_region(machine,"gfx4");
	int i;

	for (i=0;i<0x80000;i++)
	{
		R[i] = BITSWAP8(R[i],0,1,2,3,4,5,6,7);
	}

	dump_to_file(machine,R, 0x00000, 0x10000);
	dump_to_file(machine,R, 0x10000, 0x08000);
	dump_to_file(machine,R, 0x20000, 0x10000);
	dump_to_file(machine,R, 0x30000, 0x08000);
	dump_to_file(machine,R, 0x40000, 0x10000);
	dump_to_file(machine,R, 0x50000, 0x08000);
	dump_to_file(machine,R, 0x60000, 0x10000);
	dump_to_file(machine,R, 0x70000, 0x08000);
}
#endif
/******************************************************************************/

GAME( 1987, hbarrel,  0,        hbarrel,  hbarrel,  hbarrel,  ROT270, "Data East USA",         "Heavy Barrel (US)", 0 )
//GAME( 1987, hbarrelw, hbarrel,  hbarrel,  hbarrel,  hbarrelw, ROT270, "Data East Corporation", "Heavy Barrel (World)", 0 )
GAME( 1988, baddudes, 0,        baddudes, baddudes, baddudes, ROT0,   "Data East USA",         "Bad Dudes vs. Dragonninja (US)", 0 )
//GAME( 1988, drgninja, baddudes, baddudes, baddudes, baddudes, ROT0,   "Data East Corporation", "Dragonninja (Japan)", 0 )
//GAME( 1988, birdtry,  0,        birdtry,  birdtry,  birdtry,  ROT270, "Data East Corporation", "Birdie Try (Japan)", GAME_UNEMULATED_PROTECTION )
GAME( 1988, robocop,  0,        robocop,  robocop,  robocop,  ROT0,   "Data East Corporation", "Robocop (World revision 4)", 0 )
//GAME( 1988, robocopw, robocop,  robocop,  robocop,  robocop,  ROT0,   "Data East Corporation", "Robocop (World revision 3)", 0 )
//GAME( 1988, robocopj, robocop,  robocop,  robocop,  robocop,  ROT0,   "Data East Corporation", "Robocop (Japan)", 0 )
//GAME( 1988, robocopu, robocop,  robocop,  robocop,  robocop,  ROT0,   "Data East USA",         "Robocop (US revision 1)", 0 )
//GAME( 1988, robocopu0,robocop,  robocop,  robocop,  robocop,  ROT0,   "Data East USA",         "Robocop (US revision 0)", 0 )
//GAME( 1988, robocopb, robocop,  robocopb, robocop,  robocop,  ROT0,   "bootleg",               "Robocop (World bootleg)", 0)
//GAME( 1988, automat,  robocop,  automat,  robocop,  robocop,  ROT0,   "bootleg",               "Automat (bootleg of Robocop)", GAME_NOT_WORKING )
GAME( 1989, hippodrm, 0,        hippodrm, hippodrm, hippodrm, ROT0,   "Data East USA",         "Hippodrome (US)", 0 )
//GAME( 1989, ffantasy, hippodrm, hippodrm, hippodrm, hippodrm, ROT0,   "Data East Corporation", "Fighting Fantasy (Japan revision 2)", 0 )
//GAME( 1989, ffantasya,hippodrm, hippodrm, hippodrm, hippodrm, ROT0,   "Data East Corporation", "Fighting Fantasy (Japan)", 0 )
GAME( 1989, slyspy,   0,        slyspy,   slyspy,   slyspy,   ROT0,   "Data East USA",         "Sly Spy (US revision 3)", 0 )
//GAME( 1989, slyspy2,  slyspy,   slyspy,   slyspy,   slyspy,   ROT0,   "Data East USA",         "Sly Spy (US revision 2)", 0 )
//GAME( 1989, secretag, slyspy,   slyspy,   slyspy,   slyspy,   ROT0,   "Data East Corporation", "Secret Agent (World)", 0 )
//GAME( 1989, secretab, slyspy,   secretab, slyspy,   slyspy,   ROT0,   "bootleg",               "Secret Agent (bootleg)", GAME_NOT_WORKING )
GAME( 1989, midres,   0,        midres,   midres,   0,        ROT0,   "Data East Corporation", "Midnight Resistance (World)", 0 )
//GAME( 1989, midresu,  midres,   midres,   midres,   0,        ROT0,   "Data East USA",         "Midnight Resistance (US)", 0 )
//GAME( 1989, midresj,  midres,   midres,   midres,   0,        ROT0,   "Data East Corporation", "Midnight Resistance (Japan)", 0 )
//GAME( 1989, midresb,  midres,   midres,   midres,   0,        ROT0,   "bootleg",               "Midnight Resistance (bootleg with 68705)", GAME_NOT_WORKING ) // need to hook up 68705
GAME( 1990, bouldash, 0,        slyspy,   bouldash, slyspy,   ROT0,   "Data East Corporation (licensed from First Star)", "Boulder Dash / Boulder Dash Part 2 (World)", 0 )
//GAME( 1990, bouldashj,bouldash, slyspy,   bouldash, slyspy,   ROT0,   "Data East Corporation (licensed from First Star)", "Boulder Dash / Boulder Dash Part 2 (Japan)", 0 )
