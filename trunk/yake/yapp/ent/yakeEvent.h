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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_ENT_EVENT_H
#define YAKE_ENT_EVENT_H

namespace yake {
namespace ent {

	typedef std::map<String,boost::any> ParamList; //@todo

	class Event;

	struct YAKE_ENT_API EventData
	{
		Event*		evt;
		ParamList	params;
	};
	struct YAKE_ENT_API EventCallback
	{
		virtual ~EventCallback() {}
		virtual void onFire( const EventData& evtData ) = 0;
	};
	class YAKE_ENT_API Event
	{
		Event();
	public:
		Event(const String& n) : name(n) {}
		String getName() const;

		void fire(const ParamList& params);

		typedef boost::function<void(const ParamList&)> CbFn;
		void addCallback(const CbFn& fn);
		void addCallback(EventCallback* pCb);
	private:
		typedef std::deque<EventCallback*> CbList;
		CbList		callbacks;
		typedef std::deque<CbFn> CbFns;
		CbFns		callbackFns;

		String		name;
	};

} // namespace yake
} // namespace ent
#endif