dnl ALSA soundcard-configuration
dnl Find out which cards to compile driver for
dnl Copyright (c) by Anders Semb Hermansen <ahermans@vf.telia.no>

AC_DEFUN(ALSA_CARDS_INIT, [
	CONFIG_SND_INTERWAVE="0"
	CONFIG_SND_INTERWAVE_STB="0"
	CONFIG_SND_GUSMAX="0"
	CONFIG_SND_GUSEXTREME="0"
	CONFIG_SND_GUSCLASSIC="0"
	CONFIG_SND_AUDIODRIVE1688="0"
	CONFIG_SND_AUDIODRIVE18XX="0"
	CONFIG_SND_SB8="0"
	CONFIG_SND_SB16="0"
	CONFIG_SND_SBAWE="0"
	CONFIG_SND_OPL3SA="0"
	CONFIG_SND_MOZART="0"
	CONFIG_SND_SONICVIBES="0"
	CONFIG_SND_AUDIOPCI="0"
	CONFIG_SND_CARD_AD1848="0"
	CONFIG_SND_CARD_CS4231="0"
	CONFIG_SND_CARD_CS4232="0"
	CONFIG_SND_CARD_CS4236="0"
	CONFIG_SND_ESSSOLO1="0"
	CONFIG_SND_OPTI9XX="0"
	CONFIG_SND_SERIAL="0"
	CONFIG_SND_GUS="0"
	CONFIG_SND_CS4231="0"
	CONFIG_SND_TEA6330T="0"
	CONFIG_SND_ES1688="0"
	CONFIG_SND_ES18XX="0"
	CONFIG_SND_MPU401_UART="0"
	CONFIG_SND_OPL3="0"
	CONFIG_SND_SB_DSP="0"
	CONFIG_SND_EMU8000="0"
	CONFIG_SND_AD1848="0"
	CONFIG_SND_S3_86C617="0"
	CONFIG_SND_ENS1370="0"
	CONFIG_SND_AC97_CODEC="0"
	CONFIG_SND_CS4236="0"
	CONFIG_SND_ES1938="0"
	CONFIG_SND_UART16550="0"
])

