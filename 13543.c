log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    if (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}