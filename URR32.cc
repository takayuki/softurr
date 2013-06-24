/*
 *  URR32.cc -- this file is part of SoftURR
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
 *  "Standard C Library"
 *    P. J. Plauger, Prentice Hall PTR, 1991
 *  "Printing Floating-Point Numbers Quickly and Accurately"
 *     R. G. Burger, R.K. Dybvig, Proceedings of the SIGPLAN '96, 1996
 */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ArbInt.h"
#include "String.h"
#include "URR32.h"

static ArbInt arb_0;
static ArbInt arb_1(1);
static ArbInt arb_2(2);
static ArbInt arb_10(10);

void
arb_movui(ArbInt* x,uint32_t y)
{
  x->moveUInt32(y);
}

void
arb_movsi(ArbInt* x,int32_t y)
{
  x->moveInt32(y);
}

void
arb_mov(ArbInt* x,ArbInt* y)
{
  x->move(*y);
}

int
arb_mul(ArbInt* x,ArbInt* y)
{
  return ArbInt::imul(x,y);
}

int
arb_pow(ArbInt* x,ArbInt* y,int exp)
{
  assert(0 <= exp);
  if (exp == 0) {
    x->moveOne();
  } else if (exp == 1) {
    x->move(*y);
  } else {
    arb_pow(x,y,exp/2);
    arb_mul(x,x);
    if (exp % 2)
      arb_mul(x,y);
  }
  return 0;
}

int
arb_div(ArbInt* q,ArbInt* r,ArbInt* x,ArbInt* y)
{
  return ArbInt::idiv(q,r,x,y);
}

int
arb_neg(ArbInt* x)
{
  return ArbInt::neg(x);
}

int
arb_add(ArbInt* x,ArbInt* y)
{
  return ArbInt::iadd(x,y);
}

int
arb_add(ArbInt* x,uint32_t _y)
{
  ArbInt y;

  arb_movui(&y,_y);
  return ArbInt::iadd(x,&y);
}

int
arb_sub(ArbInt* x,ArbInt* y)
{
  return ArbInt::isub(x,y);
}

int arb_je(ArbIntFlags* f)  { return ArbInt::je(f);  }
int arb_jg(ArbIntFlags* f)  { return ArbInt::jg(f);  }
int arb_jge(ArbIntFlags* f) { return ArbInt::jge(f); }
int arb_jl(ArbIntFlags* f)  { return ArbInt::jl(f);  }
int arb_jle(ArbIntFlags* f) { return ArbInt::jle(f); }

int
arb_cmp(ArbInt* x,ArbInt* y,ArbIntFlags* f)
{
  return ArbInt::cmp(x,y,f);
}

int
URR32::decode(int32_t* s,int32_t* e,uint32_t* m,urr32_t input)
{
  return urr32_decode(s,e,m,input);
}

int
URR32::encode(urr32_t* output,int32_t s,int32_t e,uint32_t m)
{
  if (urr32_encode(output,s,e,m) == -1) {
    mov(output,urr32_nan); return -1;
  } else {
    return 0;
  }
}

int
URR32::unld(urr32_t* i,uint32_t* m,urr32_t x)
{
  int32_t aux,sign,exp;
  uint32_t mant,mask;

  if (decode(&aux,&exp,&mant,x) == -1) {
    mov(i,urr32_nan); return -1;
  }
  sign = (aux >> 31) | 0x1;
  if (exp < 0) {
    *i = 0; *m = mant; return 0;
  } else if (31 <= exp) {
    *i = x; *m = 0; return 0;
  }
  mask = (1 << (31 - exp)) - 1;
  *m = mant & mask;
  if (encode(i,sign,exp,mant & ~mask) == -1) {
    mov(i,urr32_nan); return -1;
  }
  return 0;
}

int
URR32::unld(int32_t* i,urr32_t x)
{
  int32_t aux,sign,exp;
  uint32_t mant;

  if (decode(&aux,&exp,&mant,x) == -1)
    return -1;
  sign = (aux >> 31) | 0x1;
  if (exp < 0) {
    *i = 0; return 0;
  } else if (31 <= exp) {
    return -1;
  }
  *i = mant >> (31 - exp);
  if (sign == -1) *i = -(*i);
  return 0;
}

