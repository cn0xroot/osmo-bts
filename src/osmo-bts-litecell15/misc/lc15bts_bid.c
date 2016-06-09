/* Copyright (C) 2015 by Yves Godin <support@nuranwireless.com>
 * 
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "lc15bts_bid.h"

#define BOARD_REV_SYSFS		"/var/lc15/platform/revision"
#define BOARD_OPT_SYSFS		"/var/lc15/platform/option"
 
static const int option_type_mask[_NUM_OPTION_TYPES] = {
        [LC15BTS_OPTION_OCXO]		= 0x07,
	[LC15BTS_OPTION_FPGA]		= 0x03,
	[LC15BTS_OPTION_PA]		= 0x01,
	[LC15BTS_OPTION_BAND]		= 0x03,
	[LC15BTS_OPTION_TX_ISO_BYP]	= 0x01,
	[LC15BTS_OPTION_RX_DUP_BYP]	= 0x01,
	[LC15BTS_OPTION_RX_PB_BYP]	= 0x01,
	[LC15BTS_OPTION_RX_DIV]		= 0x01,
	[LC15BTS_OPTION_RX1A]		= 0x01,
	[LC15BTS_OPTION_RX1B]		= 0x01,
	[LC15BTS_OPTION_RX2A]		= 0x01,
	[LC15BTS_OPTION_RX2B]		= 0x01,
	[LC15BTS_OPTION_DDR_32B]	= 0x01,
	[LC15BTS_OPTION_DDR_ECC]	= 0x01,
	[LC15BTS_OPTION_LOG_DET]	= 0x01,
	[LC15BTS_OPTION_DUAL_LOG_DET]	= 0x01,
};

static const int option_type_shift[_NUM_OPTION_TYPES] = {
        [LC15BTS_OPTION_OCXO]		= 0,
	[LC15BTS_OPTION_FPGA]		= 3,
	[LC15BTS_OPTION_PA]		= 5,
	[LC15BTS_OPTION_BAND]		= 6,
	[LC15BTS_OPTION_TX_ISO_BYP]	= 8,
	[LC15BTS_OPTION_RX_DUP_BYP]	= 9,
	[LC15BTS_OPTION_RX_PB_BYP]	= 10,
	[LC15BTS_OPTION_RX_DIV]		= 11,
	[LC15BTS_OPTION_RX1A]		= 12,
	[LC15BTS_OPTION_RX1B]		= 13,
	[LC15BTS_OPTION_RX2A]		= 14,
	[LC15BTS_OPTION_RX2B]		= 15,
	[LC15BTS_OPTION_DDR_32B]	= 16,
	[LC15BTS_OPTION_DDR_ECC]	= 17,
	[LC15BTS_OPTION_LOG_DET]	= 18,
	[LC15BTS_OPTION_DUAL_LOG_DET]	= 19,
};


static int board_rev = -1;
static int board_option = -1;


int lc15bts_rev_get(void) 
{
	FILE *fp;
	char rev;

	if (board_rev != -1) {
		return board_rev;
	}

        fp = fopen(BOARD_REV_SYSFS, "r");
        if (fp == NULL) return -1;

        if (fscanf(fp, "%c", &rev) != 1) {
                fclose( fp );
                return -1;
        }
        fclose(fp);

	board_rev = rev;
	return board_rev;
}

int lc15bts_model_get(void)
{
        FILE *fp;
        int opt;


        if (board_option == -1) {
        	fp = fopen(BOARD_OPT_SYSFS, "r");
	        if (fp == NULL) {
			return -1;
		}

	        if (fscanf(fp, "%X", &opt) != 1) {
	                fclose( fp );
	                return -1;
	        }
	        fclose(fp);
	
	        board_option = opt;
	}
        return board_option;
}

int lc15bts_option_get(enum lc15bts_option_type type)
{
	int rc;
	int option;

	if (type >= _NUM_OPTION_TYPES) {
		return -EINVAL;
	}

	if (board_option == -1) {
		rc = lc15bts_model_get();
		if (rc < 0) return rc;
	}

	option = (board_option >> option_type_shift[type])
		& option_type_mask[type];

	return option;	
}
