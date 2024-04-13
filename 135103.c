GfxState::ReusablePathIterator::ReusablePathIterator(GfxPath *pathA) : path(pathA), subPathOff(0), coordOff(0), numCoords(0), curSubPath(nullptr)
{
    if (path->getNumSubpaths()) {
        curSubPath = path->getSubpath(subPathOff);
        numCoords = curSubPath->getNumPoints();
    }
}