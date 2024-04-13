bool Hints::readSharedObjectsTable(Stream *str)
{
    StreamBitReader sbr(str);

    const unsigned int firstSharedObjectNumber = sbr.readBits(32);

    const unsigned int firstSharedObjectOffset = sbr.readBits(32) + hintsLength;

    const unsigned int nSharedGroupsFirst = sbr.readBits(32);

    const unsigned int nSharedGroups = sbr.readBits(32);

    const unsigned int nBitsNumObjects = sbr.readBits(16);

    const unsigned int groupLengthLeast = sbr.readBits(32);

    const unsigned int nBitsDiffGroupLength = sbr.readBits(16);

    if ((!nSharedGroups) || (nSharedGroups >= INT_MAX / (int)sizeof(unsigned int))) {
        error(errSyntaxWarning, -1, "Invalid number of shared object groups");
        return false;
    }
    if ((!nSharedGroupsFirst) || (nSharedGroupsFirst > nSharedGroups)) {
        error(errSyntaxWarning, -1, "Invalid number of first page shared object groups");
        return false;
    }
    if (nBitsNumObjects > 32 || nBitsDiffGroupLength > 32) {
        error(errSyntaxWarning, -1, "Invalid shared object groups bit length");
        return false;
    }

    groupLength = (unsigned int *)gmallocn_checkoverflow(nSharedGroups, sizeof(unsigned int));
    groupOffset = (unsigned int *)gmallocn_checkoverflow(nSharedGroups, sizeof(unsigned int));
    groupHasSignature = (unsigned int *)gmallocn_checkoverflow(nSharedGroups, sizeof(unsigned int));
    groupNumObjects = (unsigned int *)gmallocn_checkoverflow(nSharedGroups, sizeof(unsigned int));
    groupXRefOffset = (unsigned int *)gmallocn_checkoverflow(nSharedGroups, sizeof(unsigned int));
    if (!groupLength || !groupOffset || !groupHasSignature || !groupNumObjects || !groupXRefOffset) {
        error(errSyntaxWarning, -1, "Failed to allocate memory for shared object groups");
        return false;
    }

    sbr.resetInputBits(); // reset on byte boundary. Not in specs!
    for (unsigned int i = 0; i < nSharedGroups && !sbr.atEOF(); i++) {
        groupLength[i] = groupLengthLeast + sbr.readBits(nBitsDiffGroupLength);
    }
    if (sbr.atEOF()) {
        return false;
    }

    groupOffset[0] = objectOffsetFirst;
    for (unsigned int i = 1; i < nSharedGroupsFirst; i++) {
        groupOffset[i] = groupOffset[i - 1] + groupLength[i - 1];
    }
    if (nSharedGroups > nSharedGroupsFirst) {
        groupOffset[nSharedGroupsFirst] = firstSharedObjectOffset;
        for (unsigned int i = nSharedGroupsFirst + 1; i < nSharedGroups; i++) {
            groupOffset[i] = groupOffset[i - 1] + groupLength[i - 1];
        }
    }

    sbr.resetInputBits(); // reset on byte boundary. Not in specs!
    for (unsigned int i = 0; i < nSharedGroups && !sbr.atEOF(); i++) {
        groupHasSignature[i] = sbr.readBits(1);
    }
    if (sbr.atEOF()) {
        return false;
    }

    sbr.resetInputBits(); // reset on byte boundary. Not in specs!
    for (unsigned int i = 0; i < nSharedGroups && !sbr.atEOF(); i++) {
        if (groupHasSignature[i]) {
            // readBits doesn't supports more than 32 bits.
            sbr.readBits(32);
            sbr.readBits(32);
            sbr.readBits(32);
            sbr.readBits(32);
        }
    }
    if (sbr.atEOF()) {
        return false;
    }

    sbr.resetInputBits(); // reset on byte boundary. Not in specs!
    for (unsigned int i = 0; i < nSharedGroups && !sbr.atEOF(); i++) {
        groupNumObjects[i] = nBitsNumObjects ? 1 + sbr.readBits(nBitsNumObjects) : 1;
    }

    for (unsigned int i = 0; i < nSharedGroupsFirst; i++) {
        groupNumObjects[i] = 0;
        groupXRefOffset[i] = 0;
    }
    if (nSharedGroups > nSharedGroupsFirst) {
        groupXRefOffset[nSharedGroupsFirst] = mainXRefEntriesOffset + 20 * firstSharedObjectNumber;
        for (unsigned int i = nSharedGroupsFirst + 1; i < nSharedGroups; i++) {
            groupXRefOffset[i] = groupXRefOffset[i - 1] + 20 * groupNumObjects[i - 1];
        }
    }

    return !sbr.atEOF();
}