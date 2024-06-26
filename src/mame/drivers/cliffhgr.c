/*********************************************************

    Stern Cliffhanger Laserdisc Hardware

    Driver by Ernesto Corvi

Hardware description:
- Laserdisc Player is a Pioneer PR-8210
- Optionally, you can use a Pioneer LD-V1100 through the Infrared Port
- Main Processor is a Z80 at 4 MHz
- Video chip is a TMS9128NL with 16KB of VRAM

Serial protocol:
- General information:
0's and 1's are transmitted by signal space differential.
A short space between ACTIVE signals means a 0, and a long space means a 1.
- Wired: Each 'Active' pulse is sent by pulsing the wire.
- Infrared: Each 'Active' pulse is sent by toggling the Infrared LED 10 times on and off.

More info on the PR-8210:
http://www.laserdiscarchive.co.uk/laserdisc_archive/pioneer/pioneer_pr-8210/pioneer_pr-8210.htm

More info on the LD-V1100:
http://www.laserdiscarchive.co.uk/laserdisc_archive/pioneer/pioneer_ld-1100/pioneer_ld-1100.htm

Interrupts:
The frame decoder reads in the Phillips code from the composite signal into
3x8 bit flip flops. If bit 7 of the code is set, then an IRQ is generated.
Phillips codes come in scanline 17 and 18 of the composite signal for each
field, so if we have valid codes, we would have 4 irq's per frame.
NMIs are triggered by the TMS9128NL chip. The TMS9128NL SYNC signal is hooked
up to the composite SYNC signal from the frame decoder.

Goal To Go Side detection:
The side detection code expects to read a chapter Phillips code of 0x881DDD
for Side 1, or 0x8F7DDD for Side 2. That would be chapter 1 for Side 1, or
chapter number 119 for Side 2.

Audio:
The lower two bits on Port $46 enable each two fixed-tone generators that
use a 555 to generate the waveform for the 'blip' sounds.

IO Ports:
0x39: R ????????
0x44: W TMS9128NL VRAM Port
0x45: R TMS9128NL VRAM Port
0x46: W Sound/Overlay
0x50: R Reads lower byte of Phillips code
0x51: R Reads middle byte of Phillips code
0x52: R Reads high byte of Phillips code
0x53: R Clears the flip flop that generated the IRQ
0x54: W TMS9128NL REG Port
0x55: R TMS9128NL REG Port
0x57: W Clears the serial->parallel chips of the Phillips code reader.
0x60: W Input Port/Dipswitch selector
0x62: R Input Port/Dipswitch data read
0x64: - Unused in the schematics, but used in the code (maybe as delay?)
0x66: LD Data Port (D0 is serial line)
0x68: W Coin Counter(D6)
0x6A: W /LAMP0 (Infrared?) (D4/D5)
0x6C: - Unused
0x6E: W CPU Board Test LED ON
0x6F: W CPU Board Test LED OFF

Sound/Overlay:
bit 0: Enable tone generator 1
bit 1: Enable tone generator 2
bit 4: Enable/Disable video overlay

GTG Side select codes:
Side 1 = 0x881DDD (or 0x880000 | ( 0x01 << 12 ) | 0x0DDD)
Side 2 = 0x8F7DDD (or 0x880000 | ( 0x77 << 12 ) | 0x0DDD)

*********************************************************/
#if 0

#include "driver.h"
#include "cpu/z80/z80.h"
#include "render.h"
#include "machine/laserdsc.h"
#include "video/tms9928a.h"
#include "sound/discrete.h"

#define CLIFF_ENABLE_SND_1	NODE_01
#define CLIFF_ENABLE_SND_2	NODE_02

static const device_config *laserdisc;

static int port_bank = 0;
static int phillips_code = 0;

static emu_timer *irq_timer;

/********************************************************/

static WRITE8_HANDLER( cliff_test_led_w )
{
	set_led_status(0, offset ^ 1);
}

static WRITE8_HANDLER( cliff_port_bank_w )
{
	/* writing 0x0f clears the LS174 flip flop */
	if (data == 0x0f)
		port_bank = 0;
	else
		port_bank = data & 0x0f; /* only D3-D0 are connected */
}