int
URR32::ld(urr32_t* x,int32_t s)
{
  int sign = 1,exp = 31;
  uint32_t u;

  if (s == 0) {
    *x = 0; return 0;
  }
  if (s < 0) {
    sign = -1; s = -s;
  }
  u = (uint32_t)s;
  while (!(u & 0x80000000)) {
    u <<= 1; exp--; 
  }
  if (encode(x,sign,exp,u) == -1) {
    mov(x,urr32_nan); return -1;
  }
  return 0;
}

int
URR32::neg(urr32_t* y,urr32_t x)
{
  *y = (urr32_t)(-((int32_t)x));
  return 0;
}

int
URR32::add(urr32_t* z,urr32_t x,urr32_t y)
{
  if (urr32_add(z,x,y) == -1) {
    mov(z,urr32_nan); return -1;
  } else {
    return 0;
  }
}

int
URR32::sub(urr32_t* z,urr32_t x,urr32_t y)
{
  if (urr32_sub(z,x,y) == -1) {
    mov(z,urr32_nan); return -1;
  } else {
    return 0;
  }
}

int
URR32::mul(urr32_t* z,urr32_t x,urr32_t y)
{
  if (urr32_mul(z,x,y) == -1) {
    mov(z,urr32_nan); return -1;
  } else {
    return 0;
  }
}

int
URR32::div(urr32_t* z,urr32_t x,urr32_t y)
{
  if (urr32_div(z,x,y) == -1) {
    mov(z,urr32_nan); return -1;
  } else {
    return 0;
  }
}

int URR32::je(URR32Flags* f)  { return f->z == 1; }
int URR32::jg(URR32Flags* f)  { return f->o == f->s && f->z == 0; }
int URR32::jge(URR32Flags* f) { return f->o == f->s; }
int URR32::jl(URR32Flags* f)  { return f->o != f->s; }
int URR32::jle(URR32Flags* f) { return f->o != f->s || f->z == 1; }

int
URR32::cmp(urr32_t _x,urr32_t _y,URR32Flags* f)
{
  int32_t x,y,z;
  uint32_t bi,bo;

  x = (int32_t)_x; y = (int32_t)_y;
  z = x - y;
  
  bi = ((uint32_t)((~x & y) | (~(x ^ y) & z))) >> 31;
  bo = ((uint32_t)((z ^ x ^ y) & 0x80000000)) >> 31;
  f->o = bi ^ bo;
  if (z == 0) f->z = 1;
  else f->z = 0;
  f->s = ((uint32_t)z) >> 31;
  f->c = bi;
  return 0;
}

int
URR32::mov(urr32_t* x,const char* p)
{
  return read(p,x);
}

int
URR32::mov(urr32_t* y,urr32_t x)
{
  *y = x;
  return 0;
}

int
URR32::zero(urr32_t x)
{
  return x == 0;
}

int
URR32::negative(urr32_t x)
{
  return 0x80000000 & x;
}

int
URR32::ceil(urr32_t* y,urr32_t x)
{
  urr32_t p,m;

  unld(&p,&m,x);
  mov(y,p);
  if (!negative(x) && !zero(m)) add(y,*y,urr32_1);
  return 0;
}

int
URR32::floor(urr32_t* y,urr32_t x)
{
  urr32_t p,m;

  unld(&p,&m,x);
  mov(y,p);
  if (negative(x) && !zero(m)) sub(y,*y,urr32_1);
  return 0;
}

static urr32_t urr32_s[7],urr32_c[7];

