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
    label {TTT certify v0.1.3 09.12.2015} open
    xywh {166 164 1280 765} type Double align 20 visible
  } {
    Fl_Group {} {
      label Bearbeiter open
      xywh {405 155 350 160} box GTK_DOWN_BOX align 5
    } {
      Fl_Input inp_test_person_name {
        label Name
        xywh {535 205 210 25} deactivate
      }
      Fl_Input inp_test_person_supervisor {
        label Verantwortlicher
        xywh {535 240 210 25} deactivate
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
btn_test_person_save->show ();}
        xywh {580 165 70 30}
      }
      Fl_Button btn_test_person_save {
        label {@filesave speichern}
        callback {// insert into database
int id = myTTT->new_test_person (inp_test_person_name->value(), inp_test_person_supervisor->value(), vi_test_person_uncertainty->value() / 100.0);
vi_test_person_id->value (id);
vi_test_person_id->show ();

inp_test_person_name->deactivate ();
inp_test_person_supervisor->deactivate ();
vi_test_person_uncertainty->deactivate ();

btn_test_person_new->show ();
btn_test_person_save->hide ();}
        xywh {650 165 95 30}
      }
      Fl_Value_Input vi_test_person_id {
        label id
        callback {load_test_person(o->value ());}
        xywh {535 165 40 30} maximum 100 step 1
      }
      Fl_Value_Input vi_test_person_uncertainty {
        label {Messunsicherheit [%]}
        xywh {685 275 60 25} maximum 100 step 0.1 deactivate
      }
    }
    Fl_Group {} {
      label {Drehmoment-Schraubwerkzeug} open
      xywh {4 23 395 739} box GTK_DOWN_BOX align 5
    } {
      Fl_Value_Input vi_test_object_id {
        label id
        callback {load_test_object(o->value ());}
        xywh {174 30 40 30} maximum 500 step 1
      }
      Fl_Input inp_test_object_serial {
        label Seriennummer
        xywh {174 69 210 25} deactivate
      }
      Fl_Input inp_test_object_manufacturer {
        label Hersteller
        xywh {174 102 210 25} deactivate
      }
      Fl_Input inp_test_object_model {
        label Modell
        xywh {174 137 210 25} deactivate
      }
      Fl_Choice choice_test_object_type {
        label {DIN 6789 Typ}
        callback {update_test_object_type_class();
update_test_object_accuracy();} open
        xywh {174 172 60 28} down_box BORDER_BOX when 1 deactivate
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
        xywh {199 406 90 25} down_box BORDER_BOX deactivate
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
        tooltip {Maß von der Messachse bis zur Mitte des Handhaltebereichs des Griffs oder des markierten Lastangriffspunkts, es sei denn, der Kraftangriffspunkt ist markiert;} xywh {199 436 90 25} maximum 150 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_min_torque {
        label {Unterer Grenzwert [Nm]}
        tooltip {Unterer Grenzwert des vom Hersteller angegebenen Messbereichs TA} xywh {199 471 90 25} align 132 maximum 100 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_max_torque {
        label {Oberer Grenzwert [Nm]}
        callback {update_test_object_accuracy();}
        tooltip {Oberer Grenzwert des vom Hersteller angegebenen Messbereichs TE} xywh {199 506 90 25} maximum 100 step 0.01 deactivate
      }
      Fl_Value_Input vi_test_object_resolution {
        label {Auflösung [Nm]}
        tooltip {Auflösung von der Anzeige r} xywh {199 541 90 25} maximum 10 step 0.01 deactivate
      }
      Fl_Input mi_test_object_attachments {
        label Anbauteile
        tooltip {Kennung aller Bauteile des Drehmoment-Schraubwerkzeugs einschließlich Passstücke und austauschbarer Aufsätze} xywh {40 580 335 95} type Multiline align 5 deactivate
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

update_test_object_type_class();
update_test_object_accuracy ();}
        xywh {219 30 70 30}
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

//window->hide ();}
        xywh {289 30 95 30}
      }
      Fl_Group {} {
        label {Höchstzulässige Abweichung} open
        xywh {39 696 340 59} box GTK_DOWN_BOX align 5
      } {
        Fl_Value_Input vi_test_object_accuracy {
          label {%}
          xywh {294 716 55 25} align 8 maximum 20 step 0.01 deactivate
        }
        Fl_Round_Button rb_accuracy_from_din6789 {
          label {aus der DIN EN ISO 6789}
          callback {vi_test_object_accuracy->deactivate ();
update_test_object_accuracy();}
          tooltip {Use DIN EN ISO 6789-1:2015 chapter 5.1.5} xywh {49 701 190 25} type Radio down_box ROUND_DOWN_BOX deactivate
        }
        Fl_Round_Button rb_manufacturer_accuracy {
          label Herstellerangabe
          callback {vi_test_object_accuracy->activate ();}
          xywh {49 726 240 25} type Radio down_box ROUND_DOWN_BOX deactivate
        }
      }
      Fl_Box cbox {
        label Klassifizierung
        xywh {19 246 365 135} box GTK_DOWN_BOX align 133
        class cairo_box
      }
    }
    Fl_Group {} {
      label Umgebungsbedingungen open
      tooltip {DIN EN ISO 6789-1:2015 6.3} xywh {405 23 350 95} box GTK_DOWN_BOX align 5
    } {
      Fl_Value_Input vi_temperature {
        label {Kalibriertemperatur [°C]}
        tooltip {Erlaubter Bereich laut DIN EN ISO 6789-1:2015 6.3 18°C bis 28°C} xywh {665 44 60 25} maximum 40 step 0.1 value 18
      }
      Fl_Value_Input vi_humidity {
        label {relative Luftfeuchte [%rH]}
        tooltip {Erlaubter Bereich laut DIN EN ISO 6789-1:2015 6.3 max. 90%} xywh {665 79 60 25} maximum 100 step 0.1 value 90
      }
    }
    Fl_Group {} {
      label {Messgerät} open
      xywh {405 351 350 412} box GTK_DOWN_BOX align 5
    } {
      Fl_Value_Output vo_torque_tester_id {
        label id
        xywh {550 368 25 25}
      }
      Fl_Output out_torque_tester_serial {
        label Seriennummer
        xywh {550 403 195 25} color 49
      }
      Fl_Output out_torque_tester_manufacturer {
        label Hersteller
        xywh {550 438 195 25} color 49
      }
      Fl_Output out_torque_tester_model {
        label Model
        xywh {550 474 195 25} color 49
      }
      Fl_Output out_torque_tester_cal_date {
        label Kalibrierdatum
        xywh {550 509 195 26} color 49
      }
      Fl_Output out_torque_tester_next_cal_date {
        label {Kalibrierfälligkeit}
        xywh {550 546 195 26} color 49 align 132
      }
      Fl_Output out_torque_tester_cal_number {
        label {Kalibrierschein Nr.}
        xywh {550 582 195 26} color 49
      }
      Fl_Value_Output vo_torque_tester_max_torque {
        label {Nominalwert [Nm]}
        xywh {675 623 70 25} step 0.01
      }
      Fl_Value_Output vo_torque_tester_resolution {
        label {Auflösung [Nm]}
        xywh {675 658 70 25} step 0.01
      }
      Fl_Value_Output vo_torque_tester_uncertainty {
        label {erweiterte Messunsicherheit [%]} selected
        xywh {675 694 70 25} maximum 100 step 0.01
      }
    }
    Fl_Group {} {
      label {Prüfung} open
      xywh {761 23 515 739} box GTK_DOWN_BOX align 5
    } {
      Fl_Button btn_start {
        label Start
        callback {double temp = vi_temperature->value ();
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

if (myTTT->run ())
{
 btn_result->hide ();
 btn_start->deactivate ();
 btn_stop->activate ();
}
else
{
 btn_start->activate ();
 btn_stop->deactivate ();
}

Fl::add_timeout(0.01, run_cb);}
        xywh {1153 61 110 30} box GTK_UP_BOX
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
        xywh {1153 96 110 30} box GTK_UP_BOX
      }
      Fl_Button btn_confirm {
        label {Bestätigung}
        callback {myTTT->set_confirmation ();}
        xywh {1153 131 110 30} box GTK_UP_BOX
      }
      Fl_Group {} {
        label {Prüfverfahren} open
        xywh {776 53 364 115} box GTK_UP_BOX align 5
      } {
        Fl_Round_Button rb_quick_peak {
          label {Schnellprüfung}
          xywh {791 68 160 25} type Radio down_box ROUND_DOWN_BOX value 1
        }
        Fl_Round_Button rb_din_6789 {
          label {DIN EN ISO 6789-1}
          xywh {791 98 195 25} type Radio down_box ROUND_DOWN_BOX
        }
        Fl_Round_Button rb_ASME {
          label ASME
          xywh {791 128 195 24} type Radio down_box ROUND_DOWN_BOX
        }
        Fl_Value_Input vi_single_peak {
          label {Nominalwert [Nm]}
          xywh {1070 68 50 25} step 0.1
        }
      }
      Fl_Value_Output vo_peak_torque {
        label {Messwert [Nm]}
        xywh {1110 503 145 60} align 5 step 0.01 textsize 30
      }
      Fl_Progress step_progress {
        xywh {840 654 420 25}
      }
      Fl_Button btn_result {
        xywh {775 690 486 50}
      }
      Fl_Value_Output vo_step_progress {
        label {%}
        xywh {780 654 35 25} align 8 step 1
      }
      Fl_Text_Display td_instruction {
        label Anweisung
        xywh {780 340 480 114} labelsize 18 align 5 textsize 20
      }
      Fl_Group {} {
        label {Zeitüberwachung} open
        tooltip {Mindestzeitraum nach DIN EN ISO 6789-1:2015 Kapitel 6.2.4} xywh {776 203 270 80} box GTK_UP_BOX align 5
      } {
        Fl_Round_Button rb_repeat_until_okay {
          label {Wiederholung bis normgerecht}
          xywh {791 218 245 26} type Radio down_box ROUND_DOWN_BOX value 1
        }
        Fl_Round_Button rb_ignore_timing {
          label {keine Wiederholung}
          xywh {791 248 195 25} type Radio down_box ROUND_DOWN_BOX
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
