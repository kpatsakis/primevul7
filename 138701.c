run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam   values[4];
  GimpPDBStatusType  status = GIMP_PDB_SUCCESS;
  GError            *error  = NULL;
  gint32             image_ID;

  INIT_I18N ();

  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;

  if (strcmp (name, LOAD_PROC) == 0)
    {
      image_ID = load_image (param[1].data.d_string, FALSE, &error);
    }
  else if (strcmp (name, LOAD_THUMB_PROC) == 0)
    {
      image_ID = load_image (param[0].data.d_string, TRUE, &error);
    }
  else
    {
      status = GIMP_PDB_CALLING_ERROR;
    }

  if (status == GIMP_PDB_SUCCESS)
    {
      if (image_ID != -1)
        {
          /* The GIF format only tells you how many bits per pixel
           *  are in the image, not the actual number of used indices (D'OH!)
           *
           * So if we're not careful, repeated load/save of a transparent GIF
           *  without intermediate indexed->RGB->indexed pumps up the number of
           *  bits used, as we add an index each time for the transparent
           *  colour.  Ouch.  We either do some heavier analysis at save-time,
           *  or trim down the number of GIMP colours at load-time.  We do the
           *  latter for now.
           */
#ifdef GIFDEBUG
          g_print ("GIF: Highest used index is %d\n", highest_used_index);
#endif
          if (! promote_to_rgb)
            gimp_image_set_colormap (image_ID,
                                     gimp_cmap, highest_used_index + 1);

          *nreturn_vals = 2;
          values[1].type         = GIMP_PDB_IMAGE;
          values[1].data.d_image = image_ID;

          if (strcmp (name, LOAD_THUMB_PROC) == 0)
            {
              *nreturn_vals = 4;
              values[2].type         = GIMP_PDB_INT32;
              values[2].data.d_int32 = gimp_image_width (image_ID);
              values[3].type         = GIMP_PDB_INT32;
              values[3].data.d_int32 = gimp_image_height (image_ID);
            }
        }
      else
        {
          status = GIMP_PDB_EXECUTION_ERROR;

          if (error)
            {
              *nreturn_vals = 2;
              values[1].type          = GIMP_PDB_STRING;
              values[1].data.d_string = error->message;
            }
        }
    }

  values[0].data.d_status = status;
}