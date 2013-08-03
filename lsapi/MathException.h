//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if !defined(MATHEXCEPTION_H)
#define MATHEXCEPTION_H

#include <stdexcept>
#include <string>


/**
 * Exception thrown when an error occurs parsing or evaluating a math
 * expression.
 */
class MathException : public std::runtime_error
{
public:
    /**
     * Constructs a MathException with the specified message.
     */
    MathException(const std::wstring& message) throw() : std::runtime_error(std::string(message.begin(), message.end()))
    {
        this->sMessage = message;
    }

    const std::wstring & GetException() const
    {
        return sMessage;
    }

    std::wstring sMessage;
};


#endif
