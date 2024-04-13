void CLASS setPentaxBodyFeatures (unsigned id)
{
  switch (id) {
  case 0x12994:
  case 0x12aa2:
  case 0x12b1a:
  case 0x12b60:
  case 0x12b7e:
  case 0x12b80:
  case 0x12b9c:
  case 0x12b9d:
  case 0x12ba2:
  case 0x12c1e:
  case 0x12c20:
  case 0x12cd2:
  case 0x12cd4:
  case 0x12cfa:
  case 0x12d72:
  case 0x12d73:
  case 0x12db8:
  case 0x12dfe:
  case 0x12e6c:
  case 0x12e76:
  case 0x12ef8:
  case 0x12f52:
  case 0x12f70:
  case 0x12f71:
  case 0x12fb6:
  case 0x12fc0:
  case 0x12fca:
  case 0x1301a:
    imgdata.lens.makernotes.LensMount = Pentax_K;
    imgdata.lens.makernotes.CameraMount = Pentax_K;
    imgdata.lens.makernotes.CameraFormat = APSC;
    break;
  case 0x12e08:
  case 0x13010:
    imgdata.lens.makernotes.LensMount = Pentax_645;
    imgdata.lens.makernotes.LensFormat = MF;
    imgdata.lens.makernotes.CameraMount = Pentax_645;
    imgdata.lens.makernotes.CameraFormat = MF;
    break;
  case 0x12ee4:
  case 0x12f66:
  case 0x12f7a:
  case 0x1302e:
    imgdata.lens.makernotes.LensMount = Pentax_Q;
    imgdata.lens.makernotes.CameraMount = Pentax_Q;
    break;
  default:
    imgdata.lens.makernotes.LensMount = FixedLens;
    imgdata.lens.makernotes.CameraMount = FixedLens;
  }
  return;
}