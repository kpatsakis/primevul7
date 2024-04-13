fstring_fix_node_location(const node *parent, node *n, char *expr_str)
{
    char *substr = NULL;
    char *start;
    int lines = LINENO(parent) - 1;
    int cols = parent->n_col_offset;
    /* Find the full fstring to fix location information in `n`. */
    while (parent && parent->n_type != STRING)
        parent = parent->n_child;
    if (parent && parent->n_str) {
        substr = strstr(parent->n_str, expr_str);
        if (substr) {
            start = substr;
            while (start > parent->n_str) {
                if (start[0] == '\n')
                    break;
                start--;
            }
            cols += substr - start;
            /* Fix lineno in mulitline strings. */
            while ((substr = strchr(substr + 1, '\n')))
                lines--;
        }
    }
    fstring_shift_node_locations(n, lines, cols);
}