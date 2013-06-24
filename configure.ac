#
#  configure.ac -- this file is part of SoftURR
#  Copyright (C) 2006 Takayuki Usui
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#
AC_PREREQ(2.59)
AC_INIT([SoftURR], [0.1], [takayuki@genesis.ne.jp], [softurr])
AC_CONFIG_SRCDIR([softurr32.h])
AC_CONFIG_HEADER([config.h])

AC_PROG_CXX
AC_PROG_CC
AC_PATH_PROG([NASM], [nasm])
if test -z "$NASM"; then
  AC_MSG_ERROR([install nasm (version 0.98.39 is preferable) on your system])
fi

AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])
AC_HEADER_STDBOOL
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
