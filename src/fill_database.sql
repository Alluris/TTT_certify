/*
  18.11.2015 Andreas Weber
  TTT Datenbank und Tabellen mit einigen dummy Werten füllen

  https://www.sqlite.org/docs.html
*/

INSERT INTO test_person(name, supervisor, uncertainty) VALUES ("Daniel Betz", "master of desaster", 0.005);
INSERT INTO test_person(name, supervisor, uncertainty) VALUES ("Andreas Weber", "foo", 0.001);
INSERT INTO test_person(name, supervisor, uncertainty) VALUES ("Axel Fischer", "bar", 0.01);

INSERT INTO test_object (active,
                         equipment_number,
                         serial_number,
                         manufacturer,
                         model,
                         DIN_type,
                         DIN_class,
                         dir_of_rotation,
                         lever_length,
                         min_torque,
                         max_torque,
                         resolution,
                         attachments,
                         accuracy)
       VALUES (1, "X1",    "12345", "Stahlwille", "730N/2", "II", "A", 1, 0.226, 2, 10, 0.2, "6/8 auf 1/4''", 0.0),  -- max stimmt nicht, ist 20Nm
              (1, "B1",    "S44780", "PROXXON", "MicroClick MC 320", "II", "A", 1, 0.6, 60, 320, 1, "Nuss123", 0.03), --http://www.proxxon.com/de/industrial/23354.php?list
              (1, "C1",    "XZK44", "PROXXON", "MicroClick MC 5", "II", "D", 1, 0, 1, 5, 0.1, "", 0.06),              --http://www.proxxon.com/de/industrial/23347.php?list
              (1, "12x k", "9982", "ZEMO", "M-34 Nm R855285", "I", "A", 1, 0.3429, 7, 34, 1, "Aufsatz B", 0.02),      --http://www.zemo-tools.de/files/pdf/drehmomentschluessel_m.pdf
              (1, "9-34",  "2345", "GEDORE", "Typ83", "I", "B", 1, 0.2, 0.8, 4, 0.1, "ein Adapter", 0.03),            --http://www.zorotools.de/i/product/Z00047453/?gclid=CIfI8_T9y8kCFRcYGwodRfIKFg
              (1, "A1",  "FAKE1", "ACME", "dummy6", "I",   "A", 0, 0.32, 2, 10, 0.1, "", 0.00), --rechts/links, zulässige Abweichung aus der DIN
              (1, "A2",  "FAKE2", "ACME", "dummy7", "I",   "A", 0, 0.32, 2, 10, 0.1, "", 0.02), --rechts/links, zulässige Abweichung vom Hersteller
              (1, "A3",  "FAKE3", "ACME", "dummy8", "I",   "A", 1, 0.32, 2, 10, 0.1, "", 0.00), --nur rechts
              (1, "A4",  "FAKE4", "ACME", "dummy9", "I",   "A", 2, 0.32, 2, 10, 0.1, "", 0.00), --nur links
              (1, "A5",  "FAKE5", "ACME", "dummy10", "I",   "B", 1, 0.32, 2, 10, 0.1, "", 0.00),
              (1, "A6",  "FAKE6", "ACME", "dummy11", "I",   "C", 1, 0.32, 2, 10, 0.1, "", 0.00),
              (1, "A7",  "FAKE7", "ACME", "dummy12", "I",   "D", 1, 0,    2, 10, 0.1, "", 0.00),
              (1, "A8",  "FAKE8", "ACME", "dummy13", "I",   "E", 1, 0,    2, 10, 0.1, "", 0.00),
              (1, "A9",  "FAKE9", "ACME", "dummy14", "II",  "A", 1, 0.32, 2, 10, 0.1, "", 0.00),
              (1, "A10", "FAKE10", "ACME", "dummy15", "II",  "B", 1, 0.32, 2, 10, 0.1, "", 0.00),
              (1, "A11", "FAKE11", "ACME", "dummy16", "II",  "C", 1, 0.32, 2, 10, 0.1, "", 0.00),
              (1, "A12", "FAKE12", "ACME", "dummy17", "II",  "D", 1, 0,    2, 10, 0.1, "", 0.00),
              (1, "A13", "FAKE13", "ACME", "dummy18", "II", "E", 1, 0,    2, 10, 0.1, "", 0.00),
              (1, "A14", "FAKE14", "ACME", "dummy19", "II", "F", 1, 0,    2, 10, 0.1, "", 0.00),
              (1, "A15", "FAKE15", "ACME", "dummy20", "II", "G", 1, 0.32, 2, 10, 0.1, "", 0.00),
              (1, "A16", "FAKE16", "ACME", "dummy21", "II", "A", 2, 0.32, 2, 10, 0.1, "", 0.00),  --nur links
              (1, "A17", "FAKE17", "ACME", "dummy22", "II", "A", 0, 0.32, 2, 10, 0.1, "", 0.00);  --rechts/links

