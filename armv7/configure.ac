dnl Copyright (C) 2013 Gabriel Krisman Bertazi <gabriel@krisman.be>
dnl
dnl This file is part of arm-sim.
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([arm-sim], [0.1], [gabriel@krisman.be], [arm-sim])

dnl If you update this auxiliary directory, remember to also update the
dnl value of $AUX_DIR in the 'bootstrap.sh' script.
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([dist-bzip2 foreign])

dnl Check for a C compiler.
AC_PROG_CC
AC_PROG_CXX

dnl Check for acsim.
AC_CHECK_PROG([acsim],[acsim],[yes],[no])
AM_CONDITIONAL([FOUND_ACSIM], [test "x$acsim" = xyes])
AM_COND_IF([FOUND_ACSIM],,[AC_MSG_ERROR([required program 'acsim' not found.])])

AC_CONFIG_FILES([Makefile src/Makefile tools/Makefile])
AC_OUTPUT

