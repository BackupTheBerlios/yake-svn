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
#ifndef YAKE_THREAD_ZTHREADTHREADFORWARDER_H
#define YAKE_THREAD_ZTHREADTHREADFORWARDER_H

#include <zthread/Thread.h>

namespace yake {
namespace thread {

	class YAKE_THREAD_API Thread : public IThread
	{
	private:
		struct RunnableAdapter : public ZThread::Runnable
		{
			SharedIRunnablePtr	m_r;
			RunnableAdapter(SharedIRunnablePtr pR) : m_r(pR)
			{}
			virtual void run() 
			{
				m_r->run();
			}
		};
		Thread();
		Thread(const Thread&);
		Thread(SharedIRunnablePtr pRunnable)
		{
			YAKE_ASSERT( pRunnable );
			m_t = new ZThread::Thread( new RunnableAdapter(pRunnable) );
			YAKE_ASSERT( m_t );
		}
	public:
		~Thread()
		{
			delete m_t;
		}
		static Thread* create(IRunnablePtr pRunnable) throw( ThreadException )
		{
			return create( SharedIRunnablePtr(pRunnable) );
		}
		static Thread* create(SharedIRunnablePtr pRunnable) throw( ThreadException )
		{
			try {
				if (!pRunnable.get())
					return 0;
				return new Thread(pRunnable);
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
		virtual void yield() throw( ThreadException )
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			try {
				m_t->yield();
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
		virtual void sleep(const uint32 ms) throw( ThreadException )
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			try {
				m_t->sleep( ms );
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
		virtual void wait(const uint32 ms = 0) throw( ThreadException )
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			try {
				m_t->wait( ms );
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
	private:
		ZThread::Thread*	m_t;
	};

} // namespace thread
} // namespace yake

#endif