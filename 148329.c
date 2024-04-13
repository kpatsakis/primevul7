asn1_expand_any_defined_by (asn1_node definitions, asn1_node * element)
{
  char name[2 * ASN1_MAX_NAME_SIZE + 1],
    value[ASN1_MAX_NAME_SIZE];
  int retCode = ASN1_SUCCESS, result;
  int len, len2, len3;
  asn1_node p, p2, p3, aux = NULL;
  char errorDescription[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
  const char *definitionsName;

  if ((definitions == NULL) || (*element == NULL))
    return ASN1_ELEMENT_NOT_FOUND;

  definitionsName = definitions->name;

  p = *element;
  while (p)
    {

      switch (type_field (p->type))
	{
	case ASN1_ETYPE_ANY:
	  if ((p->type & CONST_DEFINED_BY) && (p->value))
	    {
	      /* search the "DEF_BY" element */
	      p2 = p->down;
	      while ((p2) && (type_field (p2->type) != ASN1_ETYPE_CONSTANT))
		p2 = p2->right;

	      if (!p2)
		{
		  retCode = ASN1_ERROR_TYPE_ANY;
		  break;
		}

	      p3 = _asn1_get_up (p);

	      if (!p3)
		{
		  retCode = ASN1_ERROR_TYPE_ANY;
		  break;
		}

	      p3 = p3->down;
	      while (p3)
		{
		  if (!(strcmp (p3->name, p2->name)))
		    break;
		  p3 = p3->right;
		}

	      if ((!p3) || (type_field (p3->type) != ASN1_ETYPE_OBJECT_ID) ||
		  (p3->value == NULL))
		{

		  p3 = _asn1_get_up (p);
		  p3 = _asn1_get_up (p3);

		  if (!p3)
		    {
		      retCode = ASN1_ERROR_TYPE_ANY;
		      break;
		    }

		  p3 = p3->down;

		  while (p3)
		    {
		      if (!(strcmp (p3->name, p2->name)))
			break;
		      p3 = p3->right;
		    }

		  if ((!p3) || (type_field (p3->type) != ASN1_ETYPE_OBJECT_ID)
		      || (p3->value == NULL))
		    {
		      retCode = ASN1_ERROR_TYPE_ANY;
		      break;
		    }
		}

	      /* search the OBJECT_ID into definitions */
	      p2 = definitions->down;
	      while (p2)
		{
		  if ((type_field (p2->type) == ASN1_ETYPE_OBJECT_ID) &&
		      (p2->type & CONST_ASSIGN))
		    {
		      snprintf(name, sizeof(name), "%s.%s", definitionsName, p2->name);

		      len = ASN1_MAX_NAME_SIZE;
		      result =
			asn1_read_value (definitions, name, value, &len);

		      if ((result == ASN1_SUCCESS)
			  && (!_asn1_strcmp (p3->value, value)))
			{
			  p2 = p2->right;	/* pointer to the structure to
						   use for expansion */
			  while ((p2) && (p2->type & CONST_ASSIGN))
			    p2 = p2->right;

			  if (p2)
			    {
			      snprintf(name, sizeof(name), "%s.%s", definitionsName, p2->name);

			      result =
				asn1_create_element (definitions, name, &aux);
			      if (result == ASN1_SUCCESS)
				{
				  _asn1_cpy_name (aux, p);
				  len2 =
				    asn1_get_length_der (p->value,
							 p->value_len, &len3);
				  if (len2 < 0)
				    return ASN1_DER_ERROR;

				  result =
				    asn1_der_decoding (&aux, p->value + len3,
						       len2,
						       errorDescription);
				  if (result == ASN1_SUCCESS)
				    {

				      _asn1_set_right (aux, p->right);
				      _asn1_set_right (p, aux);

				      result = asn1_delete_structure (&p);
				      if (result == ASN1_SUCCESS)
					{
					  p = aux;
					  aux = NULL;
					  break;
					}
				      else
					{	/* error with asn1_delete_structure */
					  asn1_delete_structure (&aux);
					  retCode = result;
					  break;
					}
				    }
				  else
				    {	/* error with asn1_der_decoding */
				      retCode = result;
				      break;
				    }
				}
			      else
				{	/* error with asn1_create_element */
				  retCode = result;
				  break;
				}
			    }
			  else
			    {	/* error with the pointer to the structure to exapand */
			      retCode = ASN1_ERROR_TYPE_ANY;
			      break;
			    }
			}
		    }
		  p2 = p2->right;
		}		/* end while */

	      if (!p2)
		{
		  retCode = ASN1_ERROR_TYPE_ANY;
		  break;
		}

	    }
	  break;
	default:
	  break;
	}


      if (p->down)
	{
	  p = p->down;
	}
      else if (p == *element)
	{
	  p = NULL;
	  break;
	}
      else if (p->right)
	p = p->right;
      else
	{
	  while (1)
	    {
	      p = _asn1_get_up (p);
	      if (p == *element)
		{
		  p = NULL;
		  break;
		}
	      if (p->right)
		{
		  p = p->right;
		  break;
		}
	    }
	}
    }

  return retCode;
}