Goffset Hints::getPageOffset(int page)
{
    if ((page < 1) || (page > nPages)) {
        return 0;
    }

    if (page - 1 > pageFirst) {
        return pageOffset[page - 1];
    } else if (page - 1 < pageFirst) {
        return pageOffset[page];
    } else {
        return pageOffset[0];
    }
}