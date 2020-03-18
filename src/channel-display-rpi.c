
/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
   Copyright (C) 2015-2016 CodeWeavers, Inc

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/
#include "config.h"
#include <stdio.h>
#include <libyuv.h>

#include "spice-client.h"
#include "spice-common.h"
#include "spice-channel-priv.h"
#include "common/recorder.h"

#include "channel-display-priv.h"


#include 


typedef struct openmax_context {



}openmax_context_t



typedef struct SpiceGstFrame SpiceGstFrame;

typedef struct SpiceGstDecoder {
    VideoDecoder base;

    /* ---------- GStreamer pipeline ---------- */
	int                 width;
    int                 height;
    uint32_t l          last_mm_time;
    guint               timer_id;

    pthread_t           thread;
    int                 thread_stats;

    GAsyncQueue     *   asyncDecoderQueue; 
    GAsyncQueue     *   asyncDisplayQueue;
} SpiceGstDecoder;

#define VALID_VIDEO_CODEC_TYPE(codec) \
    (codec > 0 && codec < G_N_ELEMENTS(gst_opts))


typedef enum {
  GST_PLAY_FLAG_VIDEO             = (1 << 0),
  GST_PLAY_FLAG_AUDIO             = (1 << 1),
  GST_PLAY_FLAG_TEXT              = (1 << 2),
  GST_PLAY_FLAG_VIS               = (1 << 3),
  GST_PLAY_FLAG_SOFT_VOLUME       = (1 << 4),
  GST_PLAY_FLAG_NATIVE_AUDIO      = (1 << 5),
  GST_PLAY_FLAG_NATIVE_VIDEO      = (1 << 6),
  GST_PLAY_FLAG_DOWNLOAD          = (1 << 7),
  GST_PLAY_FLAG_BUFFERING         = (1 << 8),
  GST_PLAY_FLAG_DEINTERLACE       = (1 << 9),
  GST_PLAY_FLAG_SOFT_COLORBALANCE = (1 << 10),
  GST_PLAY_FLAG_FORCE_FILTERS     = (1 << 11),
} SpiceGstPlayFlags;

/* ---------- SpiceGstFrame ---------- */

struct SpiceGstFrame {
    SpiceFrame *    sframe; 
	uint8_t *       data[4];
	int             data_linesize[4];
	int             width;
    int             height;
};

typedef struct decoderBuffer {
    int             status;
    void   *        data;
    int             size;
}decoderBuffer_t;






static void * openmax_decoder_thread_hanlder(void * arg);

