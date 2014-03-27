# This file is part of g2048.
#
# Copyright 2014 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
#
# g2048 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# g2048 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with g2048.  If not, see <http://www.gnu.org/licenses/>.

bin_PROGRAMS +=   \
	bin/g2048 \
	$(NULL)

bin_g2048_SOURCES =            \
	src/g2048/g2048-tile.h \
	src/g2048/g2048-tile.c \
	src/g2048/g2048-grid.h \
	src/g2048/g2048-grid.c \
	src/g2048/g2048.c      \
	$(NULL)

bin_g2048_LDADD =  \
	$(AM_LIBS) \
	$(NULL)