static READ8_HANDLER( cliff_port_r )
{
	static const char *const banknames[] = { "BANK0", "BANK1", "BANK2", "BANK3", "BANK4", "BANK5", "BANK6" };

	if (port_bank < 7)
		return input_port_read(space->machine,  banknames[port_bank]);

	/* output is pulled up for non-mapped ports */
	return 0xff;
}

static READ8_HANDLER( cliff_phillips_code_r )
{
	if (laserdisc != NULL)
		return (phillips_code >> (8 * offset)) & 0xff;

	return 0x00;
}

static WRITE8_HANDLER( cliff_phillips_clear_w )
{
	/* reset serial to parallel converters */
}

static WRITE8_HANDLER( cliff_coin_counter_w )
{
	coin_counter_w(0, (data & 0x40) ? 1 : 0 );
}

static READ8_HANDLER( cliff_irq_ack_r )
{
	/* deassert IRQ on the CPU */
	cputag_set_input_line(space->machine, "maincpu", 0, CLEAR_LINE);

	return 0x00;
}

static WRITE8_DEVICE_HANDLER( cliff_sound_overlay_w )
{
	int sound = data & 3;
	int overlay = (data & 0x10) ? 1 : 0;

	/* configure pen 0 and 1 as transparent in the renderer and use it as the compositing color */
	if (overlay)
	{
		palette_set_color(device->machine, 0, palette_get_color(device->machine, 0) & MAKE_ARGB(0,255,255,255));
		palette_set_color(device->machine, 1, palette_get_color(device->machine, 1) & MAKE_ARGB(0,255,255,255));
	}
	else
	{
		palette_set_color(device->machine, 0, palette_get_color(device->machine, 0) | MAKE_ARGB(255,0,0,0));
		palette_set_color(device->machine, 1, palette_get_color(device->machine, 1) | MAKE_ARGB(255,0,0,0));
	}

	/* audio */
	discrete_sound_w(device, CLIFF_ENABLE_SND_1, sound & 1);
	discrete_sound_w(device, CLIFF_ENABLE_SND_2, (sound >> 1) & 1);
}

static WRITE8_HANDLER( cliff_ldwire_w )
{
	laserdisc_line_w(laserdisc, LASERDISC_LINE_CONTROL, (data & 1) ? ASSERT_LINE : CLEAR_LINE);
}


/********************************************************/

static INTERRUPT_GEN( cliff_vsync )
{
	/* clock the video chip every 60Hz */
	TMS9928A_interrupt(device->machine);
}

static TIMER_CALLBACK( cliff_irq_callback )
{
	phillips_code = 0;

	switch (param)
	{
		case 17:
			phillips_code = laserdisc_get_field_code(laserdisc, LASERDISC_CODE_LINE17, TRUE);
			param = 18;
			break;

		case 18:
			phillips_code = laserdisc_get_field_code(laserdisc, LASERDISC_CODE_LINE18, TRUE);
			param = 17;
			break;
	}

	/* if we have a valid code, trigger an IRQ */
	if (phillips_code & 0x800000)
	{
//      printf("%2d:code = %06X\n", param, phillips_code);
		cputag_set_input_line(machine, "maincpu", 0, ASSERT_LINE);
	}

	timer_adjust_oneshot(irq_timer, video_screen_get_time_until_pos(machine->primary_screen, param * 2, 0), param);
}

static void vdp_interrupt(running_machine *machine, int state)
{
	cputag_set_input_line(machine, "maincpu", INPUT_LINE_NMI, state ? ASSERT_LINE : CLEAR_LINE);
}



static MACHINE_START( cliffhgr )
{
	laserdisc = devtag_get_device(machine, "laserdisc");
	irq_timer = timer_alloc(machine, cliff_irq_callback, NULL);
}

static MACHINE_RESET( cliffhgr )
{
	port_bank = 0;
	phillips_code = 0;
	timer_adjust_oneshot(irq_timer, video_screen_get_time_until_pos(machine->primary_screen, 17, 0), 17);
}

/********************************************************/

