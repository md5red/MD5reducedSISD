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

#ifndef BRUTER_H
 #define BRUTER_H


#include "Plain.h"
#include "Charset.h"
#include "Package.h"
#include "Stats.h"
#include <boost/thread.hpp> 

class CBruter
{
private:
	CPlain *myPlain;
	CCharset *myCharset;
	CPackage *myPackage;
	CStats *myStats;

	bool m_bCracked; // Thread-safe? Yes, at the very most 1 writer and a few readers (it wouldn't matter much anyway).
	static bool m_bAbort; // Static boolean used for aborting. Only one writer.

	boost::thread *m_thThreads; // Thread container.
	char *m_cResult; // The result.
	
public:
	CBruter(CPlain *forPlain, CCharset *forCharset, CPackage *forPackage, CStats *forStats);
	~CBruter();

	void Launch(); // Launch our threads.
	void Wait(); // ... and wait for them to finish.
	static void Abort(); // Static method used for aborting.

	bool HasResult(); // We got something?
	char* GetResult(); // Get what we have.

private:
	void ThreadEntry(const int threadIdx);

	void Loop_4char(const int threadIdx);
	void Loop_5char(const int threadIdx);
	void Loop_6char(const int threadIdx);
	void Loop_7char(const int threadIdx);
	void Loop_8char(const int threadIdx);
	void Loop_9char(const int threadIdx);
	void Loop_10char(const int threadIdx);
	void Loop_11char(const int threadIdx);
};


#endif