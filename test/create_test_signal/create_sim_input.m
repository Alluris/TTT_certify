## Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>
##
## Create simulation input
##
## This file is part of TTT_certify.
##
## TTT_certify is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## TTT_certify is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  See COPYING
## If not, see <http://www.gnu.org/licenses/>.

function data = create_sim_input (tt, to, bad_deviation, bad_rise_time)

  if (bad_deviation)
    to.uncertainty *= 2;
  endif

  FS = 900;
  data = zeros ([], 2);

  ## 5s Pause "Drehmoment-Schraubwerkzeug entfernen und Messgerät für Tara nicht bewegen"
  data = [data; delay(5.5,FS)];

  ## 3 Vorbelastungen mit dem Höchstwert des Messgeräts
  if (to.min_torque < 0 && tt.max_torque > 0)
    tt.max_torque *= -1;
  endif
  data = [data; sin_peak(2,   1.20 * tt.max_torque, FS)];
  data = [data; sin_peak(1.5, 1.05 * tt.max_torque, FS)];
  data = [data; sin_peak(2.5, 1.10 * tt.max_torque, FS)];

  ## 5s Pause für tara Messgerät
  data = [data; delay(5.5,FS)];

  ## 0.2s Tara bestätigen
  data = [data; confirmation(0.2,FS)];

  ## Durchgang mit min, 60% und 100%
  if (has_no_scale (to))
    v_torque = to.max_torque;
  else
    v_torque = [to.min_torque, 0.6 * to.max_torque, to.max_torque];
  endif

  for torque = v_torque

    ## 3 Vorbelastungen des Drehmoment-Schraubwerkzeugs
    tmp = torque * [1-to.uncertainty, 1, 1+to.uncertainty];
    for k=1:3
      data = [data; sin_peak(2,tmp(k),FS)];
    endfor

    ## 7s Pause für Prüfling Tara
    data = [data; delay(7,FS)];

    ## 0.2s Prüfling Tara bestätigen
    data = [data; confirmation(0.2,FS)];

    ## 1s Pause
    data = [data; delay(1,FS)];

    ## 5/10 Belastungen
    num_peaks = 5;
    if (has_no_scale (to))
      num_peaks = 10;
    endif

    ## Die IIC und IIF werden beim Maximum vorbelastet aber beim unteren Grenzwert gemessen
    if (has_no_scale (to) && !has_fixed_trigger (to))
      torque = to.min_torque;
    endif

    tmp = torque * linspace (1-to.uncertainty*0.98, 1+to.uncertainty*0.98, num_peaks);
    for k=1:num_peaks

      if (strcmp (to.type, "II"))
        ## FIXME: rt_max bei Schraubendrehern noch nicht getestet
        [rt_min, rt_max] = DIN_rise_time (torque, to);

        if (bad_rise_time)
          rt_min *= 0.8;
        else
          rt_min *= 1.1;
        endif

        len = polyval ([4.18973407921475 0.00450378387264098], rt_min);
        tdata = click_peak(len, tmp(k), 1.2*tmp(k), FS);
        #printf ("rise_time = %f\n", get_rise_time (tdata (:, 1)));
        data = [data; tdata];
      else
        data = [data; sin_peak(2, tmp(k), FS)];
      endif

      ## 1.5s Pause
      data = [data; delay(1.5,FS)];
    endfor

    ## 3s Pause
    data = [data; delay(3,FS)];

  endfor

endfunction