static void * openmax_decoder_thread_hanlder(void * arg)
{
   OMX_VIDEO_PARAM_PORTFORMATTYPE format;
   OMX_TIME_CONFIG_CLOCKSTATETYPE cstate;
   COMPONENT_T *video_decode = NULL, *video_scheduler = NULL, *video_render = NULL, *clock = NULL;
   COMPONENT_T *list[5];
   TUNNEL_T tunnel[4];
   ILCLIENT_T *client;
   FILE *in;
   int status = 0;
   unsigned int data_len = 0;

   memset(list, 0, sizeof(list));
   memset(tunnel, 0, sizeof(tunnel));

   if((in = fopen(filename, "rb")) == NULL)
      return -2;

   if((client = ilclient_init()) == NULL)
   {
      fclose(in);
      return -3;
   }

   if(OMX_Init() != OMX_ErrorNone)
   {
      ilclient_destroy(client);
      fclose(in);
      return -4;
   }

   // create video_decode
   if(ilclient_create_component(client, &video_decode, "video_decode", ILCLIENT_DISABLE_ALL_PORTS | ILCLIENT_ENABLE_INPUT_BUFFERS) != 0)
      status = -14;
   list[0] = video_decode;

   // create video_render
   if(status == 0 && ilclient_create_component(client, &video_render, "video_render", ILCLIENT_DISABLE_ALL_PORTS) != 0)
      status = -14;
   list[1] = video_render;

   // create clock
   if(status == 0 && ilclient_create_component(client, &clock, "clock", ILCLIENT_DISABLE_ALL_PORTS) != 0)
      status = -14;
   list[2] = clock;

   memset(&cstate, 0, sizeof(cstate));
   cstate.nSize = sizeof(cstate);
   cstate.nVersion.nVersion = OMX_VERSION;
   cstate.eState = OMX_TIME_ClockStateWaitingForStartTime;
   cstate.nWaitMask = 1;
   if(clock != NULL && OMX_SetParameter(ILC_GET_HANDLE(clock), OMX_IndexConfigTimeClockState, &cstate) != OMX_ErrorNone)
      status = -13;

   // create video_scheduler
   if(status == 0 && ilclient_create_component(client, &video_scheduler, "video_scheduler", ILCLIENT_DISABLE_ALL_PORTS) != 0)
      status = -14;
   list[3] = video_scheduler;

   set_tunnel(tunnel, video_decode, 131, video_scheduler, 10);
   set_tunnel(tunnel+1, video_scheduler, 11, video_render, 90);
   set_tunnel(tunnel+2, clock, 80, video_scheduler, 12);

   // setup clock tunnel first
   if(status == 0 && ilclient_setup_tunnel(tunnel+2, 0, 0) != 0)
      status = -15;
   else
      ilclient_change_component_state(clock, OMX_StateExecuting);

   if(status == 0)
      ilclient_change_component_state(video_decode, OMX_StateIdle);

   memset(&format, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
   format.nSize = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
   format.nVersion.nVersion = OMX_VERSION;
   format.nPortIndex = 130;
   format.eCompressionFormat = OMX_VIDEO_CodingAVC;

   if(status == 0 &&
      OMX_SetParameter(ILC_GET_HANDLE(video_decode), OMX_IndexParamVideoPortFormat, &format) == OMX_ErrorNone &&
      ilclient_enable_port_buffers(video_decode, 130, NULL, NULL, NULL) == 0)
   {
      OMX_BUFFERHEADERTYPE *buf;
      int port_settings_changed = 0;
      int first_packet = 1;

      ilclient_change_component_state(video_decode, OMX_StateExecuting);

      while((buf = ilclient_get_input_buffer(video_decode, 130, 1)) != NULL)
      {
         // feed data and wait until we get port settings changed
         unsigned char *dest = buf->pBuffer;

         data_len += fread(dest, 1, buf->nAllocLen-data_len, in);

         if(port_settings_changed == 0 &&
            ((data_len > 0 && ilclient_remove_event(video_decode, OMX_EventPortSettingsChanged, 131, 0, 0, 1) == 0) ||
             (data_len == 0 && ilclient_wait_for_event(video_decode, OMX_EventPortSettingsChanged, 131, 0, 0, 1,
                                                       ILCLIENT_EVENT_ERROR | ILCLIENT_PARAMETER_CHANGED, 10000) == 0)))
         {
            port_settings_changed = 1;

            if(ilclient_setup_tunnel(tunnel, 0, 0) != 0)
            {
               status = -7;
               break;
            }

            ilclient_change_component_state(video_scheduler, OMX_StateExecuting);

            // now setup tunnel to video_render
            if(ilclient_setup_tunnel(tunnel+1, 0, 1000) != 0)
            {
               status = -12;
               break;
            }

            ilclient_change_component_state(video_render, OMX_StateExecuting);
         }
         if(!data_len)
            break;

         buf->nFilledLen = data_len;
         data_len = 0;

         buf->nOffset = 0;
         if(first_packet)
         {
            buf->nFlags = OMX_BUFFERFLAG_STARTTIME;
            first_packet = 0;
         }
         else
            buf->nFlags = OMX_BUFFERFLAG_TIME_UNKNOWN;

         if(OMX_EmptyThisBuffer(ILC_GET_HANDLE(video_decode), buf) != OMX_ErrorNone)
         {
            status = -6;
            break;
         }
      }

      buf->nFilledLen = 0;
      buf->nFlags = OMX_BUFFERFLAG_TIME_UNKNOWN | OMX_BUFFERFLAG_EOS;

      if(OMX_EmptyThisBuffer(ILC_GET_HANDLE(video_decode), buf) != OMX_ErrorNone)
         status = -20;

      // wait for EOS from render
      ilclient_wait_for_event(video_render, OMX_EventBufferFlag, 90, 0, OMX_BUFFERFLAG_EOS, 0,
                              ILCLIENT_BUFFER_FLAG_EOS, -1);

      // need to flush the renderer to allow video_decode to disable its input port
      ilclient_flush_tunnels(tunnel, 0);

   }

   fclose(in);

   ilclient_disable_tunnel(tunnel);
   ilclient_disable_tunnel(tunnel+1);
   ilclient_disable_tunnel(tunnel+2);
   ilclient_disable_port_buffers(video_decode, 130, NULL, NULL, NULL);
   ilclient_teardown_tunnels(tunnel);

   ilclient_state_transition(list, OMX_StateIdle);
   ilclient_state_transition(list, OMX_StateLoaded);

   ilclient_cleanup_components(list);

   OMX_Deinit();

   ilclient_destroy(client);

   return NULL;
}






/* ---------- GStreamer pipeline ---------- */

static void openmax_schedule_frame(SpiceGstDecoder *decoder);

/* main context */
static gboolean openmax_display_frame(gpointer video_decoder)
{
    SpiceGstDecoder *decoder = (SpiceGstDecoder*)video_decoder;
    SpiceGstFrame *gstframe;
   
    gstframe = g_queue_pop_head(decoder->displayQueue);
    if(gstframe == NULL) {
        return;
    }

    stream_display_frame(decoder->base.stream, gstframe->encoded_frame,
                         gstframe->width, gstframe->height,
						 gstframe->data_linesize[0], gstframe->data[0]);

    decoder->displayQueueSize--;
    free_gst_frame(gstframe);
    decoder->timer_id = 0;
    schedule_frame(decoder);
    return G_SOURCE_REMOVE;
}

/* main loop or GStreamer streaming thread */
static void openmax_schedule_frame(SpiceGstDecoder *decoder)
{   
    while (!decoder->timer_id) {
        guint32 now = stream_get_time(decoder->base.stream);
        SpiceGstFrame *gstframe = g_queue_peek_head(decoder->displayQueue);
        if (!gstframe) {
            return;
        }		
        int32_t tmp = gstframe->encoded_frame->mm_time - now;
        if ( tmp > 0) {
			//spice_warning("spice_mmtime_diff now %d mm_time %d", now, gstframe->encoded_frame->mm_time);
            decoder->timer_id = g_timeout_add(tmp,display_frame, decoder);
        }else if(tmp <= 0 && tmp > -100 ) {
            decoder->timer_id = g_timeout_add(0,display_frame, decoder);
        } else {
            spice_warning("%s: rendering too late by %u ms (ts: %u, mmtime: %u), dropping",
                        __FUNCTION__, now - gstframe->encoded_frame->mm_time,
                        gstframe->encoded_frame->mm_time, now);
            stream_dropped_frame_on_playback(decoder->base.stream);	
            g_queue_pop_head(decoder->displayQueue);				
            free_gst_frame(gstframe);
        }
    }
}

/* ---------- VideoDecoder's public API ---------- */
static void spice_openmax_decoder_reschedule(VideoDecoder *video_decoder)
{
    SpiceGstDecoder *decoder = (SpiceGstDecoder*)video_decoder;

    guint timer_id;
    timer_id = decoder->timer_id;
    decoder->timer_id = 0;
 
    if (timer_id != 0) {
        g_source_remove(timer_id);
    }
    schedule_frame(decoder);
}

/* main context */
static void spice_openmax_decoder_destroy(VideoDecoder *video_decoder)
{
    spice_warning("spice_gst_decoder_destroy close stream");
    SpiceGstDecoder *decoder = (SpiceGstDecoder*)video_decoder;

    if (decoder->timer_id) {
        g_source_remove(decoder->timer_id);
    }
    
    if(decoder->asyncDecoderQueue) {
        g_async_queue_unref(decoder->asyncDecoderQueue);
        decoder->asyncDecoderQueue = NULL;
    }

    if(decoder->asyncDisplayQueue) {
        g_async_queue_unref(decoder->asyncDisplayQueue);
        decoder->asyncDisplayQueue = NULL;
    }


    g_free(decoder);
}


static gboolean spice_openmax_decoder_queue_frame(VideoDecoder *video_decoder,
                                              SpiceFrame *frame, int latency)
{

    SpiceGstDecoder *decoder = (SpiceGstDecoder*)video_decoder;

    if (frame->size == 0) {
        SPICE_DEBUG("got an empty frame buffer!");
        spice_frame_free(frame);
        return TRUE;
    }

    if (spice_mmtime_diff(frame->mm_time, decoder->last_mm_time) < 0) {
        SPICE_DEBUG("new-frame-time < last-frame-time (%u < %u):"
                    " resetting stream",
                    frame->mm_time, decoder->last_mm_time);
        /* Let GStreamer deal with the frame anyway */
    }

    decoder->last_mm_time = frame->mm_time;

    if (latency < 0 &&
        decoder->base.codec_type == SPICE_VIDEO_CODEC_TYPE_MJPEG) {
        /* Dropping MJPEG frames has no impact on those that follow and
         * saves CPU so do it.
         */
        SPICE_DEBUG("dropping a late MJPEG frame");
        spice_frame_free(frame);
        return TRUE;
    }

    if (decoder->avdec_ctx == NULL) {
        /* An error occurred, causing the GStreamer pipeline to be freed */
        spice_warning("An error occurred, stopping the video stream");
        spice_frame_free(frame);
        return FALSE;
    }
        

    

    return TRUE;
}

G_GNUC_INTERNAL
VideoDecoder* create_openmax_decoder(int codec_type, display_stream *stream)
{
    SpiceGstDecoder *decoder = NULL;
  
    g_return_val_if_fail(VALID_VIDEO_CODEC_TYPE(codec_type), NULL);

    decoder = g_new0(SpiceGstDecoder, 1);
    
    decoder->base.destroy       = spice_openmax_decoder_destroy;
    decoder->base.reschedule    = spice_openmax_decoder_reschedule;
    decoder->base.queue_frame   = spice_openmax_decoder_queue_frame;
    decoder->base.codec_type    = codec_type;
    decoder->base.stream        = stream;

    decoder->asyncDecoderQueue  = NULL;
    decoder->asyncDisplayQueue  = NULL;

    decoder->thread_stats       = 1;

    decoder->asyncDecoderQueue  =  g_async_queue_new();
    if(!decoder->asyncDecoderQueue) {
        goto FAILED;
    }

    decoder->asyncDisplayQueue  =  g_async_queue_new();
    if(!decoder->asyncDisplayQueue) {
        goto FAILED;
    }
    
    int ret = pthread_create(&decoder->thread, NULL, openmax_decoder_thread_hanlder,(void *)decoder);
    if(ret < 0) {
        goto FAILED;
    }

    pthread_detach(decoder->thread);

    return (VideoDecoder*)decoder;

FAILED:
    decoder->thread_stats       = 0;

    if(decoder->asyncDecoderQueue) {
        g_async_queue_unref(decoder->asyncDecoderQueue);
        decoder->asyncDecoderQueue = NULL;
    }

    if(decoder->asyncDisplayQueue) {
        g_async_queue_unref(decoder->asyncDisplayQueue);
        decoder->asyncDisplayQueue = NULL;
    }

    spice_warning(" <- create_openmax_decoder failed ->");

    g_free(decoder);

    return NULL;
}



G_GNUC_INTERNAL
gboolean openmax_video_has_codec(int codec_type)
{

    return TRUE;
}



