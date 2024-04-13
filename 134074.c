static bool is_smac_param(enum pp_token_type toktype)
{
    return toktype >= TOK_SMAC_START_PARAMS;
}