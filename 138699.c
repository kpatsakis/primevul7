query (void)
{
  static const GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32,  "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_STRING, "raw-filename", "The name entered"             }
  };
  static const GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE,  "image",        "Output image"                 }
  };
  static const GimpParamDef thumb_args[] =
  {
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_INT32,  "thumb-size",   "Preferred thumbnail size"     }
  };
  static const GimpParamDef thumb_return_vals[] =
  {
    { GIMP_PDB_IMAGE,  "image",        "Output image"                 },
    { GIMP_PDB_INT32,  "image-width",  "Width of full-sized image"    },
    { GIMP_PDB_INT32,  "image-height", "Height of full-sized image"   }
  };

  gimp_install_procedure (LOAD_PROC,
                          "Loads files of Compuserve GIF file format",
                          "FIXME: write help for gif_load",
                          "Spencer Kimball, Peter Mattis, Adam Moss, David Koblas",
                          "Spencer Kimball, Peter Mattis, Adam Moss, David Koblas",
                          "1995-2006",
                          N_("GIF image"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (load_args),
                          G_N_ELEMENTS (load_return_vals),
                          load_args, load_return_vals);

  gimp_register_file_handler_mime (LOAD_PROC, "image/gif");
  gimp_register_magic_load_handler (LOAD_PROC,
                                    "gif",
                                    "",
                                    "0,string,GIF8");

  gimp_install_procedure (LOAD_THUMB_PROC,
                          "Loads only the first frame of a GIF image, to be "
                          "used as a thumbnail",
                          "",
                          "Sven Neumann",
                          "Sven Neumann",
                          "2006",
                          NULL,
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (thumb_args),
                          G_N_ELEMENTS (thumb_return_vals),
                          thumb_args, thumb_return_vals);

  gimp_register_thumbnail_loader (LOAD_PROC, LOAD_THUMB_PROC);
}