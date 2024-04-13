_XkbCheckTypeName(Atom name, int typeNdx)
{
    const char *str;

    str = NameForAtom(name);
    if ((strcmp(str, "ONE_LEVEL") == 0) || (strcmp(str, "TWO_LEVEL") == 0) ||
        (strcmp(str, "ALPHABETIC") == 0) || (strcmp(str, "KEYPAD") == 0))
        return FALSE;
    return TRUE;
}