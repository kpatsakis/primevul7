static inline bool xfrm_id_proto_valid(u8 proto)
{
	switch (proto) {
	case IPPROTO_AH:
	case IPPROTO_ESP:
	case IPPROTO_COMP:
#if IS_ENABLED(CONFIG_IPV6)
	case IPPROTO_ROUTING:
	case IPPROTO_DSTOPTS:
#endif
		return true;
	default:
		return false;
	}
}