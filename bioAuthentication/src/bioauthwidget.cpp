#include "bioauthwidget.h"
#include "ui_bioauthwidget.h"
#include <QMovie>
#include "generic.h"

BioAuthWidget::BioAuthWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BioAuthWidget),
    bioAuth(nullptr)
{
    ui->setupUi(this);
}

BioAuthWidget::~BioAuthWidget()
{
    delete ui;
}

void BioAuthWidget::on_btnPasswdAuth_clicked()
{
    if(bioAuth->isAuthenticating())
        bioAuth->stopAuth();

    Q_EMIT switchToPassword();
}

void BioAuthWidget::on_btnMore_clicked()
{
    Q_EMIT selectDevice();
}

void BioAuthWidget::on_btnRetry_clicked()
{
    if(bioAuth && !bioAuth->isAuthenticating()) {
        setMovie();
        bioAuth->startAuth();
    }
}

void BioAuthWidget::onBioAuthNotify(const QString &notifyMsg)
{
    ui->lblBioNotify->setText(notifyMsg);
}

void BioAuthWidget::onBioAuthComplete(uid_t uid, bool ret)
{
    setImage();

    Q_EMIT authComplete(uid, ret);
}

void BioAuthWidget::setMovie()
{
    QString typeString = bioTypeStrings.at(device.biotype);
    QString moviePath = QString("%1/images/%2.gif").arg(GET_STR(UKUI_BIOMETRIC)).arg(typeString);
    QMovie *movie = new QMovie(moviePath);
    movie->setScaledSize(QSize(ui->lblBioImage->width(), ui->lblBioImage->height()));

    ui->lblBioImage->setMovie(movie);
    movie->start();

    qDebug() << "set movie " << moviePath;
}

void BioAuthWidget::setImage()
{
    QString typeString = bioTypeStrings.at(device.biotype);
    QString pixmapPath = QString("%1/images/%2.png").arg(GET_STR(UKI_BIOMETRIC)).arg(typeString);
    QPixmap pixmap(pixmapPath);
    pixmap = pixmap.scaled(ui->lblBioImage->width(), ui->lblBioImage->height(),
                           Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->lblBioImage->setPixmap(pixmap);

    qDebug() << "set pixmap " << typeString;
}

void BioAuthWidget::startAuth(uid_t uid, const DeviceInfo &device)
{
    this->uid = uid;
    this->device = device;

    if(bioAuth) {
        delete bioAuth;
        bioAuth = nullptr;
    }

    setMovie();

    bioAuth = new BioAuth(uid, device, this);
    connect(bioAuth, &BioAuth::notify, this, &BioAuthWidget::onBioAuthNotify);
    connect(bioAuth, &BioAuth::authComplete, this, &BioAuthWidget::onBioAuthComplete);

    bioAuth->startAuth();
}

void BioAuthWidget::setMoreDevices(bool hasMore)
{
    ui->btnMore->setVisible(hasMore);
}