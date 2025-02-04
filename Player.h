#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"

class Player :
    public GameObject
{
    int playerImage_;
    Point pos_;
public:
	Player();
    ~Player();
    void Update() override;
    void Draw() override;
	bool CheckHit(const Rect& me,const Rect& other);
};

