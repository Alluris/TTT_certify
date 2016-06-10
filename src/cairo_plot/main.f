# data file for the Fltk User Interface Designer (fluid)
version 1.0303 
header_name {.h} 
code_name {.cxx}
decl {\#include "cairo_plot.h"} {public local
} 

decl {\#include "cairo_star.h"} {public local
} 

Function {} {open
} {
  Fl_Window mainwin {
    label {start test} open
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
      label fill
      callback {plot->clear_points ();

for (int k=0;k<10;++k)
  plot->add_point(k, 0.2*k);

plot->redraw ();}
      xywh {90 405 155 40}
    }
  }
} 
