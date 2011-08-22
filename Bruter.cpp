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
#include "Bruter.h"
#include "MD5.h"

using namespace std;

// Declare the static variable.
bool CBruter::m_bAbort;


CBruter::CBruter(CPlain *forPlain, CCharset *forCharset, CPackage *forPackage, CStats *forStats)
	: myPlain(forPlain)
	, myCharset(forCharset)
	, myPackage(forPackage)
	, myStats(forStats)
{
	// Make some space for the result and the thread container.
	m_cResult = new char[forPlain->GetLen() + 1];
	m_thThreads = new boost::thread[boost::thread::hardware_concurrency()];

	// Null termination.
	m_cResult[forPlain->GetLen()] = '\0';

	// Init our vars.
	m_bCracked = m_bAbort = false;
}

CBruter::~CBruter()
{
	SAFE_DELETE(m_cResult);
	SAFE_DELETE(m_thThreads);
}

void CBruter::Launch()
{
	for (unsigned int i = 0; i < boost::thread::hardware_concurrency(); i++)
		// Create threads.
		m_thThreads[i] = boost::thread(&CBruter::ThreadEntry, this, i);
}

void CBruter::Wait()
{
	for (unsigned int i = 0; i < boost::thread::hardware_concurrency(); i++)
		// Wait for the threads to finish.
		m_thThreads[i].join();
}

void CBruter::Abort()
{
	// Set abort to true which will eventually cancel all bruting threads.
	m_bAbort = true;
}

bool CBruter::HasResult()
{
	// We got a result?
	return m_bCracked;
}

char* CBruter::GetResult()
{
	// Return our result.
	return m_cResult;
}

void CBruter::ThreadEntry(const int threadIdx)
{
	// Run the loop.
	switch (myPlain->GetLen()) {
		case 4:
			Loop_4char(threadIdx);
			break;
		case 5:
			Loop_5char(threadIdx);
			break;
		case 6:
			Loop_6char(threadIdx);
			break;
		case 7:
			Loop_7char(threadIdx);
			break;
		case 8:
			Loop_8char(threadIdx);
			break;
		case 9:
			Loop_9char(threadIdx);
			break;
		case 10:
			Loop_10char(threadIdx);
			break;
		case 11:
			Loop_11char(threadIdx);
			break;
		default:
			break;
	}
}

/* This is a bruter which uses reversing.
	Keep in mind:
		=> data[1] needs to be constant as long as possible. This means:
		Change data[0] first (the first four chars),
		once these loops complete, change the last 2 chars, re-reverse
		and start over.
	Also, why specialized?
		=> The varibal which holds our current plain resides on the stack
		(instead of the heap if we were to use new/delete) whick makes this
		quite a bit faster.
*/
void CBruter::Loop_4char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 4;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[8];
	plain[length] = (char)0x80;

	// Before we begin with the inner loop, lets re-reverse using
	// the new data[1], which are the last 2 chars of our plain as
	// a 32 bit integer.
	MD5Reverse(plain, length, threadIdx);

	// Enter the loop.
	for (int i0 = split->firstChar; i0 <= split->lastChar; i0++) {
		plain[0] = myCharset->GetAt(i0);

		for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
			plain[1] = myCharset->GetAt(i1);

			for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
				plain[2] = myCharset->GetAt(i2);

				for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
					plain[3] = myCharset->GetAt(i3);

					if (MD5Calculate(plain, length, threadIdx)) {
						// Copy to result.
						memcpy(m_cResult, plain, length);

						// Update status.
						m_bCracked = true;

						// Bye.
						return;
					}
				}
			}

			// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
			if (m_bCracked || m_bAbort)
				return;

			// Update stats (this needs to be done after charsetLen^2).
			myStats->Update(myPlain, myCharset, threadIdx);
		}
	}
}

void CBruter::Loop_5char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 5;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[8];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		// Before we begin with the inner loop, lets re-reverse using
		// the new data[1], which are the last 2 chars of our plain as
		// a 32 bit integer.
		MD5Reverse(plain, length, threadIdx);

		for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
			plain[0] = myCharset->GetAt(i0);

			for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
				plain[1] = myCharset->GetAt(i1);

				for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
					plain[2] = myCharset->GetAt(i2);

					for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
						plain[3] = myCharset->GetAt(i3);

						if (MD5Calculate(plain, length, threadIdx)) {
							// Copy to result.
							memcpy(m_cResult, plain, length);

							// Update status.
							m_bCracked = true;

							// Bye.
							return;
						}
					}
				}

				// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
				if (m_bCracked || m_bAbort)
					return;

				// Update stats (this needs to be done after charsetLen^2).
				myStats->Update(myPlain, myCharset, threadIdx);
			}
		}
	}
}

