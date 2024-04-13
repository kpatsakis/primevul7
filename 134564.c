void CLASS apply_profile (const char *input, const char *output)
{
  char *prof;
  cmsHPROFILE hInProfile=0, hOutProfile=0;
  cmsHTRANSFORM hTransform;
  FILE *fp;
  unsigned size;

  if (strcmp (input, "embed"))
    hInProfile = cmsOpenProfileFromFile (input, "r");
  else if (profile_length) {
#ifndef LIBRAW_LIBRARY_BUILD
    prof = (char *) malloc (profile_length);
    merror (prof, "apply_profile()");
    fseek (ifp, profile_offset, SEEK_SET);
    fread (prof, 1, profile_length, ifp);
    hInProfile = cmsOpenProfileFromMem (prof, profile_length);
    free (prof);
#else
    hInProfile = cmsOpenProfileFromMem (imgdata.color.profile, profile_length);
#endif
  } else
    {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_NO_EMBEDDED_PROFILE;
#endif
#ifdef DCRAW_VERBOSE
          fprintf (stderr,_("%s has no embedded profile.\n"), ifname);
#endif
    }
  if (!hInProfile)
      {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_NO_INPUT_PROFILE;
#endif
          return;
      }
  if (!output)
    hOutProfile = cmsCreate_sRGBProfile();
  else if ((fp = fopen (output, "rb"))) {
    fread (&size, 4, 1, fp);
    fseek (fp, 0, SEEK_SET);
    oprof = (unsigned *) malloc (size = ntohl(size));
    merror (oprof, "apply_profile()");
    fread (oprof, 1, size, fp);
    fclose (fp);
    if (!(hOutProfile = cmsOpenProfileFromMem (oprof, size))) {
      free (oprof);
      oprof = 0;
    }
  }
#ifdef DCRAW_VERBOSE
 else
    fprintf (stderr,_("Cannot open file %s!\n"), output);
#endif
  if (!hOutProfile)
      {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_BAD_OUTPUT_PROFILE;
#endif
          goto quit;
      }
#ifdef DCRAW_VERBOSE
  if (verbose)
    fprintf (stderr,_("Applying color profile...\n"));
#endif
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_APPLY_PROFILE,0,2);
#endif
  hTransform = cmsCreateTransform (hInProfile, TYPE_RGBA_16,
	hOutProfile, TYPE_RGBA_16, INTENT_PERCEPTUAL, 0);
  cmsDoTransform (hTransform, image, image, width*height);
  raw_color = 1;		/* Don't use rgb_cam with a profile */
  cmsDeleteTransform (hTransform);
  cmsCloseProfile (hOutProfile);
quit:
  cmsCloseProfile (hInProfile);
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_APPLY_PROFILE,1,2);
#endif
}