int
URR32::_sin(urr32_t* y,urr32_t x,int qoff)
{
  urr32_t g,g2,f;
  int quad;

  mul(&g,x,urr32_4_pi);
  if (!negative(g)) add(&g,g,urr32_1);
  else sub(&g,g,urr32_1);
  div(&g,g,urr32_2);
  unld(&quad,g);
  qoff += quad & 0x3;
  ld(&g,quad);
  mul(&g,g,urr32_pi_2);
  sub(&g,x,g);

  if (qoff & 0x1) {
    mov(&f,urr32_c[6]);
    mul(&g2,g,g);
    mul(&f,f,g2); add(&f,f,urr32_c[5]);
    mul(&f,f,g2); add(&f,f,urr32_c[4]);
    mul(&f,f,g2); add(&f,f,urr32_c[3]);
    mul(&f,f,g2); add(&f,f,urr32_c[2]);
    mul(&f,f,g2); add(&f,f,urr32_c[1]);
    mul(&f,f,g2); add(&f,f,urr32_c[0]);
  } else {
    mov(&f,urr32_s[5]);
    mul(&g2,g,g);
    mul(&f,f,g2); add(&f,f,urr32_s[4]);
    mul(&f,f,g2); add(&f,f,urr32_s[3]);
    mul(&f,f,g2); add(&f,f,urr32_s[2]);
    mul(&f,f,g2); add(&f,f,urr32_s[1]);
    mul(&f,f,g2); add(&f,f,urr32_s[0]);
    mul(&f,f,g);
  }
  if (qoff & 2) neg(&f,f);
  mov(y,f);
  return 0;
}

int
URR32::sin(urr32_t* y,urr32_t x)
{
  return _sin(y,x,0);
}

int
URR32::cos(urr32_t* y,urr32_t x)
{
  return _sin(y,x,1);
}

URR32::URR32(void)
{
  init();
}

URR32::~URR32(void)
{
}

urr32_t urr32_nan;
urr32_t urr32_10;
urr32_t urr32_2;
urr32_t urr32_1;
urr32_t urr32_0;
urr32_t urr32_1_2;
urr32_t urr32_pi;
urr32_t urr32_pi_2;
urr32_t urr32_pi_4;
urr32_t urr32_1_pi;
urr32_t urr32_2_pi;
urr32_t urr32_4_pi;

int
urr32_degree(int exp)
{
  int d,e;

  assert(-536870912 <= exp && exp <= 536870912);
  if (exp == 0) {
    d = 0;
  } else if (0 < exp) {
    d = 0; e = 1;
    while (e <= exp) { d++; e <<= 1; }
  } else {
    d = 0; e = -1;
    while (exp < e) { d++; e <<= 1; }
  }
  return d;
}

int
urr32_p(int degree)
{
  /* precision: width of mantissa without hidden bit */

  assert(0 <= degree && degree <= 30);
  if (degree == 0)
    return 29;
  if (degree < 15)
    return 30 - (degree * 2);
  else
    return 0;
}

int
urr32_e(int degree,int exp)
{
  /* e value: 2^e is grap between any two consecutive numbers */
  assert(degree == urr32_degree(exp));

  return (-urr32_p(degree)) + exp;
}

int
urr32_e(int exp)
{
  return (-urr32_p(urr32_degree(exp))) + exp;
}

void
readGenerate(ArbInt* r,ArbInt* s,
	     int32_t* exponent,
	     uint32_t* mantissa,int index,int size)
{
  ArbInt q,r1;
  int d;
  ArbIntFlags f;

  arb_mul(r,&arb_2);
  arb_div(&q,r,r,s);
  assert(q.digit[0] <= 1); d = q.digit[0];

  if (d == 1)
    *mantissa |= (((uint32_t)0x80000000) >> index);
  assert(0x80000000 & *mantissa);
  if (r->zero())
    return;
  if (index < size) {
    readGenerate(r,s,exponent,mantissa,index+1,size);
  } else {
    arb_mov(&r1,r);
    arb_mul(&r1,&arb_2);
    arb_cmp(s,&r1,&f);
    if (arb_jl(&f) || (arb_je(&f) &&
		       (*mantissa & ((uint32_t)0x80000000) >> index))) {
      *mantissa += (((uint32_t)0x80000000) >> index);
      if (!(0x80000000 & *mantissa)) {
	*mantissa = 0x80000000 | (*mantissa >> 1);
	*exponent += 1;
      }
    }
  }
}

int
readScale(ArbInt* r,ArbInt* s,int k,URR32Read* st)
{
  ArbInt r1;
  ArbIntFlags f;

  arb_mov(&r1,r);
  arb_cmp(&r1,s,&f);
  if (arb_jge(&f)) {
    arb_mul(s,&arb_2);
    return readScale(r,s,k+1,st);
  }
  arb_mul(&r1,&arb_2);
  arb_cmp(&r1,s,&f);
  if (arb_jl(&f)) {
    arb_mul(r,&arb_2);
    return readScale(r,s,k-1,st);
  }
  return k;
}

