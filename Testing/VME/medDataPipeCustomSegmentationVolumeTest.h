/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDataPipeCustomSegmentationVolumeTest.h,v $
Language:  C++
Date:      $Date: 2011-04-28 14:46:18 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi, Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __CPP_UNIT_medDataPipeCustomSegmentationVolumeTest_H__
#define __CPP_UNIT_medDataPipeCustomSegmentationVolumeTest_H__

class mafVMEVolumeGray;

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class medDataPipeCustomSegmentationVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( medDataPipeCustomSegmentationVolumeTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( DeepCopyTest );
  CPPUNIT_TEST( SetGetVolumeTest );
  CPPUNIT_TEST( SetGetAutomaticSegmentationThresholdModalityTest );
  CPPUNIT_TEST( AddGetRangeTest );
  CPPUNIT_TEST( UpdateRangeTest );
  CPPUNIT_TEST( DeleteRangeTest );
  CPPUNIT_TEST( RemoveAllRangesTest );
  CPPUNIT_TEST( GetNumberOfRangesTest );
  CPPUNIT_TEST( SetGetAutomaticSegmentationGlobalThresholdTest );
  CPPUNIT_TEST( CheckNumberOfThresholdsTest );
  CPPUNIT_TEST( SetGetManualVolumeMaskTest );
  CPPUNIT_TEST( GetAutomaticOutputTest );
  CPPUNIT_TEST( GetManualOutputTest );
  CPPUNIT_TEST( GetRefinementOutputTest );
  CPPUNIT_TEST( GetRegionGrowingOutputTest );
  CPPUNIT_TEST( SetGetRegionGrowingUpperThresholdTest );
  CPPUNIT_TEST( SetGetRegionGrowingLowerThresholdTest );
  CPPUNIT_TEST( SetGetRegionGrowingSliceRangeTest );
  CPPUNIT_TEST( AddGetSeedTest );
  CPPUNIT_TEST( DeleteSeedTest );
  CPPUNIT_TEST( RemoveAllSeedsTest );
  CPPUNIT_TEST( GetNumberOfSeedsTest );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void DeepCopyTest();
  void SetGetVolumeTest();
  void SetGetAutomaticSegmentationThresholdModalityTest();
  void AddGetRangeTest();
  void UpdateRangeTest();
  void DeleteRangeTest();
  void RemoveAllRangesTest();
  void GetNumberOfRangesTest();
  void SetGetAutomaticSegmentationGlobalThresholdTest();
  void CheckNumberOfThresholdsTest();
  void SetGetManualVolumeMaskTest();

  void GetAutomaticOutputTest();
  void GetManualOutputTest();
  void GetRefinementOutputTest();
  void GetRegionGrowingOutputTest();
  void SetGetRegionGrowingUpperThresholdTest();
  void SetGetRegionGrowingLowerThresholdTest();
  void SetGetRegionGrowingSliceRangeTest();
  void AddGetSeedTest();
  void DeleteSeedTest();
  void RemoveAllSeedsTest();
  void GetNumberOfSeedsTest();

  mafVMEVolumeGray *m_VolumeSP;
  mafVMEVolumeGray *m_VolumeRG;
};


int main( int argc, char* argv[] )
{
	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that colllects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	runner.addTest( medDataPipeCustomSegmentationVolumeTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif