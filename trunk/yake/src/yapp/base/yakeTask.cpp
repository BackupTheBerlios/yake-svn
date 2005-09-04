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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yakeTask.h>

namespace yake {
namespace task {

	const uint32 TaskManager::TE_SINGLE_EXEC = 0x00000001;

	TaskManager::~TaskManager()
	{
		for (TaskList::iterator it = mTasks.begin(); it != mTasks.end(); ++it)
		{
			delete (*it).pTask;
		}
		mTasks.clear();
	}

	void TaskManager::addTask( ITask* pTask, bool bExecuteOnlyOnce )
	{
		YAKE_ASSERT( pTask );
		TaskEntry e;
		e.flags = 0;
		if (bExecuteOnlyOnce)
			e.flags |= TE_SINGLE_EXEC;
		e.pTask = pTask;
		e.timeLeft = 0.;
		mTasks.push_back( e );
		mTaskListNeedsSorting = true;
	}

	void TaskManager::removeTask( ITask* pTask )
	{
		YAKE_ASSERT( pTask );
		TaskList::iterator itRemove = std::find( mTasks.begin(), mTasks.end(), pTask );
		if (itRemove != mTasks.end())
			mTasks.erase( itRemove );
	}

	void TaskManager::update(real timeElapsed)
	{
		std::cout << "--tasks(" << timeElapsed << ")\n";
		if (timeElapsed <= 0.001)
			return;
		// sort tasks by priority (descending)
		//if (mTaskListNeedsSorting)
		//	std::sort( mTasks.begin(), mTasks.end(), TaskEntry::sortDescending );

		// iterate over tasks
		TaskList::iterator it = mTasks.begin();
		while (it != mTasks.end())
		{
			TaskEntry& e = *it;

			e.timeLeft -= timeElapsed;

			// tasks time up? (re)set timer, execute
			if (e.timeLeft <= 0.)
			{
				YAKE_ASSERT( e.pTask );
				real freq = e.pTask->getExecutionFrequency();
				// we know that e.timeLeft is < 0
				if (freq <= 0.)
					e.pTask->execute( timeElapsed );
				else
				{
					while (e.timeLeft < freq)
					{
						e.pTask->execute( freq );
						e.timeLeft += freq;
					}
				}
				if (e.flags & TE_SINGLE_EXEC)
				{
					//e.flags |= TE_FINISHED;
					it = mTasks.erase( it ); // erase task and get next in line
				}
				else
					++it;
			}
			else
				++it;
		}
	}

} // task
} // yake