AC_DEFUN(ALSA_CARDS_ALL, [
	CONFIG_SND_INTERWAVE="1"
	AC_DEFINE(CONFIG_SND_INTERWAVE)
	CONFIG_SND_INTERWAVE_STB="1"
	AC_DEFINE(CONFIG_SND_INTERWAVE_STB)
	CONFIG_SND_GUSMAX="1"
	AC_DEFINE(CONFIG_SND_GUSMAX)
	CONFIG_SND_GUSEXTREME="1"
	AC_DEFINE(CONFIG_SND_GUSEXTREME)
	CONFIG_SND_GUSCLASSIC="1"
	AC_DEFINE(CONFIG_SND_GUSCLASSIC)
	CONFIG_SND_AUDIODRIVE1688="1"
	AC_DEFINE(CONFIG_SND_AUDIODRIVE1688)
	CONFIG_SND_AUDIODRIVE18XX="1"
	AC_DEFINE(CONFIG_SND_AUDIODRIVE18XX)
	CONFIG_SND_SB8="1"
	AC_DEFINE(CONFIG_SND_SB8)
	CONFIG_SND_SB16="1"
	AC_DEFINE(CONFIG_SND_SB16)
	CONFIG_SND_SBAWE="1"
	AC_DEFINE(CONFIG_SND_SBAWE)
	CONFIG_SND_OPL3SA="1"
	AC_DEFINE(CONFIG_SND_OPL3SA)
	CONFIG_SND_MOZART="1"
	AC_DEFINE(CONFIG_SND_MOZART)
	CONFIG_SND_SONICVIBES="1"
	AC_DEFINE(CONFIG_SND_SONICVIBES)
	CONFIG_SND_AUDIOPCI="1"
	AC_DEFINE(CONFIG_SND_AUDIOPCI)
	CONFIG_SND_CARD_AD1848="1"
	AC_DEFINE(CONFIG_SND_CARD_AD1848)
	CONFIG_SND_CARD_CS4231="1"
	AC_DEFINE(CONFIG_SND_CARD_CS4231)
	CONFIG_SND_CARD_CS4232="1"
	AC_DEFINE(CONFIG_SND_CARD_CS4232)
	CONFIG_SND_CARD_CS4236="1"
	AC_DEFINE(CONFIG_SND_CARD_CS4236)
	CONFIG_SND_ESSSOLO1="1"
	AC_DEFINE(CONFIG_SND_ESSSOLO1)
	CONFIG_SND_OPTI9XX="1"
	AC_DEFINE(CONFIG_SND_OPTI9XX)
	CONFIG_SND_SERIAL="1"
	AC_DEFINE(CONFIG_SND_SERIAL)
	CONFIG_SND_GUS="1"
	AC_DEFINE(CONFIG_SND_GUS)
	CONFIG_SND_CS4231="1"
	AC_DEFINE(CONFIG_SND_CS4231)
	CONFIG_SND_TEA6330T="1"
	AC_DEFINE(CONFIG_SND_TEA6330T)
	CONFIG_SND_ES1688="1"
	AC_DEFINE(CONFIG_SND_ES1688)
	CONFIG_SND_ES18XX="1"
	AC_DEFINE(CONFIG_SND_ES18XX)
	CONFIG_SND_MPU401_UART="1"
	AC_DEFINE(CONFIG_SND_MPU401_UART)
	CONFIG_SND_OPL3="1"
	AC_DEFINE(CONFIG_SND_OPL3)
	CONFIG_SND_SB_DSP="1"
	AC_DEFINE(CONFIG_SND_SB_DSP)
	CONFIG_SND_EMU8000="1"
	AC_DEFINE(CONFIG_SND_EMU8000)
	CONFIG_SND_AD1848="1"
	AC_DEFINE(CONFIG_SND_AD1848)
	CONFIG_SND_S3_86C617="1"
	AC_DEFINE(CONFIG_SND_S3_86C617)
	CONFIG_SND_ENS1370="1"
	AC_DEFINE(CONFIG_SND_ENS1370)
	CONFIG_SND_AC97_CODEC="1"
	AC_DEFINE(CONFIG_SND_AC97_CODEC)
	CONFIG_SND_CS4236="1"
	AC_DEFINE(CONFIG_SND_CS4236)
	CONFIG_SND_ES1938="1"
	AC_DEFINE(CONFIG_SND_ES1938)
	CONFIG_SND_UART16550="1"
	AC_DEFINE(CONFIG_SND_UART16550)
])

