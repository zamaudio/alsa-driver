/*
 * ALC267/ALC268 quirk models
 * included by patch_realtek.c
 */

/* ALC268 models */
enum {
	ALC268_AUTO,
	ALC267_QUANTA_IL1,
	ALC268_3ST,
#ifdef CONFIG_SND_DEBUG
	ALC268_TEST,
#endif
	ALC268_MODEL_LAST /* last tag */
};

/*
 *  ALC268 channel source setting (2 channel)
 */
#define ALC268_DIGOUT_NID	ALC880_DIGOUT_NID
#define alc268_modes		alc260_modes

static const hda_nid_t alc268_dac_nids[2] = {
	/* front, hp */
	0x02, 0x03
};

static const hda_nid_t alc268_adc_nids[2] = {
	/* ADC0-1 */
	0x08, 0x07
};

static const hda_nid_t alc268_adc_nids_alt[1] = {
	/* ADC0 */
	0x08
};

static const hda_nid_t alc268_capsrc_nids[2] = { 0x23, 0x24 };

static const struct snd_kcontrol_new alc268_base_mixer[] = {
	/* output mixer control */
	HDA_CODEC_VOLUME("Front Playback Volume", 0x2, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Front Playback Switch", 0x14, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Headphone Playback Volume", 0x3, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Headphone Playback Switch", 0x15, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	HDA_CODEC_VOLUME("Front Mic Boost Volume", 0x19, 0, HDA_INPUT),
	HDA_CODEC_VOLUME("Line In Boost Volume", 0x1a, 0, HDA_INPUT),
	{ }
};

static const struct hda_verb alc268_eapd_verbs[] = {
	{0x14, AC_VERB_SET_EAPD_BTLENABLE, 2},
	{0x15, AC_VERB_SET_EAPD_BTLENABLE, 2},
	{ }
};

static const struct snd_kcontrol_new alc267_quanta_il1_mixer[] = {
	HDA_CODEC_VOLUME("Speaker Playback Volume", 0x2, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Speaker Playback Switch", 0x14, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Headphone Playback Volume", 0x3, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Headphone Playback Switch", 0x15, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Mic Capture Volume", 0x23, 0x0, HDA_OUTPUT),
	HDA_BIND_MUTE("Mic Capture Switch", 0x23, 2, HDA_OUTPUT),
	HDA_CODEC_VOLUME("Mic Boost Volume", 0x18, 0, HDA_INPUT),
	HDA_CODEC_VOLUME("Internal Mic Boost Volume", 0x19, 0, HDA_INPUT),
	{ }
};

static const struct hda_verb alc267_quanta_il1_verbs[] = {
	{0x15, AC_VERB_SET_UNSOLICITED_ENABLE, ALC_HP_EVENT | AC_USRSP_EN},
	{0x18, AC_VERB_SET_UNSOLICITED_ENABLE, ALC_MIC_EVENT | AC_USRSP_EN},
	{ }
};

static void alc267_quanta_il1_setup(struct hda_codec *codec)
{
	struct alc_spec *spec = codec->spec;
	spec->autocfg.hp_pins[0] = 0x15;
	spec->autocfg.speaker_pins[0] = 0x14;
	spec->ext_mic_pin = 0x18;
	spec->int_mic_pin = 0x19;
	spec->auto_mic = 1;
	spec->automute = 1;
	spec->automute_mode = ALC_AUTOMUTE_PIN;
}

/*
 * generic initialization of ADC, input mixers and output mixers
 */
static const struct hda_verb alc268_base_init_verbs[] = {
	/* Unmute DAC0-1 and set vol = 0 */
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},
	{0x03, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},

	/*
	 * Set up output mixers (0x0c - 0x0e)
	 */
	/* set vol=0 to output mixers */
	{0x0e, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
        {0x0e, AC_VERB_SET_CONNECT_SEL, 0x00},

	{0x0f, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x10, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},

	{0x14, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x40},
	{0x15, AC_VERB_SET_PIN_WIDGET_CONTROL, 0xc0},
	{0x16, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x40},
	{0x18, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x24},
	{0x19, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x24},
	{0x1a, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x20},
	{0x1c, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x20},
	{0x1d, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x20},

	{0x14, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},
	{0x15, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},
	{0x16, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},
	{0x18, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},
	{0x1a, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},

	/* set PCBEEP vol = 0, mute connections */
	{0x1d, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0f, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_MUTE(1)},
	{0x10, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_MUTE(1)},

	/* Unmute Selector 23h,24h and set the default input to mic-in */

	{0x23, AC_VERB_SET_CONNECT_SEL, 0x00},
	{0x23, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_UNMUTE},
	{0x24, AC_VERB_SET_CONNECT_SEL, 0x00},
	{0x24, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_UNMUTE},

	{ }
};

/* only for model=test */
#ifdef CONFIG_SND_DEBUG
/*
 * generic initialization of ADC, input mixers and output mixers
 */
static const struct hda_verb alc268_volume_init_verbs[] = {
	/* set output DAC */
	{0x02, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},
	{0x03, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_ZERO},

	{0x18, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x24},
	{0x19, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x24},
	{0x1a, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x20},
	{0x1c, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x20},
	{0x1d, AC_VERB_SET_PIN_WIDGET_CONTROL, 0x20},

	{0x0e, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x0f, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},
	{0x10, AC_VERB_SET_AMP_GAIN_MUTE, AMP_IN_UNMUTE(0)},

	{0x18, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},
	{0x1a, AC_VERB_SET_AMP_GAIN_MUTE, AMP_OUT_MUTE},
	{ }
};
#endif /* CONFIG_SND_DEBUG */

static const struct snd_kcontrol_new alc268_capture_nosrc_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x23, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x23, 0x0, HDA_OUTPUT),
	{ } /* end */
};

