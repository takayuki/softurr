/*
 *  ArbInt.cc -- this file is part of SoftURR
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
 *  "Hacker's Delight"
 *     Henry S. Warren, Addison Wesley Professional 2003
 *  "Introduction to Functional Programming using Haskell"
 *     Richard Bird, 2nd edition, Prentice Hall Press, 1998
 */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "ArbInt.h"

#ifndef MAX
#define MAX(x,y) (x > y ? x : y);
#endif

static ArbInt arbint_0;
static ArbInt arbint_1(1);

int
ArbInt::add1(ArbInt* x,uint16_t d)
{
  int m = x->ndigit;
  uint32_t co;
  uint32_t u,w;

  co = d;
  for (int i = 0; i < m; i++) {
    u = x->getDigit(i);
    w = u + co;
    co = w >> 16;
    x->setDigit(i,w);
  }
  if (co != 0) {
    x->zeroExtend();
    x->setDigit(m,co);
  }
  x->zeroTrim();
  return 0;
}

int
ArbInt::add(ArbInt* x,ArbInt* y)
{
  int m = MAX(x->ndigit,y->ndigit);
  uint32_t co;
  uint32_t u,v,w;

  x->zeroExtend(m);
  co = 0;
  for (int i = 0; i < m; i++) {
    u = x->getDigit(i);
    v = y->getDigitOrZero(i);
    w = u + v + co;
    co = w >> 16;
    x->setDigit(i,w);
  }
  if (co != 0) {
    x->zeroExtend();
    x->setDigit(m,co);
  }
  x->zeroTrim();
  return 0;
}

int
ArbInt::iadd(ArbInt* x,ArbInt* y)
{
  int m = MAX(x->ndigit,y->ndigit);
  uint32_t co,ci;
  uint32_t u,v,w;

  x->signExtend(m);
  co = 0; ci = 0;
  for (int i = 0; i < m; i++) {
    u = x->getDigit(i);
    v = y->getDigitOrSign(i);
    w = u + v + co;
    co = w >> 16;
    ci = ((w ^ u ^ v) & 0x8000) >> 15;
    x->setDigit(i,w);
  }
  if (co ^ ci) {
    x->signExtend();
    x->setDigit(m,((int16_t)(co << 15)) >> 15);
  }
  x->signTrim();
  return 0;
}

int ArbInt::je(ArbIntFlags* f)  { return f->z == 1; }
int ArbInt::ja(ArbIntFlags* f)  { return f->c == 0 && f->z == 0; }
int ArbInt::jae(ArbIntFlags* f) { return f->c == 0; }
int ArbInt::jb(ArbIntFlags* f)  { return f->c == 1; }
int ArbInt::jbe(ArbIntFlags* f) { return f->c == 1 || f->z == 1; }
int ArbInt::jg(ArbIntFlags* f)  { return f->o == f->s && f->z == 0; }
int ArbInt::jge(ArbIntFlags* f) { return f->o == f->s; }
int ArbInt::jl(ArbIntFlags* f)  { return f->o != f->s; }
int ArbInt::jle(ArbIntFlags* f) { return f->o != f->s || f->z == 1; }

int
ArbInt::cmp(ArbInt* _x,ArbInt* y,ArbIntFlags* f)
{
  ArbInt x(*_x);
  int m = MAX(x.ndigit,y->ndigit);
  uint32_t bi,bo;
  uint32_t u,v,w;

  x.signExtend(m);
  bi = 0; bo = 0;
  for (int i = 0; i < m; i++) {
    u = x.getDigit(i);
    v = y->getDigitOrSign(i);
    w = u - v - bi;
    bi = (w & 0x10000) >> 16;
    bo = ((w ^ u ^ v) & 0x8000) >> 15;
    x.setDigit(i,w);
  }
  f->o = bi ^ bo;
  f->z = 1;
  for (int i = 0; i < x.ndigit; i++)
    if (x.digit[i] != 0) { f->z = 0; break; }
  f->s = x.digit[x.ndigit-1] >> 15;
  f->c = bi;
  return 0;
}