AC_DEFUN(ALSA_CARDS_SELECT, [
dnl Check for which cards to compile driver for...
AC_MSG_CHECKING(for which soundcards to compile driver for)
AC_ARG_WITH(cards,
  [  --with-cards=<list>     compile driver for cards in <list>. ]
  [                        cards may be separated with commas. ]
  [                        "all" compiles all drivers ],
  cards="$withval", cards="all")
if test "$cards" = "all"; then
  ALSA_CARDS_ALL
  AC_MSG_RESULT(all)
else
  cards=`echo $cards | sed 's/,/ /g'`
  for card in $cards
  do
    case "$card" in
	interwave)
		CONFIG_SND_INTERWAVE="1"
		AC_DEFINE(CONFIG_SND_INTERWAVE)
		CONFIG_SND_GUS="1"
		AC_DEFINE(CONFIG_SND_GUS)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		;;
	interwave-stb)
		CONFIG_SND_INTERWAVE_STB="1"
		AC_DEFINE(CONFIG_SND_INTERWAVE_STB)
		CONFIG_SND_GUS="1"
		AC_DEFINE(CONFIG_SND_GUS)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		CONFIG_SND_TEA6330T="1"
		AC_DEFINE(CONFIG_SND_TEA6330T)
		;;
	gusmax)
		CONFIG_SND_GUSMAX="1"
		AC_DEFINE(CONFIG_SND_GUSMAX)
		CONFIG_SND_GUS="1"
		AC_DEFINE(CONFIG_SND_GUS)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		;;
	gusextreme)
		CONFIG_SND_GUSEXTREME="1"
		AC_DEFINE(CONFIG_SND_GUSEXTREME)
		CONFIG_SND_GUS="1"
		AC_DEFINE(CONFIG_SND_GUS)
		CONFIG_SND_ES1688="1"
		AC_DEFINE(CONFIG_SND_ES1688)
		;;
	gusclassic)
		CONFIG_SND_GUSCLASSIC="1"
		AC_DEFINE(CONFIG_SND_GUSCLASSIC)
		CONFIG_SND_GUS="1"
		AC_DEFINE(CONFIG_SND_GUS)
		;;
	audiodrive1688)
		CONFIG_SND_AUDIODRIVE1688="1"
		AC_DEFINE(CONFIG_SND_AUDIODRIVE1688)
		CONFIG_SND_ES1688="1"
		AC_DEFINE(CONFIG_SND_ES1688)
		;;
	audiodrive18xx)
		CONFIG_SND_AUDIODRIVE18XX="1"
		AC_DEFINE(CONFIG_SND_AUDIODRIVE18XX)
		CONFIG_SND_ES18XX="1"
		AC_DEFINE(CONFIG_SND_ES18XX)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	sb8)
		CONFIG_SND_SB8="1"
		AC_DEFINE(CONFIG_SND_SB8)
		CONFIG_SND_SB_DSP="1"
		AC_DEFINE(CONFIG_SND_SB_DSP)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	sb16)
		CONFIG_SND_SB16="1"
		AC_DEFINE(CONFIG_SND_SB16)
		CONFIG_SND_SB_DSP="1"
		AC_DEFINE(CONFIG_SND_SB_DSP)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	sbawe)
		CONFIG_SND_SBAWE="1"
		AC_DEFINE(CONFIG_SND_SBAWE)
		CONFIG_SND_SB_DSP="1"
		AC_DEFINE(CONFIG_SND_SB_DSP)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		CONFIG_SND_EMU8000="1"
		AC_DEFINE(CONFIG_SND_EMU8000)
		;;
	opl3sa)
		CONFIG_SND_OPL3SA="1"
		AC_DEFINE(CONFIG_SND_OPL3SA)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	mozart)
		CONFIG_SND_MOZART="1"
		AC_DEFINE(CONFIG_SND_MOZART)
		CONFIG_SND_AD1848="1"
		AC_DEFINE(CONFIG_SND_AD1848)
		;;
	sonicvibes)
		CONFIG_SND_SONICVIBES="1"
		AC_DEFINE(CONFIG_SND_SONICVIBES)
		CONFIG_SND_S3_86C617="1"
		AC_DEFINE(CONFIG_SND_S3_86C617)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	audiopci)
		CONFIG_SND_AUDIOPCI="1"
		AC_DEFINE(CONFIG_SND_AUDIOPCI)
		CONFIG_SND_ENS1370="1"
		AC_DEFINE(CONFIG_SND_ENS1370)
		CONFIG_SND_AC97_CODEC="1"
		AC_DEFINE(CONFIG_SND_AC97_CODEC)
		;;
	card-ad1848)
		CONFIG_SND_CARD_AD1848="1"
		AC_DEFINE(CONFIG_SND_CARD_AD1848)
		CONFIG_SND_AD1848="1"
		AC_DEFINE(CONFIG_SND_AD1848)
		;;
	card-cs4231)
		CONFIG_SND_CARD_CS4231="1"
		AC_DEFINE(CONFIG_SND_CARD_CS4231)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		;;
	card-cs4232)
		CONFIG_SND_CARD_CS4232="1"
		AC_DEFINE(CONFIG_SND_CARD_CS4232)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	card-cs4236)
		CONFIG_SND_CARD_CS4236="1"
		AC_DEFINE(CONFIG_SND_CARD_CS4236)
		CONFIG_SND_CS4236="1"
		AC_DEFINE(CONFIG_SND_CS4236)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	esssolo1)
		CONFIG_SND_ESSSOLO1="1"
		AC_DEFINE(CONFIG_SND_ESSSOLO1)
		CONFIG_SND_ES1938="1"
		AC_DEFINE(CONFIG_SND_ES1938)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	opti9xx)
		CONFIG_SND_OPTI9XX="1"
		AC_DEFINE(CONFIG_SND_OPTI9XX)
		CONFIG_SND_CS4231="1"
		AC_DEFINE(CONFIG_SND_CS4231)
		CONFIG_SND_AD1848="1"
		AC_DEFINE(CONFIG_SND_AD1848)
		CONFIG_SND_MPU401_UART="1"
		AC_DEFINE(CONFIG_SND_MPU401_UART)
		CONFIG_SND_OPL3="1"
		AC_DEFINE(CONFIG_SND_OPL3)
		;;
	serial)
		CONFIG_SND_SERIAL="1"
		AC_DEFINE(CONFIG_SND_SERIAL)
		CONFIG_SND_UART16550="1"
		AC_DEFINE(CONFIG_SND_UART16550)
		;;
	*)
		echo "Unknown soundcard $card, exiting!"
		exit 1
		;;
    esac
  done
  AC_MSG_RESULT($cards)