static const struct snd_kcontrol_new alc268_capture_alt_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x23, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x23, 0x0, HDA_OUTPUT),
	_DEFINE_CAPSRC(1),
	{ } /* end */
};

static const struct snd_kcontrol_new alc268_capture_mixer[] = {
	HDA_CODEC_VOLUME("Capture Volume", 0x23, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE("Capture Switch", 0x23, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME_IDX("Capture Volume", 1, 0x24, 0x0, HDA_OUTPUT),
	HDA_CODEC_MUTE_IDX("Capture Switch", 1, 0x24, 0x0, HDA_OUTPUT),
	_DEFINE_CAPSRC(2),
	{ } /* end */
};

static const struct hda_input_mux alc268_capture_source = {
	.num_items = 4,
	.items = {
		{ "Mic", 0x0 },
		{ "Front Mic", 0x1 },
		{ "Line", 0x2 },
		{ "CD", 0x3 },
	},
};

#ifdef CONFIG_SND_DEBUG
static const struct snd_kcontrol_new alc268_test_mixer[] = {
	/* Volume widgets */
	HDA_CODEC_VOLUME("LOUT1 Playback Volume", 0x02, 0x0, HDA_OUTPUT),
	HDA_CODEC_VOLUME("LOUT2 Playback Volume", 0x03, 0x0, HDA_OUTPUT),
	HDA_BIND_MUTE_MONO("Mono sum Playback Switch", 0x0e, 1, 2, HDA_INPUT),
	HDA_BIND_MUTE("LINE-OUT sum Playback Switch", 0x0f, 2, HDA_INPUT),
	HDA_BIND_MUTE("HP-OUT sum Playback Switch", 0x10, 2, HDA_INPUT),
	HDA_BIND_MUTE("LINE-OUT Playback Switch", 0x14, 2, HDA_OUTPUT),
	HDA_BIND_MUTE("HP-OUT Playback Switch", 0x15, 2, HDA_OUTPUT),
	HDA_BIND_MUTE("Mono Playback Switch", 0x16, 2, HDA_OUTPUT),
	HDA_CODEC_VOLUME("MIC1 Capture Volume", 0x18, 0x0, HDA_INPUT),
	HDA_BIND_MUTE("MIC1 Capture Switch", 0x18, 2, HDA_OUTPUT),
	HDA_CODEC_VOLUME("MIC2 Capture Volume", 0x19, 0x0, HDA_INPUT),
	HDA_CODEC_VOLUME("LINE1 Capture Volume", 0x1a, 0x0, HDA_INPUT),
	HDA_BIND_MUTE("LINE1 Capture Switch", 0x1a, 2, HDA_OUTPUT),
	/* The below appears problematic on some hardwares */
	/*HDA_CODEC_VOLUME("PCBEEP Playback Volume", 0x1d, 0x0, HDA_INPUT),*/
	HDA_CODEC_VOLUME("PCM-IN1 Capture Volume", 0x23, 0x0, HDA_OUTPUT),
	HDA_BIND_MUTE("PCM-IN1 Capture Switch", 0x23, 2, HDA_OUTPUT),
	HDA_CODEC_VOLUME("PCM-IN2 Capture Volume", 0x24, 0x0, HDA_OUTPUT),
	HDA_BIND_MUTE("PCM-IN2 Capture Switch", 0x24, 2, HDA_OUTPUT),

	/* Modes for retasking pin widgets */
	ALC_PIN_MODE("LINE-OUT pin mode", 0x14, ALC_PIN_DIR_INOUT),
	ALC_PIN_MODE("HP-OUT pin mode", 0x15, ALC_PIN_DIR_INOUT),
	ALC_PIN_MODE("MIC1 pin mode", 0x18, ALC_PIN_DIR_INOUT),
	ALC_PIN_MODE("LINE1 pin mode", 0x1a, ALC_PIN_DIR_INOUT),

	/* Controls for GPIO pins, assuming they are configured as outputs */
	ALC_GPIO_DATA_SWITCH("GPIO pin 0", 0x01, 0x01),
	ALC_GPIO_DATA_SWITCH("GPIO pin 1", 0x01, 0x02),
	ALC_GPIO_DATA_SWITCH("GPIO pin 2", 0x01, 0x04),
	ALC_GPIO_DATA_SWITCH("GPIO pin 3", 0x01, 0x08),

	/* Switches to allow the digital SPDIF output pin to be enabled.
	 * The ALC268 does not have an SPDIF input.
	 */
	ALC_SPDIF_CTRL_SWITCH("SPDIF Playback Switch", 0x06, 0x01),

	/* A switch allowing EAPD to be enabled.  Some laptops seem to use
	 * this output to turn on an external amplifier.
	 */
	ALC_EAPD_CTRL_SWITCH("LINE-OUT EAPD Enable Switch", 0x0f, 0x02),
	ALC_EAPD_CTRL_SWITCH("HP-OUT EAPD Enable Switch", 0x10, 0x02),

	{ } /* end */
};
#endif

/*
 * configuration and preset
 */
static const char * const alc268_models[ALC268_MODEL_LAST] = {
	[ALC267_QUANTA_IL1]	= "quanta-il1",
	[ALC268_3ST]		= "3stack",
#ifdef CONFIG_SND_DEBUG
	[ALC268_TEST]		= "test",
#endif
	[ALC268_AUTO]		= "auto",
};

static const struct snd_pci_quirk alc268_cfg_tbl[] = {
	SND_PCI_QUIRK(0x1043, 0x1205, "ASUS W7J", ALC268_3ST),
	SND_PCI_QUIRK(0x152d, 0x0771, "Quanta IL1", ALC267_QUANTA_IL1),
	{}
};

static const struct alc_config_preset alc268_presets[] = {
	[ALC267_QUANTA_IL1] = {
		.mixers = { alc267_quanta_il1_mixer, alc268_beep_mixer },
		.cap_mixer = alc268_capture_nosrc_mixer,
		.init_verbs = { alc268_base_init_verbs, alc268_eapd_verbs,
				alc267_quanta_il1_verbs },
		.num_dacs = ARRAY_SIZE(alc268_dac_nids),
		.dac_nids = alc268_dac_nids,
		.num_adc_nids = ARRAY_SIZE(alc268_adc_nids_alt),
		.adc_nids = alc268_adc_nids_alt,
		.hp_nid = 0x03,
		.num_channel_mode = ARRAY_SIZE(alc268_modes),
		.channel_mode = alc268_modes,
		.unsol_event = alc_sku_unsol_event,
		.setup = alc267_quanta_il1_setup,
		.init_hook = alc_inithook,
	},
	[ALC268_3ST] = {
		.mixers = { alc268_base_mixer, alc268_beep_mixer },
		.cap_mixer = alc268_capture_alt_mixer,
		.init_verbs = { alc268_base_init_verbs },
		.num_dacs = ARRAY_SIZE(alc268_dac_nids),
		.dac_nids = alc268_dac_nids,
                .num_adc_nids = ARRAY_SIZE(alc268_adc_nids_alt),
                .adc_nids = alc268_adc_nids_alt,
		.capsrc_nids = alc268_capsrc_nids,
		.hp_nid = 0x03,
		.dig_out_nid = ALC268_DIGOUT_NID,
		.num_channel_mode = ARRAY_SIZE(alc268_modes),
		.channel_mode = alc268_modes,
		.input_mux = &alc268_capture_source,
	},
#ifdef CONFIG_SND_DEBUG
	[ALC268_TEST] = {
		.mixers = { alc268_test_mixer },
		.cap_mixer = alc268_capture_mixer,
		.init_verbs = { alc268_base_init_verbs, alc268_eapd_verbs,
				alc268_volume_init_verbs,
				alc268_beep_init_verbs },
		.num_dacs = ARRAY_SIZE(alc268_dac_nids),
		.dac_nids = alc268_dac_nids,
		.num_adc_nids = ARRAY_SIZE(alc268_adc_nids_alt),
		.adc_nids = alc268_adc_nids_alt,
		.capsrc_nids = alc268_capsrc_nids,
		.hp_nid = 0x03,
		.dig_out_nid = ALC268_DIGOUT_NID,
		.num_channel_mode = ARRAY_SIZE(alc268_modes),
		.channel_mode = alc268_modes,
		.input_mux = &alc268_capture_source,
	},
#endif
};

