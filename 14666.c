vdollar_percent_expand(int *parseerror, int dollar, int percent,
    const char *string, va_list ap)
{
#define EXPAND_MAX_KEYS	16
	u_int num_keys = 0, i;
	struct {
		const char *key;
		const char *repl;
	} keys[EXPAND_MAX_KEYS];
	struct sshbuf *buf;
	int r, missingvar = 0;
	char *ret = NULL, *var, *varend, *val;
	size_t len;

	if ((buf = sshbuf_new()) == NULL)
		fatal_f("sshbuf_new failed");
	if (parseerror == NULL)
		fatal_f("null parseerror arg");
	*parseerror = 1;

	/* Gather keys if we're doing percent expansion. */
	if (percent) {
		for (num_keys = 0; num_keys < EXPAND_MAX_KEYS; num_keys++) {
			keys[num_keys].key = va_arg(ap, char *);
			if (keys[num_keys].key == NULL)
				break;
			keys[num_keys].repl = va_arg(ap, char *);
			if (keys[num_keys].repl == NULL) {
				fatal_f("NULL replacement for token %s",
				    keys[num_keys].key);
			}
		}
		if (num_keys == EXPAND_MAX_KEYS && va_arg(ap, char *) != NULL)
			fatal_f("too many keys");
		if (num_keys == 0)
			fatal_f("percent expansion without token list");
	}

	/* Expand string */
	for (i = 0; *string != '\0'; string++) {
		/* Optionally process ${ENVIRONMENT} expansions. */
		if (dollar && string[0] == '$' && string[1] == '{') {
			string += 2;  /* skip over '${' */
			if ((varend = strchr(string, '}')) == NULL) {
				error_f("environment variable '%s' missing "
				    "closing '}'", string);
				goto out;
			}
			len = varend - string;
			if (len == 0) {
				error_f("zero-length environment variable");
				goto out;
			}
			var = xmalloc(len + 1);
			(void)strlcpy(var, string, len + 1);
			if ((val = getenv(var)) == NULL) {
				error_f("env var ${%s} has no value", var);
				missingvar = 1;
			} else {
				debug3_f("expand ${%s} -> '%s'", var, val);
				if ((r = sshbuf_put(buf, val, strlen(val))) !=0)
					fatal_fr(r, "sshbuf_put ${}");
			}
			free(var);
			string += len;
			continue;
		}

		/*
		 * Process percent expansions if we have a list of TOKENs.
		 * If we're not doing percent expansion everything just gets
		 * appended here.
		 */
		if (*string != '%' || !percent) {
 append:
			if ((r = sshbuf_put_u8(buf, *string)) != 0)
				fatal_fr(r, "sshbuf_put_u8 %%");
			continue;
		}
		string++;
		/* %% case */
		if (*string == '%')
			goto append;
		if (*string == '\0') {
			error_f("invalid format");
			goto out;
		}
		for (i = 0; i < num_keys; i++) {
			if (strchr(keys[i].key, *string) != NULL) {
				if ((r = sshbuf_put(buf, keys[i].repl,
				    strlen(keys[i].repl))) != 0)
					fatal_fr(r, "sshbuf_put %%-repl");
				break;
			}
		}
		if (i >= num_keys) {
			error_f("unknown key %%%c", *string);
			goto out;
		}
	}
	if (!missingvar && (ret = sshbuf_dup_string(buf)) == NULL)
		fatal_f("sshbuf_dup_string failed");
	*parseerror = 0;
 out:
	sshbuf_free(buf);
	return *parseerror ? NULL : ret;
#undef EXPAND_MAX_KEYS
}