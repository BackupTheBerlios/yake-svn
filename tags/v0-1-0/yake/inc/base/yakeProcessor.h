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

#ifndef YAKE_BASE_PROCESSOR_H
#define YAKE_BASE_PROCESSOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <inc/base/yakeString.h>
#include <inc/base/templates/yakeSingleton.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

class Processor
{
// Types
public:
  struct Extensions
  {
    bool hasFPU;        // Floating Point Unit.
    bool hasVME;        // Virtuel 8086 Mode Enhancements.
    bool hasDE;         // Debugging extensions.
    bool hasPSE;        // Page Size Extensions.
    bool hasTSC;        // Time Stamp Counter.
    bool hasMSR;        // Model Specific Registers
    bool hasPAE;        // Physical Address Extension.
    bool hasMCE;        // Machine Check Exception.
    bool hasCX8;        // COMPXCHG8B Instruction.
    bool hasAPIC;       // Advanced Programmable Interrupt Controller.
    unsigned int apicID;
    bool hasSEP;        // Fast System Call.
    bool hasMTRR;       // Memory Type Range Registers.
    bool hasPGE;        // PTE Global Flag.
    bool hasMCA;        // Machine Check Architecture.
    bool hasCMOV;       // Conditional Move And Compare Instructions.
    bool hasFGPAT;      // Page Attribute Table.
    bool hasPSE36;      // 36bit Page Size Extension.
    bool hasPSN;        // Processor Serial rNumber.
    bool hasCLFLUSH;    // CFLUSH Instruction.
    unsigned int clflushInstructionCacheLineSize;
    bool hasDS;         // Debug Store.
    bool hasACPI;       // Thermal Monitor And Clock Control.
    bool hasEMMX;       // Enhanced Multimedia Extensions.
    bool hasMMX;        // Multimedia Extensions.
    bool hasFXSR;       // Fast Streaming SIMD Extensions Save Restore.
    bool hasSSE;        // Streaming SIMD Extensions.
    bool hasSSE2;       // Streaming SIMD2 Extensions.
    bool hasSS;         // Self Snoop.
    bool hasHT;         // Hyper Threading.
    unsigned int hyperThreadingSiblings;
    bool hasTM;         // Thermal Monitor.
    bool hasIA64;       // Intel 64Bit Architecture.
    bool has3DNow;      // 3D Now! Instruction Extensions.
    bool hasE3DNow;     // Enhanced 3D Now! Instruction Extensions.
    bool hasAA64;       // AMD64BitArchitecture.
  };

  struct Cache
  {
    bool isPresent;
		String size;
    unsigned int numAssociativeWays;
    unsigned int lineSize;
    bool isSectored;
  };

  struct TLB
  {
    bool isPresent;
    String pageSize;
    unsigned int numAssociativeWays;
    unsigned int numEntries;
  };
  
  struct Info
  {
    enum Family { INTEL_I386, INTEL_I486, INTEL_PENTIUM, INTEL_PENTIUM_PRO, INTEL_PENTIUM_4, INTEL_IA64, INTEL_UNKNOWN_PENTIUM_4_OR_HIGHER, AMD_80486, AMD_5x86, AMD_K5, AMD_K6, AMD_K7, UNKNOWN_FAMILY };
    enum Type { ORIGINAL_OEM, OVERDRIVE, DUAL_CAPABLE, RESERVED, UNKNOWN_TYPE };

    String vendor;
    Family family;
    String familyString;
    Type type;
    String typeString;
    String model;
    String brandId;
    String serial;

    Extensions extensions;
    Cache level1InstructionCache;
    Cache level1DataCache;
    Cache level2Cache;
    Cache level3Cache;
    Cache traceCache;
    TLB instructionTlb;
    TLB dataTlb;
  };

// Class
public:
  Processor();

// Methods
public:
  const String& getName() const    { return mName; }
	const float getFrequency() const { return mFrequency; }
	const Info& getInfo() const      { return mInfo; }

  String toString() const;
  
YAKE_BUILD_SINGLETON( Processor )

// Data
private:
  String mName;
  float  mFrequency;
  Info   mInfo;
};

} // base
} // yake

#endif // YAKE_BASE_PROCESSOR_H
