#ifndef __GST_CUSTOMVIDEOFILTER_H__
#define __GST_CUSTOMVIDEOFILTER_H__

#include <gst/gst.h>

#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

G_BEGIN_DECLS

#define GST_TYPE_CUSTOMVIDEOFILTER \
  (gst_customvideofilter_get_type())
#define GST_CUSTOMVIDEOFILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CUSTOMVIDEOFILTER,GstCustomVideoFilter))
#define GST_CUSTOMVIDEOFILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CUSTOMVIDEOFILTER,GstCustomVideoFilterClass))
#define GST_IS_CUSTOMVIDEOFILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CUSTOMVIDEOFILTER))
#define GST_IS_CUSTOMVIDEOFILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CUSTOMVIDEOFILTER))

typedef struct _GstCustomVideoFilter GstCustomVideoFilter;
typedef struct _GstCustomVideoFilterClass GstCustomVideoFilterClass;

struct _GstCustomVideoFilter
{
  GstVideoFilter videofilter;

  /* < private > */
  gint map_width, map_height;
  guint32 *map;
  gint video_width_margin;
};

struct _GstCustomVideoFilterClass
{
  GstVideoFilterClass parent_class;
};

GType gst_customvideofilter_get_type (void);

G_END_DECLS

#endif /* __GST_CUSTOMVIDEOFILTER_H__ */