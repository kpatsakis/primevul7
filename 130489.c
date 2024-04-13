compare_entry(PredHook hook,
	      TERMTYPE2 *tp GCC_UNUSED,
	      bool quiet)
/* compare two entries */
{
    PredIdx i, j;
    NCURSES_CONST char *name;

    if (!quiet)
	fputs("    comparing booleans.\n", stdout);
    for_each_boolean(j, tp) {
	i = BoolIndirect(j);
	name = ExtBoolname(tp, (int) i, bool_names);

	if (isObsolete(outform, name))
	    continue;

	(*hook) (CMP_BOOLEAN, i, name);
    }

    if (!quiet)
	fputs("    comparing numbers.\n", stdout);
    for_each_number(j, tp) {
	i = NumIndirect(j);
	name = ExtNumname(tp, (int) i, num_names);

	if (isObsolete(outform, name))
	    continue;

	(*hook) (CMP_NUMBER, i, name);
    }

    if (!quiet)
	fputs("    comparing strings.\n", stdout);
    for_each_string(j, tp) {
	i = StrIndirect(j);
	name = ExtStrname(tp, (int) i, str_names);

	if (isObsolete(outform, name))
	    continue;

	(*hook) (CMP_STRING, i, name);
    }

    /* (void) fputs("    comparing use entries.\n", stdout); */
    (*hook) (CMP_USE, 0, "use");

}