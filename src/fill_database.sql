/*
  18.05.2016 Andreas Weber
  TTT Datenbank und Tabellen mit einigen dummy Werten füllen

  https://www.sqlite.org/docs.html
*/

INSERT INTO test_person(name, supervisor, uncertainty) VALUES ("Max Mustermann", "John Doe", 0.001);

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
                         accuracy,
                         peak_trigger2_factor)
       VALUES (1, "X1",    "12345", "Stahlwille", "730N/2", "II", "A", 1, 0.226, 2, 10, 0.2, "6/8 auf 1/4''", 0.0, 0.8),  -- max stimmt nicht, ist 20Nm
              (1, "B1",    "S44780", "PROXXON", "MicroClick MC 320", "II", "A", 1, 0.6, 60, 320, 1, "Nuss123", 0.03, 0.7), --http://www.proxxon.com/de/industrial/23354.php?list
              (1, "C1",    "XZK44", "PROXXON", "MicroClick MC 5", "II", "D", 1, 0, 1, 5, 0.1, "", 0.06, 0.8),              --http://www.proxxon.com/de/industrial/23347.php?list
              (1, "12x k", "9982", "ZEMO", "M-34 Nm R855285", "I", "A", 1, 0.3429, 7, 34, 1, "Aufsatz B", 0.02, 0.8),      --http://www.zemo-tools.de/files/pdf/drehmomentschluessel_m.pdf
              (1, "9-34",  "2345", "GEDORE", "Typ83", "I", "B", 1, 0.2, 0.8, 4, 0.1, "ein Adapter", 0.03, 0.8),            --http://www.zorotools.de/i/product/Z00047453/?gclid=CIfI8_T9y8kCFRcYGwodRfIKFg
              (1, "123",   "SN15-492265", "Garant", "65 6050_6", "II", "A", 0, 0.155, 1, 6, 0.1, "", 0.00, 0.8);  --rechts/links
