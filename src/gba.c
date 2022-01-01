#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *) 0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
  // waits for VBlank. wait() waits for a complete cycle.

  // Write a while loop that keeps going until we're in vBlank:
  while (SCANLINECOUNTER < 160) {

  }

  // Finally, increment the vBlank counter:
  vBlankCounter++;
}

// wait for VDraw
void waitForVDraw(void) {
  while (SCANLINECOUNTER >= 160);
}

// wait for the next VBlank period
void wait(void) {
  waitForVDraw();
  waitForVBlank();
}

static int __qran_seed = 42;
static int qran(void) {
  __qran_seed = 1664525 * __qran_seed + 1013904223;
  return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max) { return (qran() * (max - min) >> 15) + min; }

void setPixel(int row, int col, u16 color) {
  videoBuffer[row * 240 + col] = color;
}

void drawRectDMA(int row, int col, int width, int height, volatile u16 color) {
  for (int i = 0; i < height; i++) {
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer + ((row + i) * 240 + col);
    DMA[3].cnt = DMA_ON | DMA_SOURCE_FIXED | width;    
  }
}

void drawFullScreenImageDMA(const u16 *image) {
  DMA[3].src = image;
  DMA[3].dst = videoBuffer;
  DMA[3].cnt = DMA_ON | (240 * 160);
}

void drawImageDMA(int row, int col, int width, int height, const u16 *image) {
  for (int i = 0; i < height; i++) {
    DMA[3].src = image + (width * i);
    DMA[3].dst = videoBuffer + ((row + i) * 240 + col);
    DMA[3].cnt = DMA_ON | width;
  }
}

void fillScreenDMA(volatile u16 color) {
  drawRectDMA(0, 0, 240, 160, color);
}

void drawChar(int row, int col, char ch, u16 color) {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48]) {
        setPixel(row + j, col + i, color);
      }
    }
  }
}

void drawString(int row, int col, char *str, u16 color) {
  while (*str) {
    drawChar(row, col, *str++, color);
    col += 6;
  }
}

void drawCenteredString(int row, int col, int width, int height, char *str, u16 color) {
  u32 len = 0;
  char *strCpy = str;
  while (*strCpy) {
    len++;
    strCpy++;
  }

  u32 strWidth = 6 * len;
  u32 strHeight = 8;

  int new_row = row + ((height - strHeight) >> 1);
  int new_col = col + ((width - strWidth) >> 1);
  drawString(new_row, new_col, str, color);
}
