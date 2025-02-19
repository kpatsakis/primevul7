_XkbCheckAtoms(CARD32 *wire, int nAtoms, int swapped, Atom *pError)
{
    register int i;

    for (i = 0; i < nAtoms; i++, wire++) {
        if (swapped) {
            swapl(wire);
        }
        if ((((Atom) *wire) != None) && (!ValidAtom((Atom) *wire))) {
            *pError = ((Atom) *wire);
            return NULL;
        }
    }
    return wire;
}