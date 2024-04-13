static void set_matrix_element_info(uint32_t type, uint32_t *size,
                                    matrix_type_t *decoded_type)
{
  switch (type)
  {
  case 0:
    *size = 2;
    *decoded_type = M_INT; /* known to be true */
    break;
  case 1:
    *size = 4;
    *decoded_type = M_UINT; /* TODO: unknown ???? */
    break;
  case 2:
    *size = 4;
    *decoded_type = M_UINT; /* TODO: unknown ???? */
    break;
  case 3:
    *size = 4;
    *decoded_type = M_FLOAT; /* known to be true */
    break;
  case 5:
    *size = 1;
    *decoded_type = M_UINT; /* TODO: unknown ???? */
    break;
  case 6:
    *size = 2;
    *decoded_type = M_UINT; /* TODO: unknown ???? */
    break;
  default:
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  }
}