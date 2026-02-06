/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Non-UI controller object used to manage the interaction with the AIVDemo audio unit.
*/

import Foundation
import AudioToolbox
import CoreAudioKit
import AVFoundation

// A simple wrapper type to prevent exposing the Core Audio AUAudioUnitPreset in the UI layer.
public struct Preset {
    fileprivate init(preset: AUAudioUnitPreset) {
        audioUnitPreset = preset
    }
    fileprivate let audioUnitPreset: AUAudioUnitPreset
    public var number: Int { return audioUnitPreset.number }
    public var name: String { return audioUnitPreset.name }
}

// Delegate protocol to be adopted to be notified of parameter value changes.
public protocol AUManagerDelegate: AnyObject {
    // Legacy parameters removed
}

// Controller object used to manage the interaction with the audio unit and its user interface.
public class AudioUnitManager {

    /// The user-selected audio unit.
    private var audioUnit: AIVDemo?

    public weak var delegate: AUManagerDelegate? 

    public private(set) var viewController: AIVDemoViewController!

    // Gets the audio unit's defined presets.
    public var presets: [Preset] {
        guard let audioUnitPresets = audioUnit?.factoryPresets else {
            return []
        }
        return audioUnitPresets.map { preset -> Preset in
            return Preset(preset: preset)
        }
    }

    // Retrieves or sets the audio unit's current preset.
    public var currentPreset: Preset? {
        get {
            guard let preset = audioUnit?.currentPreset else { return nil }
            return Preset(preset: preset)
        }
        set {
            audioUnit?.currentPreset = newValue?.audioUnitPreset
        }
    }

    /// The playback engine used to play audio.
    private let playEngine = SimplePlayEngine()

    // A token for our registration to observe parameter value changes.
    private var parameterObserverToken: AUParameterObserverToken!

    // The AudioComponentDescription matching the AIVExtension Info.plist
    private var componentDescription: AudioComponentDescription = {

        // Ensure that AudioUnit type, subtype, and manufacturer match the extension's Info.plist values
        var componentDescription = AudioComponentDescription()
        componentDescription.componentType = kAudioUnitType_Effect
        componentDescription.componentSubType = 0x666c7472 /*'fltr'*/
        componentDescription.componentManufacturer = 0x44656d6f /*'Demo'*/
        componentDescription.componentFlags = 0
        componentDescription.componentFlagsMask = 0

        return componentDescription
    }()

    private let componentName = "Demo: AIVDemo"

    public init() {

        viewController = loadViewController()

        /*
         Register our `AUAudioUnit` subclass, `AIVDemo`, to make it able
         to be instantiated via its component description.

         Note that this registration is local to this process.
         */
        AUAudioUnit.registerSubclass(AIVDemo.self,
                                     as: componentDescription,
                                     name: componentName,
                                     version: UInt32.max)

        AVAudioUnit.instantiate(with: componentDescription) { audioUnit, error in
            guard error == nil, let audioUnit = audioUnit else {
                fatalError("Could not instantiate audio unit: \(String(describing: error))")
            }
            self.audioUnit = audioUnit.auAudioUnit as? AIVDemo
            self.connectParametersToControls()
            self.playEngine.connect(avAudioUnit: audioUnit)
        }
    }

    // Loads the audio unit's view controller from the extension bundle.
    private func loadViewController() -> AIVDemoViewController {
        // Locate the app extension's bundle in the main app's PlugIns directory
        guard let url = Bundle.main.builtInPlugInsURL?.appendingPathComponent("AIVExtension.appex"),
            let appexBundle = Bundle(url: url) else {
                fatalError("Could not find app extension bundle URL.")
        }

        #if os(iOS)
        let storyboard = Storyboard(name: "MainInterface", bundle: appexBundle)
        guard let controller = storyboard.instantiateInitialViewController() as? AIVDemoViewController else {
            fatalError("Unable to instantiate AIVDemoViewController")
        }
        return controller
        #elseif os(macOS)
        return AIVDemoViewController(nibName: "AIVDemoViewController", bundle: appexBundle)
        #endif
    }

    /**
     Called after instantiating our audio unit, to find the AU's parameters and
     connect them to our controls.
     */
    private func connectParametersToControls() {

        guard let audioUnit = audioUnit else {
            fatalError("Couldn't locate AIVDemo")
        }

        viewController.audioUnit = audioUnit
        
        // No standalone parameter handling needed - pure View Controller hosting
    }

    @discardableResult
    public func togglePlayback() -> Bool {
        return playEngine.togglePlay()
    }

    public func toggleView() {
        viewController.toggleViewConfiguration()
    }

    public func cleanup() {
        playEngine.stopPlaying()

        guard let parameterTree = audioUnit?.parameterTree else { return }
        parameterTree.removeParameterObserver(parameterObserverToken)
    }
}
