#include "ventana.h"

#include <QCoreApplication>
#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QImage>
#include <QImageReader>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QResizeEvent>
#include <QStringList>
#include <QVBoxLayout>

Ventana::Ventana(QWidget *parent)
    : QWidget(parent),
      m_backgroundLabel(new QLabel(this)),
      m_overlayCard(new QFrame(this)),
      m_titleLabel(new QLabel(this)),
      m_subtitleLabel(new QLabel(this)),
      m_avatarLabel(new QLabel(this)),
      m_descriptionLabel(new QLabel(this)),
      m_contactLabel(new QLabel(this)),
      m_detailsLabel(new QLabel(this))
{
    inicializar();
}

QString Ventana::nombrePantalla() const
{
    return "VentanaPrincipal";
}

void Ventana::inicializar()
{
    setWindowTitle("Perfil profesional");
    setStyleSheet("QWidget { background: #0f1720; color: #f3f5f7; }");

    m_backgroundLabel->setScaledContents(false);
    m_backgroundLabel->lower();
    m_backgroundLabel->setAlignment(Qt::AlignCenter);

    m_overlayCard->setObjectName("overlayCard");
    m_overlayCard->setStyleSheet(
        "#overlayCard {"
        "background-color: rgba(12, 20, 28, 185);"
        "border: 1px solid rgba(255, 255, 255, 45);"
        "border-radius: 24px;"
        "}"
        "QLabel { background: transparent; }");

    QHBoxLayout *layout = new QHBoxLayout(m_overlayCard);
    layout->setContentsMargins(36, 36, 36, 36);
    layout->setSpacing(28);

    m_avatarLabel->setFixedSize(220, 220);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setStyleSheet(
        "background-color: rgba(255,255,255,25);"
        "border-radius: 110px;"
        "border: 2px solid rgba(255,255,255,70);");

    m_avatarPixmap = loadProfilePhoto();
    updateAvatar();

    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->setSpacing(12);

    m_titleLabel->setText("Facundo Cenci Heredia Ariel");
    m_titleLabel->setStyleSheet("font-size: 34px; font-weight: 700; color: #ffffff;");

    m_subtitleLabel->setText("Computer Engineering student | C++ and Qt desktop development");
    m_subtitleLabel->setWordWrap(true);
    m_subtitleLabel->setStyleSheet("font-size: 18px; color: #dbe7f0;");

    m_descriptionLabel->setText(
        "Computer Engineering student in 3rd year at Universidad Blas Pascal, based in Cordoba, Argentina. "
        "I have a solid foundation in C++ programming, programming logic and data structures, and I am currently "
        "strengthening my workflow with Git, Docker and modern desktop development tools.");
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("font-size: 16px; color: #eef4f8;");

    m_contactLabel->setText(
        "Contact\n"
        "facundocenci@gmail.com | +54 351 517 7835\n"
        "Cordoba, Argentina");
    m_contactLabel->setWordWrap(true);
    m_contactLabel->setStyleSheet(
        "font-size: 15px; color: #f4f7fa; background-color: rgba(255,255,255,18); "
        "border-radius: 14px; padding: 14px;");

    m_detailsLabel->setText(
        "Professional profile\n"
        "- Seeking my first professional experience in software development.\n"
        "- Focused on clean desktop interfaces, robust logic and practical problem solving.\n\n"
        "Technical skills\n"
        "- C++: Intermediate\n"
        "- Tools: Git (basic), Docker (learning)\n"
        "- Environments: Visual Studio, Qt Creator\n"
        "- Other: Data structures and programming logic\n\n"
        "Education and languages\n"
        "- Computer Engineering, Universidad Blas Pascal (2024 - ongoing, expected graduation 2028/2029)\n"
        "- High School Diploma, Maestro Diehl\n"
        "- English: Intermediate, preparing for First Certificate\n"
        "- Spanish: Native");
    m_detailsLabel->setWordWrap(true);
    m_detailsLabel->setStyleSheet("font-size: 15px; color: #d9e4eb;");

    textLayout->addWidget(m_titleLabel);
    textLayout->addWidget(m_subtitleLabel);
    textLayout->addWidget(m_descriptionLabel);
    textLayout->addWidget(m_contactLabel);
    textLayout->addWidget(m_detailsLabel);
    textLayout->addStretch();

    layout->addWidget(m_avatarLabel, 0, Qt::AlignTop);
    layout->addLayout(textLayout, 1);
}

void Ventana::aplicarTema(const QPixmap &imagen)
{
    m_pixmap = imagen;
    refreshPixmap();
}

void Ventana::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_backgroundLabel->setGeometry(rect());

    const int marginX = width() / 10;
    const int marginY = height() / 7;
    m_overlayCard->setGeometry(marginX, marginY, width() - (marginX * 2), height() - (marginY * 2));

    updateAvatar();
    refreshPixmap();
}

void Ventana::updateAvatar()
{
    if (m_avatarPixmap.isNull()) {
        return;
    }

    const int avatarSize = qMin(m_avatarLabel->width(), m_avatarLabel->height());
    const QPixmap scaled = m_avatarPixmap.scaled(avatarSize, avatarSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPixmap circular(avatarSize, avatarSize);
    circular.fill(Qt::transparent);

    QPainter painter(&circular);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath clipPath;
    clipPath.addEllipse(0, 0, avatarSize, avatarSize);
    painter.setClipPath(clipPath);
    painter.drawPixmap(0, 0, scaled);
    painter.end();

    m_avatarLabel->setPixmap(circular);
}

QPixmap Ventana::loadProfilePhoto() const
{
    const QString appDir = QCoreApplication::applicationDirPath();
    QStringList candidates;
    QDir dir(appDir);

    for (int depth = 0; depth < 5; ++depth) {
        candidates << dir.filePath("assets/profile.jpg");
        if (!dir.cdUp()) {
            break;
        }
    }

    candidates << QDir::current().absoluteFilePath("assets/profile.jpg");

    for (const QString &path : candidates) {
        QImageReader reader(path);
        if (!reader.canRead()) {
            continue;
        }

        reader.setAutoTransform(true);
        const QImage image = reader.read();
        if (!image.isNull()) {
            return QPixmap::fromImage(image);
        }
    }

    return QPixmap(":/assets/avatar.svg");
}

void Ventana::refreshPixmap()
{
    if (!m_pixmap.isNull()) {
        m_backgroundLabel->setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}