void CBruter::Loop_6char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 6;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[8];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		for (int i5 = 0; i5 < myCharset->GetLen(); i5++) {
			plain[5] = myCharset->GetAt(i5);

			// Before we begin with the inner loop, lets re-reverse using
			// the new data[1], which are the last 2 chars of our plain as
			// a 32 bit integer.
			MD5Reverse(plain, length, threadIdx);

			for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
				plain[0] = myCharset->GetAt(i0);

				for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
					plain[1] = myCharset->GetAt(i1);
		
					for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
						plain[2] = myCharset->GetAt(i2);

						for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
							plain[3] = myCharset->GetAt(i3);

							if (MD5Calculate(plain, length, threadIdx)) {
								// Copy to result.
								memcpy(m_cResult, plain, length);

								// Update status.
								m_bCracked = true;

								// Bye.
								return;
							}
						}
					}

					// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
					if (m_bCracked || m_bAbort)
						return;

					// Update stats (this needs to be done after charsetLen^2).
					myStats->Update(myPlain, myCharset, threadIdx);
				}
			}
		}
	}
}

void CBruter::Loop_7char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 7;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[8];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		for (int i5 = 0; i5 < myCharset->GetLen(); i5++) {
			plain[5] = myCharset->GetAt(i5);

			for (int i6 = 0; i6 < myCharset->GetLen(); i6++) {
				plain[6] = myCharset->GetAt(i6);

				// Before we begin with the inner loop, lets re-reverse using
				// the new data[1], which are the last 2 chars of our plain as
				// a 32 bit integer.
				MD5Reverse(plain, length, threadIdx);

				for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
					plain[0] = myCharset->GetAt(i0);

					for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
						plain[1] = myCharset->GetAt(i1);

						for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
							plain[2] = myCharset->GetAt(i2);

							for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
								plain[3] = myCharset->GetAt(i3);

								if (MD5Calculate(plain, length, threadIdx)) {
									// Copy to result.
									memcpy(m_cResult, plain, length);

									// Update status.
									m_bCracked = true;

									// Bye.
									return;
								}
							}
						}

						// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
						if (m_bCracked || m_bAbort)
							return;

						// Update stats (this needs to be done after charsetLen^2).
						myStats->Update(myPlain, myCharset, threadIdx);
					}
				}
			}
		}
	}
}

void CBruter::Loop_8char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 8;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[12];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		for (int i5 = 0; i5 < myCharset->GetLen(); i5++) {
			plain[5] = myCharset->GetAt(i5);

			for (int i6 = 0; i6 < myCharset->GetLen(); i6++) {
				plain[6] = myCharset->GetAt(i6);

				for (int i7 = 0; i7 < myCharset->GetLen(); i7++) {
					plain[7] = myCharset->GetAt(i7);

					// Before we begin with the inner loop, lets re-reverse using
					// the new data[1], which are the last 2 chars of our plain as
					// a 32 bit integer.
					MD5Reverse_8plus(plain, length, threadIdx);

					for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
						plain[0] = myCharset->GetAt(i0);

						for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
							plain[1] = myCharset->GetAt(i1);

							for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
								plain[2] = myCharset->GetAt(i2);

								for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
									plain[3] = myCharset->GetAt(i3);

									if (MD5Calculate_8plus(plain, length, threadIdx)) {
										// Copy to result.
										memcpy(m_cResult, plain, length);

										// Update status.
										m_bCracked = true;

										// Bye.
										return;
									}
								}
							}

							// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
							if (m_bCracked || m_bAbort)
								return;

							// Update stats (this needs to be done after charsetLen^2).
							myStats->Update(myPlain, myCharset, threadIdx);
						}
					}
				}
			}
		}
	}
}

void CBruter::Loop_9char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 9;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[12];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		for (int i5 = 0; i5 < myCharset->GetLen(); i5++) {
			plain[5] = myCharset->GetAt(i5);

			for (int i6 = 0; i6 < myCharset->GetLen(); i6++) {
				plain[6] = myCharset->GetAt(i6);

				for (int i7 = 0; i7 < myCharset->GetLen(); i7++) {
					plain[7] = myCharset->GetAt(i7);

					for (int i8 = 0; i8 < myCharset->GetLen(); i8++) {
						plain[8] = myCharset->GetAt(i8);

						// Before we begin with the inner loop, lets re-reverse using
						// the new data[1], which are the last 2 chars of our plain as
						// a 32 bit integer.
						MD5Reverse_8plus(plain, length, threadIdx);

						for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
							plain[0] = myCharset->GetAt(i0);

							for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
								plain[1] = myCharset->GetAt(i1);

								for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
									plain[2] = myCharset->GetAt(i2);

									for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
										plain[3] = myCharset->GetAt(i3);

										if (MD5Calculate_8plus(plain, length, threadIdx)) {
											// Copy to result.
											memcpy(m_cResult, plain, length);

											// Update status.
											m_bCracked = true;

											// Bye.
											return;
										}
									}
								}

								// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
								if (m_bCracked || m_bAbort)
									return;

								// Update stats (this needs to be done after charsetLen^2).
								myStats->Update(myPlain, myCharset, threadIdx);
							}
						}
					}
				}
			}
		}
	}
}

