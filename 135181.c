void GfxRadialShading::getParameterRange(double *lower, double *upper, double xMin, double yMin, double xMax, double yMax)
{
    double cx, cy, cr, dx, dy, dr;
    double a, x_focus, y_focus;
    double mindr, minx, miny, maxx, maxy;
    double range[2];
    bool valid;

    // A radial pattern is considered degenerate if it can be
    // represented as a solid or clear pattern.  This corresponds to one
    // of the two cases:
    //
    // 1) The radii are both very small:
    //      |dr| < FLT_EPSILON && min (r0, r1) < FLT_EPSILON
    //
    // 2) The two circles have about the same radius and are very
    //    close to each other (approximately a cylinder gradient that
    //    doesn't move with the parameter):
    //      |dr| < FLT_EPSILON && max (|dx|, |dy|) < 2 * FLT_EPSILON

    if (xMin >= xMax || yMin >= yMax || (fabs(r0 - r1) < RADIAL_EPSILON && (std::min<double>(r0, r1) < RADIAL_EPSILON || std::max<double>(fabs(x0 - x1), fabs(y0 - y1)) < 2 * RADIAL_EPSILON))) {
        *lower = *upper = 0;
        return;
    }

    range[0] = range[1] = 0;
    valid = false;

    x_focus = y_focus = 0; // silence gcc

    cx = x0;
    cy = y0;
    cr = r0;
    dx = x1 - cx;
    dy = y1 - cy;
    dr = r1 - cr;

    // translate by -(cx, cy) to simplify computations
    xMin -= cx;
    yMin -= cy;
    xMax -= cx;
    yMax -= cy;

    // enlarge boundaries slightly to avoid rounding problems in the
    // parameter range computation
    xMin -= RADIAL_EPSILON;
    yMin -= RADIAL_EPSILON;
    xMax += RADIAL_EPSILON;
    yMax += RADIAL_EPSILON;

    // enlarge boundaries even more to avoid rounding problems when
    // testing if a point belongs to the box
    minx = xMin - RADIAL_EPSILON;
    miny = yMin - RADIAL_EPSILON;
    maxx = xMax + RADIAL_EPSILON;
    maxy = yMax + RADIAL_EPSILON;

    // we dont' allow negative radiuses, so we will be checking that
    // t*dr >= mindr to consider t valid
    mindr = -(cr + RADIAL_EPSILON);

    // After the previous transformations, the start circle is centered
    // in the origin and has radius cr. A 1-unit change in the t
    // parameter corresponds to dx,dy,dr changes in the x,y,r of the
    // circle (center coordinates, radius).
    //
    // To compute the minimum range needed to correctly draw the
    // pattern, we start with an empty range and extend it to include
    // the circles touching the bounding box or within it.

    // Focus, the point where the circle has radius == 0.
    //
    // r = cr + t * dr = 0
    // t = -cr / dr
    //
    // If the radius is constant (dr == 0) there is no focus (the
    // gradient represents a cylinder instead of a cone).
    if (fabs(dr) >= RADIAL_EPSILON) {
        double t_focus;

        t_focus = -cr / dr;
        x_focus = t_focus * dx;
        y_focus = t_focus * dy;
        if (minx <= x_focus && x_focus <= maxx && miny <= y_focus && y_focus <= maxy) {
            valid = radialExtendRange(range, t_focus, valid);
        }
    }

    // Circles externally tangent to box edges.
    //
    // All circles have center in (dx, dy) * t
    //
    // If the circle is tangent to the line defined by the edge of the
    // box, then at least one of the following holds true:
    //
    //   (dx*t) + (cr + dr*t) == x0 (left   edge)
    //   (dx*t) - (cr + dr*t) == x1 (right  edge)
    //   (dy*t) + (cr + dr*t) == y0 (top    edge)
    //   (dy*t) - (cr + dr*t) == y1 (bottom edge)
    //
    // The solution is only valid if the tangent point is actually on
    // the edge, i.e. if its y coordinate is in [y0,y1] for left/right
    // edges and if its x coordinate is in [x0,x1] for top/bottom edges.
    //
    // For the first equation:
    //
    //   (dx + dr) * t = x0 - cr
    //   t = (x0 - cr) / (dx + dr)
    //   y = dy * t
    //
    // in the code this becomes:
    //
    //   t_edge = (num) / (den)
    //   v = (delta) * t_edge
    //
    // If the denominator in t is 0, the pattern is tangent to a line
    // parallel to the edge under examination. The corner-case where the
    // boundary line is the same as the edge is handled by the focus
    // point case and/or by the a==0 case.

    // circles tangent (externally) to left/right/top/bottom edge
    radialEdge(xMin - cr, dx + dr, dy, miny, maxy, dr, mindr, valid, range);
    radialEdge(xMax + cr, dx - dr, dy, miny, maxy, dr, mindr, valid, range);
    radialEdge(yMin - cr, dy + dr, dx, minx, maxx, dr, mindr, valid, range);
    radialEdge(yMax + cr, dy - dr, dx, minx, maxx, dr, mindr, valid, range);

    // Circles passing through a corner.
    //
    // A circle passing through the point (x,y) satisfies:
    //
    // (x-t*dx)^2 + (y-t*dy)^2 == (cr + t*dr)^2
    //
    // If we set:
    //   a = dx^2 + dy^2 - dr^2
    //   b = x*dx + y*dy + cr*dr
    //   c = x^2 + y^2 - cr^2
    // we have:
    //   a*t^2 - 2*b*t + c == 0

    a = dx * dx + dy * dy - dr * dr;
    if (fabs(a) < RADIAL_EPSILON * RADIAL_EPSILON) {
        double b;

        // Ensure that gradients with both a and dr small are
        // considered degenerate.
        // The floating point version of the degeneracy test implemented
        // in _radial_pattern_is_degenerate() is:
        //
        //  1) The circles are practically the same size:
        //     |dr| < RADIAL_EPSILON
        //  AND
        //  2a) The circles are both very small:
        //      min (r0, r1) < RADIAL_EPSILON
        //   OR
        //  2b) The circles are very close to each other:
        //      max (|dx|, |dy|) < 2 * RADIAL_EPSILON
        //
        // Assuming that the gradient is not degenerate, we want to
        // show that |a| < RADIAL_EPSILON^2 implies |dr| >= RADIAL_EPSILON.
        //
        // If the gradient is not degenerate yet it has |dr| <
        // RADIAL_EPSILON, (2b) is false, thus:
        //
        //   max (|dx|, |dy|) >= 2*RADIAL_EPSILON
        // which implies:
        //   4*RADIAL_EPSILON^2 <= max (|dx|, |dy|)^2 <= dx^2 + dy^2
        //
        // From the definition of a, we get:
        //   a = dx^2 + dy^2 - dr^2 < RADIAL_EPSILON^2
        //   dx^2 + dy^2 - RADIAL_EPSILON^2 < dr^2
        //   3*RADIAL_EPSILON^2 < dr^2
        //
        // which is inconsistent with the hypotheses, thus |dr| <
        // RADIAL_EPSILON is false or the gradient is degenerate.

        assert(fabs(dr) >= RADIAL_EPSILON);

        // If a == 0, all the circles are tangent to a line in the
        // focus point. If this line is within the box extents, we
        // should add the circle with infinite radius, but this would
        // make the range unbounded. We will be limiting the range to
        // [0,1] anyway, so we simply add the biggest legitimate
        // circle (it happens for 0 or for 1).
        if (dr < 0) {
            valid = radialExtendRange(range, 0, valid);
        } else {
            valid = radialExtendRange(range, 1, valid);
        }

        // Nondegenerate, nonlimit circles passing through the corners.
        //
        // a == 0 && a*t^2 - 2*b*t + c == 0
        //
        // t = c / (2*b)
        //
        // The b == 0 case has just been handled, so we only have to
        // compute this if b != 0.

        // circles touching each corner
        radialCorner1(xMin, yMin, b, dx, dy, cr, dr, mindr, valid, range);
        radialCorner1(xMin, yMax, b, dx, dy, cr, dr, mindr, valid, range);
        radialCorner1(xMax, yMin, b, dx, dy, cr, dr, mindr, valid, range);
        radialCorner1(xMax, yMax, b, dx, dy, cr, dr, mindr, valid, range);
    } else {
        double inva, b, c, d;

        inva = 1 / a;

        // Nondegenerate, nonlimit circles passing through the corners.
        //
        // a != 0 && a*t^2 - 2*b*t + c == 0
        //
        // t = (b +- sqrt (b*b - a*c)) / a
        //
        // If the argument of sqrt() is negative, then no circle
        // passes through the corner.

        // circles touching each corner
        radialCorner2(xMin, yMin, a, b, c, d, dx, dy, cr, inva, dr, mindr, valid, range);
        radialCorner2(xMin, yMax, a, b, c, d, dx, dy, cr, inva, dr, mindr, valid, range);
        radialCorner2(xMax, yMin, a, b, c, d, dx, dy, cr, inva, dr, mindr, valid, range);
        radialCorner2(xMax, yMax, a, b, c, d, dx, dy, cr, inva, dr, mindr, valid, range);
    }

    *lower = std::max<double>(0., std::min<double>(1., range[0]));
    *upper = std::max<double>(0., std::min<double>(1., range[1]));
}