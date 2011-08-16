/*
 * ALC269/ALC270/ALC275/ALC276 quirk models
 * included by patch_realtek.c
 */

/* ALC269 models */
enum {
	ALC269_AUTO,
	ALC269_BASIC,
	ALC269_QUANTA_FL1,
	ALC269_AMIC,
	ALC269_DMIC,
	ALC269VB_AMIC,
	ALC269VB_DMIC,
	ALC269_MODEL_LAST /* last tag */
};

/*
 *  ALC269 channel source setting (2 channel)
 */
#define ALC269_DIGOUT_NID	ALC880_DIGOUT_NID

#define alc269_dac_nids		alc260_dac_nids

static const hda_nid_t alc269_adc_nids[1] = {
	/* ADC1 */
	0x08,
};

static const hda_nid_t alc269_capsrc_nids[1] = {
	0x23,
};

static const hda_nid_t alc269vb_adc_nids[1] = {
	/* ADC1 */
	0x09,
};

static const hda_nid_t alc269vb_capsrc_nids[1] = {
	0x22,
};

#define alc269_modes		alc260_modes
#define alc269_capture_source	alc880_lg_lw_capture_source

static const struct snd_kcontrol_new alc269_base_mixer[] = {
	HDA_CODEC_VOLUME("Front Playback Volume", 0x02, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Front Playback Switch", 0x14, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Line Playback Volume", 0x0b, 0x02, HDA_INPUT),
	HDA_CODEC_MUTE("Line Playback Switch", 0x0b, 0x02, HDA_INPUT),
	HDA_CODEC_VOLUME("Mic Playback Volume", 0x0b, 0x0, HDA_INPUT),
	HDA_CODEC_MUTE("Mic Playback Switch", 0x0b, 0x0, HDA_INPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	HDA_CODEC_VOLUME("Front Mic Playback Volume", 0x0b, 0x01, HDA_INPUT),
	HDA_CODEC_MUTE("Front Mic Playback Switch", 0x0b, 0x01, HDA_INPUT),
	HDA_CODEC_VOLUME("Front Mic Boost Volume", 0x19, 0, HDA_INPUT),
	HDA_CODEC_MUTE("Headphone Playback Switch", 0x15, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE_MONO("Mono Playback Switch", 0x16, 2, 0x0, HDA_OUTPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc269_laptop_mixer[] = {
	HDA_CODEC_MUTE("Speaker Playback Switch", 0x14, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Speaker Playback Volume", 0x02, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Headphone Playback Switch", 0x15, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Headphone Playback Volume", 0x03, 0x0, HDA_OUTPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc269vb_laptop_mixer[] = {
	HDA_CODEC_MUTE("Speaker Playback Switch", 0x14, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Speaker Playback Volume", 0x02, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Headphone Playback Switch", 0x21, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Headphone Playback Volume", 0x03, 0x0, HDA_OUTPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc269_asus_mixer[] = {
	HDA_CODEC_VOLUME("Master Playback Volume", 0x02, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Master Playback Switch", 0x0c, 0x0, HDA_INPUT),
	{ } /* end */
};

/* capture mixer elements */
static const struct snd_kcontrol_new alc269_laptop_analog_capture_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x08, 0x0, HDA_INPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x08, 0x0, HDA_INPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	HDA_CODEC_VOLUME("Internal Mic Boost Volume", 0x19, 0, HDA_INPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc269_laptop_digital_capture_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x08, 0x0, HDA_INPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x08, 0x0, HDA_INPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc269vb_laptop_analog_capture_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x09, 0x0, HDA_INPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x09, 0x0, HDA_INPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	HDA_CODEC_VOLUME("Internal Mic Boost Volume", 0x19, 0, HDA_INPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc269vb_laptop_digital_capture_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x09, 0x0, HDA_INPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x09, 0x0, HDA_INPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	{ } /* end */
};

static const struct hda_verb alc269_laptop_dmic_init_verbs[] = {
	{0x15, AC_VERB_SET_CONNECT_SEL, 0x01},
	{0x23, AC_VERB_SET_CONNECT_SEL, 0x05},
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, 0xb026 },
	{0x08, AC_VERB_SET_AMP_GAIN_MUTE, (0x7019 | (0x00 << 8))},
	{0x12, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},
	{0x18, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_MIC_EVENT},
	{0x15, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_HP_EVENT},
	{}
};

static const struct hda_verb alc269_laptop_amic_init_verbs[] = {
	{0x15, AC_VERB_SET_CONNECT_SEL, 0x01},
	{0x23, AC_VERB_SET_CONNECT_SEL, 0x01},
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, 0xb026 },
	{0x08, AC_VERB_SET_AMP_GAIN_MUTE, (0x701b | (0x00 << 8))},
	{0x18, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_MIC_EVENT},
	{0x15, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_HP_EVENT},
	{}
};

static const struct hda_verb alc269vb_laptop_dmic_init_verbs[] = {
	{0x21, AC_VERB_SET_CONNECT_SEL, 0x01},
	{0x22, AC_VERB_SET_CONNECT_SEL, 0x06},
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, 0xb026 },
	{0x09, AC_VERB_SET_AMP_GAIN_MUTE, (0x7019 | (0x00 << 8))},
	{0x12, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},
	{0x18, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_MIC_EVENT},
	{0x21, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_HP_EVENT},
	{}
};

static const struct hda_verb alc269vb_laptop_amic_init_verbs[] = {
	{0x21, AC_VERB_SET_CONNECT_SEL, 0x01},
	{0x22, AC_VERB_SET_CONNECT_SEL, 0x01},
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, 0xb026 },
	{0x09, AC_VERB_SET_AMP_GAIN_MUTE, (0x7019 | (0x00 << 8))},
	{0x12, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},
	{0x18, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_MIC_EVENT},
	{0x21, AC_VERB_SET_UNSOLICITED_ENABLE, AC_USRSP_EN | ALC_HP_EVENT},
	{}
};

static void alc269_laptop_amic_setup(struct hda_codec *codec)
{
	struct alc_spec *spec = codec->spec;
	spec->autocfg.hp_pins[0] = 0x15;
	spec->autocfg.speaker_pins[0] = 0x14;
	spec->automute_mixer_nid[0] = 0x0c;
	spec->automute = 1;
	spec->automute_mode = ALC_AUTOMUTE_MIXER;
	spec->ext_mic_pin = 0x18;
	spec->int_mic_pin = 0x19;
	spec->auto_mic = 1;
}

static void alc269_laptop_dmic_setup(struct hda_codec *codec)
{
	struct alc_spec *spec = codec->spec;
	spec->autocfg.hp_pins[0] = 0x15;
	spec->autocfg.speaker_pins[0] = 0x14;
	spec->automute_mixer_nid[0] = 0x0c;
	spec->automute = 1;
	spec->automute_mode = ALC_AUTOMUTE_MIXER;
	spec->ext_mic_pin = 0x18;
	spec->int_mic_pin = 0x12;
	spec->auto_mic = 1;
}

static void alc269vb_laptop_amic_setup(struct hda_codec *codec)
{
	struct alc_spec *spec = codec->spec;
	spec->autocfg.hp_pins[0] = 0x21;
	spec->autocfg.speaker_pins[0] = 0x14;
	spec->automute_mixer_nid[0] = 0x0c;
	spec->automute = 1;
	spec->automute_mode = ALC_AUTOMUTE_MIXER;
	spec->ext_mic_pin = 0x18;
	spec->int_mic_pin = 0x19;
	spec->auto_mic = 1;
}

static void alc269vb_laptop_dmic_setup(struct hda_codec *codec)
{
	struct alc_spec *spec = codec->spec;
	spec->autocfg.hp_pins[0] = 0x21;
	spec->autocfg.speaker_pins[0] = 0x14;
	spec->automute_mixer_nid[0] = 0x0c;
	spec->automute = 1;
	spec->automute_mode = ALC_AUTOMUTE_MIXER;
	spec->ext_mic_pin = 0x18;
	spec->int_mic_pin = 0x12;
	spec->auto_mic = 1;
}

/*
 * generic initialization of ADC, input mixers and output mixers
 */
static const struct hda_verb alc269_init_verbs[] = {
	/*
	 * Unmute ADC0 and set the default input to mic-in
	 */
	{0x08, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},

	/*
	 * Set up output mixers (0x02 - 0x03)
	 */
	/* set vol=0 to output mixers */
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},
	{0x03, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},

	/* set up input amps for analog loopback */
	/* Amp Indices: DAC = 0, mixer = 1 */
	{0x0c, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0c, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(1)},
	{0x0d, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0d, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(1)},
	{0x0e, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0e, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(1)},

	{0x14, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_OUT},
	{0x15, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_HP},
	{0x16, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_OUT},
	{0x18, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_VREF80},
	{0x19, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_VREF80},
	{0x1a, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},
	{0x1b, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},

	{0x14, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_UNMUTE},
	{0x15, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_UNMUTE},

	/* FIXME: use Mux-type input source selection */
	/* Mixer elements: 0x18, 19, 1a, 1b, 1d, 0b */
	/* Input mixer1: unmute Mic, F-Mic, Line, CD inputs */
	{0x23, AC_VERB_SET_CONNECT_SEL, 0x00},

	/* set EAPD */
	{0x14, AC_VERB_SET_EAPD_BTLENABLE, 2},
	{ }
};

static const struct hda_verb alc269vb_init_verbs[] = {
	/*
	 * Unmute ADC0 and set the default input to mic-in
	 */
	{0x09, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},

	/*
	 * Set up output mixers (0x02 - 0x03)
	 */
	/* set vol=0 to output mixers */
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},
	{0x03, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},

	/* set up input amps for analog loopback */
	/* Amp Indices: DAC = 0, mixer = 1 */
	{0x0c, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0c, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(1)},
	{0x0d, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0d, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(1)},
	{0x0f, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0f, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(1)},

	{0x14, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_OUT},
	{0x21, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_HP},
	{0x16, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_OUT},
	{0x18, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_VREF80},
	{0x19, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_VREF80},
	{0x1a, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},
	{0x1b, AC_VERB_SET_PIN_WIDGET_CONTROL, PIN_IN},

	{0x14, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_UNMUTE},
	{0x21, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_UNMUTE},

	/* FIXME: use Mux-type input source selection */
	/* Mixer elements: 0x18, 19, 1a, 1b, 1d, 0b */
	/* Input mixer1: unmute Mic, F-Mic, Line, CD inputs */
	{0x22, AC_VERB_SET_CONNECT_SEL, 0x00},

	/* set EAPD */
	{0x14, AC_VERB_SET_EAPD_BTLENABLE, 2},
	{ }
};

/*
 * configuration and preset
 */
static const char * const alc269_models[ALC269_MODEL_LAST] = {
	[ALC269_BASIC]			= "basic",
	[ALC269_AMIC]			= "laptop-amic",
	[ALC269_DMIC]			= "laptop-dmic",
	[ALC269_AUTO]			= "auto",
};

static const struct snd_pci_quirk alc269_cfg_tbl[] = {
	SND_PCI_QUIRK(0x1043, 0x8330, "ASUS Eeepc P703 P900A",
		      ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1013, "ASUS N61Da", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1113, "ASUS N63Jn", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1143, "ASUS B53f", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1133, "ASUS UJ20ft", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1183, "ASUS K72DR", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x11b3, "ASUS K52DR", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x11e3, "ASUS U33Jc", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1273, "ASUS UL80Jt", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1283, "ASUS U53Jc", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x12b3, "ASUS N82JV", ALC269VB_AMIC),
	SND_PCI_QUIRK(0x1043, 0x12d3, "ASUS N61Jv", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x13a3, "ASUS UL30Vt", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1373, "ASUS G73JX", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1383, "ASUS UJ30Jc", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x13d3, "ASUS N61JA", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1413, "ASUS UL50", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1443, "ASUS UL30", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1453, "ASUS M60Jv", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1483, "ASUS UL80", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x14f3, "ASUS F83Vf", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x14e3, "ASUS UL20", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1513, "ASUS UX30", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1593, "ASUS N51Vn", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x15a3, "ASUS N60Jv", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x15b3, "ASUS N60Dp", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x15c3, "ASUS N70De", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x15e3, "ASUS F83T", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1643, "ASUS M60J", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1653, "ASUS U50", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1693, "ASUS F50N", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x16a3, "ASUS F5Q", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1723, "ASUS P80", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1743, "ASUS U80", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1773, "ASUS U20A", ALC269_AMIC),
	SND_PCI_QUIRK(0x1043, 0x1883, "ASUS F81Se", ALC269_AMIC),
	SND_PCI_QUIRK(0x104d, 0x9071, "Sony VAIO", ALC269_AUTO),
	SND_PCI_QUIRK(0x152d, 0x1778, "Quanta ON1", ALC269_DMIC),
	SND_PCI_QUIRK(0x17aa, 0x3be9, "Quanta Wistron", ALC269_AMIC),
	SND_PCI_QUIRK(0x17aa, 0x3bf8, "Quanta FL1", ALC269_AMIC),
	SND_PCI_QUIRK(0x17ff, 0x059a, "Quanta EL3", ALC269_DMIC),
	SND_PCI_QUIRK(0x17ff, 0x059b, "Quanta JR1", ALC269_DMIC),
	{}
};

static const struct alc_config_preset alc269_presets[] = {
	[ALC269_BASIC] = {
		.mixers = { alc269_base_mixer },
		.init_verbs = { alc269_init_verbs },
		.num_dacs = ARRAY_SIZE(alc269_dac_nids),
		.dac_nids = alc269_dac_nids,
		.hp_nid = 0x03,
		.num_channel_mode = ARRAY_SIZE(alc269_modes),
		.channel_mode = alc269_modes,
		.input_mux = &alc269_capture_source,
	},
	[ALC269_AMIC] = {
		.mixers = { alc269_laptop_mixer },
		.cap_mixer = alc269_laptop_analog_capture_mixer,
		.init_verbs = { alc269_init_verbs,
				alc269_laptop_amic_init_verbs },
		.num_dacs = ARRAY_SIZE(alc269_dac_nids),
		.dac_nids = alc269_dac_nids,
		.hp_nid = 0x03,
		.num_channel_mode = ARRAY_SIZE(alc269_modes),
		.channel_mode = alc269_modes,
		.unsol_event = alc_sku_unsol_event,
		.setup = alc269_laptop_amic_setup,
		.init_hook = alc_inithook,
	},
	[ALC269_DMIC] = {
		.mixers = { alc269_laptop_mixer },
		.cap_mixer = alc269_laptop_digital_capture_mixer,
		.init_verbs = { alc269_init_verbs,
				alc269_laptop_dmic_init_verbs },
		.num_dacs = ARRAY_SIZE(alc269_dac_nids),
		.dac_nids = alc269_dac_nids,
		.hp_nid = 0x03,
		.num_channel_mode = ARRAY_SIZE(alc269_modes),
		.channel_mode = alc269_modes,
		.unsol_event = alc_sku_unsol_event,
		.setup = alc269_laptop_dmic_setup,
		.init_hook = alc_inithook,
	},
	[ALC269VB_AMIC] = {
		.mixers = { alc269vb_laptop_mixer },
		.cap_mixer = alc269vb_laptop_analog_capture_mixer,
		.init_verbs = { alc269vb_init_verbs,
				alc269vb_laptop_amic_init_verbs },
		.num_dacs = ARRAY_SIZE(alc269_dac_nids),
		.dac_nids = alc269_dac_nids,
		.hp_nid = 0x03,
		.num_channel_mode = ARRAY_SIZE(alc269_modes),
		.channel_mode = alc269_modes,
		.unsol_event = alc_sku_unsol_event,
		.setup = alc269vb_laptop_amic_setup,
		.init_hook = alc_inithook,
	},
	[ALC269VB_DMIC] = {
		.mixers = { alc269vb_laptop_mixer },
		.cap_mixer = alc269vb_laptop_digital_capture_mixer,
		.init_verbs = { alc269vb_init_verbs,
				alc269vb_laptop_dmic_init_verbs },
		.num_dacs = ARRAY_SIZE(alc269_dac_nids),
		.dac_nids = alc269_dac_nids,
		.hp_nid = 0x03,
		.num_channel_mode = ARRAY_SIZE(alc269_modes),
		.channel_mode = alc269_modes,
		.unsol_event = alc_sku_unsol_event,
		.setup = alc269vb_laptop_dmic_setup,
		.init_hook = alc_inithook,
	},
};

