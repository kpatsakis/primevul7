static int smac_nparam(enum pp_token_type toktype)
{
    return toktype - TOK_SMAC_START_PARAMS;
}