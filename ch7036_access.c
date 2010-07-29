// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

#include "ch7036.h"

/* Fast load of firmware does not work yet */
/* #define FASTLOAD */
int ch7036_debugi2c = 0;
int ch7036_cur_page = -1;
static int ch_page_map[5] = {-1, 0, 1, 3, 4};
static int ch_page_revmap[8] = {1, 2, -1, 3, 4, -1, -1, -1};

static unsigned char es_map[16] = {
  0x08,0x09,0x0F,0x10,0x11,0x12,0x13,0x14,
  0x15,0x16,0x17,0x28,0x29,0x47,0x48,0x49
};

static unsigned char changed[5][256];

typedef struct {
  unsigned char size; // total size should be less than 16 bytes
  unsigned char ver_major;
  unsigned char ver_minor;
  unsigned char did;
  // CH7036 device id
  unsigned char rid;
  // CH7036 revision id
  unsigned char capbility; // FW capability;
  // 0x01: EDID_Reading
  unsigned char reserved;
} FW7036_CFG;


static int smbus_write_byte_data(int dev, int reg, int data)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;
  int res;

  if (dev < 0) {
    printf("WR_%d {0x%02x, 0x%02x} -> %d\n", ch7036_cur_page, reg, data, 0);
    return 0;
  }

  value.byte = data;
  iodata.read_write = I2C_SMBUS_WRITE;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_BYTE_DATA;
  iodata.data = &value;
  res = ioctl(dev, I2C_SMBUS, &iodata);
  if (res < 0)
    fprintf(stderr, "Write byte fail (%d, %02x, %02x): %s",
	    dev, reg, data, strerror(errno));
  if (ch7036_debugi2c)
    printf("WR_%d {0x%02x, 0x%02x} -> %d\n", ch7036_cur_page, reg, data, res);
  return res;
}

static int smbus_write_word_data(int dev, int reg, int data)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;
  int res;

  if (dev < 0) {
    printf("WR_%d {0x%02x, 0x%04x} -> %d\n", ch7036_cur_page, reg, data, 0);
    return 0;
  }

  value.word = data;
  iodata.read_write = I2C_SMBUS_WRITE;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_WORD_DATA;
  iodata.data = &value;
  res = ioctl(dev, I2C_SMBUS, &iodata);
  if (res < 0)
    fprintf(stderr, "Write word fail (%d, %04x, %02x): %s",
	    dev, reg, data, strerror(errno));
  if (ch7036_debugi2c)
    printf("WR_%d {0x%02x, 0x%04x} -> %d\n", ch7036_cur_page, reg, data, res);
  return res;
}

#ifdef FASTLOAD
static int smbus_write_reg_bytes(int dev, int reg, int size, 
				 unsigned char *data)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;
  int res;

  if (size > I2C_SMBUS_BLOCK_MAX) {
    fprintf(stderr, "Bad call to smbus_write_reg_bytes with size %d\n", size);
    return -1;
  }

  if (dev < 0) {
    printf("WR_%d {0x%02x, 0x%02x,...[%d]} -> %d\n", 
	   ch7036_cur_page, reg, data[0], size, 0);
    return 0;
  }

  memcpy(&value.block[1], data, size);
  value.block[0] = size;
  iodata.read_write = I2C_SMBUS_WRITE;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_I2C_BLOCK_DATA;
  iodata.data = &value;
  res = ioctl(dev, I2C_SMBUS, &iodata);
  if (res < 0)
    fprintf(stderr, "Write bytes fail (%d, %02x, %02x,...[%d]): %s\n",
	    dev, reg, data[0], size, strerror(errno));
  if (ch7036_debugi2c)
    printf("WR_%d {0x%02x, 0x%02x,...[%d]} -> %d\n", 
	   ch7036_cur_page, reg, data[0], size, res);
  return res;
}
#endif

