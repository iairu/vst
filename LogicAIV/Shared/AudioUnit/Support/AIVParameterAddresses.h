#pragma once

#include <AudioToolbox/AudioToolbox.h>

#ifdef __cplusplus
namespace AIVParameterAddress {
#endif

enum {
  gain = 0,
  bypass = 1,
  pitchAmount = 2,
  pitchSpeed = 3,

  eqBand1Freq = 4,
  eqBand1Gain = 5,
  eqBand1Q = 6,
  eqBand2Freq = 7,
  eqBand2Gain = 8,
  eqBand2Q = 9,
  eqBand3Freq = 10,
  eqBand3Gain = 11,
  eqBand3Q = 12,

  compThresh = 13,
  compRatio = 14,
  compAttack = 15,
  compRelease = 16,
  compMakeup = 17,

  satDrive = 18,
  satType = 19,

  delayTime = 20,
  delayFeedback = 21,
  delayMix = 22,

  reverbSize = 23,
  reverbDamp = 24,
  reverbMix = 25
};

#ifdef __cplusplus
}
#endif
