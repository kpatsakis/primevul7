deinterleave_int (STATE * state)
{	int read_len ;
	int ch, k ;

	do
	{	read_len = sf_readf_int (state->infile, state->din.i, BUFFER_LEN) ;

		for (ch = 0 ; ch < state->channels ; ch ++)
		{	for (k = 0 ; k < read_len ; k++)
				state->dout.i [k] = state->din.i [k * state->channels + ch] ;
			sf_write_int (state->outfile [ch], state->dout.i, read_len) ;
			} ;
		}
	while (read_len > 0) ;

} /* deinterleave_int */