static int smbus_read_byte_data(int dev, int reg)
{
  struct i2c_smbus_ioctl_data iodata;
  union i2c_smbus_data value;

  if (dev < 0) {
    printf("RD_%d {0x%02x} -> 0x%02x\n", ch7036_cur_page, reg, 0);
    return 0;
  }

  iodata.read_write = I2C_SMBUS_READ;
  iodata.command = reg;
  iodata.size = I2C_SMBUS_BYTE_DATA;
  iodata.data = &value;
  if (ioctl(dev, I2C_SMBUS, &iodata) < 0) {
    fprintf(stderr, "Read byte fail (%d, %02x): %s",
	    dev, reg, strerror(errno));
    return -1;
  }
  if (ch7036_debugi2c)
    printf("RD_%d {0x%02x} -> 0x%02x\n", ch7036_cur_page, reg, value.byte);
  return value.byte;
}


int ch_write_reg(int dev, int page, int reg, int data)
{
  int res;

  if (((page < 1) || (page > 4)) && (reg != CH7036_PAGE))
    return -1;

  if ((page != ch7036_cur_page) && (reg != CH7036_PAGE)) {
    if (ch_write_reg(dev, ch7036_cur_page, CH7036_PAGE, ch_page_map[page]) < 0)
      return -1;
  }

  res = smbus_write_byte_data(dev, reg, data);

  if (res < 0)
    return -1;

  if (reg == CH7036_PAGE) {
    ch7036_cur_page = ch_page_revmap[data & 7];
  }
  return 0;
}

int ch_read_reg(int dev, int page, int reg)
{
  int res;

  if ((page < 1) || (page > 4))
    return -1;

  if (page != ch7036_cur_page) {
    if (ch_write_reg(dev, ch7036_cur_page, CH7036_PAGE, ch_page_map[page]) < 0)
      return -1;
  }
  res =  smbus_read_byte_data(dev, reg);

  return res;
}

int ch_write_reg_sequence (int dev, struct reg_ch7036* seq)
{
  int i, res;
  int n_regs = ((seq[0].index & 0xf) << 8) + seq[0].value;

  for (i=1; i <= n_regs; i++) {
    res = smbus_write_byte_data(dev, seq[i].index, seq[i].value);
    changed[ch_page_map[ch7036_cur_page]][seq[i].index] = 1;

    if (res < 0)
      return -1;
    if (seq[i].index == CH7036_PAGE) 
      ch7036_cur_page = ch_page_revmap[seq[i].value & 7];
  }
  return 0;
}

int ch_restore_notin_seq (int dev, struct reg_ch7036* seq)
{
  int i, j;
  int n_regs = ((seq[0].index & 0xf) << 8) + seq[0].value;
  int loc_cp = ch7036_cur_page;
  int err;

  /* Clear the changed vector for anything we are about to write */
  for (i=1; i <= n_regs; i++) {
    changed[ch_page_map[loc_cp]][seq[i].index] = 0;
    if (seq[i].index == CH7036_PAGE) 
      loc_cp = ch_page_revmap[seq[i].value & 7];
  }
  
  for(j=0; j < 5; j++)
    for(i=0; i < 128; i++) if (changed[j][i]) {
	if (i == CH7036_PAGE) continue;
	err = ch_write_reg(dev, ch_page_revmap[j], i, GenDefault(j, i));
	if (err != 0) return err;
      }
  return 0;
}

void ch_reset_datapath (int dev)
{
  /* Reset the datapath then remove reset */
  ch_write_reg(dev, CH7036_RESET, 0x2e);
  ch_write_reg(dev, CH7036_RESET, 0x2f);
}

/* Email from Chrontel 5/20 to reset to defaults */
/* Note: will need to redo step1 after this! */
void ch_reset_todefault (int dev)
{
  int i;
  /* Reset registers then remove reset */
  ch_write_reg(dev, CH7036_RESET, 0x2d);
  ch_write_reg(dev, CH7036_RESET, 0x2f);

  for(i=0; i < 256; i++) 
    changed[0][i] = changed[1][i] = changed[2][i] = 
      changed[3][i] = changed[4][i] = 0;

}

