/*
MD5reducedSISD
Copyright (C) 2011 Tobias Sielaff

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAIN_H
 #define MAIN_H

// Gernal.
#define VERSION "1.0.0"

// Build config.
//#define DEBUG // Enables a few more error prints/mem leak tests.
//#define EXTENDED_INFO // Prints a few extra lines.
#define PACKAGE_SIZE (12) // 3x SSE2 instructions using 128 bit registers.
#define UPDATE_EVERY (500) // Print to console every x ms. This aquires a lock, so its wise to set this as high as possible.

// Helpers.
#define SAFE_DELETE(ptr)	\
	if (ptr != NULL)		\
		delete[] ptr;		\
	ptr = NULL;


#endif