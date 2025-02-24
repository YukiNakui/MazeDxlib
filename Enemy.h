#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include "./Stage.h"
#include <queue>
#include <vector>
#include <cmath>

struct Node {
    int x, y;
    float gCost; // 移動コスト
    float hCost; // ヒューリスティックコスト
    float fCost() const { return gCost + hCost; } // 総コスト

    bool operator>(const Node& other) const {
        return fCost() > other.fCost();
    }
};

class Enemy :
    public GameObject
{
    Point pos_;
    bool isAlive_;
    float speed_;
    DIR forward_;//進行方向
    Stage* stage_;
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    
    void XCloserMove();
    void YCloserMove();
    void XYCloserMove();
    void XYCloserMoveRandom();
    void AStarMove();
    // ヒューリスティック関数 (プレイヤーとの距離を計算)
    float Heuristic(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2); // マンハッタン距離
    }
    void RetracePath(std::vector<std::vector<Node>>& cameFrom, Node current);
};

