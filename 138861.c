find_single_ipv6_orport(const smartlist_t *list,
                        tor_addr_t *addr_out,
                        uint16_t *port_out)
{
  int ret = 0;
  tor_assert(list != NULL);
  tor_assert(addr_out != NULL);
  tor_assert(port_out != NULL);

  SMARTLIST_FOREACH_BEGIN(list, directory_token_t *, t) {
    tor_addr_t a;
    maskbits_t bits;
    uint16_t port_min, port_max;
    tor_assert(t->n_args >= 1);
    /* XXXX Prop186 the full spec allows much more than this. */
    if (tor_addr_parse_mask_ports(t->args[0], 0,
                                  &a, &bits, &port_min,
                                  &port_max) == AF_INET6 &&
        bits == 128 &&
        port_min == port_max) {
      /* Okay, this is one we can understand. Use it and ignore
         any potential more addresses in list. */
      tor_addr_copy(addr_out, &a);
      *port_out = port_min;
      ret = 1;
      break;
    }
  } SMARTLIST_FOREACH_END(t);

  return ret;
}