#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"


class Enemy :
    public GameObject
{
    Point pos_;
    bool isAlive_;
    float speed_;
    DIR dir_;
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    DIR SetRandomDir();
};

