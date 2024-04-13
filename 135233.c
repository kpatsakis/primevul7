void GfxState::ReusablePathIterator::reset()
{
    coordOff = 0;
    subPathOff = 0;
    curSubPath = path->getSubpath(0);
    numCoords = curSubPath->getNumPoints();
}