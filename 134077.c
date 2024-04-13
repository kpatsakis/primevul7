static void clear_smacro(SMacro *s)
{
    free_smacro_members(s);
    /* Wipe everything except the next pointer */
    memset(&s->next + 1, 0, sizeof *s - sizeof s->next);
}