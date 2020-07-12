/**
 *  A simple interactive texas holdem poker program.
 *  Copyright (C) 2020, Matt Zimmerer, mzimmere@gmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/

#include <utl/string>

namespace Exception {

	///  The maximum error message length
	static constexpr size_t MAX_ERROR_MESSAGE_LEN = 64;

	/** A wrapper for exceptions. When compiled for embedded devices, this will be an infinite loop
	 *  @param message A message to pass with this exception
	 */
	void EXCEPTION(const utl::string<MAX_ERROR_MESSAGE_LEN>& message);
};