static FILE *inc_fopen_search(const char *file, char **slpath,
                              enum incopen_mode omode, enum file_flags fmode)
{
    const struct strlist_entry *ip = strlist_head(ipath_list);
    FILE *fp;
    const char *prefix = "";
    char *sp;
    bool found;

    while (1) {
        sp = nasm_catfile(prefix, file);
        if (omode == INC_PROBE) {
            fp = NULL;
            found = nasm_file_exists(sp);
        } else {
            fp = nasm_open_read(sp, fmode);
            found = (fp != NULL);
        }
        if (found) {
            *slpath = sp;
            return fp;
        }

        nasm_free(sp);

        if (!ip) {
            *slpath = NULL;
            return NULL;
        }

        prefix = ip->str;
        ip = ip->next;
    }
}