#include "cairo_plot.h"
#include <cmath>

cairo_plot::cairo_plot (int x, int y, int w, int h, const char *l)
  : cairo_box (x, y, w, h, l),
    border (0.1),
    linewidth (3),
    gridlinewidth (1)
{

  //dummy plot
  set_xtick (0, 2, 8);
  set_ytick (0, 1, 6);
  for (double k=0; k<6; k+=0.1)
    add_point (k, 3+sin(k)*2);
}

void cairo_plot::cairo_draw_label (double x, double y, int align, const char *str, double size)
{
  cairo_save (cr);

  double dx = size;
  double dy = size;

  cairo_device_to_user_distance (cr, &dx, &dy);

  printf ("dx=%f, dy=%f align=%i\n", dx, dy, align);
  cairo_set_font_size (cr, dx);

  cairo_text_extents_t extents;
  cairo_text_extents (cr, str, &extents);

  if (align == 0)
    {
      x = x - (extents.width/2 + extents.x_bearing);
      y = y + (extents.height/2 + extents.y_bearing);
    }
  else if (align == 1)  //top
    {
      x = x - (extents.width/2 + extents.x_bearing);
      y = y - extents.height + dy/3;
    }
  else if (align == 2) //right
    {
      x = x - (extents.width + extents.x_bearing) - dx/3;
      y = y + (extents.height/2 + extents.y_bearing);
    }

  cairo_move_to (cr, x, y);
  cairo_scale (cr, 1, -1);
  cairo_show_text (cr, str);
  cairo_restore (cr);
}

void cairo_plot::cairo_draw_grid ()
{
  cairo_save (cr);

  for (vector<double>::iterator it = xtick.begin() ; it != xtick.end(); ++it)
    {
      cairo_move_to(cr, *it, min_ytick);
      cairo_line_to(cr, *it, max_ytick);
    }

  for (vector<double>::iterator it = ytick.begin() ; it != ytick.end(); ++it)
    {
      cairo_move_to(cr, min_xtick, *it);
      cairo_line_to(cr, max_xtick, *it);
    }

  //static const double dashed[] = {4.0, 21.0, 2.0};
  //static int len_dashed  = sizeof(dashed) / sizeof(dashed[0]);
  static const double dash_len = 5;
  cairo_set_dash (cr, &dash_len, 1, 0);
  cairo_identity_matrix (cr);
  cairo_set_line_width (cr, 0.5);
  cairo_stroke (cr);
  cairo_restore (cr);
}

void cairo_plot::cairo_draw_axes ()
{
  cairo_save (cr);

  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  for (vector<double>::iterator it = xtick.begin() ; it != xtick.end(); ++it)
    {
      ostringstream tmp;
      tmp << *it;
      cairo_draw_label (*it, min_ytick, 1, tmp.str().c_str (), 15);
    }

  for (vector<double>::iterator it = ytick.begin() ; it != ytick.end(); ++it)
    {
      ostringstream tmp;
      tmp << *it;
      cairo_draw_label (min_xtick, *it, 2, tmp.str().c_str (), 15);
    }

  cairo_identity_matrix (cr);
  cairo_restore (cr);
}

void cairo_plot::cairo_draw()
{
  printf ("w=%i h=%i ----------------------------\n", w (), h ());
  cairo_identity_matrix (cr);
  // lower left
  cairo_translate(cr, x (), y () + h ());

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.5);
  cairo_scale (cr, w (), -h ());

  print_matrix ();

  // full plot space 0..1 in X and Y
  // here we could plot some legend

  // use 5% and 95% as 0..1
  cairo_translate(cr, border, border);
  cairo_scale (cr, 1 - 2 * border, 1 - 2 * border);

  // now scale to xtick and ytick
  cairo_scale (cr, 1/(max_xtick - min_xtick), 1/(max_ytick - min_ytick));

  cairo_draw_grid ();
  cairo_draw_axes ();

  // draw data
  assert (xdata.size () == ydata.size ());
  vector<double>::iterator xit = xdata.begin();
  vector<double>::iterator yit = ydata.begin();
  cairo_move_to (cr, *xit++, *yit++);

  for (; xit != xdata.end(); xit++, yit++)
    {
      cairo_line_to(cr, *xit, *yit);
    }

  //cairo_move_to(cr, 0.0, 0.0);
  //cairo_line_to(cr, 8, 5);

  // identify CTM so linewidth is in pixels
  cairo_identity_matrix (cr);
  cairo_set_line_width (cr, linewidth);
  cairo_stroke (cr);

}
