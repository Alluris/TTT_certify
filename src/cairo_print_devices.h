// generated from create_cairo_code.m 08-Dec-2015 08:33:51

#include <cairo/cairo.h>
#include <libintl.h>
#include <string.h>

#ifndef CAIRO_PRINT_DEVICES_H
#define CAIRO_PRINT_DEVICES_H
double cairo_print_Typ1_KlasseA (cairo_t *cr);
double cairo_print_Typ1_KlasseB (cairo_t *cr);
double cairo_print_Typ1_KlasseC (cairo_t *cr);
double cairo_print_Typ1_KlasseD (cairo_t *cr);
double cairo_print_Typ1_KlasseE (cairo_t *cr);
double cairo_print_Typ2_KlasseA (cairo_t *cr);
double cairo_print_Typ2_KlasseB (cairo_t *cr);
double cairo_print_Typ2_KlasseC (cairo_t *cr);
double cairo_print_Typ2_KlasseD (cairo_t *cr);
double cairo_print_Typ2_KlasseE (cairo_t *cr);
double cairo_print_Typ2_KlasseF (cairo_t *cr);
double cairo_print_Typ2_KlasseG (cairo_t *cr);

double cairo_print_str (cairo_t *cr, const char* tc);
double cairo_print_id (cairo_t *cr, int id);
int type_class_description_id (int id, char *description, int len);
int type_class_description_str (const char *tc, char *description, int len);
#endif
