usage_exit (void)
{	puts ("\nUsage : sndfile-deinterleave <filename>\n") ;
	puts (
		"Split a mutli-channel file into a set of mono files.\n"
		"\n"
		"If the input file is named 'a.wav', the output files will be named\n"
		"a_00.wav, a_01.wav and so on.\n"
		) ;
	printf ("Using %s.\n\n", sf_version_string ()) ;
	exit (1) ;
} /* usage_exit */