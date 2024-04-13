pixGlobalNormNoSatRGB(PIX       *pixd,
                      PIX       *pixs,
                      l_int32    rval,
                      l_int32    gval,
                      l_int32    bval,
                      l_int32    factor,
                      l_float32  rank)
{
l_int32    mapval;
l_float32  rankrval, rankgval, rankbval;
l_float32  rfract, gfract, bfract, maxfract;

    PROCNAME("pixGlobalNormNoSatRGB");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (factor < 1)
        return (PIX *)ERROR_PTR("sampling factor < 1", procName, NULL);
    if (rank < 0.0 || rank > 1.0)
        return (PIX *)ERROR_PTR("rank not in [0.0 ... 1.0]", procName, NULL);
    if (rval <= 0 || gval <= 0 || bval <= 0)
        return (PIX *)ERROR_PTR("invalid estim. color values", procName, NULL);

        /* The max value for each component may be larger than the
         * input estimated background value.  In that case, mapping
         * for those pixels would saturate.  To prevent saturation,
         * we compute the fraction for each component by which we
         * would oversaturate.  Then take the max of these, and
         * reduce, uniformly over all components, the output intensity
         * by this value.  Then no component will saturate.
         * In practice, if rank < 1.0, a fraction of pixels
         * may have a component saturate.  By keeping rank close to 1.0,
         * that fraction can be made arbitrarily small. */
    pixGetRankValueMaskedRGB(pixs, NULL, 0, 0, factor, rank, &rankrval,
                             &rankgval, &rankbval);
    rfract = rankrval / (l_float32)rval;
    gfract = rankgval / (l_float32)gval;
    bfract = rankbval / (l_float32)bval;
    maxfract = L_MAX(rfract, gfract);
    maxfract = L_MAX(maxfract, bfract);
#if  DEBUG_GLOBAL
    lept_stderr("rankrval = %7.2f, rankgval = %7.2f, rankbval = %7.2f\n",
                rankrval, rankgval, rankbval);
    lept_stderr("rfract = %7.4f, gfract = %7.4f, bfract = %7.4f\n",
                rfract, gfract, bfract);
#endif  /* DEBUG_GLOBAL */

    mapval = (l_int32)(255. / maxfract);
    pixd = pixGlobalNormRGB(pixd, pixs, rval, gval, bval, mapval);
    return pixd;
}