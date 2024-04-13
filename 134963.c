SV*
Perl_invlist_clone(pTHX_ SV* const invlist, SV* new_invlist)
{
    /* Return a new inversion list that is a copy of the input one, which is
     * unchanged.  The new list will not be mortal even if the old one was. */

    const STRLEN nominal_length = _invlist_len(invlist);
    const STRLEN physical_length = SvCUR(invlist);
    const bool offset = *(get_invlist_offset_addr(invlist));

    PERL_ARGS_ASSERT_INVLIST_CLONE;

    if (new_invlist == NULL) {
        new_invlist = _new_invlist(nominal_length);
    }
    else {
        sv_upgrade(new_invlist, SVt_INVLIST);
        initialize_invlist_guts(new_invlist, nominal_length);
    }

    *(get_invlist_offset_addr(new_invlist)) = offset;
    invlist_set_len(new_invlist, nominal_length, offset);
    Copy(SvPVX(invlist), SvPVX(new_invlist), physical_length, char);

    return new_invlist;