int
ArbInt::isub(ArbInt* x,ArbInt* y)
{
  int m = MAX(x->ndigit,y->ndigit);
  uint32_t bi,bo;
  uint32_t u,v,w;

  x->signExtend(m);
  bi = 0; bo = 0;
  for (int i = 0; i < m; i++) {
    u = x->getDigit(i);
    v = y->getDigitOrSign(i);
    w = u - v - bi;
    bi = (w & 0x10000) >> 16;
    bo = ((w ^ u ^ v) & 0x8000) >> 15;
    x->setDigit(i,w);
  }
  if (bi ^ bo) {
    x->zeroExtend();
    x->setDigit(m,((int16_t)(bo << 15)) >> 15);
  }
  x->signTrim();
  return 0;
}

int
ArbInt::mul1(ArbInt* x,uint16_t d)
{
  int m = x->ndigit;
  uint32_t co;
  uint32_t u,w;

  co = 0;
  for (int i = 0; i < m; i++) {
    u = x->getDigit(i);
    w = u * d + co;
    co = w >> 16;
    x->setDigit(i,w);
  }
  if (co != 0) {
    x->zeroExtend();
    x->setDigit(m,co);
  }
  x->zeroTrim();
  return 0;
}

int
ArbInt::mul(ArbInt* x,ArbInt* y)
{
  ArbInt z(*x);
  int m = x->ndigit;
  int n = y->ndigit;
  uint32_t u,v,w,k,t;

  z.moveZero(m+n);
  for (int j = 0; j < n; j++) {
    k = 0;
    v = y->getDigit(j);
    for (int i = 0; i < m; i++) {
      u = x->getDigit(i);
      w = z.getDigit(j+i);
      t = u * v + w + k;
      z.setDigit(j+i,t);
      k = t >> 16;
    }
    z.setDigit(j+m,k);
  }
  z.zeroTrim();
  x->move(z);
  return 0;
}

int
ArbInt::imul(ArbInt* x,ArbInt* y)
{
  ArbInt z(*x);
  int m = x->ndigit;
  int n = y->ndigit;
  uint32_t u,v,w,k,t,b;

  z.moveZero(m+n);
  for (int j = 0; j < n; j++) {
    k = 0;
    v = y->getDigit(j);
    for (int i = 0; i < m; i++) {
      u = x->getDigit(i);
      w = z.getDigit(j+i);
      t = u * v + w + k;
      z.setDigit(j+i,t);
      k = t >> 16;
    }
    z.setDigit(j+m,k);
  }

  u = x->getDigit(m-1);
  if (u >> 15) {
    b = 0;
    for (int j = 0; j < n; j++) {
      v = y->getDigit(j);
      w = z.getDigit(j+m);
      t = w - v - b;
      z.setDigit(m+j,t);
      b = ((((~w) & v) | ((~(w ^ v)) & t)) & 0x8000) >> 15;
    }
  }
  v = y->getDigit(n-1);
  if (v >> 15) {
    b = 0;
    for (int i = 0; i < m; i++) {
      u = x->getDigit(i);
      w = z.getDigit(n+i);
      t = w - u - b;
      z.setDigit(n+i,t);
      b = ((((~w) & u) | ((~(w ^ u)) & t)) & 0x8000) >> 15;
    }
  }
  z.signTrim();
  x->move(z);
  return 0;
}

int
ArbInt::div1(ArbInt* q,ArbInt* r,ArbInt* x,uint16_t d)
{
  int m = x->ndigit;
  uint32_t u,w;
  uint32_t rhat;

  if (d == 0)
    abort();
  q->moveZero(m);
  rhat = 0;
  for (int i = m-1; 0 <= i; i--) {
    u = (rhat << 16) + x->getDigit(i);
    w = u / d;
    q->setDigit(i,w);
    rhat = u - w * d;
  }
  q->zeroTrim();
  r->moveUInt32(rhat);
  return 0;
}

