int GamutSampler(register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void* Cargo)
{
    GAMUTCHAIN*  t = (GAMUTCHAIN* ) Cargo;
    cmsCIELab LabIn1, LabOut1;
    cmsCIELab LabIn2, LabOut2;
    cmsUInt16Number Proof[cmsMAXCHANNELS], Proof2[cmsMAXCHANNELS];
    cmsFloat64Number dE1, dE2, ErrorRatio;

    // Assume in-gamut by default.
    ErrorRatio = 1.0;

    // Convert input to Lab
    cmsDoTransform(t -> hInput, In, &LabIn1, 1);

    // converts from PCS to colorant. This always
    // does return in-gamut values,
    cmsDoTransform(t -> hForward, &LabIn1, Proof, 1);

    // Now, do the inverse, from colorant to PCS.
    cmsDoTransform(t -> hReverse, Proof, &LabOut1, 1);

    memmove(&LabIn2, &LabOut1, sizeof(cmsCIELab));

    // Try again, but this time taking Check as input
    cmsDoTransform(t -> hForward, &LabOut1, Proof2, 1);
    cmsDoTransform(t -> hReverse, Proof2, &LabOut2, 1);

    // Take difference of direct value
    dE1 = cmsDeltaE(&LabIn1, &LabOut1);

    // Take difference of converted value
    dE2 = cmsDeltaE(&LabIn2, &LabOut2);


    // if dE1 is small and dE2 is small, value is likely to be in gamut
    if (dE1 < t->Thereshold && dE2 < t->Thereshold)
        Out[0] = 0;
    else {

        // if dE1 is small and dE2 is big, undefined. Assume in gamut
        if (dE1 < t->Thereshold && dE2 > t->Thereshold)
            Out[0] = 0;
        else
            // dE1 is big and dE2 is small, clearly out of gamut
            if (dE1 > t->Thereshold && dE2 < t->Thereshold)
                Out[0] = (cmsUInt16Number) _cmsQuickFloor((dE1 - t->Thereshold) + .5);
            else  {

                // dE1 is big and dE2 is also big, could be due to perceptual mapping
                // so take error ratio
                if (dE2 == 0.0)
                    ErrorRatio = dE1;
                else
                    ErrorRatio = dE1 / dE2;

                if (ErrorRatio > t->Thereshold)
                    Out[0] = (cmsUInt16Number)  _cmsQuickFloor((ErrorRatio - t->Thereshold) + .5);
                else
                    Out[0] = 0;
            }
    }


    return TRUE;
}