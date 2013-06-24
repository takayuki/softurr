/*
 *  softurr32.h -- this file is part of SoftURR
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
#ifndef _SOFTURR32_H
#define _SOFTURR32_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

typedef uint32_t urr32_t;

#if defined(__cplusplus)
extern "C" {
#endif
int urr32_decode(int32_t*,int32_t*,uint32_t*,urr32_t);
int urr32_encode(urr32_t*,int32_t,int32_t,uint32_t);
int urr32_add(urr32_t*,urr32_t,urr32_t);
int urr32_sub(urr32_t*,urr32_t,urr32_t);
int urr32_mul(urr32_t*,urr32_t,urr32_t);
int urr32_div(urr32_t*,urr32_t,urr32_t);
#if defined(__cplusplus)
}
#endif
#endif
