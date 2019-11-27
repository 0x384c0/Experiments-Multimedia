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
  GstCustomVideoFilter *customvideofilter = GST_CUSTOMVIDEOFILTER (filter);
  guint map_size;
  gint width, height;

  width = GST_VIDEO_INFO_WIDTH (in_info);
  height = GST_VIDEO_INFO_HEIGHT (in_info);

  customvideofilter->map_width = width / 4;
  customvideofilter->map_height = height / 4;
  customvideofilter->video_width_margin = width % 4;

  map_size = customvideofilter->map_width * customvideofilter->map_height * sizeof (guint32) * 2;

  g_free (customvideofilter->map);
  customvideofilter->map = (guint32 *) g_malloc0 (map_size);

  return TRUE;
}

static GstFlowReturn
gst_customvideofilter_transform_frame (GstVideoFilter * vfilter, GstVideoFrame * in_frame,
    GstVideoFrame * out_frame)
{
  GstCustomVideoFilter *filter = GST_CUSTOMVIDEOFILTER (vfilter);
  gint x, y, r, g, b;
  guint32 *src, *dest;
  guint32 p, q;
  guint32 v0, v1, v2, v3;
  gint width, map_height, map_width;
  gint video_width_margin;
  guint32 *map;
  GstFlowReturn ret = GST_FLOW_OK;

  map = filter->map;
  map_height = filter->map_height;
  map_width = filter->map_width;
  video_width_margin = filter->video_width_margin;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);

  src += width * 4 + 4;
  dest += width * 4 + 4;

  for (y = 1; y < map_height - 1; y++) {
    for (x = 1; x < map_width - 1; x++) {
      p = *src;
      q = *(src - 4);

      /* difference between the current pixel and left neighbor. */
      r = ((p & 0xff0000) - (q & 0xff0000)) >> 16;
      g = ((p & 0xff00) - (q & 0xff00)) >> 8;
      b = (p & 0xff) - (q & 0xff);
      r *= r;
      g *= g;
      b *= b;
      r = r >> 5;               /* To lack the lower bit for saturated addition,  */
      g = g >> 5;               /* divide the value with 32, instead of 16. It is */
      b = b >> 4;               /* same as `v2 &= 0xfefeff' */
      if (r > 127)
        r = 127;
      if (g > 127)
        g = 127;
      if (b > 255)
        b = 255;
      v2 = (r << 17) | (g << 9) | b;

      /* difference between the current pixel and upper neighbor. */
      q = *(src - width * 4);
      r = ((p & 0xff0000) - (q & 0xff0000)) >> 16;
      g = ((p & 0xff00) - (q & 0xff00)) >> 8;
      b = (p & 0xff) - (q & 0xff);
      r *= r;
      g *= g;
      b *= b;
      r = r >> 5;
      g = g >> 5;
      b = b >> 4;
      if (r > 127)
        r = 127;
      if (g > 127)
        g = 127;
      if (b > 255)
        b = 255;
      v3 = (r << 17) | (g << 9) | b;

      v0 = map[(y - 1) * map_width * 2 + x * 2];
      v1 = map[y * map_width * 2 + (x - 1) * 2 + 1];
      map[y * map_width * 2 + x * 2] = v2;
      map[y * map_width * 2 + x * 2 + 1] = v3;
      r = v0 + v1;
      g = r & 0x01010100;
      dest[0] = r | (g - (g >> 8));
      r = v0 + v3;
      g = r & 0x01010100;
      dest[1] = r | (g - (g >> 8));
      dest[2] = v3;
      dest[3] = v3;
      r = v2 + v1;
      g = r & 0x01010100;
      dest[width] = r | (g - (g >> 8));
      r = v2 + v3;
      g = r & 0x01010100;
      dest[width + 1] = r | (g - (g >> 8));
      dest[width + 2] = v3;
      dest[width + 3] = v3;
      dest[width * 2] = v2;
      dest[width * 2 + 1] = v2;
      dest[width * 2 + 2] = 0;
      dest[width * 2 + 3] = 0;
      dest[width * 3] = v2;
      dest[width * 3 + 1] = v2;
      dest[width * 3 + 2] = 0;
      dest[width * 3 + 3] = 0;

      src += 4;
      dest += 4;
    }
    src += width * 3 + 8 + video_width_margin;
    dest += width * 3 + 8 + video_width_margin;
  }

  return ret;
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