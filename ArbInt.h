/*
 *  ArbInt.h -- this file is part of SoftURR
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
#ifndef _ARBINT_H
#define _ARBINT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#define DIGITSIZE sizeof(uint16_t)
#define CHUNKSIZE 4

struct ArbIntFlags {
  bool o,s,z,c;
};

class ArbInt {
 public:
  static int add1(ArbInt*,uint16_t);
  static int add(ArbInt*,ArbInt*);
  static int iadd(ArbInt*,ArbInt*);
  static int je(ArbIntFlags*);
  static int ja(ArbIntFlags*);
  static int jae(ArbIntFlags*);
  static int jb(ArbIntFlags*);
  static int jbe(ArbIntFlags*);
  static int jg(ArbIntFlags*);
  static int jge(ArbIntFlags*);
  static int jl(ArbIntFlags*);
  static int jle(ArbIntFlags*);
  static int cmp(ArbInt*,ArbInt*,ArbIntFlags*);
  static int isub(ArbInt*,ArbInt*);
  static int mul1(ArbInt*,uint16_t);
  static int mul(ArbInt*,ArbInt*);
  static int imul(ArbInt*,ArbInt*);
  static int div1(ArbInt*,ArbInt*,ArbInt*,uint16_t);
  static int div(ArbInt*,ArbInt*,ArbInt*,ArbInt*);
  static int idiv(ArbInt*,ArbInt*,ArbInt*,ArbInt*);
  static int neg(ArbInt*);
  static int shl(ArbInt*,int);
  static int shr(ArbInt*,int);
  
 public:
  uint16_t* digit;
  int size;
  int ndigit;

 public:
  ArbInt(void);
  ArbInt(uint16_t);
  ArbInt(const char*,const char** = 0);
  ArbInt(const ArbInt&);
  ~ArbInt(void);

 protected:
  ArbInt(uint16_t,uint16_t);
  ArbInt(uint16_t,uint16_t,uint16_t);
  ArbInt(uint16_t,uint16_t,uint16_t,uint16_t);

 public:
  void moveZero(void);
  void moveZero(int);
  void moveOne(void);
  void move(const ArbInt&);
  void moveInt32(int32_t);
  void moveUInt32(uint32_t);

 public:
  int zero(void);
  int negative(void);

 public:
  void signCoerce(void);

 public:
  void signTrim(void);
  void zeroTrim(void);

  public:
  void signExtend(void);
  void signExtend(int);
  void zeroExtend(void);
  void zeroExtend(int);

 public:
  uint16_t getDigit(int);
  uint16_t getDigitOrSign(int);
  uint16_t getDigitOrZero(int);

 public:
  void setDigit(int,uint16_t);

 public:
  int read(const char*,const char** = 0);
  void show(char**);
};

#endif
