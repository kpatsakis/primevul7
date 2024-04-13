ipmi_spd_print(uint8_t *spd_data, int len)
{
	int k = 0;
	int ii = 0;

	if (len < 92)
		return -1; /* we need first 91 bytes to do our thing */

	printf(" Memory Type           : %s\n",
	       val2str(spd_data[2], spd_memtype_vals));

	if (spd_data[2] == 0x0B)	/* DDR3 SDRAM */
	{
		int iPN;
		int sdram_cap = 0;
		int pri_bus_width = 0;
		int sdram_width = 0;
		int ranks = 0;
		int mem_size = 0;

		if (len < 148)
			return -1; /* we need first 91 bytes to do our thing */


		sdram_cap = ldexp(256,(spd_data[4]&15));
		pri_bus_width = ldexp(8,(spd_data[8]&7));
		sdram_width = ldexp(4,(spd_data[7]&7));
		ranks = ldexp(1,((spd_data[7]&0x3F)>>3));
		mem_size = (sdram_cap/8) * (pri_bus_width/sdram_width) * ranks;
		printf(" SDRAM Capacity        : %d MB\n", sdram_cap );
		printf(" Memory Banks          : %s\n", val2str(spd_data[4]>>4, ddr3_banks_vals));
		printf(" Primary Bus Width     : %d bits\n", pri_bus_width );
		printf(" SDRAM Device Width    : %d bits\n", sdram_width );
		printf(" Number of Ranks       : %d\n", ranks );
		printf(" Memory size           : %d MB\n", mem_size );

		/* printf(" Memory Density        : %s\n", val2str(spd_data[4]&15, ddr3_density_vals)); */
		printf(" 1.5 V Nominal Op      : %s\n", (((spd_data[6]&1) != 0) ? "No":"Yes" ) );
		printf(" 1.35 V Nominal Op     : %s\n", (((spd_data[6]&2) != 0) ? "No":"Yes" ) );
		printf(" 1.2X V Nominal Op     : %s\n", (((spd_data[6]&4) != 0) ? "No":"Yes" ) );
		printf(" Error Detect/Cor      : %s\n", val2str(spd_data[8]>>3, ddr3_ecc_vals));

		printf(" Manufacturer          : ");
		switch (spd_data[117]&127)
		{
		case	0:
			printf("%s\n", val2str(spd_data[118], jedec_id1_vals));
			break;

		case	1:
			printf("%s\n", val2str(spd_data[118], jedec_id2_vals));
			break;

		case	2:
			printf("%s\n", val2str(spd_data[118], jedec_id3_vals));
			break;

		case	3:
			printf("%s\n", val2str(spd_data[118], jedec_id4_vals));
			break;

		case	4:
			printf("%s\n", val2str(spd_data[118], jedec_id5_vals));
			break;

		case	5:
			printf("%s\n", val2str(spd_data[118], jedec_id6_vals));
			break;

		case	6:
			printf("%s\n", val2str(spd_data[118], jedec_id7_vals));
			break;

		case	7:
			printf("%s\n", val2str(spd_data[118], jedec_id8_vals));
			break;

		case	8:
			printf("%s\n", val2str(spd_data[118], jedec_id9_vals));
			break;

		default:
			printf("%s\n", "JEDEC JEP106 update required" );

		}

		printf(" Manufacture Date      : year %c%c week %c%c\n",
		'0'+(spd_data[120]>>4), '0'+(spd_data[120]&15), '0'+(spd_data[121]>>4), '0'+(spd_data[121]&15) );

		printf(" Serial Number         : %02x%02x%02x%02x\n",
		spd_data[122], spd_data[123], spd_data[124], spd_data[125]);

		printf(" Part Number           : ");
		for (iPN = 128; iPN < 146; iPN++) {
			printf("%c", spd_data[iPN]);
		}
		printf("\n");
	} else if (spd_data[2] == 0x0C)	/* DDR4 SDRAM */
	{
		int i;
		int sdram_cap = 0;
		int pri_bus_width = 0;
		int sdram_width = 0;
		int mem_size = 0;
		int lrank_dimm;
		uint32_t year;
		uint32_t week;

		if (len < 348)
			return -1;

		/* "Logical rank" referes to the individually addressable die
		 * in a 3DS stack and has no meaning for monolithic or
		 * multi-load stacked SDRAMs; however, for the purposes of
		 * calculating the capacity of the module, one should treat
		 * monolithic and multi-load stack SDRAMs as having one logical
		 * rank per package rank.
		 */
		lrank_dimm = (spd_data[12]>>3&0x3) + 1; /* Number of Package Ranks per DIMM */
		if ((spd_data[6] & 0x3) == 0x2) { /* 3DS package Type */
			lrank_dimm *= ((spd_data[6]>>4)&0x3) + 1; /* Die Count */
		}
		sdram_cap = ldexp(256,(spd_data[4]&15));
		pri_bus_width = ldexp(8,(spd_data[13]&7));
		sdram_width = ldexp(4,(spd_data[12]&7));
		mem_size = (sdram_cap/8) * (pri_bus_width/sdram_width) * lrank_dimm;
		printf(" SDRAM Package Type    : %s\n", val2str((spd_data[6]>>7), ddr4_package_type));
		printf(" Technology            : %s\n", val2str((spd_data[3]&15), ddr4_technology_type));
		printf(" SDRAM Die Count       : %d\n", ((spd_data[6]>>4) & 3)+1);
		printf(" SDRAM Capacity        : %d Mb\n", sdram_cap );
		printf(" Memory Bank Group     : %s\n", val2str((spd_data[4]>>6 & 0x3), ddr4_bank_groups));
		printf(" Memory Banks          : %s\n", val2str((spd_data[4]>>4 & 0x3), ddr4_banks_vals));
		printf(" Primary Bus Width     : %d bits\n", pri_bus_width );
		printf(" SDRAM Device Width    : %d bits\n", sdram_width );
		printf(" Logical Rank per DIMM : %d\n", lrank_dimm );
		printf(" Memory size           : %d MB\n", mem_size );

		printf(" Memory Density        : %s\n", val2str(spd_data[4]&15, ddr4_density_vals));
		printf(" 1.2 V Nominal Op      : %s\n", (((spd_data[11]&3) != 3) ? "No":"Yes" ) );
		printf(" TBD1 V Nominal Op     : %s\n", (((spd_data[11]>>2&3) != 3) ? "No":"Yes" ) );
		printf(" TBD2 V Nominal Op     : %s\n", (((spd_data[11]>>4&3) != 3) ? "No":"Yes" ) );
		printf(" Error Detect/Cor      : %s\n", val2str(spd_data[13]>>3, ddr4_ecc_vals));

		printf(" Manufacturer          : ");
		switch (spd_data[320]&127)
		{
		case	0:
			printf("%s\n", val2str(spd_data[321], jedec_id1_vals));
			break;

		case	1:
			printf("%s\n", val2str(spd_data[321], jedec_id2_vals));
			break;

		case	2:
			printf("%s\n", val2str(spd_data[321], jedec_id3_vals));
			break;

		case	3:
			printf("%s\n", val2str(spd_data[321], jedec_id4_vals));
			break;

		case	4:
			printf("%s\n", val2str(spd_data[321], jedec_id5_vals));
			break;

		case	5:
			printf("%s\n", val2str(spd_data[321], jedec_id6_vals));
			break;

		case	6:
			printf("%s\n", val2str(spd_data[321], jedec_id7_vals));
			break;

		case	7:
			printf("%s\n", val2str(spd_data[321], jedec_id8_vals));
			break;

		case	8:
			printf("%s\n", val2str(spd_data[321], jedec_id9_vals));
			break;

		default:
			printf("%s\n", "JEDEC JEP106 update required");

		}

		year = ((spd_data[323] >> 4) * 10) + (spd_data[323] & 15);
		week = ((spd_data[324]>>4) * 10) + (spd_data[324] & 15);
		printf(" Manufacture Date      : year %4d week %2d\n",
		       2000 + year, week);

		printf(" Serial Number         : %02x%02x%02x%02x\n",
		spd_data[325], spd_data[326], spd_data[327], spd_data[328]);

		printf(" Part Number           : ");
		for (i=329; i <= 348; i++)
		{
			printf( "%c", spd_data[i]);
		}
		printf("\n");
	}
	else
	{
		if (len < 100) {
			return (-1);
		}
		ii = (spd_data[3] & 0x0f) + (spd_data[4] & 0x0f) - 17;
		k = ((spd_data[5] & 0x7) + 1) * spd_data[17];

		if(ii > 0 && ii <= 12 && k > 0) {
			printf(" Memory Size           : %d MB\n", ((1 << ii) * k));
		} else {
			printf(" Memory Size    INVALID: %d, %d, %d, %d\n", spd_data[3],
					spd_data[4], spd_data[5], spd_data[17]);
		}
		printf(" Voltage Intf          : %s\n",
		val2str(spd_data[8], spd_voltage_vals));
		printf(" Error Detect/Cor      : %s\n",
		val2str(spd_data[11], spd_config_vals));

		/* handle jedec table bank continuation values */
		printf(" Manufacturer          : ");
		if (spd_data[64] != 0x7f)
			printf("%s\n",
			val2str(spd_data[64], jedec_id1_vals));
		else {
			if (spd_data[65] != 0x7f)
				printf("%s\n",
				val2str(spd_data[65], jedec_id2_vals));
			else {
				if (spd_data[66] != 0x7f)
					printf("%s\n",
					val2str(spd_data[66], jedec_id3_vals));
				else {
					if (spd_data[67] != 0x7f)
						printf("%s\n",
						val2str(spd_data[67], jedec_id4_vals));
					else {
						if (spd_data[68] != 0x7f)
							printf("%s\n",
							val2str(spd_data[68], jedec_id5_vals));
						else {
							if (spd_data[69] != 0x7f)
								printf("%s\n",
								val2str(spd_data[69], jedec_id6_vals));
							else {
								if (spd_data[70] != 0x7f)
									printf("%s\n",
									val2str(spd_data[70], jedec_id7_vals));
								else {
									if (spd_data[71] != 0x7f)
										printf("%s\n",
										val2str(spd_data[71], jedec_id8_vals));
									else
										printf("%s\n",
										val2str(spd_data[72], jedec_id9_vals));
								}
							}
						}
					}
				}
			}
		}

		if (spd_data[73]) {
			char part[19];
			memcpy(part, spd_data+73, 18);
			part[18] = 0;
			printf(" Part Number           : %s\n", part);
		}

		printf(" Serial Number         : %02x%02x%02x%02x\n",
		spd_data[95], spd_data[96], spd_data[97], spd_data[98]);
	}

	if (verbose) {
		printf("\n");
		printbuf(spd_data, len, "SPD DATA");
	}

	return 0;
}