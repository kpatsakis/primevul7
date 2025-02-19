hpdelim2(char **cp, char *delim)
{
	char *s, *old;

	if (cp == NULL || *cp == NULL)
		return NULL;

	old = s = *cp;
	if (*s == '[') {
		if ((s = strchr(s, ']')) == NULL)
			return NULL;
		else
			s++;
	} else if ((s = strpbrk(s, ":/")) == NULL)
		s = *cp + strlen(*cp); /* skip to end (see first case below) */

	switch (*s) {
	case '\0':
		*cp = NULL;	/* no more fields*/
		break;

	case ':':
	case '/':
		if (delim != NULL)
			*delim = *s;
		*s = '\0';	/* terminate */
		*cp = s + 1;
		break;

	default:
		return NULL;
	}

	return old;
}