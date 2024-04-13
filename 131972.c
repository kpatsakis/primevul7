deinterleave_double (STATE * state)
{	int read_len ;
	int ch, k ;

	do
	{	read_len = sf_readf_double (state->infile, state->din.d, BUFFER_LEN) ;

		for (ch = 0 ; ch < state->channels ; ch ++)
		{	for (k = 0 ; k < read_len ; k++)
				state->dout.d [k] = state->din.d [k * state->channels + ch] ;
			sf_write_double (state->outfile [ch], state->dout.d, read_len) ;
			} ;
		}
	while (read_len > 0) ;

} /* deinterleave_double */