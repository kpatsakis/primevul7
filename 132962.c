Hints::~Hints()
{
    gfree(nObjects);
    gfree(pageObjectNum);
    gfree(xRefOffset);
    gfree(pageLength);
    gfree(pageOffset);
    for (int i = 0; i < nPages; i++) {
        if (numSharedObject[i]) {
            gfree(sharedObjectId[i]);
        }
    }
    gfree(sharedObjectId);
    gfree(numSharedObject);

    gfree(groupLength);
    gfree(groupOffset);
    gfree(groupHasSignature);
    gfree(groupNumObjects);
    gfree(groupXRefOffset);
}