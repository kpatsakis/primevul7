ctl_putclock(
	int id,
	struct refclockstat *pcs,
	int mustput
	)
{
	char buf[CTL_MAX_DATA_LEN];
	char *s, *t, *be;
	const char *ss;
	int i;
	const struct ctl_var *k;

	switch (id) {

	case CC_TYPE:
		if (mustput || pcs->clockdesc == NULL
		    || *(pcs->clockdesc) == '\0') {
			ctl_putuint(clock_var[id].text, pcs->type);
		}
		break;
	case CC_TIMECODE:
		ctl_putstr(clock_var[id].text,
			   pcs->p_lastcode,
			   (unsigned)pcs->lencode);
		break;

	case CC_POLL:
		ctl_putuint(clock_var[id].text, pcs->polls);
		break;

	case CC_NOREPLY:
		ctl_putuint(clock_var[id].text,
			    pcs->noresponse);
		break;

	case CC_BADFORMAT:
		ctl_putuint(clock_var[id].text,
			    pcs->badformat);
		break;

	case CC_BADDATA:
		ctl_putuint(clock_var[id].text,
			    pcs->baddata);
		break;

	case CC_FUDGETIME1:
		if (mustput || (pcs->haveflags & CLK_HAVETIME1))
			ctl_putdbl(clock_var[id].text,
				   pcs->fudgetime1 * 1e3);
		break;

	case CC_FUDGETIME2:
		if (mustput || (pcs->haveflags & CLK_HAVETIME2))
			ctl_putdbl(clock_var[id].text,
				   pcs->fudgetime2 * 1e3);
		break;

	case CC_FUDGEVAL1:
		if (mustput || (pcs->haveflags & CLK_HAVEVAL1))
			ctl_putint(clock_var[id].text,
				   pcs->fudgeval1);
		break;

	case CC_FUDGEVAL2:
		if (mustput || (pcs->haveflags & CLK_HAVEVAL2)) {
			if (pcs->fudgeval1 > 1)
				ctl_putadr(clock_var[id].text,
					   pcs->fudgeval2, NULL);
			else
				ctl_putrefid(clock_var[id].text,
					     pcs->fudgeval2);
		}
		break;

	case CC_FLAGS:
		ctl_putuint(clock_var[id].text, pcs->flags);
		break;

	case CC_DEVICE:
		if (pcs->clockdesc == NULL ||
		    *(pcs->clockdesc) == '\0') {
			if (mustput)
				ctl_putstr(clock_var[id].text,
					   "", 0);
		} else {
			ctl_putstr(clock_var[id].text,
				   pcs->clockdesc,
				   strlen(pcs->clockdesc));
		}
		break;

	case CC_VARLIST:
		s = buf;
		be = buf + sizeof(buf);
		if (strlen(clock_var[CC_VARLIST].text) + 4 >
		    sizeof(buf))
			break;	/* really long var name */

		snprintf(s, sizeof(buf), "%s=\"",
			 clock_var[CC_VARLIST].text);
		s += strlen(s);
		t = s;

		for (k = clock_var; !(EOV & k->flags); k++) {
			if (PADDING & k->flags)
				continue;

			i = strlen(k->text);
			if (s + i + 1 >= be)
				break;

			if (s != t)
				*s++ = ',';
			memcpy(s, k->text, i);
			s += i;
		}

		for (k = pcs->kv_list; k && !(EOV & k->flags); k++) {
			if (PADDING & k->flags)
				continue;

			ss = k->text;
			if (NULL == ss)
				continue;

			while (*ss && *ss != '=')
				ss++;
			i = ss - k->text;
			if (s + i + 1 >= be)
				break;

			if (s != t)
				*s++ = ',';
			memcpy(s, k->text, (unsigned)i);
			s += i;
			*s = '\0';
		}
		if (s + 2 >= be)
			break;

		*s++ = '"';
		*s = '\0';
		ctl_putdata(buf, (unsigned)(s - buf), 0);
		break;
	}
}