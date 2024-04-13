static Token *expand_mmac_params(Token * tline)
{
    Token **tail, *thead;
    bool changed = false;
    MMacro *mac = istk->mstk.mmac;

    tail = &thead;
    thead = NULL;

    while (tline) {
        bool change;
        bool err_not_mac = false;
        Token *t = tline;
        const char *text = tok_text(t);
        int type = t->type;

        tline = tline->next;
        t->next = NULL;

        switch (type) {
        case TOK_LOCAL_SYMBOL:
            change = true;

            if (!mac) {
                err_not_mac = true;
                break;
            }

            type = TOK_ID;
            text = nasm_asprintf("..@%"PRIu64".%s", mac->unique, text+2);
            break;
        case TOK_MMACRO_PARAM:
        {
            Token *tt = NULL;

            change = true;

            if (!mac) {
                err_not_mac = true;
                break;
            }

            if (strchr(text, ':')) {
                /* It is a range */
                expand_mmac_params_range(mac, t, &tail);
                text = NULL;
                break;
            }

            switch (text[1]) {
                /*
                 * We have to make a substitution of one of the
                 * forms %1, %-1, %+1, %%foo, %0, %00.
                 */
            case '0':
                if (!text[2]) {
                    type = TOK_NUMBER;
                    text = nasm_asprintf("%d", mac->nparam);
                    break;
                }
                if (text[2] != '0' || text[3])
                    goto invalid;
                /* a possible captured label == mac->params[0] */
                /* fall through */
            default:
            {
                unsigned long n;
                char *ep;

                n = strtoul(text + 1, &ep, 10);
                if (unlikely(*ep))
                    goto invalid;

                if (n <= mac->nparam) {
                    n = mmac_rotate(mac, n);
                    dup_tlistn(mac->params[n], mac->paramlen[n], &tail);
                }
                text = NULL;
                break;
            }
            case '-':
            case '+':
            {
                int cc;
                unsigned long n;
                char *ep;

                n = strtoul(tok_text(t) + 2, &ep, 10);
                if (unlikely(*ep))
                    goto invalid;

                if (n && n <= mac->nparam) {
                    n = mmac_rotate(mac, n);
                    tt = mac->params[n];
                }
                cc = find_cc(tt);
                if (cc == -1) {
                    nasm_nonfatal("macro parameter `%s' is not a condition code",
                                  tok_text(t));
                    text = NULL;
                    break;
                }

                type = TOK_ID;
                if (text[1] == '-') {
                    int ncc = inverse_ccs[cc];
                    if (unlikely(ncc == -1)) {
                        nasm_nonfatal("condition code `%s' is not invertible",
                                      conditions[cc]);
                        break;
                    }
                    cc = ncc;
                }
                text = nasm_strdup(conditions[cc]);
                break;
            }

            invalid:
                nasm_nonfatal("invalid macro parameter: `%s'", text);
                text = NULL;
                break;
            }
            break;
        }

        case TOK_PREPROC_Q:
            if (mac) {
                type = TOK_ID;
                text = nasm_strdup(mac->iname);
                change = true;
            } else {
                change = false;
            }
            break;

        case TOK_PREPROC_QQ:
            if (mac) {
                type = TOK_ID;
                text = nasm_strdup(mac->name);
                change = true;
            } else {
                change = false;
            }
            break;

        case TOK_INDIRECT:
        {
            Token *tt;

            tt = tokenize(tok_text(t));
            tt = expand_mmac_params(tt);
            tt = expand_smacro(tt);
            /* Why dup_tlist() here? We should own tt... */
            dup_tlist(tt, &tail);
            text = NULL;
            change = true;
            break;
        }

        default:
            change = false;
            break;
        }

        if (err_not_mac) {
            nasm_nonfatal("`%s': not in a macro call", text);
            text = NULL;
            change = true;
        }

        if (change) {
            if (!text) {
                delete_Token(t);
            } else {
                *tail = t;
                tail = &t->next;
		set_text(t, text, tok_strlen(text));
                t->type = type;
            }
            changed = true;
        } else {
            *tail = t;
            tail = &t->next;
        }
    }

    *tail = NULL;

    if (changed) {
        const struct tokseq_match t[] = {
            {
                PP_CONCAT_MASK(TOK_ID)          |
                PP_CONCAT_MASK(TOK_FLOAT),          /* head */
                PP_CONCAT_MASK(TOK_ID)          |
                PP_CONCAT_MASK(TOK_NUMBER)      |
                PP_CONCAT_MASK(TOK_FLOAT)       |
                PP_CONCAT_MASK(TOK_OTHER)           /* tail */
            },
            {
                PP_CONCAT_MASK(TOK_NUMBER),         /* head */
                PP_CONCAT_MASK(TOK_NUMBER)          /* tail */
            }
        };
        paste_tokens(&thead, t, ARRAY_SIZE(t), false);
    }

    return thead;
}