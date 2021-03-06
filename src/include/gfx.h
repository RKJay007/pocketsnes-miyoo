/*******************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002 Gary Henderson (gary.henderson@ntlworld.com) and
			    Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2001 - 2004 John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2004 Brad Jorsch (anomie@users.sourceforge.net),
			    funkyass (funkyass@spam.shaw.ca),
			    Joel Yliluoma (http://iki.fi/bisqwit/)
			    Kris Bleakley (codeviolation@hotmail.com),
			    Matthew Kendora,
			    Nach (n-a-c-h@users.sourceforge.net),
			    Peter Bortas (peter@bortas.org) and
			    zones (kasumitokoduck@yahoo.com)

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003 zsKnight (zsknight@zsnes.com),
			    _Demo_ (_demo_@zsnes.com), and Nach

  C4 C++ code
  (c) Copyright 2003 Brad Jorsch

  DSP-1 emulator code
  (c) Copyright 1998 - 2004 Ivar (ivar@snes9x.com), _Demo_, Gary Henderson,
			    John Weidman, neviksti (neviksti@hotmail.com),
			    Kris Bleakley, Andreas Naive

  DSP-2 emulator code
  (c) Copyright 2003 Kris Bleakley, John Weidman, neviksti, Matthew Kendora, and
		     Lord Nightmare (lord_nightmare@users.sourceforge.net

  OBC1 emulator code
  (c) Copyright 2001 - 2004 zsKnight, pagefault (pagefault@zsnes.com) and
			    Kris Bleakley
  Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code
  (c) Copyright 2002 Matthew Kendora with research by
		     zsKnight, John Weidman, and Dark Force

  S-DD1 C emulator code
  (c) Copyright 2003 Brad Jorsch with research by
		     Andreas Naive and John Weidman

  S-RTC C emulator code
  (c) Copyright 2001 John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003 Feather, Kris Bleakley, John Weidman and Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003 zsKnight, _Demo_, and pagefault

  Super FX C emulator code
  (c) Copyright 1997 - 1999 Ivar, Gary Henderson and John Weidman


  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004 Marcus Comstedt (marcus@mc.pp.se)


  Specific ports contains the works of other authors. See headers in
  individual files.

  Snes9x homepage: http://www.snes9x.com

  Permission to use, copy, modify and distribute Snes9x in both binary and
  source form, for non-commercial purposes, is hereby granted without fee,
  providing that this license information and copyright notice appear with
  all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes
  charging money for Snes9x or software derived from Snes9x.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
*******************************************************************************/

#ifndef _GFX_H_
#define _GFX_H_

#include "port.h"
#include "snes9x.h"

START_EXTERN_C
void S9xStartScreenRefresh();
void S9xDrawScanLine(uint8 Line);
void S9xEndScreenRefresh();
void S9xSetupOBJ();
void S9xUpdateScreen();
void RenderLine(uint8 line);
void S9xBuildDirectColourMaps();

// External port interface which must be implemented or initialised for each
// port.
extern struct SGFX GFX;

bool8 S9xGraphicsInit();
void S9xGraphicsDeinit();
bool8 S9xInitUpdate(void);
bool8 S9xDeinitUpdate(int Width, int Height, bool8 sixteen_bit);
void S9xSyncSpeed();

#ifdef GFX_MULTI_FORMAT
bool8 S9xSetRenderPixelFormat(int format);
#endif

END_EXTERN_C

struct SGFX {
	// Initialize these variables
	uint8 *Screen;
	uint8 *SubScreen;
	uint8 *ZBuffer;
	uint8 *SubZBuffer;
	uint32 Pitch;

	// Setup in call to S9xGraphicsInit()
	int Delta;
	uint16 *ZERO;
	uint32 RealPitch; // True pitch of Screen buffer.
	uint32 Pitch2;	  // Same as RealPitch except while using speed up hack for Glide.
	uint32 ZPitch;	  // Pitch of ZBuffer
	uint32 PPL;	  // Number of pixels on each of Screen buffer
	uint32 PPLx2;
	uint32 PixSize;
	uint8 *S;
	uint8 *DB;
	uint32 DepthDelta;
	uint8 Z1;      // Depth for comparison
	uint8 Z2;      // Depth to save
	uint8 ZSprite; // Used to ensure only 1st sprite is drawn per pixel
	uint32 FixedColour;
	const char *InfoString;
	uint32 InfoStringTimeout;
	uint32 StartY;
	uint32 EndY;
	struct ClipData *pCurrentClip;
	uint32 Mode7Mask;
	uint32 Mode7PriorityMask;
	uint8 OBJWidths[128];
	uint8 OBJVisibleTiles[128];
	struct {
		uint8 RTOFlags;
		int16 Tiles;
		struct {
			int8 Sprite;
			uint8 Line;
		} OBJ[32];
	} OBJLines[SNES_HEIGHT_EXTENDED];

	uint8 r212c;
	uint8 r212d;
	uint8 r2130;
	uint8 r2131;
	bool8 Pseudo;

#ifdef GFX_MULTI_FORMAT
	uint32 PixelFormat;
	uint32 (*BuildPixel)(uint32 R, uint32 G, uint32 B);
	uint32 (*BuildPixel2)(uint32 R, uint32 G, uint32 B);
	void (*DecomposePixel)(uint32 Pixel, uint32 &R, uint32 &G, uint32 &B);
#endif
};

struct SLineData {
	struct {
		uint16 VOffset;
		uint16 HOffset;
	} BG[4];
};

#define H_FLIP 0x4000
#define V_FLIP 0x8000
#define BLANK_TILE 2

