#include "MediaFlyout.h"
#include "ui_MediaFlyout.h"
#include "Utils.h"
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

MediaFlyout::MediaFlyout(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MediaFlyout)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowDoesNotAcceptFocus);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(width());
    borderColor = Utils::getTheme() == "light" ? QColor(255, 255, 255, 32) : QColor(0, 0, 0, 52);

    ui->prev->setIcon(Utils::getButtonsIcon("prev"));
    ui->next->setIcon(Utils::getButtonsIcon("next"));
    ui->pause->setIcon(Utils::getButtonsIcon("pause"));

    connect(ui->next, &QToolButton::clicked, this, &MediaFlyout::onNextClicked);
    connect(ui->prev, &QToolButton::clicked, this, &MediaFlyout::onPrevClicked);
    connect(ui->pause, &QToolButton::clicked, this, &MediaFlyout::onPauseClicked);
}

MediaFlyout::~MediaFlyout()
{
    delete ui;
}

void MediaFlyout::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setBrush(this->palette().color(QPalette::Window));
    painter.setPen(Qt::NoPen);

    QPainterPath path;
    path.addRoundedRect(this->rect().adjusted(1, 1, -1, -1), 8, 8);
    painter.drawPath(path);

    QPen borderPen(borderColor);
    borderPen.setWidth(1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath borderPath;
    borderPath.addRoundedRect(this->rect().adjusted(0, 0, -1, -1), 8, 8);
    painter.drawPath(borderPath);
}

void MediaFlyout::animateIn(QRect trayIconGeometry, int panelHeight)
{
    QPoint trayIconPos = trayIconGeometry.topLeft();
    int trayIconCenterX = trayIconPos.x() + trayIconGeometry.width() / 2;

    int panelX = trayIconCenterX - this->width() / 2;
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int startY = screenGeometry.bottom();
    int targetY = trayIconGeometry.top() - this->height() - 12 - 12 - panelHeight;

    this->move(panelX, startY);
    this->show();

    const int durationMs = 300;
    const int refreshRate = 1;
    const double totalSteps = durationMs / refreshRate;

    int currentStep = 0;
    QTimer *animationTimer = new QTimer(this);

    animationTimer->start(refreshRate);

    connect(animationTimer, &QTimer::timeout, this, [=, this]() mutable {
        if (currentStep >= totalSteps) {
            animationTimer->stop();
            animationTimer->deleteLater();
            this->move(panelX, targetY); // Ensure final position is set
            return;
        }

        double t = static_cast<double>(currentStep) / totalSteps; // Normalized time (0 to 1)
        // Easing function: Smooth deceleration
        double easedT = 1 - pow(1 - t, 3);

        // Interpolated Y position
        int currentY = startY + easedT * (targetY - startY);
        this->move(panelX, currentY);

        ++currentStep;
    });
}

void MediaFlyout::animateOut(QRect trayIconGeometry)
{
    QPoint trayIconPos = trayIconGeometry.topLeft();
    int trayIconCenterX = trayIconPos.x() + trayIconGeometry.width() / 2;

    int panelX = trayIconCenterX - this->width() / 2; // Center horizontally
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int startY = this->y();
    int targetY = screenGeometry.bottom(); // Move to the bottom of the screen

    const int durationMs = 300;
    const int refreshRate = 1;
    const double totalSteps = durationMs / refreshRate;

    int currentStep = 0;
    QTimer *animationTimer = new QTimer(this);

    animationTimer->start(refreshRate);

    connect(animationTimer, &QTimer::timeout, this, [=, this]() mutable {
        if (currentStep >= totalSteps) {
            animationTimer->stop();
            animationTimer->deleteLater();

            return;
        }

        double t = static_cast<double>(currentStep) / totalSteps; // Normalized time (0 to 1)
        // Easing function: Smooth deceleration
        double easedT = 1 - pow(1 - t, 3);

        // Interpolated Y position
        int currentY = startY + easedT * (targetY - startY);
        this->move(panelX, currentY);

        ++currentStep;
    });
}

void MediaFlyout::updateUi(MediaSession session)
{
    ui->title->setText(session.title);
    ui->artist->setText(session.artist);
    ui->prev->setEnabled(session.canGoPrevious);
    ui->next->setEnabled(session.canGoNext);

    QPixmap originalIcon = session.icon.pixmap(64, 64);
    QPixmap roundedIcon = roundPixmap(originalIcon, 8);
    ui->icon->setPixmap(roundedIcon);

    QString playPause;
    if (session.playbackState == "Playing") {
        playPause = "pause";
    } else {
        playPause = "play";
    }
    ui->pause->setIcon(Utils::getButtonsIcon(playPause));
}

QPixmap MediaFlyout::roundPixmap(const QPixmap &src, int radius) {
    if (src.isNull()) {
        return QPixmap();
    }

    QPixmap dest(src.size());
    dest.fill(Qt::transparent);

    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, src.width(), src.height()), radius, radius);

    painter.setClipPath(path);
    painter.drawPixmap(0, 0, src);
    painter.end();

    return dest;
}

void MediaFlyout::onPrevClicked()
{
    emit requestPrev();
    //Utils::sendPrevKey();
}

void MediaFlyout::onNextClicked()
{
    emit requestNext();
    //Utils::sendNextKey();
}

void MediaFlyout::onPauseClicked()
{
    emit requestPause();
    //Utils::sendPlayPauseKey();
}
