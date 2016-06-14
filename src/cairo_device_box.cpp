#include "cairo_device_box.h"

cairo_device_box::cairo_device_box (int x, int y, int w, int h, const char *l)
  : cairo_box (x, y, w, h, l),
    id (-1),
    border (0.1)
{

}

void cairo_device_box::cairo_draw()
{
  if (id >= 0)
    {
      cairo_identity_matrix (cr);
      cairo_translate (cr, x (), y ());
      cairo_set_line_width (cr, 0.002);
      cairo_scale (cr, (1 - 2 * border) * w (), (1 - 2 * border) * w ());
      cairo_translate (cr, border, border);

      cairo_print_id (cr, id);
    }
}
