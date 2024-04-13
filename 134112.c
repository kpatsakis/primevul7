static void free_smacro_table(struct hash_table *smt)
{
    clear_smacro_table(smt, CLEAR_ALLDEFINE);
}