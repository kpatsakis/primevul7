Hints::Hints(BaseStream *str, Linearization *linearization, XRef *xref, SecurityHandler *secHdlr)
{
    mainXRefEntriesOffset = linearization->getMainXRefEntriesOffset();
    nPages = linearization->getNumPages();
    pageFirst = linearization->getPageFirst();
    pageEndFirst = linearization->getEndFirst();
    pageObjectFirst = linearization->getObjectNumberFirst();
    if (pageObjectFirst < 0 || pageObjectFirst >= xref->getNumObjects()) {
        error(errSyntaxWarning, -1, "Invalid reference for first page object ({0:d}) in linearization table ", pageObjectFirst);
        pageObjectFirst = 0;
    }
    XRefEntry *pageObjectFirstXRefEntry = xref->getEntry(pageObjectFirst);
    if (!pageObjectFirstXRefEntry) {
        error(errSyntaxWarning, -1, "No XRef entry for first page object");
        pageOffsetFirst = 0;
    } else {
        pageOffsetFirst = pageObjectFirstXRefEntry->offset;
    }

    if (nPages >= INT_MAX / (int)sizeof(unsigned int)) {
        error(errSyntaxWarning, -1, "Invalid number of pages ({0:d}) for hints table", nPages);
        nPages = 0;
    }
    nObjects = (unsigned int *)gmallocn_checkoverflow(nPages, sizeof(unsigned int));
    pageObjectNum = (int *)gmallocn_checkoverflow(nPages, sizeof(int));
    xRefOffset = (unsigned int *)gmallocn_checkoverflow(nPages, sizeof(unsigned int));
    pageLength = (unsigned int *)gmallocn_checkoverflow(nPages, sizeof(unsigned int));
    pageOffset = (Goffset *)gmallocn_checkoverflow(nPages, sizeof(Goffset));
    numSharedObject = (unsigned int *)gmallocn_checkoverflow(nPages, sizeof(unsigned int));
    sharedObjectId = (unsigned int **)gmallocn_checkoverflow(nPages, sizeof(unsigned int *));
    if (!nObjects || !pageObjectNum || !xRefOffset || !pageLength || !pageOffset || !numSharedObject || !sharedObjectId) {
        error(errSyntaxWarning, -1, "Failed to allocate memory for hints table");
        nPages = 0;
    }

    if (nPages != 0) {
        memset(pageLength, 0, nPages * sizeof(unsigned int));
        memset(pageOffset, 0, nPages * sizeof(unsigned int));
        memset(numSharedObject, 0, nPages * sizeof(unsigned int));
        memset(pageObjectNum, 0, nPages * sizeof(int));
    }

    groupLength = nullptr;
    groupOffset = nullptr;
    groupHasSignature = nullptr;
    groupNumObjects = nullptr;
    groupXRefOffset = nullptr;

    ok = true;
    readTables(str, linearization, xref, secHdlr);
}