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
#ifndef YAKE_BASE_PCH_H
#define YAKE_BASE_PCH_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include "templates/yakeSmartAssert.h"
#include "templates/yakeRegistry.h"
#include "templates/yakeVector.h"
#include "templates/yakePointer.h"
#include <yake/base/templates/yakeDeque.h>

#include "mpl/yakeAbstractFactory.h"
#include "mpl/yakeSequences.h"

#include "yakePrerequisites.h"
#include "yakeVersion.h"
#include "yakeString.h"
#include "yakeException.h"
#include "yakeLog.h"
#include "yakeOutputStream.h"
#include "yakeSerializable.h"
#include "yakeUniqueName.h"
#include "yakePlugin.h"
#include "yakeDebugOutputStream.h"
#include "yakeLibrary.h"

#include "math/yakeQuaternion.h"
#include "math/yakeVector3.h"
#include "math/yakeVector4.h"
#include "math/yakeMatrix3.h"
#include "math/yakeMatrix4.h"
#include "math/yakeColor.h"
#include "math/yakeGeometry.h"
#include "math/yakeSimpleSpline.h"

#include "yakeState.h"
#include "yakeParamHolder.h"
#include "yakeMovable.h"
#include "yakeCommandInterface.h"

#include "yakeTimer.h"



#endif // YAKE_BASE_PCH_H
