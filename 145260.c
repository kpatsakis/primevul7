tokenize(
	const char *line,
	char **tokens,
	int *ntok
	)
{
	register const char *cp;
	register char *sp;
	static char tspace[MAXLINE];

	sp = tspace;
	cp = line;
	for (*ntok = 0; *ntok < MAXTOKENS; (*ntok)++) {
		tokens[*ntok] = sp;

		/* Skip inter-token whitespace */
		while (ISSPACE(*cp))
		    cp++;

		/* If we're at EOL we're done */
		if (ISEOL(*cp))
		    break;

		/* If this is the 2nd token and the first token begins
		 * with a ':', then just grab to EOL.
		 */

		if (*ntok == 1 && tokens[0][0] == ':') {
			do {
				if (sp - tspace >= MAXLINE)
					goto toobig;
				*sp++ = *cp++;
			} while (!ISEOL(*cp));
		}

		/* Check if this token begins with a double quote.
		 * If yes, continue reading till the next double quote
		 */
		else if (*cp == '\"') {
			++cp;
			do {
				if (sp - tspace >= MAXLINE)
					goto toobig;
				*sp++ = *cp++;
			} while ((*cp != '\"') && !ISEOL(*cp));
			/* HMS: a missing closing " should be an error */
		}
		else {
			do {
				if (sp - tspace >= MAXLINE)
					goto toobig;
				*sp++ = *cp++;
			} while ((*cp != '\"') && !ISSPACE(*cp) && !ISEOL(*cp));
			/* HMS: Why check for a " in the previous line? */
		}

		if (sp - tspace >= MAXLINE)
			goto toobig;
		*sp++ = '\0';
	}
	return;

  toobig:
	*ntok = 0;
	fprintf(stderr,
		"***Line `%s' is too big\n",
		line);
	return;
}