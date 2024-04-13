ospf_db_desc_proc (struct stream *s, struct ospf_interface *oi,
		   struct ospf_neighbor *nbr, struct ospf_db_desc *dd,
		   u_int16_t size)
{
  struct ospf_lsa *new, *find;
  struct lsa_header *lsah;

  stream_forward_getp (s, OSPF_DB_DESC_MIN_SIZE);
  for (size -= OSPF_DB_DESC_MIN_SIZE;
       size >= OSPF_LSA_HEADER_SIZE; size -= OSPF_LSA_HEADER_SIZE) 
    {
      lsah = (struct lsa_header *) STREAM_PNT (s);
      stream_forward_getp (s, OSPF_LSA_HEADER_SIZE);

      /* Unknown LS type. */
      if (lsah->type < OSPF_MIN_LSA || lsah->type >= OSPF_MAX_LSA)
	{
	  zlog_warn ("Packet [DD:RECV]: Unknown LS type %d.", lsah->type);
	  OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
	  return;
	}

#ifdef HAVE_OPAQUE_LSA
      if (IS_OPAQUE_LSA (lsah->type)
      &&  ! CHECK_FLAG (nbr->options, OSPF_OPTION_O))
        {
          zlog_warn ("LSA[Type%d:%s]: Opaque capability mismatch?", lsah->type, inet_ntoa (lsah->id));
          OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
          return;
        }
#endif /* HAVE_OPAQUE_LSA */

      switch (lsah->type)
        {
        case OSPF_AS_EXTERNAL_LSA:
#ifdef HAVE_OPAQUE_LSA
	case OSPF_OPAQUE_AS_LSA:
#endif /* HAVE_OPAQUE_LSA */
          /* Check for stub area.  Reject if AS-External from stub but
             allow if from NSSA. */
          if (oi->area->external_routing == OSPF_AREA_STUB)
            {
              zlog_warn ("Packet [DD:RECV]: LSA[Type%d:%s] from %s area.",
                         lsah->type, inet_ntoa (lsah->id),
                         (oi->area->external_routing == OSPF_AREA_STUB) ?\
                         "STUB" : "NSSA");
              OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_SeqNumberMismatch);
              return;
            }
          break;
	default:
	  break;
        }

      /* Create LS-request object. */
      new = ospf_ls_request_new (lsah);

      /* Lookup received LSA, then add LS request list. */
      find = ospf_lsa_lookup_by_header (oi->area, lsah);
      
      /* ospf_lsa_more_recent is fine with NULL pointers */
      switch (ospf_lsa_more_recent (find, new))
        {
          case -1:
            /* Neighbour has a more recent LSA, we must request it */
            ospf_ls_request_add (nbr, new);
          case 0:
            /* If we have a copy of this LSA, it's either less recent
             * and we're requesting it from neighbour (the case above), or
             * it's as recent and we both have same copy (this case).
             *
             * In neither of these two cases is there any point in
             * describing our copy of the LSA to the neighbour in a
             * DB-Summary packet, if we're still intending to do so.
             *
             * See: draft-ogier-ospf-dbex-opt-00.txt, describing the
             * backward compatible optimisation to OSPF DB Exchange /
             * DB Description process implemented here.
             */
            if (find)
              ospf_lsdb_delete (&nbr->db_sum, find);
            ospf_lsa_discard (new);
            break;
          default:
            /* We have the more recent copy, nothing specific to do:
             * - no need to request neighbours stale copy
             * - must leave DB summary list copy alone
             */
            if (IS_DEBUG_OSPF_EVENT)
              zlog_debug ("Packet [DD:RECV]: LSA received Type %d, "
                         "ID %s is not recent.", lsah->type, inet_ntoa (lsah->id));
            ospf_lsa_discard (new);
        }
    }

  /* Master */
  if (IS_SET_DD_MS (nbr->dd_flags))
    {
      nbr->dd_seqnum++;

      /* Both sides have no More, then we're done with Exchange */
      if (!IS_SET_DD_M (dd->flags) && !IS_SET_DD_M (nbr->dd_flags))
	OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_ExchangeDone);
      else
	ospf_db_desc_send (nbr);
    }
  /* Slave */
  else
    {
      nbr->dd_seqnum = ntohl (dd->dd_seqnum);

      /* Send DD packet in reply. 
       * 
       * Must be done to acknowledge the Master's DD, regardless of
       * whether we have more LSAs ourselves to describe.
       *
       * This function will clear the 'More' bit, if after this DD
       * we have no more LSAs to describe to the master..
       */
      ospf_db_desc_send (nbr);
      
      /* Slave can raise ExchangeDone now, if master is also done */
      if (!IS_SET_DD_M (dd->flags) && !IS_SET_DD_M (nbr->dd_flags))
	OSPF_NSM_EVENT_SCHEDULE (nbr, NSM_ExchangeDone);
    }
  
  /* Save received neighbor values from DD. */
  ospf_db_desc_save_current (nbr, dd);
}