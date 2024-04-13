pixCleanBackgroundToWhite(PIX       *pixs,
                          PIX       *pixim,
                          PIX       *pixg,
                          l_float32  gamma,
                          l_int32    blackval,
                          l_int32    whiteval)
{
l_int32  d;
PIX     *pixd;

    PROCNAME("pixCleanBackgroundToWhite");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    d = pixGetDepth(pixs);
    if (d != 8 && d != 32)
        return (PIX *)ERROR_PTR("depth not 8 or 32", procName, NULL);

    pixd = pixBackgroundNormSimple(pixs, pixim, pixg);
    if (!pixd)
        return (PIX *)ERROR_PTR("background norm failedd", procName, NULL);
    pixGammaTRC(pixd, pixd, gamma, blackval, whiteval);
    return pixd;
}