int
URR32::read(const char* p,urr32_t* output)
{
  URR32Read st;
  char ch;
  int ks = 1;
  ArbInt k,k1;
  ArbInt kmax,kmin;
  ArbIntFlags f;

  ch = (int)*p++;
  st.d = ""; st.s = 1; st.k = 0;
  if (ch == '+') {
    ch = *p++;
  } else if (ch == '-') {
    st.s = -1; ch = *p++;
  }
  if (ch == '.') {
    ch = *p++;
    if (isdigit(ch)) {
      st.d += ch; arb_sub(&k1,&arb_1);
      ch = *p++; goto pnt;
    } else {
      goto err;
    }
  } else if (isdigit(ch)) {
    while (isdigit(ch)) { st.d += ch; ch = *p++; }
    if (ch == '.') {
      ch = *p++; goto pnt;
    } else if (ch == 'e' || ch == 'E') {
      ch = *p++; goto exp;
    } else if (ch == 0) {
      goto ok;
    } else {
      goto err;
    }
  } else {
    goto err;
  }
 pnt:
  while (isdigit(ch)) {
    st.d += ch; arb_sub(&k1,&arb_1);
    ch = *p++;
  }
  if (ch == 'e' || ch == 'E') {
    ch = *p++; goto exp;
  } else if (ch == 0) {
    goto ok;
  } else {
    goto err;
  }
 exp:
  if (ch == '+' || ch == '-') {
    ks = (ch == '+') ? 1 : -1; ch = *p++;
    if (isdigit(ch)) {
      arb_movui(&k,ch - '0'); ch = *p++; goto exp1;
    } else {
      goto err;
    }
  } else if (isdigit(ch)) {
    goto exp1;
  } else {
    goto err;
  }
 exp1:
  while (isdigit(ch)) {
    arb_mul(&k,&arb_10);
    arb_add(&k,ch - '0');
    ch = *p++;
  }
  if (ch == 0) goto ok;
  else goto err;
 ok:
  arb_movsi(&kmax,1023);
  arb_movsi(&kmin,-1024);
  if (ks == -1)
    arb_neg(&k);
  arb_add(&k,&k1);
  arb_cmp(&k,&kmax,&f);
  if (arb_jg(&f))
    goto err;
  arb_cmp(&k,&kmin,&f);
  if (arb_jl(&f))
    goto err;
  st.k = (int16_t)k.digit[0];
  return read(&st,output);
 err:
  mov(output,urr32_nan); return -1;
}

int
URR32::read(URR32Read* st,urr32_t* output)
{
  ArbInt f,r,s,be;
  int p;
  int32_t exponent;
  uint32_t mantissa = 0;

  if (st->d.refs() == 0)
    abort();
  f.read(st->d.refs());
  if (f.negative()) {
    arb_neg(&f);
    st->s = -1;
  }
  if (f.zero()) {
    *output = 0; return 0;
  }
  if (0 <= st->k) {
    arb_pow(&r,&arb_10,st->k);
    arb_mul(&r,&f);
    arb_movsi(&s,1);
  } else {
    arb_mov(&r,&f);
    arb_pow(&s,&arb_10,-st->k);
  }
  exponent = readScale(&r,&s,0,st);
  p = urr32_p(urr32_degree(--exponent));
  readGenerate(&r,&s,&exponent,&mantissa,0,p);
  return encode(output,st->s,exponent,mantissa);
}

