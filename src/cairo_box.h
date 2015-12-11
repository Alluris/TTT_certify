/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

class cairo_box: Draw with cairo into a Fl_Box.

This file is part of TTT_certify.

TTT_certify is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TTT_certify is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  See ../COPYING
If not, see <http://www.gnu.org/licenses/>.

*/

/*
 This class was inspired from David M. Allens' CairoBox
 https://blog.as.uky.edu/allen/?page_id=677
 Original licence: "Feel free to use this code in any way, but at your own risk."
*/

#ifndef CAIROBOX
#define CAIROBOX

#include <FL/Fl_Box.H>
#include <cairo.h>

#ifdef WIN32
#  include <cairo-win32.h>
#elif defined (__APPLE__)
#  include <cairo-quartz.h>
#else
#  include <cairo-xlib.h>
#endif

#include <iostream>

class cairo_box : public Fl_Box
{
  void draw(void);
  cairo_t*          cr;
  cairo_surface_t*  surface;
  cairo_surface_t*  set_surface(int wo, int ho);
public:
  virtual void graphic(cairo_t* cr, double, double, double, double);
  cairo_box(int x, int y, int w, int h, const char *l=0);
};

#endif