void CBruter::Loop_10char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 10;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[12];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		for (int i5 = 0; i5 < myCharset->GetLen(); i5++) {
			plain[5] = myCharset->GetAt(i5);

			for (int i6 = 0; i6 < myCharset->GetLen(); i6++) {
				plain[6] = myCharset->GetAt(i6);

				for (int i7 = 0; i7 < myCharset->GetLen(); i7++) {
					plain[7] = myCharset->GetAt(i7);

					for (int i8 = 0; i8 < myCharset->GetLen(); i8++) {
						plain[8] = myCharset->GetAt(i8);

						for (int i9 = 0; i9 < myCharset->GetLen(); i9++) {
							plain[9] = myCharset->GetAt(i9);

							// Before we begin with the inner loop, lets re-reverse using
							// the new data[1], which are the last 2 chars of our plain as
							// a 32 bit integer.
							MD5Reverse_8plus(plain, length, threadIdx);

							for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
								plain[0] = myCharset->GetAt(i0);

								for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
									plain[1] = myCharset->GetAt(i1);

									for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
										plain[2] = myCharset->GetAt(i2);

										for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
											plain[3] = myCharset->GetAt(i3);

											if (MD5Calculate_8plus(plain, length, threadIdx)) {
												// Copy to result.
												memcpy(m_cResult, plain, length);

												// Update status.
												m_bCracked = true;

												// Bye.
												return;
											}
										}
									}

									// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
									if (m_bCracked || m_bAbort)
										return;

									// Update stats (this needs to be done after charsetLen^2).
									myStats->Update(myPlain, myCharset, threadIdx);
								}
							}
						}
					}
				}
			}
		}
	}
}

void CBruter::Loop_11char(const int threadIdx)
{
	// Our constant for the length.
	const int length = 11;
	
	// The work this thread needs to do.
	Split_t *split = myPackage->GetPackage(threadIdx);

	// Our current plain with the message
	// end bit already set. While seeming obsolete,
	// this is needed for the reversal.
	char plain[12];
	plain[length] = (char)0x80;

	// This is the outer loop, change the last 2 chars (=> data[1]).
	for (int i4 = split->firstChar; i4 <= split->lastChar; i4++) {
		plain[4] = myCharset->GetAt(i4);

		for (int i5 = 0; i5 < myCharset->GetLen(); i5++) {
			plain[5] = myCharset->GetAt(i5);

			for (int i6 = 0; i6 < myCharset->GetLen(); i6++) {
				plain[6] = myCharset->GetAt(i6);

				for (int i7 = 0; i7 < myCharset->GetLen(); i7++) {
					plain[7] = myCharset->GetAt(i7);

					for (int i8 = 0; i8 < myCharset->GetLen(); i8++) {
						plain[8] = myCharset->GetAt(i8);

						for (int i9 = 0; i9 < myCharset->GetLen(); i9++) {
							plain[9] = myCharset->GetAt(i9);

							for (int i10 = 0; i10 < myCharset->GetLen(); i10++) {
								plain[10] = myCharset->GetAt(i10);

								// Before we begin with the inner loop, lets re-reverse using
								// the new data[1], which are the last 2 chars of our plain as
								// a 32 bit integer.
								MD5Reverse_8plus(plain, length, threadIdx);

								for (int i0 = 0; i0 < myCharset->GetLen(); i0++) {
									plain[0] = myCharset->GetAt(i0);

									for (int i1 = 0; i1 < myCharset->GetLen(); i1++) {
										plain[1] = myCharset->GetAt(i1);

										for (int i2 = 0; i2 < myCharset->GetLen(); i2++) {
											plain[2] = myCharset->GetAt(i2);

											for (int i3 = 0; i3 < myCharset->GetLen(); i3++) {
												plain[3] = myCharset->GetAt(i3);

												if (MD5Calculate_8plus(plain, length, threadIdx)) {
													// Copy to result.
													memcpy(m_cResult, plain, length);

													// Update status.
													m_bCracked = true;

													// Bye.
													return;
												}
											}
										}

										// Already cracked? (don't check every single plain, once every charsetLen^2 is enough)
										if (m_bCracked || m_bAbort)
											return;

										// Update stats (this needs to be done after charsetLen^2).
										myStats->Update(myPlain, myCharset, threadIdx);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}