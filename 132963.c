std::vector<ByteRange> *Hints::getPageRanges(int page)
{
    if ((page < 1) || (page > nPages)) {
        return nullptr;
    }

    int idx;
    if (page - 1 > pageFirst) {
        idx = page - 1;
    } else if (page - 1 < pageFirst) {
        idx = page;
    } else {
        idx = 0;
    }

    ByteRange pageRange;
    std::vector<ByteRange> *v = new std::vector<ByteRange>;

    pageRange.offset = pageOffset[idx];
    pageRange.length = pageLength[idx];
    v->push_back(pageRange);

    pageRange.offset = xRefOffset[idx];
    pageRange.length = 20 * nObjects[idx];
    v->push_back(pageRange);

    for (unsigned int j = 0; j < numSharedObject[idx]; j++) {
        unsigned int k = sharedObjectId[idx][j];

        pageRange.offset = groupOffset[k];
        pageRange.length = groupLength[k];
        v->push_back(pageRange);

        pageRange.offset = groupXRefOffset[k];
        pageRange.length = 20 * groupNumObjects[k];
        v->push_back(pageRange);
    }

    return v;
}