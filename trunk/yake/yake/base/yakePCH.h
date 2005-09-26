/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef YAKE_BASE_PCH_H
#define YAKE_BASE_PCH_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include "yakePrerequisites.h"

#include "templates/yakeSmartAssert.h"
#include "templates/yakeRegistry.h"
#include "templates/yakeVector.h"
#include "templates/yakePointer.h"
#include <yake/base/templates/yakeDeque.h>
#include <yake/base/templates/delete.h>
#include <yake/base/templates/yakeVariant.h>
#include "templates/yakeAlgorithms.h"

#include <yake/base/mpl/yakeAbstractFactory.h>
#include <yake/base/mpl/sequences.h>

#include "yakePrerequisites.h"
#include "yakeVersion.h"
#include "yakeString.h"
#include "yakeException.h"
#include "yakeLog.h"
#include "yakeOutputStream.h"
#include "yakeUniqueName.h"
#include "yakePlugin.h"
//#include "yakeDebugOutputStream.h"
#include "yakeLibrary.h"
#include <yake/base/type_info.h>

#include "math/yakeQuaternion.h"
#include "math/yakeVector3.h"
#include "math/yakeVector4.h"
#include "math/yakeMatrix3.h"
#include "math/yakeMatrix4.h"
#include "math/yakeColor.h"
#include "math/yakeGeometry.h"
#include "math/yakeSimpleSpline.h"
#include "math/yakePlane.h"
#include "math/yakeRay.h"

#include "yakeParamHolder.h"
#include "yakeMovable.h"
#include "yakeCommandInterface.h"

#include "yakeTimer.h"



#endif // YAKE_BASE_PCH_H
