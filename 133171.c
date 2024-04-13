static void get_matrix_copy(camf_entry_t *entry)
{
  uint32_t element_size = entry->matrix_element_size;
  uint32_t elements = entry->matrix_elements;
  int i, size = (entry->matrix_decoded_type == M_FLOAT ? sizeof(double)
                                                       : sizeof(uint32_t)) *
                elements;

  entry->matrix_decoded = malloc(size);

  switch (element_size)
  {
  case 4:
    switch (entry->matrix_decoded_type)
    {
    case M_INT:
    case M_UINT:
      memcpy(entry->matrix_decoded, entry->matrix_data, size);
      break;
    case M_FLOAT:
      for (i = 0; i < elements; i++)
        ((double *)entry->matrix_decoded)[i] =
            (double)((float *)entry->matrix_data)[i];
      break;
    default:
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    }
    break;
  case 2:
    switch (entry->matrix_decoded_type)
    {
    case M_INT:
      for (i = 0; i < elements; i++)
        ((int32_t *)entry->matrix_decoded)[i] =
            (int32_t)((int16_t *)entry->matrix_data)[i];
      break;
    case M_UINT:
      for (i = 0; i < elements; i++)
        ((uint32_t *)entry->matrix_decoded)[i] =
            (uint32_t)((uint16_t *)entry->matrix_data)[i];
      break;
    default:
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    }
    break;
  case 1:
    switch (entry->matrix_decoded_type)
    {
    case M_INT:
      for (i = 0; i < elements; i++)
        ((int32_t *)entry->matrix_decoded)[i] =
            (int32_t)((int8_t *)entry->matrix_data)[i];
      break;
    case M_UINT:
      for (i = 0; i < elements; i++)
        ((uint32_t *)entry->matrix_decoded)[i] =
            (uint32_t)((uint8_t *)entry->matrix_data)[i];
      break;
    default:
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    }
    break;
  default:
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  }
}