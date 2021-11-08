## Diverse simulierte Profile erstellen

to.id = 6; ## ("FAKE", "ACME", "dummy1", "I",   "A", 0, 0.32, 2, 10, 0.1, "", 0.00), --rechts/links, zulässige Abweichung aus der DIN
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "A";
to.uncertainty = 0.06;
data1 = create_sim_input (tt, to, 0, 0);
to.min_torque = -2;
to.max_torque = -10;
data2 = create_sim_input (tt, to, 0, 0);
data = [data1; data2];
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 7; ## ("FAKE", "ACME", "dummy2", "I",   "A", 0, 0.32, 2, 10, 0.1, "", 0.02), --rechts/links, zulässige Abweichung vom Hersteller
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "A";
to.uncertainty = 0.02;
data1 = create_sim_input (tt, to, 0, 0);
to.min_torque = -2;
to.max_torque = -10;
data2 = create_sim_input (tt, to, 0, 0);
data = [data1; data2];
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 8; ## ("FAKE", "ACME", "dummy3", "I",   "A", 1, 0.32, 2, 10, 0.1, "", 0.00), --nur rechts
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "A";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 9; ##("FAKE", "ACME", "dummy4", "I",   "A", 2, 0.32, 2, 10, 0.1, "", 0.00), --nur links
tt.max_torque = 10;
to.min_torque = -2;
to.max_torque = -10;
to.type = "I";
to.class = "A";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 10; ##("FAKE", "ACME", "dummy5", "I",   "B", 1, 0.32, 2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "B";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 11; ## ("FAKE", "ACME", "dummy6", "I",   "C", 1, 0.32, 2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "C";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 12; ## ("FAKE", "ACME", "dummy7", "I",   "D", 1, 0,    2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "D";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 13; ## ("FAKE", "ACME", "dummy8", "I",   "E", 1, 0,    2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "I";
to.class = "E";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 14; ##("FAKE", "ACME", "dummy9", "II",  "A", 1, 0.32, 2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "A";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 15; ##("FAKE", "ACME", "dummy10", "II",  "B", 1, 0.32, 2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "B";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 16; ## ("FAKE", "ACME", "dummy11", "II",  "C", 1, 0.32, 2, 10, 0.1, "", 0.02),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "C";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 17; ## ("FAKE", "ACME", "dummy12", "II",  "D", 1, 0,    2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "D";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 18; ## ("FAKE", "ACME", "dummy13", "II", "E", 1, 0,    2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "E";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 19; ## ("FAKE", "ACME", "dummy14", "II", "F", 1, 0,    2, 10, 0.1, "", 0.00),
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "F";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
data = create_sim_input (tt, to, 0, 1);
fn = sprintf ("test_object_id%i_bad_risetime", to.id);
save_sim_input (fn, data);
clear all

to.id = 20; ## ("FAKE", "ACME", "dummy15", "II", "G", 1, 0.32, 2, 10, 0.1, "", 0.00);
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "G";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 21; ## ("FAKE", "ACME", "dummy16", "II", "A", 2, 0.32, 2, 10, 0.1, "", 0.00), -- nur links
tt.max_torque = 10;
to.min_torque = -2;
to.max_torque = -10;
to.type = "II";
to.class = "A";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 22; ## ("FAKE", "ACME", "dummy17", "II", "A", 2, 0.32, 0, 10, 0.1, "", 0.00), -- rechts/links
tt.max_torque = 10;
to.min_torque = 2;
to.max_torque = 10;
to.type = "II";
to.class = "A";
to.uncertainty = 0.06;
data1 = create_sim_input (tt, to, 0, 0);
to.min_torque = -2;
to.max_torque = -10;
data2 = create_sim_input (tt, to, 0, 0);
data = [data1; data2];
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all

to.id = 24; ## (1, "A24", "FAKE24", "ACME", "dummy24", "II",  "A", 1, 0.15, 1.0, 2.0, 0.02, "xyz foo bar", 0.00, 0.6);
tt.max_torque = 2;
to.min_torque = 1;
to.max_torque = 2;
to.type = "II";
to.class = "A";
to.uncertainty = 0.06;
data = create_sim_input (tt, to, 0, 0);
fn = sprintf ("test_object_id%i", to.id);
save_sim_input (fn, data);
clear all