static ADDRESS_MAP_START( mainmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_ROM		/* ROM */
	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)	/* NVRAM */
	AM_RANGE(0xe800, 0xefff) AM_RAM		/* RAM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( mainport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x44, 0x44) AM_WRITE(TMS9928A_vram_w)
	AM_RANGE(0x45, 0x45) AM_READ(TMS9928A_vram_r)
	AM_RANGE(0x46, 0x46) AM_DEVWRITE("discrete", cliff_sound_overlay_w)
	AM_RANGE(0x50, 0x52) AM_READ(cliff_phillips_code_r)
	AM_RANGE(0x53, 0x53) AM_READ(cliff_irq_ack_r)
	AM_RANGE(0x54, 0x54) AM_WRITE(TMS9928A_register_w)
	AM_RANGE(0x55, 0x55) AM_READ(TMS9928A_register_r)
	AM_RANGE(0x57, 0x57) AM_WRITE(cliff_phillips_clear_w)
	AM_RANGE(0x60, 0x60) AM_WRITE(cliff_port_bank_w)
	AM_RANGE(0x62, 0x62) AM_READ(cliff_port_r)
	AM_RANGE(0x64, 0x64) AM_WRITENOP /* unused in schematics, may be used as timing delay for IR interface */
	AM_RANGE(0x66, 0x66) AM_WRITE(cliff_ldwire_w)
	AM_RANGE(0x68, 0x68) AM_WRITE(cliff_coin_counter_w)
	AM_RANGE(0x6a, 0x6a) AM_WRITENOP /* /LAMP0 (Infrared?) */
	AM_RANGE(0x6e, 0x6f) AM_WRITE(cliff_test_led_w)
ADDRESS_MAP_END


/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( cliffhgr )
	PORT_START("BANK0")
	PORT_BIT ( 0x3F, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_SERVICE2 )	/* SW2 on CPU PCB */
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_SERVICE1 )	/* SW1 on CPU PCB */

	PORT_START("BANK1")
	PORT_DIPNAME( 0xc0, 0xc0, "Should Have Hint" )		PORT_DIPLOCATION("E11:7,8")
	PORT_DIPSETTING(    0xc0, "Never" )
	PORT_DIPSETTING(    0x80, "After 1st Player Mistake" )
	PORT_DIPSETTING(    0x40, "After 2nd Player Mistake" )
	PORT_DIPSETTING(    0x00, "After 3rd Player Mistake" )
	PORT_DIPNAME( 0x20, 0x00, "Action/Stick Hints" )		PORT_DIPLOCATION("E11:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Display Score and Lives During Animation" )	PORT_DIPLOCATION("E11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Regular Length Scenes" )	PORT_DIPLOCATION("E11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "No Hanging Scene" )		PORT_DIPLOCATION("E11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )		PORT_DIPLOCATION("E11:1,2")
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )

	PORT_START("BANK2")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_B ) )		PORT_DIPLOCATION("F11:5,6")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )

	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )		PORT_DIPLOCATION("F11:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )

	PORT_START("BANK3")
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Allow_Continue ) )	PORT_DIPLOCATION("G11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Short Scenes" )			PORT_DIPLOCATION("G11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ) )	PORT_DIPLOCATION("G11:6")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Disc Test" )				PORT_DIPLOCATION("G11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Player Immortality" )	PORT_DIPLOCATION("G11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Free_Play ) )	PORT_DIPLOCATION("G11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Switch Test" )			PORT_DIPLOCATION("G11:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Service Index" )			PORT_DIPLOCATION("G11:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("BANK4")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0f, 0x0f, "Move Difficulty" )		PORT_DIPLOCATION("H11:1,2,3,4")
	PORT_DIPSETTING(    0x0f, "0 (Easiest)" )
	PORT_DIPSETTING(    0x0e, "1" )
	PORT_DIPSETTING(    0x0d, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x0b, "4" )
	PORT_DIPSETTING(    0x0a, "5" )
	PORT_DIPSETTING(    0x09, "6" )
	PORT_DIPSETTING(    0x08, "7" )
	PORT_DIPSETTING(    0x07, "8" )
	PORT_DIPSETTING(    0x06, "9" )
	PORT_DIPSETTING(    0x05, "10" )
	PORT_DIPSETTING(    0x04, "11" )
	PORT_DIPSETTING(    0x03, "12" )
	PORT_DIPSETTING(    0x02, "13" )
	PORT_DIPSETTING(    0x01, "14" )
	PORT_DIPSETTING(    0x00, "15 (Hardest)" )

	PORT_START("BANK5")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_TILT )

	PORT_START("BANK6")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT ( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( cliffhga )
	PORT_START("BANK0")
	PORT_BIT ( 0x3F, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_SERVICE2 )	/* SW2 on CPU PCB */
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_SERVICE1 )	/* SW1 on CPU PCB */

	PORT_START("BANK1")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )		PORT_DIPLOCATION("E11:1,2")
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "6" )

	PORT_START("BANK2")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_B ) )		PORT_DIPLOCATION("F11:5,6")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
