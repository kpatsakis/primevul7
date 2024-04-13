static void free_smacro(SMacro *s)
{
    free_smacro_members(s);
    nasm_free(s);
}