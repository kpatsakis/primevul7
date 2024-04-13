static void pp_error_list_macros(errflags severity)
{
    const MMacro *m;

    severity |= ERR_PP_LISTMACRO | ERR_NO_SEVERITY | ERR_HERE;

    while ((m = src_error_down())) {
        if ((m->nolist & NL_LIST) || !m->where.filename)
            break;
	nasm_error(severity, "... from macro `%s' defined", m->name);
    }

    src_error_reset();
}