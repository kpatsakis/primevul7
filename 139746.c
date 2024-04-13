DU_findSOPClassAndInstanceInFile(
  const char *fname,
  char* sopClass,
  size_t sopClassSize,
  char* sopInstance,
  size_t sopInstanceSize,
  OFBool tolerateSpacePaddedUIDs)
{
    DcmFileFormat ff;
    if (! ff.loadFile(fname, EXS_Unknown, EGL_noChange).good())
        return OFFalse;

    /* look in the meta-header first */
    OFBool found = DU_findSOPClassAndInstanceInDataSet(
        ff.getMetaInfo(), sopClass, sopClassSize, sopInstance, sopInstanceSize, tolerateSpacePaddedUIDs);

    if (!found) {
        found = DU_findSOPClassAndInstanceInDataSet(
            ff.getDataset(), sopClass, sopClassSize, sopInstance, sopInstanceSize, tolerateSpacePaddedUIDs);
    }

    return found;
}