/* Turn the monitor on */
void ch_monitor_on(int dev)
{
  ch_write_reg(dev, CH7036_PWRST2, 0xD0); // HDMI driver, SDRAM. SPDIF on
  ch_write_reg(dev, CH7036_PWRST3, 0x0F); // VGA dac powered down
  ch_write_reg(dev, CH7036_PWRST4, 0x02); // SDRAM, HDMI power on, RGB off

  ch_write_reg(dev, CH7036_MAGIC2_0E, 0x13); // MAGIC
  ch_write_reg(dev, CH7036_SDTIME1, 0x20); // HDMI driver, PLL power on
  
  ch_write_reg(dev, CH7036_PWRSTATE1, 0x84); // Normal (pll on)
  ch_reset_datapath(dev);
}

/* Turn the monitor off */
void ch_monitor_off(int dev)
{
  ch_write_reg(dev, CH7036_PWRST2, 0xDF); // HDMI driver, SDRAM. SPDIF off
  ch_write_reg(dev, CH7036_PWRST3, 0xFF); // VGA dac powered down
  ch_write_reg(dev, CH7036_PWRST4, 0x7F); // Stop hdmi,scaler, pwr down

  ch_write_reg(dev, CH7036_MAGIC2_0E, 0x93); // MAGIC
  ch_write_reg(dev, CH7036_SDTIME1, 0x29); // HDMI driver, PLL power off
  
  ch_write_reg(dev, CH7036_PWRSTATE1, 0x94); // pll power down
}

/* Turn the monitor off but keep DDC running so EDID can still be read */
void ch_monitor_off_keep_ddc(int dev)
{
  ch_write_reg(dev, CH7036_PWRST2, 0xDF); // HDMI driver, SDRAM. SPDIF off
  ch_write_reg(dev, CH7036_PWRST3, 0xFF); // VGA dac powered down
  ch_write_reg(dev, CH7036_PWRST4, 0x7F); // Stop hdmi,scaler, pwr down

  /* Chrontel email response 2010-06-17 to Off preventing EDID reads */
  /* Please remove both  0xE and 0x16 registers */
  /* 0xE will power down DDC */
  
  ch_write_reg(dev, CH7036_PWRSTATE1, 0x94); // pll power down
}

int ch_hdmi_is_on(int dev)
{
  int res;
  res = ch_read_reg(dev, CH7036_PWRST2);
  if (res < 0) {
    fprintf(stderr, "Failed to read powerstate 2 to check hdmi on\n");
    return 0;
  }
  /* Check if HDMI driver is powered down (bit set) */
  return (res & 0x8) ? 0 : 1;
}

/* Calculate INC registers from values the chip provides */

void ch_calculate_incs(int dev)
{
  unsigned int hinca, hincb, hinc;
  unsigned int vinca, vincb, vinc;
  unsigned int hdinca, hdincb, hdinc;

  //first read out parameters:

  hinca = ((ch_read_reg(dev, CH7036_HINCA1) << 3) | 
	   (ch_read_reg(dev, CH7036_HINCA2) & 0x7));
  hincb = ((ch_read_reg(dev, CH7036_HINCB1) << 3) | 
	   (ch_read_reg(dev, CH7036_HINCB2) & 0x7));
  vinca = ((ch_read_reg(dev, CH7036_VINCA1) << 3) | 
	   (ch_read_reg(dev, CH7036_VINCA2) & 0x7));
  vincb = ((ch_read_reg(dev, CH7036_VINCB1) << 3) | 
	   (ch_read_reg(dev, CH7036_VINCB2) & 0x7));
  hdinca = ((ch_read_reg(dev, CH7036_HDINCA1) << 3) | 
	    (ch_read_reg(dev, CH7036_HDINCA2) & 0x7));
  hdincb = ((ch_read_reg(dev, CH7036_HDINCB1) << 3) | 
	    (ch_read_reg(dev, CH7036_HDINCB2) & 0x7));

  /* calculate them */
  /* NOTE: this code works because the values are 11-bit */
  /* so the shift in a 32-bit uint will not drop any bits from the top */
  if (hinca == 0 || hincb == 0) {
    hinc = 0;
  } else {
    hinc = (1 << 20) * hinca / hincb;
  }

  if (vinca == 0 || vincb == 0) {
    vinc = 0;
  } else {
    vinc = (1 << 20) * vinca / vincb;
  }

  if (hdinca == 0 || hdincb == 0) {
    hdinc = 0;
  } else {
    hdinc = (1 << 20) * hdinca / hdincb;
  }

  ch_write_reg(dev, CH7036_HINCC1,(hinc>>16) & 0xFF);
  ch_write_reg(dev, CH7036_HINCC2,(hinc>>8)  & 0xFF);
  ch_write_reg(dev, CH7036_HINCC3, hinc      & 0xFF);

  ch_write_reg(dev, CH7036_VINCC1,(vinc>>16) & 0xFF);
  ch_write_reg(dev, CH7036_VINCC2,(vinc>>8)  & 0xFF);
  ch_write_reg(dev, CH7036_VINCC3, vinc      & 0xFF);

  ch_write_reg(dev, CH7036_HDINCC1,(hdinc>>16) & 0xFF);
  ch_write_reg(dev, CH7036_HDINCC2,(hdinc>>8)  & 0xFF);
  ch_write_reg(dev, CH7036_HDINCC3, hdinc      & 0xFF);
}



