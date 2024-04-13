static u32 sock_ops_convert_ctx_access(enum bpf_access_type type,
				       const struct bpf_insn *si,
				       struct bpf_insn *insn_buf,
				       struct bpf_prog *prog,
				       u32 *target_size)
{
	struct bpf_insn *insn = insn_buf;
	int off;

	switch (si->off) {
	case offsetof(struct bpf_sock_ops, op) ...
	     offsetof(struct bpf_sock_ops, replylong[3]):
		BUILD_BUG_ON(FIELD_SIZEOF(struct bpf_sock_ops, op) !=
			     FIELD_SIZEOF(struct bpf_sock_ops_kern, op));
		BUILD_BUG_ON(FIELD_SIZEOF(struct bpf_sock_ops, reply) !=
			     FIELD_SIZEOF(struct bpf_sock_ops_kern, reply));
		BUILD_BUG_ON(FIELD_SIZEOF(struct bpf_sock_ops, replylong) !=
			     FIELD_SIZEOF(struct bpf_sock_ops_kern, replylong));
		off = si->off;
		off -= offsetof(struct bpf_sock_ops, op);
		off += offsetof(struct bpf_sock_ops_kern, op);
		if (type == BPF_WRITE)
			*insn++ = BPF_STX_MEM(BPF_W, si->dst_reg, si->src_reg,
					      off);
		else
			*insn++ = BPF_LDX_MEM(BPF_W, si->dst_reg, si->src_reg,
					      off);
		break;

	case offsetof(struct bpf_sock_ops, family):
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common, skc_family) != 2);

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
					      struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_H, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common, skc_family));
		break;

	case offsetof(struct bpf_sock_ops, remote_ip4):
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common, skc_daddr) != 4);

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_W, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common, skc_daddr));
		break;

	case offsetof(struct bpf_sock_ops, local_ip4):
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common, skc_rcv_saddr) != 4);

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
					      struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_W, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common,
					       skc_rcv_saddr));
		break;

	case offsetof(struct bpf_sock_ops, remote_ip6[0]) ...
	     offsetof(struct bpf_sock_ops, remote_ip6[3]):
#if IS_ENABLED(CONFIG_IPV6)
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common,
					  skc_v6_daddr.s6_addr32[0]) != 4);

		off = si->off;
		off -= offsetof(struct bpf_sock_ops, remote_ip6[0]);
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_W, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common,
					       skc_v6_daddr.s6_addr32[0]) +
				      off);
#else
		*insn++ = BPF_MOV32_IMM(si->dst_reg, 0);
#endif
		break;

	case offsetof(struct bpf_sock_ops, local_ip6[0]) ...
	     offsetof(struct bpf_sock_ops, local_ip6[3]):
#if IS_ENABLED(CONFIG_IPV6)
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common,
					  skc_v6_rcv_saddr.s6_addr32[0]) != 4);

		off = si->off;
		off -= offsetof(struct bpf_sock_ops, local_ip6[0]);
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_W, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common,
					       skc_v6_rcv_saddr.s6_addr32[0]) +
				      off);
#else
		*insn++ = BPF_MOV32_IMM(si->dst_reg, 0);
#endif
		break;

	case offsetof(struct bpf_sock_ops, remote_port):
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common, skc_dport) != 2);

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_H, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common, skc_dport));
#ifndef __BIG_ENDIAN_BITFIELD
		*insn++ = BPF_ALU32_IMM(BPF_LSH, si->dst_reg, 16);
#endif
		break;

	case offsetof(struct bpf_sock_ops, local_port):
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common, skc_num) != 2);

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_H, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common, skc_num));
		break;

	case offsetof(struct bpf_sock_ops, is_fullsock):
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern,
						is_fullsock),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern,
					       is_fullsock));
		break;

	case offsetof(struct bpf_sock_ops, state):
		BUILD_BUG_ON(FIELD_SIZEOF(struct sock_common, skc_state) != 1);

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_B, si->dst_reg, si->dst_reg,
				      offsetof(struct sock_common, skc_state));
		break;

	case offsetof(struct bpf_sock_ops, rtt_min):
		BUILD_BUG_ON(FIELD_SIZEOF(struct tcp_sock, rtt_min) !=
			     sizeof(struct minmax));
		BUILD_BUG_ON(sizeof(struct minmax) <
			     sizeof(struct minmax_sample));

		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(
						struct bpf_sock_ops_kern, sk),
				      si->dst_reg, si->src_reg,
				      offsetof(struct bpf_sock_ops_kern, sk));
		*insn++ = BPF_LDX_MEM(BPF_W, si->dst_reg, si->dst_reg,
				      offsetof(struct tcp_sock, rtt_min) +
				      FIELD_SIZEOF(struct minmax_sample, t));
		break;

/* Helper macro for adding read access to tcp_sock or sock fields. */
#define SOCK_OPS_GET_FIELD(BPF_FIELD, OBJ_FIELD, OBJ)			      \
	do {								      \
		BUILD_BUG_ON(FIELD_SIZEOF(OBJ, OBJ_FIELD) >		      \
			     FIELD_SIZEOF(struct bpf_sock_ops, BPF_FIELD));   \
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(			      \
						struct bpf_sock_ops_kern,     \
						is_fullsock),		      \
				      si->dst_reg, si->src_reg,		      \
				      offsetof(struct bpf_sock_ops_kern,      \
					       is_fullsock));		      \
		*insn++ = BPF_JMP_IMM(BPF_JEQ, si->dst_reg, 0, 2);	      \
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(			      \
						struct bpf_sock_ops_kern, sk),\
				      si->dst_reg, si->src_reg,		      \
				      offsetof(struct bpf_sock_ops_kern, sk));\
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(OBJ,		      \
						       OBJ_FIELD),	      \
				      si->dst_reg, si->dst_reg,		      \
				      offsetof(OBJ, OBJ_FIELD));	      \
	} while (0)