struct SBG {
	uint32 TileSize;
	uint32 BitShift;
	uint32 TileShift;
	uint32 TileAddress;
	uint32 NameSelect;
	uint32 SCBase;

	uint32 StartPalette;
	uint32 PaletteShift;
	uint32 PaletteMask;

	uint8 *Buffer;
	uint8 *Buffered;
	bool8 DirectColourMode;
};

struct SLineMatrixData {
	short MatrixA;
	short MatrixB;
	short MatrixC;
	short MatrixD;
	short CentreX;
	short CentreY;
};

extern uint32 odd_high[4][16];
extern uint32 odd_low[4][16];
extern uint32 even_high[4][16];
extern uint32 even_low[4][16];
extern SBG BG;
extern uint16 DirectColourMaps[8][256];

extern uint8 add32_32[32][32];
extern uint8 add32_32_half[32][32];
extern uint8 sub32_32[32][32];
extern uint8 sub32_32_half[32][32];
extern uint8 mul_brightness[16][32];

// Could use BSWAP instruction on Intel port...
#define SWAP_DWORD(dw)                                                                                                 \
	dw = ((dw & 0xff) << 24) | ((dw & 0xff00) << 8) | ((dw & 0xff0000) >> 8) | ((dw & 0xff000000) >> 24)

#ifdef FAST_LSB_WORD_ACCESS
#define READ_2BYTES(s) (*(uint16 *)(s))
#define WRITE_2BYTES(s, d) *(uint16 *)(s) = (d)
#else
#ifdef LSB_FIRST
#define READ_2BYTES(s) (*(uint8 *)(s) | (*((uint8 *)(s) + 1) << 8))
#define WRITE_2BYTES(s, d) *(uint8 *)(s) = (d), *((uint8 *)(s) + 1) = (d) >> 8
#else // else MSB_FISRT
#define READ_2BYTES(s) (*(uint8 *)(s) | (*((uint8 *)(s) + 1) << 8))
#define WRITE_2BYTES(s, d) *(uint8 *)(s) = (d), *((uint8 *)(s) + 1) = (d) >> 8
#endif // LSB_FIRST
#endif // i386

#define SUB_SCREEN_DEPTH 0
#define MAIN_SCREEN_DEPTH 32

inline uint16 COLOR_ADD(uint16 C1, uint16 C2)
{
	const int RED_MASK = 0x1F << RED_SHIFT_BITS;
	const int GREEN_MASK = 0x1F << GREEN_SHIFT_BITS;
	const int BLUE_MASK = 0x1F;

	int rb = C1 & (RED_MASK | BLUE_MASK);
	rb += C2 & (RED_MASK | BLUE_MASK);
	int rbcarry = rb & ((0x20 << RED_SHIFT_BITS) | (0x20 << 0));
	int g = (C1 & (GREEN_MASK)) + (C2 & (GREEN_MASK));
	int rgbsaturate = (((g & (0x20 << GREEN_SHIFT_BITS)) | rbcarry) >> 5) * 0x1f;
	uint16 retval = (rb & (RED_MASK | BLUE_MASK)) | (g & GREEN_MASK) | rgbsaturate;
#if GREEN_SHIFT_BITS == 6
	retval |= (retval & 0x0400) >> 5;
#endif

	return retval;
}

inline uint16 COLOR_ADD1_2(uint16 C1, uint16 C2)
{
	return ((((((C1)&RGB_REMOVE_LOW_BITS_MASK) + ((C2)&RGB_REMOVE_LOW_BITS_MASK)) >> 1) +
		 ((C1) & (C2)&RGB_LOW_BITS_MASK)) |
		ALPHA_BITS_MASK);
}

inline uint16 COLOR_SUB(uint16 C1, uint16 C2)
{
	int rb1 = (C1 & (THIRD_COLOR_MASK | FIRST_COLOR_MASK)) | ((0x20 << 0) | (0x20 << RED_SHIFT_BITS));
	int rb2 = C2 & (THIRD_COLOR_MASK | FIRST_COLOR_MASK);
	int rb = rb1 - rb2;
	int rbcarry = rb & ((0x20 << RED_SHIFT_BITS) | (0x20 << 0));
	int g = ((C1 & (SECOND_COLOR_MASK)) | (0x20 << GREEN_SHIFT_BITS)) - (C2 & (SECOND_COLOR_MASK));
	int rgbsaturate = (((g & (0x20 << GREEN_SHIFT_BITS)) | rbcarry) >> 5) * 0x1f;
	uint16 retval = ((rb & (THIRD_COLOR_MASK | FIRST_COLOR_MASK)) | (g & SECOND_COLOR_MASK)) & rgbsaturate;
#if GREEN_SHIFT_BITS == 6
	retval |= (retval & 0x0400) >> 5;
#endif

	return retval;
}

inline uint16 COLOR_SUB1_2(uint16 C1, uint16 C2)
{
	return GFX.ZERO[(((C1) | RGB_HI_BITS_MASKx2) - ((C2)&RGB_REMOVE_LOW_BITS_MASK)) >> 1];
}

typedef void (*NormalTileRenderer)(uint32 Tile, uint32 Offset, uint32 StartLine, uint32 LineCount);
typedef void (*ClippedTileRenderer)(uint32 Tile, uint32 Offset, uint32 StartPixel, uint32 Width, uint32 StartLine,
				    uint32 LineCount);
typedef void (*LargePixelRenderer)(uint32 Tile, uint32 Offset, uint32 StartPixel, uint32 Pixels, uint32 StartLine,
				   uint32 LineCount);

#endif
