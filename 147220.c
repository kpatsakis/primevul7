static void draw_cylon(char buffer[], size_t buflen, unsigned width, unsigned pos) {
        char *p = buffer;

        assert(buflen >= CYLON_BUFFER_EXTRA + width + 1);
        assert(pos <= width+1); /* 0 or width+1 mean that the center light is behind the corner */

        if (pos > 1) {
                if (pos > 2)
                        p = mempset(p, ' ', pos-2);
                p = stpcpy(p, ANSI_RED_ON);
                *p++ = '*';
        }

        if (pos > 0 && pos <= width) {
                p = stpcpy(p, ANSI_HIGHLIGHT_RED_ON);
                *p++ = '*';
        }

        p = stpcpy(p, ANSI_HIGHLIGHT_OFF);

        if (pos < width) {
                p = stpcpy(p, ANSI_RED_ON);
                *p++ = '*';
                if (pos < width-1)
                        p = mempset(p, ' ', width-1-pos);
                strcpy(p, ANSI_HIGHLIGHT_OFF);
        }
}