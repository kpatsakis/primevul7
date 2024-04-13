unsigned int getCMSNChannels(cmsColorSpaceSignature cs)
{
    switch (cs) {
    case cmsSigXYZData:
    case cmsSigLuvData:
    case cmsSigLabData:
    case cmsSigYCbCrData:
    case cmsSigYxyData:
    case cmsSigRgbData:
    case cmsSigHsvData:
    case cmsSigHlsData:
    case cmsSigCmyData:
    case cmsSig3colorData:
        return 3;
        break;
    case cmsSigGrayData:
        return 1;
        break;
    case cmsSigCmykData:
    case cmsSig4colorData:
        return 4;
        break;
    case cmsSig2colorData:
        return 2;
        break;
    case cmsSig5colorData:
        return 5;
        break;
    case cmsSig6colorData:
        return 6;
        break;
    case cmsSig7colorData:
        return 7;
        break;
    case cmsSig8colorData:
        return 8;
        break;
    case cmsSig9colorData:
        return 9;
        break;
    case cmsSig10colorData:
        return 10;
        break;
    case cmsSig11colorData:
        return 11;
        break;
    case cmsSig12colorData:
        return 12;
        break;
    case cmsSig13colorData:
        return 13;
        break;
    case cmsSig14colorData:
        return 14;
        break;
    case cmsSig15colorData:
        return 15;
    default:
        break;
    }
    return 3;
}