INSERT INTO torque_tester (serial_number,
                         manufacturer,
                         model,
                         next_calibration_date,
                         calibration_date,
                         calibration_number,
                         max_torque,
                         resolution,
                         uncertainty_of_measurement)
       VALUES ("L.12345", "Alluris GmbH & Co. KG", "TTT", "2017-01", "2000-05-03", "calnumber fillme", 10, 0.01, 0.005);

INSERT INTO measurement (norm,
                         test_person_id,
                         test_object_id,
                         torque_tester_id,
                         start_time,
                         end_time,
                         raw_data_filename,
                         temperature,
                         humidity)
       VALUES ("ISO 6789", 1, 1, 1, "2015-11-17 10:01:02", "2015-11-17 10:18:22", "foo.bar", 21.34, 56.78),
              ("ISO 6789-1", 1, 16, 1, "2015-11-17 11:01:02", "2015-11-17 11:18:22", "foo.bar", 21.34, 66.78);

INSERT INTO measurement_item ( ts,
                               measurement,
                               nominal_value,
                               indicated_value,
                               rise_time)
       VALUES ("2015-11-17 11:01:02", 1, -2, -2.15, 0.4),
              ("2015-11-17 11:01:05", 1, -2, -2.23, 0.7),
              ("2015-11-17 11:01:08", 1, -2, -1.98, 0.55),
              ("2015-11-17 11:01:09", 1, -2, -2.14, 0.6),
              ("2015-11-17 11:01:10", 1, -2, -1.95, 1.2),

              ("2015-11-17 11:01:21", 1, -6, -6.15, 0.52),
              ("2015-11-17 11:01:22", 1, -6, -6.16, 1.1),
              ("2015-11-17 11:01:23", 1, -6, -6.11, 1.2),
              ("2015-11-17 11:01:24", 1, -6, -5.98, 0.6),
              ("2015-11-17 11:01:25", 1, -6, -6.02, 1.0),

              ("2015-11-17 11:01:41", 1, -10, -10.05, 1),
              ("2015-11-17 11:01:42", 1, -10, -10.06, 1),
              ("2015-11-17 11:01:43", 1, -10, -9.91, 1),
              ("2015-11-17 11:01:44", 1, -10, -10.02, 1),
              ("2015-11-17 11:01:45", 1, -10, -10.58, 1),

              ("2015-11-17 10:01:02", 1, 2, 1.95, 1),
              ("2015-11-17 10:01:05", 1, 2, 2.23, 1),
              ("2015-11-17 10:01:08", 1, 2, 2.11, 1),
              ("2015-11-17 10:01:09", 1, 2, 2.14, 1),
              ("2015-11-17 10:01:10", 1, 2, 2.01, 1),

              ("2015-11-17 10:01:21", 1, 6, 6.05, 1),
              ("2015-11-17 10:01:22", 1, 6, 6.06, 1),
              ("2015-11-17 10:01:23", 1, 6, 6.01, 1),
              ("2015-11-17 10:01:24", 1, 6, 5.98, 1),
              ("2015-11-17 10:01:25", 1, 6, 6.08, 1),

              ("2015-11-17 10:01:41", 1, 10, 10.05, 0.5),
              ("2015-11-17 10:01:42", 1, 10, 10.06, 1.1),
              ("2015-11-17 10:01:43", 1, 10, 9.97, 0.8),
              ("2015-11-17 10:01:44", 1, 10, 10.02, 1.2),
              ("2015-11-17 10:01:45", 1, 10, 10.58, 1.3),

              ("2015-11-17 11:01:21", 2, 5, 6.05, 0.42),
              ("2015-11-17 11:01:22", 2, 5, 6.06, 1.1),
              ("2015-11-17 11:01:23", 2, 5, 6.01, 1.2),
              ("2015-11-17 11:01:24", 2, 5, 5.98, 0.6),
              ("2015-11-17 11:01:25", 2, 5, 6.02, 1.0),
              ("2015-11-17 11:01:27", 2, 5, 6.05, 0.52),
              ("2015-11-17 11:01:28", 2, 5, 6.06, 1.1),
              ("2015-11-17 11:01:30", 2, 5, 6.01, 1.2),
              ("2015-11-17 11:01:32", 2, 5, 5.99, 0.6),
              ("2015-11-17 11:01:35", 2, 5, 6.23, 1.0);

--              (datetime (), 1, 40, 44),
--              (datetime (), 1, 12, 22);
