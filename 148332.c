asn1_der_decoding_startEnd (asn1_node element, const void *ider, int ider_len,
			    const char *name_element, int *start, int *end)
{
  asn1_node node, node_to_find;
  int result = ASN1_DER_ERROR;

  node = element;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  node_to_find = asn1_find_node (node, name_element);

  if (node_to_find == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  *start = node_to_find->start;
  *end = node_to_find->end;

  if (*start == 0 && *end == 0)
    {
      if (ider == NULL || ider_len == 0)
        return ASN1_GENERIC_ERROR;

      /* it seems asn1_der_decoding() wasn't called before. Do it now */
      result = asn1_der_decoding (&node, ider, ider_len, NULL);
      if (result != ASN1_SUCCESS)
        {
          warn();
          return result;
        }

      node_to_find = asn1_find_node (node, name_element);
      if (node_to_find == NULL)
        return ASN1_ELEMENT_NOT_FOUND;

      *start = node_to_find->start;
      *end = node_to_find->end;
    }

  if (*end < *start)
    return ASN1_GENERIC_ERROR;

  return ASN1_SUCCESS;
}