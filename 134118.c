static int ppscan(void *private_data, struct tokenval *tokval)
{
    struct ppscan *pps = private_data;
    Token *tline;
    const char *txt;

    do {
	if (pps->ntokens && (tline = pps->tptr)) {
	    pps->ntokens--;
	    pps->tptr = tline->next;
	} else {
	    pps->tptr = NULL;
	    pps->ntokens = 0;
	    return tokval->t_type = TOKEN_EOS;
	}
    } while (tline->type == TOK_WHITESPACE || tline->type == TOK_COMMENT);

    txt = tok_text(tline);
    tokval->t_charptr = (char *)txt; /* Fix this */

    if (txt[0] == '$') {
	if (!txt[1]) {
	    return tokval->t_type = TOKEN_HERE;
	} else if (txt[1] == '$' && !txt[2]) {
	    return tokval->t_type = TOKEN_BASE;
	} else if (tline->type == TOK_ID) {
	    tokval->t_charptr++;
	    return tokval->t_type = TOKEN_ID;
	}
    }

    switch (tline->type) {
    default:
	if (tline->len == 1)
	    return tokval->t_type = txt[0];
	/* fall through */
    case TOK_ID:
	return nasm_token_hash(txt, tokval);

    case TOK_NUMBER:
    {
        bool rn_error;
        tokval->t_integer = readnum(txt, &rn_error);
        if (rn_error)
            return tokval->t_type = TOKEN_ERRNUM;
        else
            return tokval->t_type = TOKEN_NUM;
    }

    case TOK_FLOAT:
        return tokval->t_type = TOKEN_FLOAT;

    case TOK_STRING:
	tokval->t_charptr = (char *)unquote_token(tline);
        tokval->t_inttwo = tline->len;
	return tokval->t_type = TOKEN_STR;
    }
}