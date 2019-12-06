/**
 * SECTION:element-customvideofilter
 * @title: customvideofilter
 *
 * CustomVideoFilter detects edges and display it in good old low resolution
 * computer way.
 *
 * ## Example launch line
 * |[
 * gst-launch-1.0 -v videotestsrc ! customvideofilter ! videoconvert ! autovideosink
 * ]| This pipeline shows the effect of customvideofilter on a test stream.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "gstcustomvideofilter.h"

#define gst_customvideofilter_parent_class parent_class
G_DEFINE_TYPE (GstCustomVideoFilter, gst_customvideofilter, GST_TYPE_VIDEO_FILTER);

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define CAPS_STR GST_VIDEO_CAPS_MAKE ("{  BGRx, RGBx }")
#else
#define CAPS_STR GST_VIDEO_CAPS_MAKE ("{  xBGR, xRGB }")
#endif

static GstStaticPadTemplate gst_customvideofilter_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CAPS_STR)
    );

static GstStaticPadTemplate gst_customvideofilter_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CAPS_STR)
    );

static gboolean
gst_customvideofilter_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info)
{
  return TRUE;
}

static GstFlowReturn
gst_customvideofilter_transform_frame (GstVideoFilter * vfilter, GstVideoFrame * in_frame,
    GstVideoFrame * out_frame)
{
  GstCustomVideoFilter *filter = GST_CUSTOMVIDEOFILTER (vfilter);


  guint32
  *src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0),
  *dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  gint
  width = GST_VIDEO_FRAME_WIDTH (in_frame),
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      
      dest[0] = src[0];
      
      src += 1;
      dest += 1;
    }
  }

  return GST_FLOW_OK;
}

static gboolean
gst_customvideofilter_start (GstBaseTransform * trans)
{
  GstCustomVideoFilter *customvideofilter = GST_CUSTOMVIDEOFILTER (trans);

  if (customvideofilter->map)
    memset (customvideofilter->map, 0,
        customvideofilter->map_width * customvideofilter->map_height * sizeof (guint32) * 2);
  return TRUE;
}

static void
gst_customvideofilter_finalize (GObject * object)
{
  GstCustomVideoFilter *customvideofilter = GST_CUSTOMVIDEOFILTER (object);

  g_free (customvideofilter->map);
  customvideofilter->map = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_customvideofilter_class_init (GstCustomVideoFilterClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstElementClass *gstelement_class = (GstElementClass *) klass;
  GstBaseTransformClass *trans_class = (GstBaseTransformClass *) klass;
  GstVideoFilterClass *vfilter_class = (GstVideoFilterClass *) klass;

  gobject_class->finalize = gst_customvideofilter_finalize;

  gst_element_class_set_static_metadata (gstelement_class, "CustomVideoFilter effect",
      "Filter/Effect/Video",
      "Apply customvideofilter detect on video", "Wim Taymans <wim.taymans@chello.be>");

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_customvideofilter_sink_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_customvideofilter_src_template);

  trans_class->start = GST_DEBUG_FUNCPTR (gst_customvideofilter_start);

  vfilter_class->set_info = GST_DEBUG_FUNCPTR (gst_customvideofilter_set_info);
  vfilter_class->transform_frame =
      GST_DEBUG_FUNCPTR (gst_customvideofilter_transform_frame);
}

static void
gst_customvideofilter_init (GstCustomVideoFilter * customvideofilter)
{
}


static gboolean
customvideofilter_init (GstPlugin * customvideofilter)
{
  return gst_element_register (customvideofilter, "customvideofilter", GST_RANK_NONE,
      GST_TYPE_CUSTOMVIDEOFILTER);
}

/* gstreamer looks for this structure to register plugins
 *
 * exchange the string 'Template customvideofilter' with your customvideofilter description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    customvideofilter,
    "Template customvideofilter",
    customvideofilter_init,
    PACKAGE_VERSION,
    GST_LICENSE,
    GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN
)