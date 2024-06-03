#include "widget.h"
#include "ui_widget.h"
#include "qpainter.h"
#include "QKeyEvent"
#include "game.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    isGameStarted = false;
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(startGame()));
    // Create and start the timer for updating the GUI, the centipede, the bullet, the player
    itsDisplayTimer = new QTimer(this);
    itsCentipedeTimer = new QTimer(this);
    itsBulletTimer = new QTimer(this);
    itsPlayerTimer = new QTimer(this);

    connect(itsDisplayTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(itsPlayerTimer, SIGNAL(timeout()), this, SLOT(movePlayer()));
    connect(itsBulletTimer, SIGNAL(timeout()), this, SLOT(moveBullet()));
    connect(itsCentipedeTimer, SIGNAL(timeout()), this, SLOT(moveCentipede()));
}

Widget::~Widget()
{
    delete ui;
    delete itsBulletTimer;
    delete itsCentipedeTimer;
    delete itsDisplayTimer;
    delete itsPlayerTimer;
    delete itsGame;
}

void Widget::paintEvent(QPaintEvent *event)
{
    if(isGameStarted)
    {
        Q_UNUSED(event); //pour éviter les avertissements du compilateur concernant des variables non utilisées
        QPainter painter(this);
        // Draw the Bricks and the avatar
        drawCentipede(& painter);
        drawPlayer(& painter);
        drawBullet(& painter);
        drawMushrooms(& painter);
    }
}


void Widget::drawCentipede(QPainter & painter)
{
    painter.drawImage(0, 0, *itsBackgroundImage);
}

void Widget::drawBullet(QPainter & painter)
{

}

void Widget::drawPlayer(QPainter & painter)
{
    painter.drawImage(0, 0, *itsBackgroundImage);
}

void Widget::drawMushrooms(QPainter & painter)
{
    painter.drawImage(0, 0, *itsBackgroundImage);
}

void Widget::movePlayer(QKeyEvent * event)
{
    // Handle key press events for left and right arrow keys
    if (event->key() == Qt::Key_Z)
    {
        itsGame;
    }
    if (event->key() == Qt::Key_Q)
    {
        itsGame;
    }
    if (event->key() == Qt::Key_S)
    {
        itsGame;
    }
    if (event->key() == Qt::Key_D)
    {
        itsGame;
    }
    if (event->key() == Qt::Key_Space)
    {
        itsGame->shoot();
    }
}

void Widget::moveBullet()
{
    itsGame->get;
}
