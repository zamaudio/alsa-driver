#ifndef SOUND_FIREWIRE_AMDTP_H_INCLUDED
#define SOUND_FIREWIRE_AMDTP_H_INCLUDED

#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include "packets-buffer.h"

/**
 * enum cip_flags - describes details of the streaming protocol
 * @CIP_NONBLOCKING: In non-blocking mode, each packet contains
 *	sample_rate/8000 samples, with rounding up or down to adjust
 *	for clock skew and left-over fractional samples.  This should
 *	be used if supported by the device.
 */
enum cip_flags {
	CIP_NONBLOCKING = 0x00,
	CIP_BLOCKING	= 0x01,
};

/**
 * enum cip_sfc - a stream's sample rate
 */
enum cip_sfc {
	CIP_SFC_32000  = 0,
	CIP_SFC_44100  = 1,
	CIP_SFC_48000  = 2,
	CIP_SFC_88200  = 3,
	CIP_SFC_96000  = 4,
	CIP_SFC_176400 = 5,
	CIP_SFC_192000 = 6,
};

#define AMDTP_OUT_PCM_FORMAT_BITS	(SNDRV_PCM_FMTBIT_S16 | \
					 SNDRV_PCM_FMTBIT_S32)
/*
 * This module support maximum 8 MIDI streams
 * This is not in MMA/AMEI RP-027 but for our convinience.
 * Then AMDTP packets include maximum 2 quadlets in each data blocks.
 */
#define AMDTP_MAX_MIDI_STREAMS 16

struct fw_unit;
struct fw_iso_context;
struct snd_pcm_substream;

enum amdtp_stream_direction {
	AMDTP_STREAM_RECEIVE = 0,
	AMDTP_STREAM_TRANSMIT
};

enum amdtp_stream_sync_mode {
	AMDTP_STREAM_SYNC_TO_DRIVER,
	AMDTP_STREAM_SYNC_TO_DEVICE
};

struct amdtp_stream {
	struct fw_unit *unit;
	enum cip_flags flags;
	enum amdtp_stream_direction direction;
	struct fw_iso_context *context;
	struct mutex mutex;

	bool run;
	wait_queue_head_t run_wait;

	enum cip_sfc sfc;
	unsigned int data_block_quadlets;
	unsigned int pcm_channels;
	unsigned int midi_ports;
	void (*transfer_samples)(struct amdtp_stream *s,
				 struct snd_pcm_substream *pcm,
				 __be32 *buffer, unsigned int frames);

	unsigned int source_node_id_field;
	struct iso_packets_buffer buffer;
	unsigned int max_payload_size;

	struct snd_pcm_substream *pcm;
	struct tasklet_struct period_tasklet;

	int packet_index;
	unsigned int data_block_counter;

	unsigned int data_block_state;

	unsigned int last_syt_offset;
	unsigned int syt_offset_state;

	unsigned int pcm_buffer_pointer;
	unsigned int pcm_period_pointer;
	bool pointer_flush;

	struct snd_rawmidi_substream *midi[AMDTP_MAX_MIDI_STREAMS];
	unsigned long midi_triggered;	/* bit table for each MIDI substream */

	enum amdtp_stream_sync_mode sync_mode;
	struct amdtp_stream *sync_slave;
	
	bool use_digimagic;

	void *sort_table;
	void *left_packets;
	unsigned int remain_packets;
};

int amdtp_stream_init(struct amdtp_stream *s, struct fw_unit *unit,
		enum amdtp_stream_direction direction, enum cip_flags flags);
void amdtp_stream_destroy(struct amdtp_stream *s);

void amdtp_stream_set_rate(struct amdtp_stream *s, unsigned int rate);
unsigned int amdtp_stream_get_max_payload(struct amdtp_stream *s);

int amdtp_stream_start(struct amdtp_stream *s, int channel, int speed);
void amdtp_stream_update(struct amdtp_stream *s);
void amdtp_stream_stop(struct amdtp_stream *s);

void amdtp_stream_set_pcm_format(struct amdtp_stream *s,
				 snd_pcm_format_t format);
void amdtp_stream_pcm_prepare(struct amdtp_stream *s);
unsigned long amdtp_stream_pcm_pointer(struct amdtp_stream *s);
void amdtp_stream_pcm_abort(struct amdtp_stream *s);
bool amdtp_stream_wait_run(struct amdtp_stream *s);

void amdtp_stream_midi_add(struct amdtp_stream *s,
			   struct snd_rawmidi_substream *substream);
void amdtp_stream_midi_remove(struct amdtp_stream *s,
			      struct snd_rawmidi_substream *substream);
bool amdtp_stream_midi_running(struct amdtp_stream *s);

/**
 * amdtp_stream_set_pcm - configure format of PCM samples
 * @s: the AMDTP stream to be configured
 * @pcm_channels: the number of PCM samples in each data block, to be encoded
 *                as AM824 multi-bit linear audio
 *
 * This function must not be called while the stream is running.
 */
static inline void amdtp_stream_set_pcm(struct amdtp_stream *s,
					unsigned int pcm_channels)
{
	s->pcm_channels = pcm_channels;
}

/**
 * amdtp_stream_set_midi - configure format of MIDI data
 * @s: the AMDTP stream to be configured
 * @midi_ports: the number of MIDI ports (i.e., MPX-MIDI Data Channels)
 *
 * This function must not be called while the stream is running.
 */
static inline void amdtp_stream_set_midi(struct amdtp_stream *s,
					 unsigned int midi_ports)
{
	s->midi_ports = midi_ports;
}

/**
 * amdtp_stream_running - check stream is running or not
 * @s: the AMDTP stream
 *
 * If this function returns true, the stream is running.
 */
static inline bool amdtp_stream_running(struct amdtp_stream *s)
{
	return !IS_ERR(s->context);
}

/**
 * amdtp_streaming_error - check for streaming error
 * @s: the AMDTP stream
 *
 * If this function returns true, the stream's packet queue has stopped due to
 * an asynchronous error.
 */
static inline bool amdtp_streaming_error(struct amdtp_stream *s)
{
	return s->packet_index < 0;
}

/**
 * amdtp_stream_pcm_running - check PCM stream is running or not
 * @s: the AMDTP stream
 *
 * If this function returns true, PCM stream in the stream is running.
 */
static inline bool amdtp_stream_pcm_running(struct amdtp_stream *s)
{
	return !IS_ERR_OR_NULL(s->pcm);
}

/**
 * amdtp_stream_pcm_trigger - start/stop playback from a PCM device
 * @s: the AMDTP stream
 * @pcm: the PCM device to be started, or %NULL to stop the current device
 *
 * Call this function on a running isochronous stream to enable the actual
 * transmission of PCM data.  This function should be called from the PCM
 * device's .trigger callback.
 */
static inline void amdtp_stream_pcm_trigger(struct amdtp_stream *s,
					    struct snd_pcm_substream *pcm)
{
	ACCESS_ONCE(s->pcm) = pcm;
}

static inline bool cip_sfc_is_base_44100(enum cip_sfc sfc)
{
	return sfc & 1;
}

static inline void amdtp_stream_set_sync_mode(enum amdtp_stream_sync_mode mode,
					      struct amdtp_stream *master,
					      struct amdtp_stream *slave)
{
	master->sync_mode = mode;
	slave->sync_mode = mode;
	master->sync_slave = slave;
	slave->sync_slave = ERR_PTR(-1);
}

#endif
