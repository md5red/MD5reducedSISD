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
#include <boost/thread.hpp> 
#include "Package.h"

using namespace std;


CPackage::CPackage(CCharset *forCharset)
{
	Initialize(forCharset);
}

CPackage::~CPackage()
{
}

Split_t *CPackage::GetPackage(const int forThread)
{
	return &(m_vSplitted[forThread]);
}

void CPackage::Initialize(CCharset *forCharset)
{
	double size = static_cast<double>(forCharset->GetLen());

	// Go.
	for (unsigned int i = 0; i < boost::thread::hardware_concurrency(); i++) {
		// Init.
		Split_t split;

		// Calc.
		split.firstChar = (i == 0) ? (int)(size / boost::thread::hardware_concurrency() * i) : (int)(size / boost::thread::hardware_concurrency() * i + 1);
		split.lastChar = (int)(size / boost::thread::hardware_concurrency() * (i + 1));
		if (split.lastChar > (forCharset->GetLen() - 1))
			split.lastChar = (int)(forCharset->GetLen() - 1);

		// Save.
		m_vSplitted.push_back(split);
	}
}