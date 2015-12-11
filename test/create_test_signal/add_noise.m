function y = add_noise (y, amp)

  y(:, 1) += amp * randn (rows (y), 1);

endfunction
