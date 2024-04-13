static Token *expand_smacro(Token *tline)
{
    smacro_deadman.total  = nasm_limit[LIMIT_MACRO_TOKENS];
    smacro_deadman.levels = nasm_limit[LIMIT_MACRO_LEVELS];
    smacro_deadman.triggered = false;
    return expand_smacro_noreset(tline);
}