/* Load Firmware */

int ch_load_firmware(int dev, char* fname)
{
  unsigned char buf[10240];
  FILE* fp;
  long fs, fs1;
  int res = -1;

  do {

    fp = fopen(fname, "rb");
    if (fp==NULL) {
      fprintf(stderr, "Could not open %s to read firmware\n", fname);
      break;
    }
    fseek( fp, 0, SEEK_END);
    fs = ftell(fp);
    fseek( fp, 0, SEEK_SET);

    fs1 = fread( buf, 1, fs, fp);
    if (fs1 != fs) {
      printf("File %s reading error!\n", fname);
      break;
    }

    printf("Firmware size = %d bytes.\n", (int)fs);
    /* Initialize the firmware download */
    ch_write_reg(dev, 4, 0x52, 0x2b);
    ch_write_reg(dev, 4, 0x5b, 0x9e);
    ch_write_reg(dev, 4, 0x5b, 0xb3);
    smbus_write_byte_data(dev, 0x03, 0x07);
    /*  Download the firmware */
    /*  "The downloading can be byte by byte, or using I2C block write" */
    /* FIXME mdhayter: Did not get the block write to work with count > 1 */
    /* The SMBus hangs completely using I2C_SMBUS_I2C_BLOCK which is the  */
    /* version that does not send the length required by Chrontel (unless */
    /* the length is set to 1). The transfers complete ok using the */
    /* I2C_SMBUS_BLOCK version, but the firmware is no good (as expected) */
    /* Could it be the kernel driver? */
    /* Using word seems ok and halves the number of transactions */
#ifdef SLOWLOAD
    for (fs1=0; fs1<fs; fs1++) 
      smbus_write_byte_data(dev, 0x07, buf[fs1]);
#endif

#ifndef FASTLOAD
    for(fs1 = 0; (fs1+1) < fs; fs1+=2)
      smbus_write_word_data(dev, 0x07, buf[fs1] | (buf[fs1+1]<<8));
    if (fs1 < fs)
      smbus_write_byte_data(dev, 0x07, buf[fs1]);
#else
#define TXSIZE 32
    for(fs1 = 0; (fs1+TXSIZE) < fs; fs1+=TXSIZE)
      smbus_write_reg_bytes(dev, 0x07, TXSIZE, buf+fs1);
    if (fs1 < fs)
      smbus_write_reg_bytes(dev, 0x07, fs-fs1, buf+fs1);
#endif

    /*  Finishing the download and run the firmware */
    smbus_write_byte_data(dev, 0x03, 0x04);
    smbus_write_byte_data(dev, 0x52, 0x2F);
    smbus_write_byte_data(dev, 0x03, 0x00);
    ch7036_cur_page = -1;
    res = 0;

  } while(0);

  if (fp!=NULL) fclose(fp);
  return res;
}


