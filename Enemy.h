#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"

class Enemy :
    public GameObject
{
    Point pos_;
    bool isAlive_;
    float speed_;
    DIR forward_;//進行方向
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    
    void XCloserMove();
    void YCloserMove();
    void XYCloserMove();
    void XYCloserMoveRandom();//ランダム
    void RightHandMove();     //右手法
	void LeftHandMove();      //左手法
};

