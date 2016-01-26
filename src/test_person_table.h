/*

Copyright (C) 2016 Alluris GmbH & Co. KG <weber@alluris.de>

Search database for test persons

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

#include "db_search_table.h"

#ifndef _TEST_PERSON_TABLE_
#define _TEST_PERSON_TABLE_

typedef void(select_cb)(int id);

class test_person_table : public db_search_table
{
private:
  vector<test_person> vtp;

  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0)
  {
    static char s[40];
    switch ( context )
      {
      case CONTEXT_STARTPAGE:                   // before page is drawn..
        fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
        return;
      case CONTEXT_COL_HEADER:                  // Draw column headers

        switch (COL)
          {
          case 0:
            snprintf (s, 40, "%s", gettext ("Name"));
            break;
          case 1:
            snprintf (s, 40, "%s", gettext ("Verantwortlicher"));
            break;
          case 2:
            snprintf (s, 40, "%s", gettext ("Messunsicherheit"));
            break;
          }
        DrawHeader(s,X,Y,W,H);
        return;
      case CONTEXT_ROW_HEADER:                  // Draw row headers
        sprintf(s,"%03d:", vtp[ROW].id);
        DrawHeader(s,X,Y,W,H);
        return;
      case CONTEXT_CELL:                        // Draw data in cells
        switch (COL)
          {
          case 0:
            snprintf (s, 40, "%s", vtp[ROW].name.c_str ());
            break;
          case 1:
            snprintf (s, 40, "%s", vtp[ROW].supervisor.c_str ());
            break;
          case 2:
            snprintf (s, 40, "%.1f%%", vtp[ROW].uncertainty * 100);
            break;
          }
        DrawData(s,X,Y,W,H,row_selected (ROW) ? FL_YELLOW : FL_WHITE);
        return;
      default:
        return;
      }
  }

public:

  test_person_table(int X, int Y, int W, int H, const char *L=0) : db_search_table(X,Y,W,H,L)
  {
    // Rows
    rows(0);             // how many rows
    row_header(1);              // enable row headers (along left)
    row_height_all(20);         // default height of rows
    row_resize(0);              // disable row resizing

    // Cols
    cols(3);              // how many columns
    col_header(1);        // enable column headers (along top)
    col_width (0, 200);   // Name
    col_width (1, 200);   // Verantwortlicher
    col_width (2, 170);   // Messunsicherheit
    col_resize(1);        // enable column resizing

    type (SELECT_SINGLE); // Only single rows can be selected
    Fl_Table::when (FL_WHEN_NEVER);
  }

  void search_name (string str)
  {
    search (NAME, str);
  }

  void search (enum test_person_search_field field, string str)
  {
    vtp.clear ();
    search_test_persons (db, field, subst_wildcards (str), vtp);
    rows (vtp.size());
  }

  void do_select_cb ()
  {
    if (cb)
      {
        for (int k = 0; k < rows (); ++k)
          if (row_selected (k))
            {
              cb(vtp[k].id);
              return;
            }
      }
  }

  int handle(int event)
  {
    int ret = Fl_Table_Row::handle(event);
    if (Fl::event_clicks ())
      {
        do_select_cb ();
        return 1;
      }
    return ret;
  }
};

#endif
