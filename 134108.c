static char *pp_getline(void)
{
    char *line = NULL;
    Token *tline;

    while (true) {
        tline = pp_tokline();
        if (tline == &tok_pop) {
            /*
             * We popped the macro/include stack. If istk is empty,
             * we are at end of input, otherwise just loop back.
             */
            if (!istk)
                break;
        } else {
            /*
             * De-tokenize the line and emit it.
             */
            line = detoken(tline, true);
            free_tlist(tline);
            break;
        }
    }

    if (list_option('e') && istk && !istk->nolist && line && line[0]) {
        char *buf = nasm_strcat(" ;;; ", line);
        lfmt->line(LIST_MACRO, -1, buf);
        nasm_free(buf);
    }

    return line;
}