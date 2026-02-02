/*
See LICENSE folder for this sample’s licensing information.

Abstract:
AIVDemoViewController is the app extension's principal class, responsible for creating both the audio unit and its view.
*/

import CoreAudioKit
import AIVFramework

extension AIVDemoViewController: AUAudioUnitFactory {

    public func createAudioUnit(with componentDescription: AudioComponentDescription) throws -> AUAudioUnit {
        audioUnit = try AIVDemo(componentDescription: componentDescription, options: [])
        return audioUnit!
    }
}
