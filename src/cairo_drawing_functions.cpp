/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

Create calibration reports using cairo

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

#include "cairo_drawing_functions.h"

void print_page_number (cairo_t *cr, int num)
{
  char str[20];
  snprintf (str, 20, "Seite %i", num);
  cairo_centered_text (cr, 10, 28, str);
}

report_result create_DIN6789_report ( sqlite3 *db,
                                      int measurement_id,
                                      const char *report_fn,
                                      bool repeat_on_tolerance_violation)

{
  struct report_result ret;
  ret.values_below_max_deviation = true;
  ret.timing_violation = false;

  measurement mm;
  mm.load_with_id (db, measurement_id);

  // DEBUGGING
  cout << mm.tp;
  cout << mm.to;
  cout << mm.tt;
  cout << mm;

  /********************** CAIRO DRAWING *************************************/
  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_pdf_surface_create (report_fn, 596, 843); //A4 portrait
  cr = cairo_create (surface);

  // use cm scale
  cairo_scale (cr, 1/2.54*72, 1/2.54*72);

  double left_c1 = 1.5;
  double left_c2 = 8.5;
  int page_num = 1;

  // init
  cairo_font_extents_t fe;
  //cairo_text_extents_t te;
  cairo_set_font_size (cr, 0.5);
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

  cairo_select_font_face (cr, "Georgia",
                          CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  // cairo_select_font_face mit cairo_debug_reset_static_data verursacht:
  // cairo_report: ../../../../src/cairo-hash.c:217: _cairo_hash_table_destroy:
  // Assertion `hash_table->live_entries == 0' failed.

  cairo_font_extents (cr, &fe);

  // Überschrift
  cairo_move_to (cr, left_c1, 3.2);
  if (! repeat_on_tolerance_violation)
    cairo_show_text (cr, "Kalibrierschein nach DIN EN ISO 6789-1:2015-02");
  else
    cairo_show_text (cr, "Kalibrierschein");

  cairo_move_to (cr, left_c1, 3.2 + 0.7);
  cairo_show_text (cr, "Calibration Certificate");

  // horizontale Linie
  cairo_horizontal_line (cr, left_c1, 5.4);

  // Kalibrierdatum (end_time der Messung)
  cairo_set_font_size (cr, 0.4);
  double top = 6;
  top = cairo_print_two_columns (cr, left_c1, left_c2, top, "Kalibrierdatum", "Date of Calibration", mm.end_time);

  // Gegenstand
  top = mm.to.cairo_print (cr, left_c1, left_c2, top);

  // Neue Seite
  print_page_number (cr, page_num++);
  cairo_show_page (cr);
  top = 2;

  // horizontale Linie, Kalibrierverfahren
  top = cairo_horizontal_line (cr, left_c1, top);

  top = cairo_print_two_columns (cr, left_c1, left_c2, top, "Kalibrierverfahren", "Calibration method", "");
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  if (! repeat_on_tolerance_violation)
    top = cairo_print_text (cr, left_c1, top, "Das Gerät wurde nach den Vorschriften der DIN EN ISO 6789-1:2015 kalibriert.\n"
                            "Angegeben ist die erweiterte Messunsicherheit, die sich aus der Standardmessunsicherheit\n"
                            "durch Multiplikation mit dem Erweiterungsfaktor k=2 ergibt. Der Wert der Messgröße liegt\n"
                            "mit einer Wahrscheinlichkeit von 95% im zugeordneten Werteintervall.");

  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
  if (! repeat_on_tolerance_violation)
    top = cairo_print_text (cr, left_c1, top + 0.2, "The instrument was calibrated according directive DIN EN ISO 6789-1:2015.\n"
                            "Stated is the expanded uncertainty. The exanded uncertainty assigned to the measurement\n"
                            "results is obtained by multiplying the standard uncertainty by the coverage factor k=2.\n"
                            "The value of the measurand lies within the asign range of values with a probability of 95%.");

  // Prüfer
  top = cairo_horizontal_line (cr, left_c1, top);
  top = mm.tp.cairo_print (cr, left_c1, left_c2, top);

  // Torque Tester
  top = cairo_horizontal_line (cr, left_c1, top);
  top = cairo_print_two_columns (cr, left_c1, left_c2, top, "Verwendete Mess- und Prüfeinrichtungen", "Test and equipment used", "");
  top = mm.tt.cairo_print (cr, left_c1, left_c2, top + 0.5);

  // Neue Seite
  print_page_number (cr, page_num++);
  cairo_show_page (cr);
  top = 2;
  top = cairo_horizontal_line (cr, left_c1, top);
  top = mm.cairo_print_header (cr, left_c1, left_c2, top);

  print_page_number (cr, page_num++);
  cairo_show_page (cr);
  top = 2;

  top = cairo_horizontal_line (cr, left_c1, top);
  // Messung
  top = mm.cairo_print (cr, left_c1, left_c2, top, ret.values_below_max_deviation, ret.timing_violation);

  // Konformitätsaussage
  top = mm.cairo_print_conformity (cr, left_c1, top + 0.5, ret.values_below_max_deviation);

  // Bemerkungen
  top = cairo_print_two_columns (cr, left_c1, left_c2, top + 0.5, "Bemerkungen", "Remarks", "");

  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  top = cairo_print_text (cr, left_c1, top, "Die englische Fassung dieses Kalibrierscheines ist eine unverbindliche Übersetzung.\n"
                          "Im Zweifelsfall gilt der deutsche Originaltext.");

  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_NORMAL);
  top = cairo_print_text (cr, left_c1, top, "The english version of the calibration certificate is not a binding translation.\n"
                          "If any matter gives rise to controversy, the german original text is valid.");

  top += 1;
  top = cairo_print_two_columns (cr, left_c1, left_c2, top, "Ende des Kalibrierscheines", "End of calibration certificate", "");

  print_page_number (cr, page_num++);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  printf ("cairo_get_reference_count (cr) = %i\n", cairo_get_reference_count (cr));
  printf ("cairo_surface_get_reference_count (surface) = %i\n", cairo_surface_get_reference_count (surface));

  return ret;
}

