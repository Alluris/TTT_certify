.PHONY:clean
.PHONY:all

test_object_id22.log: do_create_sim_input.m
	octave --no-gui do_create_sim_input.m

#simple_click_min0.0_max10.0.log: create_simple_click_input.m
#	octave -q --no-gui --eval "create_simple_click_input ('simple_click', 10.0, 900);"

clean:
	rm -f *.log *.png octave-workspace
