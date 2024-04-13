static char *line_from_file(FILE *f)
{
    int c;
    unsigned int size, next;
    const unsigned int delta = 512;
    const unsigned int pad = 8;
    bool cont = false;
    char *buffer, *p;

    istk->where.lineno += istk->lineskip + istk->lineinc;
    src_set_linnum(istk->where.lineno);
    istk->lineskip = 0;

    size = delta;
    p = buffer = nasm_malloc(size);

    do {
        c = fgetc(f);

        switch (c) {
        case EOF:
            if (p == buffer) {
                nasm_free(buffer);
                return NULL;
            }
            c = 0;
            break;

        case '\r':
            next = fgetc(f);
            if (next != '\n')
                ungetc(next, f);
            if (cont) {
                cont = false;
                continue;
            }
            c = 0;
            break;

        case '\n':
            if (cont) {
                cont = false;
                continue;
            }
            c = 0;
            break;

        case 032:               /* ^Z = legacy MS-DOS end of file mark */
            c = 0;
            break;

        case '\\':
            next = fgetc(f);
            ungetc(next, f);
            if (next == '\r' || next == '\n') {
                cont = true;
                istk->lineskip += istk->lineinc;
                continue;
            }
            break;
        }

        if (p >= (buffer + size - pad)) {
            buffer = nasm_realloc(buffer, size + delta);
            p = buffer + size - pad;
            size += delta;
        }

        *p++ = c;
    } while (c);

    return buffer;
}