bool create_quick_test_report ( sqlite3 *db,
                                int measurement_id,
                                const char *report_fn)

{
  bool ret;
  measurement mm;
  mm.load_with_id (db, measurement_id);

  // DEBUGGING
  cout << mm.tp;
  cout << mm.to;
  cout << mm.tt;
  cout << mm;

  /********************** CAIRO DRAWING *************************************/
  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_pdf_surface_create (report_fn, 596, 843); //A4 portrait
  cr = cairo_create (surface);

  // use cm scale
  cairo_scale (cr, 1/2.54*72, 1/2.54*72);

  double left_c1 = 1.5;
  double left_c2 = 8.5;

  // init
  cairo_font_extents_t fe;
  //cairo_text_extents_t te;
  cairo_set_font_size (cr, 0.5);
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

  cairo_select_font_face (cr, "Georgia",
                          CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  // cairo_select_font_face mit cairo_debug_reset_static_data verursacht:
  // cairo_report: ../../../../src/cairo-hash.c:217: _cairo_hash_table_destroy:
  // Assertion `hash_table->live_entries == 0' failed.

  cairo_font_extents (cr, &fe);

  // Überschrift
  cairo_move_to (cr, left_c1, 3.2);
  cairo_show_text (cr, "Schnellkalibrierung");
  cairo_move_to (cr, left_c1, 3.2 + 0.7);
  cairo_show_text (cr, "Quick check");

  // horizontale Linie
  cairo_horizontal_line (cr, left_c1, 5.4);

  // Kalibrierdatum (end_time der Messung)
  cairo_set_font_size (cr, 0.4);
  double top = 6;
  top = cairo_print_two_columns (cr, left_c1, left_c2, top, "Kalibrierdatum", "Date of Calibration", mm.end_time);

  // Gegenstand
  top = mm.to.cairo_print (cr, left_c1, left_c2, top);


  // FIXME: was soll hier rein?
  // hier irgendwo prüfen ob wert in der toleranz liegt
  ret = 0;

  top += 1;
  top = cairo_print_two_columns (cr, left_c1, left_c2, top, "Ende des Kalibrierscheines", "End of calibration certificate", "");

  //print_page_number (cr, page_num++);

  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  printf ("cairo_get_reference_count (cr) = %i\n", cairo_get_reference_count (cr));
  printf ("cairo_surface_get_reference_count (surface) = %i\n", cairo_surface_get_reference_count (surface));
  return ret;
}
