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

#ifndef _DSL_REPORTER_BINTR_H
#define _DSL_REPORTER_BINTR_H

#include "Dsl.h"
#include "DslApi.h"
#include "DslElementr.h"
#include "DslBintr.h"
#include "DslDetectionEvent.h"

namespace DSL
{
    /**
     * @brief convenience macros for shared pointer abstraction
     */
    #define DSL_REPORTER_PTR std::shared_ptr<ReporterBintr>
    #define DSL_REPORTER_NEW(name) \
        std::shared_ptr<ReporterBintr>(new ReporterBintr(name))
        
    class ReporterBintr : public Bintr
    {
    public: 
    
        ReporterBintr(const char* name);

        ~ReporterBintr();

        /**
         * @brief Adds the ReporterBintr to a Parent Pipeline Bintr
         * @param[in] pParentBintr Parent Pipeline to add this Bintr to
         */
        bool AddToParent(DSL_BASE_PTR pParentBintr);

        /**
         * @brief Links all Child Elementrs owned by this Bintr
         * @return true if all links were succesful, false otherwise
         */
        bool LinkAll();
        
        /**
         * @brief Unlinks all Child Elemntrs owned by this Bintr
         * Calling UnlinkAll when in an unlinked state has no effect.
         */
        void UnlinkAll();
        
        /**
         * @brief Adds a detection type to this ReporterBintr
         * @param[in] name unique name of the Event to add
         * @param[in] newEvent shared pointer to detection event to add
         * @return true if successful add, false otherwise
         */
        bool AddDetectionEvent(const char* name, DSL_EVENT_DETECTION_PTR newEvent);
        
        /**
         * @brief Removes a uniquely named Event from the RepoterBintr
         * @param[in] name unique name of the Event to remove
         * @return true if successful remove, false otherwise
         */
        bool RemoveDetectionEvent(const char* name);
        
        /**
         * @brief Removes all detection events from this ReporterBintr
         */
        void RemoveAllDetectionEvents();
        
        /**
         * @brief Determines if a uniquely named Event is a child (in-use) by this ReporterBintr
         * @param name unique name of the Event to check for
         * @return true if the event is a child of this ReporterBinter
         */
        bool IsChildEvent(const char* name);
        
    private:

        /**
         * @brief Queue Elementr as both Sink and Source for this ReporterBintr
         */
        DSL_ELEMENT_PTR m_pQueue;
        
        /**
         * @brief map of all Detection Events in use by this ReporterBinter
         */
        std::map <std::string, DSL_EVENT_DETECTION_PTR> m_detectionEvents;

    };
}

#endif // _DSL_REPORTER_BINTR_H