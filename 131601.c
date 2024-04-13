static void print_canon_ace_list(const char *name, canon_ace *ace_list)
{
	int count = 0;

	if( DEBUGLVL( 10 )) {
		dbgtext( "print_canon_ace_list: %s\n", name );
		for (;ace_list; ace_list = ace_list->next, count++)
			print_canon_ace(ace_list, count );
	}
}