fi
AC_SUBST(CONFIG_SND_INTERWAVE)
AC_SUBST(CONFIG_SND_INTERWAVE_STB)
AC_SUBST(CONFIG_SND_GUSMAX)
AC_SUBST(CONFIG_SND_GUSEXTREME)
AC_SUBST(CONFIG_SND_GUSCLASSIC)
AC_SUBST(CONFIG_SND_AUDIODRIVE1688)
AC_SUBST(CONFIG_SND_AUDIODRIVE18XX)
AC_SUBST(CONFIG_SND_SB8)
AC_SUBST(CONFIG_SND_SB16)
AC_SUBST(CONFIG_SND_SBAWE)
AC_SUBST(CONFIG_SND_OPL3SA)
AC_SUBST(CONFIG_SND_MOZART)
AC_SUBST(CONFIG_SND_SONICVIBES)
AC_SUBST(CONFIG_SND_AUDIOPCI)
AC_SUBST(CONFIG_SND_CARD_AD1848)
AC_SUBST(CONFIG_SND_CARD_CS4231)
AC_SUBST(CONFIG_SND_CARD_CS4232)
AC_SUBST(CONFIG_SND_CARD_CS4236)
AC_SUBST(CONFIG_SND_ESSSOLO1)
AC_SUBST(CONFIG_SND_OPTI9XX)
AC_SUBST(CONFIG_SND_SERIAL)
AC_SUBST(CONFIG_SND_GUS)
AC_SUBST(CONFIG_SND_CS4231)
AC_SUBST(CONFIG_SND_TEA6330T)
AC_SUBST(CONFIG_SND_ES1688)
AC_SUBST(CONFIG_SND_ES18XX)
AC_SUBST(CONFIG_SND_MPU401_UART)
AC_SUBST(CONFIG_SND_OPL3)
AC_SUBST(CONFIG_SND_SB_DSP)
AC_SUBST(CONFIG_SND_EMU8000)
AC_SUBST(CONFIG_SND_AD1848)
AC_SUBST(CONFIG_SND_S3_86C617)
AC_SUBST(CONFIG_SND_ENS1370)
AC_SUBST(CONFIG_SND_AC97_CODEC)
AC_SUBST(CONFIG_SND_CS4236)
AC_SUBST(CONFIG_SND_ES1938)
AC_SUBST(CONFIG_SND_UART16550)
])
