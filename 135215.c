void GfxAxialShading::getParameterRange(double *lower, double *upper, double xMin, double yMin, double xMax, double yMax)
{
    double pdx, pdy, invsqnorm, tdx, tdy, t, range[2];

    // Linear gradients are orthogonal to the line passing through their
    // extremes. Because of convexity, the parameter range can be
    // computed as the convex hull (one the real line) of the parameter
    // values of the 4 corners of the box.
    //
    // The parameter value t for a point (x,y) can be computed as:
    //
    //   t = (p2 - p1) . (x,y) / |p2 - p1|^2
    //
    // t0  is the t value for the top left corner
    // tdx is the difference between left and right corners
    // tdy is the difference between top and bottom corners

    pdx = x1 - x0;
    pdy = y1 - y0;
    const double invsqnorm_denominator = (pdx * pdx + pdy * pdy);
    if (unlikely(invsqnorm_denominator == 0)) {
        *lower = 0;
        *upper = 0;
        return;
    }
    invsqnorm = 1.0 / invsqnorm_denominator;
    pdx *= invsqnorm;
    pdy *= invsqnorm;

    t = (xMin - x0) * pdx + (yMin - y0) * pdy;
    tdx = (xMax - xMin) * pdx;
    tdy = (yMax - yMin) * pdy;

    // Because of the linearity of the t value, tdx can simply be added
    // the t0 to move along the top edge. After this, *lower and *upper
    // represent the parameter range for the top edge, so extending it
    // to include the whole box simply requires adding tdy to the
    // correct extreme.

    range[0] = range[1] = t;
    if (tdx < 0)
        range[0] += tdx;
    else
        range[1] += tdx;

    if (tdy < 0)
        range[0] += tdy;
    else
        range[1] += tdy;

    *lower = std::max<double>(0., std::min<double>(1., range[0]));
    *upper = std::max<double>(0., std::min<double>(1., range[1]));
}