/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Adapter object providing a Swift-accessible interface to the filter's underlying
DSP code.
*/

#import <AudioToolbox/AudioToolbox.h>

typedef NS_ENUM(AUParameterAddress, AIVParameterAddress) {
  AIVParameterAddressGain = 0,
  AIVParameterAddressBypass = 1,
  AIVParameterAddressPitchAmount = 2,
  AIVParameterAddressPitchSpeed = 3,

  AIVParameterAddressEQBand1Freq = 4,
  AIVParameterAddressEQBand1Gain = 5,
  AIVParameterAddressEQBand1Q = 6,
  AIVParameterAddressEQBand2Freq = 7,
  AIVParameterAddressEQBand2Gain = 8,
  AIVParameterAddressEQBand2Q = 9,
  AIVParameterAddressEQBand3Freq = 10,
  AIVParameterAddressEQBand3Gain = 11,
  AIVParameterAddressEQBand3Q = 12,

  AIVParameterAddressCompThresh = 13,
  AIVParameterAddressCompRatio = 14,
  AIVParameterAddressCompAttack = 15,
  AIVParameterAddressCompRelease = 16,
  AIVParameterAddressCompMakeup = 17,

  AIVParameterAddressSatDrive = 18,
  AIVParameterAddressSatType = 19,

  AIVParameterAddressDelayTime = 20,
  AIVParameterAddressDelayFeedback = 21,
  AIVParameterAddressDelayMix = 22,

  AIVParameterAddressReverbSize = 23,
  AIVParameterAddressReverbDamp = 24,
  AIVParameterAddressReverbMix = 25,

  AIVParameterAddressAutoLevelTarget = 26,
  AIVParameterAddressAutoLevelRange = 27,
  AIVParameterAddressAutoLevelSpeed = 28,

  AIVParameterAddressDeesserThresh = 29,
  AIVParameterAddressDeesserFreq = 30,
  AIVParameterAddressDeesserRatio = 31,
  AIVParameterAddressDeesserRange = 32,

  // Gate
  AIVParameterAddressGateThresh = 40,
  AIVParameterAddressGateRange = 41,
  AIVParameterAddressGateAttack = 42,
  AIVParameterAddressGateHold = 43,
  AIVParameterAddressGateRelease = 44,
  AIVParameterAddressGateHysteresis = 45,

  AIVParameterAddressCutoff = 50,
  AIVParameterAddressResonance = 51,

  AIVParameterAddressInputGain = 34,
  AIVParameterAddressSaturation = 35,
  AIVParameterAddressPhaseInvert = 36
};

@class AIVDemoViewController;

NS_ASSUME_NONNULL_BEGIN

#ifdef __OBJC__
@interface AIVDSPKernelAdapter : NSObject

@property(nonatomic) AUAudioFrameCount maximumFramesToRender;
@property(nonatomic, readonly) AUAudioUnitBus *inputBus;
@property(nonatomic, readonly) AUAudioUnitBus *outputBus;

- (void)setParameter:(AUParameter *)parameter value:(AUValue)value;
- (AUValue)valueForParameter:(AUParameter *)parameter;

- (void)allocateRenderResources;
- (void)deallocateRenderResources;
- (AUInternalRenderBlock)internalRenderBlock;

- (NSArray<NSNumber *> *)magnitudesForFrequencies:
    (NSArray<NSNumber *> *)frequencies;

@end
#endif

NS_ASSUME_NONNULL_END
