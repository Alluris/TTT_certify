# data file for the Fltk User Interface Designer (fluid)
version 1.0304
header_name {.h}
code_name {.cxx}
decl {\#include "cairo_plot.h"} {public local
}

decl {\#include "cairo_star.h"} {public local
}

Function {} {open
} {
  Fl_Window mainwin {
    label {start test} open selected
    xywh {199 126 960 575} type Double resizable visible
  } {
    Fl_Box plot {
      xywh {60 40 360 270} box UP_BOX align 64
      class cairo_plot
    }
    Fl_Box {} {
      xywh {505 45 360 265} box UP_BOX align 64
      class cairo_star
    }
    Fl_Button {} {
      label {y = 0.5 * x}
      callback {plot->clear_points ();

for (int k=0;k<10;++k)
  plot->add_point(k, 0.5*k);

plot->redraw ();}
      xywh {90 405 195 40}
    }
    Fl_Button {} {
      label {y = cos (x)}
      callback {plot->clear_points ();

for (int k=0;k<100;++k)
  {
    double p = k/10.0;
    plot->add_point(p, cos (p));
  }
plot->redraw ();}
      xywh {90 460 195 40}
    }
  }
}
