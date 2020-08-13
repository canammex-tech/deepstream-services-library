################################################################################
# The MIT License
#
# Copyright (c) 2019-2020, Robert Howell. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################

#!/usr/bin/env python

import sys
sys.path.insert(0, "../../")
import time

from dsl import *

# Filespecs for the Primary GIE
inferConfigFile = '../../test/configs/config_infer_primary_nano.txt'
modelEngineFile = '../../test/models/Primary_Detector_Nano/resnet10.caffemodel_b8_gpu0_fp16.engine'
tracker_config_file = '../../test/configs/iou_config.txt'

# Best to keep Tiler dimensions the same as Streamux/Source
TILER_WIDTH = DSL_DEFAULT_STREAMMUX_WIDTH
TILER_HEIGHT = DSL_DEFAULT_STREAMMUX_HEIGHT

WINDOW_WIDTH = TILER_WIDTH
WINDOW_HEIGHT = TILER_HEIGHT

## 
# Function to be called on XWindow KeyRelease event
## 
def xwindow_key_event_handler(key_string, client_data):
    print('key released = ', key_string)
    if key_string.upper() == 'P':
        dsl_pipeline_pause('pipeline')
    elif key_string.upper() == 'R':
        dsl_pipeline_play('pipeline')
    elif key_string.upper() == 'Q' or key_string == '':
        dsl_main_loop_quit()
 
## 
# Function to be called on XWindow Delete event
## 
def xwindow_delete_event_handler(client_data):
    print('delete window event')
    dsl_main_loop_quit()

## 
# Function to be called on End-of-Stream (EOS) event
## 
def eos_event_listener(client_data):
    print('Pipeline EOS event')
    dsl_main_loop_quit()

## 
# Function to be called on every change of Pipeline state
## 
def state_change_listener(old_state, new_state, client_data):
    print('previous state = ', old_state, ', new state = ', new_state)
    if new_state == DSL_STATE_PLAYING:
        dsl_pipeline_dump_to_dot('pipeline', "state-playing")
        
## 
# Meter Sink client callback funtion
## 
def meter_sink_handler(session_avgs, interval_avgs, source_count, client_data):
    print('Meter Sink Callback called')
    if new_state == DSL_STATE_PLAYING:
        dsl_pipeline_dump_to_dot('pipeline', "state-playing")
        

def main(args):

    # Since we're not using args, we can Let DSL initialize GST on first call
    while True:
    
    
        retval = dsl_display_type_rgba_color_new('full-white', red=1.0, green=1.0, blue=1.0, alpha = 1.0)
        if retval != DSL_RETURN_SUCCESS:
            return retval

        retval = dsl_display_type_rgba_color_new('opaque-black', red=0.0, green=0.0, blue=0.0, alpha = 0.0)
        if retval != DSL_RETURN_SUCCESS:
            return retval

        retval = dsl_display_type_rgba_font_new('arial-14-white', font='arial', size=14, color='full-white')
        if retval != DSL_RETURN_SUCCESS:
            return retval
            
        retval = dsl_display_type_source_name_new('source-name', 
            x_offset=15, y_offset=20, font='arial-14-white', has_bg_color=False, bg_color='full-white')
        if retval != DSL_RETURN_SUCCESS:
            return retval
            
        retval = dsl_display_type_source_dimensions_new('dimensions', 
            x_offset=15, y_offset=50, font='arial-14-white', has_bg_color=False, bg_color='full-white')
        if retval != DSL_RETURN_SUCCESS:
            return retval
            
        # Create a new Action to display the Source Dimensions
        retval = dsl_ode_action_display_meta_add_new('add-source-name', 'source-name')
        if retval != DSL_RETURN_SUCCESS:
            return retval
            
        # Create a new Action to display the Source Dimensions
        retval = dsl_ode_action_display_meta_add_new('add-dimensions', 'dimensions')
        if retval != DSL_RETURN_SUCCESS:
            return retval
        
        # For debug
        retval = dsl_ode_action_print_new('print-to-console')
        if retval != DSL_RETURN_SUCCESS:
            break

        # Create an Always triger to overlay our Display Types on every frame
        retval = dsl_ode_trigger_always_new('always-trigger', when=DSL_ODE_PRE_OCCURRENCE_CHECK)
        if retval != DSL_RETURN_SUCCESS:
            return retval

        retval = dsl_ode_trigger_action_add_many('always-trigger', actions=[
            'add-dimensions',
            'add-source-name',  
            'print-to-console',
            None])
            
        # New ODE Handler to handle all ODE Triggers with their Areas and Actions    
        retval = dsl_pph_ode_new('ode-handler')
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pph_ode_trigger_add('ode-handler', trigger='always-trigger')
        if retval != DSL_RETURN_SUCCESS:
            break
            

        ############################################################################################
        #
        # Create the remaining Pipeline components
        # Four New URI File Sources
        retval = dsl_source_uri_new('North Camera', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 0)
        if retval != DSL_RETURN_SUCCESS:
            break
        dsl_source_uri_new('South Camera', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 0)
        dsl_source_uri_new('East Camera', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 0)
        dsl_source_uri_new('West Camera', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 0)

        # New Primary GIE using the filespecs above, with interval and Id
        retval = dsl_gie_primary_new('primary-gie', inferConfigFile, modelEngineFile, 4)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New KTL Tracker, setting max width and height of input frame
        retval = dsl_tracker_ktl_new('ktl-tracker', 480, 288)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Tiler, setting width and height, use default cols/rows set by source count
        retval = dsl_tiler_new('tiler', TILER_WIDTH, TILER_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add the ODE Pad Probe Handler to the Sink pad of the Tiler
        retval = dsl_tiler_pph_add('tiler', 'ode-handler', DSL_PAD_SINK)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New OSD with clock enabled... using default values.
        retval = dsl_osd_new('on-screen-display', False)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Window Sink, 0 x/y offsets and same dimensions as Tiled Display
        retval = dsl_sink_window_new('window-sink', 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add all the components to our pipeline
        retval = dsl_pipeline_new_component_add_many('pipeline', 
            ['North Camera', 'South Camera', 'East Camera', 'West Camera', 
            'primary-gie', 'ktl-tracker', 'tiler', 'on-screen-display', 'window-sink', None])
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add the XWindow event handler functions defined above
        retval = dsl_pipeline_xwindow_key_event_handler_add("pipeline", xwindow_key_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_xwindow_delete_event_handler_add("pipeline", xwindow_delete_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        ## Add the listener callback functions defined above
        retval = dsl_pipeline_state_change_listener_add('pipeline', state_change_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_eos_listener_add('pipeline', eos_event_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Play the pipeline
        retval = dsl_pipeline_play('pipeline')
        if retval != DSL_RETURN_SUCCESS:
            break

        dsl_main_loop_run()
        break

    # Print out the final result
    print(dsl_return_value_to_string(retval))

    dsl_pipeline_delete_all()
    dsl_component_delete_all()

if __name__ == '__main__':
    sys.exit(main(sys.argv))
