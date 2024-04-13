DU_findSOPClassAndInstanceInDataSet(
  DcmItem *obj,
  char* sopClass,
  size_t sopClassSize,
  char* sopInstance,
  size_t sopInstanceSize,
  OFBool tolerateSpacePaddedUIDs)
{
    OFBool result = (DU_getStringDOElement(obj, DCM_SOPClassUID, sopClass, sopClassSize) &&
        DU_getStringDOElement(obj, DCM_SOPInstanceUID, sopInstance, sopInstanceSize));

    if (tolerateSpacePaddedUIDs)
    {
        /* gracefully correct space-padded UID strings */
        int slength;

        if ((0 < (slength=OFstatic_cast(int, strlen(sopClass))))&&(sopClass[slength-1]==' '))
            sopClass[slength-1]=0;
        if ((0 < (slength=OFstatic_cast(int, strlen(sopInstance))))&&(sopInstance[slength-1]==' '))
            sopInstance[slength-1]=0;
    }
    return result;
}