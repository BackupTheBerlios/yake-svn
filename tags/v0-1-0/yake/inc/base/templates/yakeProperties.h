/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_BASE_TEMPLATES_PROPERTIES_H
#define YAKE_BASE_TEMPLATES_PROPERTIES_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
#define YAKE_DECLARE_PROPERTY_GET( type, getfunc, var )                             \
struct _dp_##getfunc                                                                 \
{                                                                                    \
    inline operator type()                                                           \
    {                                                                                \
        return ((THIS_CLASS *) ((char *) this - offsetof( THIS_CLASS, var )))->getfunc();        \
    }                                                                                \
};                                                                                   \
friend struct _dp_##getfunc;                                                         \
_dp_##getfunc var

#define YAKE_DECLARE_PROPERTY_PUT( type, putfunc, var )                             \
struct _dp_##putfunc                                                                 \
{                                                                                    \
    inline type operator=(const type & value)                                        \
    {                                                                                \
        return ((THIS_CLASS *) ((char *) this - offsetof( THIS_CLASS, var )))->putfunc( value ); \
    }                                                                                \
};                                                                                   \
friend struct _dp_##putfunc;                                                         \
_dp_##putfunc var

#define YAKE_DECLARE_PROPERTY( type, getfunc, putfunc, var )                        \
struct _dp_##getfunc_##putfunc                                                       \
{                                                                                    \
    inline operator type()                                                           \
    {                                                                                \
        return ((THIS_CLASS *) ((char *) this - offsetof( THIS_CLASS, var )))->getfunc();        \
    }                                                                                \
    inline type operator=(const type & value)                                        \
    {                                                                                \
        return ((THIS_CLASS *) ((char *) this - offsetof( THIS_CLASS, var )))->putfunc( value ); \
    }                                                                                \
};                                                                                   \
friend struct _dp_##getfunc_##putfunc;                                               \
	_dp_##getfunc##_##putfunc var

#endif // YAKE_BASE_TEMPLATES_PROPERTIES_H
