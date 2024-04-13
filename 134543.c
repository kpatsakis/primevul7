void CLASS setOlympusBodyFeatures (unsigned long id)
{
  if ((id == 0x4434303430) ||
      (id == 0x4434303431) ||
      ((id >= 0x5330303030) && (id <= 0x5330303939)))
    {
      imgdata.lens.makernotes.CameraFormat = FT;
      imgdata.lens.makernotes.CameraMount = FT;
    }
  else
    {
      imgdata.lens.makernotes.LensMount = FixedLens;
      imgdata.lens.makernotes.CameraMount = FixedLens;
    }

  if ((id == 0x4434303430) ||
      (id == 0x4434303431) ||
      ((id >= 0x5330303033) && (id <= 0x5330303138)) ||
      (id == 0x5330303233) ||
      (id == 0x5330303239) ||
      (id == 0x5330303330) ||
      (id == 0x5330303333))
    {
      imgdata.lens.makernotes.CameraMount = FT;
    }
  else if (imgdata.lens.makernotes.CameraMount != FixedLens)
    {
      imgdata.lens.makernotes.CameraMount = mFT;
    }
  return;
}