/* Helper macro for adding write access to tcp_sock or sock fields.
 * The macro is called with two registers, dst_reg which contains a pointer
 * to ctx (context) and src_reg which contains the value that should be
 * stored. However, we need an additional register since we cannot overwrite
 * dst_reg because it may be used later in the program.
 * Instead we "borrow" one of the other register. We first save its value
 * into a new (temp) field in bpf_sock_ops_kern, use it, and then restore
 * it at the end of the macro.
 */
#define SOCK_OPS_SET_FIELD(BPF_FIELD, OBJ_FIELD, OBJ)			      \
	do {								      \
		int reg = BPF_REG_9;					      \
		BUILD_BUG_ON(FIELD_SIZEOF(OBJ, OBJ_FIELD) >		      \
			     FIELD_SIZEOF(struct bpf_sock_ops, BPF_FIELD));   \
		if (si->dst_reg == reg || si->src_reg == reg)		      \
			reg--;						      \
		if (si->dst_reg == reg || si->src_reg == reg)		      \
			reg--;						      \
		*insn++ = BPF_STX_MEM(BPF_DW, si->dst_reg, reg,		      \
				      offsetof(struct bpf_sock_ops_kern,      \
					       temp));			      \
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(			      \
						struct bpf_sock_ops_kern,     \
						is_fullsock),		      \
				      reg, si->dst_reg,			      \
				      offsetof(struct bpf_sock_ops_kern,      \
					       is_fullsock));		      \
		*insn++ = BPF_JMP_IMM(BPF_JEQ, reg, 0, 2);		      \
		*insn++ = BPF_LDX_MEM(BPF_FIELD_SIZEOF(			      \
						struct bpf_sock_ops_kern, sk),\
				      reg, si->dst_reg,			      \
				      offsetof(struct bpf_sock_ops_kern, sk));\
		*insn++ = BPF_STX_MEM(BPF_FIELD_SIZEOF(OBJ, OBJ_FIELD),	      \
				      reg, si->src_reg,			      \
				      offsetof(OBJ, OBJ_FIELD));	      \
		*insn++ = BPF_LDX_MEM(BPF_DW, reg, si->dst_reg,		      \
				      offsetof(struct bpf_sock_ops_kern,      \
					       temp));			      \
	} while (0)

#define SOCK_OPS_GET_OR_SET_FIELD(BPF_FIELD, OBJ_FIELD, OBJ, TYPE)	      \
	do {								      \
		if (TYPE == BPF_WRITE)					      \
			SOCK_OPS_SET_FIELD(BPF_FIELD, OBJ_FIELD, OBJ);	      \
		else							      \
			SOCK_OPS_GET_FIELD(BPF_FIELD, OBJ_FIELD, OBJ);	      \
	} while (0)

	case offsetof(struct bpf_sock_ops, snd_cwnd):
		SOCK_OPS_GET_FIELD(snd_cwnd, snd_cwnd, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, srtt_us):
		SOCK_OPS_GET_FIELD(srtt_us, srtt_us, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, bpf_sock_ops_cb_flags):
		SOCK_OPS_GET_FIELD(bpf_sock_ops_cb_flags, bpf_sock_ops_cb_flags,
				   struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, snd_ssthresh):
		SOCK_OPS_GET_FIELD(snd_ssthresh, snd_ssthresh, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, rcv_nxt):
		SOCK_OPS_GET_FIELD(rcv_nxt, rcv_nxt, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, snd_nxt):
		SOCK_OPS_GET_FIELD(snd_nxt, snd_nxt, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, snd_una):
		SOCK_OPS_GET_FIELD(snd_una, snd_una, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, mss_cache):
		SOCK_OPS_GET_FIELD(mss_cache, mss_cache, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, ecn_flags):
		SOCK_OPS_GET_FIELD(ecn_flags, ecn_flags, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, rate_delivered):
		SOCK_OPS_GET_FIELD(rate_delivered, rate_delivered,
				   struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, rate_interval_us):
		SOCK_OPS_GET_FIELD(rate_interval_us, rate_interval_us,
				   struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, packets_out):
		SOCK_OPS_GET_FIELD(packets_out, packets_out, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, retrans_out):
		SOCK_OPS_GET_FIELD(retrans_out, retrans_out, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, total_retrans):
		SOCK_OPS_GET_FIELD(total_retrans, total_retrans,
				   struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, segs_in):
		SOCK_OPS_GET_FIELD(segs_in, segs_in, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, data_segs_in):
		SOCK_OPS_GET_FIELD(data_segs_in, data_segs_in, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, segs_out):
		SOCK_OPS_GET_FIELD(segs_out, segs_out, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, data_segs_out):
		SOCK_OPS_GET_FIELD(data_segs_out, data_segs_out,
				   struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, lost_out):
		SOCK_OPS_GET_FIELD(lost_out, lost_out, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, sacked_out):
		SOCK_OPS_GET_FIELD(sacked_out, sacked_out, struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, sk_txhash):
		SOCK_OPS_GET_OR_SET_FIELD(sk_txhash, sk_txhash,
					  struct sock, type);
		break;

	case offsetof(struct bpf_sock_ops, bytes_received):
		SOCK_OPS_GET_FIELD(bytes_received, bytes_received,
				   struct tcp_sock);
		break;

	case offsetof(struct bpf_sock_ops, bytes_acked):
		SOCK_OPS_GET_FIELD(bytes_acked, bytes_acked, struct tcp_sock);
		break;

	}
	return insn - insn_buf;
}