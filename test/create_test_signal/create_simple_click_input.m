## Einfachen input für Typ II
## (ohne mehrmaliger Vorbelastung, tara usw.

## passendes Testprogramm


function data = create_simple_click_input (type_class, torque, FS)

  data = zeros ([], 2);

  ## 1 Vorbelastung Messgerät
  data = [data; sin_peak(2, torque, FS)];

  ## 1 Vorbelastung Test object
  data = [data; sin_peak(2, torque, FS)];

  ## click peak und 1.5s pause
  data = [data; click_peak(3, torque, 1.2*torque, FS)];
  data = [data; delay(1.5,FS)];

  ## sin peak und 1.5s pause
  data = [data; sin_peak(5, torque, FS)];
  data = [data; delay(1.5,FS)];

  save_sim_input (data, type_class, 0, torque, FS);

  fn = sprintf ("%s%s_%.1f", type, class, torque);
  save_sim_input (fn, data);

endfunction
