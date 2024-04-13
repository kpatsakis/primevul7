ModuleExport void UnregisterDIBImage(void)
{
  (void) UnregisterMagickInfo("ICODIB");
  (void) UnregisterMagickInfo("DIB");
}