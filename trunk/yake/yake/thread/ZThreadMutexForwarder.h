/*
   ------------------------------------------------------------------------------------
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
#ifndef YAKE_THREAD_ZTHREADMUTEXFORWARDER_H
#define YAKE_THREAD_ZTHREADMUTEXFORWARDER_H

#include <zthread/Mutex.h>

namespace yake {
namespace thread {

	/** @todo exception handling
	*/
	class ZThreadMutexForwarder
	{
	public:
		void acquire()
		{
			try {
				mtx_.acquire();
			} 
			catch(ZThread::Deadlock_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: deadlock in acquire()");
			} 
			catch(ZThread::Interrupted_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: interrupt in acquire()");
			}
		}
		bool tryAcquire(const uint32 timeout)
		{
			try {
				return mtx_.tryAcquire(timeout);
			}
			catch(ZThread::Deadlock_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: deadlock in tryAcquire()");
			} 
			catch(ZThread::Interrupted_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: interrupt in tryAcquire()");
			}
		}
		void release()
		{
			try {
				mtx_.release();
			} catch(ZThread::InvalidOp_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: invalid op in release()");
			}
		}
	private:
		ZThread::Mutex	mtx_;
	};

} // namespace thread
} // namespace yake

#endif

