cmsBool CMSEXPORT cmsDesaturateLab(cmsCIELab* Lab,
                                   double amax, double amin,
                                   double bmax, double bmin)
{

    // Whole Luma surface to zero

    if (Lab -> L < 0) {

        Lab-> L = Lab->a = Lab-> b = 0.0;
        return FALSE;
    }

    // Clamp white, DISCARD HIGHLIGHTS. This is done
    // in such way because icc spec doesn't allow the
    // use of L>100 as a highlight means.

    if (Lab->L > 100)
        Lab -> L = 100;

    // Check out gamut prism, on a, b faces

    if (Lab -> a < amin || Lab->a > amax||
        Lab -> b < bmin || Lab->b > bmax) {

            cmsCIELCh LCh;
            double h, slope;

            // Falls outside a, b limits. Transports to LCh space,
            // and then do the clipping


            if (Lab -> a == 0.0) { // Is hue exactly 90?

                // atan will not work, so clamp here
                Lab -> b = Lab->b < 0 ? bmin : bmax;
                return TRUE;
            }

            cmsLab2LCh(&LCh, Lab);

            slope = Lab -> b / Lab -> a;
            h = LCh.h;

            // There are 4 zones

            if ((h >= 0. && h < 45.) ||
                (h >= 315 && h <= 360.)) {

                    // clip by amax
                    Lab -> a = amax;
                    Lab -> b = amax * slope;
            }
            else
                if (h >= 45. && h < 135.)
                {
                    // clip by bmax
                    Lab -> b = bmax;
                    Lab -> a = bmax / slope;
                }
                else
                    if (h >= 135. && h < 225.) {
                        // clip by amin
                        Lab -> a = amin;
                        Lab -> b = amin * slope;

                    }
                    else
                        if (h >= 225. && h < 315.) {
                            // clip by bmin
                            Lab -> b = bmin;
                            Lab -> a = bmin / slope;
                        }
                        else  {
                            cmsSignalError(0, cmsERROR_RANGE, "Invalid angle");
                            return FALSE;
                        }

    }

    return TRUE;
}