void GfxState::ReusablePathIterator::setCoord(double x, double y)
{
    curSubPath->setX(coordOff, x);
    curSubPath->setY(coordOff, y);
}