void
showGenerate(ArbInt* r,ArbInt* s,ArbInt* mp,ArbInt* mm,URR32Show* p)
{
  ArbInt q1,r1,r2;
  int d,tc1,tc2;
  ArbIntFlags f;

  arb_mov(&r1,r);
  arb_mul(&r1,&arb_10);
  arb_div(&q1,&r1,&r1,s);
  assert(q1.digit[0] <= 9); d = q1.digit[0];
  arb_mul(mp,&arb_10);
  arb_mul(mm,&arb_10);

  tc1 = tc2 = 0;
  arb_cmp(&r1,mm,&f);
  if (p->lowOK && arb_jle(&f)) tc1 = 1;
  else if (arb_jl(&f)) tc1 = 1;
  arb_mov(&r2,&r1);
  arb_add(&r2,mp);
  arb_cmp(&r2,s,&f);
  if (p->highOK && arb_jge(&f)) tc2 = 1;
  else if (arb_jg(&f)) tc2 = 1;

  if (!tc1) {
    if (!tc2) { p->d += (d+'0'); showGenerate(&r1,s,mp,mm,p); }
    else { assert(d < 9); p->d += ((d+1)+'0'); }
  } else {
    if (!tc2) {
      p->d += (d+'0');
    } else {
      arb_mul(&r1,&arb_2);
      arb_cmp(&r1,s,&f);
      if (arb_jl(&f)) { p->d += (d +'0'); }
      else { assert(d < 9); p->d += ((d+1)+'0'); }
    }
  }
  return;
}

void
showScale(ArbInt* r,ArbInt* s,ArbInt* mp,ArbInt* mm,int k,URR32Show* st)
{
  ArbInt r1;
  ArbIntFlags f;

  arb_mov(&r1,r);
  arb_add(&r1,mp);
  arb_cmp(&r1,s,&f);
  if ((st->highOK && arb_jge(&f)) || arb_jg(&f)) {
    arb_mul(s,&arb_10);
    showScale(r,s,mp,mm,k+1,st);
    return;
  }
  arb_mul(&r1,&arb_10);
  arb_cmp(&r1,s,&f);
  if ((st->highOK && arb_jl(&f)) || arb_jle(&f)) {
    arb_mul(r,&arb_10);
    arb_mul(mp,&arb_10);
    arb_mul(mm,&arb_10);
    showScale(r,s,mp,mm,k-1,st);
    return;
  }
  st->k = k;
  showGenerate(r,s,mp,mm,st);
  return;
}

int
URR32::show(URR32Show* st,urr32_t input)
{
  int32_t aux,sign,degree,exponent;
  uint32_t mantissa,round;
  urr32_t k,b;
  int e,ep;
  ArbInt f,r,s,be,bp,bep,mm,mp;

  if (decode(&aux,&exponent,&mantissa,input) == -1)
    goto err;
  if (mantissa == 0) goto zero;
  sign = (aux >> 31) | 0x1;
  degree = aux & 0xFF;
  if (12 < degree)
    goto approxim;
  if (mantissa == 0x80000000) {
    e = urr32_e(degree,exponent);
    ep = urr32_e(exponent-1);
  } else {
    e = urr32_e(degree,exponent);
    ep = e;
  }
  mantissa >>= 31 - urr32_p(degree);
  round = mantissa & 0x1;

  if (0 <= e) {
    assert(ep <= e);
    if (e == ep) {
      arb_pow(&be,&arb_2,e);
      arb_movui(&f,mantissa);
      arb_mov(&r,&f);
      arb_mul(&r,&be);
      arb_mul(&r,&arb_2);
      arb_mov(&s,&arb_2);
      arb_mov(&mp,&be);
      arb_mov(&mm,&be);
    } else if (e > ep) {
      arb_pow(&be,&arb_2,e);
      arb_pow(&bp,&arb_2,(e-ep));
      arb_pow(&bep,&arb_2,e+(e-ep));
      arb_movui(&f,mantissa);
      arb_mov(&r,&f);
      arb_mul(&r,&bep);
      arb_mul(&r,&arb_2);
      arb_mov(&s,&bp);
      arb_mul(&s,&arb_2);
      arb_mov(&mp,&bep);
      arb_mov(&mm,&be);
    }
  } else {
    if (exponent <= -32768 || e == ep) {
      arb_pow(&be,&arb_2,-e);
      arb_movui(&f,mantissa);
      arb_mov(&r,&f);
      arb_mul(&r,&arb_2);
      arb_mov(&s,&be);
      arb_mul(&s,&arb_2);
      arb_mov(&mp,&arb_1);
      arb_mov(&mm,&arb_1);
    } else if (e > ep) {
      arb_pow(&bp,&arb_2,(e-ep));
      arb_pow(&bep,&arb_2,-e+(e-ep));
      arb_movui(&f,mantissa);
      arb_mov(&r,&f);
      arb_mul(&r,&bp);
      arb_mul(&r,&arb_2);
      arb_mov(&s,&bep);
      arb_mul(&s,&arb_2);
      arb_mov(&mp,&bp);
      arb_mov(&mm,&arb_1);
    } else {
      arb_pow(&be,&arb_2,-e);
      arb_pow(&bp,&arb_2,(ep-e));
      arb_movui(&f,mantissa);
      arb_mov(&r,&f);
      arb_mul(&r,&arb_2);
      arb_mov(&s,&be);
      arb_mul(&s,&arb_2);
      arb_mov(&mp,&arb_1);
      arb_mov(&mm,&bp);
    }
  }
  st->d = "";
  st->s = sign;
  st->k = 0;
  st->lowOK = round;
  st->highOK = round;
  showScale(&r,&s,&mp,&mm,0,st);
  return 1;
 approxim:
  ld(&k,exponent);
  mov(&b,"3.3219280948873621817"); /* log2(10) */
  div(&k,k,b);
  ceil(&k,k);
  st->d = "1";
  st->s = sign;
  unld(&st->k,k);
  return 2;
 zero:
  st->d = "0";
  st->s = 0;
  st->k = 0;
  return 0;
 err:
  st->d = "nan";
  st->s = 0;
  st->k = 0;
  return -1;
}

