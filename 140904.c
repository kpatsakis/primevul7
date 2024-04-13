ctl_putsys(
	int varid
	)
{
	l_fp tmp;
	char str[256];
	u_int u;
	double kb;
	double dtemp;
	const char *ss;
#ifdef AUTOKEY
	struct cert_info *cp;
#endif	/* AUTOKEY */
#ifdef KERNEL_PLL
	static struct timex ntx;
	static u_long ntp_adjtime_time;

	static const double to_ms =
# ifdef STA_NANO
		1.0e-6; /* nsec to msec */
# else
		1.0e-3; /* usec to msec */
# endif

	/*
	 * CS_K_* variables depend on up-to-date output of ntp_adjtime()
	 */
	if (CS_KERN_FIRST <= varid && varid <= CS_KERN_LAST &&
	    current_time != ntp_adjtime_time) {
		ZERO(ntx);
		if (ntp_adjtime(&ntx) < 0)
			msyslog(LOG_ERR, "ntp_adjtime() for mode 6 query failed: %m");
		else
			ntp_adjtime_time = current_time;
	}
#endif	/* KERNEL_PLL */

	switch (varid) {

	case CS_LEAP:
		ctl_putuint(sys_var[CS_LEAP].text, sys_leap);
		break;

	case CS_STRATUM:
		ctl_putuint(sys_var[CS_STRATUM].text, sys_stratum);
		break;

	case CS_PRECISION:
		ctl_putint(sys_var[CS_PRECISION].text, sys_precision);
		break;

	case CS_ROOTDELAY:
		ctl_putdbl(sys_var[CS_ROOTDELAY].text, sys_rootdelay *
			   1e3);
		break;

	case CS_ROOTDISPERSION:
		ctl_putdbl(sys_var[CS_ROOTDISPERSION].text,
			   sys_rootdisp * 1e3);
		break;

	case CS_REFID:
		if (sys_stratum > 1 && sys_stratum < STRATUM_UNSPEC)
			ctl_putadr(sys_var[varid].text, sys_refid, NULL);
		else
			ctl_putrefid(sys_var[varid].text, sys_refid);
		break;

	case CS_REFTIME:
		ctl_putts(sys_var[CS_REFTIME].text, &sys_reftime);
		break;

	case CS_POLL:
		ctl_putuint(sys_var[CS_POLL].text, sys_poll);
		break;

	case CS_PEERID:
		if (sys_peer == NULL)
			ctl_putuint(sys_var[CS_PEERID].text, 0);
		else
			ctl_putuint(sys_var[CS_PEERID].text,
				    sys_peer->associd);
		break;

	case CS_PEERADR:
		if (sys_peer != NULL && sys_peer->dstadr != NULL)
			ss = sptoa(&sys_peer->srcadr);
		else
			ss = "0.0.0.0:0";
		ctl_putunqstr(sys_var[CS_PEERADR].text, ss, strlen(ss));
		break;

	case CS_PEERMODE:
		u = (sys_peer != NULL)
			? sys_peer->hmode
			: MODE_UNSPEC;
		ctl_putuint(sys_var[CS_PEERMODE].text, u);
		break;

	case CS_OFFSET:
		ctl_putdbl6(sys_var[CS_OFFSET].text, last_offset * 1e3);
		break;

	case CS_DRIFT:
		ctl_putdbl(sys_var[CS_DRIFT].text, drift_comp * 1e6);
		break;

	case CS_JITTER:
		ctl_putdbl6(sys_var[CS_JITTER].text, sys_jitter * 1e3);
		break;

	case CS_ERROR:
		ctl_putdbl(sys_var[CS_ERROR].text, clock_jitter * 1e3);
		break;

	case CS_CLOCK:
		get_systime(&tmp);
		ctl_putts(sys_var[CS_CLOCK].text, &tmp);
		break;

	case CS_PROCESSOR:
#ifndef HAVE_UNAME
		ctl_putstr(sys_var[CS_PROCESSOR].text, str_processor,
			   sizeof(str_processor) - 1);
#else
		ctl_putstr(sys_var[CS_PROCESSOR].text,
			   utsnamebuf.machine, strlen(utsnamebuf.machine));
#endif /* HAVE_UNAME */
		break;

	case CS_SYSTEM:
#ifndef HAVE_UNAME
		ctl_putstr(sys_var[CS_SYSTEM].text, str_system,
			   sizeof(str_system) - 1);
#else
		snprintf(str, sizeof(str), "%s/%s", utsnamebuf.sysname,
			 utsnamebuf.release);
		ctl_putstr(sys_var[CS_SYSTEM].text, str, strlen(str));
#endif /* HAVE_UNAME */
		break;

	case CS_VERSION:
		ctl_putstr(sys_var[CS_VERSION].text, Version,
			   strlen(Version));
		break;

	case CS_STABIL:
		ctl_putdbl(sys_var[CS_STABIL].text, clock_stability *
			   1e6);
		break;

	case CS_VARLIST:
	{
		char buf[CTL_MAX_DATA_LEN];
		//buffPointer, firstElementPointer, buffEndPointer
		char *buffp, *buffend;
		int firstVarName;
		const char *ss1;
		int len;
		const struct ctl_var *k;

		buffp = buf;
		buffend = buf + sizeof(buf);
		if (buffp + strlen(sys_var[CS_VARLIST].text) + 4 > buffend)
			break;	/* really long var name */

		snprintf(buffp, sizeof(buf), "%s=\"",sys_var[CS_VARLIST].text);
		buffp += strlen(buffp);
		firstVarName = TRUE;
		for (k = sys_var; !(k->flags & EOV); k++) {
			if (k->flags & PADDING)
				continue;
			len = strlen(k->text);
			if (buffp + len + 1 >= buffend)
				break;
			if (!firstVarName)
				*buffp++ = ',';
			else
				firstVarName = FALSE;
			memcpy(buffp, k->text, len);
			buffp += len;
		}

		for (k = ext_sys_var; k && !(k->flags & EOV); k++) {
			if (k->flags & PADDING)
				continue;
			if (NULL == k->text)
				continue;
			ss1 = strchr(k->text, '=');
			if (NULL == ss1)
				len = strlen(k->text);
			else
				len = ss1 - k->text;
			if (buffp + len + 1 >= buffend)
				break;
			if (firstVarName) {
				*buffp++ = ',';
				firstVarName = FALSE;
			}
			memcpy(buffp, k->text,(unsigned)len);
			buffp += len;
		}
		if (buffp + 2 >= buffend)
			break;

		*buffp++ = '"';
		*buffp = '\0';

		ctl_putdata(buf, (unsigned)( buffp - buf ), 0);
		break;
	}

	case CS_TAI:
		if (sys_tai > 0)
			ctl_putuint(sys_var[CS_TAI].text, sys_tai);
		break;

	case CS_LEAPTAB:
	{
		leap_signature_t lsig;
		leapsec_getsig(&lsig);
		if (lsig.ttime > 0)
			ctl_putfs(sys_var[CS_LEAPTAB].text, lsig.ttime);
		break;
	}

	case CS_LEAPEND:
	{
		leap_signature_t lsig;
		leapsec_getsig(&lsig);
		if (lsig.etime > 0)
			ctl_putfs(sys_var[CS_LEAPEND].text, lsig.etime);
		break;
	}

#ifdef LEAP_SMEAR
	case CS_LEAPSMEARINTV:
		if (leap_smear_intv > 0)
			ctl_putuint(sys_var[CS_LEAPSMEARINTV].text, leap_smear_intv);
		break;

	case CS_LEAPSMEAROFFS:
		if (leap_smear_intv > 0)
			ctl_putdbl(sys_var[CS_LEAPSMEAROFFS].text,
				   leap_smear.doffset * 1e3);
		break;
#endif	/* LEAP_SMEAR */

	case CS_RATE:
		ctl_putuint(sys_var[CS_RATE].text, ntp_minpoll);
		break;

	case CS_MRU_ENABLED:
		ctl_puthex(sys_var[varid].text, mon_enabled);
		break;

	case CS_MRU_DEPTH:
		ctl_putuint(sys_var[varid].text, mru_entries);
		break;

	case CS_MRU_MEM:
		kb = mru_entries * (sizeof(mon_entry) / 1024.);
		u = (u_int)kb;
		if (kb - u >= 0.5)
			u++;
		ctl_putuint(sys_var[varid].text, u);
		break;

	case CS_MRU_DEEPEST:
		ctl_putuint(sys_var[varid].text, mru_peakentries);
		break;

	case CS_MRU_MINDEPTH:
		ctl_putuint(sys_var[varid].text, mru_mindepth);
		break;

	case CS_MRU_MAXAGE:
		ctl_putint(sys_var[varid].text, mru_maxage);
		break;

	case CS_MRU_MAXDEPTH:
		ctl_putuint(sys_var[varid].text, mru_maxdepth);
		break;

	case CS_MRU_MAXMEM:
		kb = mru_maxdepth * (sizeof(mon_entry) / 1024.);
		u = (u_int)kb;
		if (kb - u >= 0.5)
			u++;
		ctl_putuint(sys_var[varid].text, u);
		break;

	case CS_SS_UPTIME:
		ctl_putuint(sys_var[varid].text, current_time);
		break;

	case CS_SS_RESET:
		ctl_putuint(sys_var[varid].text,
			    current_time - sys_stattime);
		break;

	case CS_SS_RECEIVED:
		ctl_putuint(sys_var[varid].text, sys_received);
		break;

	case CS_SS_THISVER:
		ctl_putuint(sys_var[varid].text, sys_newversion);
		break;

	case CS_SS_OLDVER:
		ctl_putuint(sys_var[varid].text, sys_oldversion);
		break;

	case CS_SS_BADFORMAT:
		ctl_putuint(sys_var[varid].text, sys_badlength);
		break;

	case CS_SS_BADAUTH:
		ctl_putuint(sys_var[varid].text, sys_badauth);
		break;

	case CS_SS_DECLINED:
		ctl_putuint(sys_var[varid].text, sys_declined);
		break;

	case CS_SS_RESTRICTED:
		ctl_putuint(sys_var[varid].text, sys_restricted);
		break;

	case CS_SS_LIMITED:
		ctl_putuint(sys_var[varid].text, sys_limitrejected);
		break;

	case CS_SS_KODSENT:
		ctl_putuint(sys_var[varid].text, sys_kodsent);
		break;

	case CS_SS_PROCESSED:
		ctl_putuint(sys_var[varid].text, sys_processed);
		break;

	case CS_BCASTDELAY:
		ctl_putdbl(sys_var[varid].text, sys_bdelay * 1e3);
		break;

	case CS_AUTHDELAY:
		LFPTOD(&sys_authdelay, dtemp);
		ctl_putdbl(sys_var[varid].text, dtemp * 1e3);
		break;

	case CS_AUTHKEYS:
		ctl_putuint(sys_var[varid].text, authnumkeys);
		break;

	case CS_AUTHFREEK:
		ctl_putuint(sys_var[varid].text, authnumfreekeys);
		break;

	case CS_AUTHKLOOKUPS:
		ctl_putuint(sys_var[varid].text, authkeylookups);
		break;

	case CS_AUTHKNOTFOUND:
		ctl_putuint(sys_var[varid].text, authkeynotfound);
		break;

	case CS_AUTHKUNCACHED:
		ctl_putuint(sys_var[varid].text, authkeyuncached);
		break;

	case CS_AUTHKEXPIRED:
		ctl_putuint(sys_var[varid].text, authkeyexpired);
		break;

	case CS_AUTHENCRYPTS:
		ctl_putuint(sys_var[varid].text, authencryptions);
		break;

	case CS_AUTHDECRYPTS:
		ctl_putuint(sys_var[varid].text, authdecryptions);
		break;

	case CS_AUTHRESET:
		ctl_putuint(sys_var[varid].text,
			    current_time - auth_timereset);
		break;

		/*
		 * CTL_IF_KERNLOOP() puts a zero if the kernel loop is
		 * unavailable, otherwise calls putfunc with args.
		 */
#ifndef KERNEL_PLL
# define	CTL_IF_KERNLOOP(putfunc, args)	\
		ctl_putint(sys_var[varid].text, 0)
#else
# define	CTL_IF_KERNLOOP(putfunc, args)	\
		putfunc args
#endif

		/*
		 * CTL_IF_KERNPPS() puts a zero if either the kernel
		 * loop is unavailable, or kernel hard PPS is not
		 * active, otherwise calls putfunc with args.
		 */
#ifndef KERNEL_PLL
# define	CTL_IF_KERNPPS(putfunc, args)	\
		ctl_putint(sys_var[varid].text, 0)
#else
# define	CTL_IF_KERNPPS(putfunc, args)			\
		if (0 == ntx.shift)				\
			ctl_putint(sys_var[varid].text, 0);	\
		else						\
			putfunc args	/* no trailing ; */
#endif

	case CS_K_OFFSET:
		CTL_IF_KERNLOOP(
			ctl_putdblf,
			(sys_var[varid].text, 0, -1, to_ms * ntx.offset)
		);
		break;

	case CS_K_FREQ:
		CTL_IF_KERNLOOP(
			ctl_putsfp,
			(sys_var[varid].text, ntx.freq)
		);
		break;

	case CS_K_MAXERR:
		CTL_IF_KERNLOOP(
			ctl_putdblf,
			(sys_var[varid].text, 0, 6,
			 to_ms * ntx.maxerror)
		);
		break;

	case CS_K_ESTERR:
		CTL_IF_KERNLOOP(
			ctl_putdblf,
			(sys_var[varid].text, 0, 6,
			 to_ms * ntx.esterror)
		);
		break;

	case CS_K_STFLAGS:
#ifndef KERNEL_PLL
		ss = "";
#else
		ss = k_st_flags(ntx.status);
#endif
		ctl_putstr(sys_var[varid].text, ss, strlen(ss));
		break;

	case CS_K_TIMECONST:
		CTL_IF_KERNLOOP(
			ctl_putint,
			(sys_var[varid].text, ntx.constant)
		);
		break;

	case CS_K_PRECISION:
		CTL_IF_KERNLOOP(
			ctl_putdblf,
			(sys_var[varid].text, 0, 6,
			    to_ms * ntx.precision)
		);
		break;

	case CS_K_FREQTOL:
		CTL_IF_KERNLOOP(
			ctl_putsfp,
			(sys_var[varid].text, ntx.tolerance)
		);
		break;

	case CS_K_PPS_FREQ:
		CTL_IF_KERNPPS(
			ctl_putsfp,
			(sys_var[varid].text, ntx.ppsfreq)
		);
		break;

	case CS_K_PPS_STABIL:
		CTL_IF_KERNPPS(
			ctl_putsfp,
			(sys_var[varid].text, ntx.stabil)
		);
		break;

	case CS_K_PPS_JITTER:
		CTL_IF_KERNPPS(
			ctl_putdbl,
			(sys_var[varid].text, to_ms * ntx.jitter)
		);
		break;

	case CS_K_PPS_CALIBDUR:
		CTL_IF_KERNPPS(
			ctl_putint,
			(sys_var[varid].text, 1 << ntx.shift)
		);
		break;

	case CS_K_PPS_CALIBS:
		CTL_IF_KERNPPS(
			ctl_putint,
			(sys_var[varid].text, ntx.calcnt)
		);
		break;

	case CS_K_PPS_CALIBERRS:
		CTL_IF_KERNPPS(
			ctl_putint,
			(sys_var[varid].text, ntx.errcnt)
		);
		break;

	case CS_K_PPS_JITEXC:
		CTL_IF_KERNPPS(
			ctl_putint,
			(sys_var[varid].text, ntx.jitcnt)
		);
		break;

	case CS_K_PPS_STBEXC:
		CTL_IF_KERNPPS(
			ctl_putint,
			(sys_var[varid].text, ntx.stbcnt)
		);
		break;

	case CS_IOSTATS_RESET:
		ctl_putuint(sys_var[varid].text,
			    current_time - io_timereset);
		break;

	case CS_TOTAL_RBUF:
		ctl_putuint(sys_var[varid].text, total_recvbuffs());
		break;

	case CS_FREE_RBUF:
		ctl_putuint(sys_var[varid].text, free_recvbuffs());
		break;

	case CS_USED_RBUF:
		ctl_putuint(sys_var[varid].text, full_recvbuffs());
		break;

	case CS_RBUF_LOWATER:
		ctl_putuint(sys_var[varid].text, lowater_additions());
		break;

	case CS_IO_DROPPED:
		ctl_putuint(sys_var[varid].text, packets_dropped);
		break;

	case CS_IO_IGNORED:
		ctl_putuint(sys_var[varid].text, packets_ignored);
		break;

	case CS_IO_RECEIVED:
		ctl_putuint(sys_var[varid].text, packets_received);
		break;

	case CS_IO_SENT:
		ctl_putuint(sys_var[varid].text, packets_sent);
		break;

	case CS_IO_SENDFAILED:
		ctl_putuint(sys_var[varid].text, packets_notsent);
		break;

	case CS_IO_WAKEUPS:
		ctl_putuint(sys_var[varid].text, handler_calls);
		break;

	case CS_IO_GOODWAKEUPS:
		ctl_putuint(sys_var[varid].text, handler_pkts);
		break;

	case CS_TIMERSTATS_RESET:
		ctl_putuint(sys_var[varid].text,
			    current_time - timer_timereset);
		break;

	case CS_TIMER_OVERRUNS:
		ctl_putuint(sys_var[varid].text, alarm_overflow);
		break;

	case CS_TIMER_XMTS:
		ctl_putuint(sys_var[varid].text, timer_xmtcalls);
		break;

	case CS_FUZZ:
		ctl_putdbl(sys_var[varid].text, sys_fuzz * 1e3);
		break;
	case CS_WANDER_THRESH:
		ctl_putdbl(sys_var[varid].text, wander_threshold * 1e6);
		break;
#ifdef AUTOKEY
	case CS_FLAGS:
		if (crypto_flags)
			ctl_puthex(sys_var[CS_FLAGS].text,
			    crypto_flags);
		break;

	case CS_DIGEST:
		if (crypto_flags) {
			strlcpy(str, OBJ_nid2ln(crypto_nid),
			    COUNTOF(str));
			ctl_putstr(sys_var[CS_DIGEST].text, str,
			    strlen(str));
		}
		break;

	case CS_SIGNATURE:
		if (crypto_flags) {
			const EVP_MD *dp;

			dp = EVP_get_digestbynid(crypto_flags >> 16);
			strlcpy(str, OBJ_nid2ln(EVP_MD_pkey_type(dp)),
			    COUNTOF(str));
			ctl_putstr(sys_var[CS_SIGNATURE].text, str,
			    strlen(str));
		}
		break;

	case CS_HOST:
		if (hostval.ptr != NULL)
			ctl_putstr(sys_var[CS_HOST].text, hostval.ptr,
			    strlen(hostval.ptr));
		break;

	case CS_IDENT:
		if (sys_ident != NULL)
			ctl_putstr(sys_var[CS_IDENT].text, sys_ident,
			    strlen(sys_ident));
		break;

	case CS_CERTIF:
		for (cp = cinfo; cp != NULL; cp = cp->link) {
			snprintf(str, sizeof(str), "%s %s 0x%x",
			    cp->subject, cp->issuer, cp->flags);
			ctl_putstr(sys_var[CS_CERTIF].text, str,
			    strlen(str));
			ctl_putcal(sys_var[CS_REVTIME].text, &(cp->last));
		}
		break;

	case CS_PUBLIC:
		if (hostval.tstamp != 0)
			ctl_putfs(sys_var[CS_PUBLIC].text,
			    ntohl(hostval.tstamp));
		break;
#endif	/* AUTOKEY */

	default:
		break;
	}
}