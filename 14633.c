set_sock_tos(int fd, int tos)
{
#ifndef IP_TOS_IS_BROKEN
	int af;

	switch ((af = get_sock_af(fd))) {
	case -1:
		/* assume not a socket */
		break;
	case AF_INET:
# ifdef IP_TOS
		debug3_f("set socket %d IP_TOS 0x%02x", fd, tos);
		if (setsockopt(fd, IPPROTO_IP, IP_TOS,
		    &tos, sizeof(tos)) == -1) {
			error("setsockopt socket %d IP_TOS %d: %s:",
			    fd, tos, strerror(errno));
		}
# endif /* IP_TOS */
		break;
	case AF_INET6:
# ifdef IPV6_TCLASS
		debug3_f("set socket %d IPV6_TCLASS 0x%02x", fd, tos);
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_TCLASS,
		    &tos, sizeof(tos)) == -1) {
			error("setsockopt socket %d IPV6_TCLASS %d: %.100s:",
			    fd, tos, strerror(errno));
		}
# endif /* IPV6_TCLASS */
		break;
	default:
		debug2_f("unsupported socket family %d", af);
		break;
	}
#endif /* IP_TOS_IS_BROKEN */
}