int
ArbInt::div(ArbInt* q,ArbInt* r,ArbInt* _x,ArbInt* _y)
{
  ArbInt x(*_x),y(*_y);
  ArbIntFlags f;
  int m = x.ndigit;
  int n = y.ndigit;
  int i,j,s,t;
  uint32_t b = 65536;
  uint16_t x0,x1,x2,y1,y2;
  uint32_t qhat,rhat;
  uint32_t p,k;

  if (y.zero())
    abort();
  if (n == 1)
    return div1(q,r,&x,y.digit[0]);
  if (m < n) {
    q->moveZero();
    r->move(x);
    return 0;
  }
  if (m == n) {
    cmp(&x,&y,&f);
    if (jb(&f)) {
      q->moveZero();
      r->move(x);
      return 0;
    }
    if (je(&f)) {
      q->moveOne();
      r->moveZero();
      return 0;
    }
  }
  q->moveZero(m-n+1);
  r->moveZero(n);

  x.zeroExtend();
  s = 0;
  while (!y.negative()) {
    shl(&y,1); shl(&x,1);
    s++;
  }
  y1 = y.getDigit(n-1);
  y2 = y.getDigit(n-2);
  for (j = m - n; 0 <= j; j--) {
    x0 = x.getDigit(j+n);
    x1 = x.getDigit(j+n-1);
    x2 = x.getDigit(j+n-2);
    qhat = (x0 * b + x1) / y1;
    rhat = (x0 * b + x1) - qhat * y1;
  again:
    if (b <= qhat || (rhat * b + x2) < (qhat * y2)) {
      qhat--;
      rhat += y1;
      if (rhat < b)
	goto again;
    }
    k = 0;
    for (i = 0; i < n; i++) {
      p = qhat * y.getDigit(i);
      t = x.getDigit(i+j) - k - (p & 0xFFFF);
      x.setDigit(i+j,t);
      k = (p >> 16) - (t >> 16);
    }
    t = x.getDigit(n+j) - k;
    x.setDigit(n+j,t);

    q->setDigit(j,qhat);
    if (t < 0) {
      q->setDigit(j,qhat-1);
      k = 0;
      for (i = 0; i < n; i++) {
	t = x.getDigit(i+j) + y.getDigit(i) + k;
	x.setDigit(j+i,t);
	k = t >> 16;
      }
      t = x.getDigit(j+n) + k;
      x.setDigit(j+n,t);
    }
  }
  r->move(x);
  shr(r,s);
  q->zeroTrim();
  r->zeroTrim();
  return 0;
}

int
ArbInt::idiv(ArbInt* q,ArbInt* r,ArbInt* _x,ArbInt* _y)
{
  ArbInt x(*_x),y(*_y);

  if (y.zero()) abort();
  if (x.negative()) { neg(&x); x.zeroTrim(); }
  if (y.negative()) { neg(&y); y.zeroTrim(); }

  div(q,r,&x,&y);
  q->signCoerce(); r->signCoerce();

  if (_x->negative() ^ _y->negative()) neg(q);
  if (_x->negative()) neg(r);
  return 0;
}

int
ArbInt::neg(ArbInt* x)
{
  for (int i = 0; i < x->ndigit; i++)
    x->setDigit(i,~(x->getDigit(i)));
  iadd(x,&arbint_1);
  return 0;
}

int
ArbInt::shl(ArbInt* x,int b)
{
  int m = x->ndigit;
  int s,t,i;
  uint32_t u,v;

  if (b <= 0)
    return 0;
  s = b / 16; t = b % 16;
  if (t == 0) {
    i = m-s-1;
    while (0 <= i) {
      v = x->getDigit(i);
      x->setDigit((i--)+s,v);
    }
    i += s;
    while (0 <= i)
      x->setDigit(i--,0);
  } else {
    i = m-s-1;
    while (1 <= i) {
      u = x->getDigit(i);
      v = x->getDigit(i-1);
      x->setDigit((i--)+s,(u << t | v >> (16-t)));
    }
    u = x->getDigit(i);
    x->setDigit((i--)+s,(u << t));
    i += s;
    while (0 <= i)
      x->setDigit(i--,0);
  }
  return 0;
}

