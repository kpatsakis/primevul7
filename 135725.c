static void run_external_diff(const char *pgm,
			      const char *name,
			      const char *other,
			      struct diff_filespec *one,
			      struct diff_filespec *two,
			      const char *xfrm_msg,
			      int complete_rewrite)
{
	const char *spawn_arg[10];
	struct diff_tempfile *temp = diff_temp;
	int retval;
	static int atexit_asked = 0;
	const char *othername;
	const char **arg = &spawn_arg[0];

	othername = (other? other : name);
	if (one && two) {
		prepare_temp_file(name, &temp[0], one);
		prepare_temp_file(othername, &temp[1], two);
		if (! atexit_asked &&
		    (temp[0].name == temp[0].tmp_path ||
		     temp[1].name == temp[1].tmp_path)) {
			atexit_asked = 1;
			atexit(remove_tempfile);
		}
		signal(SIGINT, remove_tempfile_on_signal);
	}

	if (one && two) {
		*arg++ = pgm;
		*arg++ = name;
		*arg++ = temp[0].name;
		*arg++ = temp[0].hex;
		*arg++ = temp[0].mode;
		*arg++ = temp[1].name;
		*arg++ = temp[1].hex;
		*arg++ = temp[1].mode;
		if (other) {
			*arg++ = other;
			*arg++ = xfrm_msg;
		}
	} else {
		*arg++ = pgm;
		*arg++ = name;
	}
	*arg = NULL;
	fflush(NULL);
	retval = run_command_v_opt(spawn_arg, 0);
	remove_tempfile();
	if (retval) {
		fprintf(stderr, "external diff died, stopping at %s.\n", name);
		exit(1);
	}
}