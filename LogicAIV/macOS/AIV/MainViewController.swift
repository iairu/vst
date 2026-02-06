/*
See LICENSE folder for this sample’s licensing information.

Abstract:
The view controller for the main view of the app.
*/

import Cocoa
import AIVFramework

class MainViewController: NSViewController {

    let audioUnitManager = AudioUnitManager()

    @IBOutlet var playButton: NSButton!
    @IBOutlet var toggleButton: NSButton!

    @IBOutlet var cutoffSlider: NSSlider!
    @IBOutlet var cutoffTextField: NSTextField!

    @IBOutlet var resonanceSlider: NSSlider!
    @IBOutlet var resonanceTextField: NSTextField!

    @IBOutlet var containerView: NSView!

    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Clear all existing subviews (this removes the Storyboard layout: Parameters Box, etc.)
        self.view.subviews.forEach { $0.removeFromSuperview() }
        
        embedPlugInView()
        populatePresetMenu()
        audioUnitManager.delegate = self
    }

    override func viewWillAppear() {
        super.viewWillAppear()
        view.window?.delegate = self
        
        // Remove "Toggle" button from toolbar if present
        if let toolbar = view.window?.toolbar {
            if let index = toolbar.items.firstIndex(where: { $0.label == "Toggle" }) {
                toolbar.removeItem(at: index)
            }
        }
    }

    private func embedPlugInView() {
        guard let controller = audioUnitManager.viewController else {
            fatalError("Could not load audio unit's view controller.")
        }

        // Present the view controller's view full screen
        addChild(controller)
        view.addSubview(controller.view)
        controller.view.pinToSuperviewEdges()
    }

    private func populatePresetMenu() {
        guard let presetMenu = NSApplication.shared.mainMenu?.item(withTag: 666)?.submenu else { return }
        for preset in audioUnitManager.presets {
            let menuItem = NSMenuItem(title: preset.name,
                                      action: #selector(handleMenuSelection(_:)),
                                      keyEquivalent: "\(preset.number + 1)")
            menuItem.tag = preset.number
            presetMenu.addItem(menuItem)
        }

        if let currentPreset = audioUnitManager.currentPreset {
            presetMenu.item(at: currentPreset.number)?.state = .on
        }
    }

    @objc
    private func handleMenuSelection(_ sender: NSMenuItem) {
        sender.menu?.items.forEach { $0.state = .off }
        sender.state = .on
        audioUnitManager.currentPreset = audioUnitManager.presets[sender.tag]
    }

    // MARK: Action Methods

    /// Handles Play/Stop button touches.
    @IBAction func togglePlay(_ sender: NSButton) {
        audioUnitManager.togglePlayback()
    }

    @IBAction func toggleView(_ sender: NSButton) {
        audioUnitManager.toggleView()
    }

    /// Cutoff frequency value changed handler
    @IBAction func cutoffSliderValueChanged(_ sender: NSSlider) {
        // audioUnitManager.cutoffValue = frequencyValueForSliderLocation(sender.floatValue)
    }

    /// Resonance value changed handler
    @IBAction func resonanceSliderValueChanged(_ sender: NSSlider) {
        // audioUnitManager.resonanceValue = sender.floatValue
    }

    // MARK: Private

    private func logValueForNumber(_ number: Float) -> Float {
        return log(number) / log(2)
    }

    private func frequencyValueForSliderLocation(_ location: Float) -> Float {
        var value = pow(2, location)
        value = (value - 1) / 511

        value *= (defaultMaxHertz - defaultMinHertz)

        return value + defaultMinHertz
    }
}

extension MainViewController: NSWindowDelegate {

    func windowWillClose(_ notification: Notification) {
        audioUnitManager.cleanup()
    }
}

extension MainViewController: AUManagerDelegate {
    // Legacy support removed
}

