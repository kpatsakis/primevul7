static unsigned int FNAME(golomb_decoding)(const unsigned int l, const unsigned int bits,
                                           unsigned int * const codewordlen)
{
    if (bits > VNAME(family).notGRprefixmask[l]) { /*GR*/
        const unsigned int zeroprefix = cnt_l_zeroes(bits);       /* leading zeroes in codeword */
        const unsigned int cwlen = zeroprefix + 1 + l;            /* codeword length */
        (*codewordlen) = cwlen;
        return (zeroprefix << l) | ((bits >> (32 - cwlen)) & bppmask[l]);
    } else { /* not-GR */
        const unsigned int cwlen = VNAME(family).notGRcwlen[l];
        (*codewordlen) = cwlen;
        return VNAME(family).nGRcodewords[l] + ((bits) >> (32 - cwlen) &
                                                bppmask[VNAME(family).notGRsuffixlen[l]]);
    }
}