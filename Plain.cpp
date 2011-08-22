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

#include "Main.h"
#include <iostream>
#include "Plain.h"

using namespace std;


// This resembles a immutable class (see Scala). You need another
// length? Create a new object.
// At the moment, only used for remembering the length of the current
// plain, but it's better expendable that way.
CPlain::CPlain(const int len)
	: m_iLen(len)
{
}

CPlain::~CPlain()
{
}

int CPlain::GetLen()
{
	return m_iLen;
}