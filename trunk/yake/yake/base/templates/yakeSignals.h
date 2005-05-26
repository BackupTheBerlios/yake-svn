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
#ifndef INC_YAKE_SIGNALS_H
#define INC_YAKE_SIGNALS_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif

#if defined( YAKE_SIGNALS_USE_BOOST )
#	include "boost/signal.hpp"
#	include "boost/function.hpp"

// WORKAROUND: Problems with boost::bind involving anonymous namespace members _1, _2 etc
// occur when using PCH (linking errors, '..::_1 already defined ...').
// This problem is known and several threads on it can be found on the boost mailing list.
// We use this solution (source: http://mail.python.org/pipermail/c++-sig/2003-October/005936.html):
// [Excerpt from the message:]
//
//	As I get it VC7.1 also has this bug with anonymous namespace:
//	... : error LNK2005:
//	"class boost::arg<6> `anonymous namespace'::_6"
//	... already defined in ...
//
//	fixes if BOOST_MSVC <= 1300  replaced with BOOST_MSVC <= 1310 in
//	boost/bind/placeholders.hpp line 39
//
//	Best regards, Kerim (mailto:warkid at hotbox.ru)
//
#	include "boost/bind.hpp"
#elif defined( YAKE_SIGNALS_USE_TTL )
#	include <ttl/sig/signal.hpp>
#	include <boost/bind.hpp>
#endif

#if defined( YAKE_SIGNALS_USE_BOOST )
#	pragma message("Yake signal config: yake.base.templates.signals.boost")
#	if YAKE_COMPILER == COMPILER_MSVC && YAKE_COMP_VER > 1300
#		define Bind0( METHOD, OBJECT ) boost::bind( METHOD, OBJECT, _1 )
#	else
#		define Bind0( METHOD, OBJECT ) boost::bind( METHOD, OBJECT )
#	endif
#	define Bind1( METHOD, OBJECT ) boost::bind( METHOD, OBJECT, _1 )
#	define Bind2( METHOD, OBJECT ) boost::bind( METHOD, OBJECT, _1, _2 )
#	define BindRef1( METHOD, REFOBJECT ) boost::bind( METHOD, boost::ref( REFOBJECT ), _1 )
#	define BindRef0( METHOD, REFOBJECT ) boost::bind( METHOD, boost::ref( REFOBJECT ) )
	// We're using the portable/compatible syntax for Signal even though some compilers
	// would be able to process what boost calls the 'preferred' syntax.
#	define Signal0 boost::signal
#	define Signal1 boost::signal
#	define Signal2 boost::signal
#	define Signal3 boost::signal
#	define Function0 boost::function0
#	define Function1 boost::function1
#	define Function2 boost::function2
#	define Function3 boost::function3
	typedef boost::signals::connection SignalConnection;
/*
#	define SIG boost
#	define FUNC boost
//#	define Bind boost::bind
#	define Ref( REFOBJECT ) boost::ref( REFOBJECT )
*/
#elif defined( YAKE_SIGNALS_USE_TTL )
#	pragma message("Yake signal config: yake.base.templates.signals.ttl")

#	if YAKE_COMPILER == COMPILER_MSVC && YAKE_COMP_VER > 1300
#		define Bind0( METHOD, OBJECT ) boost::bind( METHOD, OBJECT, _1 )
#	else
#		define Bind0( METHOD, OBJECT ) boost::bind( METHOD, OBJECT )
#	endif
#	define Bind1( METHOD, OBJECT ) boost::bind( METHOD, OBJECT, _1 )
#	define Bind2( METHOD, OBJECT ) boost::bind( METHOD, OBJECT, _1, _2 )
#	define BindRef1( METHOD, REFOBJECT ) boost::bind( METHOD, boost::ref( REFOBJECT ), _1 )
#	define BindRef0( METHOD, REFOBJECT ) boost::bind( METHOD, boost::ref( REFOBJECT ) )
#	define SignalX ttl::sig::signal
//#	define Signal ttl::sig::signal // can't use this macro as it collides with some MS COM method named "Signal"
#	define Signal0 ttl::sig::signal
#	define Signal1 ttl::sig::signal
#	define Signal2 ttl::sig::signal
#	define Signal3 ttl::sig::signal
namespace yake {
	typedef ttl::sig::connection SignalConnection;
} // namespace yake
#else
#	error("Yake signal error: No signals provider.")
#endif

namespace yake 
{


} // yake


#define YAKE_MEMBERSIGNAL_VIRTUALIMPL( ACCESS, SIGNATURE, NAME ) \
ACCESS: \
	virtual SignalConnection subscribeTo##NAME##(const Sig##NAME##Type::slot_type& slot ) \
	{ return mSig##NAME.connect(slot); } \
private: \
	Sig##NAME##Type	mSig##NAME; \
ACCESS:

#define YAKE_MEMBERSIGNAL_FIRE_FN0(ACCESS, NAME) \
ACCESS: \
	void fire##NAME##() \
	{ mSig##NAME##(); }

#define YAKE_MEMBERSIGNAL_FIRE_FN1(ACCESS, NAME, PARAMS, PARAMSCALL) \
ACCESS: \
	void fire##NAME##(PARAMS) \
	{ mSig##NAME##(PARAMSCALL); }

#define YAKE_MEMBERSIGNAL( ACCESS, SIGNATURE, NAME ) \
ACCESS: \
	typedef SignalX<SIGNATURE> Sig##NAME##Type; \
	SignalConnection subscribeTo##NAME##(const Sig##NAME##Type::slot_type& slot ) \
	{ return mSig##NAME.connect(slot); } \
private: \
	Sig##NAME##Type	mSig##NAME;

#endif
