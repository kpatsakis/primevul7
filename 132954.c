int Hints::getPageObjectNum(int page)
{
    if ((page < 1) || (page > nPages)) {
        return 0;
    }

    if (page - 1 > pageFirst) {
        return pageObjectNum[page - 1];
    } else if (page - 1 < pageFirst) {
        return pageObjectNum[page];
    } else {
        return pageObjectNum[0];
    }
}