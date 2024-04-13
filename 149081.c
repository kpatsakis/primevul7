void CLASS parseCanonMakernotes(unsigned tag, unsigned type, unsigned len)
{

  if (tag == 0x0001)
    Canon_CameraSettings();
  else if (tag == 0x0002) // focal length
  {
    imgdata.lens.makernotes.FocalType = get2();
    imgdata.lens.makernotes.CurFocal = get2();
    if (imgdata.lens.makernotes.CanonFocalUnits > 1)
    {
      imgdata.lens.makernotes.CurFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
    }
  }

  else if (tag == 0x0004) // shot info
  {
    short tempAp;
    fseek(ifp, 30, SEEK_CUR);
    imgdata.other.FlashEC = _CanonConvertEV((signed short)get2());
    fseek(ifp, 8 - 32, SEEK_CUR);
    if ((tempAp = get2()) != 0x7fff)
      imgdata.lens.makernotes.CurAp = _CanonConvertAperture(tempAp);
    if (imgdata.lens.makernotes.CurAp < 0.7f)
    {
      fseek(ifp, 32, SEEK_CUR);
      imgdata.lens.makernotes.CurAp = _CanonConvertAperture(get2());
    }
    if (!aperture)
      aperture = imgdata.lens.makernotes.CurAp;
  }

  else if (tag == 0x0095 && // lens model tag
           !imgdata.lens.makernotes.Lens[0])
  {
    fread(imgdata.lens.makernotes.Lens, 2, 1, ifp);
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
    if (imgdata.lens.makernotes.Lens[0] < 65) // non-Canon lens
      fread(imgdata.lens.makernotes.Lens + 2, 62, 1, ifp);
    else
    {
      char efs[2];
      imgdata.lens.makernotes.LensFeatures_pre[0] = imgdata.lens.makernotes.Lens[0];
      imgdata.lens.makernotes.LensFeatures_pre[1] = imgdata.lens.makernotes.Lens[1];
      fread(efs, 2, 1, ifp);
      if (efs[0] == 45 && (efs[1] == 83 || efs[1] == 69 || efs[1] == 77))
      { // "EF-S, TS-E, MP-E, EF-M" lenses
        imgdata.lens.makernotes.Lens[2] = imgdata.lens.makernotes.LensFeatures_pre[2] = efs[0];
        imgdata.lens.makernotes.Lens[3] = imgdata.lens.makernotes.LensFeatures_pre[3] = efs[1];
        imgdata.lens.makernotes.Lens[4] = 32;
        if (efs[1] == 83)
        {
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF_S;
          imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
        }
        else if (efs[1] == 77)
        {
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF_M;
        }
      }
      else
      { // "EF" lenses
        imgdata.lens.makernotes.Lens[2] = 32;
        imgdata.lens.makernotes.Lens[3] = efs[0];
        imgdata.lens.makernotes.Lens[4] = efs[1];
      }
      fread(imgdata.lens.makernotes.Lens + 5, 58, 1, ifp);
    }
  }

  else if (tag == 0x00a9)
  {
    long int save1 = ftell(ifp);
    fseek(ifp, save1 + (0x5 << 1), SEEK_SET);
    Canon_WBpresets(0, 0);
    fseek(ifp, save1, SEEK_SET);
  }

  else if (tag == 0x00e0) // sensor info
  {
    imgdata.makernotes.canon.SensorWidth = (get2(), get2());
    imgdata.makernotes.canon.SensorHeight = get2();
    imgdata.makernotes.canon.SensorLeftBorder = (get2(), get2(), get2());
    imgdata.makernotes.canon.SensorTopBorder = get2();
    imgdata.makernotes.canon.SensorRightBorder = get2();
    imgdata.makernotes.canon.SensorBottomBorder = get2();
    imgdata.makernotes.canon.BlackMaskLeftBorder = get2();
    imgdata.makernotes.canon.BlackMaskTopBorder = get2();
    imgdata.makernotes.canon.BlackMaskRightBorder = get2();
    imgdata.makernotes.canon.BlackMaskBottomBorder = get2();
  }

  else if (tag == 0x4001 && len > 500)
  {
    int c;
    long int save1 = ftell(ifp);
    switch (len)
    {
    case 582:
      imgdata.makernotes.canon.CanonColorDataVer = 1; // 20D / 350D
      {
        fseek(ifp, save1 + (0x23 << 1), SEEK_SET);
        Canon_WBpresets(2, 2);
        fseek(ifp, save1 + (0x4b << 1), SEEK_SET);
        Canon_WBCTpresets(1); // ABCT
      }
      break;
    case 653:
      imgdata.makernotes.canon.CanonColorDataVer = 2; // 1Dmk2 / 1DsMK2
      {
        fseek(ifp, save1 + (0x27 << 1), SEEK_SET);
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0xa4 << 1), SEEK_SET);
        Canon_WBCTpresets(1); // ABCT
      }
      break;
    case 796:
      imgdata.makernotes.canon.CanonColorDataVer = 3; // 1DmkIIN / 5D / 30D / 400D
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();
      {
        fseek(ifp, save1 + (0x4e << 1), SEEK_SET);
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0x85 << 1), SEEK_SET);
        Canon_WBCTpresets(0);                       // BCAT
        fseek(ifp, save1 + (0x0c4 << 1), SEEK_SET); // offset 196 short
        int bls = 0;
        FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c] = get2());
        imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
      }
      break;
    // 1DmkIII / 1DSmkIII / 1DmkIV / 5DmkII
    // 7D / 40D / 50D / 60D / 450D / 500D
    // 550D / 1000D / 1100D
    case 674:
    case 692:
    case 702:
    case 1227:
    case 1250:
    case 1251:
    case 1337:
    case 1338:
    case 1346:
      imgdata.makernotes.canon.CanonColorDataVer = 4;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();
      {
        fseek(ifp, save1 + (0x53 << 1), SEEK_SET);
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0xa8 << 1), SEEK_SET);
        Canon_WBCTpresets(0);                       // BCAT
        fseek(ifp, save1 + (0x0e7 << 1), SEEK_SET); // offset 231 short
        int bls = 0;
        FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c] = get2());
        imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
      }
      if ((imgdata.makernotes.canon.CanonColorDataSubVer == 4) || (imgdata.makernotes.canon.CanonColorDataSubVer == 5))
      {
        fseek(ifp, save1 + (0x2b9 << 1), SEEK_SET); // offset 697 shorts
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      else if ((imgdata.makernotes.canon.CanonColorDataSubVer == 6) ||
               (imgdata.makernotes.canon.CanonColorDataSubVer == 7))
      {
        fseek(ifp, save1 + (0x2d0 << 1), SEEK_SET); // offset 720 shorts
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      else if (imgdata.makernotes.canon.CanonColorDataSubVer == 9)
      {
        fseek(ifp, save1 + (0x2d4 << 1), SEEK_SET); // offset 724 shorts
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      break;

    case 5120:
      imgdata.makernotes.canon.CanonColorDataVer = 5; // PowerSot G10, G12, G5 X, EOS M3, EOS M5
      {
        fseek(ifp, save1 + (0x56 << 1), SEEK_SET);
        if ((unique_id == 0x03970000) || // G7 X Mark II
            (unique_id == 0x04100000) || // G9 X Mark II
            (unique_id == 0x80000394))   // EOS M5
        {
          fseek(ifp, 18, SEEK_CUR);
          FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Other][c ^ (c >> 1)] = get2();
          fseek(ifp, 8, SEEK_CUR);
          Canon_WBpresets(8, 24);
          fseek(ifp, 168, SEEK_CUR);
          FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_WW][c ^ (c >> 1)] = get2();
          fseek(ifp, 24, SEEK_CUR);
          Canon_WBCTpresets(2); // BCADT
          fseek(ifp, 6, SEEK_CUR);
        }
        else
        {
          FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Other][c ^ (c >> 1)] = get2();
          get2();
          Canon_WBpresets(2, 12);
          fseek(ifp, save1 + (0xba << 1), SEEK_SET);
          Canon_WBCTpresets(2);                       // BCADT
          fseek(ifp, save1 + (0x108 << 1), SEEK_SET); // offset 264 short
        }
        int bls = 0;
        FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c] = get2());
        imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
      }
      break;

    case 1273:
    case 1275:
      imgdata.makernotes.canon.CanonColorDataVer = 6; // 600D / 1200D
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();
      {
        fseek(ifp, save1 + (0x67 << 1), SEEK_SET);
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0xbc << 1), SEEK_SET);
        Canon_WBCTpresets(0);                       // BCAT
        fseek(ifp, save1 + (0x0fb << 1), SEEK_SET); // offset 251 short
        int bls = 0;
        FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c] = get2());
        imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
      }
      fseek(ifp, save1 + (0x1e4 << 1), SEEK_SET); // offset 484 shorts
      imgdata.makernotes.canon.SpecularWhiteLevel = get2();
      FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      break;

    // 1DX / 5DmkIII / 6D / 100D / 650D / 700D / EOS M / 7DmkII / 750D / 760D
    case 1312:
    case 1313:
    case 1316:
    case 1506:
      imgdata.makernotes.canon.CanonColorDataVer = 7;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();
      {
        fseek(ifp, save1 + (0x80 << 1), SEEK_SET);
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0xd5 << 1), SEEK_SET);
        Canon_WBCTpresets(0);                       // BCAT
        fseek(ifp, save1 + (0x114 << 1), SEEK_SET); // offset 276 shorts
        int bls = 0;
        FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c] = get2());
        imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
      }
      if (imgdata.makernotes.canon.CanonColorDataSubVer == 10)
      {
        fseek(ifp, save1 + (0x1fd << 1), SEEK_SET); // offset 509 shorts
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      else if (imgdata.makernotes.canon.CanonColorDataSubVer == 11)
      {
        fseek(ifp, save1 + (0x2dd << 1), SEEK_SET); // offset 733 shorts
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      break;

    // 5DS / 5DS R / 80D / 1300D / 5D4
    case 1560:
    case 1592:
    case 1353:
      imgdata.makernotes.canon.CanonColorDataVer = 8;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();
      {
        fseek(ifp, save1 + (0x85 << 1), SEEK_SET);
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0x107 << 1), SEEK_SET);
        Canon_WBCTpresets(0);                       // BCAT
        fseek(ifp, save1 + (0x146 << 1), SEEK_SET); // offset 326 shorts
        int bls = 0;
        FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c] = get2());
        imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
      }
      if (imgdata.makernotes.canon.CanonColorDataSubVer == 14) // 1300D
      {
        fseek(ifp, save1 + (0x231 << 1), SEEK_SET);
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      else
      {
        fseek(ifp, save1 + (0x30f << 1), SEEK_SET); // offset 783 shorts
        imgdata.makernotes.canon.SpecularWhiteLevel = get2();
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
      }
      break;
    }
    fseek(ifp, save1, SEEK_SET);
  }
}