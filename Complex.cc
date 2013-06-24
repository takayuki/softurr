/*
 *  Complex.cc -- this file is part of SoftURR
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
#include "Complex.h"

Complex::Complex(void)
{
  URR32::ld(&this->re,0);
  URR32::ld(&this->im,0);
}

Complex::Complex(const Complex& src)
{
  URR32::mov(&this->re,src.re);
  URR32::mov(&this->im,src.im);
}

Complex::~Complex(void)
{
}

const Complex&
Complex::operator=(const Complex& src)
{
  URR32::mov(&this->re,src.re);
  URR32::mov(&this->im,src.im);
  return *this;
}

const Complex&
Complex::operator=(int x)
{
  URR32::ld(&this->re,x);
  URR32::ld(&this->im,0);
  return *this;
}

const Complex&
Complex::operator<<=(urr32_t x)
{
  urr32_t y;
  URR32::ld(&y,0);
  URR32::mov(&this->re,x);
  URR32::mov(&this->im,y);
  return *this;
}

const Complex&
Complex::operator+=(const Complex& src)
{
  URR32::add(&this->re,this->re,src.re);
  URR32::add(&this->im,this->im,src.im);
  return *this;
}

const Complex&
Complex::operator-=(const Complex& src)
{
  URR32::sub(&this->re,this->re,src.re);
  URR32::sub(&this->im,this->im,src.im);
  return *this;
}

const Complex&
Complex::operator*=(const Complex& src)
{
  urr32_t a = this->re;
  urr32_t b = this->im;
  urr32_t c = src.re;
  urr32_t d = src.im;
  urr32_t s,t;

  URR32::mul(&s,a,c);
  URR32::mul(&t,b,d);
  URR32::sub(&this->re,s,t);
  URR32::mul(&s,b,c);
  URR32::mul(&t,a,d);
  URR32::add(&this->im,s,t);
  return *this;
}

const Complex&
Complex::operator/=(const Complex& src)
{
  urr32_t a = this->re;
  urr32_t b = this->im;
  urr32_t c = src.re;
  urr32_t d = src.im;
  urr32_t s,t,u,v;

  URR32::mul(&s,a,c);
  URR32::mul(&t,b,d);
  URR32::add(&this->re,s,t);
  URR32::mul(&u,c,c);
  URR32::mul(&v,d,d);
  URR32::add(&u,u,v);
  URR32::div(&this->re,this->re,u);
  URR32::mul(&s,b,c);
  URR32::mul(&t,a,d);
  URR32::sub(&this->im,s,t);
  URR32::div(&this->im,this->im,u);
  return *this;
}

const Complex&
Complex::operator/=(int x)
{
  urr32_t d;
  URR32::ld(&d,x);
  URR32::div(&this->re,this->re,d);
  URR32::div(&this->im,this->im,d);
  return *this;
}

const Complex&
Complex::expi(urr32_t arg)
{
  URR32::cos(&this->re,arg);
  URR32::sin(&this->im,arg);
  return *this;
}

int
Complex::show(String* _s)
{
  String& s = *_s;
  String u,v;
  URR32::show(&u,this->re);
  URR32::show(&v,this->im);
  s = "";
  s += "Re="; s += u.refs(); s += ",";
  s += "Im="; s += v.refs();
  return 0;
}
