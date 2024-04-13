SV*
Perl__new_invlist(pTHX_ IV initial_size)
{

    /* Return a pointer to a newly constructed inversion list, with enough
     * space to store 'initial_size' elements.  If that number is negative, a
     * system default is used instead */

    SV* new_list;

    if (initial_size < 0) {
	initial_size = 10;
    }

    new_list = newSV_type(SVt_INVLIST);
    initialize_invlist_guts(new_list, initial_size);

    return new_list;