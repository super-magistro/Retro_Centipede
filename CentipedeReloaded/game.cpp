#include <random>
#include "game.h"

using namespace std;

Game::Game(QRect board)
    :itsScore(0), itsCentipedes(new vector<Centipede*>), itsMushrooms(new vector<Mushroom*>), itsBullet(nullptr),
    itsPlayer(new Player({board.x() + board.width()/2 - PLAYER_SIZE/2, board.y() + board.height() - PLAYER_SIZE - 1})), itsBoard(board),
    itsPlayerZone(board.x(), board.y() + (4 * board.height()) / 5, board.width(), board.height() / 5)
{
    spawnCentipede();
    createMushrooms();
}

Game::~Game()
{
    // Deletion of centipedes' vector and each centipede inside
    for (vector<Centipede*>::iterator it = itsCentipedes->begin(); it < itsCentipedes->end(); it++)
    {
        delete *it;
    }
    delete itsCentipedes;

    // Deletion of mushrooms' vector and each mushroom inside
    for (vector<Mushroom*>::iterator it = itsMushrooms->begin(); it < itsMushrooms->end(); it++)
    {
        delete *it;
    }
    delete itsMushrooms;

    // Deletion of the bullet and the player
    delete itsBullet;
    delete itsPlayer;
}

void Game::spawnCentipede()
{
    Centipede * newCentipede = new Centipede();
    BodyPart * currentPart = newCentipede->getItsHead();
    currentPart->setItsPosition(CENTIPEDE_SPAWN_POSITION);
    newCentipede->setItsDirection({-1,1});
    for(int i = 0; i < CENTIPEDE_LENGTH; i++)
    {
        BodyPart * newPart = new BodyPart();
        Position newPos;
        newPos.posX = currentPart->getItsPosition().posX + CENTIPEDE_BODYPART_SIZE;
        newPos.posY = currentPart->getItsPosition().posY;
        newPart->setItsPosition(newPos);
        currentPart->setItsChild(newPart);
        newPart->setItsParent(currentPart);
        currentPart = newPart;
        newCentipede->setItsTail(currentPart);
    }
    itsCentipedes->push_back(newCentipede);
}

void Game::createMushrooms()
{
    random_device rd;
    default_random_engine eng(rd());

    uniform_int_distribution<int> randX(0, 30 - 1);
    uniform_int_distribution<int> randY(0, 31 - 1);

    while (itsMushrooms->size() < MUSHROOMS_AMOUNT)
    {
        // Generate a position
        int genX = itsBoard.x() + randX(eng) * (itsBoard.width() / 30);
        int genY = itsBoard.y() + randY(eng) * (itsBoard.height() / 31);

        bool validPos = true;

        // Check if a mushroom already exist at the same position
        for (vector<Mushroom*>::iterator it = itsMushrooms->begin(); it < itsMushrooms->end(); it++)
        {
            if ((*it)->getItsPosition().posX == genX && (*it)->getItsPosition().posY == genY)
            {
                validPos = false;
                break;
            }
        }
        if (!validPos) continue;

        // Simulate the mushroom hitbox for next checks
        QRect previewHitbox = QRect(genX, genY, MUSHROOM_SIZE, MUSHROOM_SIZE);

        // Check if the mushroom want to spawn on a centipede
        for (vector<Centipede*>::iterator it = itsCentipedes->begin(); it < itsCentipedes->end(); it++)
        {
            for (BodyPart* actualPart = (*it)->getItsHead(); actualPart != nullptr; actualPart = actualPart->getItsChild())
            {
                if (isColliding(previewHitbox, actualPart->getItsHitBox()))
                {
                    validPos = false;
                    break;
                }
            }
        }
        if (!validPos) continue;

        // Check if the mushroom want to spawn on the player
        if (isColliding(previewHitbox, itsPlayer->getItsHitBox())) continue;

        // Check if the mushroom want to spawn on the bullet
        if (itsBullet != nullptr && isColliding(previewHitbox, itsBullet->getItsHitBox())) continue;

        // Create the mushroom, must be executed only if the position is valid
        itsMushrooms->push_back(new Mushroom(genX, genY));
    }
}

void Game::shoot()
{
    if (itsBullet == nullptr)
    {
        int newX = itsPlayer->getItsPosition().posX /*+ PLAYER_SIZE / 2 - BULLET_SIZE / 2*/;
        int newY = itsPlayer->getItsPosition().posY /*+ PLAYER_SIZE / 2 - BULLET_SIZE / 2*/;
        itsBullet = new Bullet(newX, newY);
    }
}

void Game::moveBullet()
{
    itsBullet->updatePos();

    if(itsBullet->getItsPosition().posY < itsBoard.y())
    {
        itsBullet = nullptr;
    }
}

