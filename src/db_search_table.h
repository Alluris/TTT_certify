/*

Copyright (C) 2016 Alluris GmbH & Co. KG <weber@alluris.de>

Database search table

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

#include <FL/Fl.H>
#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include "sqlite_interface.h"

typedef void(select_cb)(int id);

#ifndef _DB_SEARCH_TABLE_
#define _DB_SEARCH_TABLE_

class db_search_table : public Fl_Table_Row
{
protected:

  sqlite3 *db;
  select_cb* cb;

  void DrawHeader(const char *s, int X, int Y, int W, int H)
  {
    fl_push_clip(X,Y,W,H);
    fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
    fl_color(FL_BLACK);
    fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    fl_pop_clip();
  }

  void DrawData(const char *s, int X, int Y, int W, int H, Fl_Color bgcolor)
  {
    fl_push_clip(X,Y,W,H);
    // Draw cell bg
    fl_color(bgcolor);
    fl_rectf(X,Y,W,H);
    // Draw cell data
    fl_color(FL_GRAY0);
    fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    // Draw box border
    fl_color(color());
    fl_rect(X,Y,W,H);
    fl_pop_clip();
  }

public:

  db_search_table (int X, int Y, int W, int H, const char *L=0) : Fl_Table_Row(X,Y,W,H,L), cb(0)
  {

  }

  ~db_search_table()
  {
    disconnect_DB ();
  }

  void connect_DB (string database_fn)
  {
    int rc = sqlite3_open(database_fn.c_str (), &db);
    if( rc )
      {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        throw runtime_error ("Can't open sqlite database");
      }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
  }

  void disconnect_DB ()
  {
    sqlite3_close (db);
  }

  void set_select_cb (select_cb *c)
  {
    cb = c;
  }
};

#endif
