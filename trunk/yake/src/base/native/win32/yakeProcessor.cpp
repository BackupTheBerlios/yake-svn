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

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <inc/base/yakePCH.h>
#include <inc/base/yakePrerequisites.h>
//#include <inc/base/Flipcode_Processor.h>
#include <inc/base/mpl/yakeCast.h>


//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace native
{
/*
// todo
YAKE_BASE_NATIVE_API void Processor_RetrieveInfo( std::wstring& name, float& frequency, Processor::Info& info )
{
#define CACHE_COPY( Dest, Src ) Dest.isPresent = Src.bPresent; Dest.size = Widen( Src.strSize ); Dest.numAssociativeWays = Src.uiAssociativeWays; Dest.lineSize = Src.uiLineSize; Dest.isSectored = Src.bSectored;
#define TLB_COPY( Dest, Src )   Dest.isPresent = Src.bPresent; Dest.pageSize = Widen( Src.strPageSize ); Dest.numAssociativeWays = Src.uiAssociativeWays; Dest.numEntries = Src.uiEntries;
  static CProcessor processor_s;

  frequency = static_cast< float >( processor_s.GetCPUFrequency( 50 ) ) / 1000000.0f;
  const ProcessorInfo* pinfo = processor_s.GetCPUInfo();
  name = Widen( processor_s.GetCPUName() );


  info.vendor = Widen( pinfo->strVendor );
  info.family = pinfo->family;
  info.familyString = Widen( pinfo->strFamily );
  info.type = pinfo->type;
  info.typeString = Widen( pinfo->strType );
  info.model = Widen( pinfo->strModel );
  info.brandId = Widen( pinfo->strBrandID );
  info.serial = pinfo->strProcessorSerial ? Widen( pinfo->strProcessorSerial ) : L"";
  info.extensions.hasFPU = pinfo->_Ext.FPU_FloatingPointUnit;
  info.extensions.hasVME = pinfo->_Ext.VME_Virtual8086ModeEnhancements;
  info.extensions.hasDE = pinfo->_Ext.DE_DebuggingExtensions;
  info.extensions.hasPSE = pinfo->_Ext.PSE_PageSizeExtensions;
  info.extensions.hasTSC = pinfo->_Ext.TSC_TimeStampCounter;
  info.extensions.hasMSR = pinfo->_Ext.MSR_ModelSpecificRegisters;
  info.extensions.hasPAE = pinfo->_Ext.PAE_PhysicalAddressExtension;
  info.extensions.hasMCE = pinfo->_Ext.MCE_MachineCheckException;
  info.extensions.hasCX8 = pinfo->_Ext.CX8_COMPXCHG8B_Instruction;
  info.extensions.hasAPIC = pinfo->_Ext.APIC_AdvancedProgrammableInterruptController;
  info.extensions.apicID = pinfo->_Ext.APIC_ID;
  info.extensions.hasSEP = pinfo->_Ext.SEP_FastSystemCall;
  info.extensions.hasMTRR = pinfo->_Ext.MTRR_MemoryTypeRangeRegisters;
  info.extensions.hasPGE = pinfo->_Ext.PGE_PTE_GlobalFlag;
  info.extensions.hasMCA = pinfo->_Ext.MCA_MachineCheckArchitecture;
  info.extensions.hasCMOV = pinfo->_Ext.CMOV_ConditionalMoveAndCompareInstructions;
  info.extensions.hasFGPAT = pinfo->_Ext.FGPAT_PageAttributeTable;
  info.extensions.hasPSE36 = pinfo->_Ext.PSE36_36bitPageSizeExtension;
  info.extensions.hasPSN = pinfo->_Ext.PN_ProcessorSerialNumber;
  info.extensions.hasCLFLUSH = pinfo->_Ext.CLFSH_CFLUSH_Instruction;
  info.extensions.clflushInstructionCacheLineSize = pinfo->_Ext.CLFLUSH_InstructionCacheLineSize;
  info.extensions.hasDS = pinfo->_Ext.DS_DebugStore;
  info.extensions.hasACPI = pinfo->_Ext.ACPI_ThermalMonitorAndClockControl;
  info.extensions.hasEMMX = pinfo->_Ext.EMMX_MultimediaExtensions;
  info.extensions.hasMMX = pinfo->_Ext.MMX_MultimediaExtensions;
  info.extensions.hasFXSR = pinfo->_Ext.FXSR_FastStreamingSIMD_ExtensionsSaveRestore;
  info.extensions.hasSSE = pinfo->_Ext.SSE_StreamingSIMD_Extensions;
  info.extensions.hasSSE2 = pinfo->_Ext.SSE2_StreamingSIMD2_Extensions;
  info.extensions.hasSS = pinfo->_Ext.SS_SelfSnoop;
  info.extensions.hasHT = pinfo->_Ext.HT_HyperThreading;
  info.extensions.hyperThreadingSiblings = pinfo->_Ext.HT_HyterThreadingSiblings;
  info.extensions.hasTM = pinfo->_Ext.TM_ThermalMonitor;
  info.extensions.hasIA64 = pinfo->_Ext.IA64_Intel64BitArchitecture;
  info.extensions.has3DNow = pinfo->_Ext._3DNOW_InstructionExtensions;
  info.extensions.hasE3DNow = pinfo->_Ext._E3DNOW_InstructionExtensions;
  info.extensions.hasAA64 = pinfo->_Ext.AA64_AMD64BitArchitecture;
  CACHE_COPY( info.level1InstructionCache, pinfo->_L1.Instruction )
  CACHE_COPY( info.level1DataCache, pinfo->_L1.Data )
  CACHE_COPY( info.level2Cache, pinfo->_L2 )
  CACHE_COPY( info.level3Cache, pinfo->_L3 )
  CACHE_COPY( info.traceCache, pinfo->_Trace )
  TLB_COPY( info.instructionTlb, pinfo->_Instruction )
  TLB_COPY( info.dataTlb, pinfo->_Data )
}*/


} // native
} // base
} // yake