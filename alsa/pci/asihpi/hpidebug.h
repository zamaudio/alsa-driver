/*****************************************************************************
Copyright (C) 1997-2003 AudioScience, Inc. All rights reserved.

This software is provided 'as-is', without any express or implied warranty.
In no event will AudioScience Inc. be held liable for any damages arising
from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This copyright notice and list of conditions may not be altered or removed 
   from any source distribution.

AudioScience, Inc. <support@audioscience.com>

( This license is GPL compatible see http://www.gnu.org/licenses/license-list.html#GPLCompatibleLicenses )

Debug macros.

*****************************************************************************/

#ifndef _HPIDEBUG_H
#define _HPIDEBUG_H

#include "hpi.h"
#include "hpios.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TEXT
#define TEXT(s) s
#endif

//#define HPI_DEBUG_VERBOSE

#ifdef HPI_DEBUG_VERBOSE
#  ifndef HPI_DEBUG /* DEBUG_VERBOSE implies DEBUG */
#   define HPI_DEBUG
#  endif
#endif

/* Define debugging levels.  */
enum {HPI_DEBUG_LEVEL_ERROR,   /* Always log errors */
      HPI_DEBUG_LEVEL_WARNING,
      HPI_DEBUG_LEVEL_NOTICE,
      HPI_DEBUG_LEVEL_INFO,
      HPI_DEBUG_LEVEL_DEBUG,
      HPI_DEBUG_LEVEL_VERBOSE /* Same printk level as DEBUG */
};

#ifndef HPI_DEBUG_LEVEL_DEFAULT
#define HPI_DEBUG_LEVEL_DEFAULT HPI_DEBUG_LEVEL_NOTICE
#endif

/* an OS can define an extra flag string that is appended to
   the start of each message, eg see hpios_linux.h */
#ifndef HPI_DEBUG_FLAG_ERROR
#define HPI_DEBUG_FLAG_ERROR
#define HPI_DEBUG_FLAG_WARNING
#define HPI_DEBUG_FLAG_NOTICE
#define HPI_DEBUG_FLAG_INFO
#define HPI_DEBUG_FLAG_DEBUG
#define HPI_DEBUG_FLAG_VERBOSE
#endif

/* OSes without printf-like function must provide plain string output
   and define HPIOS_DEBUG_PRINT to the name of this function.
   The printf functionality then provided by hpidebug.c::hpi_debug_printf
*/
#if defined(HPIOS_DEBUG_PRINT) && !defined(HPIOS_DEBUG_PRINTF)
#define HPIOS_DEBUG_PRINTF hpi_debug_printf
void hpi_debug_printf(char *fmt, ...);
#endif

#if !defined(HPIOS_DEBUG_PRINT) && !defined(HPIOS_DEBUG_PRINTF) && defined(HPI_DEBUG)
#undef HPI_DEBUG
#undef HPI_DEBUG_VERBOSE
#ifdef _MSC_VER
#pragma message "Warning: Can't debug with this build because no debug print functions defined"
#endif
#endif

/* HPI_DEBUG_VERBOSE causes file and line to be printed with each message */
#ifdef HPI_DEBUG_VERBOSE
# define FILE_LINE __FILE__ ":" __stringify(__LINE__) " "
#else
# define FILE_LINE "HPI: "
#endif

#ifdef HPI_DEBUG

