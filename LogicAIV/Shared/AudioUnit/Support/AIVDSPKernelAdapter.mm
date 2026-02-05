/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Adapter object providing a Swift-accessible interface to the filter's underlying
DSP code.
*/

// 1. C++ Standard Library (must be first in Obj-C++ files sometimes)
#import <algorithm>
#import <vector>

// 2. System Frameworks
#import <AVFoundation/AVFoundation.h>
#import <CoreAudioKit/AUViewController.h>

// 3. Project Headers (Obj-C)
#import "AIVDSPKernelAdapter.h"
#import "BufferedAudioBus.hpp"

// 4. Project Headers (C++)
#import "AIVDSPKernel.hpp"

@implementation AIVDSPKernelAdapter {
  // C++ members need to be ivars; they would be copied on access if they were
  // properties.
  AIVDSPKernel _kernel;
  BufferedInputBus _inputBus;
}

- (instancetype)init {

  if (self = [super init]) {
    AVAudioFormat *format =
        [[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100
                                                       channels:2];
    // Create a DSP kernel to handle the signal processing.
    _kernel.initialize(format.channelCount, format.channelCount,
                       format.sampleRate);
    _kernel.setParameter(AIVParameterAddressGain, 0.5);

    // Create the input and output busses.
    _inputBus.init(format, 8);
    _outputBus = [[AUAudioUnitBus alloc] initWithFormat:format error:nil];
  }
  return self;
}

- (AUAudioUnitBus *)inputBus {
  return _inputBus.bus;
}

- (NSArray<NSNumber *> *)magnitudesForFrequencies:
    (NSArray<NSNumber *> *)frequencies {
  // Legacy visualization support - specific to the old EQ.
  // For now we will return empty or simple passthrough, as visualizer needs to
  // be rewritten for multi-band. Ideally we would query the specific EQ bands
  // from the kernel if accessible.

  // Placeholder to prevent crash but logic needs update for 3-band EQ
  NSMutableArray<NSNumber *> *magnitudes =
      [NSMutableArray arrayWithCapacity:frequencies.count];
  for (NSNumber *number in frequencies) {
    // Return unity gain (1.0) for now as visualizer connection is TODO
    [magnitudes addObject:@(1.0)];
  }
  return [NSArray arrayWithArray:magnitudes];
}

- (void)setParameter:(AUParameter *)parameter value:(AUValue)value {
  _kernel.setParameter(parameter.address, value);
}

- (AUValue)valueForParameter:(AUParameter *)parameter {
  return _kernel.getParameter(parameter.address);
}

- (AUAudioFrameCount)maximumFramesToRender {
  return _kernel.maximumFramesToRender();
}

- (void)setMaximumFramesToRender:(AUAudioFrameCount)maximumFramesToRender {
  _kernel.setMaximumFramesToRender(maximumFramesToRender);
}

- (void)allocateRenderResources {
  _inputBus.allocateRenderResources(self.maximumFramesToRender);
  _kernel.initialize(self.outputBus.format.channelCount,
                     self.outputBus.format.channelCount,
                     self.outputBus.format.sampleRate);
}

- (void)deallocateRenderResources {
  _inputBus.deallocateRenderResources();
}

#pragma mark - AUAudioUnit (AUAudioUnitImplementation)

// Subclassers must provide a AUInternalRenderBlock (via a getter) to implement
// rendering.
- (AUInternalRenderBlock)internalRenderBlock {
  /*
   Capture in locals to avoid ObjC member lookups. If "self" is captured in
   render, we're doing it wrong.
   */
  // Specify captured objects are mutable.
  __block AIVDSPKernel *state = &_kernel;
  __block BufferedInputBus *input = &_inputBus;

  return ^AUAudioUnitStatus(
      AudioUnitRenderActionFlags *actionFlags, const AudioTimeStamp *timestamp,
      AVAudioFrameCount frameCount, NSInteger outputBusNumber,
      AudioBufferList *outputData, const AURenderEvent *realtimeEventListHead,
      AURenderPullInputBlock pullInputBlock) {
    AudioUnitRenderActionFlags pullFlags = 0;

    if (frameCount > state->maximumFramesToRender()) {
      return kAudioUnitErr_TooManyFramesToProcess;
    }

    AUAudioUnitStatus err =
        input->pullInput(&pullFlags, timestamp, frameCount, 0, pullInputBlock);

    if (err != 0) {
      return err;
    }

    AudioBufferList *inAudioBufferList = input->mutableAudioBufferList;

    AudioBufferList *outAudioBufferList = outputData;
    if (outAudioBufferList->mBuffers[0].mData == nullptr) {
      for (UInt32 i = 0; i < outAudioBufferList->mNumberBuffers; ++i) {
        outAudioBufferList->mBuffers[i].mData =
            inAudioBufferList->mBuffers[i].mData;
      }
    }

    // Process Events
    const AURenderEvent *event = realtimeEventListHead;
    while (event != nullptr) {
      state->handleOneEvent(timestamp->mSampleTime, event);
      event = event->head.next;
    }

    // Bridge AudioBufferList to std::vector or raw pointers for C++ kernel
    int inputChannelCount =
        inAudioBufferList
            ->mNumberBuffers; // Should be 2 (Planar) via BufferedBus
    int outputBufferCount = outAudioBufferList->mNumberBuffers;

    float *inputChannels[inputChannelCount];
    float *outputChannels[2]; // Max 2 channels for this plugin

    for (int i = 0; i < inputChannelCount; ++i) {
      inputChannels[i] = (float *)inAudioBufferList->mBuffers[i].mData;
    }

    bool outputIsInterleaved =
        (outputBufferCount == 1 &&
         outAudioBufferList->mBuffers[0].mNumberChannels == 2);

    // Assign Output Pointers
    if (outputIsInterleaved) {
      // Use scratch planar buffers from Kernel
      outputChannels[0] = state->getScratchPointer(0);
      outputChannels[1] = state->getScratchPointer(1);
    } else {
      // Standard Planar
      for (int i = 0; i < outputBufferCount; ++i) {
        outputChannels[i] = (float *)outAudioBufferList->mBuffers[i].mData;
      }
    }

    // Process (Planar inputs -> Planar outputs (scratch or direct))
    state->process(inputChannels, outputChannels, timestamp->mSampleTime,
                   frameCount,
                   inputChannelCount); // Use input channel count (2)

    // Handle Interleaved Output (mix planar scratch back to interleaved output)
    if (outputIsInterleaved) {
      float *interleavedOut = (float *)outAudioBufferList->mBuffers[0].mData;
      float *l = outputChannels[0];
      float *r = outputChannels[1];

      for (int i = 0; i < frameCount; ++i) {
        interleavedOut[2 * i] = l[i];
        interleavedOut[2 * i + 1] = r[i];
      }
    }

    return noErr;
  };
}

@end