int
ArbInt::shr(ArbInt* x,int b)
{
  int m = x->ndigit;
  int s,t,i;
  uint32_t u,v;

  if (b <= 0)
    return 0;
  s = b / 16; t = b % 16;
  if (m <= s) {
    i = 0;
    while (i < m)
      x->setDigit(i++,0);
  } else {
    if (t == 0) {
      i = 0;
      while ((i+s) < m) {
        v = x->getDigit(i+s);
        x->setDigit(i++,v);
      }
      while (i < m)
        x->setDigit(i++,0);
    } else {
      i = 0;
      while ((i+s) < (m-1)) {
        v = x->getDigit(i+s);
        u = x->getDigit(i+s+1);
        x->setDigit(i++,(u << (16-t) | v >> t));
      }
      v = x->getDigit(i+s);
      x->setDigit(i++,(v >> t));
      while (i < m)
        x->setDigit(i++,0);
    }
  }
  return 0;
}

ArbInt::ArbInt(void)
{
  this->size = CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = 1;
  this->digit[0] = 0;
}

ArbInt::ArbInt(uint16_t x0)
{
  this->size = CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = 1;
  this->digit[0] = x0;
}

ArbInt::ArbInt(uint16_t x1,uint16_t x0)
{
  this->size = CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = 2;
  this->digit[0] = x0;
  this->digit[1] = x1;
}

ArbInt::ArbInt(uint16_t x2,uint16_t x1,uint16_t x0)
{
  this->size = CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = 3;
  this->digit[0] = x0;
  this->digit[1] = x1;
  this->digit[2] = x2;
}

ArbInt::ArbInt(uint16_t x3,uint16_t x2,uint16_t x1,uint16_t x0)
{
  this->size = CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = 4;
  this->digit[0] = x0;
  this->digit[1] = x1;
  this->digit[2] = x2;
  this->digit[3] = x3;
}

ArbInt::ArbInt(const char* s,const char** p)
{
  this->size = CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = 1;
  this->digit[0] = 0;
  this->read(s,p);
}

int
ArbInt::read(const char* s,const char** p)
{
  int sign = 1;

  this->ndigit = 1;
  this->digit[0] = 0;
  if (*s == '+') s++;
  else if (*s == '-') { sign = -1; s++; }
  while (*s != 0 && isdigit(*s)) {
    mul1(this,10);
    add1(this,((*s++) - '0'));
  }
  if (p != 0) {
    *p = s;
  } else {
    if (*s != 0) { abort(); }
  }
  this->signCoerce();
  if (sign == -1) neg(this);
  return 1;
}

ArbInt::ArbInt(const ArbInt& x)
  : digit(0),size(0),ndigit(0)
{
  this->move(x);
}

ArbInt::~ArbInt(void)
{
  free(this->digit);
}

void
ArbInt::moveZero(void)
{
  this->moveUInt32(0);
}

void
ArbInt::moveZero(int ext)
{
  this->moveUInt32(0);
  this->zeroExtend(ext);
}

void
ArbInt::moveOne(void)
{
  this->moveUInt32(1);
}

void
ArbInt::moveInt32(int32_t val)
{
  this->ndigit = 2;
  this->digit[0] = val & 0xFFFF;
  this->digit[1] = val >> 16;
  this->signTrim();
}

void
ArbInt::moveUInt32(uint32_t val)
{
  this->ndigit = 2;
  this->digit[0] = val & 0xFFFF;
  this->digit[1] = val >> 16;
  this->signCoerce();
  this->signTrim();
}

void
ArbInt::move(const ArbInt& x)
{
  if (this->digit)
    free(this->digit);
  this->size = ((x.ndigit/CHUNKSIZE) + 1) * CHUNKSIZE;
  this->digit = (uint16_t*)malloc(DIGITSIZE*this->size);
  this->ndigit = x.ndigit;
  for (int i = 0; i < this->ndigit; i++)
    this->digit[i] = x.digit[i];
}

int
ArbInt::zero(void)
{
  for (int i = 0; i < this->ndigit; i++)
    if (this->digit[i] != 0) return 0;
  return 1;
}

