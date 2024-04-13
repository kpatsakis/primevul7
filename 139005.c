format_event (session *sess, int index, char **args, char *o, gsize sizeofo, unsigned int stripcolor_args)
{
	int len, ii, numargs;
	gsize oi;
	char *i, *ar, d, a, done_all = FALSE;

	i = pntevts[index];
	numargs = te[index].num_args & 0x7f;

	oi = ii = len = d = a = 0;
	o[0] = 0;

	if (i == NULL)
		return;

	while (done_all == FALSE)
	{
		d = i[ii++];
		switch (d)
		{
		case 0:
			memcpy (&len, &(i[ii]), sizeof (int));
			ii += sizeof (int);
			if (oi + len > sizeofo)
			{
				printf ("Overflow in display_event (%s)\n", i);
				o[0] = 0;
				return;
			}
			memcpy (&(o[oi]), &(i[ii]), len);
			oi += len;
			ii += len;
			break;
		case 1:
			a = i[ii++];
			if (a > numargs)
			{
				fprintf (stderr,
							"HexChat DEBUG: display_event: arg > numargs (%d %d %s)\n",
							a, numargs, i);
				break;
			}
			ar = args[(int) a + 1];
			if (ar == NULL)
			{
				printf ("arg[%d] is NULL in print event\n", a + 1);
			} else
			{
				if (strlen (ar) > sizeofo - oi - 4)
					ar[sizeofo - oi - 4] = 0;	/* Avoid buffer overflow */
				if (stripcolor_args & ARG_FLAG(a + 1)) len = strip_color2 (ar, -1, &o[oi], STRIP_ALL);
				else len = strip_hidden_attribute (ar, &o[oi]);
				oi += len;
			}
			break;
		case 2:
			o[oi++] = '\n';
			o[oi++] = 0;
			done_all = TRUE;
			continue;
		case 3:
			if (prefs.hex_text_indent)
				o[oi++] = '\t';
			else
				o[oi++] = ' ';
			break;
		}
	}
	o[oi] = 0;
	if (*o == '\n')
		o[0] = 0;
}