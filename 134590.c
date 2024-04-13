void CLASS processNikonLensData (uchar *LensData, unsigned len)
{
  ushort i;
  if (len < 20) {
    switch (len) {
    case 9:
      i = 2;
      break;
    case 15:
      i = 7;
      break;
    case 16:
      i = 8;
      break;
    }
    imgdata.lens.nikon.NikonLensIDNumber = LensData[i];
    imgdata.lens.nikon.NikonLensFStops = LensData[i + 1];
    imgdata.lens.makernotes.LensFStops = (float)imgdata.lens.nikon.NikonLensFStops /12.0f;
    imgdata.lens.makernotes.MinFocal = 5.0f * powf64(2.0f, (float)LensData[i + 2] / 24.0f);
    imgdata.lens.makernotes.MaxFocal = 5.0f * powf64(2.0f, (float)LensData[i + 3] / 24.0f);
    imgdata.lens.makernotes.MaxAp4MinFocal = powf64(2.0f, (float)LensData[i + 4] / 24.0f);
    imgdata.lens.makernotes.MaxAp4MaxFocal = powf64(2.0f, (float)LensData[i + 5] / 24.0f);
    imgdata.lens.nikon.NikonMCUVersion = LensData[i + 6];
    if (i != 2)
      {
        imgdata.lens.makernotes.CurFocal = 5.0f * powf64(2.0f, (float)LensData[i - 1] / 24.0f);
        imgdata.lens.nikon.NikonEffectiveMaxAp = powf64(2.0f, (float)LensData[i + 7] / 24.0f);
      }
    imgdata.lens.makernotes.LensID =
      (unsigned long long) LensData[i] << 56 |
      (unsigned long long) LensData[i + 1] << 48 |
      (unsigned long long) LensData[i + 2] << 40 |
      (unsigned long long) LensData[i + 3] << 32 |
      (unsigned long long) LensData[i + 4] << 24 |
      (unsigned long long) LensData[i + 5] << 16 |
      (unsigned long long) LensData[i + 6] << 8 |
      (unsigned long long) imgdata.lens.nikon.NikonLensType;

  }
  else if ((len == 459) || (len == 590))
    {
      memcpy(imgdata.lens.makernotes.Lens, LensData + 390, 64);
    }
  else if (len == 509)
    {
      memcpy(imgdata.lens.makernotes.Lens, LensData + 391, 64);
    }
  else if (len == 879)
    {
      memcpy(imgdata.lens.makernotes.Lens, LensData + 680, 64);
    }
  free (LensData);
  return;
}