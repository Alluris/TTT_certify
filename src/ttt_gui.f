# data file for the Fltk User Interface Designer (fluid)
version 1.0303 
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

decl {\#include "ttt.h"} {public global
} 

decl {\#include "cairo_box.h"} {public global
} 

decl {\#include "cairo_print_devices.h"} {public global
} 

decl {void run_cb(void *)} {public global
} 

decl {ttt *myTTT;} {public local
} 

decl {Fl_Text_Buffer *instruction_buff;} {public local
} 

Function {create_widgets()} {open return_type void
} {
  Fl_Window mainwin {
    label {TTT certify v0.1.4 11.12.2015} open
    xywh {291 104 1280 765} type Double color 40 labelfont 1 align 20 visible
  } {
    Fl_Group {} {
      label Bearbeiter open
      xywh {406 141 350 190} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Input inp_test_person_name {
        label Name
        xywh {536 211 210 25} deactivate
      }
      Fl_Input inp_test_person_supervisor {
        label Verantwortlicher
        xywh {536 246 210 25} deactivate
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
btn_test_person_save->show ();
btn_test_person_abort->show ();}
        xywh {581 171 70 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Button btn_test_person_save {
        label {@filesave speichern}
        callback {// insert into database
int id = myTTT->new_test_person (inp_test_person_name->value(), inp_test_person_supervisor->value(), vi_test_person_uncertainty->value() / 100.0);
vi_test_person_id->value (id);

btn_test_person_abort->do_callback ();}
        xywh {651 171 95 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Value_Input vi_test_person_id {
        label id
        callback {load_test_person(o->value ());}
        xywh {536 171 40 30} minimum 1 maximum 100 step 1
      }
      Fl_Value_Input vi_test_person_uncertainty {
        label {Messunsicherheit [%]}
        xywh {686 281 60 25} maximum 100 step 0.1 deactivate
      }
      Fl_Button btn_test_person_abort {
        label {@undo abbrechen}
        callback {load_test_person (vi_test_person_id->value ());
vi_test_person_id->show ();

inp_test_person_name->deactivate ();
inp_test_person_supervisor->deactivate ();
vi_test_person_uncertainty->deactivate ();

btn_test_person_new->show ();
btn_test_person_save->hide ();
btn_test_person_abort->hide ();}
        xywh {471 171 110 30} box GLEAM_THIN_UP_BOX
      }
    }
    Fl_Group {} {
      label {Drehmoment-Schraubwerkzeug} open
      xywh {4 6 396 756} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vi_test_object_id {
        label id
        callback {load_test_object(o->value ());}
        xywh {174 40 40 30} minimum 1 maximum 500 step 1
      }
      Fl_Input inp_test_object_serial {
        label Seriennummer
        xywh {174 75 210 25} deactivate
      }
      Fl_Input inp_test_object_manufacturer {
        label Hersteller
        xywh {174 105 210 25} deactivate
      }
      Fl_Input inp_test_object_model {
        label Modell
        xywh {174 137 210 25} deactivate
      }
      Fl_Choice choice_test_object_type {
        label {DIN 6789 Typ}
        callback {update_test_object_type_class();
update_test_object_accuracy();} open
        xywh {174 172 210 28} down_box BORDER_BOX when 1 deactivate
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
        xywh {285 405 90 25} down_box BORDER_BOX deactivate
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
        tooltip {Maß von der Messachse bis zur Mitte des Handhaltebereichs des Griffs oder des markierten Lastangriffspunkts, es sei denn, der Kraftangriffspunkt ist markiert;} xywh {285 435 90 25} maximum 150 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_min_torque {
        label {Unterer Grenzwert [Nm]}
        tooltip {Unterer Grenzwert des vom Hersteller angegebenen Messbereichs TA} xywh {285 470 90 25} align 132 maximum 100 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_max_torque {
        label {Oberer Grenzwert [Nm]}
        callback {update_test_object_accuracy();}
        tooltip {Oberer Grenzwert des vom Hersteller angegebenen Messbereichs TE} xywh {285 505 90 25} maximum 100 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_resolution {
        label {Auflösung [Nm]}
        tooltip {Auflösung von der Anzeige r} xywh {285 540 90 25} maximum 10 step 0.01 deactivate
      }
      Fl_Input mi_test_object_attachments {
        label Anbauteile
        tooltip {Kennung aller Bauteile des Drehmoment-Schraubwerkzeugs einschließlich Passstücke und austauschbarer Aufsätze} xywh {40 579 335 95} type Multiline align 5 deactivate
      }
      Fl_Button btn_test_object_new {
        label {@filenew neu}
        callback {vi_test_object_id->hide ();

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
rb_accuracy_from_din6789->activate ();
rb_manufacturer_accuracy->activate ();

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

rb_accuracy_from_din6789->set ();
rb_manufacturer_accuracy->clear ();

btn_test_object_new->hide ();
btn_test_object_save->show ();
btn_test_object_abort->show ();

update_test_object_type_class();
update_test_object_accuracy ();}
        xywh {219 40 70 30} box GLEAM_THIN_UP_BOX
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

// insert into database
int id = myTTT->new_test_object
  (inp_test_object_serial->value (),
   inp_test_object_manufacturer->value (),
   inp_test_object_model->value (),
   type_class,
   choice_test_object_dir_of_rotation->value (),
   vi_test_object_lever_length->value ()/100.0,
   vi_test_object_min_torque->value (),
   vi_test_object_max_torque->value (),
   vi_test_object_resolution->value (),
   mi_test_object_attachments->value (),
   accuracy);

vi_test_object_id->value (id);
btn_test_object_abort->do_callback ();}
        xywh {289 40 95 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Group {} {
        label {Höchstzulässige Abweichung} open
        xywh {37 694 340 59} box GTK_DOWN_BOX align 5
      } {
        Fl_Value_Input vi_test_object_accuracy {
          label {%}
          xywh {292 714 55 25} align 8 maximum 20 step 0.01 deactivate
        }
        Fl_Round_Button rb_accuracy_from_din6789 {
          label {aus der DIN EN ISO 6789}
          callback {vi_test_object_accuracy->deactivate ();
update_test_object_accuracy();}
          tooltip {Use DIN EN ISO 6789-1:2015 chapter 5.1.5} xywh {47 699 190 25} type Radio down_box ROUND_DOWN_BOX deactivate
        }
        Fl_Round_Button rb_manufacturer_accuracy {
          label Herstellerangabe
          callback {vi_test_object_accuracy->activate ();}
          xywh {47 724 240 25} type Radio down_box ROUND_DOWN_BOX deactivate
        }
      }
      Fl_Box cbox {
        label Klassifizierung
        xywh {19 246 365 135} box GTK_DOWN_BOX align 137
        class cairo_box
      }
      Fl_Button btn_test_object_abort {
        label {@undo abbrechen}
        callback {load_test_object (vi_test_object_id->value ());
vi_test_object_id->show ();

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
rb_accuracy_from_din6789->deactivate ();
rb_manufacturer_accuracy->deactivate ();

btn_test_object_new->show ();
btn_test_object_save->hide ();
btn_test_object_abort->hide ();}
        xywh {109 40 110 30} box GLEAM_THIN_UP_BOX
      }
    }
    Fl_Group {} {
      label Umgebungsbedingungen open
      tooltip {DIN EN ISO 6789-1:2015 6.3} xywh {406 6 350 123} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Value_Input vi_temperature {
        label {Kalibriertemperatur [°C]}
        tooltip {Erlaubter Bereich laut DIN EN ISO 6789-1:2015 6.3 18°C bis 28°C} xywh {666 44 60 25} maximum 40 step 0.1 value 18
      }
      Fl_Value_Input vi_humidity {
        label {relative Luftfeuchte [%rH]}
        tooltip {Erlaubter Bereich laut DIN EN ISO 6789-1:2015 6.3 max. 90%} xywh {666 79 60 25} maximum 100 step 0.1 value 90
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
        xywh {676 658 70 25} step 0.01 deactivate
      }
      Fl_Value_Input vo_torque_tester_uncertainty {
        label {erweiterte Messunsicherheit [%]}
        xywh {676 694 70 25} step 0.01 deactivate
      }
    }
    Fl_Group {} {
      label {Prüfung} open
      xywh {761 6 515 756} box GLEAM_UP_BOX labelfont 1 labelsize 18 align 21
    } {
      Fl_Button btn_start {
        label Start
        callback {if (btn_test_object_save->visible ())
  {
    fl_alert ("Bitte zuerst das Anlegen eines neuen Drehmoment-Schraubwerkzeugs beenden oder abbrechen.");
    return;
  }

if (btn_test_person_save->visible ())
  {
    fl_alert ("Bitte zuerst das Anlegen eines neuen Bearbeiters beenden oder abbrechen.");
    return;
  }

double temp = vi_temperature->value ();
double humidity = vi_humidity->value ();

// extended uncertainty, thus 1/2
double torque_tester_uncertainty = vo_torque_tester_uncertainty->value () / 200.0;
double test_person_uncertainty =  vi_test_person_uncertainty->value ()/100.0;
double total_extended_uncertainty = 2 * sqrt (torque_tester_uncertainty*torque_tester_uncertainty + test_person_uncertainty*test_person_uncertainty);

if (rb_quick_peak->value ())
  {
    if ( vi_single_peak->value () != 0)
      myTTT->start_sequencer_single_peak (temp, humidity, vi_single_peak->value ());
    else
      fl_alert ( gettext ("Nominalwert muss <> 0 Nm sein"));
  }
else if (rb_din_6789->value ())
  {
    if (! (total_extended_uncertainty < vi_test_object_accuracy->value ()/400.0))
       fl_alert ( gettext ("Das Intervall der maximalen relativen erweiterten Messunsicherheit\\n"
                           "aus Messgerät und Anwender muss kleiner als ein Viertel der\\n"	
  			   "höchstzulässigen Abweichung des Drehmoment-Schraubwerkszeugs sein."));
      else if (temp > 28.0 || temp < 18.0)
        fl_alert ( gettext ("Kalibriertemperatur außerhalb des erlaubten Bereichs, siehe DIN 6789-1:2015 Kapitel 6.3"));
        else
          {
            if (humidity > 90)
             fl_alert ( gettext ("relative Luftfeuchte außerhalb des erlaubten Bereichs, siehe DIN 6789-1:2015 Kapitel 6.3"));
            else
             myTTT->start_sequencer_DIN6789 (temp, humidity, rb_repeat_until_okay->value ());
          }
  }
else if (rb_ASME->value ())
  fl_alert ("FIXME: Noch nicht implementiert");

update_run_activation ();
Fl::add_timeout(0.01, run_cb);}
        xywh {785 215 110 30} box GLEAM_THIN_UP_BOX
      }
      Fl_Value_Output vo_nominal_value {
        label {Nominalwert [Nm]}
        xywh {785 503 145 60} align 5 minimum -100 maximum 100 step 0.01 textsize 30
      }
      Fl_Output to_step {
        label {aktueller Schritt}
        xywh {776 613 484 30} align 5
      }
      Fl_Dial dial_torque {
        xywh {955 470 130 130} type Line minimum -20 maximum 20
      }
      Fl_Button btn_stop {
        label Stopp
        callback {myTTT->stop_sequencer();
btn_result->show ();
btn_result->color (FL_RED);
btn_result->copy_label (gettext ("Kalibrierung durch Benutzer abgebrochen"));}
        xywh {962 215 110 30} box GLEAM_THIN_UP_BOX deactivate
      }
      Fl_Button btn_confirm {
        label {Bestätigung}
        callback {myTTT->set_confirmation ();}
        xywh {1140 215 110 30} box GLEAM_THIN_UP_BOX deactivate
      }
      Fl_Value_Output vo_peak_torque {
        label {Messwert [Nm]}
        xywh {1110 503 145 60} align 5 step 0.01 textsize 30
      }
      Fl_Progress step_progress {
        xywh {840 654 420 25} selection_color 178
      }
      Fl_Button btn_result {
        xywh {775 690 486 50}
      }
      Fl_Value_Output vo_step_progress {
        label {%}
        xywh {780 654 35 25} align 8 step 1
      }
      Fl_Text_Display td_instruction {
        label Anweisung selected
        xywh {780 340 480 114} labelsize 18 align 5 textsize 20
      }
      Fl_Round_Button rb_quick_peak {
        label {Schnellprüfung}
        callback {if (o->value ())
  {
   grp_rise_time->hide ();
   vi_single_peak->show ();
  }}
        xywh {785 50 145 25} type Radio down_box ROUND_DOWN_BOX value 1
      }
      Fl_Round_Button rb_ASME {
        label ASME
        callback {if (o->value ())
  {
   grp_rise_time->hide ();
   vi_single_peak->hide ();
  }}
        xywh {785 77 145 24} type Radio down_box ROUND_DOWN_BOX
      }
      Fl_Round_Button rb_din_6789 {
        label {DIN EN ISO 6789-1}
        callback {if (o->value ())
  {
   grp_rise_time->show ();
   vi_single_peak->hide ();
  }}
        xywh {785 104 170 25} type Radio down_box ROUND_DOWN_BOX
      }
      Fl_Value_Input vi_single_peak {
        label {Nominalwert [Nm]}
        xywh {1085 50 50 25} step 0.1
      }
      Fl_Group grp_rise_time {
        label {Zeitüberwachung} open
        xywh {915 125 270 65} align 4
      } {
        Fl_Round_Button rb_repeat_until_okay {
          label {Wiederholung bis normgerecht}
          xywh {925 131 245 26} type Radio down_box ROUND_DOWN_BOX value 1
        }
        Fl_Round_Button rb_ignore_timing {
          label {keine Wiederholung}
          xywh {925 160 195 25} type Radio down_box ROUND_DOWN_BOX
        }
      }
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
        inp_test_object_serial->value(myTTT->get_test_object_serial ().c_str ());
        inp_test_object_manufacturer->value(myTTT->get_test_object_manufacturer ().c_str ());
        inp_test_object_model->value(myTTT->get_test_object_model ().c_str ());

  string type_class = myTTT->get_test_object_type ();
  int ind = choice_test_object_type->find_index (type_class.c_str ());
  //cout << "type_class = " << type_class << " ind = " << ind << endl;
  choice_test_object_type->value (ind);

        rb_din_6789->setonly ();
	grp_rise_time->show ();

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
            rb_accuracy_from_din6789->set ();
            rb_manufacturer_accuracy->clear ();
            accuracy = myTTT->get_test_object_accuracy_from_DIN ();
          }
        else
          {
            rb_accuracy_from_din6789->clear ();
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
  code {if (rb_accuracy_from_din6789->value ())
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
  btn_start->deactivate ();
  btn_stop->activate ();
  btn_confirm->activate ();
  vi_test_object_id->deactivate ();
  btn_test_object_new->deactivate ();
  vi_test_person_id->deactivate ();
  btn_test_person_new->deactivate ();
  vi_temperature->deactivate ();
  vi_humidity->deactivate ();
}
else
{
  btn_start->activate ();
  btn_stop->deactivate ();
  btn_confirm->deactivate ();
  vi_test_object_id->activate ();
  btn_test_object_new->activate ();
  vi_test_person_id->activate ();
  btn_test_person_new->activate ();
  vi_temperature->activate ();
  vi_humidity->activate ();
}} {}
} 