/* Note -- this only works if firmware is loaded */
/* will time out if the firmware is not running  */
int ch_mcu_version(int dev, int newFirmware, int verbose)
{
  unsigned char data[16];
  FW7036_CFG *config = (FW7036_CFG *) &data[0];
  struct timeval tv;
  /* This timeout needs to be tuned. 500,000 works but is a bit */
  /* long when trying to detect the firmware going away         */
  /* 100,000 seems to sometimes give false timeouts             */
  /* 200,000 is good except when the firmware is restarting     */
  struct timeval tout = {0, 200000};
  if (newFirmware) tout.tv_usec = 500000;

  ch7036_cur_page = -1;

  // Send Host Request
  smbus_write_byte_data(dev, 0x03, 0x00);
  smbus_write_byte_data(dev, 0x4F, 0x5F);
  // Wait the MCU response
  gettimeofday(&tv, NULL);
  timeradd(&tv, &tout, &tout);

  while (1) {
    smbus_write_byte_data(dev, 0x03, 0x00);
    if (((0x7F & smbus_read_byte_data(dev, 0x4F))==0x1F) || (dev < 0)) {
      int i;
      smbus_write_byte_data(dev, 0x03, 0x04);
      for (i=0; i<sizeof(FW7036_CFG); i++) {
	// data[16] holds the return parameter block from MCU
	data[i] = smbus_read_byte_data(dev, es_map[i] );
      }
      smbus_write_byte_data(dev, 0x03, 0x00);
      break;
    }
    gettimeofday(&tv, NULL);
    if (timercmp(&tv, &tout, >)) {
      fprintf(stderr, "Timeout waiting for MCU version\n");
      return -1;
    }
  }
  if (verbose)
    printf("CH7036 MCU ver %d.%d, Device %02x rev %d, capability 0x%x\n",
	   config->ver_major, config->ver_minor,
	   config->did, config->rid, config->capbility);
  return (config->ver_major<<8) | config->ver_minor;
}

/* Note -- this only works if firmware is loaded */
/* Always says hdmi detected if firmware is not loaded */
int ch_hdmi_detected(int dev)
{
  int res;
  res = ch_read_reg(dev, CH7036_HDMI_ST);
  if (res < 0) return res;

  res = (res & 0x10) ? 1 : 0;
  return res;
}

/* Note -- this only works if firmware is loaded */
/* will time out if the firmware is not running  */
int ch_get_edid(int dev, int edid_block, unsigned char* edid)
{
  int j;
  int res;
  struct timeval tv;
  struct timeval tout = {0, 500000};
  ch7036_cur_page = -1;

  // edid_block
  for (j=0; j<8; j++) {
    // Send Host Request
    res = smbus_write_byte_data(dev, 0x03, 0x00);
    if (res < 0) return res;
    // one edid block contains 128 bytes, so we need read 8 blocks
    res = smbus_write_byte_data(dev, 0x50, ((edid_block*8)+j));
    if (res < 0) return res;
    res = smbus_write_byte_data(dev, 0x4F, 0x41);
    if (res < 0) return res;
    // Wait the MCU response
    gettimeofday(&tv, NULL);
    timeradd(&tv, &tout, &tout);
    while (1) {
      res = smbus_write_byte_data(dev, 0x03, 0x00);
      if (res < 0) return res;
      // Order in dummy case should do one read...
      if (((0x7F & smbus_read_byte_data(dev, 0x4F))==0x01) || (dev < 0)) {
	int i;
	smbus_write_byte_data(dev, 0x03, 0x04);
	for (i=0; i<16; i++) {
	  edid[j*16 + i] =  smbus_read_byte_data(dev, es_map[i]);
	}
	smbus_write_byte_data(dev, 0x03, 0x00);
	break;
      }
      gettimeofday(&tv, NULL);
      if (timercmp(&tv, &tout, >)) {
	fprintf(stderr, "Timeout waiting for MCU to return EDID\n");
	return -1;
      }
    }
  }
  return 0;
}


