void GfxUnivariateShading::setupCache(const Matrix *ctm, double xMin, double yMin, double xMax, double yMax)
{
    double sMin, sMax, tMin, tMax, upperBound;
    int i, j, nComps, maxSize;

    gfree(cacheBounds);
    cacheBounds = nullptr;
    cacheSize = 0;

    if (unlikely(getNFuncs() < 1))
        return;

    // NB: there can be one function with n outputs or n functions with
    // one output each (where n = number of color components)
    nComps = getNFuncs() * funcs[0]->getOutputSize();

    getParameterRange(&sMin, &sMax, xMin, yMin, xMax, yMax);
    upperBound = ctm->norm() * getDistance(sMin, sMax);
    maxSize = ceil(upperBound);
    maxSize = std::max<int>(maxSize, 2);

    {
        double x[4], y[4];

        ctm->transform(xMin, yMin, &x[0], &y[0]);
        ctm->transform(xMax, yMin, &x[1], &y[1]);
        ctm->transform(xMin, yMax, &x[2], &y[2]);
        ctm->transform(xMax, yMax, &x[3], &y[3]);

        xMin = xMax = x[0];
        yMin = yMax = y[0];
        for (i = 1; i < 4; i++) {
            xMin = std::min<double>(xMin, x[i]);
            yMin = std::min<double>(yMin, y[i]);
            xMax = std::max<double>(xMax, x[i]);
            yMax = std::max<double>(yMax, y[i]);
        }
    }

    if (maxSize > (xMax - xMin) * (yMax - yMin)) {
        return;
    }

    if (t0 < t1) {
        tMin = t0 + sMin * (t1 - t0);
        tMax = t0 + sMax * (t1 - t0);
    } else {
        tMin = t0 + sMax * (t1 - t0);
        tMax = t0 + sMin * (t1 - t0);
    }

    cacheBounds = (double *)gmallocn_checkoverflow(maxSize, sizeof(double) * (nComps + 2));
    if (unlikely(!cacheBounds))
        return;
    cacheCoeff = cacheBounds + maxSize;
    cacheValues = cacheCoeff + maxSize;

    if (cacheSize != 0) {
        for (j = 0; j < cacheSize; ++j) {
            cacheCoeff[j] = 1 / (cacheBounds[j + 1] - cacheBounds[j]);
        }
    } else if (tMax != tMin) {
        double step = (tMax - tMin) / (maxSize - 1);
        double coeff = (maxSize - 1) / (tMax - tMin);

        cacheSize = maxSize;

        for (j = 0; j < cacheSize; ++j) {
            cacheBounds[j] = tMin + j * step;
            cacheCoeff[j] = coeff;

            for (i = 0; i < nComps; ++i) {
                cacheValues[j * nComps + i] = 0;
            }
            for (i = 0; i < getNFuncs(); ++i) {
                funcs[i]->transform(&cacheBounds[j], &cacheValues[j * nComps + i]);
            }
        }
    }

    lastMatch = 1;
}