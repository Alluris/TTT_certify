/*
 * simple plot using cairo
 */

#ifndef CAIRO_PLOT_H
#define CAIRO_PLOT_H

#include "cairo_box.h"

#include <iostream>
#include <vector>
#include <algorithm>    // std::min_element, std::max_element
#include <sstream>      // ostringstream
#include <cassert>
#include <cmath> //only for testing, FIXME

using namespace std;

class cairo_plot : public cairo_box
{
private:
  double border;     // as fraction, default 10%
  double linewidth;  // in pixels, default 3px
  double gridlinewidth;  // in pixels, default 3px

  vector<double> xtick;
  vector<double> ytick;

  double min_xtick;
  double max_xtick;
  double min_ytick;
  double max_ytick;

  vector<double> xdata;
  vector<double> ydata;

  void cairo_draw_label (double x, double y, int align, const char *str, double size);
  void cairo_draw_grid ();
  void cairo_draw_axes ();
  void cairo_draw ();

public:
  cairo_plot(int x, int y, int w, int h, const char *l=0);

  void add_point (double x, double y)
  {
    xdata.push_back (x);
    ydata.push_back (y);
  }

  void clear_points ()
  {
    xdata.clear ();
    ydata.clear ();
  }

  void set_xtick (double start, double step, double stop)
  {
    for (double k=start; k<stop; k+=step)
      xtick.push_back (k);
    xtick.push_back (stop);

    min_xtick = *min_element (xtick.begin (), xtick.end());
    max_xtick = *max_element (xtick.begin (), xtick.end());
  }

  void set_ytick (double start, double step, double stop)
  {
    for (double k=start; k<stop; k+=step)
      ytick.push_back (k);
    ytick.push_back (stop);

    min_ytick = *min_element (ytick.begin (), ytick.end());
    max_ytick = *max_element (ytick.begin (), ytick.end());
  }

};

#endif
