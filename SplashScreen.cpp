#include "SplashScreen.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QLinearGradient>
#include <QRadialGradient>

SplashScreen::SplashScreen(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);

    // Size: compact and centered like a macOS app launch
    setFixedSize(320, 420);

    // Center on screen
    if (auto *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeo = screen->availableGeometry();
        move(screenGeo.center() - rect().center());
    }

    // Opacity effect for fade animations
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);
}

SplashScreen::~SplashScreen() {}

void SplashScreen::startAnimation()
{
    show();
    raise();

    // --- Phase 1: Fade in ---
    m_fadeInAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeInAnim->setDuration(600);
    m_fadeInAnim->setStartValue(0.0);
    m_fadeInAnim->setEndValue(1.0);
    m_fadeInAnim->setEasingCurve(QEasingCurve::OutCubic);

    // --- Phase 2: Progress bar fill (subtle, builds anticipation) ---
    m_progressAnim = new QPropertyAnimation(this, "m_progress", this);
    m_progressAnim->setDuration(900);
    m_progressAnim->setStartValue(0.0);
    m_progressAnim->setEndValue(1.0);
    m_progressAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Chain: fade in first, then progress fills during the hold
    connect(m_fadeInAnim, &QPropertyAnimation::finished, this, [this]() {
        m_progressAnim->start();
    });

    // After progress completes, hold briefly then fade out
    connect(m_progressAnim, &QPropertyAnimation::finished, this, [this]() {
        QTimer::singleShot(400, this, &SplashScreen::fadeOutAndClose);
    });

    m_fadeInAnim->start();
}

void SplashScreen::fadeOutAndClose()
{
    m_fadeOutAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeOutAnim->setDuration(350);
    m_fadeOutAnim->setStartValue(1.0);
    m_fadeOutAnim->setEndValue(0.0);
    m_fadeOutAnim->setEasingCurve(QEasingCurve::InCubic);

    connect(m_fadeOutAnim, &QPropertyAnimation::finished, this, [this]() {
        hide();
        emit finished();
        close();
    });

    m_fadeOutAnim->start();
}

void SplashScreen::setProgress(qreal p)
{
    m_progress = p;
    update(); // repaint the progress indicator
}

void SplashScreen::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int w = width();
    const int cx = w / 2;

    // --- Background: rounded rect with subtle fill ---
    {
        QPainterPath bgPath;
        bgPath.addRoundedRect(rect(), 20, 20);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0x0D, 0x0D, 0x1A));
        painter.drawPath(bgPath);
    }

    // --- Subtle glow behind the logo ---
    {
        QRadialGradient glow(cx, 145, 90);
        glow.setColorAt(0.0, QColor(0x4C, 0xAF, 0x50, 40));
        glow.setColorAt(0.5, QColor(0x4C, 0xAF, 0x50, 10));
        glow.setColorAt(1.0, Qt::transparent);
        painter.setBrush(glow);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(cx, 145), 90, 90);
    }

    // --- Logo: geometric "M" inside a rounded square ---
    const int logoSize = 88;
    const int logoX = cx - logoSize / 2;
    const int logoY = 88;
    const qreal logoRadius = 20.0;

    // Rounded square background for logo
    {
        QPainterPath iconBg;
        iconBg.addRoundedRect(logoX, logoY, logoSize, logoSize, logoRadius, logoRadius);
        painter.setBrush(QColor(0x4C, 0xAF, 0x50)); // brand green
        painter.setPen(Qt::NoPen);
        painter.drawPath(iconBg);
    }

    // Draw a stylized "M" — two overlapping document shapes
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0xFF, 0xFF, 0xFF, 230));

        const int pad = 18;
        const int barW = 16;
        const int gap = 6;
        const int barH = logoSize - pad * 2;

        // Left bar
        int leftX = logoX + pad;
        int leftY = logoY + pad;
        painter.drawRoundedRect(leftX, leftY, barW, barH, 4, 4);

        // Right bar
        int rightX = logoX + logoSize - pad - barW;
        painter.drawRoundedRect(rightX, leftY, barW, barH, 4, 4);

        // Middle line (lower)
        int midW = barW;
        int midX = cx - midW / 2;
        int midY = logoY + logoSize - pad - barW * 2 - gap;
        int midH = barW * 2 + gap;
        painter.drawRoundedRect(midX, midY, midW, midH, 4, 4);

        // Top connecting dot
        int dotY = logoY + pad + 4;
        painter.drawRoundedRect(midX, dotY, midW, barW, 4, 4);
    }

    // --- App name ---
    {
        QFont nameFont("Segoe UI", 26, QFont::Light);
        nameFont.setHintingPreference(QFont::PreferNoHinting);
        painter.setFont(nameFont);
        painter.setPen(QColor(0xFF, 0xFF, 0xFF, 240));
        painter.drawText(QRectF(0, 200, w, 36), Qt::AlignHCenter | Qt::AlignVCenter, "MD Editor");
    }

    // --- Tagline ---
    {
        QFont tagFont("Segoe UI", 13, QFont::Light);
        tagFont.setLetterSpacing(QFont::AbsoluteSpacing, 3.0);
        painter.setFont(tagFont);
        painter.setPen(QColor(0xFF, 0xFF, 0xFF, 110));
        painter.drawText(QRectF(0, 232, w, 20), Qt::AlignHCenter | Qt::AlignVCenter, "FOR  AI");
    }

    // --- Subtle progress bar ---
    {
        const int barW = 160;
        const int barH = 2;
        const int barX = cx - barW / 2;
        const int barY = 290;

        // Track
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0xFF, 0xFF, 0xFF, 30));
        painter.drawRoundedRect(barX, barY, barW, barH, 1, 1);

        // Fill
        qreal fillW = barW * m_progress;
        if (fillW > 4) {
            painter.setBrush(QColor(0x4C, 0xAF, 0x50, 180));
            painter.drawRoundedRect(barX, barY, (int)fillW, barH, 1, 1);
        }
    }
}
