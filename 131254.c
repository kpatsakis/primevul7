dtv_property_legacy_params_sync(struct dvb_frontend *fe,
				const struct dtv_frontend_properties *c,
				struct dvb_frontend_parameters *p)
{
	p->frequency = c->frequency;
	p->inversion = c->inversion;

	switch (dvbv3_type(c->delivery_system)) {
	case DVBV3_UNKNOWN:
		dev_err(fe->dvb->device,
				"%s: doesn't know how to handle a DVBv3 call to delivery system %i\n",
				__func__, c->delivery_system);
		return -EINVAL;
	case DVBV3_QPSK:
		dev_dbg(fe->dvb->device, "%s: Preparing QPSK req\n", __func__);
		p->u.qpsk.symbol_rate = c->symbol_rate;
		p->u.qpsk.fec_inner = c->fec_inner;
		break;
	case DVBV3_QAM:
		dev_dbg(fe->dvb->device, "%s: Preparing QAM req\n", __func__);
		p->u.qam.symbol_rate = c->symbol_rate;
		p->u.qam.fec_inner = c->fec_inner;
		p->u.qam.modulation = c->modulation;
		break;
	case DVBV3_OFDM:
		dev_dbg(fe->dvb->device, "%s: Preparing OFDM req\n", __func__);
		switch (c->bandwidth_hz) {
		case 10000000:
			p->u.ofdm.bandwidth = BANDWIDTH_10_MHZ;
			break;
		case 8000000:
			p->u.ofdm.bandwidth = BANDWIDTH_8_MHZ;
			break;
		case 7000000:
			p->u.ofdm.bandwidth = BANDWIDTH_7_MHZ;
			break;
		case 6000000:
			p->u.ofdm.bandwidth = BANDWIDTH_6_MHZ;
			break;
		case 5000000:
			p->u.ofdm.bandwidth = BANDWIDTH_5_MHZ;
			break;
		case 1712000:
			p->u.ofdm.bandwidth = BANDWIDTH_1_712_MHZ;
			break;
		case 0:
		default:
			p->u.ofdm.bandwidth = BANDWIDTH_AUTO;
		}
		p->u.ofdm.code_rate_HP = c->code_rate_HP;
		p->u.ofdm.code_rate_LP = c->code_rate_LP;
		p->u.ofdm.constellation = c->modulation;
		p->u.ofdm.transmission_mode = c->transmission_mode;
		p->u.ofdm.guard_interval = c->guard_interval;
		p->u.ofdm.hierarchy_information = c->hierarchy;
		break;
	case DVBV3_ATSC:
		dev_dbg(fe->dvb->device, "%s: Preparing VSB req\n", __func__);
		p->u.vsb.modulation = c->modulation;
		break;
	}
	return 0;
}