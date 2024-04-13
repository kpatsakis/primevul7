void GfxState::ReusablePathIterator::next()
{
    ++coordOff;
    if (coordOff == numCoords) {
        ++subPathOff;
        if (subPathOff < path->getNumSubpaths()) {
            coordOff = 0;
            curSubPath = path->getSubpath(subPathOff);
            numCoords = curSubPath->getNumPoints();
        }
    }
}