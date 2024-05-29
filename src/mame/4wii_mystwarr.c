/******************************************************************************

    4wii_mystwarr.c

    driver info for the subtarget Konami Mystic Warriors.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

    The list of all available drivers. Drivers have to be included here to be
    recognized by the executable.

    To save some typing, we use a hack here. This file is recursively #included
    twice, with different definitions of the DRIVER() macro. The first one
    declares external references to the drivers; the second one builds an array
    storing all the drivers.

******************************************************************************/
//  WII Note
//  Games with //# are not working (or not supported)
//  Games with //- are clones of working games
//  Games with /**/ are BIOS or not working ones needed by working clones
//  Games with //S are excluded because lack sounds
//  Games with //U are unsupported due the size
//  Games with //N are shit excluded by me (Nebiun)
//  Games that require CHD files are not supported

#include "driver.h"

#ifndef DRIVER_RECURSIVE

#define DRIVER_RECURSIVE

/* step 1: declare all external references */
#define DRIVER(NAME) GAME_EXTERN(NAME);
#include "4wii_mystwarr.c"

/* step 2: define the drivers[] array */
#undef DRIVER
#define DRIVER(NAME) &GAME_NAME(NAME),
const game_driver * const drivers[] __attribute__((aligned(32))) =
{
#include "4wii_mystwarr.c"
    0   /* end of array */
};

#else   /* DRIVER_RECURSIVE */

    /* Konami 'Mystic Warriors' hardware games */
    DRIVER( gaiapols )  /* GX123 (c) 1993 (Europe) */
    DRIVER( gaiapolsu ) /* GX123 (c) 1993 (US) */
    DRIVER( gaiapolsj ) /* GX123 (c) 1993 (Japan) */
    DRIVER( mystwarr )  /* GX128 (c) 1993 (World) */
    DRIVER( mystwarru ) /* GX128 (c) 1993 (US) */
    DRIVER( mystwarrj ) /* GX128 (c) 1993 (Japan) */
    DRIVER( viostorm )  /* GX168 (c) 1993 (Europe) */
    DRIVER( viostormu ) /* GX168 (c) 1993 (US) */
    DRIVER( viostormub )    /* GX168 (c) 1993 (US) */
    DRIVER( viostormj ) /* GX168 (c) 1993 (Japan) */
    DRIVER( viostorma ) /* GX168 (c) 1993 (Asia) */
    DRIVER( mmaulers )  /* GX170 (c) 1993 (Europe) */
    DRIVER( dadandrn )  /* GX170 (c) 1993 (Japan) */
    DRIVER( metamrph )  /* GX224 (c) 1993 (Europe) */
    DRIVER( metamrphu ) /* GX224 (c) 1993 (US) */
    DRIVER( metamrphj ) /* GX224 (c) 1993 (Japan) */
    DRIVER( mtlchamp )  /* GX234 (c) 1993 (World) */
    DRIVER( mtlchamp1 ) /* GX234 (c) 1993 (World) */
    DRIVER( mtlchampu ) /* GX234 (c) 1993 (US) */
    DRIVER( mtlchampj ) /* GX234 (c) 1993 (Japan) */
    DRIVER( mtlchampa ) /* GX234 (c) 1993 (Japan) */

#endif  /* DRIVER_RECURSIVE */
