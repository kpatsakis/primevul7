ex_finish(exarg_T *eap)
{
    if (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    else
	emsg(_(e_finish_used_outside_of_sourced_file));
}