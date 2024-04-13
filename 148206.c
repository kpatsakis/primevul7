static inline unsigned int FNAME(golomb_code)(const BYTE n, const unsigned int l)
{
    return VNAME(family).golomb_code[n][l];
}