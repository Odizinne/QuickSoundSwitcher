// include/soundpanelbridge.h
#ifndef SOUNDPANELBRIDGE_H
#define SOUNDPANELBRIDGE_H

#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>
#include <QSettings>
#include <QScreen>
#include <QGuiApplication>
#include <QTranslator>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class SoundPanelBridge : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int panelMode READ panelMode NOTIFY panelModeChanged)
    Q_PROPERTY(QString taskbarPosition READ taskbarPosition NOTIFY taskbarPositionChanged)

    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaInfoChanged)
    Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY mediaInfoChanged)
    Q_PROPERTY(bool isMediaPlaying READ isMediaPlaying NOTIFY mediaInfoChanged)
    Q_PROPERTY(QString mediaArt READ mediaArt NOTIFY mediaInfoChanged)

public:
    explicit SoundPanelBridge(QObject* parent = nullptr);
    ~SoundPanelBridge() override;

    static SoundPanelBridge* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    static SoundPanelBridge* instance();

    int panelMode() const;
    QString taskbarPosition() const;

    Q_INVOKABLE void refreshPanelModeState();
    Q_INVOKABLE bool getShortcutState();
    Q_INVOKABLE void setStartupShortcut(bool enabled);
    Q_INVOKABLE bool getDarkMode();
    Q_INVOKABLE QString getAppVersion() const;
    Q_INVOKABLE QString getQtVersion() const;
    Q_INVOKABLE QString getCommitHash() const;
    Q_INVOKABLE QString getBuildTimestamp() const;

    QString mediaTitle() const;
    QString mediaArtist() const;
    bool isMediaPlaying() const;
    QString mediaArt() const;

    Q_INVOKABLE void playPause();
    Q_INVOKABLE void nextTrack();
    Q_INVOKABLE void previousTrack();
    Q_INVOKABLE void startMediaMonitoring();
    Q_INVOKABLE void stopMediaMonitoring();

    Q_INVOKABLE QString getCurrentLanguageCode() const;
    Q_INVOKABLE void changeApplicationLanguage(int languageIndex);
    Q_INVOKABLE QString getLanguageCodeFromIndex(int index) const;

    Q_INVOKABLE void toggleChatMixFromShortcut(bool enabled);
    Q_INVOKABLE void suspendGlobalShortcuts();
    Q_INVOKABLE void resumeGlobalShortcuts();
    bool areGlobalShortcutsSuspended() const;
    void requestChatMixNotification(QString message);

    Q_INVOKABLE void downloadLatestTranslations();
    Q_INVOKABLE void cancelTranslationDownload();
    Q_INVOKABLE QStringList getLanguageNativeNames() const;
    Q_INVOKABLE QStringList getLanguageCodes() const;

    Q_INVOKABLE void openLegacySoundSettings();
    Q_INVOKABLE void openModernSoundSettings();

private slots:
    void checkForTranslationUpdates();

signals:
    void panelModeChanged();
    void taskbarPositionChanged();
    void mediaInfoChanged();
    void languageChanged();
    void chatMixEnabledChanged(bool enabled);
    void chatMixNotificationRequested(QString message);
    void translationDownloadStarted();
    void translationDownloadProgress(const QString& language, int bytesReceived, int bytesTotal);
    void translationDownloadFinished(bool success, const QString& message);
    void translationDownloadError(const QString& errorMessage);
    void translationFileCompleted(const QString& language, int completed, int total);

private:
    static SoundPanelBridge* m_instance;
    QSettings settings;

    int m_currentPanelMode = 0;

    QString detectTaskbarPosition() const;
    QString m_mediaTitle;
    QString m_mediaArtist;
    bool m_isMediaPlaying = false;
    QString m_mediaArt;

    QTranslator *translator;

    bool m_globalShortcutsSuspended = false;

    QNetworkAccessManager* m_networkManager;
    QList<QNetworkReply*> m_activeDownloads;
    int m_totalDownloads;
    int m_completedDownloads;
    int m_failedDownloads;

    void downloadTranslationFile(const QString& languageCode, const QString& githubUrl);
    void onTranslationFileDownloaded();
    QString getTranslationDownloadPath() const;

    QTimer* m_autoUpdateTimer;
};

#endif // SOUNDPANELBRIDGE_H