/* Note VERBOSE and DEBUG messages don't get all the baggage of file and line etc
We don't expect users to use or understand verbose, it will generate huge outputs
*/
#define HPI_DEBUG_LOG0(level,fmt)										\
	do {																\
		if ( hpiDebugLevel >= HPI_DEBUG_LEVEL_##level ) {					\
			if ( HPI_DEBUG_LEVEL_##level >=  HPI_DEBUG_LEVEL_DEBUG )		\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level fmt);			\
			else														\
				HPIOS_DEBUG_PRINTF( HPI_DEBUG_FLAG_##level  FILE_LINE  __stringify(level)  " " fmt); \
		}																\
	} while (0)

#define HPI_DEBUG_LOG1(level,fmt,p1) 									\
	do {																\
		if (hpiDebugLevel >= HPI_DEBUG_LEVEL_##level) {					\
			if ( HPI_DEBUG_LEVEL_##level >=  HPI_DEBUG_LEVEL_DEBUG )		\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level fmt,p1);		\
			else														\
				HPIOS_DEBUG_PRINTF( HPI_DEBUG_FLAG_##level  FILE_LINE  __stringify(level)  " " fmt,p1);	\
		}																\
	} while (0)

#define HPI_DEBUG_LOG2(level,fmt,p1,p2)									\
	do {																\
		if (hpiDebugLevel>=HPI_DEBUG_LEVEL_##level) {					\
			if (HPI_DEBUG_LEVEL_##level >=  HPI_DEBUG_LEVEL_DEBUG)		\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level fmt,p1,p2);	\
			else														\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level FILE_LINE __stringify(level) " " fmt,p1,p2); \
		}																\
	} while (0)

#define HPI_DEBUG_LOG3(level,fmt,p1,p2,p3) 							\
	do {																\
		if (hpiDebugLevel>=HPI_DEBUG_LEVEL_##level) {					\
			if (HPI_DEBUG_LEVEL_##level >=  HPI_DEBUG_LEVEL_DEBUG)		\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level fmt,p1,p2,p3); \
			else														\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level FILE_LINE __stringify(level) " " fmt,p1,p2,p3); \
		}																\
	} while (0)

#define HPI_DEBUG_LOG4(level,fmt,p1,p2,p3,p4)							\
	do {																\
		if (hpiDebugLevel>=HPI_DEBUG_LEVEL_##level) {					\
			if (HPI_DEBUG_LEVEL_##level >=  HPI_DEBUG_LEVEL_INFO)		\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level fmt,p1,p2,p3,p4); \
			else														\
				HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_##level FILE_LINE __stringify(level) " " fmt,p1,p2,p3,p4); \
		}																\
	} while (0)

#define HPIOS_DEBUG_STRING(s) HPI_DEBUG_LOG0(VERBOSE,s "\n")

#else
#define HPI_DEBUG_LOG0(level,fmt)
#define HPI_DEBUG_LOG1(level,fmt,p1)
#define HPI_DEBUG_LOG2(level,fmt,p1,p2)
#define HPI_DEBUG_LOG3(level,fmt,p1,p2,p3)
#define HPI_DEBUG_LOG4(level,fmt,p1,p2,p3,p4)
#define HPIOS_DEBUG_STRING(s)
#endif

void HPI_DebugInit(void);
int HPI_DebugLevelSet(int level);
int HPI_DebugLevelGet(void);
extern int hpiDebugLevel;	// needed by Linux driver for dynamic debug level changes

void hpi_debug_message(HPI_MESSAGE *phm);
void hpi_debug_response(HPI_RESPONSE *phr);

#ifdef HPI_OS_LINUX
extern void
hpi_debug_data(HW16 *pdata, HW32 len);
#else
extern void
hpi_debug_data(HW16 HUGE *pdata, HW32 len);
#endif

#ifndef HPI_DEBUG_DATA
#define HPI_DEBUG_DATA(pdata,len)										\
	do {																\
		if (hpiDebugLevel >= HPI_DEBUG_LEVEL_VERBOSE) hpi_debug_data(pdata,len); \
	} while (0)
#endif

#ifndef HPI_DEBUG_MESSAGE
#define HPI_DEBUG_MESSAGE(phm)									\
	do {														\
		if (hpiDebugLevel >= HPI_DEBUG_LEVEL_DEBUG) {			\
			HPIOS_DEBUG_PRINTF(HPI_DEBUG_FLAG_DEBUG FILE_LINE);	\
			hpi_debug_message(phm);								\
		}														\
	} while (0)
#endif


#ifndef HPI_DEBUG_RESPONSE
#define HPI_DEBUG_RESPONSE(phr)									\
	do {														\
		if ((hpiDebugLevel >= HPI_DEBUG_LEVEL_DEBUG) && (phr->wError))	\
			HPI_DEBUG_LOG1(ERROR,"HPI %d\n", phr->wError); \
		else if (hpiDebugLevel >= HPI_DEBUG_LEVEL_VERBOSE) \
			HPI_DEBUG_LOG0(VERBOSE,"OK\n"); \
	} while (0)
#endif

/* Be careful with these macros.  Ensure that they are used within a block.
   Otherwise the second if might have an else after it... */
#define HPI_PRINT_ERROR \
HPIOS_DEBUG_PRINTF("%s,%d ",__FILE__,__LINE__); \
HPIOS_DEBUG_PRINTF

#define HPI_PRINT_INFO \
if (hpiDebugLevel >= HPI_DEBUG_LEVEL_INFO) \
    HPIOS_DEBUG_PRINTF("%s,%d: ",__FILE__,__LINE__); \
if (hpiDebugLevel >= HPI_DEBUG_LEVEL_INFO) \
    HPIOS_DEBUG_PRINTF

#define HPI_PRINT_DEBUG \
if (hpiDebugLevel >= HPI_DEBUG_LEVEL_DEBUG) \
    HPIOS_DEBUG_PRINTF( "%s,%d: ",__FILE__,__LINE__); \
if (hpiDebugLevel >= HPI_DEBUG_LEVEL_DEBUG) \
    HPIOS_DEBUG_PRINTF

#define HPI_PRINT_VERBOSE \
if (hpiDebugLevel >= HPI_DEBUG_LEVEL_VERBOSE) \
    HPIOS_DEBUG_PRINTF("%s,%d: ",__FILE__,__LINE__); \
if (hpiDebugLevel >= HPI_DEBUG_LEVEL_VERBOSE) \
    HPIOS_DEBUG_PRINTF

#if !defined(HPI_DEBUG)
#undef HPI_DEBUG_DATA
#undef HPI_DEBUG_MESSAGE
#undef HPI_DEBUG_RESPONSE
#undef HPIOS_DEBUG_PRINTF
#undef HPI_PRINT_ERROR
#undef HPI_PRINT_INFO
#undef HPI_PRINT_DEBUG
#undef HPI_PRINT_VERBOSE

#define HPI_DEBUG_DATA
#define HPI_DEBUG_MESSAGE
#define HPI_DEBUG_RESPONSE
#define HPIOS_DEBUG_PRINTF
#define HPI_PRINT_ERROR
#define HPI_PRINT_INFO
#define HPI_PRINT_DEBUG
#define HPI_PRINT_VERBOSE


#endif

/* These strings should be generated using a macro which defines
   the corresponding symbol values.  */
#define HPI_OBJ_STRINGS		\
{				\
  TEXT("HPI_OBJ_SUBSYSTEM"),		\
  TEXT("HPI_OBJ_ADAPTER"),		\
  TEXT("HPI_OBJ_OSTREAM"),		\
  TEXT("HPI_OBJ_ISTREAM"),		\
  TEXT("HPI_OBJ_MIXER"),		\
  TEXT("HPI_OBJ_NODE"),		\
  TEXT("HPI_OBJ_CONTROL"),		\
  TEXT("HPI_OBJ_NVMEMORY"),		\
  TEXT("HPI_OBJ_DIGITALIO"),		\
  TEXT("HPI_OBJ_WATCHDOG"),		\
  TEXT("HPI_OBJ_CLOCK"),		\
  TEXT("HPI_OBJ_PROFILE"),		\
  TEXT("HPI_OBJ_CONTROLEX")		\
};


#define HPI_SUBSYS_STRINGS	\
{				\
  TEXT("HPI_SUBSYS_OPEN"),		\
  TEXT("HPI_SUBSYS_GET_VERSION"),	\
  TEXT("HPI_SUBSYS_GET_INFO"),	\
  TEXT("HPI_SUBSYS_FIND_ADAPTERS"),	\
  TEXT("HPI_SUBSYS_CREATE_ADAPTER"),	\
  TEXT("HPI_SUBSYS_CLOSE"),		\
  TEXT("HPI_SUBSYS_DELETE_ADAPTER"), \
  TEXT("HPI_SUBSYS_DRIVER_LOAD"), \
  TEXT("HPI_SUBSYS_DRIVER_UNLOAD"), \
  TEXT("HPI_SUBSYS_READ_PORT_8"),	\
  TEXT("HPI_SUBSYS_WRITE_PORT_8")	\
};


#define HPI_ADAPTER_STRINGS	\
{				\
  TEXT("HPI_ADAPTER_OPEN"),		\
  TEXT("HPI_ADAPTER_CLOSE"),		\
  TEXT("HPI_ADAPTER_GET_INFO"),	\
  TEXT("HPI_ADAPTER_GET_ASSERT"),	\
  TEXT("HPI_ADAPTER_TEST_ASSERT"),    \
  TEXT("HPI_ADAPTER_SET_MODE"),       \
  TEXT("HPI_ADAPTER_GET_MODE"),       \
  TEXT("HPI_ADAPTER_ENABLE_CAPABILITY"),\
  TEXT("HPI_ADAPTER_SELFTEST"),        \
  TEXT("HPI_ADAPTER_FIND_OBJECT"),     \
  TEXT("HPI_ADAPTER_QUERY_FLASH"),     \
  TEXT("HPI_ADAPTER_START_FLASH"),     \
  TEXT("HPI_ADAPTER_PROGRAM_FLASH"),   \
  TEXT("HPI_ADAPTER_SET_PROPERTY"),    \
  TEXT("HPI_ADAPTER_GET_PROPERTY"),    \
  TEXT("HPI_ADAPTER_ENUM_PROPERTY")    \
};

#define HPI_OSTREAM_STRINGS	\
{				\
  TEXT("HPI_OSTREAM_OPEN"),		\
  TEXT("HPI_OSTREAM_CLOSE"),		\
  TEXT("HPI_OSTREAM_WRITE"),		\
  TEXT("HPI_OSTREAM_START"),		\
  TEXT("HPI_OSTREAM_STOP"),		\
  TEXT("HPI_OSTREAM_RESET"),		\
  TEXT("HPI_OSTREAM_GET_INFO"),	\
  TEXT("HPI_OSTREAM_QUERY_FORMAT"),	\
  TEXT("HPI_OSTREAM_DATA"),		\
  TEXT("HPI_OSTREAM_SET_VELOCITY"),	\
  TEXT("HPI_OSTREAM_SET_PUNCHINOUT"), \
  TEXT("HPI_OSTREAM_SINEGEN"),        \
  TEXT("HPI_OSTREAM_ANC_RESET"),      \
  TEXT("HPI_OSTREAM_ANC_GET_INFO"),   \
  TEXT("HPI_OSTREAM_ANC_READ"),       \
  TEXT("HPI_OSTREAM_SET_TIMESCALE"),		\
  TEXT("HPI_OSTREAM_SET_FORMAT"), \
  TEXT("HPI_OSTREAM_HOSTBUFFER_ALLOC"), \
  TEXT("HPI_OSTREAM_HOSTBUFFER_FREE"), \
  TEXT("HPI_OSTREAM_GROUP_ADD"),\
  TEXT("HPI_OSTREAM_GROUP_GETMAP"), \
  TEXT("HPI_OSTREAM_GROUP_RESET"), \
};

#define HPI_ISTREAM_STRINGS	\
{				\
  TEXT("HPI_ISTREAM_OPEN"),		\
  TEXT("HPI_ISTREAM_CLOSE"),		\
  TEXT("HPI_ISTREAM_SET_FORMAT"),	\
  TEXT("HPI_ISTREAM_READ"),		\
  TEXT("HPI_ISTREAM_START"),		\
  TEXT("HPI_ISTREAM_STOP"),		\
  TEXT("HPI_ISTREAM_RESET"),		\
  TEXT("HPI_ISTREAM_GET_INFO"),	\
  TEXT("HPI_ISTREAM_QUERY_FORMAT"),	\
  TEXT("HPI_ISTREAM_ANC_RESET"),      \
  TEXT("HPI_ISTREAM_ANC_GET_INFO"),   \
  TEXT("HPI_ISTREAM_ANC_WRITE"),   \
  TEXT("HPI_ISTREAM_HOSTBUFFER_ALLOC"),	\
  TEXT("HPI_ISTREAM_HOSTBUFFER_FREE"), \
  TEXT("HPI_ISTREAM_GROUP_ADD"), \
  TEXT("HPI_ISTREAM_GROUP_GETMAP"), \
  TEXT("HPI_ISTREAM_GROUP_RESET"), \
};

#define HPI_MIXER_STRINGS	\
{				\
  TEXT("HPI_MIXER_OPEN"),		\
  TEXT("HPI_MIXER_CLOSE"),		\
  TEXT("HPI_MIXER_GET_INFO"),		\
  TEXT("HPI_MIXER_GET_NODE_INFO"),	\
  TEXT("HPI_MIXER_GET_CONTROL"),	\
  TEXT("HPI_MIXER_SET_CONNECTION"),	\
  TEXT("HPI_MIXER_GET_CONNECTIONS"),	\
  TEXT("HPI_MIXER_GET_CONTROL_BY_INDEX"),	\
  TEXT("HPI_MIXER_GET_CONTROL_ARRAY_BY_INDEX"),	\
  TEXT("HPI_MIXER_GET_CONTROL_MULTIPLE_VALUES"),	\
  TEXT("HPI_MIXER_STORE"),	\
};

#define HPI_CONTROL_STRINGS	\
{				\
  TEXT("HPI_CONTROL_GET_INFO"),	\
  TEXT("HPI_CONTROL_GET_STATE"),	\
  TEXT("HPI_CONTROL_SET_STATE")	\
};

#define HPI_NVMEMORY_STRINGS	\
{				\
  TEXT("HPI_NVMEMORY_OPEN"),		\
  TEXT("HPI_NVMEMORY_READ_BYTE"),	\
  TEXT("HPI_NVMEMORY_WRITE_BYTE")	\
};

#define HPI_DIGITALIO_STRINGS	\
{				\
  TEXT("HPI_GPIO_OPEN"),		\
  TEXT("HPI_GPIO_READ_BIT"),	\
  TEXT("HPI_GPIO_WRITE_BIT"),	\
  TEXT("HPI_GPIO_READ_ALL")\
};

#define HPI_WATCHDOG_STRINGS	\
{				\
  TEXT("HPI_WATCHDOG_OPEN"),		\
  TEXT("HPI_WATCHDOG_SET_TIME"),	\
  TEXT("HPI_WATCHDOG_PING")		\
};

#define HPI_CLOCK_STRINGS	\
{				\
  TEXT("HPI_CLOCK_OPEN"),		\
  TEXT("HPI_CLOCK_SET_TIME"),		\
  TEXT("HPI_CLOCK_GET_TIME")		\
};

#define HPI_PROFILE_STRINGS	\
{				\
  TEXT("HPI_PROFILE_OPEN_ALL"),	\
  TEXT("HPI_PROFILE_START_ALL"),	\
  TEXT("HPI_PROFILE_STOP_ALL"),	\
  TEXT("HPI_PROFILE_GET"),		\
  TEXT("HPI_PROFILE_GET_IDLECOUNT"),  \
  TEXT("HPI_PROFILE_GET_NAME"),       \
  TEXT("HPI_PROFILE_GET_UTILIZATION") \
};

#define HPI_CONTROL_TYPE_STRINGS \
{ \
	TEXT("no control (0)"), \
	TEXT("HPI_CONTROL_CONNECTION"), \
	TEXT("HPI_CONTROL_VOLUME"), \
	TEXT("HPI_CONTROL_METER"), \
	TEXT("HPI_CONTROL_MUTE"), \
	TEXT("HPI_CONTROL_MULTIPLEXER"), \
	TEXT("HPI_CONTROL_AESEBU_TRANSMITTER"), \
	TEXT("HPI_CONTROL_AESEBU_RECEIVER"), \
	TEXT("HPI_CONTROL_LEVEL"), \
	TEXT("HPI_CONTROL_TUNER"), \
	TEXT("HPI_CONTROL_ONOFFSWITCH"), \
	TEXT("HPI_CONTROL_VOX"), \
	TEXT("HPI_CONTROL_AES18_TRANSMITTER"), \
	TEXT("HPI_CONTROL_AES18_RECEIVER"), \
	TEXT("HPI_CONTROL_AES18_BLOCKGENERATOR"), \
	TEXT("HPI_CONTROL_CHANNEL_MODE"), \
	TEXT("HPI_CONTROL_BITSTREAM"), \
	TEXT("HPI_CONTROL_SAMPLECLOCK"), \
	TEXT("HPI_CONTROL_MICROPHONE"), \
	TEXT("HPI_CONTROL_PARAMETRIC_EQ"), \
	TEXT("HPI_CONTROL_COMPANDER"), \
	TEXT("HPI_CONTROL_COBRANET"), \
	TEXT("HPI_CONTROL_TONE_DETECT") \
	TEXT("HPI_CONTROL_SILENCE_DETECT") \
}
#define NUM_CONTROL_STRINGS 24
#if (NUM_CONTROL_STRINGS != (HPI_CONTROL_LAST_INDEX+1))
#error TEXT("Control type strings don't match #defines")
#endif

#define HPI_SOURCENODE_STRINGS \
{ \
	TEXT("no source"), \
	TEXT("HPI_SOURCENODE_OSTREAM"), \
	TEXT("HPI_SOURCENODE_LINEIN"), \
	TEXT("HPI_SOURCENODE_AESEBU_IN"), \
	TEXT("HPI_SOURCENODE_TUNER"), \
	TEXT("HPI_SOURCENODE_RF"), \
	TEXT("HPI_SOURCENODE_CLOCK_SOURCE"), \
	TEXT("HPI_SOURCENODE_RAW_BITSTREAM"), \
	TEXT("HPI_SOURCENODE_MICROPHONE"), \
	TEXT("HPI_SOURCENODE_COBRANET") \
}
#define NUM_SOURCENODE_STRINGS 10
#if (NUM_SOURCENODE_STRINGS != (HPI_SOURCENODE_LAST_INDEX-HPI_SOURCENODE_BASE+1))
#error TEXT("Sourcenode strings don't match #defines")
#endif

#define HPI_DESTNODE_STRINGS \
{ \
	TEXT("no destination"), \
	TEXT("HPI_DESTNODE_ISTREAM"), \
	TEXT("HPI_DESTNODE_LINEOUT"), \
	TEXT("HPI_DESTNODE_AESEBU_OUT"), \
	TEXT("HPI_DESTNODE_RF"), \
	TEXT("HPI_DESTNODE_SPEAKER"), \
	TEXT("HPI_DESTNODE_COBRANET") \
}
#define NUM_DESTNODE_STRINGS 7
#if (NUM_DESTNODE_STRINGS != (HPI_DESTNODE_LAST_INDEX-HPI_DESTNODE_BASE+1))
#error "Destnode strings don't match #defines"
#endif

#define HPI_CONTROL_CHANNEL_MODE_STRINGS \
{ \
	TEXT("XXX HPI_CHANNEL_MODE_ERROR XXX"), \
	TEXT("HPI_CHANNEL_MODE_NORMAL"), \
	TEXT("HPI_CHANNEL_MODE_SWAP"), \
	TEXT("HPI_CHANNEL_MODE_LEFT_ONLY"), \
	TEXT("HPI_CHANNEL_MODE_RIGHT_ONLY") \
}

////////////////////////////////////////////////////////////////////////////////
#ifdef SGT_SERIAL_LOGGING
void HpiDebug_SerialLog_Init( void );
void HpiDebug_SerialLog_SendBuffer(char* pszBuffer);
#endif

#ifdef __cplusplus
}
#endif
#endif /* _HPIDEBUG_H  */