int
ArbInt::negative(void)
{
  return this->digit[this->ndigit-1] >> 15;
}

void
ArbInt::signCoerce(void)
{
  if (this->negative()) this->zeroExtend();
}

void
ArbInt::zeroTrim(void)
{
  for (int i = (this->ndigit-1); 1 <= i; i--) {
    if (this->digit[i] == 0) this->ndigit -= 1;
    else break;
  }
}

void
ArbInt::signTrim(void)
{
  for (int i = (this->ndigit-1); 1 <= i; i--) {
    if (this->digit[i] == (uint16_t)(((int16_t)(this->digit[i-1])) >> 15))
      this->ndigit -= 1;
    else
      break;
  }
}

void
ArbInt::signExtend(void)
{
  this->signExtend(this->ndigit+1);
}

void
ArbInt::signExtend(int ext)
{
  assert(0 <= ext && ext <= 0x1000);

  if (ext <= this->ndigit)
    return;
  if (this->size < ext) {
    int newsize = ((ext/CHUNKSIZE) + 1) * CHUNKSIZE;
    this->digit = (uint16_t*)realloc(this->digit,DIGITSIZE*newsize);
    this->size = newsize;
  }
  uint16_t high = this->digit[this->ndigit-1];
  uint16_t fill = ((int16_t)high) >> 15;
  for (int i = this->ndigit; i < ext; i++) {
    this->digit[i] = fill;
  }
  this->ndigit = ext;
  return;
}

void
ArbInt::zeroExtend(void)
{
  this->zeroExtend(this->ndigit+1);
}

void
ArbInt::zeroExtend(int ext)
{
  assert(0 <= ext && ext <= 0x1000);

  if (ext <= this->ndigit)
    return;
  if (this->size < ext) {
    int newsize = ((ext/CHUNKSIZE) + 1) * CHUNKSIZE;
    this->digit = (uint16_t*)realloc(this->digit,DIGITSIZE*newsize);
    this->size = newsize;
  }
  for (int i = this->ndigit; i < ext; i++) {
    this->digit[i] = 0;
  }
  this->ndigit = ext;
  return;
}

uint16_t
ArbInt::getDigit(int pos)
{
  assert(0 <= pos && pos < this->ndigit);

  return this->digit[pos];
}

uint16_t
ArbInt::getDigitOrSign(int pos)
{
  assert(0 <= pos);

  if (0 <= pos && pos < this->ndigit)
    return this->digit[pos];
  else
    return (((int16_t)this->digit[this->ndigit-1]) >> 15);
}

uint16_t
ArbInt::getDigitOrZero(int pos)
{
  assert(0 <= pos);

  if (0 <= pos && pos < this->ndigit)
    return this->digit[pos];
  else
    return 0;
}

void
ArbInt::setDigit(int pos,uint16_t newdigit)
{
  assert (0 <= pos && pos < this->ndigit);

  this->digit[pos] = newdigit;
}

void
ArbInt::show(char** p)
{
  ArbInt x(*this),y(1);
  ArbIntFlags f;
  ArbInt q,r;
  char* s;
  int i,j;

  if (this->zero()) {
    s = (char*)malloc(2);
    if (s == 0)
      abort();
    s[0] = '0'; s[1] = 0;
    *p = s;
    return;
  }
  i = 0; j = 0;
  if (this->negative()) {
    i++; neg(&x);
  }
  while (1) {
    cmp(&x,&y,&f);
    if (jae(&f)) {
      mul1(&y,10); i++; continue;
    }
    break;
  }
  s = (char*)malloc(i+1);
  if (s == 0)
    abort();
  if (this->negative()) {
    s[j++] = '-';
  }
  div1(&q,&r,&y,10);
  y.move(q);
  while (1) {
    cmp(&y,&arbint_0,&f);
    if (jbe(&f))
      break;
    div(&q,&r,&x,&y);
    assert(q.digit[0] <= 9);
    s[j++] = (q.digit[0] - 0) + '0';
    x.move(r);
    div1(&q,&r,&y,10);
    y.move(q);
  }
  s[j] = 0;
  *p = s;
  return;
}
