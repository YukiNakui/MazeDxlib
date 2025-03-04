#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include "./Stage.h"

class Enemy :
    public GameObject
{
    Point pos_;
    bool isAlive_;
    float speed_;
    DIR forward_;//進行方向
    Stage* stage_;


    Point targetPos_;

    //A*の経路
    std::vector<Point> path_;
    int pathIndex_;

    enum ENEMYMODE {
        RANDOM,
        RIGHT_HAND,
        LEFT_HAND,
        BFS,
        DFS,
        DIJKSTRA,
        ASTAR,
        MAX_ENEMYMODE
    };

    ENEMYMODE chaseMode_;
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);

    void DisplayImgui();
};

