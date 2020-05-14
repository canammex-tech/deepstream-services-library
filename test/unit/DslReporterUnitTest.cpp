/*
The MIT License

Copyright (c) 2019-Present, ROBERT HOWELL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in-
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "catch.hpp"
#include "DslReporterBintr.h"

using namespace DSL;

SCENARIO( "A new ReporterBintr is created correctly", "[ReporterBintr]" )
{
    GIVEN( "Attributes for a new ReporterBintr" ) 
    {
        std::string reporterName = "reporter";

        WHEN( "A new Reporter is created" )
        {
            DSL_REPORTER_PTR pReporterBintr = DSL_REPORTER_NEW(reporterName.c_str());

            THEN( "The ReporterBintr's memebers are setup and returned correctly" )
            {
                REQUIRE( pReporterBintr->GetGstObject() != NULL );
            }
        }
    }
}

SCENARIO( "A new ReporterBintr can LinkAll Child Elementrs", "[ReporterBintr]" )
{
    GIVEN( "A new ReporterBintr in an Unlinked state" ) 
    {
        std::string reporterName = "reporter";

        DSL_REPORTER_PTR pReporterBintr = DSL_REPORTER_NEW(reporterName.c_str());

        WHEN( "A new ReporterBintr is Linked" )
        {
            REQUIRE( pReporterBintr->LinkAll() == true );

            THEN( "The ReporterBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pReporterBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A Linked ReporterBintr can UnlinkAll Child Elementrs", "[ReporterBintr]" )
{
    GIVEN( "A ReporterBintr in a linked state" ) 
    {
        std::string reporterName = "reporter";

        DSL_REPORTER_PTR pReporterBintr = DSL_REPORTER_NEW(reporterName.c_str());

        REQUIRE( pReporterBintr->LinkAll() == true );

        WHEN( "A ReporterBintr is Linked" )
        {
            pReporterBintr->UnlinkAll();

            THEN( "The ReporterBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pReporterBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A ReporterBintr can add and remove a DetectionEvent", "[ReporterBintr]" )
{
    GIVEN( "A new ReporterBintr and DetectionEvent" ) 
    {
        std::string reporterName = "reporter";
        std::string eventName = "first-occurence";
        uint classId(1);

        DSL_REPORTER_PTR pReporterBintr = DSL_REPORTER_NEW(reporterName.c_str());

        DSL_EVENT_FIRST_OCCURRENCE_PTR pFirstOccurrenceEvent = 
            DSL_EVENT_FIRST_OCCURRENCE_NEW(eventName.c_str(), classId);

        WHEN( "A the Event is added to the ReportBintr" )
        {
            REQUIRE( pReporterBintr->AddDetectionEvent(eventName.c_str(), pFirstOccurrenceEvent) == true );
            
            // ensure that the Event can not be added twice
            REQUIRE( pReporterBintr->AddDetectionEvent(eventName.c_str(), pFirstOccurrenceEvent) == false );
            
            THEN( "The Event can be found and removed" )
            {
                REQUIRE( pReporterBintr->IsChildEvent(eventName.c_str()) == true );
                REQUIRE( pFirstOccurrenceEvent->IsParent(pReporterBintr) == true );
                REQUIRE( pFirstOccurrenceEvent->IsInUse() == true );
                
                REQUIRE( pReporterBintr->RemoveDetectionEvent(eventName.c_str()) == true );
                
                REQUIRE( pFirstOccurrenceEvent->IsParent(pReporterBintr) == false );
                REQUIRE( pReporterBintr->IsChildEvent(eventName.c_str()) == false );
                REQUIRE( pFirstOccurrenceEvent->IsInUse() == false );

                // ensure removal fails on second call, 
                REQUIRE( pReporterBintr->RemoveDetectionEvent(eventName.c_str()) == false );
            }
        }
    }
}
