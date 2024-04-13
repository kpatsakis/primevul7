_asn1_delete_not_used (asn1_node node)
{
  asn1_node p, p2;

  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  p = node;
  while (p)
    {
      if (p->type & CONST_NOT_USED)
	{
	  p2 = NULL;
	  if (p != node)
	    {
	      p2 = _asn1_find_left (p);
	      if (!p2)
		p2 = _asn1_get_up (p);
	    }
	  asn1_delete_structure (&p);
	  p = p2;
	}

      if (!p)
	break;			/* reach node */

      if (p->down)
	{
	  p = p->down;
	}
      else
	{
	  if (p == node)
	    p = NULL;
	  else if (p->right)
	    p = p->right;
	  else
	    {
	      while (1)
		{
		  p = _asn1_get_up (p);
		  if (p == node)
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
    }
  return ASN1_SUCCESS;
}