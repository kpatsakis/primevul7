static bool radialExtendRange(double range[2], double value, bool valid)
{
    if (!valid)
        range[0] = range[1] = value;
    else if (value < range[0])
        range[0] = value;
    else if (value > range[1])
        range[1] = value;

    return true;
}