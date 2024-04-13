int EstimateTAC(register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void * Cargo)
{
    cmsTACestimator* bp = (cmsTACestimator*) Cargo;
    cmsFloat32Number RoundTrip[cmsMAXCHANNELS];
    cmsUInt32Number i;
    cmsFloat32Number Sum;


    // Evaluate the xform
    cmsDoTransform(bp->hRoundTrip, In, RoundTrip, 1);

    // All all amounts of ink
    for (Sum=0, i=0; i < bp ->nOutputChans; i++)
            Sum += RoundTrip[i];

    // If above maximum, keep track of input values
    if (Sum > bp ->MaxTAC) {

            bp ->MaxTAC = Sum;

            for (i=0; i < bp ->nOutputChans; i++) {
                bp ->MaxInput[i] = In[i];
            }
    }

    return TRUE;

    cmsUNUSED_PARAMETER(Out);
}