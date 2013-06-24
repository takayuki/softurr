/*
 *  URR32.h -- this file is part of SoftURR
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
#ifndef _URR32_H
#define _URR32_H

#include "String.h"
#include "softurr32.h"

struct URR32Flags {
  bool o,s,z,c;
};

class URR32Read {
 public:
  String d;
  int s,k;
};

class URR32Show {
 public:
  String d;
  int s,k;
  int lowOK,highOK;
};

class URR32 {
 public:
  static int decode(int32_t*,int32_t*,uint32_t*,urr32_t);
  static int encode(urr32_t*,int32_t,int32_t,uint32_t);
  static int unld(urr32_t*,uint32_t*,urr32_t);
  static int unld(int32_t*,urr32_t);
  static int ld(urr32_t*,int);
  static int neg(urr32_t*,urr32_t);
  static int add(urr32_t*,urr32_t,urr32_t);
  static int sub(urr32_t*,urr32_t,urr32_t);
  static int mul(urr32_t*,urr32_t,urr32_t);
  static int div(urr32_t*,urr32_t,urr32_t);
  static int je(URR32Flags*);
  static int jg(URR32Flags*);
  static int jge(URR32Flags*);
  static int jl(URR32Flags*);
  static int jle(URR32Flags*);
  static int cmp(urr32_t,urr32_t,URR32Flags*);
  static int zero(urr32_t);
  static int negative(urr32_t);

 public:
  static int mov(urr32_t*,const char*);
  static int mov(urr32_t*,urr32_t);

 public:
  static int ceil(urr32_t*,urr32_t);
  static int floor(urr32_t*,urr32_t);
  static int _sin(urr32_t*,urr32_t,int);
  static int sin(urr32_t*,urr32_t);
  static int cos(urr32_t*,urr32_t);
  static int init(void);

 public:
  static int read(const char*,urr32_t*);
  static int read(URR32Read*,urr32_t*);
  static int show(String*,urr32_t);
  static int show(URR32Show*,urr32_t);

 private:
  URR32(void);
  ~URR32(void);
};

extern urr32_t urr32_nan;
extern urr32_t urr32_10;
extern urr32_t urr32_2;
extern urr32_t urr32_1;
extern urr32_t urr32_0;
extern urr32_t urr32_1_2;
extern urr32_t urr32_pi;
extern urr32_t urr32_pi_2;
extern urr32_t urr32_pi_4;
extern urr32_t urr32_1_pi;
extern urr32_t urr32_2_pi;
extern urr32_t urr32_4_pi;

#endif
