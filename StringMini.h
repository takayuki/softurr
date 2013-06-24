/*
 *  StringMini.h -- this file is part of SoftURR
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
#ifndef _STRINGMINI_H
#define _STRINGMINI_H

#define STRINGMINI_INITSIZE  8

class StringMini {

 protected:
  char* buf;
  int len;
  int size;

 protected:
  int clear(void);
  int init(int);
  int release(void);

 protected:
  StringMini& copy(const StringMini&);
  char* concat(const char*);
  char* concat(const char*,const int);

 public:
  StringMini(void);
  StringMini(const char*);
  StringMini(const char*,int);
  StringMini(const StringMini&);
  virtual ~StringMini(void);

 public:
  const char* refs(void);
  int length(void);

 public:
  StringMini& operator=(const int);
  StringMini& operator=(const char*);
  StringMini& operator=(const StringMini&);
  StringMini& operator+=(const int);
  StringMini& operator+=(const char*);
  StringMini& operator+=(const StringMini&);

 public:
  char* prepend(const char*);
  char* prepend(const char*,int);
};

#endif

