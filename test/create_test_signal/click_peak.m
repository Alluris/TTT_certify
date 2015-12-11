# len in seconds until click
# second peak has approx 1/5 len width
# delay for 2.2s after click
function y = click_peak (len, amp_peak1, amp_peak2, FS = 900)
  y1 = sin_peak (2.2 * len, 1, FS);
  ind1 = 1:round(len * FS);
  y1 = y1(ind1');
  ## scale to amp_peak1
  y1 *= amp_peak1/max(y1);

  y2 = sin_peak (0.2* len, amp_peak2, FS);
  ind2 = round(rows(y2)*0.15):rows(y2);
  y2 = y2(ind2');

  # linearer Abfall nach erstem Peak
  t_decline = 0.05;
  y_decline = interp1 ([0, t_decline], [y1(end), y2(1)], linspace (0, t_decline, FS * t_decline)');

  y = [y1; y_decline; y2];
  y = [y, zeros(rows(y), 1)];

  delay_len = 2.2 - ((rows (y2) + rows (y_decline)) / FS);
  y = [y; delay(delay_len, FS)];

endfunction

%!demo
%! y = click_peak (3, 1, 1.2);
%! t = linspace (0, rows(y)/900, rows(y));
%! plot (t, y)
%! hold on
%! line ([t(end), t(end)], [0, 1]);
%! hold off
