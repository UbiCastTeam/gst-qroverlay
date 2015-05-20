/* GStreamer
 *
 * unit test for qroverlay
 *
 * Copyright (C) <2015> Anthony Violo <anthony.violo@ubicast.eu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <unistd.h>
#include <gst/check/gstcheck.h>

typedef struct _GstQrOverlayTestContext
{
  GstElement *pipeline;
  GstElement *src;
  GstElement *filter;
  GstElement *qroverlay;
  GstElement *convert;
  GstElement *sink;
} GstQrOverlayTestContext;

typedef struct _FormatConversion
{
  const gchar *in_caps;
  const gchar *out_caps;
  gboolean expected_result;
} FormatConversion;

/*
 * Update this table as and when the conversion is supported(or unsupported) in videobox
 */
static const FormatConversion conversion_table[] = {
  {"video/x-raw,format={I420}", "video/x-raw,format={AYUV}", TRUE},
};

static gboolean
bus_handler (GstBus * bus, GstMessage * message, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (message->type) {
    case GST_MESSAGE_EOS:{
      GST_LOG ("EOS event received");
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_ERROR:{
      GError *gerror;
      gchar *debug;
      gst_message_parse_error (message, &gerror, &debug);
      g_error ("Error from %s: %s (%s)\n",
          GST_ELEMENT_NAME (GST_MESSAGE_SRC (message)), gerror->message,
          GST_STR_NULL (debug));
      g_error_free (gerror);
      g_free (debug);
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_WARNING:{
      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}


static void
qroverlay_test_init_context (GstQrOverlayTestContext * ctx)
{
  fail_unless (ctx != NULL);

  ctx->pipeline = gst_pipeline_new ("pipeline");
  fail_unless (ctx->pipeline != NULL);
  ctx->src = gst_element_factory_make ("videotestsrc", "src");
  fail_unless (ctx->src != NULL, "Failed to create videotestsrc element");
  ctx->filter = gst_element_factory_make ("capsfilter", "filter");
  fail_unless (ctx->filter != NULL, "Failed to create capsfilter element");
  ctx->qroverlay = gst_element_factory_make ("qroverlay", "qr");
  fail_unless (ctx->qroverlay != NULL, "Failed to create qroverlay element");
  ctx->convert = gst_element_factory_make ("videoconvert", "convert");
  fail_unless (ctx->convert != NULL, "Failed to create videoconvert element");
  ctx->sink = gst_element_factory_make ("ximagesink", "sink");
  fail_unless (ctx->sink != NULL, "Failed to create ximagesink element");

  gst_bin_add_many (GST_BIN (ctx->pipeline), ctx->src, ctx->filter,
      ctx->qroverlay, ctx->convert, ctx->sink, NULL);
  fail_unless (gst_element_link_many (ctx->src, ctx->filter, ctx->qroverlay, ctx->convert,
          ctx->sink, NULL) == TRUE, "Can not link elements");

  fail_unless (gst_element_set_state (ctx->pipeline,
          GST_STATE_READY) != GST_STATE_CHANGE_FAILURE,
      "couldn't set pipeline to READY state");

  GST_LOG ("qroverlay context inited");
}

static void
qroverlay_test_deinit_context (GstQrOverlayTestContext * ctx)
{
  GST_LOG ("deiniting qroverlay context");

  gst_element_set_state (ctx->pipeline, GST_STATE_NULL);
  gst_object_unref (ctx->pipeline);
  memset (ctx, 0x00, sizeof (GstQrOverlayTestContext));
}

GST_START_TEST (test_caps_transform)
{
  GstStateChangeReturn state_ret;
  GstQrOverlayTestContext ctx;
  guint conversions_test_size;
  guint itr;
  gboolean link_res;
  GMainLoop *loop;
  GstBus *bus;

  qroverlay_test_init_context (&ctx);
  gst_util_set_object_arg (G_OBJECT (ctx.src), "num-buffers", "1");
  gst_util_set_object_arg (G_OBJECT (ctx.src), "pattern", "2");
  gst_util_set_object_arg (G_OBJECT (ctx.filter), "caps",
  	"video/x-raw, format=(string)I420, width=(int)320, height=(int)240, framerate=(fraction)60000/1001");
  loop = g_main_loop_new (NULL, TRUE);
  fail_unless (loop != NULL);

  bus = gst_element_get_bus (ctx.pipeline);
  fail_unless (bus != NULL);

  gst_bus_add_watch (bus, bus_handler, loop);
  gst_object_unref (bus);

  conversions_test_size = G_N_ELEMENTS (conversion_table);
  for (itr = 0; itr < conversions_test_size; itr++) {
    gst_element_unlink_many (ctx.src, ctx.filter, ctx.qroverlay, ctx.convert,
        ctx.sink, NULL);
    gst_util_set_object_arg (G_OBJECT (ctx.filter), "caps",
        conversion_table[itr].in_caps);

    /* Link with new input and output format from conversion table */
    link_res =
        gst_element_link_many (ctx.src, ctx.filter, ctx.qroverlay, ctx.convert,
        ctx.sink, NULL);

    /* Check if the specified format conversion is supported or not by qroverlay */
    fail_unless (link_res == conversion_table[itr].expected_result,
        "qroverlay can not convert from '%s'' to '%s'",
        conversion_table[itr].in_caps, conversion_table[itr].out_caps);

    if (link_res == FALSE) {
      GST_LOG ("elements linking failed");
      continue;
    }

    state_ret = gst_element_set_state (ctx.pipeline, GST_STATE_PLAYING);
    fail_unless (state_ret != GST_STATE_CHANGE_FAILURE,
        "couldn't set pipeline to PLAYING state");

    g_main_loop_run (loop);

    state_ret = gst_element_set_state (ctx.pipeline, GST_STATE_READY);
    fail_unless (state_ret != GST_STATE_CHANGE_FAILURE,
        "couldn't set pipeline to READY state");
  }

  g_main_loop_unref (loop);

  qroverlay_test_deinit_context (&ctx);
}

GST_END_TEST;

static Suite *
qroverlay_suite (void)
{
  Suite *s = suite_create ("qroverlay");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_caps_transform);

  return s;
}

int
main (int argc, char **argv)
{
  int nf;

  Suite *s = qroverlay_suite ();
  SRunner *sr = srunner_create (s);

  gst_check_init (&argc, &argv);

  srunner_run_all (sr, CK_NORMAL);
  nf = srunner_ntests_failed (sr);
  srunner_free (sr);

  return nf;
}