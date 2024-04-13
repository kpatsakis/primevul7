static char *read_line(void)
{
    char *line;
    FILE *f = istk->fp;

    if (f)
        line = line_from_file(f);
    else
        line = line_from_stdmac();

    if (!line)
        return NULL;

    if (!istk->nolist)
        lfmt->line(LIST_READ, istk->where.lineno, line);

    return line;
}