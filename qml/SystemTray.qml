import QtQuick
import Qt.labs.platform as Platform
import Odizinne.QuickSoundSwitcher

Platform.SystemTrayIcon {
    id: systemTray
    visible: true
    signal togglePanelRequested()
    icon.source: Constants.getTrayIcon(AudioBridge.outputVolume, AudioBridge.outputMuted)
    tooltip: "Quick Sound Switcher"

    Component.onCompleted: {
        if (UserSettings.firstRun === undefined || UserSettings.firstRun === true) {
            UserSettings.firstRun = false
            systemTray.showMessage(
                qsTr("Quick Sound Switcher"),
                qsTr("Access sound panel from the system tray. This notification won't show again."),
                Platform.SystemTrayIcon.NoIcon,
                3000
            )
        }
    }

    onActivated: function(reason) {
        if (reason === Platform.SystemTrayIcon.Trigger) {
            togglePanelRequested()
        }
    }

    menu: Platform.Menu {
        Platform.MenuItem {
            enabled: false
            text: systemTray.truncateText(qsTr("Output: ") + (AudioBridge.isReady ?
                  systemTray.getOutputDeviceInfo() : "Loading..."), 50)
        }
        Platform.MenuItem {
            enabled: false
            text: systemTray.truncateText(qsTr("Input: ") + (AudioBridge.isReady ?
                  systemTray.getInputDeviceInfo() : "Loading..."), 50)
        }
        Platform.MenuSeparator {}
        Platform.MenuItem {
            text: qsTr("Windows sound settings (Legacy)")
            onTriggered: SoundPanelBridge.openLegacySoundSettings()
        }
        Platform.MenuItem {
            text: qsTr("Windows sound settings (Modern)")
            onTriggered: SoundPanelBridge.openModernSoundSettings()
        }
        Platform.MenuSeparator {}
        Platform.MenuItem {
            text: qsTr("Exit")
            onTriggered: Qt.quit()
        }
    }

    function truncateText(text, maxLength) {
        if (text.length <= maxLength) return text
        return text.substring(0, maxLength - 3) + "..."
    }

    function getOutputDeviceInfo() {
        if (!AudioBridge.isReady) return "Loading..."
        let defaultIndex = AudioBridge.outputDevices.currentDefaultIndex
        let deviceName = "Unknown Device"
        if (defaultIndex >= 0) {
            deviceName = AudioBridge.outputDevices.getDeviceName(defaultIndex)
        }
        return deviceName
    }

    function getInputDeviceInfo() {
        if (!AudioBridge.isReady) return "Loading..."
        let defaultIndex = AudioBridge.inputDevices.currentDefaultIndex
        let deviceName = "Unknown Device"
        if (defaultIndex >= 0) {
            deviceName = AudioBridge.inputDevices.getDeviceName(defaultIndex)
        }
        return deviceName
    }
}
