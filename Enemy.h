#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include "./Stage.h"

class Enemy :
    public GameObject
{
    Point pos_;
    DIR forward_;//�i�s����
    Stage* stage_;
    Point targetPos_;

    std::vector<Point> path_;
    int pathIndex_;
    
    struct Node {
        int x, y;
        int g, h, f;
        Node* parent;
        Node(int _x, int _y, int _g, int _h, Node* _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
    };

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
    bool IsWallBlocked(DIR dir);
    void RandomMove();        //�����_��
    void RightHandMove();     //�E��@
    void LeftHandMove();      //����@

    //�o�H�T���p
    std::vector<Point> BFSFindPath(Stage* stage, Point start, Point goal);
    std::vector<Point> DFSFindPath(Stage* stage, Point start, Point goal);
    std::vector<Point> DijkstraFindPath(Stage* stage, Point start, Point goal);
    void DisplayImgui();//Imgui�\��
};

