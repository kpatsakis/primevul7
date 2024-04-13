make_case_word(char_u *fword, char_u *cword, int flags)
{
    if (flags & WF_ALLCAP)
	// Make it all upper-case
	allcap_copy(fword, cword);
    else if (flags & WF_ONECAP)
	// Make the first letter upper-case
	onecap_copy(fword, cword, TRUE);
    else
	// Use goodword as-is.
	STRCPY(cword, fword);
}