# LogicAIV Installation Guide

## Requirements
- macOS 12.0 or later
- Logic Pro X (or any AUv3 compatible host like GarageBand, Reaper)

## Installation Steps
1. **Download**: Download the `AIV.app.zip` from the [Releases Page](../../releases).
2. **Install**: Unzip and drag `AIV.app` into your `/Applications` folder.
   > **Note**: It is important to move it to `/Applications` so the system trusts it.
3. **Register**: Double-click `AIV.app` to open it. You should see the Rack View Standalone interface. This registers the Audio Unit extension with the system. You can close the app after a few seconds.
4. **Logic Pro**:
   - Open Logic Pro.
   - Logic should automatically scan and validate the new plugin.
   - If not found, go to **Logic Pro > Settings > Plug-in Manager**, locate "Demo" -> "AIV", and ensure it is checked/validated.
5. **Usage**:
   - In a Logic Pro Audio Track, go to the Audio FX slot.
   - Navigate to **Audio Units > Demo > AIV**.

## Troubleshooting
- **"App is damaged"**: Run `xattr -cr /Applications/AIV.app` in Terminal if macOS blocks it due to quarantine.
- **Not showing in Logic**: Restart your Mac or run `killall -9 AudioComponentRegistrar` in Terminal to force a rescan.
