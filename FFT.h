/*
 *  FFT.h -- this file is part of SoftURR
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
#ifndef _FFT_H
#define _FFT_H

#include "Complex.h"

class FFT {
 public:
  int d,n;
  Complex* g;

 public:
  FFT(int);
  ~FFT(void);

 public:
  int dft(urr32_t*);
  int idft(urr32_t*);
};

#endif
