/*
 *  demo.cc -- this file is part of SoftURR
 *  Copyright (C) 2006 Takayuki Usui
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdio.h>
#include <stdlib.h>
#include "FFT.h"

void
unpack(char* b,uint32_t x)
{
  uint32_t q = 0x80000000;
  uint32_t i = 32;

  while (0 < q) {
    if (i < 32 && (i % 8) == 0) *b++ = '_';
    if (x & q) *b++ = '1'; else *b++ = '0';
    q >>= 1; i--;
  }
  *b = 0;
}

int
main(int argc,char* argv[])
{
  FFT fft(3);
  String o;
  char b[36];
  int x[8] = { 0,0,0,0,0,0,0,0 };
  urr32_t s[8],t[8];

  for (int i = 0; i < 8; i++) {
    if ((i+1) < argc)
      x[i] = atoi(argv[i+1]);
  }
  for (int i = 0; i < 8; i++) {
    URR32::ld(&s[i],x[i]);
    unpack(b,s[i]);
    printf("s[%d]=%s %d\n",i,b,x[i]);
  }
  printf("Applying Fourier Transform...");
  fft.dft(s);
  printf("done\n");
  for (int i = 0; i < 8; i++) {
    fft.g[i].show(&o);
    printf("g[%d]={%s}\n",i,o.refs());
  }
  printf("Applying Inverse Fourier Transform...");
  fft.idft(t);
  printf("done\n");
  for (int i = 0; i < 8; i++) {
    unpack(b,t[i]);
    URR32::show(&o,t[i]);
    printf("t[%d]=%s %s\n",i,b,o.refs());
  }
  return 0;
}

