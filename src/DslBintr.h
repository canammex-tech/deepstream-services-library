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

#ifndef _DSL_BINTR_H
#define _DSL_BINTR_H

#include "Dsl.h"

#define INIT_MEMORY(m) memset(&m, 0, sizeof(m));
#define LINK_TRUE true
#define LINK_FALSE false

namespace DSL
{

    typedef enum _bintrTypes
    {
        SOURCE_CAMERA_CSI = 0,
        SOURCE_CAMERA_V4L2,
        SOURCE_URI_SINGLE,
        SOURCE_URI_MULTI,
        SINK_OVERLAY,
        SINK_RTSP_STREAMING,
        SINK_FILE,
        SINK_EGL,
        SINK_FAKE,
        STREAM_MUTEX,
        GIE_PRIMARY,
        GIE_SECONDARY,
        TILED_DISPLAY
    } BintrTypes;

    
    /**
     * @class Bintr
     * @brief 
     */
    class Bintr : public std::enable_shared_from_this<Bintr>
    {
    public:

        /**
         * @brief 
         */
        Bintr(const char* name)
            : m_gpuId(0)
            , m_nvbufMemoryType(0)
            , m_pSinkPad(NULL)
            , m_pSourcePad(NULL)
            , m_pParentBintr(NULL)
            , m_pSourceBintr(NULL)
            , m_pDestBintr(NULL)
        { 
            LOG_FUNC(); 
            LOG_INFO("New bintr:: " << name);
            
            m_name = name;

            m_pBin = gst_bin_new((gchar*)name);
            if (!m_pBin)
            {
                LOG_ERROR("Failed to create new bin for component'" << name << "'");
                throw;  
            }
            
            g_mutex_init(&m_bintrMutex);
        };
        
        ~Bintr()
        {
            LOG_FUNC();
            LOG_INFO("Delete bintr:: " << m_name);

            g_mutex_clear(&m_bintrMutex);
        };
        
        void LinkTo(std::shared_ptr<Bintr> pDestBintr)
        { 
            LOG_FUNC();
            LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_bintrMutex);
            
            m_pDestBintr = pDestBintr;

            pDestBintr->m_pSourceBintr = 
                std::dynamic_pointer_cast<Bintr>(shared_from_this());
            
            if (!gst_element_link(m_pBin, pDestBintr->m_pBin))
            {
                LOG_ERROR("Failed to link " << m_name << " to "
                    << pDestBintr->m_name);
                throw;
            }
        };

        void AddChild(std::shared_ptr<Bintr> pChildBintr)
        {
            LOG_FUNC();
            LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_bintrMutex);
            
            pChildBintr->m_pParentBintr = 
                std::dynamic_pointer_cast<Bintr>(shared_from_this());

            m_pChildBintrs.push_back(pChildBintr);
                            
            if (!gst_bin_add(GST_BIN(m_pBin), pChildBintr->m_pBin))
            {
                LOG_ERROR("Failed to add " << pChildBintr->m_name << " to " << m_name);
                throw;
            }
        };
        
        virtual void AddToParent(std::shared_ptr<Bintr> pParentBintr)
        {
            LOG_FUNC();
                
            pParentBintr->AddChild(shared_from_this());
        }

        GstElement* MakeElement(const gchar * factoryname, const gchar * name, bool linkToPrev)
        {
            LOG_FUNC();
            LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_bintrMutex);

            GstElement* pElement = gst_element_factory_make(factoryname, name);
            if (!pElement)
            {
                LOG_ERROR("Failed to create new Element '" << name << "'");
                throw;  
            }

            if (!gst_bin_add(GST_BIN(m_pBin), pElement))
            {
                LOG_ERROR("Failed to add " << name << " to " << m_name);
                throw;
            }
            
            if (linkToPrev)
            {
                // If not the first element
                if (m_pLinkedChildElements.size())
                {
                    // link the previous to the new element 
                    if (!gst_element_link(m_pLinkedChildElements.back(), pElement))
                    {
                        LOG_ERROR("Failed to link new element " << name << " for " << m_name);
                        throw;
                    }
                    LOG_INFO("Successfully linked new element " << name << " for " << m_name);
                }
                m_pLinkedChildElements.push_back(pElement);
            }
            return pElement;
        };
        
        void AddSinkGhostPad()
        {
            LOG_FUNC();
            LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_bintrMutex);
            
            if (!m_pLinkedChildElements.size())
            {
                LOG_ERROR("Failed to add Sink Pad for '" << m_name <<"' No Elements");
                throw;
            }

            // get Sink pad for first child element in the ordered list
            m_pSinkPad = gst_element_get_static_pad(m_pLinkedChildElements.front(), "sink");
            if (!m_pSinkPad)
            {
                LOG_ERROR("Failed to add Sink Pad for '" << m_name <<" '");
                throw;
            }
                    };
            
        void AddSrcGhostPad()
        {
            LOG_FUNC();
            LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_bintrMutex);
            
            if (!m_pLinkedChildElements.size())
            {
                LOG_ERROR("Failed to add Source Pad for '" << m_name <<"' No Elements");
                throw;
            }
            
            // get Source pad for last child element in the ordered list
            m_pSourcePad = gst_element_get_static_pad(m_pLinkedChildElements.back(), "src");
            if (!m_pSourcePad)
            {
                LOG_ERROR("Failed to add Source Pad for '" << m_name <<" '");
                throw;
            }
        };

        void AddGhostPads()
        {
            AddSinkGhostPad();
            AddSrcGhostPad();
        }
            
    public:

        /**
         @brief
         */
        std::string m_name;

        /**
         @brief
         */
        GstElement* m_pBin;
        
        /**
         @brief
         */
        std::vector<GstElement*> m_pLinkedChildElements;
        
        /**
         @brief
         */
        guint m_gpuId;

        /**
         @brief
         */
        guint m_nvbufMemoryType;

        /**
         @brief
         */
        GstPad *m_pSinkPad;
        
        /**
         @brief
         */
        GstPad *m_pSourcePad; 
        
        /**
         @brief
         */
        std::shared_ptr<Bintr> m_pParentBintr;
        
        /**
         @brief
         */
        std::vector<std::shared_ptr<Bintr>> m_pChildBintrs;
        
        /**
         @brief
         */
        std::shared_ptr<Bintr> m_pSourceBintr;

        /**
         @brief
         */
        std::shared_ptr<Bintr> m_pDestBintr;
        
        /**
         * @brief mutex to protect bintr reentry
         */
        GMutex m_bintrMutex;
        
    };


} // DSL

#endif // _DSL_BINTR_H