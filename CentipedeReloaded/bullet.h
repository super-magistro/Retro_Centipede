#include <QRect>
#include "typeDef.h"

#ifndef BULLET_H
#define BULLET_H

class Bullet
{
private:
    QRect itsHitBox;
    Position itsPosition;
public:
    Bullet(int x, int y);
    ~Bullet();
    void updatePos();
    QRect getItsHitBox();
    Position getItsPosition();
};

#endif // BULLET_H
