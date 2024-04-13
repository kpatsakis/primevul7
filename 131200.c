spell_valid_case(
    int	    wordflags,	    // flags for the checked word.
    int	    treeflags)	    // flags for the word in the spell tree
{
    return ((wordflags == WF_ALLCAP && (treeflags & WF_FIXCAP) == 0)
	    || ((treeflags & (WF_ALLCAP | WF_KEEPCAP)) == 0
		&& ((treeflags & WF_ONECAP) == 0
					   || (wordflags & WF_ONECAP) != 0)));
}