//  PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_1C ) )

	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )		PORT_DIPLOCATION("F11:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
//  PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )

	PORT_START("BANK3")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("G11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Play Thru/Random" )		PORT_DIPLOCATION("G11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ) )	PORT_DIPLOCATION("G11:6")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Scene Jump/Disc Test" )	PORT_DIPLOCATION("G11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Immortality" )			PORT_DIPLOCATION("G11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Free_Play ) )	PORT_DIPLOCATION("G11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Switch Test" )			PORT_DIPLOCATION("G11:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Service Index" )			PORT_DIPLOCATION("G11:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("BANK4")
	PORT_DIPNAME( 0xf0, 0xf0, "Hint Difficulty" )		PORT_DIPLOCATION("H11:5,6,7,8")
	PORT_DIPSETTING(    0xf0, "0 (Most Hints)" )
	PORT_DIPSETTING(    0xe0, "1" )
	PORT_DIPSETTING(    0xd0, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0xb0, "4" )
	PORT_DIPSETTING(    0xa0, "5" )
	PORT_DIPSETTING(    0x90, "6" )
	PORT_DIPSETTING(    0x80, "7" )
	PORT_DIPSETTING(    0x70, "8" )
	PORT_DIPSETTING(    0x60, "9" )
	PORT_DIPSETTING(    0x50, "10" )
	PORT_DIPSETTING(    0x40, "11" )
	PORT_DIPSETTING(    0x30, "12" )
	PORT_DIPSETTING(    0x20, "13" )
	PORT_DIPSETTING(    0x10, "14" )
	PORT_DIPSETTING(    0x00, "15 (Least Hints)" )
	PORT_DIPNAME( 0x0f, 0x0f, "Move Difficulty" )		PORT_DIPLOCATION("H11:1,2,3,4")
	PORT_DIPSETTING(    0x0f, "0 (Easiest)" )
	PORT_DIPSETTING(    0x0e, "1" )
	PORT_DIPSETTING(    0x0d, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x0b, "4" )
	PORT_DIPSETTING(    0x0a, "5" )
	PORT_DIPSETTING(    0x09, "6" )
	PORT_DIPSETTING(    0x08, "7" )
	PORT_DIPSETTING(    0x07, "8" )
	PORT_DIPSETTING(    0x06, "9" )
	PORT_DIPSETTING(    0x05, "10" )
	PORT_DIPSETTING(    0x04, "11" )
	PORT_DIPSETTING(    0x03, "12" )
	PORT_DIPSETTING(    0x02, "13" )
	PORT_DIPSETTING(    0x01, "14" )
	PORT_DIPSETTING(    0x00, "15 (Hardest)" )

	PORT_START("BANK5")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_TILT )

	PORT_START("BANK6")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT ( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( goaltogo )
	PORT_START("BANK0")
	PORT_BIT ( 0x3F, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_SERVICE2 )	/* SW2 on CPU PCB */
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_SERVICE1 )	/* SW1 on CPU PCB */

	PORT_START("BANK1")
	PORT_DIPNAME( 0x80, 0x80, "Should Have Hint" )		PORT_DIPLOCATION("E11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Max. Game Time Timer" )	PORT_DIPLOCATION("E11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Action/Stick Hints" )	PORT_DIPLOCATION("E11:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )		PORT_DIPLOCATION("E11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Single Coin Continue" ) 	PORT_DIPLOCATION("E11:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) ) PORT_CONDITION("BANK1",0x01,PORTCOND_EQUALS,0x00)
	PORT_DIPSETTING(    0x00, DEF_STR( On ) ) PORT_CONDITION("BANK1",0x01,PORTCOND_EQUALS,0x00)
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Allow_Continue ) )	PORT_DIPLOCATION("E11:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("BANK2")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_B ) )		PORT_DIPLOCATION("F11:5,6")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )

	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )		PORT_DIPLOCATION("F11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )		PORT_DIPLOCATION("F11:1,2")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )

	PORT_START("BANK3")
	PORT_DIPNAME( 0x80, 0x00, "Display Diagram Before Play" )	PORT_DIPLOCATION("G11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "Display Score During Game" )		PORT_DIPLOCATION("G11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Demo_Sounds ) )	PORT_DIPLOCATION("G11:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "Scene Jump/Disc Test" )	PORT_DIPLOCATION("G11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Immortality" )			PORT_DIPLOCATION("G11:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Free_Play ) )	PORT_DIPLOCATION("G11:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Switch Test" )			PORT_DIPLOCATION("G11:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, "Service Index" )			PORT_DIPLOCATION("G11:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("BANK4")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )		PORT_DIPLOCATION("H11:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0f, 0x0f, "Move Difficulty" )		PORT_DIPLOCATION("H11:1,2,3,4")
	PORT_DIPSETTING(    0x0f, "0 (Easiest)" )
	PORT_DIPSETTING(    0x0e, "1" )
	PORT_DIPSETTING(    0x0d, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x0b, "4" )
	PORT_DIPSETTING(    0x0a, "5" )
	PORT_DIPSETTING(    0x09, "6" )
	PORT_DIPSETTING(    0x08, "7" )
	PORT_DIPSETTING(    0x07, "8" )
	PORT_DIPSETTING(    0x06, "9" )
	PORT_DIPSETTING(    0x05, "10" )
	PORT_DIPSETTING(    0x04, "11" )
	PORT_DIPSETTING(    0x03, "12" )
	PORT_DIPSETTING(    0x02, "13" )
	PORT_DIPSETTING(    0x01, "14" )
	PORT_DIPSETTING(    0x00, "15 (Hardest)" )

	PORT_START("BANK5")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT ( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT ( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT ( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT ( 0x80, IP_ACTIVE_LOW, IPT_TILT )

	PORT_START("BANK6")
	PORT_BIT ( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT ( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT ( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT ( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT ( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



/*************************************
 *
 *  Video/sound interfaces
 *
 *************************************/

static const TMS9928a_interface tms9928a_interface =
{
	TMS99x8A,		/* TMS9128NL on the board */
	0x4000,
	0,0,
	vdp_interrupt
};

DISCRETE_SOUND_EXTERN( cliffhgr );



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( cliffhgr )

	MDRV_CPU_ADD("maincpu", Z80, 4000000)       /* 4MHz */
	MDRV_CPU_PROGRAM_MAP(mainmem)
	MDRV_CPU_IO_MAP(mainport)
	MDRV_CPU_VBLANK_INT("screen", cliff_vsync)

	MDRV_MACHINE_START(cliffhgr)
	MDRV_MACHINE_RESET(cliffhgr)

	MDRV_NVRAM_HANDLER(generic_0fill)

	MDRV_LASERDISC_ADD("laserdisc", PIONEER_PR8210, "screen", "ldsound")
	MDRV_LASERDISC_OVERLAY(tms9928a, 15+32*8+15, 27+24*8+24, BITMAP_FORMAT_INDEXED16)
	MDRV_LASERDISC_OVERLAY_CLIP(15-12, 15+32*8+12-1, 27-9, 27+24*8+9-1)

	/* start with the TMS9928a video configuration */
	MDRV_IMPORT_FROM(tms9928a)

	/* override video rendering and raw screen info */
	MDRV_DEVICE_REMOVE("screen")
	MDRV_LASERDISC_SCREEN_ADD_NTSC("screen", BITMAP_FORMAT_INDEXED16)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD("ldsound", LASERDISC, 0)
	MDRV_SOUND_ROUTE(0, "lspeaker", 1.0)
	MDRV_SOUND_ROUTE(1, "rspeaker", 1.0)

	MDRV_SOUND_ADD("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG_DISCRETE(cliffhgr)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.0)
MACHINE_DRIVER_END



/*************************************
*
*  ROM definitions
*
*************************************/

ROM_START( cliffhgr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cliff_u1.bin",	0x0000, 0x2000, CRC(a86ec38f) SHA1(bfca1b1c084f5b7b1e0ccb2f3616ecea1340f04c) )
	ROM_LOAD( "cliff_u2.bin",	0x2000, 0x2000, CRC(b8d33b6b) SHA1(02778f87a78199129c758a8fb0629b9ba74cab99) )
	ROM_LOAD( "cliff_u3.bin",	0x4000, 0x2000, CRC(75a64cd2) SHA1(18fe4d8885b59ec8b8c28b5d7141a27164c982ac) )
	ROM_LOAD( "cliff_u4.bin",	0x6000, 0x2000, CRC(906b2af1) SHA1(65fadd2fec90f47c91ac4928f342c79ab8bc6ef0) )
	ROM_LOAD( "cliff_u5.bin",	0x8000, 0x2000, CRC(5922e710) SHA1(10637baba4d16dc333aeb0ab88ee251f44e1a115) )

	DISK_REGION( "laserdisc" )
	DISK_IMAGE_READONLY( "cliffhgr", 0, SHA1(4442995c824d7891a2a19c607bb3301d696fbdc8) )
ROM_END

ROM_START( cliffhgra )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cliff_alt_0.bin",	0x0000, 0x2000, CRC(27caa67c) SHA1(70d8270766b8712d4250b1a23489007d59eb262f) )
	ROM_LOAD( "cliff_alt_1.bin",	0x2000, 0x2000, CRC(6e5f1515) SHA1(1c4116f4f5910857408826d73c630abbf1434119) )
	ROM_LOAD( "cliff_alt_2.bin",	0x4000, 0x2000, CRC(045f895d) SHA1(364e259a9630d87ca917c7a9dc1a94d6f0d0eba5) )
	ROM_LOAD( "cliff_alt_3.bin",	0x6000, 0x2000, CRC(54cdb4a1) SHA1(6b1d73aec029af4a88ca2f883b4ed706d153592d) )

	DISK_REGION( "laserdisc" )
	DISK_IMAGE_READONLY( "cliffhgr", 0, SHA1(4442995c824d7891a2a19c607bb3301d696fbdc8) )
ROM_END

ROM_START( goaltogo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "gtg.rm0",	0x0000, 0x2000, CRC(d8efddea) SHA1(69a076fed60ebabad3032d8c10804f57a0904327) )
	ROM_LOAD( "gtg.rm1",	0x2000, 0x2000, CRC(69953d38) SHA1(2a51aa785a4576db8b046e128bbfc1b3949d7bf7) )
	ROM_LOAD( "gtg.rm2",	0x4000, 0x2000, CRC(b043e205) SHA1(8992c0e294f59bd9331fb3a50a0dfd8d5c194fa3) )
	ROM_LOAD( "gtg.rm3",	0x6000, 0x2000, CRC(ec305f5e) SHA1(e205fac699db4ca28a87f56f89cc6cf185ad540d) )
	ROM_LOAD( "gtg.rm4",	0x8000, 0x2000, CRC(9e4c8aa2) SHA1(002c0940d3890141f85f98f854fd30cc1e340d45) )

	DISK_REGION( "laserdisc" )
	DISK_IMAGE_READONLY( "goaltog1", 0, NO_DUMP )
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( cliff )
{
	TMS9928A_configure(&tms9928a_interface);
}
#endif


/*************************************
 *
 *  Game drivers
 *
 *************************************/

//GAME( 1983, cliffhgr, 0,        cliffhgr, cliffhgr, cliff, ROT0, "Stern Electronics", "Cliff Hanger", 0)
//GAME( 1983, cliffhgra,cliffhgr, cliffhgr, cliffhga, cliff, ROT0, "Stern Electronics", "Cliff Hanger (Alt)", 0)
//GAME( 1983, goaltogo, 0,        cliffhgr, goaltogo, cliff, ROT0, "Stern Electronics", "Goal To Go", GAME_NOT_WORKING)
