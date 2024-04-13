/* extern */ x3f_t *x3f_new_from_file(LibRaw_abstract_datastream *infile)
{
  if (!infile)
    return NULL;
  INT64 fsize = infile->size();
  x3f_t *x3f = (x3f_t *)calloc(1, sizeof(x3f_t));
  if (!x3f)
    throw LIBRAW_EXCEPTION_ALLOC;
  try
  {
    x3f_info_t *I = NULL;
    x3f_header_t *H = NULL;
    x3f_directory_section_t *DS = NULL;
    int i, d;

    I = &x3f->info;
    I->error = NULL;
    I->input.file = infile;
    I->output.file = NULL;

    /* Read file header */
    H = &x3f->header;
    infile->seek(0, SEEK_SET);
    GET4(H->identifier);

    if (H->identifier != X3F_FOVb)
    {
      free(x3f);
      return NULL;
    }

    GET4(H->version);
    GETN(H->unique_identifier, SIZE_UNIQUE_IDENTIFIER);
    /* TODO: the meaning of the rest of the header for version >= 4.0 (Quattro)
     * is unknown */
    if (H->version < X3F_VERSION_4_0)
    {
      GET4(H->mark_bits);
      GET4(H->columns);
      GET4(H->rows);
      GET4(H->rotation);
      if (H->version >= X3F_VERSION_2_1)
      {
        int num_ext_data =
            H->version >= X3F_VERSION_3_0 ? NUM_EXT_DATA_3_0 : NUM_EXT_DATA_2_1;

        GETN(H->white_balance, SIZE_WHITE_BALANCE);
        if (H->version >= X3F_VERSION_2_3)
          GETN(H->color_mode, SIZE_COLOR_MODE);
        GETN(H->extended_types, num_ext_data);
        for (i = 0; i < num_ext_data; i++)
          GET4F(H->extended_data[i]);
      }
    }

    /* Go to the beginning of the directory */
    infile->seek(-4, SEEK_END);
    infile->seek(x3f_get4(infile), SEEK_SET);

    /* Read the directory header */
    DS = &x3f->directory_section;
    GET4(DS->identifier);
    GET4(DS->version);
    GET4(DS->num_directory_entries);

    if (DS->num_directory_entries > 50)
      goto _err; // too much direntries, most likely broken file

    if (DS->num_directory_entries > 0)
    {
      size_t size = DS->num_directory_entries * sizeof(x3f_directory_entry_t);
      DS->directory_entry = (x3f_directory_entry_t *)calloc(1, size);
    }

    /* Traverse the directory */
    for (d = 0; d < DS->num_directory_entries; d++)
    {
      x3f_directory_entry_t *DE = &DS->directory_entry[d];
      x3f_directory_entry_header_t *DEH = &DE->header;
      uint32_t save_dir_pos;

      /* Read the directory entry info */
      GET4(DE->input.offset);
      GET4(DE->input.size);
      if (DE->input.offset + DE->input.size > fsize * 2)
        goto _err;

      DE->output.offset = 0;
      DE->output.size = 0;

      GET4(DE->type);

      /* Save current pos and go to the entry */
      save_dir_pos = infile->tell();
      infile->seek(DE->input.offset, SEEK_SET);

      /* Read the type independent part of the entry header */
      DEH = &DE->header;
      GET4(DEH->identifier);
      GET4(DEH->version);

      /* NOTE - the tests below could be made on DE->type instead */

      if (DEH->identifier == X3F_SECp)
      {
        x3f_property_list_t *PL = &DEH->data_subsection.property_list;
        if (!PL)
          goto _err;
        /* Read the property part of the header */
        GET4(PL->num_properties);
        GET4(PL->character_format);
        GET4(PL->reserved);
        GET4(PL->total_length);

        /* Set all not read data block pointers to NULL */
        PL->data = NULL;
        PL->data_size = 0;
      }

      if (DEH->identifier == X3F_SECi)
      {
        x3f_image_data_t *ID = &DEH->data_subsection.image_data;
        if (!ID)
          goto _err;
        /* Read the image part of the header */
        GET4(ID->type);
        GET4(ID->format);
        ID->type_format = (ID->type << 16) + (ID->format);
        GET4(ID->columns);
        GET4(ID->rows);
        GET4(ID->row_stride);

        /* Set all not read data block pointers to NULL */
        ID->huffman = NULL;

        ID->data = NULL;
        ID->data_size = 0;
      }

      if (DEH->identifier == X3F_SECc)
      {
        x3f_camf_t *CAMF = &DEH->data_subsection.camf;
        if (!CAMF)
          goto _err;
        /* Read the CAMF part of the header */
        GET4(CAMF->type);
        GET4(CAMF->tN.val0);
        GET4(CAMF->tN.val1);
        GET4(CAMF->tN.val2);
        GET4(CAMF->tN.val3);

        /* Set all not read data block pointers to NULL */
        CAMF->data = NULL;
        CAMF->data_size = 0;

        /* Set all not allocated help pointers to NULL */
        CAMF->table.element = NULL;
        CAMF->table.size = 0;
        CAMF->tree.nodes = NULL;
        CAMF->decoded_data = NULL;
        CAMF->decoded_data_size = 0;
        CAMF->entry_table.element = NULL;
        CAMF->entry_table.size = 0;
      }

      /* Reset the file pointer back to the directory */
      infile->seek(save_dir_pos, SEEK_SET);
    }

    return x3f;
  _err:
    if (x3f)
    {
      DS = &x3f->directory_section;
      if (DS && DS->directory_entry)
        free(DS->directory_entry);
      free(x3f);
    }
    return NULL;
  }
  catch (...)
  {
    x3f_directory_section_t *DS = &x3f->directory_section;
    if (DS && DS->directory_entry)
      free(DS->directory_entry);
    free(x3f);
    return NULL;
  }
}