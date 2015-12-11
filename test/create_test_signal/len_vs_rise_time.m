## click_peak len vs. rise_time

l = linspace (0.1, 5, 20)';

t = zeros (numel (l), 1);
for k=1:numel(l)

  data = click_peak (l(k), 1.2, 1.456, 900);
  t(k) = get_rise_time (data(:,1));

endfor

p = polyfit (t, l, 1)
plot (l, t);
grid on
