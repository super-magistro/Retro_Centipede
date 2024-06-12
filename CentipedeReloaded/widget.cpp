#include "widget.h"
#include "ui_widget.h"

#include <QTransform>

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget), isGameStarted(false)
{
    ui->setupUi(this);

    // Change title of the window and set minimum size of the window
    setWindowTitle("Centipede Reloaded - v1.0");
    setMinimumSize(400, 300);

    // Change background color of the widget
    //QPalette bgColor = QPalette();
    //bgColor.setColor(QPalette::Window, Qt::darkGray);
    //setAutoFillBackground(true);
    //setPalette(bgColor);

    // Create timers for updating the GUI, the centipede, the bullet, the player
    itsDisplayTimer = new QTimer(this);
    itsCentipedeTimer = new QTimer(this);
    itsBulletTimer = new QTimer(this);
    itsPlayerTimer = new QTimer(this);
    itsPowerUpMovementTimer = new QTimer(this);
    itsRafaleTimer = new QTimer(this);
    itsPiercingTimer = new QTimer(this);
    itsSpiderAppearTimer = new QTimer(this);
    itsSpiderTimer = new QTimer(this);

    // Loading assets
    itsCentiBodyImg.load(":/assets/centipede_body1.png");
    itsCentiHeadImg.load(":/assets/centipede_head1.png");
    itsCentiTailImg.load(":/assets/centipede_tail1.png");
    itsPlayerImg.load(":/assets/player.png");
    itsMushState1Img.load(":/assets/mushroom_state1.png");
    itsMushState2Img.load(":/assets/mushroom_state2.png");
    itsMushState3Img.load(":/assets/mushroom_state3.png");
    itsMushState4Img.load(":/assets/mushroom_state4.png");
    itsSpiderImg.load(":/assets/spider.png");
    itsBulletImg.load(":/assets/bullet.png");
    itsRafalePuImg.load(":/assets/rafale.png");
    itsTranspercantPuImg.load(":/assets/balle_transpercante.png");
    itsHerbicidePuImg.load(":/assets/herbicide.png");
    itsGrassTexture.load(":/assets/grass.png");
    itsDarkGrassTexture.load(":/assets/grass_dark.png");

    // ---- ROTATE EXAMPLE: ----
    //itsCentiHeadImg = itsCentiHeadImg.transformed(QTransform().rotate(90.0));
    // -------------------------

    // Initialize the direction of the player
    itsPlayerDirection.dirX = 0;
    itsPlayerDirection.dirY = 0;

    // Change background of the widget
    QPalette pal = palette();
    itsDarkGrassTexture = itsDarkGrassTexture.scaled(QSize(width() * 5 / 100, width() * 5 / 100), Qt::KeepAspectRatio);
    QBrush brush(itsDarkGrassTexture);
    pal.setBrush(QPalette::Window, brush);
    setPalette(pal);

    // Connect buttons to their method
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(ui->back_button, SIGNAL(clicked()), this, SLOT(backToMenu()));
    connect(ui->back_button_2, SIGNAL(clicked()), this, SLOT(backToMenu()));
    connect(ui->buttonHowToPlay, SIGNAL(clicked()), this, SLOT(goToHowToPlay()));
    connect(ui->backToMenuButton_2, SIGNAL(clicked()), this, SLOT(backToMenu()));
    connect(ui->resumeGameButton, SIGNAL(clicked()), this, SLOT(resumeGame()));
    connect(ui->backToMenuButton, SIGNAL(clicked()), this, SLOT(backToMenu()));
    connect(ui->leaderboardButton, SIGNAL(clicked()), this, SLOT(displayLeaderboard()));
    connect(ui->backToMenu, SIGNAL(clicked()), this, SLOT(backToMenu()));

    //Set style sheets for the ui
    //ui->HowToPlay->setStyleSheet("../../../styleSheets/styleSheetHowtoPlay");

    // Connect timers to their method
    connect(itsDisplayTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(itsPlayerTimer, SIGNAL(timeout()), this, SLOT(movePlayer()));
    connect(itsBulletTimer, SIGNAL(timeout()), this, SLOT(moveBullet()));
    connect(itsCentipedeTimer, SIGNAL(timeout()), this, SLOT(moveCentipede()));
    connect(itsPowerUpMovementTimer, SIGNAL(timeout()), this, SLOT(movePowerUps()));
    connect(itsRafaleTimer, SIGNAL(timeout()), this, SLOT(rafaleShot()));
    connect(itsPiercingTimer, SIGNAL(timeout()), this, SLOT(piercingEnd()));
    connect(itsSpiderAppearTimer, SIGNAL(timeout()), this, SLOT(spiderAppear()));
    connect(itsSpiderTimer, SIGNAL(timeout()), this, SLOT(moveSpider()));

    itsLeaderboard = new Leaderboard(":/leaderboard.txt");
    ui->stackedWidget->setCurrentIndex(0);

    //change the color of the font
    this->setStyleSheet("QLabel { color: white; }");

    //ui->stackedWidget->setGeometry(QRect(0, 0, width(), height()));
    //ui->stackedWidget->setFixedSize(QSize(width(), height()));
    //setStyleSheet("QRect#page { background-image: url(:/assets/grass.png);"
    //              " background-size: 200px; }");
}

Widget::~Widget()
{
    // Delete all pointers
    delete ui;
    delete itsBulletTimer;
    delete itsCentipedeTimer;
    delete itsDisplayTimer;
    delete itsPlayerTimer;
    delete itsPowerUpMovementTimer;
    delete itsRafaleTimer;
    delete itsPiercingTimer;
    delete itsGame;
    delete itsLeaderboard;
    delete itsSpiderAppearTimer;
    delete itsSpiderTimer;
}

void Widget::paintEvent(QPaintEvent *event)
{
    // Avoid warnings of unused variable of Qt
    Q_UNUSED(event);
    if (isGameStarted && !isGamePaused)
    {
        QPainter painter(this);

        // Paint in light gray the area of the game board
        itsGrassTexture.load(":/assets/grass.png");
        itsGrassTexture = itsGrassTexture.scaled(QSize(width() * 5 / 100, width() * 5 / 100), Qt::KeepAspectRatio);
        QBrush brush(itsGrassTexture);
        brush.setStyle(Qt::TexturePattern);
        painter.fillRect(itsGame->getItsBoard(), brush);
        //painter.drawImage(itsGame->getItsBoard(), QImage(":/assets/grass.png"));

        // Draw each entities of the game
        drawCentipede(painter);
        drawPlayer(painter);
        drawBullet(painter);
        drawMushrooms(painter);
        drawPowerUps(painter);
        drawHeadUpDisplay(painter);
        drawSpider(painter);

        // Check if the game has ended
        endGame();
    }
}

void Widget::resizeEvent(QResizeEvent *event)
{
    // Avoid warnings of unused variable of Qt
    Q_UNUSED(event);

    // Change background of the widget
    QPalette pal = palette();
    itsDarkGrassTexture.load(":/assets/grass_dark.png");
    itsDarkGrassTexture = itsDarkGrassTexture.scaled(QSize(width() * 5 / 100, width() * 5 / 100), Qt::KeepAspectRatio);
    QBrush brush(itsDarkGrassTexture);
    brush.setStyle(Qt::TexturePattern);
    pal.setBrush(QPalette::Window, brush);
    setPalette(pal);

    if (itsGame != nullptr)
    {
        // Calculate the size of itsBoard based on the smallest dimension of the window
        int maxHeight = height() * 95 / 100; // 95% of the window height
        int maxBoardHeight = maxHeight;

        // Find the largest multiple of BOARD_HEIGHT that fits within the available height
        int boardHeight = (maxBoardHeight / BOARD_HEIGHT) * BOARD_HEIGHT;
        int boardWidth = boardHeight * BOARD_WIDTH / BOARD_HEIGHT;
        int boardX = width() / 2 - boardWidth / 2;
        int boardY = height() * 5 / 100;


        itsCentipedeTimer->stop();
        itsGame->setBoard(QRect(boardX, boardY, boardWidth, boardHeight));

        // Adjust timers or other parameters based on the new window size
        itsCentipedeTimer->start(4000 / boardWidth); // set the speed of it
        itsBulletTimer->start(3000 / boardHeight); // Set the speed of the bullet
        itsPlayerTimer->start(2500 / boardWidth); // Set the speed of the player
        if(itsGame->getItsSpider() != nullptr)
        {
            itsSpiderTimer->start(4000 / boardWidth);
        }
    }
}


void Widget::keyPressEvent(QKeyEvent * event)
{
    if((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && ui->stackedWidget->currentIndex() == 7)
    {
        processNewScore();
    }

    if(!isGameStarted) return;
    // Handle key press events for Z, Q, S, D and Space keys
    if (event->key() == Qt::Key_Z)
    {
        itsPlayerDirection.dirY = -1;
    }
    if (event->key() == Qt::Key_Q)
    {
        itsPlayerDirection.dirX = -1;
    }
    if (event->key() == Qt::Key_S)
    {
        itsPlayerDirection.dirY = 1;
    }
    if (event->key() == Qt::Key_D)
    {
        itsPlayerDirection.dirX = 1;
    }
    if (event->key() == Qt::Key_Space)
    {
        if(!itsGame->getIsRafaleActive()) itsGame->shoot();
    }
    if (event->key() == Qt::Key_Escape)
    {
        if(isGamePaused)
        {
            resumeGame();
        }
        else
        {
            pauseGame();
        }
    }
}

void Widget::keyReleaseEvent(QKeyEvent * event)
{
    // Handle key release events for Z, Q, S and D keys
    if (event->key() == Qt::Key_Z && itsPlayerDirection.dirY == -1)
    {
        itsPlayerDirection.dirY = 0;
    }
    if (event->key() == Qt::Key_Q && itsPlayerDirection.dirX == -1)
    {
        itsPlayerDirection.dirX = 0;
    }
    if (event->key() == Qt::Key_S && itsPlayerDirection.dirY == 1)
    {
        itsPlayerDirection.dirY = 0;
    }
    if (event->key() == Qt::Key_D && itsPlayerDirection.dirX == 1)
    {
        itsPlayerDirection.dirX = 0;
    }
}

void Widget::drawPlayer(QPainter & painter)
{
    if(SHOW_HITBOXES)
    {
        // shows the player hitbox
        painter.setPen(Qt::gray);
        painter.setBrush(Qt::SolidPattern);
        painter.drawRect(itsGame->getItsPlayer()->getItsHitBox());
    }
    else
    {
        // Draws the player at his position
        painter.drawImage(itsGame->getItsPlayer()->getItsHitBox(), itsPlayerImg);
    }
}

void Widget::drawMushrooms(QPainter & painter)
{
    //painter.drawImage(0, 0, *itsBackgroundImage);
    for(Mushroom * mushroom : *itsGame->getItsMushrooms())
    {
        if(SHOW_HITBOXES)
        {
            // displays the mushroom hitbox
            painter.setBrush(Qt::SolidPattern);
            if(mushroom->getItsState() == 4)
            {
                painter.setPen(Qt::red);
                painter.drawRect(mushroom->getItsHitBox());
            }
            else if(mushroom->getItsState() == 3)
            {
                painter.setPen(Qt::magenta);
                painter.drawRect(mushroom->getItsHitBox());
            }
            else if(mushroom->getItsState() == 2)
            {
                painter.setPen(Qt::yellow);
                painter.drawRect(mushroom->getItsHitBox());
            }
            else if(mushroom->getItsState() == 1)
            {
                painter.setPen(Qt::green);
                painter.drawRect(mushroom->getItsHitBox());
            }
        }
        else
        {
            switch (mushroom->getItsState()) {
            case 4:
                painter.drawImage(mushroom->getItsHitBox(), itsMushState1Img);
                break;
            case 3:
                painter.drawImage(mushroom->getItsHitBox(), itsMushState2Img);
                break;
            case 2:
                painter.drawImage(mushroom->getItsHitBox(), itsMushState3Img);
                break;
            case 1:
                painter.drawImage(mushroom->getItsHitBox(), itsMushState4Img);
                break;
            default:
                break;
            }
            // displays the mushroom image
        }
    }
}

void Widget::drawCentipede(QPainter & painter)
{
    for (vector<Centipede *>::iterator it = itsGame->getItsCentipedes()->begin(); it != itsGame->getItsCentipedes()->end(); ++it) {

        if(SHOW_HITBOXES)
        {
            // Display the hitbox of the tail
            painter.setPen(Qt::darkGreen);
            painter.setBrush(Qt::SolidPattern);
            painter.drawRect((*it)->getItsTail()->getItsHitBox()); 
        }
        else
        {
            // Display the image of the tail of a centipede
            painter.drawImage((*it)->getItsTail()->getItsHitBox(), itsCentiTailImg);
        }

        // Iterates on body parts of the centipede (head and tail excluded) in descending order
        for (BodyPart* currentPart = (*it)->getItsTail()->getItsParent(); currentPart->getItsParent() != nullptr; currentPart = currentPart->getItsParent())
        {
            if (SHOW_HITBOXES)
            {
                // Display the hitbox of the body part
                painter.setPen(Qt::cyan);
                painter.setBrush(Qt::SolidPattern);
                painter.drawRect(currentPart->getItsHitBox());
            }
            else
            {
                // Display the image of the body of a centipede
                painter.drawImage(currentPart->getItsHitBox(), itsCentiBodyImg);
            }
        }

        if (SHOW_HITBOXES)
        {
            // Display the hitbox of the head
            painter.setPen(Qt::blue);
            painter.setBrush(Qt::SolidPattern);
            painter.drawRect((*it)->getItsHead()->getItsHitBox());
        }
        else
        {
            // Display the image of the head of a centipede
            painter.drawImage((*it)->getItsHead()->getItsHitBox(), itsCentiHeadImg);
        }
    }
}

void Widget::drawBullet(QPainter & painter)
{
    // Check if the bullet exists
    for(Bullet* bullet : itsGame->getItsBullets())
    {
        if(SHOW_HITBOXES)
        {
            // displays the bullet hitbox
            painter.setPen(Qt::green);
            painter.setBrush(Qt::SolidPattern);
            painter.drawRect(bullet->getItsHitBox());
        }
        else
        {
            // displays the bullet image
            painter.drawImage(bullet->getItsHitBox(), itsBulletImg);
        }
    }
}

void Widget::drawPowerUps(QPainter & painter)
{
    for (PowerUp* powerup : itsGame->getItsPowerups())
    {
        if (SHOW_HITBOXES)
        {
            switch (powerup->getItsType())
            {
            case rafale:
                painter.setPen(Qt::black);
                painter.setBrush(Qt::SolidPattern);
                break;
            case transpercant:
                painter.setPen(Qt::yellow);
                painter.setBrush(Qt::SolidPattern);
                break;
            case herbicide:
                painter.setPen(Qt::green);
                painter.setBrush(Qt::SolidPattern);
                break;
            default:
                break;
            }
            painter.drawRect(powerup->getItsHitbox());
        }
        else
        {
            switch (powerup->getItsType())
            {
            case rafale:
                painter.drawImage(powerup->getItsHitbox(), itsRafalePuImg);
                break;
            case transpercant:
                painter.drawImage(powerup->getItsHitbox(), itsTranspercantPuImg);
                break;
            case herbicide:
                painter.drawImage(powerup->getItsHitbox(), itsHerbicidePuImg);
                break;
            default:
                break;
            }
        }
    }
}

void Widget::drawHeadUpDisplay(QPainter & painter)
{
    // Set the font and color for the text
    QFont font("Arial", 9, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::white);

    // Draw the score
    QString scoreText = QString("Score: %1").arg(itsGame->getItsScore());
    painter.drawText((this->width() * 0.1 - (QFontMetrics(font).boundingRect(scoreText).width() / 2)), (this->height() * 7 / 200), scoreText);

    // Draw the level number
    QString levelText = QString("Level %1").arg(itsGame->getCurrentLevel());
    painter.drawText((this->width() * 0.5 - (QFontMetrics(font).boundingRect(levelText).width() / 2)), (this->height() * 7 / 200), levelText);

    // Draw the life count
    QString lifeText = QString("Life: %1").arg(itsGame->getItsPlayer()->getItsHp());
    painter.drawText((this->width() * 0.9 - (QFontMetrics(font).boundingRect(lifeText).width() / 2)), (this->height() * 7 / 200), lifeText);

    painter.drawRect(QRect(0, this->height() * 5 / 100 - 1, this->width(), 0));
}

void Widget::moveBullet()
{
    itsGame->moveBullets();

    itsGame->checkCollisions();
}

void Widget::movePlayer()
{
    itsGame->movePlayer(itsPlayerDirection);
}

void Widget::movePowerUps()
{
    itsGame->movePowerUps();
    if(itsGame->getIsRafaleActive() && !itsRafaleTimer->isActive())
    {
        remainingRafaleShots = POWERUP_RAFALE_FIRERATE * POWERUP_RAFALE_DURATION;
        itsRafaleTimer->start(1000/POWERUP_RAFALE_FIRERATE);
    }
    if(itsGame->getIsPiercingActive() && !itsPiercingTimer->isActive())
    {
        itsPiercingTimer->start(1000*POWERUP_PIERCING_DURATION);
    }
}

void Widget::startGame(int level)
{
    int boardHeight = height() * 95 / 100;
    int boardWidth = boardHeight / BOARD_WIDTH * BOARD_HEIGHT;
    int baseCentipedeTimer = (4000 / boardWidth);
    itsCentipedeTimer->start(baseCentipedeTimer - level/10); // set the speed of it
    if(!isGameStarted)
    {
        // Set the stacked widget on an empty widget
        ui->stackedWidget->setCurrentIndex(3);
        this->update();

        // Calculate game board
        int boardX = width() / 2 - boardWidth / 2;
        int boardY = height() * 5 / 100;
        itsGame = new Game({ boardX, boardY, boardWidth, boardHeight });
        isGameStarted = true;
        itsDisplayTimer->start(16); // Update every 16 equal approximatly to 60fps
        itsBulletTimer->start(3000 / boardHeight); // Set the speed of the bullet
        itsPlayerTimer->start(2500 / boardWidth); // Set the speed of the player
        itsPowerUpMovementTimer->start(baseCentipedeTimer - level/10);
        itsSpiderAppearTimer->start(1000); //à mettre là ou le niveau 2 demarre
        itsSpiderAppearProbability = INCREMENT_INTERVAL; //Set the minimum probability for the spider to appear
    }
    else
    {
        itsGame->spawnCentipede();
    }

}

void Widget::endGame()
{
    if (itsGame->isLevelWon())
    {
        startGame(itsGame->getCurrentLevel());
        return;
    }
    else if (itsGame->isGameLosed())
    {
        ui->stackedWidget->setCurrentIndex(7);
    }
    else return;

    itsDisplayTimer->stop();
    itsBulletTimer->stop();
    itsCentipedeTimer->stop();
    itsPlayerTimer->stop();
    itsPowerUpMovementTimer->stop();
    itsRafaleTimer->stop();
    itsPiercingTimer->stop();
    itsSpiderAppearTimer->stop();
    itsSpiderTimer->stop();

    isGameStarted = false;
    this->update();
}

void Widget::pauseGame()
{
    itsDisplayTimer->stop();
    itsBulletTimer->stop();
    itsCentipedeTimer->stop();
    itsPlayerTimer->stop();
    itsPowerUpMovementTimer->stop();
    itsSpiderAppearTimer->stop();
    itsSpiderTimer->stop();

    isGamePaused = true;
    ui->stackedWidget->setCurrentIndex(4);
    this->update();
}

void Widget::resumeGame()
{
    itsDisplayTimer->start();
    itsBulletTimer->start();
    itsCentipedeTimer->start();
    itsPlayerTimer->start();
    itsPowerUpMovementTimer->start();
    itsSpiderAppearTimer->start();
    itsSpiderTimer->start();

    isGamePaused = false;
    ui->stackedWidget->setCurrentIndex(3);
}

void Widget::backToMenu()
{
    isGameStarted = false;
    isGamePaused = false;
    ui->stackedWidget->setCurrentIndex(0);
}

void Widget::displayLeaderboard()
{
    try
    {
        itsLeaderboard->extract();
        map<string,int> unsortedBestScores = itsLeaderboard->getItsBestScores();
        map<int,string> bestScores = {};
        for(pair<string,int> score : unsortedBestScores)
        {
            bestScores.insert({-score.second, score.first});
        }
        QLabel* labelList[10] = {ui->position_1, ui->position_2, ui->position_3, ui->position_4, ui->position_5, ui->position_6, ui->position_7, ui->position_8, ui->position_9, ui->position_10};
        QLabel* labelList2[10] = {ui->name_1, ui->name_2, ui->name_3, ui->name_4, ui->name_5, ui->name_6, ui->name_7, ui->name_8, ui->name_9, ui->name_10};
        map<int,string>::iterator it = bestScores.begin();
        for(int i = 0; i <= 10; i++)
        {
            if(it == bestScores.end()) break;
            labelList[i]->setText(QString::fromStdString(to_string(-it->first)));
            labelList2[i]->setText(QString::fromStdString(it->second));
            ++it;
        }
    }
    catch(string & e)
    {
        qDebug() << e;
    }
    ui->stackedWidget->setCurrentIndex(6);
}

void Widget::goToHowToPlay()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void Widget::moveCentipede()
{
    itsGame->moveCentipede();
}

void Widget::rafaleShot()
{
    if(remainingRafaleShots == 0)
    {
        itsGame->setIsRafaleActive(false);
        itsRafaleTimer->stop();
    }
    else
    {
        itsGame->shoot();
        remainingRafaleShots--;
    }
}

void Widget::piercingEnd()
{
    itsGame->setIsPiercingActive(false);
    itsPiercingTimer->stop();
}

void Widget::drawSpider(QPainter & painter)
{
    // Check if the spider exists
    if(itsGame->getItsSpider() != nullptr)
    {
        if(SHOW_HITBOXES)
        {
            // displays the spider hitbox
            painter.setPen(Qt::magenta);
            painter.setBrush(Qt::SolidPattern);
            painter.drawRect(itsGame->getItsSpider()->getItsHitBox());
        }
        else
        {
            // displays the spider image
            painter.drawImage(itsGame->getItsSpider()->getItsHitBox(), itsSpiderImg);
        }
    }
}

void Widget::spiderAppear()
{
    if (isGameStarted && !isGamePaused && itsGame->getItsSpider() == nullptr) {
        itsElapsedTime += 1; // Augmenter le temps écoulé de 1 seconde

        if (itsElapsedTime % INCREMENT_INTERVAL == 0) { //if ths elapsed time is a multiple of INCREMENT_INTERVAL
            // Increase spawn chance by 5% every 5 seconds
            if (itsSpiderAppearProbability <= 100) {
                itsSpiderAppearProbability += 5;; // The probability must not exceed 100%
            }
        }

        int chance = rand() % 101;// rand() % 100 is between 0 and 99 so rand() % 100 + 1 is between 0 and 100
        if (chance < itsSpiderAppearProbability)//if the chance is lower than the probability make the spider appear
        {
            itsGame->createSpider();
            itsSpiderTimer->start(4000 / itsGame->getItsBoard().width()); // start the timer for the moveSpider
            // Reset probability and elapsed time
            itsSpiderAppearTimer->stop();
            itsSpiderAppearProbability = 5;
            itsElapsedTime = 0;
        }
    }
}

void Widget::moveSpider()
{
    if(itsGame->getItsSpider() != nullptr)
    {
        itsGame->moveSpider();
    }
    else
    {
        //stop the timer of move and start the timer to make appear the spider
        itsSpiderTimer->stop();
        itsSpiderAppearTimer->start(1000);
    }
}

void Widget::processNewScore()
{
    qDebug() << "e";
    string username = ui->usernameLineEdit->text().toStdString();
    int score = itsGame->getItsScore();
    delete itsGame;

    try
    {
        if(username != "") itsLeaderboard->addScore(score, username);
    }
    catch(string & e)
    {
        qDebug() << e;
    }

    displayLeaderboard();
}
