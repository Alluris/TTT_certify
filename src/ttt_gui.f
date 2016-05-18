# data file for the Fltk User Interface Designer (fluid)
version 1.0304
i18n_type 1
i18n_include <libintl.h>
i18n_function gettext
header_name {.h}
code_name {.cxx}
decl {//\#include "confuse.h"} {public global
}

decl {\#include <FL/fl_ask.H>} {public global
}

decl {\#include <FL/Fl_Text_Display.H>} {public global
}

decl {\#include <FL/Fl_Tooltip.H>} {public global
}

decl {\#include "ttt.h"} {public global
}

decl {\#include "cairo_box.h"} {public global
}

decl {\#include "cairo_print_devices.h"} {public global
}

decl {\#include "test_object_table.h"} {public global
}

decl {\#include "test_person_table.h"} {public global
}

decl {\#include "measurement_table.h"} {public global
}

decl {void run_cb(void *)} {public global
}

decl {ttt *myTTT;} {public local
}

decl {Fl_Text_Buffer *instruction_buff;} {public local
}

decl {bool test_object_edit_flag;} {private local
}

Function {create_widgets()} {open return_type void
} {
  Fl_Window mainwin {
    label {TTT certify v0.2.1 vom 18.05.2016 Alluris GmbH & Co. KG, Basler Str. 65 , 79100 Freiburg, software@alluris.de} open
    xywh {2086 239 1275 765} type Double color 40 labelfont 1 align 20 visible
  } {
    Fl_Group {} {
      label Bearbeiter open
      xywh {406 141 350 190} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Input inp_test_person_name {
        label Name
        xywh {536 211 210 25} when 8 deactivate
      }
      Fl_Input inp_test_person_supervisor {
        label Verantwortlicher
        xywh {536 246 210 25} when 8 deactivate
      }
      Fl_Button btn_test_person_new {
        label {@filenew neu}
        callback {vi_test_person_id->hide ();
inp_test_person_name->value ("");
inp_test_person_supervisor->value ("");
vi_test_person_uncertainty->value (0.0);

inp_test_person_name->activate ();
inp_test_person_supervisor->activate ();
vi_test_person_uncertainty->activate ();
btn_test_person_new->hide ();
btn_test_person_search->hide ();
btn_test_person_save->show ();
btn_test_person_abort->show ();}
        xywh {581 171 70 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_person_save {
        label {@filesave speichern}
        callback {string name = inp_test_person_name->value();
string supervisor =  inp_test_person_supervisor->value();
double uncertainty = vi_test_person_uncertainty->value() / 100.0;

if (name.empty () || supervisor.empty () || uncertainty == 0.0)
  {
    fl_alert (gettext ("Name, Verantwortlicher und Messunsicherheit sind Pflichtfelder"));
    return;
  }

// insert into database
int id = myTTT->new_test_person (name, supervisor, uncertainty);
vi_test_person_id->value (id);

btn_test_person_abort->do_callback ();}
        xywh {651 171 95 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Value_Input vi_test_person_id {
        label id
        callback {load_test_person(o->value ());}
        xywh {536 171 40 30} minimum 1 maximum 100 step 1 deactivate
      }
      Fl_Value_Input vi_test_person_uncertainty {
        label {erweiterte Messunsicherheit [%]}
        tooltip {erweiterte Messunsicherheit durch den Bediener} xywh {686 281 60 25} maximum 100 step 0.1 deactivate
      }
      Fl_Button btn_test_person_abort {
        label {@undo abbrechen}
        callback {load_test_person (vi_test_person_id->value ());
vi_test_person_id->show ();

inp_test_person_name->deactivate ();
inp_test_person_supervisor->deactivate ();
vi_test_person_uncertainty->deactivate ();

btn_test_person_new->show ();
btn_test_person_search->show ();
btn_test_person_save->hide ();
btn_test_person_abort->hide ();}
        xywh {471 171 110 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_person_search {
        label {@search}
        callback {test_person_win->show ();}
        xywh {711 171 35 30} box GLEAM_THIN_UP_BOX
      }
    }
    Fl_Group {} {
      label {Drehmoment-Schraubwerkzeug} open
      xywh {3 6 400 757} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vi_test_object_id {
        label id
        callback {int id = myTTT->_search_active_adjacent_test_object (o->value());
if (id)
  {
    o->value (id);
    load_test_object (id);
  }} selected
        xywh {177 40 40 30} minimum 1 maximum 500 step 1
      }
      Fl_Input inp_test_object_equipment_nr {
        label {Prüfmittelnummer}
        xywh {178 112 210 25} deactivate
      }
      Fl_Input inp_test_object_serial {
        label Seriennummer
        xywh {178 144 210 25} deactivate
      }
      Fl_Input inp_test_object_manufacturer {
        label Hersteller
        xywh {178 176 210 25} deactivate
      }
      Fl_Input inp_test_object_model {
        label Modell
        xywh {178 208 210 25} deactivate
      }
      Fl_Choice choice_test_object_type {
        label {DIN EN ISO 6789 Typ}
        callback {update_test_object_type_class();
update_test_object_accuracy();} open
        xywh {178 240 210 28} down_box BORDER_BOX when 1 deactivate
      } {
        MenuItem {} {
          label IA
          xywh {0 0 100 20}
        }
        MenuItem {} {
          label IB
          xywh {10 10 100 20}
        }
        MenuItem {} {
          label IC
          xywh {20 20 100 20}
        }
        MenuItem {} {
          label ID
          xywh {30 30 100 20}
        }
        MenuItem {} {
          label IE
          xywh {40 40 100 20}
        }
        MenuItem {} {
          label IIA
          xywh {50 50 100 20}
        }
        MenuItem {} {
          label IIB
          xywh {60 60 100 20}
        }
        MenuItem {} {
          label IIC
          xywh {70 70 100 20}
        }
        MenuItem {} {
          label IID
          xywh {80 80 100 20}
        }
        MenuItem {} {
          label IIE
          xywh {90 90 100 20}
        }
        MenuItem {} {
          label IIF
          xywh {100 100 100 20}
        }
        MenuItem {} {
          label IIG
          xywh {110 110 100 20}
        }
      }
      Fl_Choice choice_test_object_dir_of_rotation {
        label Funktionsrichtung
        callback {cout << o->value() << endl;} open
        xywh {288 442 90 25} down_box BORDER_BOX deactivate
      } {
        MenuItem {} {
          label beide
          xywh {30 30 100 20}
        }
        MenuItem {} {
          label rechts
          xywh {10 10 100 20}
        }
        MenuItem {} {
          label links
          xywh {20 20 100 20}
        }
      }
      Fl_Value_Input vi_test_object_lever_length {
        label {Hebellänge [cm]}
        tooltip {Maß von der Messachse bis zur Mitte des Handhaltebereichs des Griffs oder des markierten Lastangriffspunkts, es sei denn, der Kraftangriffspunkt ist markiert;} xywh {288 471 90 25} maximum 150 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_min_torque {
        label {Unterer Grenzwert [Nm]}
        tooltip {Unterer Grenzwert des vom Hersteller angegebenen Messbereichs TA} xywh {288 501 90 25} align 132 maximum 100 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_max_torque {
        label {Oberer Grenzwert [Nm]}
        callback {update_test_object_accuracy();}
        tooltip {Oberer Grenzwert des vom Hersteller angegebenen Messbereichs TE} xywh {288 530 90 25} maximum 100 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_resolution {
        label {Auflösung [Nm]}
        tooltip {Auflösung von der Anzeige r} xywh {288 560 90 25} maximum 10 step 0.01 deactivate
      }
      Fl_Input mi_test_object_attachments {
        label Anbauteile
        tooltip {Kennung aller Bauteile des Drehmoment-Schraubwerkzeugs einschließlich Passstücke und austauschbarer Aufsätze} xywh {43 600 335 74} type Multiline align 5 deactivate
      }
      Fl_Button btn_test_object_new {
        label {@filenew neu}
        callback {test_object_edit_flag = false;
set_test_object_fields_editable (true);
clear_test_object_fields ();

update_test_object_type_class();
update_test_object_accuracy ();}
        xywh {222 40 56 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_object_save {
        label {@filesave speichern}
        callback {int type_class_ind =  choice_test_object_type->value ();
const Fl_Menu_Item &item = choice_test_object_type->menu ()[type_class_ind];

string type_class = item.label ();
//cout << "type_class_ind=" << type_class_ind << " type_class=" << type_class << endl;

double accuracy = 0;
if (rb_manufacturer_accuracy->value())
  accuracy = vi_test_object_accuracy->value () / 100.0;

// Pflichtfelder
string equipment_nr = inp_test_object_equipment_nr->value ();
string serial = inp_test_object_serial->value ();
string manufacturer = inp_test_object_manufacturer->value ();
string model =  inp_test_object_model->value ();
double max_torque = vi_test_object_max_torque->value ();

if (equipment_nr.empty () || serial.empty () || manufacturer.empty () || model.empty () || max_torque == 0)
  {
    fl_alert (gettext ("Prüfmittelnummer, Seriennummer, Hersteller, Modell und oberer Grenzwert sind Pflichtfelder"));
    return;
  }

// insert into database
try
  {
    // check if we should delete the old test_object (copy)
    int old_id = vi_test_object_id->value ();
    cout << "old_id=" << old_id << " test_object_edit_flag=" << test_object_edit_flag << endl;
    if (test_object_edit_flag)
      myTTT->delete_test_object (old_id);

    int id = myTTT->new_test_object
             (equipment_nr,
              serial,
              manufacturer,
              model,
              type_class,
              choice_test_object_dir_of_rotation->value (),
              vi_test_object_lever_length->value ()/100.0,
              vi_test_object_min_torque->value (),
              max_torque,
              vi_test_object_resolution->value (),
              mi_test_object_attachments->value (),
              accuracy);

    vi_test_object_id->value (id);
    btn_test_object_abort->do_callback ();
  }
catch (std::runtime_error &e)
  {
    fl_alert (gettext ("Die Prüfmittelnummer muss eindeutig sein."));
  }}
        xywh {293 77 95 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Group {} {
        label {Höchstzulässige Abweichung} open
        xywh {40 694 338 59} box GTK_DOWN_BOX align 5
      } {
        Fl_Value_Input vi_test_object_accuracy {
          label {%}
          xywh {288 714 55 25} maximum 20 step 0.01 deactivate
        }
        Fl_Round_Button rb_accuracy_from_ISO6789 {
          label {aus der DIN EN ISO 6789}
          callback {vi_test_object_accuracy->deactivate ();
update_test_object_accuracy();}
          tooltip {Use DIN EN ISO 6789:2003-10 chapter 5.1.5.2} xywh {50 699 190 25} type Radio down_box ROUND_DOWN_BOX deactivate
        }
        Fl_Round_Button rb_manufacturer_accuracy {
          label Herstellerangabe
          callback {vi_test_object_accuracy->activate ();}
          xywh {50 724 240 25} type Radio down_box ROUND_DOWN_BOX deactivate
        }
      }
      Fl_Box cbox {
        label Klassifizierung
        xywh {22 302 365 135} box GTK_DOWN_BOX align 137
        class cairo_box
      }
      Fl_Button btn_test_object_abort {
        label {@undo abbrechen}
        callback {set_test_object_fields_editable (false);
vi_test_object_id->show ();
load_test_object (vi_test_object_id->value ());}
        xywh {178 77 110 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_object_search {
        label {@search}
        callback {test_object_win->show ();}
        xywh {352 40 35 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_object_delete {
        label {löschen}
        callback {//selected test_object which should be deleted
int id = vi_test_object_id->value ();
int r = fl_choice (gettext ("Wirklich löschen?"), gettext ("Ja"), gettext ("Nein"), 0);
if (r == 0)
  {
    // find adjacent test_object
    int adj_id = myTTT->delete_test_object (id);

    vi_test_object_id->value (adj_id);
    if (adj_id)
      load_test_object (adj_id);
    else
      {
        vi_test_object_id->hide ();
        clear_test_object_fields ();
      }
  }}
        xywh {8 40 70 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_object_copy {
        label kopieren
        callback {test_object_edit_flag = false;
set_test_object_fields_editable (true);

inp_test_object_equipment_nr->value ("");
inp_test_object_serial->value ("");}
        xywh {278 40 70 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_object_edit {
        label editieren
        callback {//vorerst ignorieren, immer kopieren
//if (myTTT->test_object_is_editable ())

test_object_edit_flag = true;
set_test_object_fields_editable (true);}
        xywh {78 40 70 30} box GLEAM_THIN_UP_BOX
      }
    }
    Fl_Group {} {
      label Umgebungsbedingungen open
      tooltip {DIN EN ISO 6789:2003-10 6.2} xywh {406 6 350 123} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vi_temperature {
        label {Kalibriertemperatur [°C]}
        tooltip {Erlaubter Bereich laut DIN EN ISO 6789:2003-10 6.2 18°C bis 28°C} xywh {685 44 60 25} maximum 40 step 0.1
      }
      Fl_Value_Input vi_humidity {
        label {relative Luftfeuchte [%rH]}
        tooltip {Erlaubter Bereich laut DIN EN ISO 6789:2003-10 6.2 max. 90%} xywh {685 79 60 25} maximum 100 step 0.1
      }
    }
    Fl_Group {} {
      label {Messgerät} open
      xywh {406 343 350 420} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vo_torque_tester_id {
        label id
        xywh {551 368 25 25} deactivate
      }
      Fl_Input out_torque_tester_serial {
        label Seriennummer
        xywh {551 403 195 25} deactivate
      }
      Fl_Input out_torque_tester_manufacturer {
        label Hersteller
        xywh {551 438 195 25} deactivate
      }
      Fl_Input out_torque_tester_model {
        label Model
        xywh {551 474 195 25} deactivate
      }
      Fl_Input out_torque_tester_cal_date {
        label Kalibrierdatum
        xywh {551 509 195 26} deactivate
      }
      Fl_Input out_torque_tester_next_cal_date {
        label {Kalibrierfälligkeit}
        xywh {551 546 195 26} align 132 deactivate
      }
      Fl_Input out_torque_tester_cal_number {
        label {Kalibrierschein Nr.}
        xywh {551 582 195 26} deactivate
      }
      Fl_Value_Input vo_torque_tester_max_torque {
        label {Nominalwert [Nm]}
        xywh {676 623 70 25} step 0.01 deactivate
      }
      Fl_Value_Input vo_torque_tester_resolution {
        label {Auflösung [Nm]}
        xywh {676 655 70 25} step 0.01 deactivate
      }
      Fl_Value_Input vo_torque_tester_uncertainty {
        label {erweiterte Messunsicherheit [%]}
        xywh {676 687 70 25} step 0.01 deactivate
      }
      Fl_Value_Input vo_torque_tester_peak_level {
        label {Peak Level [%]}
        xywh {676 720 70 25} step 1 deactivate
      }
    }
    Fl_Group {} {
      label {Prüfung} open
      xywh {761 6 515 757} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Button btn_start {
        label Start
        callback {double temp = vi_temperature->value ();
double humidity = vi_humidity->value ();

// extended uncertainty, thus 1/2
double torque_tester_uncertainty = vo_torque_tester_uncertainty->value () / 200.0;
double test_person_uncertainty =  vi_test_person_uncertainty->value () / 200.0;
double total_extended_uncertainty = 2 * sqrt (torque_tester_uncertainty*torque_tester_uncertainty + test_person_uncertainty*test_person_uncertainty);
double test_object_accuracy = vi_test_object_accuracy->value () / 100;

std::cout << "torque_tester_uncertainty  = " << torque_tester_uncertainty << endl;
std::cout << "test_person_uncertainty    = " << test_person_uncertainty << endl;
std::cout << "total_extended_uncertainty = " << total_extended_uncertainty << endl;
std::cout << "test_object_accuracy       = " << test_object_accuracy << endl;

string t = myTTT->get_test_object_type ();
bool use_mean_as_nominal_value = test_object::has_no_scale (t) && ! test_object::has_fixed_trigger (t);

if (btn_test_object_save->visible ())
  {
    fl_alert (gettext ("Bitte zuerst das Anlegen eines neuen Drehmoment-Schraubwerkzeugs beenden oder abbrechen."));
    return;
  }

if (btn_test_person_save->visible ())
  {
    fl_alert (gettext ("Bitte zuerst das Anlegen eines neuen Bearbeiters beenden oder abbrechen."));
    return;
  }

if (rb_like_6789_repeat->value () && use_mean_as_nominal_value)
  {

    fl_alert (gettext ("Bei Drehmoment-Schraubwerkzeugen des Typs IIC und IIF ist keine automatische Wiederholung möglich\\n"
                       "da der arithmetische Mittel erst am Ende der Messreihe berechnet werden kann und somit eine\\n"
                       "Bewertung erst dann möglich wird."));
    return;
  }

if (rb_quick_peak->value ())
  {
    if ( vi_single_peak->value () != 0 || use_mean_as_nominal_value)
      myTTT->start_sequencer_quick_check (temp, humidity, vi_single_peak->value ());
    else
      fl_alert ( gettext ("Nominalwert muss <> 0 Nm sein"));
  }
else if (rb_din_6789->value () || rb_like_6789_repeat->value ())
  {
    if (! (total_extended_uncertainty < test_object_accuracy/4))
      {
      char uncertainty_alert[512];
      snprintf (uncertainty_alert, 512,
                gettext ("Das Intervall der maximalen relativen erweiterten Messunsicherheit (=%.2f%%)\\n"
                         "aus Messgerät und Anwender muss kleiner als ein Viertel der\\n"
                         "höchstzulässigen Abweichung des Drehmoment-Schraubwerkszeugs (=%.2f%%) sein."),
                total_extended_uncertainty * 100,
                test_object_accuracy / 4.0 * 100);
                          
      fl_alert (uncertainty_alert);
      }
    else if (temp > 28.0 || temp < 18.0)
      fl_alert ( gettext ("Kalibriertemperatur außerhalb des erlaubten Bereichs, siehe DIN EN ISO 6789:2003-10 Kapitel 6.2"));
    else
      {
        if (humidity > 90)
          fl_alert ( gettext ("relative Luftfeuchte außerhalb des erlaubten Bereichs, siehe DIN EN ISO 6789:2003-10 Kapitel 6.2"));
        else
          myTTT->start_sequencer_ISO6789 (temp, humidity, rb_repeat_until_okay->value (), rb_like_6789_repeat->value ());
      }
  }

update_run_activation ();
Fl::add_timeout(0.01, run_cb);}
        xywh {785 195 110 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_stop {
        label Stopp
        callback {myTTT->stop_sequencer();
btn_result->show ();
mtable->show ();
btn_result->color (FL_RED);
btn_result->copy_label (gettext ("Kalibrierung durch Benutzer abgebrochen"));}
        xywh {962 195 110 30} box GLEAM_THIN_UP_BOX deactivate
      }
      Fl_Button btn_direction_cw {
        label {@+82redo}
        tooltip Drehrichtung xywh {1051 374 70 70} box NO_BOX deactivate
      }
      Fl_Button btn_direction_ccw {
        label {@+88undo}
        tooltip Drehrichtung xywh {920 374 70 70} box NO_BOX deactivate
      }
      Fl_Button btn_confirm {
        label {Bestätigung}
        callback {myTTT->set_confirmation ();}
        xywh {1140 195 110 30} box GLEAM_THIN_UP_BOX deactivate
      }
      Fl_Value_Output vo_nominal_value {
        label {Nominalwert [Nm]}
        xywh {785 382 145 60} align 5 minimum -100 maximum 100 step 0.01 textsize 30
      }
      Fl_Output to_step {
        label {aktueller Schritt}
        xywh {780 665 479 30} align 5
      }
      Fl_Dial dial_torque {
        xywh {970 359 101 101} type Line minimum -20 maximum 20
      }
      Fl_Value_Output vo_peak_torque {
        label {Messwert [Nm]}
        xywh {1110 382 145 60} align 5 step 0.01 textsize 30
      }
      Fl_Progress step_progress {
        xywh {850 715 410 25} selection_color 178
      }
      Fl_Button btn_result {
        xywh {780 700 481 50}
      }
      Fl_Value_Output vo_step_progress {
        label {%}
        xywh {790 715 35 25} align 8 step 1
      }
      Fl_Text_Display td_instruction {
        xywh {780 236 480 114} labelsize 18 align 5 textsize 20
      }
      Fl_Round_Button rb_quick_peak {
        label {Schnellprüfung}
        callback {update_quick_check_nominal_visibility();}
        xywh {785 40 145 25} type Radio down_box ROUND_DOWN_BOX value 1
      }
      Fl_Round_Button rb_like_6789_repeat {
        label {Ablauf nach DIN EN ISO 6789 aber mit Wiederholungen bei Überschreitung der zulässigen Abweichung}
        callback {update_quick_check_nominal_visibility();}
        xywh {785 75 260 65} type Radio down_box ROUND_DOWN_BOX align 148
      }
      Fl_Round_Button rb_din_6789 {
        label {DIN EN ISO 6789}
        callback {update_quick_check_nominal_visibility();}
        xywh {785 155 170 25} type Radio down_box ROUND_DOWN_BOX
      }
      Fl_Value_Input vi_single_peak {
        label {Nominalwert [Nm]}
        xywh {1085 40 50 25} step 0.1
      }
      Fl_Group grp_rise_time {
        label {Zeitüberwachung} open
        tooltip {Mindestzeitraum für die Anwendung von Drehmomentwerten für Typ II Werkzeuge} xywh {1080 93 180 62} box GTK_THIN_UP_BOX align 5
      } {
        Fl_Round_Button rb_repeat_until_okay {
          label {Wiederholung bis normgerecht}
          xywh {1090 97 170 28} type Radio down_box ROUND_DOWN_BOX value 1 align 148
        }
        Fl_Round_Button rb_ignore_timing {
          label {keine Wiederholung}
          xywh {1090 130 170 25} type Radio down_box ROUND_DOWN_BOX
        }
      }
      Fl_Table mtable {open
        xywh {785 465 470 175} hide
        class measurement_table
      } {}
    }
  }
  Fl_Window test_object_win {
    label {test_object search} open
    xywh {2288 448 935 645} type Double hide modal
  } {
    Fl_Table to {open
      xywh {9 110 920 480}
      class test_object_table
    } {}
    Fl_Input search_test_object_equipment_nr {
      label {Prüfmittelnummer}
      callback {btn_search_equipment_nr_number->do_callback ();}
      xywh {170 28 210 25} when 8
    }
    Fl_Input search_test_object_serial {
      label Seriennummer
      callback {btn_search_serial_number->do_callback ();}
      xywh {170 65 210 25} when 8
    }
    Fl_Input search_test_object_manufacturer {
      label Hersteller
      callback {btn_search_manufacturer->do_callback ();}
      xywh {660 28 210 25} when 8
    }
    Fl_Input search_test_object_model {
      label Modell
      callback {btn_search_model->do_callback ();}
      xywh {660 65 210 25} when 8
    }
    Fl_Button btn_search_equipment_nr_number {
      label {@search}
      callback {to->search_equipment_nr (search_test_object_equipment_nr->value());}
      xywh {390 25 35 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button btn_search_serial_number {
      label {@search}
      callback {to->search_serial (search_test_object_serial->value());}
      xywh {390 62 35 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button btn_search_manufacturer {
      label {@search}
      callback {to->search_manufacturer (search_test_object_manufacturer->value());}
      xywh {880 25 35 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button btn_search_model {
      label {@search}
      callback {to->search_model (search_test_object_model->value());}
      xywh {880 62 35 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button {} {
      label abbrechen
      callback {test_object_win->hide ();}
      xywh {740 605 90 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button btn_test_object_select {
      label {wählen}
      callback {to->do_select_cb ();}
      xywh {850 605 65 30} box GLEAM_THIN_UP_BOX
    }
  }
  Fl_Window test_person_win {
    label {test_person search} open
    xywh {2319 150 670 540} type Double hide modal
  } {
    Fl_Table tp {open
      xywh {20 70 640 410}
      class test_person_table
    } {}
    Fl_Input search_test_person_name {
      label Name
      callback {btn_search_person_name->do_callback ();}
      xywh {205 23 210 25} when 8
    }
    Fl_Button btn_search_person_name {
      label {@search}
      callback {tp->search_name (search_test_person_name->value());}
      xywh {425 20 35 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button btn_test_object_cancel {
      label abbrechen
      callback {test_person_win->hide ();}
      xywh {495 500 90 30} box GLEAM_THIN_UP_BOX
    }
    Fl_Button btn_test_person_select {
      label {wählen}
      callback {tp->do_select_cb ();}
      xywh {595 500 65 30} box GLEAM_THIN_UP_BOX
    }
  }
}

Function {load_test_person(int id)} {open return_type void
} {
  code {static int last_id = -1;
if (id > 0)
  {
    try
      {
        myTTT->load_test_person (id);
        inp_test_person_name->value(myTTT->get_test_person_name ().c_str ());
        inp_test_person_supervisor->value(myTTT->get_test_person_supervisor ().c_str ());
        vi_test_person_uncertainty->value(myTTT->get_test_person_uncertainty () * 100);
        last_id = id;
      }
    catch (std::runtime_error &e)
      {
        vi_test_person_id->value (last_id);

        //don't show error message
        //fl_alert (e.what ());
      }
  }} {}
}

Function {load_test_object(int id)} {open return_type void
} {
  code {static int last_id = -1;
if (id > 0)
  {
    try
      {
        myTTT->load_test_object (id);
        inp_test_object_equipment_nr->value(myTTT->get_test_object_equipment_nr ().c_str ());
        inp_test_object_serial->value(myTTT->get_test_object_serial ().c_str ());
        inp_test_object_manufacturer->value(myTTT->get_test_object_manufacturer ().c_str ());
        inp_test_object_model->value(myTTT->get_test_object_model ().c_str ());

        string type_class = myTTT->get_test_object_type ();
        int ind = choice_test_object_type->find_index (type_class.c_str ());
        //cout << "type_class = " << type_class << " ind = " << ind << endl;
        choice_test_object_type->value (ind);

        rb_din_6789->setonly ();

        //only Typ II devices are rise_time monitored
        if (myTTT->test_object_is_type (2))
          grp_rise_time->show ();
        else
          grp_rise_time->hide ();

        int dir =  myTTT->get_test_object_dir_of_rotation ();
        choice_test_object_dir_of_rotation->value (dir);

        vi_test_object_lever_length->value (100 * myTTT->get_test_object_lever_length ());
        vi_test_object_min_torque->value (myTTT->get_test_object_min_torque ());
        vi_test_object_max_torque->value (myTTT->get_test_object_max_torque ());

        //minimum und maximum festlegen
        vi_single_peak->maximum (vi_test_object_max_torque->value());

        vi_test_object_resolution->value(myTTT->get_test_object_resolution ());

        double accuracy =  myTTT->get_test_object_accuracy ();
        if (accuracy == 0)
          {
            rb_accuracy_from_ISO6789->set ();
            rb_manufacturer_accuracy->clear ();
            accuracy = myTTT->get_test_object_accuracy_from_DIN ();
          }
        else
          {
            rb_accuracy_from_ISO6789->clear ();
            rb_manufacturer_accuracy->set ();
          }

        vi_test_object_accuracy->value(100 * accuracy);
        mi_test_object_attachments->value(myTTT->get_test_object_attachments ().c_str ());

        //cairo box neu zeichnen
        update_test_object_type_class();

        last_id = id;
      }
    catch (std::runtime_error &e)
      {
        vi_test_object_id->value (last_id);
        //vorerst keine Fehlermeldung
        //fl_alert (e.what ());
      }
  }} {}
}

Function {load_torque_tester()} {open return_type void
} {
  code {// Seriennummer und next_cal_date des angeschlossenen TTTs wird ausgelesen
// und anhand dieser in der Datenbank gesucht

try
  {
    myTTT->load_torque_tester ();
    vo_torque_tester_id->value (myTTT->get_torque_tester_id ());
    out_torque_tester_serial->value(myTTT->get_torque_tester_serial ().c_str ());
    out_torque_tester_manufacturer->value(myTTT->get_torque_tester_manufacturer ().c_str ());
    out_torque_tester_model->value(myTTT->get_torque_tester_model ().c_str ());
    out_torque_tester_next_cal_date->value(myTTT->get_torque_tester_next_cal_date ().c_str());
    out_torque_tester_cal_date->value(myTTT->get_torque_tester_cal_date ().c_str());
    out_torque_tester_cal_number->value(myTTT->get_torque_tester_cal_number ().c_str());
    vo_torque_tester_max_torque->value(myTTT->get_torque_tester_max_torque ());
    vo_torque_tester_resolution->value(myTTT->get_torque_tester_resolution ());
    vo_torque_tester_uncertainty->value(100 * myTTT->get_torque_tester_uncertainty_of_measurement ());
    vo_torque_tester_peak_level->value (100 * myTTT->get_torque_tester_peak_level ());
    vi_single_peak->maximum (myTTT->get_torque_tester_max_torque ());
  }
catch (std::runtime_error &e)
  {
    fl_alert (e.what ());
  }} {}
}

Function {update_test_object_type_class()} {open
} {
  code {int id = choice_test_object_type->value ();
char buf[100];
type_class_description_id (id, buf, 100);
cbox->copy_label (buf);
mainwin->damage (FL_DAMAGE_ALL, cbox->x(), cbox->y()-30, cbox->w(), 29);
cbox->redraw ();

string selected_tc = choice_test_object_type->menu ()[id].label ();
//cout << "selected_tc = " << selected_tc << endl;

// hide minimum torque for fixed devices
if (test_object::has_fixed_trigger (selected_tc))
  vi_test_object_min_torque->hide ();
else
  vi_test_object_min_torque->show ();

// hide resolution for devices without scale
if (test_object::has_no_scale (selected_tc))
  vi_test_object_resolution->hide ();
else
  vi_test_object_resolution->show ();

// hide lever length for "Schraubendreher"
if (test_object::is_screwdriver (selected_tc))
  vi_test_object_lever_length->hide ();
else
  vi_test_object_lever_length->show ();} {}
}

Function {update_test_object_accuracy()} {open
} {
  code {if (rb_accuracy_from_ISO6789->value ())
  {
    int id = choice_test_object_type->value ();
    string selected_tc = choice_test_object_type->menu ()[id].label ();

    double max_t = vi_test_object_max_torque->value ();
    double acc = test_object::get_accuracy_from_DIN (selected_tc, max_t);
    vi_test_object_accuracy->value (acc * 100);
  }} {}
}

Function {update_run_activation()} {open
} {
  code {if (myTTT->run ())
  {
    btn_result->hide ();
    mtable->show ();
    btn_start->deactivate ();
    btn_stop->activate ();
    btn_confirm->activate ();
    //btn_direction_cw->show ();
    //btn_direction_ccw->show ();
    vi_test_object_id->deactivate ();
    btn_test_object_new->deactivate ();
    btn_test_object_search->deactivate ();
    btn_test_object_copy->deactivate ();
    btn_test_object_edit->deactivate ();
    btn_test_object_delete->deactivate ();

    vi_test_person_id->deactivate ();
    btn_test_person_new->deactivate ();
    btn_test_person_search->deactivate ();

    rb_quick_peak->deactivate ();
    rb_like_6789_repeat->deactivate ();
    rb_din_6789->deactivate ();
    vi_single_peak->deactivate ();
    grp_rise_time->deactivate ();

    vi_temperature->deactivate ();
    vi_humidity->deactivate ();
    to_step->show ();
    step_progress->show ();
    vo_step_progress->show ();
  }
else
  {
    btn_start->activate ();
    btn_stop->deactivate ();
    btn_confirm->deactivate ();
    btn_direction_cw->hide ();
    btn_direction_ccw->hide ();
    vi_test_object_id->activate ();
    btn_test_object_new->activate ();
    btn_test_object_search->activate ();
    btn_test_object_copy->activate ();
    btn_test_object_edit->activate ();
    btn_test_object_delete->activate ();

    vi_test_person_id->activate ();
    btn_test_person_new->activate ();
    btn_test_person_search->activate ();
    grp_rise_time->activate ();

    rb_quick_peak->activate ();
    rb_like_6789_repeat->activate ();
    rb_din_6789->activate ();
    vi_single_peak->activate ();

    vi_temperature->activate ();
    vi_humidity->activate ();
    to_step->hide ();
    step_progress->hide ();
    vo_step_progress->hide ();
  }} {}
}

Function {set_test_object_fields_editable(bool editable)} {open return_type void
} {
  code {if (editable)
  {
    vi_test_object_id->hide ();
    inp_test_object_equipment_nr->activate ();
    inp_test_object_serial->activate ();
    inp_test_object_manufacturer->activate ();
    inp_test_object_model->activate ();
    choice_test_object_type->activate ();
    choice_test_object_dir_of_rotation->activate ();
    vi_test_object_lever_length->activate ();
    vi_test_object_min_torque->activate ();
    vi_test_object_max_torque->activate ();
    vi_test_object_resolution->activate ();
    mi_test_object_attachments->activate ();
    rb_accuracy_from_ISO6789->activate ();
    rb_manufacturer_accuracy->activate ();

    btn_test_object_search->hide ();
    btn_test_object_new->hide ();
    btn_test_object_copy->hide ();
    btn_test_object_edit->hide ();
    btn_test_object_delete->hide ();
    btn_test_object_save->show ();
    btn_test_object_abort->show ();

  }
else
  {
    inp_test_object_equipment_nr->deactivate ();
    inp_test_object_serial->deactivate ();
    inp_test_object_manufacturer->deactivate ();
    inp_test_object_model->deactivate ();
    choice_test_object_type->deactivate ();
    choice_test_object_dir_of_rotation->deactivate ();
    vi_test_object_lever_length->deactivate ();
    vi_test_object_min_torque->deactivate ();
    vi_test_object_max_torque->deactivate ();
    vi_test_object_resolution->deactivate ();
    mi_test_object_attachments->deactivate ();
    vi_test_object_accuracy->deactivate ();
    rb_accuracy_from_ISO6789->deactivate ();
    rb_manufacturer_accuracy->deactivate ();

    btn_test_object_search->show ();
    btn_test_object_new->show ();
    btn_test_object_copy->show ();
    btn_test_object_edit->show ();
    btn_test_object_delete->show ();
    btn_test_object_save->hide ();
    btn_test_object_abort->hide ();
  }} {}
}

Function {clear_test_object_fields()} {open return_type void
} {
  code {inp_test_object_equipment_nr->value ("");
inp_test_object_serial->value ("");
inp_test_object_manufacturer->value ("");
inp_test_object_model->value ("");
choice_test_object_type->value(0);
choice_test_object_dir_of_rotation->value(0);
vi_test_object_lever_length->value(0);
vi_test_object_min_torque->value(0);
vi_test_object_max_torque->value(0);
vi_test_object_resolution->value(0);
mi_test_object_attachments->value("");

rb_accuracy_from_ISO6789->set ();
rb_manufacturer_accuracy->clear ();} {}
}

Function {update_quick_check_nominal_visibility()} {open return_type void
} {
  code {if (rb_quick_peak->value ())
  vi_single_peak->show ();
else
  vi_single_peak->hide ();} {}
}
