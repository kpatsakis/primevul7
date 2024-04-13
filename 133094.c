ospf_ls_upd_list_lsa (struct ospf_neighbor *nbr, struct stream *s,
                      struct ospf_interface *oi, size_t size)
{
  u_int16_t count, sum;
  u_int32_t length;
  struct lsa_header *lsah;
  struct ospf_lsa *lsa;
  struct list *lsas;

  lsas = list_new ();

  count = stream_getl (s);
  size -= OSPF_LS_UPD_MIN_SIZE; /* # LSAs */

  for (; size >= OSPF_LSA_HEADER_SIZE && count > 0;
       size -= length, stream_forward_getp (s, length), count--)
    {
      lsah = (struct lsa_header *) STREAM_PNT (s);
      length = ntohs (lsah->length);

      if (length > size)
	{
	  zlog_warn ("Link State Update: LSA length exceeds packet size.");
	  break;
	}

      /* Validate the LSA's LS checksum. */
      sum = lsah->checksum;
      if (sum != ospf_lsa_checksum (lsah))
	{
	  zlog_warn ("Link State Update: LSA checksum error %x, %x.",
		     sum, lsah->checksum);
	  continue;
	}

      /* Examine the LSA's LS type. */
      if (lsah->type < OSPF_MIN_LSA || lsah->type >= OSPF_MAX_LSA)
	{
	  zlog_warn ("Link State Update: Unknown LS type %d", lsah->type);
	  continue;
	}

      /*
       * What if the received LSA's age is greater than MaxAge?
       * Treat it as a MaxAge case -- endo.
       */
      if (ntohs (lsah->ls_age) > OSPF_LSA_MAXAGE)
        lsah->ls_age = htons (OSPF_LSA_MAXAGE);

#ifdef HAVE_OPAQUE_LSA
      if (CHECK_FLAG (nbr->options, OSPF_OPTION_O))
        {
#ifdef STRICT_OBIT_USAGE_CHECK
	  if ((IS_OPAQUE_LSA(lsah->type) &&
               ! CHECK_FLAG (lsah->options, OSPF_OPTION_O))
	  ||  (! IS_OPAQUE_LSA(lsah->type) &&
               CHECK_FLAG (lsah->options, OSPF_OPTION_O)))
            {
              /*
               * This neighbor must know the exact usage of O-bit;
               * the bit will be set in Type-9,10,11 LSAs only.
               */
              zlog_warn ("LSA[Type%d:%s]: O-bit abuse?", lsah->type, inet_ntoa (lsah->id));
              continue;
            }
#endif /* STRICT_OBIT_USAGE_CHECK */

          /* Do not take in AS External Opaque-LSAs if we are a stub. */
          if (lsah->type == OSPF_OPAQUE_AS_LSA
	      && nbr->oi->area->external_routing != OSPF_AREA_DEFAULT) 
            {
              if (IS_DEBUG_OSPF_EVENT)
                zlog_debug ("LSA[Type%d:%s]: We are a stub, don't take this LSA.", lsah->type, inet_ntoa (lsah->id));
              continue;
            }
        }
      else if (IS_OPAQUE_LSA(lsah->type))
        {
          zlog_warn ("LSA[Type%d:%s]: Opaque capability mismatch?", lsah->type, inet_ntoa (lsah->id));
          continue;
        }
#endif /* HAVE_OPAQUE_LSA */

      /* Create OSPF LSA instance. */
      lsa = ospf_lsa_new ();

      /* We may wish to put some error checking if type NSSA comes in
         and area not in NSSA mode */
      switch (lsah->type)
        {
        case OSPF_AS_EXTERNAL_LSA:
#ifdef HAVE_OPAQUE_LSA
        case OSPF_OPAQUE_AS_LSA:
          lsa->area = NULL;
          break;
        case OSPF_OPAQUE_LINK_LSA:
          lsa->oi = oi; /* Remember incoming interface for flooding control. */
          /* Fallthrough */
#endif /* HAVE_OPAQUE_LSA */
        default:
          lsa->area = oi->area;
          break;
        }

      lsa->data = ospf_lsa_data_new (length);
      memcpy (lsa->data, lsah, length);

      if (IS_DEBUG_OSPF_EVENT)
	zlog_debug("LSA[Type%d:%s]: %p new LSA created with Link State Update",
		  lsa->data->type, inet_ntoa (lsa->data->id), lsa);
      listnode_add (lsas, lsa);
    }

  return lsas;
}