int
URR32::show(String* _s,urr32_t input)
{
  URR32Show t;
  String& s = *_s;
  const char* p;
  int k,r,d;

  k = show(&t,input);
  if (k == 0 || k == -1) {
    s = t.d;
    return k;
  }
  s = "";
  if (k == 2) s += "~";
  if (t.s == -1) s += '-';
  k = t.k;
  if (-6 <= k && k <= 7) {
    if (k <= 0) {
      s += "0.";
      while (k++ < 0) s += '0';
      s += t.d;
    } else {
      p = t.d.refs();
      while (*p != 0 && 0 < k--)
	s += (int)*p++;
      while (0 < k--) s += '0';
      if (*p != 0) {
	s += '.'; s += p;
      }
    }
  } else {
    p = t.d.refs();
    s += (int)*p++;
    if (*p != 0) {
      s += '.'; s += p;
    }
    s += 'e';
    if (--k < 0) {
      s += '-'; k = -k;
    }
    r = 1;
    while (r <= k) r *= 10;
    while (0 < (r /= 10)) {
      d = k / r;
      s += d + '0';
      k -= d * r;
    }
  }
  return 0;
}


int
URR32::init(void)
{
  mov(&urr32_nan,  0x80000000);
  mov(&urr32_10,   "1.00000000000000000000e+01");
  mov(&urr32_2,    "2.00000000000000000000");
  mov(&urr32_1,    "1.00000000000000000000");
  mov(&urr32_0,    "0.00000000000000000000");
  mov(&urr32_1_2,  "0.50000000000000000000");
  mov(&urr32_pi,   "3.14159265358979323846");
  mov(&urr32_pi_2, "1.57079632679489661923");
  mov(&urr32_pi_4, "0.78539816339744830962");
  mov(&urr32_1_pi, "0.31830988618379067154");
  mov(&urr32_2_pi, "0.63661977236758134308");
  mov(&urr32_4_pi, "1.27323954473516268616");

  mov(&urr32_s[0], "1.00000000000000000000e+00");
  mov(&urr32_s[1],"-1.66666666666666324348e-01");
  mov(&urr32_s[2], "8.33333333332248946124e-03");
  mov(&urr32_s[3],"-1.98412698298579493134e-04");
  mov(&urr32_s[4], "2.75573137070700676789e-06");
  mov(&urr32_s[5],"-2.50507602534068634195e-08");

  mov(&urr32_c[0], "1.00000000000000000000e+00");
  mov(&urr32_c[1],"-5.00000000000000000000e-01");
  mov(&urr32_c[2], "4.16666666666666019037e-02");
  mov(&urr32_c[3],"-1.38888888888741095749e-03");
  mov(&urr32_c[4], "2.48015872894767294178e-05");
  mov(&urr32_c[5],"-2.75573143513906633035e-07");
  mov(&urr32_c[6], "2.08757232129817482790e-09");
  return 0;
}
