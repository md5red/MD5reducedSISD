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

#ifndef PACKAGE_H
 #define PACKAGE_H

#include <vector>
#include "Charset.h"

// The struct.
typedef struct Split_s
{
	int firstChar;
	int lastChar;
} Split_t;


// The class.
class CPackage
{
private:
	std::vector<Split_t> m_vSplitted;

public:
	CPackage(CCharset *forCharset);
	~CPackage();

	Split_t *GetPackage(const int forThread); // Splits the work in x packages, where x is the number of threads.

private:
	void Initialize(CCharset *forCharset);
};

#endif
