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
#ifndef YAPP_TASK_H
#define YAPP_TASK_H

namespace yake {
	using namespace base;
	using namespace base::templates;
namespace app {
namespace task {

	typedef uint8 TaskPriority;

	const TaskPriority TASKPRIO_LOW		=  63;
	const TaskPriority TASKPRIO_MEDIUM	= 127;
	const TaskPriority TASKPRIO_HIGH	= 191;

	/** Represents a task. Derived classes have to override two functions.
		@Remarks This task system is *not* a real multithreaded task system.
			It emulates something like that in a very remote way, though.
	*/
	class YAPP_BASE_API ITask
	{
	public:
		virtual ~ITask() {}

		/** Execute this task. */
		virtual void execute(real timeElapsed) = 0;

		/** Return this tasks current priority. */
		virtual TaskPriority getPriority() const = 0;

		/** Return the frequency this task gets executed (in Hz). */
		virtual real getExecutionFrequency() const = 0;
	};

	class YAPP_BASE_API DefTask : public ITask
	{
	protected:
		DefTask( real freq, TaskPriority prio = TASKPRIO_MEDIUM ) :
				mFreq(freq),
				mPrio(prio)
		{}
	public:
		virtual TaskPriority getPriority() const
		{ return mPrio; }

		virtual real getExecutionFrequency() const
		{ return mFreq; }
	private:
		real				mFreq;
		TaskPriority	mPrio;
	};

	/** Manages tasks. :) */
	class YAPP_BASE_API TaskManager
	{
	public:
		~TaskManager();

		//@{ Operations
		void addTask( ITask* pTask, bool bExecuteOnlyOnce = false );
		void removeTask( ITask* pTask );
		//@}

		void update(real timeElapsed);
	private:
		static const uint32 TE_SINGLE_EXEC;
		struct TaskEntry
		{
			ITask*	pTask;
			uint32	flags;
			real	timeLeft;

			bool operator == (const TaskEntry & rhs)
			{
				return (	pTask == rhs.pTask &&
							flags == rhs.flags &&
							timeLeft == rhs.timeLeft );
			}

			bool operator == (const ITask* pRhsTask)
			{
				return ( pTask == pRhsTask );
			}

			///@name Sorting
			//@{
			//operator > 
			static bool sortDescending( const TaskEntry & rStart, const TaskEntry & rEnd)
			{
				YAKE_ASSERT( rStart.pTask );
				YAKE_ASSERT( rEnd.pTask );
				return (rStart.pTask->getPriority() > rEnd.pTask->getPriority());
			}
			//@}
		};
		typedef Deque<TaskEntry> TaskList;
		TaskList		mTasks;
		bool			mTaskListNeedsSorting;
	};

/*	class PhysicsTask : public DefTask
	{
	public:
		PhysicsTask( physics::IWorld* pWorld, const real freq, const TaskPriority prio = TASKPRIO_MEDIUM ) :
			DefTask( freq, prio ),
			mWorld(pWorld)
		{
			YAKE_ASSERT( mWorld );
		}
		~PhysicsTasks();
		virtual void execute()
		{
			YAKE_ASSERT( mWorld );
			mWorld->step( timeElapsed );
		}
	private:
		physics::IWorld*		mWorld;
	};

	App:
		-mTaskPhysics: PhysicsTask
		-mTaskGraphics: GraphicsTask
		+setupTasks:
			mTaskPhysics.reset( new PhysicsTask(mPWorld.get(), 50.) );
			mTaskGraphics.reset( new GraphicsTask(mGWorld.get(), 72. ) ); // limit to refresh rate 72Hz
			OR
			mTaskGraphics.reset( new GraphicsTask(mGWorld.get(), 0. ) ); // unlimited

			mTaskMgr->addTask( mTaskPhysics.get() );
			mTaskMgr->addTask( mTaskGraphics.get() );

		+run:
			in loop:
				mTaskMgr->update(timeElapsed);
*/

} // task
} // app
} // yake

#endif