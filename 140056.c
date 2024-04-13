pixBackgroundNormSimple(PIX  *pixs,
                        PIX  *pixim,
                        PIX  *pixg)
{
    return pixBackgroundNorm(pixs, pixim, pixg,
                             DefaultTileWidth, DefaultTileHeight,
                             DefaultFgThreshold, DefaultMinCount,
                             DefaultBgVal, DefaultXSmoothSize,
                             DefaultYSmoothSize);
}