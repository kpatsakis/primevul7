Magick_Orientation_from_Exif_Orientation(const int orientation)
{
  switch (orientation)
  {
    case 1:
      return TopLeftOrientation;
    case 2:
      return TopRightOrientation;
    case 3:
      return BottomRightOrientation;
    case 4:
      return BottomLeftOrientation;
    case 5:
      return LeftTopOrientation;
    case 6:
      return RightTopOrientation;
    case 7:
      return RightBottomOrientation;
    case 8:
      return LeftBottomOrientation;
    case 0:
    default:
      return UndefinedOrientation;
  }
}