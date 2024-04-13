unsigned int getCMSColorSpaceType(cmsColorSpaceSignature cs)
{
    switch (cs) {
    case cmsSigXYZData:
        return PT_XYZ;
        break;
    case cmsSigLabData:
        return PT_Lab;
        break;
    case cmsSigLuvData:
        return PT_YUV;
        break;
    case cmsSigYCbCrData:
        return PT_YCbCr;
        break;
    case cmsSigYxyData:
        return PT_Yxy;
        break;
    case cmsSigRgbData:
        return PT_RGB;
        break;
    case cmsSigGrayData:
        return PT_GRAY;
        break;
    case cmsSigHsvData:
        return PT_HSV;
        break;
    case cmsSigHlsData:
        return PT_HLS;
        break;
    case cmsSigCmykData:
        return PT_CMYK;
        break;
    case cmsSigCmyData:
        return PT_CMY;
        break;
    case cmsSig2colorData:
    case cmsSig3colorData:
    case cmsSig4colorData:
    case cmsSig5colorData:
    case cmsSig6colorData:
    case cmsSig7colorData:
    case cmsSig8colorData:
    case cmsSig9colorData:
    case cmsSig10colorData:
    case cmsSig11colorData:
    case cmsSig12colorData:
    case cmsSig13colorData:
    case cmsSig14colorData:
    case cmsSig15colorData:
    default:
        break;
    }
    return PT_RGB;
}