_asn1_get_octet_string (asn1_node node, const unsigned char *der, int der_len,
                        int *len, unsigned flags)
{
  int len2, len3, counter, tot_len, indefinite;
  int result;
  int orig_der_len = der_len;

  counter = 0;

  if (*(der - 1) & ASN1_CLASS_STRUCTURED)
    {
      tot_len = 0;

      indefinite = asn1_get_length_der (der, der_len, &len3);
      if (IS_ERR(indefinite, flags))
	{
	  warn();
	  return ASN1_DER_ERROR;
	}

      counter += len3;
      DECR_LEN(der_len, len3);

      if (indefinite >= 0)
	indefinite += len3;

      while (1)
	{
	  if (indefinite == -1)
	    {
	      if (HAVE_TWO(der_len) && (der[counter] == 0) && (der[counter + 1] == 0))
		{
		  counter += 2;
		  DECR_LEN(der_len, 2);
		  break;
		}
	    }
	  else if (counter >= indefinite)
	    break;

          DECR_LEN(der_len, 1);
	  if (der[counter] != ASN1_TAG_OCTET_STRING)
	    {
	      warn();
	      return ASN1_DER_ERROR;
	    }

	  counter++;

	  len2 = asn1_get_length_der (der + counter, der_len, &len3);
	  if (len2 <= 0)
	    {
	      warn();
	      return ASN1_DER_ERROR;
	    }

          DECR_LEN(der_len, len3 + len2);
	  counter += len3 + len2;

	  tot_len += len2;
	}

      /* copy */
      if (node)
	{
	  unsigned char temp[ASN1_MAX_LENGTH_SIZE];
	  int ret;

	  len2 = sizeof (temp);

	  asn1_length_der (tot_len, temp, &len2);
	  _asn1_set_value (node, temp, len2);

	  ret = _asn1_extract_der_octet (node, der, orig_der_len, flags);
	  if (ret != ASN1_SUCCESS)
	    {
	      warn();
	      return ret;
	    }

	}
    }
  else
    {				/* NOT STRUCTURED */
      len2 = asn1_get_length_der (der, der_len, &len3);
      if (len2 < 0)
        {
          warn();
	  return ASN1_DER_ERROR;
	}

      DECR_LEN(der_len, len3+len2);
      counter = len3 + len2;
      if (node)
	_asn1_set_value (node, der, counter);
    }

  *len = counter;
  return ASN1_SUCCESS;

cleanup:
  return result;
}