bool Game::isColliding(QRect hitbox1, QRect hitbox2)
{
    return hitbox1.intersects(hitbox2);
}

bool Game::centipedeMushroomCollision(Centipede * centipede)
{
    QRect centipedeHitBox = {centipede->getNextPosition(CENTIPEDE_SPEED).posX, centipede->getNextPosition(CENTIPEDE_SPEED).posY, CENTIPEDE_BODYPART_SIZE, CENTIPEDE_BODYPART_SIZE};
    for(vector<Mushroom*>::iterator it = getItsMushrooms()->begin(); it != itsMushrooms->end(); ++it)
    {
        Mushroom * mushroom = *it;
        int distance = 0;
        QRect mushroomHitBox = mushroom->getItsHitBox();
        if(isColliding(centipedeHitBox, mushroomHitBox))
        {
            if(centipede->getVerticalDirection())
            {
                itsMushrooms->erase(it);
                delete mushroom;
                return false;
            }
            else
            {
                distance = abs(mushroom->getItsPosition().posX - centipede->getItsHead()->getItsPosition().posX) - CENTIPEDE_BODYPART_SIZE;
                centipede->moveForward(distance);
                centipede->setVerticalDirection(true);
                centipede->moveForward(itsBoard.height()/31);
                centipede->setItsDirection({-centipede->getItsDirection().dirX, centipede->getItsDirection().dirY});
                return true;
            }
        }
    }
    return false;
}

bool Game::isGameWon()
{
    return itsCentipedes->size() <= 0;
}

bool Game::isGameLosed()
{
    return itsPlayer->getItsHp() <= 0;
}

void Game::checkCollisions()
{
    if (itsBullet != nullptr)
    {
        for (vector<Mushroom*>::iterator it = itsMushrooms->begin(); it < itsMushrooms->end(); it++) // checks if the bullet touches a mushroom
        {
            if (isColliding((*it)->getItsHitBox(), itsBullet->getItsHitBox()))
            {
                Mushroom * mushroom = *it;
                (mushroom)->damage();
                if (mushroom->getItsState() <= 0)
                {
                    itsScore += 4;
                    itsMushrooms->erase(it);
                    delete mushroom;
                }
                itsBullet = nullptr;
                break;
            }
        }
    }

    for (vector<Centipede*>::iterator it = itsCentipedes->begin(); it != itsCentipedes->end(); ++it)
    {
        Centipede * centipede = *it;
        for (BodyPart* centiPart = centipede->getItsHead(); centiPart != nullptr; centiPart = centiPart->getItsChild())
        {
            if (isColliding(centiPart->getItsHitBox(), itsPlayer->getItsHitBox())) // player touched
            {
                itsPlayer->hit();
                for (vector<Centipede*>::iterator itDel = itsCentipedes->begin(); itDel != itsCentipedes->end(); ++itDel)
                {
                    delete *itDel;
                }
                itsCentipedes->clear();
                itsPlayer->setItsPosition({ itsBoard.x() + itsBoard.width()/2 - PLAYER_SIZE/2, itsBoard.y() + itsBoard.height() - PLAYER_SIZE - 2});
                spawnCentipede();
                return;
            }

            else if (itsBullet != nullptr && isColliding(centiPart->getItsHitBox(), itsBullet->getItsHitBox())) // centipede hit
            {
                BodyPart * newTail = centiPart->getItsParent();
                sliceCentipede(centiPart);
                centipede->setItsTail(newTail);
                itsBullet = nullptr;
                return;
            }
        }
    }
}

void Game::sliceCentipede(BodyPart* hittedPart)
{
    if (hittedPart->getItsParent() != nullptr) // body hit
    {
        // Cut the hitted centipede
        hittedPart->getItsParent()->setItsChild(nullptr);

        // Check if the hitted part is NOT the tail
        if (hittedPart->getItsChild() != nullptr)
        {
            // Set next part as the head for the new centipede ...
            BodyPart* newHead = hittedPart->getItsChild();
            hittedPart->setItsChild(nullptr);
            // ... and seperate it from the hitted part
            newHead->setItsParent(nullptr);
            BodyPart* newTail = newHead;
            // Search the tail of the centipede
            while(newHead->getItsChild() != nullptr)
            {
                newHead = newHead->getItsChild();
            }

            BodyPart * reversedHead = new BodyPart();
            BodyPart * currentParent = reversedHead;
            BodyPart * currentPart = newHead->getItsParent();
            while(currentPart != newTail) // we reverse the chained list
            {
                currentParent->setItsChild(currentPart);
                currentPart->setItsParent(currentParent);
                currentParent = currentPart;
            }
            // Create a new centipede with the tail as the head
            Centipede * newCentipede = new Centipede(reversedHead);
            newCentipede->setItsTail(newTail);
            itsCentipedes->push_back(newCentipede);
        }

        // Deletion of the hitted part
        delete hittedPart;
    }
    else // headshot
    {
        for (vector<Centipede*>::iterator it = itsCentipedes->begin(); it != itsCentipedes->end(); ++it)
        {
            Centipede * centipede = *it;
            if (centipede->getItsHead() == hittedPart)
            {
                itsCentipedes->erase(it);
                delete centipede;
                break;
            }
        }
    }
}

