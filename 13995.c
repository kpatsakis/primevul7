static gboolean get_name_index(const EnumNames names[], const char *name, uint32_t *index)
{
    if (name) {
        int i;
        for (i = 0; names[i].name; i++) {
            if (strcmp(name, names[i].name) == 0) {
                *index = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}