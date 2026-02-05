We are working on an AudioUnit extension for Logic Pro using Xcode and Google Gemini to write the code. Currently it is a working Cutoff+Resonance Filter, however we would like to robustly extend this with industry leading effects that include the following:


Auto Level: It automatically equalizes the volume, so that it's not quiet and loud at times.


Pitch: It fine-tunes false notes to make it sound professional.


Deesser: We erase those sharp "S" and "C", we clean it up like a mess.


Compressor: The absolute basics, it makes the voice fat, hard and pulls it forward.


EQ and Saturation: Gives it color and that expensive shine.


Reverb: So that it doesn't sound dry, but it has space.


This is the current working Xcode AudioUnit project file structure:


.
./Configuration
./Configuration/SampleCode.xcconfig
./Documentation
./Documentation/graph.png
./macOS
./macOS/AIVFramework
./macOS/AIVFramework/AIVDemoViewController.xib
./macOS/AIVFramework/AIVFramework.h
./macOS/AIVFramework/Support
./macOS/AIVFramework/Support/NSStoryboardExtensions.swift
./macOS/AIVFramework/Support/NSTextFieldExtensions.swift
./macOS/AIVFramework/AIVDemoViewControllerExtension.swift
./macOS/AIVFramework/Info.plist
./macOS/AIVExtension
./macOS/AIVExtension/AIVExtension.entitlements
./macOS/AIVExtension/AIVExtension.swift
./macOS/AIVExtension/Info.plist
./macOS/AIV
./macOS/AIV/MainViewController.swift
./macOS/AIV/Assets.xcassets
./macOS/AIV/Assets.xcassets/AppIcon.appiconset
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-32.png
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-256.png
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-128.png
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-512.png
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/Contents.json
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-64.png
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-16.png
./macOS/AIV/Assets.xcassets/AppIcon.appiconset/macOS-1024.png
./macOS/AIV/Assets.xcassets/Contents.json
./macOS/AIV/Base.lproj
./macOS/AIV/Base.lproj/Main.storyboard
./macOS/AIV/AIV.entitlements
./macOS/AIV/AppDelegate.swift
./macOS/AIV/Info.plist
./.DS_Store
./LICENSE
./LICENSE/LICENSE.txt
./AIV.xcodeproj
./AIV.xcodeproj/project.pbxproj
./AIV.xcodeproj/.xcodesamplecode.plist
./AIV.xcodeproj/project.xcworkspace
./AIV.xcodeproj/project.xcworkspace/xcuserdata
./AIV.xcodeproj/project.xcworkspace/xcuserdata/iairu.xcuserdatad
./AIV.xcodeproj/project.xcworkspace/xcuserdata/iairu.xcuserdatad/.!68145!UserInterfaceState.xcuserstate
./AIV.xcodeproj/project.xcworkspace/xcuserdata/iairu.xcuserdatad/.!68277!UserInterfaceState.xcuserstate
./AIV.xcodeproj/project.xcworkspace/xcuserdata/iairu.xcuserdatad/UserInterfaceState.xcuserstate
./AIV.xcodeproj/project.xcworkspace/xcuserdata/v.xcuserdatad
./AIV.xcodeproj/project.xcworkspace/xcuserdata/v.xcuserdatad/UserInterfaceState.xcuserstate
./AIV.xcodeproj/project.xcworkspace/xcshareddata
./AIV.xcodeproj/project.xcworkspace/xcshareddata/IDEWorkspaceChecks.plist
./AIV.xcodeproj/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings
./AIV.xcodeproj/project.xcworkspace/xcshareddata/swiftpm
./AIV.xcodeproj/project.xcworkspace/xcshareddata/swiftpm/configuration
./AIV.xcodeproj/xcshareddata
./AIV.xcodeproj/xcshareddata/xcschemes
./AIV.xcodeproj/xcshareddata/xcschemes/AIV iOS.xcscheme
./AIV.xcodeproj/xcshareddata/xcschemes/AIVFramework macOS.xcscheme
./AIV.xcodeproj/xcshareddata/xcschemes/AIVExtension iOS.xcscheme
./AIV.xcodeproj/xcshareddata/xcschemes/AIVExtension macOS.xcscheme
./AIV.xcodeproj/xcshareddata/xcschemes/AIVFramework iOS.xcscheme
./AIV.xcodeproj/xcshareddata/xcschemes/AIV macOS.xcscheme
./Shared
./Shared/AIVDemoViewController.swift
./Shared/AudioUnitManager.swift
./Shared/Support
./Shared/Support/Audio
./Shared/Support/Audio/Synth.aif
./Shared/Support/Audio/SimplePlayEngine.swift
./Shared/Support/User Interface
./Shared/Support/User Interface/ViewExtensions.swift
./Shared/Support/User Interface/FilterView.swift
./Shared/Support/User Interface/TypeAliases.swift
./Shared/AudioUnit
./Shared/AudioUnit/AIVDemoParameters.swift
./Shared/AudioUnit/Support
./Shared/AudioUnit/Support/ParameterRamper.hpp
./Shared/AudioUnit/Support/DSPKernel.hpp
./Shared/AudioUnit/Support/DSPKernel.mm
./Shared/AudioUnit/Support/AIVDSPKernelAdapter.h
./Shared/AudioUnit/Support/AIVDSPClasses.hpp
./Shared/AudioUnit/Support/AIVDSPKernelAdapter.mm
./Shared/AudioUnit/Support/AIVDSPKernel.hpp
./Shared/AudioUnit/Support/BufferedAudioBus.hpp
./Shared/AudioUnit/AIVDemo.swift
./iOS
./iOS/AIVFramework
./iOS/AIVFramework/AIVFramework.h
./iOS/AIVFramework/Info.plist
./iOS/AIVExtension
./iOS/AIVExtension/Base.lproj
./iOS/AIVExtension/Base.lproj/MainInterface.storyboard
./iOS/AIVExtension/AIVDemoViewControllerExtension.swift
./iOS/AIVExtension/Info.plist
./iOS/AIV
./iOS/AIV/MainViewController.swift
./iOS/AIV/Assets.xcassets
./iOS/AIV/Assets.xcassets/AppIcon.appiconset
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-20.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-20@2x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-1024.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-20@3x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-40.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-29@3x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-60@2x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/Contents.json
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-76.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-60@3x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-29@2x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-40@3x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-83.5@2x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-76@2x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-40@2x.png
./iOS/AIV/Assets.xcassets/AppIcon.appiconset/iOS-29.png
./iOS/AIV/Assets.xcassets/Contents.json
./iOS/AIV/Base.lproj
./iOS/AIV/Base.lproj/Main.storyboard
./iOS/AIV/AIV.entitlements
./iOS/AIV/AppDelegate.swift
./iOS/AIV/Info.plist
./README.md
./RESEARCH.md
./find.txt
./PROMPT_HISTORY_RESEARCH.md
./files_to_edit.txt



Please use best practices and industry leading adjustments that can be made to these files alongside enabling more than just "Cutoff" and "Resonance" by adding sliders/knobs for:



Auto Level: It automatically equalizes the volume, so that it's not quiet and loud at times.


Pitch: It fine-tunes false notes to make it sound professional.


Deesser: We erase those sharp "S" and "C", we clean it up like a mess.


Compressor: The absolute basics, it makes the voice fat, hard and pulls it forward.


EQ and Saturation: Gives it color and that expensive shine.


Reverb: So that it doesn't sound dry, but it has space.