std::vector<Centipede*>* Game::getItsCentipedes()
{
    return itsCentipedes;
}

std::vector<Mushroom*>* Game::getItsMushrooms()
{
    return itsMushrooms;
}

Bullet* Game::getItsBullet()
{
    return itsBullet;
}

Player* Game::getItsPlayer()
{
    return itsPlayer;
}

int Game::getItsScore()
{
    return itsScore;
}

void Game::movePlayer(Direction & direction)
{
    if (itsPlayerZone.x() < itsPlayer->getItsHitBox().x() + direction.dirX * PLAYER_SPEED &&
        itsPlayerZone.x() + itsPlayerZone.width() > itsPlayer->getItsHitBox().x() + itsPlayer->getItsHitBox().width() + direction.dirX * PLAYER_SPEED &&
        itsPlayerZone.y() < itsPlayer->getItsHitBox().y() + direction.dirY * PLAYER_SPEED &&
        itsPlayerZone.y() + itsPlayerZone.height() > itsPlayer->getItsHitBox().y() + itsPlayer->getItsHitBox().height() + direction.dirY * PLAYER_SPEED)
    {
        itsPlayer->updatePos(direction);
    }
}

void Game::moveCentipede()
{
    QRect zone = itsBoard;
    for(Centipede * centipede : *itsCentipedes)
    {
        //centipede->moveForward(CENTIPEDE_SPEED);
        //return;


        if(centipede->hasReachedBottom()) zone = itsPlayerZone;
        bool first = centipedeMushroomCollision(centipede);
        bool second = centipedeBoardCollision(centipede, zone);
        if(!(first || second)) // if there's no collision
        {
            if(centipede->getVerticalDirection())
            {
                centipede->moveForward(itsBoard.height()/31);
            }
            else
            {
                centipede->moveForward(CENTIPEDE_SPEED);
            }
        }
    }
}

bool Game::centipedeBoardCollision(Centipede * centipede, QRect board)
{
    QRect centipedeHitBox;
    if(centipede->getVerticalDirection())
    {
        centipedeHitBox = {centipede->getNextPosition(itsBoard.height()/31).posX, centipede->getNextPosition(itsBoard.height()/31).posY, CENTIPEDE_BODYPART_SIZE, CENTIPEDE_BODYPART_SIZE};
    }
    else
    {
        centipedeHitBox = {centipede->getNextPosition(CENTIPEDE_SPEED).posX, centipede->getNextPosition(CENTIPEDE_SPEED).posY, CENTIPEDE_BODYPART_SIZE, CENTIPEDE_BODYPART_SIZE};
    }

    if(centipedeHitBox.x() <= board.x()) // left side of the board
    {
        Position newPos = {board.x() + 1, centipede->getItsHead()->getItsPosition().posY};
        centipede->getItsHead()->setItsPosition(newPos);

        centipede->setItsDirection({1, centipede->getItsDirection().dirY});
        centipede->setVerticalDirection(true);
        return true;
    }
    else if(centipedeHitBox.x() + centipedeHitBox.width() >= board.x() + board.width()) // right side of the board
    {
        Position newPos = {board.width() + board.x() - CENTIPEDE_BODYPART_SIZE - 1, centipede->getItsHead()->getItsPosition().posY};
        centipede->getItsHead()->setItsPosition(newPos);

        centipede->setItsDirection({-1, centipede->getItsDirection().dirY});
        centipede->setVerticalDirection(true);
        return true;
    }
    if(centipedeHitBox.y() <= board.y()) // top of the board
    {
        Position newPos = {centipede->getItsHead()->getItsPosition().posX, board.y() + 1};
        centipede->getItsHead()->setItsPosition(newPos);

        centipede->setItsDirection({centipede->getItsDirection().dirX, 1});
        centipede->setVerticalDirection(false);
        return true;
    }
    else if(centipedeHitBox.y() + centipedeHitBox.height() >= board.y() + board.height()) // bottom of the board
    {
        centipede->setHasReachedBottom(true);
        Position newPos = {centipede->getItsHead()->getItsPosition().posX, board.y() + board.height() - CENTIPEDE_BODYPART_SIZE - 1};
        centipede->getItsHead()->setItsPosition(newPos);

        centipede->setItsDirection({centipede->getItsDirection().dirX, -1});
        centipede->setVerticalDirection(false);
        return true;
    }
    return false;
}
