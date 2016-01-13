/*
  18.11.2015 Andreas Weber
  TTT Datenbank und Tabellen anlegen

  https://www.sqlite.org/docs.html

*/

DROP TABLE IF EXISTS test_person;
DROP TABLE IF EXISTS test_object;
DROP TABLE IF EXISTS torque_tester;
DROP TABLE IF EXISTS measurement;
DROP TABLE IF EXISTS measurement_item;

PRAGMA foreign_keys = ON;

---------------------------------------------------------------------------

CREATE TABLE test_person (id INTEGER PRIMARY KEY,
                          name TEXT,
                          supervisor TEXT,
                          uncertainty REAL);

---------------------------------------------------------------------------

CREATE TABLE test_object (id INTEGER PRIMARY KEY,
                          equipment_number TEXT NOT NULL UNIQUE, -- Prüfmittelnummer
                          serial_number TEXT,
                          manufacturer TEXT,
                          model TEXT,
                          DIN_type TEXT,             -- I oder II
                          DIN_class TEXT,            -- A, B, C, D, E, F, G
                          dir_of_rotation INTEGER,   -- 0 = beide, 1 = nur rechtsdrehend, 2 = nur linksdrehend
                          lever_length REAL,         -- Kapitel 7f und 8e: Maß von der Messachse bis zur Mitte des Habndhaltebereichs
                                                     -- des Griffs oder des markierten Lastangriffspunkts, es sei denn, der Kraftangriffspunkt ist markiert.
                                                     -- in deutsch Lastangriffspunkt
                          min_torque REAL,
                          max_torque REAL,
                          resolution REAL,           -- r siehe 6789-2 6.2.1
                          attachments TEXT,          -- Anbauteile
                          accuracy REAL              -- relativ. 0 = aus der DIN6789, sonst Herstellerangabe
                          );

---------------------------------------------------------------------------
-- the TTT (tool torque tester)
-- a new entry is generated if the calibration date changes
-- so the old calibration certs keep their cal-date

CREATE TABLE torque_tester (id INTEGER PRIMARY KEY,
                          serial_number TEXT,
                          manufacturer TEXT,
                          model TEXT,
                          next_calibration_date TEXT,
                          calibration_date TEXT,
                          calibration_number TEXT,
                          max_torque REAL,
                          resolution REAL,
                          uncertainty_of_measurement REAL);

---------------------------------------------------------------------------

CREATE TABLE measurement (id INTEGER PRIMARY KEY,
                          test_person_id INTEGER,
                          test_object_id INTEGER,
                          torque_tester_id INTEGER,
                          start_time TEXT,
                          end_time TEXT,
                          raw_data_filename TEXT,
                          temperature REAL,   -- [°C]
                          humidity REAL,     -- [%rH]
                          FOREIGN KEY(test_person_id) REFERENCES test_person(id),
                          FOREIGN KEY(test_object_id) REFERENCES test_object(id),
                          FOREIGN KEY(torque_tester_id) REFERENCES torque_tester(id)
                          );

---------------------------------------------------------------------------

CREATE TABLE measurement_item ( id INTEGER PRIMARY KEY,
                                ts TEXT,                -- timestamp
                                measurement INTEGER,
                                nominal_value REAL,
                                indicated_value REAL,
                                rise_time REAL,         -- Only TypII. From 80% to 100%, see DIN 6789-1 chapter 6.2.4
                                FOREIGN KEY(measurement) REFERENCES measurement(id)
                                );
