pixGetBackgroundRGBMap(PIX     *pixs,
                       PIX     *pixim,
                       PIX     *pixg,
                       l_int32  sx,
                       l_int32  sy,
                       l_int32  thresh,
                       l_int32  mincount,
                       PIX    **ppixmr,
                       PIX    **ppixmg,
                       PIX    **ppixmb)
{
l_int32    w, h, wm, hm, wim, him, wpls, wplim, wplf;
l_int32    xim, yim, delx, nx, ny, i, j, k, m;
l_int32    count, rsum, gsum, bsum, rval, gval, bval;
l_int32    empty, fgpixels;
l_uint32   pixel;
l_uint32  *datas, *dataim, *dataf, *lines, *lineim, *linef;
l_float32  scalex, scaley;
PIX       *piximi, *pixgc, *pixb, *pixf, *pixims;
PIX       *pixmr, *pixmg, *pixmb;

    PROCNAME("pixGetBackgroundRGBMap");

    if (!ppixmr || !ppixmg || !ppixmb)
        return ERROR_INT("&pixm* not all defined", procName, 1);
    *ppixmr = *ppixmg = *ppixmb = NULL;
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (pixGetDepth(pixs) != 32)
        return ERROR_INT("pixs not 32 bpp", procName, 1);
    if (pixim && pixGetDepth(pixim) != 1)
        return ERROR_INT("pixim not 1 bpp", procName, 1);
    if (sx < 4 || sy < 4)
        return ERROR_INT("sx and sy must be >= 4", procName, 1);
    if (mincount > sx * sy) {
        L_WARNING("mincount too large for tile size\n", procName);
        mincount = (sx * sy) / 3;
    }

        /* Evaluate the mask pixim and make sure it is not all foreground */
    fgpixels = 0;  /* boolean for existence of fg mask pixels */
    if (pixim) {
        piximi = pixInvert(NULL, pixim);  /* set non-'image' pixels to 1 */
        pixZero(piximi, &empty);
        pixDestroy(&piximi);
        if (empty)
            return ERROR_INT("pixim all fg; no background", procName, 1);
        pixZero(pixim, &empty);
        if (!empty)  /* there are fg pixels in pixim */
            fgpixels = 1;
    }

        /* Generate the foreground mask.  These pixels will be
         * ignored when computing the background values. */
    if (pixg)  /* use the input grayscale version if it is provided */
        pixgc = pixClone(pixg);
    else
        pixgc = pixConvertRGBToGrayFast(pixs);
    pixb = pixThresholdToBinary(pixgc, thresh);
    pixf = pixMorphSequence(pixb, "d7.1 + d1.7", 0);
    pixDestroy(&pixgc);
    pixDestroy(&pixb);

        /* Generate the output mask images */
    w = pixGetWidth(pixs);
    h = pixGetHeight(pixs);
    wm = (w + sx - 1) / sx;
    hm = (h + sy - 1) / sy;
    pixmr = pixCreate(wm, hm, 8);
    pixmg = pixCreate(wm, hm, 8);
    pixmb = pixCreate(wm, hm, 8);

    /* ------------- Set up the mapping images --------------- */
        /* Note: we only compute map values in tiles that are complete.
         * In general, tiles at right and bottom edges will not be
         * complete, and we must fill them in later. */
    nx = w / sx;
    ny = h / sy;
    wpls = pixGetWpl(pixs);
    datas = pixGetData(pixs);
    wplf = pixGetWpl(pixf);
    dataf = pixGetData(pixf);
    for (i = 0; i < ny; i++) {
        lines = datas + sy * i * wpls;
        linef = dataf + sy * i * wplf;
        for (j = 0; j < nx; j++) {
            delx = j * sx;
            rsum = gsum = bsum = 0;
            count = 0;
            for (k = 0; k < sy; k++) {
                for (m = 0; m < sx; m++) {
                    if (GET_DATA_BIT(linef + k * wplf, delx + m) == 0) {
                        pixel = *(lines + k * wpls + delx + m);
                        rsum += (pixel >> 24);
                        gsum += ((pixel >> 16) & 0xff);
                        bsum += ((pixel >> 8) & 0xff);
                        count++;
                    }
                }
            }
            if (count >= mincount) {
                rval = rsum / count;
                gval = gsum / count;
                bval = bsum / count;
                pixSetPixel(pixmr, j, i, rval);
                pixSetPixel(pixmg, j, i, gval);
                pixSetPixel(pixmb, j, i, bval);
            }
        }
    }
    pixDestroy(&pixf);

        /* If there is an optional mask with fg pixels, erase the previous
         * calculation for the corresponding map pixels, setting the
         * map values in each of the 3 color maps to 0.   Then, when
         * all the map holes are filled, these erased pixels will
         * be set by the surrounding map values. */
    if (pixim) {
        wim = pixGetWidth(pixim);
        him = pixGetHeight(pixim);
        dataim = pixGetData(pixim);
        wplim = pixGetWpl(pixim);
        for (i = 0; i < ny; i++) {
            yim = i * sy + sy / 2;
            if (yim >= him)
                break;
            lineim = dataim + yim * wplim;
            for (j = 0; j < nx; j++) {
                xim = j * sx + sx / 2;
                if (xim >= wim)
                    break;
                if (GET_DATA_BIT(lineim, xim)) {
                    pixSetPixel(pixmr, j, i, 0);
                    pixSetPixel(pixmg, j, i, 0);
                    pixSetPixel(pixmb, j, i, 0);
                }
            }
        }
    }

    /* ----------------- Now fill in the holes ----------------------- */
    if (pixFillMapHoles(pixmr, nx, ny, L_FILL_BLACK) ||
        pixFillMapHoles(pixmg, nx, ny, L_FILL_BLACK) ||
        pixFillMapHoles(pixmb, nx, ny, L_FILL_BLACK)) {
        pixDestroy(&pixmr);
        pixDestroy(&pixmg);
        pixDestroy(&pixmb);
        L_WARNING("can't make the maps\n", procName);
        return 1;
    }

        /* Finally, for each connected region corresponding to the
         * fg mask, reset all pixels to their average value. */
    if (pixim && fgpixels) {
        scalex = 1. / (l_float32)sx;
        scaley = 1. / (l_float32)sy;
        pixims = pixScaleBySampling(pixim, scalex, scaley);
        pixSmoothConnectedRegions(pixmr, pixims, 2);
        pixSmoothConnectedRegions(pixmg, pixims, 2);
        pixSmoothConnectedRegions(pixmb, pixims, 2);
        pixDestroy(&pixims);
    }

    *ppixmr = pixmr;
    *ppixmg = pixmg;
    *ppixmb = pixmb;
    pixCopyResolution(*ppixmr, pixs);
    pixCopyResolution(*ppixmg, pixs);
    pixCopyResolution(*ppixmb, pixs);
    return 0;
}