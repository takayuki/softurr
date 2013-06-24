/*
 *  Complex.h -- this file is part of SoftURR
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
#ifndef _COMPLEX_H
#define _COMPLEX_H

#include "URR32.h"
#include "String.h"

class Complex {
 public:
  urr32_t re;
  urr32_t im;

 public:
  Complex(void);
  Complex(const Complex&);
  ~Complex(void);

 public:
  const Complex& Complex::operator=(const Complex&);
  const Complex& Complex::operator=(int);
  const Complex& Complex::operator<<=(urr32_t);
  const Complex& Complex::operator+=(const Complex&);
  const Complex& Complex::operator-=(const Complex&);
  const Complex& Complex::operator*=(const Complex&);
  const Complex& Complex::operator*=(int);
  const Complex& Complex::operator/=(const Complex&);
  const Complex& Complex::operator/=(int);

 public:
  const Complex& expi(urr32_t);

 public:
  int show(String*);
};

#endif
