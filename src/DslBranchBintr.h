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

#ifndef _DSL_BRANCH_H
#define _DSL_BRANCH_H

#include "Dsl.h"
#include "DslApi.h"
#include "DslGieBintr.h"
#include "DslTrackerBintr.h"
#include "DslOsdBintr.h"
#include "DslTilerBintr.h"
#include "DslPipelineSGiesBintr.h"
#include "DslMultiComponentsBintr.h"
#include "DslSinkBintr.h"
    
namespace DSL 
{
    /**
     * @brief convenience macros for shared pointer abstraction
     */
    #define DSL_BRANCH_PTR std::shared_ptr<BranchBintr>
    #define DSL_BRANCH_NEW(name) \
        std::shared_ptr<BranchBintr>(new BranchBintr(name))

    /**
     * @class BranchBintr
     * @brief 
     */
    class BranchBintr : public Bintr
    {
    public:
    
        /** 
         * 
         */
        BranchBintr(const char* name);

        /**
         * @brief adds a single GIE Bintr to this Branch 
         * @param[in] pGieBintr shared pointer to GIE Bintr to add
         */
        bool AddPrimaryGieBintr(DSL_NODETR_PTR pPrmaryGieBintr);

        /**
         * @brief adds a single Secondary GIE Nodetr to this Branch 
         * @param[in] pSecondaryGieNodetr shared pointer to SGIE Nodetr to add
         */
        bool AddSecondaryGieBintr(DSL_NODETR_PTR pSecondaryGieBintr);

        /**
         * @brief adds a single TrackerBintr to this Branch 
         * @param[in] pTrackerBintr shared pointer to the Tracker Bintr to add
         */
        bool AddTrackerBintr(DSL_NODETR_PTR pTrackerBintr);
        
        /**
         * @brief adds a single TilerBintr to this Branch 
         * @param[in] pDisplayBintr shared pointer to Tiler Bintr to add
         */
        bool AddTilerBintr(DSL_NODETR_PTR pTilerBintr);
        
        /**
         * @brief adds a single OsdBintr to this Branch 
         * @param[in] pOsdBintr shared pointer to OSD Bintr to add
         */
        bool AddOsdBintr(DSL_NODETR_PTR pOsdBintr);
        
        /**
         * @brief adds a single TeeBintr to this Branch 
         * @param[in] pDisplayBintr shared pointer to Tiler Bintr to add
         */
        bool AddTeeBintr(DSL_NODETR_PTR pTeeBintr);
        
        /**
         * @brief adds a single StreamDemuxerBintr to this Branch 
         * @param[in] pDisplayBintr shared pointer to Tiler Bintr to add
         */
        bool AddStreamDemuxerBintr(DSL_NODETR_PTR pStreamDemuxerBintr);

        /**
         * @brief adds a single SinkBintr to this Branch 
         * @param[in] pSinkBintr shared pointer to Sink Bintr to add
         */
        bool AddSinkBintr(DSL_NODETR_PTR pSinkBintr);

        /**
         * @brief check if a SinkBintr is a child of the BranchBintr
         * @param pSinkBintr
         * @return true if SinkBintr is a child, false otherwise
         */
        bool IsSinkBintrChild(DSL_NODETR_PTR pSinkBintr);

        /**
         * @brief removes a single SinkBintr from this Branch 
         * @param[in] pSinkBintr shared pointer to Sink Bintr to add
         */
        bool RemoveSinkBintr(DSL_NODETR_PTR pSinkBintr);

        /**
         * @brief returns the number of Sinks currently in use by
         * this Branch
         */
        uint GetNumSinksInUse()
        {
            if (!m_pMultiSinksBintr)
            {
                return 0;
            }
            return m_pMultiSinksBintr->GetNumChildren();
        } 

        
        bool LinkAll();
        
        void UnlinkAll();
        
        /**
         * @brief Links this BranchBintr, becoming a sink, to a Tee Elementr
         * The Tee can be either a demuxer of tee 
         * @param[in] pTee Nodre to link this Sink Nodre back to
         */
        bool LinkToSource(DSL_NODETR_PTR pTee);

        /**
         * @brief Unlinks this BranchBintr, from a previously linked to source Tee Nodetr
         */
        bool UnlinkFromSource();

    protected:
        
        uint m_batchSize;
        
        uint m_batchTimeout;

        std::vector<DSL_BINTR_PTR> m_linkedComponents;
        
        /**
         * @brief optional, one at most Primary GIE for this Branch
         */
        DSL_PRIMARY_GIE_PTR m_pPrimaryGieBintr;
        
        /**
         * @brief optional, one or more Secondary GIEs for this Branch
         */
        DSL_PIPELINE_SGIES_PTR m_pSecondaryGiesBintr;
        
        /**
         * @brief optional, one at most Tracker for this Branch
         */
        DSL_TRACKER_PTR m_pTrackerBintr;

        /**
         * @brief optional, one at most OSD for this Branch
         */
        DSL_OSD_PTR m_pOsdBintr;
                        
        /**
         * @brief optional/required, one at most Tiled Display mutually exclusive 
         * with the StreamDemuxerBintr, however, a Branch must have one or the other
         */
        DSL_TILER_PTR m_pTilerBintr;
                        
        /**
         * @brief optional/required, one at most StreamStreamDemuxerBintr mutually exclusive 
         * with the TilerBintr, however, a Pipeline must have one or the other.
         */
        DSL_STREAM_DEMUXER_PTR m_pStreamDemuxerBintr;
        
        /**
         * @brief optional, one at most m_pMultiBranchesBintr mutually exclusive 
         * with the StreamDemuxerBintr and MultiSinksBinter, however, a Pipeline must have one of the three
         */
        DSL_TEE_PTR m_pTeeBintr;
        
        /**
         * @brief parent bin for all Sink bins in this Branch
         */
        DSL_MULTI_SINKS_PTR m_pMultiSinksBintr;
        
        
    }; // Branch
    
} // Namespace

#endif // _DSL_BRANCH_H
