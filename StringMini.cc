/*
 *  StringMini.cc -- this file is part of SoftURR
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
#include <string.h>
#include "StringMini.h"

#define INITSIZE STRINGMINI_INITSIZE

int
StringMini::clear(void)
{
  this->buf = 0;
  this->len = 0;
  this->size = 0;
  return 0;
}

int
StringMini::init(int init)
{
  this->buf = (char*)malloc(init);
  this->buf[0] = 0;
  this->len = 0;
  this->size = init;
  return 0;
}

int
StringMini::release(void)
{
  if (this->buf) free(this->buf);
  return this->clear();
}

StringMini&
StringMini::copy(const StringMini& src)
{
  int i;
  if (this == &src)
    return *this;
  this->release();
  this->buf = (char*)malloc(src.size);
  this->len = src.len;
  this->size = src.size;
  for (i = 0; i < this->size; i++)
    this->buf[i] = src.buf[i];
  return *this;
}

char*
StringMini::concat(const char* src)
{
  return this->concat(src,strlen(src));
}

char*
StringMini::concat(const char* src,int len)
{
  int i;
  const char* s = src;

  i = 0;
  while (i++ < len) {
    if ((this->size-1) <= this->len) {
      if (this->size < 512) this->size *= 2;
      else this->size += 512;
      this->buf = (char*)realloc(this->buf,this->size);
    }
    this->buf[this->len++] = *s++;
  }
  this->buf[this->len] = 0;
  return this->buf;
}

StringMini::StringMini(void)
{
  this->init(INITSIZE);
}

StringMini::StringMini(const char* src)
{
  this->init(((strlen(src)/INITSIZE)+1)*INITSIZE);
  this->concat(src);
}

StringMini::StringMini(const char* src,int len)
{
  this->init(((len/INITSIZE)+1)*INITSIZE);
  this->concat(src,len);
}

StringMini::StringMini(const StringMini& src)
{
  this->clear();
  this->copy(src);
}

StringMini::~StringMini(void)
{
  this->release();
}

const char*
StringMini::refs(void)
{
  return this->buf;
}

int
StringMini::length(void)
{
  return this->len;
}

StringMini&
StringMini::operator=(const int ch)
{
  char src[2] = { ch,0 };
  this->copy(src);
  return *this;
}

StringMini&
StringMini::operator=(const char* src)
{
  StringMini s(src);
  return this->copy(s);
}

StringMini&
StringMini::operator=(const StringMini& src)
{
  return this->copy(src);
}

StringMini&
StringMini::operator+=(const int ch)
{
  char src[2] = { ch,0 };
  this->concat(src,1);
  return *this;
}

StringMini&
StringMini::operator+=(const char* src)
{
  this->concat(src);
  return *this;
}

StringMini&
StringMini::operator+=(const StringMini& src)
{
  this->concat(src.buf);
  return *this;
}

char*
StringMini::prepend(const char* src)
{
  return this->prepend(src,strlen(src));
}

char*
StringMini::prepend(const char* src,int len)
{
  StringMini s(src,len);
  s.concat(this->buf);
  this->copy(s);
  return this->buf;
}

