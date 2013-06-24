/*
 *  FFT.cc -- this file is part of SoftURR
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
/*
 * Reference:
 *  "Introduction to Algorithms, Second Edition"
 *    Thomas H. Cormen et al., The MIT Press 2001
 */
#include <stdio.h>
#include <stdlib.h>
#include "FFT.h"

FFT::FFT(int power)
{
  URR32::init();

  this->d = power;
  this->n = 1 << this->d;
  this->g = (Complex*)calloc(this->n,sizeof(Complex));
}

FFT::~FFT(void)
{
  free(g);
}

static int
rev(int d,int n)
{
  int i,r = 0;
  for (i = 0; i < d; i++)
    r |= ((n & (1 << i)) >> i) << ((d-1)-i);
  return r;
}

int
FFT::dft(urr32_t* x)
{
  int i,j,k,s,m,m2;
  Complex wm,w,t,u;
  Complex* a = this->g;
  urr32_t f,m1;

  for (i = 0; i < n; i++)
    a[rev(d,i)] <<= x[i];

  for (s = 1; s <= d; s++) {
    m = 1 << s;
    URR32::ld(&m1,m);
    URR32::ld(&f,-2);
    URR32::mul(&f,f,urr32_pi);
    URR32::div(&f,f,m1);
    wm.expi(f);
    for (k = 0; k < n; k += m) {
      w = 1; m2 = m/2;
      for (j = 0; j < m2; j++) {
	t = w; t *= a[k+j+m2];
	u = a[k+j];
	a[k+j] = u; a[k+j] += t;
	a[k+j+m2] = u; a[k+j+m2] -= t;
	w *= wm;
      }
    }
  }
  for (i = 0; i < n; i++)
    a[i] /= n;
  return 0;
}

int
FFT::idft(urr32_t* x)
{
  int i,j,k,s,m,m2;
  Complex wm,w,t,u;
  Complex a[n];
  urr32_t f,m1;

  for (i = 0; i < n; i++)
    a[rev(d,i)] = g[i];

  for (s = 1; s <= d; s++) {
    m = 1 << s;
    URR32::ld(&m1,m);
    URR32::ld(&f,2);
    URR32::mul(&f,f,urr32_pi);
    URR32::div(&f,f,m1);
    wm.expi(f);
    for (k = 0; k < n; k += m) {
      w = 1; m2 = m/2;
      for (j = 0; j < m2; j++) {
	t = w; t *= a[k+j+m2];
	u = a[k+j];
	a[k+j] = u; a[k+j] += t;
	a[k+j+m2] = u; a[k+j+m2] -= t;
	w *= wm;
      }
    }
  }
  for (i = 0; i < n; i++)
    x[i] = a[i].re;
  return 0;
}
