/*
 * C-Media CMI8788 driver for Asus Xonar cards
 *
 * Copyright (c) Clemens Ladisch <clemens@ladisch.de>
 *
 *
 *  This driver is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, version 2.
 *
 *  This driver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this driver; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/*
 * SPI 0 -> 1st PCM1796
 * SPI 1 -> 2nd PCM1796
 * SPI 2 -> 3rd PCM1796
 * SPI 3 -> CS5381
 * SPI 4 -> 4th PCM1796
 *
 * GPIO 2 -> M0 of CS5381
 * GPIO 3 -> M1 of CS5381
 * GPIO 5 <- ? (D2X only)
 * GPIO 7 -> ? (ALT?)
 * GPIO 8 -> ? (amps enable?)
 */

#include <sound/driver.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/pcm.h>
#include <sound/tlv.h>
#include "oxygen.h"

MODULE_AUTHOR("Clemens Ladisch <clemens@ladisch.de>");
MODULE_DESCRIPTION("Asus AV200 driver");
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("{{Asus,AV200}}");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;
static int enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "card index");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "enable card");

static struct pci_device_id xonar_ids[] __devinitdata = {
	{ OXYGEN_PCI_SUBID(0x1043, 0x8269) }, /* Asus Xonar D2 */
	{ OXYGEN_PCI_SUBID(0x1043, 0x82b7) }, /* Asus Xonar D2X */
	{ }
};
MODULE_DEVICE_TABLE(pci, xonar_ids);

/* register 0x12 */
#define PCM1796_MUTE		0x01
#define PCM1796_FMT_24_MSB	0x30
#define PCM1796_ATLD		0x80
/* register 0x14 */
#define PCM1796_OS_64		0x00
#define PCM1796_OS_32		0x01
#define PCM1796_OS_128		0x02

static void pcm1796_write(struct oxygen *chip, unsigned int codec,
			  u8 reg, u8 value)
{
	/* maps ALSA channel pair number to SPI output */
	static const u8 codec_map[4] = {
		0, 4, 2, 1
	};
	oxygen_write_spi(chip, OXYGEN_SPI_TRIGGER_WRITE |
			 OXYGEN_SPI_DATA_LENGTH_2 |
			 (codec_map[codec] << OXYGEN_SPI_CODEC_SHIFT) |
			 OXYGEN_SPI_MAGIC,
			 (reg << 8) | value);
}

static void xonar_init(struct oxygen *chip)
{
	unsigned int i;

	for (i = 0; i < 4; ++i) {
		pcm1796_write(chip, i, 0x12, PCM1796_FMT_24_MSB | PCM1796_ATLD);
		pcm1796_write(chip, i, 0x13, 0);
		pcm1796_write(chip, i, 0x14, PCM1796_OS_64);
		pcm1796_write(chip, i, 0x15, 0);
		pcm1796_write(chip, i, 0x10, 0xff);
		pcm1796_write(chip, i, 0x11, 0xff);
	}

	oxygen_set_bits16(chip, OXYGEN_GPIO_CONTROL, 0x8c);
	oxygen_write16_masked(chip, OXYGEN_GPIO_DATA, 0x00, 0x0c);
#if 0
	oxygen_clear_bits16(chip, OXYGEN_I2S_MULTICH_FORMAT,
			    OXYGEN_I2S_MAGIC1_MASK);
#endif
	oxygen_ac97_set_bits(chip, 0, 0x62, 0x0080);
	msleep(300);
	oxygen_set_bits16(chip, OXYGEN_GPIO_CONTROL, 0x100);
	oxygen_set_bits16(chip, OXYGEN_GPIO_DATA, 0x100);

	snd_component_add(chip->card, "PCM1796");
	snd_component_add(chip->card, "CS5381");
}

static void xonar_cleanup(struct oxygen *chip)
{
	oxygen_clear_bits16(chip, OXYGEN_GPIO_DATA, 0x100);
}

static void set_pcm1796_params(struct oxygen *chip,
			       struct snd_pcm_hw_params *params)
{
#if 0
	unsigned int i;
	u8 value;

	value = params_rate(params) >= 96000 ? PCM1796_OS_32 : PCM1796_OS_64;
	for (i = 0; i < 4; ++i)
		pcm1796_write(chip, i, 0x14, value);
#endif
}

static void update_pcm1796_volume(struct oxygen *chip)
{
	unsigned int i;

	for (i = 0; i < 4; ++i) {
		pcm1796_write(chip, i, 0x10, chip->dac_volume[i * 2]);
		pcm1796_write(chip, i, 0x11, chip->dac_volume[i * 2 + 1]);
	}
}

static void update_pcm1796_mute(struct oxygen *chip)
{
	unsigned int i;
	u8 value;

	value = PCM1796_FMT_24_MSB | PCM1796_ATLD;
	if (chip->dac_mute)
		value |= PCM1796_MUTE;
	for (i = 0; i < 4; ++i)
		pcm1796_write(chip, i, 0x12, value);
}

static void set_cs5381_params(struct oxygen *chip,
			      struct snd_pcm_hw_params *params)
{
	unsigned int value;

	if (params_rate(params) <= 54000)
		value = 0;
	else if (params_rate(params) <= 108000)
		value = 4;
	else
		value = 8;
	oxygen_write16_masked(chip, OXYGEN_GPIO_DATA, value, 0x000c);
}

const DECLARE_TLV_DB_SCALE(pcm1796_db_scale, -12000, 50, 0);

const struct oxygen_model model_xonar = {
	.shortname = "Asus AV200",
	.longname = "Asus Virtuoso 200",
	.chip = "AV200",
	.init = xonar_init,
	.cleanup = xonar_cleanup,
	.set_dac_params = set_pcm1796_params,
	.set_adc_params = set_cs5381_params,
	.update_dac_volume = update_pcm1796_volume,
	.update_dac_mute = update_pcm1796_mute,
	.dac_tlv = pcm1796_db_scale,
	.record_from_dma_b = 1,
	.cd_in_from_video_in = 1,
	.dac_minimum_volume = 15,
};

static int __devinit xonar_probe(struct pci_dev *pci,
				 const struct pci_device_id *pci_id)
{
	static int dev;
	int err;

	if (dev >= SNDRV_CARDS)
		return -ENODEV;
	if (!enable[dev]) {
		++dev;
		return -ENOENT;
	}
	err = oxygen_pci_probe(pci, index[dev], id[dev], &model_xonar);
	if (err >= 0)
		++dev;
	return err;
}

static struct pci_driver xonar_driver = {
	.name = "AV200",
	.id_table = xonar_ids,
	.probe = xonar_probe,
	.remove = __devexit_p(oxygen_pci_remove),
};

static int __init alsa_card_xonar_init(void)
{
	return pci_register_driver(&xonar_driver);
}

static void __exit alsa_card_xonar_exit(void)
{
	pci_unregister_driver(&xonar_driver);
}

module_init(alsa_card_xonar_init)
module_exit(alsa_card_xonar_exit)

EXPORT_NO_SYMBOLS;