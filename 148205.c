static inline unsigned int FNAME(golomb_code_len)(const BYTE n, const unsigned int l)
{
    return VNAME(family).golomb_code_len[n][l];
}