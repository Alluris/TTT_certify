#ifndef CAIRO_DEVICE_BOX_H
#define CAIRO_DEVICE_BOX_H

#include "cairo_box.h"
#include "cairo_print_devices.h"

using namespace std;

class cairo_device_box : public cairo_box
{
private:
  int id;
  double border;     // as fraction, default 10%

  void cairo_draw ();

public:
  cairo_device_box (int x, int y, int w, int h, const char *l=0);

  void update_id (int id_)
  {
    id = id_;
    redraw ();
  }

};

#endif
