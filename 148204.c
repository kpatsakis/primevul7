static void FNAME(golomb_coding)(Encoder *encoder, const BYTE n, const unsigned int l)
{
    encode(encoder, FNAME(golomb_code)(n, l), FNAME(golomb_code_len)(n, l));
}