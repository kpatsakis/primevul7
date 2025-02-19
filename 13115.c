reg_equi_class(int c)
{
    if (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	switch (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    case 'A': case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4:
	    case 0xc5: case 0x100: case 0x102: case 0x104: case 0x1cd:
	    case 0x1de: case 0x1e0: case 0x1fa: case 0x202: case 0x226:
	    case 0x23a: case 0x1e00: case 0x1ea0: case 0x1ea2: case 0x1ea4:
	    case 0x1ea6: case 0x1ea8: case 0x1eaa: case 0x1eac: case 0x1eae:
	    case 0x1eb0: case 0x1eb2: case 0x1eb4: case 0x1eb6:
		      regmbc('A'); regmbc(0xc0); regmbc(0xc1); regmbc(0xc2);
		      regmbc(0xc3); regmbc(0xc4); regmbc(0xc5);
		      regmbc(0x100); regmbc(0x102); regmbc(0x104);
		      regmbc(0x1cd); regmbc(0x1de); regmbc(0x1e0);
		      regmbc(0x1fa); regmbc(0x202); regmbc(0x226);
		      regmbc(0x23a); regmbc(0x1e00); regmbc(0x1ea0);
		      regmbc(0x1ea2); regmbc(0x1ea4); regmbc(0x1ea6);
		      regmbc(0x1ea8); regmbc(0x1eaa); regmbc(0x1eac);
		      regmbc(0x1eae); regmbc(0x1eb0); regmbc(0x1eb2);
		      regmbc(0x1eb4); regmbc(0x1eb6);
		      return;
	    case 'B': case 0x181: case 0x243: case 0x1e02:
	    case 0x1e04: case 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      return;
	    case 'C': case 0xc7:
	    case 0x106: case 0x108: case 0x10a: case 0x10c: case 0x187:
	    case 0x23b: case 0x1e08: case 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      return;
	    case 'D': case 0x10e: case 0x110: case 0x18a:
	    case 0x1e0a: case 0x1e0c: case 0x1e0e: case 0x1e10:
	    case 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      return;
	    case 'E': case 0xc8: case 0xc9: case 0xca: case 0xcb:
	    case 0x112: case 0x114: case 0x116: case 0x118: case 0x11a:
	    case 0x204: case 0x206: case 0x228: case 0x246: case 0x1e14:
	    case 0x1e16: case 0x1e18: case 0x1e1a: case 0x1e1c:
	    case 0x1eb8: case 0x1eba: case 0x1ebc: case 0x1ebe:
	    case 0x1ec0: case 0x1ec2: case 0x1ec4: case 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      return;
	    case 'F': case 0x191: case 0x1e1e: case 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      return;
	    case 'G': case 0x11c: case 0x11e: case 0x120:
	    case 0x122: case 0x193: case 0x1e4: case 0x1e6:
	    case 0x1f4: case 0x1e20: case 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      return;
	    case 'H': case 0x124: case 0x126: case 0x21e:
	    case 0x1e22: case 0x1e24: case 0x1e26:
	    case 0x1e28: case 0x1e2a: case 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      return;
	    case 'I': case 0xcc: case 0xcd: case 0xce: case 0xcf:
	    case 0x128: case 0x12a: case 0x12c: case 0x12e:
	    case 0x130: case 0x197: case 0x1cf: case 0x208:
	    case 0x20a: case 0x1e2c: case 0x1e2e: case 0x1ec8:
	    case 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      return;
	    case 'J': case 0x134: case 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      return;
	    case 'K': case 0x136: case 0x198: case 0x1e8: case 0x1e30:
	    case 0x1e32: case 0x1e34: case 0x2c69: case 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      return;
	    case 'L': case 0x139: case 0x13b: case 0x13d: case 0x13f:
	    case 0x141: case 0x23d: case 0x1e36: case 0x1e38:
	    case 0x1e3a: case 0x1e3c: case 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      return;
	    case 'M': case 0x1e3e: case 0x1e40: case 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      return;
	    case 'N': case 0xd1:
	    case 0x143: case 0x145: case 0x147: case 0x1f8:
	    case 0x1e44: case 0x1e46: case 0x1e48: case 0x1e4a:
	    case 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      return;
	    case 'O': case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6:
	    case 0xd8: case 0x14c: case 0x14e: case 0x150: case 0x19f:
	    case 0x1a0: case 0x1d1: case 0x1ea: case 0x1ec: case 0x1fe:
	    case 0x20c: case 0x20e: case 0x22a: case 0x22c: case 0x22e:
	    case 0x230: case 0x1e4c: case 0x1e4e: case 0x1e50: case 0x1e52:
	    case 0x1ecc: case 0x1ece: case 0x1ed0: case 0x1ed2: case 0x1ed4:
	    case 0x1ed6: case 0x1ed8: case 0x1eda: case 0x1edc: case 0x1ede:
	    case 0x1ee0: case 0x1ee2:
		      regmbc('O'); regmbc(0xd2); regmbc(0xd3); regmbc(0xd4);
		      regmbc(0xd5); regmbc(0xd6); regmbc(0xd8);
		      regmbc(0x14c); regmbc(0x14e); regmbc(0x150);
		      regmbc(0x19f); regmbc(0x1a0); regmbc(0x1d1);
		      regmbc(0x1ea); regmbc(0x1ec); regmbc(0x1fe);
		      regmbc(0x20c); regmbc(0x20e); regmbc(0x22a);
		      regmbc(0x22c); regmbc(0x22e); regmbc(0x230);
		      regmbc(0x1e4c); regmbc(0x1e4e); regmbc(0x1e50);
		      regmbc(0x1e52); regmbc(0x1ecc); regmbc(0x1ece);
		      regmbc(0x1ed0); regmbc(0x1ed2); regmbc(0x1ed4);
		      regmbc(0x1ed6); regmbc(0x1ed8); regmbc(0x1eda);
		      regmbc(0x1edc); regmbc(0x1ede); regmbc(0x1ee0);
		      regmbc(0x1ee2);
		      return;
	    case 'P': case 0x1a4: case 0x1e54: case 0x1e56: case 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      return;
	    case 'Q': case 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      return;
	    case 'R': case 0x154: case 0x156: case 0x158: case 0x210:
	    case 0x212: case 0x24c: case 0x1e58: case 0x1e5a:
	    case 0x1e5c: case 0x1e5e: case 0x2c64: case 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      return;
	    case 'S': case 0x15a: case 0x15c: case 0x15e: case 0x160:
	    case 0x218: case 0x1e60: case 0x1e62: case 0x1e64:
	    case 0x1e66: case 0x1e68: case 0x2c7e: case 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      return;
	    case 'T': case 0x162: case 0x164: case 0x166: case 0x1ac:
	    case 0x1ae: case 0x21a: case 0x23e: case 0x1e6a: case 0x1e6c:
	    case 0x1e6e: case 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      return;
	    case 'U': case 0xd9: case 0xda: case 0xdb: case 0xdc:
	    case 0x168: case 0x16a: case 0x16c: case 0x16e:
	    case 0x170: case 0x172: case 0x1af: case 0x1d3:
	    case 0x1d5: case 0x1d7: case 0x1d9: case 0x1db:
	    case 0x214: case 0x216: case 0x244: case 0x1e72:
	    case 0x1e74: case 0x1e76: case 0x1e78: case 0x1e7a:
	    case 0x1ee4: case 0x1ee6: case 0x1ee8: case 0x1eea:
	    case 0x1eec: case 0x1eee: case 0x1ef0:
		      regmbc('U'); regmbc(0xd9); regmbc(0xda);
		      regmbc(0xdb); regmbc(0xdc); regmbc(0x168);
		      regmbc(0x16a); regmbc(0x16c); regmbc(0x16e);
		      regmbc(0x170); regmbc(0x172); regmbc(0x1af);
		      regmbc(0x1d3); regmbc(0x1d5); regmbc(0x1d7);
		      regmbc(0x1d9); regmbc(0x1db); regmbc(0x214);
		      regmbc(0x216); regmbc(0x244); regmbc(0x1e72);
		      regmbc(0x1e74); regmbc(0x1e76); regmbc(0x1e78);
		      regmbc(0x1e7a); regmbc(0x1ee4); regmbc(0x1ee6);
		      regmbc(0x1ee8); regmbc(0x1eea); regmbc(0x1eec);
		      regmbc(0x1eee); regmbc(0x1ef0);
		      return;
	    case 'V': case 0x1b2: case 0x1e7c: case 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      return;
	    case 'W': case 0x174: case 0x1e80: case 0x1e82:
	    case 0x1e84: case 0x1e86: case 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      return;
	    case 'X': case 0x1e8a: case 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      return;
	    case 'Y': case 0xdd:
	    case 0x176: case 0x178: case 0x1b3: case 0x232: case 0x24e:
	    case 0x1e8e: case 0x1ef2: case 0x1ef6: case 0x1ef4: case 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      return;
	    case 'Z': case 0x179: case 0x17b: case 0x17d: case 0x1b5:
	    case 0x1e90: case 0x1e92: case 0x1e94: case 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      return;
	    case 'a': case 0xe0: case 0xe1: case 0xe2:
	    case 0xe3: case 0xe4: case 0xe5: case 0x101: case 0x103:
	    case 0x105: case 0x1ce: case 0x1df: case 0x1e1: case 0x1fb:
	    case 0x201: case 0x203: case 0x227: case 0x1d8f: case 0x1e01:
	    case 0x1e9a: case 0x1ea1: case 0x1ea3: case 0x1ea5:
	    case 0x1ea7: case 0x1ea9: case 0x1eab: case 0x1ead:
	    case 0x1eaf: case 0x1eb1: case 0x1eb3: case 0x1eb5:
	    case 0x1eb7: case 0x2c65:
		      regmbc('a'); regmbc(0xe0); regmbc(0xe1);
		      regmbc(0xe2); regmbc(0xe3); regmbc(0xe4);
		      regmbc(0xe5); regmbc(0x101); regmbc(0x103);
		      regmbc(0x105); regmbc(0x1ce); regmbc(0x1df);
		      regmbc(0x1e1); regmbc(0x1fb); regmbc(0x201);
		      regmbc(0x203); regmbc(0x227); regmbc(0x1d8f);
		      regmbc(0x1e01); regmbc(0x1e9a); regmbc(0x1ea1);
		      regmbc(0x1ea3); regmbc(0x1ea5); regmbc(0x1ea7);
		      regmbc(0x1ea9); regmbc(0x1eab); regmbc(0x1ead);
		      regmbc(0x1eaf); regmbc(0x1eb1); regmbc(0x1eb3);
		      regmbc(0x1eb5); regmbc(0x1eb7); regmbc(0x2c65);
		      return;
	    case 'b': case 0x180: case 0x253: case 0x1d6c: case 0x1d80:
	    case 0x1e03: case 0x1e05: case 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      return;
	    case 'c': case 0xe7:
	    case 0x107: case 0x109: case 0x10b: case 0x10d: case 0x188:
	    case 0x23c: case 0x1e09: case 0xa793: case 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      return;
	    case 'd': case 0x10f: case 0x111: case 0x257: case 0x1d6d:
	    case 0x1d81: case 0x1d91: case 0x1e0b: case 0x1e0d:
	    case 0x1e0f: case 0x1e11: case 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      return;
	    case 'e': case 0xe8: case 0xe9: case 0xea: case 0xeb:
	    case 0x113: case 0x115: case 0x117: case 0x119:
	    case 0x11b: case 0x205: case 0x207: case 0x229:
	    case 0x247: case 0x1d92: case 0x1e15: case 0x1e17:
	    case 0x1e19: case 0x1e1b: case 0x1eb9: case 0x1ebb:
	    case 0x1e1d: case 0x1ebd: case 0x1ebf: case 0x1ec1:
	    case 0x1ec3: case 0x1ec5: case 0x1ec7:
		      regmbc('e'); regmbc(0xe8); regmbc(0xe9);
		      regmbc(0xea); regmbc(0xeb); regmbc(0x113);
		      regmbc(0x115); regmbc(0x117); regmbc(0x119);
		      regmbc(0x11b); regmbc(0x205); regmbc(0x207);
		      regmbc(0x229); regmbc(0x247); regmbc(0x1d92);
		      regmbc(0x1e15); regmbc(0x1e17); regmbc(0x1e19);
		      regmbc(0x1e1b); regmbc(0x1e1d); regmbc(0x1eb9);
		      regmbc(0x1ebb); regmbc(0x1ebd); regmbc(0x1ebf);
		      regmbc(0x1ec1); regmbc(0x1ec3); regmbc(0x1ec5);
		      regmbc(0x1ec7);
		      return;
	    case 'f': case 0x192: case 0x1d6e: case 0x1d82:
	    case 0x1e1f: case 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     return;
	    case 'g': case 0x11d: case 0x11f: case 0x121: case 0x123:
	    case 0x1e5: case 0x1e7: case 0x260: case 0x1f5: case 0x1d83:
	    case 0x1e21: case 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      return;
	    case 'h': case 0x125: case 0x127: case 0x21f: case 0x1e23:
	    case 0x1e25: case 0x1e27: case 0x1e29: case 0x1e2b:
	    case 0x1e96: case 0x2c68: case 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      return;
	    case 'i': case 0xec: case 0xed: case 0xee: case 0xef:
	    case 0x129: case 0x12b: case 0x12d: case 0x12f:
	    case 0x1d0: case 0x209: case 0x20b: case 0x268:
	    case 0x1d96: case 0x1e2d: case 0x1e2f: case 0x1ec9:
	    case 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      return;
	    case 'j': case 0x135: case 0x1f0: case 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      return;
	    case 'k': case 0x137: case 0x199: case 0x1e9:
	    case 0x1d84: case 0x1e31: case 0x1e33: case 0x1e35:
	    case 0x2c6a: case 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      return;
	    case 'l': case 0x13a: case 0x13c: case 0x13e:
	    case 0x140: case 0x142: case 0x19a: case 0x1e37:
	    case 0x1e39: case 0x1e3b: case 0x1e3d: case 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      return;
	    case 'm': case 0x1d6f: case 0x1e3f: case 0x1e41: case 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      return;
	    case 'n': case 0xf1: case 0x144: case 0x146: case 0x148:
	    case 0x149: case 0x1f9: case 0x1d70: case 0x1d87:
	    case 0x1e45: case 0x1e47: case 0x1e49: case 0x1e4b:
	    case 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      return;
	    case 'o': case 0xf2: case 0xf3: case 0xf4: case 0xf5:
	    case 0xf6: case 0xf8: case 0x14d: case 0x14f: case 0x151:
	    case 0x1a1: case 0x1d2: case 0x1eb: case 0x1ed: case 0x1ff:
	    case 0x20d: case 0x20f: case 0x22b: case 0x22d: case 0x22f:
	    case 0x231: case 0x275: case 0x1e4d: case 0x1e4f:
	    case 0x1e51: case 0x1e53: case 0x1ecd: case 0x1ecf:
	    case 0x1ed1: case 0x1ed3: case 0x1ed5: case 0x1ed7:
	    case 0x1ed9: case 0x1edb: case 0x1edd: case 0x1edf:
	    case 0x1ee1: case 0x1ee3:
		      regmbc('o'); regmbc(0xf2); regmbc(0xf3);
		      regmbc(0xf4); regmbc(0xf5); regmbc(0xf6);
		      regmbc(0xf8); regmbc(0x14d); regmbc(0x14f);
		      regmbc(0x151); regmbc(0x1a1); regmbc(0x1d2);
		      regmbc(0x1eb); regmbc(0x1ed); regmbc(0x1ff);
		      regmbc(0x20d); regmbc(0x20f); regmbc(0x22b);
		      regmbc(0x22d); regmbc(0x22f); regmbc(0x231);
		      regmbc(0x275); regmbc(0x1e4d); regmbc(0x1e4f);
		      regmbc(0x1e51); regmbc(0x1e53); regmbc(0x1ecd);
		      regmbc(0x1ecf); regmbc(0x1ed1); regmbc(0x1ed3);
		      regmbc(0x1ed5); regmbc(0x1ed7); regmbc(0x1ed9);
		      regmbc(0x1edb); regmbc(0x1edd); regmbc(0x1edf);
		      regmbc(0x1ee1); regmbc(0x1ee3);
		      return;
	    case 'p': case 0x1a5: case 0x1d71: case 0x1d88: case 0x1d7d:
	    case 0x1e55: case 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      return;
	    case 'q': case 0x24b: case 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      return;
	    case 'r': case 0x155: case 0x157: case 0x159: case 0x211:
	    case 0x213: case 0x24d: case 0x27d: case 0x1d72: case 0x1d73:
	    case 0x1d89: case 0x1e59: case 0x1e5b: case 0x1e5d: case 0x1e5f:
	    case 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      return;
	    case 's': case 0x15b: case 0x15d: case 0x15f: case 0x161:
	    case 0x1e61: case 0x219: case 0x23f: case 0x1d74: case 0x1d8a:
	    case 0x1e63: case 0x1e65: case 0x1e67: case 0x1e69: case 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      return;
	    case 't': case 0x163: case 0x165: case 0x167: case 0x1ab:
	    case 0x1ad: case 0x21b: case 0x288: case 0x1d75: case 0x1e6b:
	    case 0x1e6d: case 0x1e6f: case 0x1e71: case 0x1e97: case 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      return;
	    case 'u': case 0xf9: case 0xfa: case 0xfb: case 0xfc:
	    case 0x169: case 0x16b: case 0x16d: case 0x16f:
	    case 0x171: case 0x173: case 0x1b0: case 0x1d4:
	    case 0x1d6: case 0x1d8: case 0x1da: case 0x1dc:
	    case 0x215: case 0x217: case 0x289: case 0x1e73:
	    case 0x1d7e: case 0x1d99: case 0x1e75: case 0x1e77:
	    case 0x1e79: case 0x1e7b: case 0x1ee5: case 0x1ee7:
	    case 0x1ee9: case 0x1eeb: case 0x1eed: case 0x1eef:
	    case 0x1ef1:
		      regmbc('u'); regmbc(0xf9); regmbc(0xfa);
		      regmbc(0xfb); regmbc(0xfc); regmbc(0x169);
		      regmbc(0x16b); regmbc(0x16d); regmbc(0x16f);
		      regmbc(0x171); regmbc(0x173); regmbc(0x1d6);
		      regmbc(0x1d8); regmbc(0x1da); regmbc(0x1dc);
		      regmbc(0x215); regmbc(0x217); regmbc(0x1b0);
		      regmbc(0x1d4); regmbc(0x289); regmbc(0x1d7e);
		      regmbc(0x1d99); regmbc(0x1e73); regmbc(0x1e75);
		      regmbc(0x1e77); regmbc(0x1e79); regmbc(0x1e7b);
		      regmbc(0x1ee5); regmbc(0x1ee7); regmbc(0x1ee9);
		      regmbc(0x1eeb); regmbc(0x1eed); regmbc(0x1eef);
		      regmbc(0x1ef1);
		      return;
	    case 'v': case 0x28b: case 0x1d8c: case 0x1e7d: case 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      return;
	    case 'w': case 0x175: case 0x1e81: case 0x1e83:
	    case 0x1e85: case 0x1e87: case 0x1e89: case 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      return;
	    case 'x': case 0x1e8b: case 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      return;
	    case 'y': case 0xfd: case 0xff: case 0x177: case 0x1b4:
	    case 0x233: case 0x24f: case 0x1e8f: case 0x1e99: case 0x1ef3:
	    case 0x1ef5: case 0x1ef7: case 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      return;
	    case 'z': case 0x17a: case 0x17c: case 0x17e: case 0x1b6:
	    case 0x1d76: case 0x1d8e: case 0x1e91: case 0x1e93:
	    case 0x1e95: case 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      return;
	}
    }
    regmbc(c);
}