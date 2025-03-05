#include "Enemy.h"
#include "globals.h"
#include"Player.h"

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <map> 
#include <stack>
#include <string>

#include "ImGui/imgui.h"

namespace
{
    Point nDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
}

//A*法
namespace AStar {

    struct Node {
        int x, y;
        int g, h, f;
        Node* parent;
        Node(int _x, int _y, int _g, int _h, Node* _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
    };

    //マンハッタン距離
    int Manhattan(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }


    Node* FindNode(const std::vector<Node*>& list, int x, int y) {
        for (Node* node : list) {
            if (node->x == x && node->y == y)
                return node;
        }
        return nullptr;
    }

    //startからgoalまでの経路
    std::vector<Point> FindPath(Stage* stage, Point start, Point goal) {
        std::vector<Point> path;
        std::vector<Node*> openList;
        std::vector<Node*> closedList;

        Node* startNode = new Node(start.x, start.y, 0, Manhattan(start.x, start.y, goal.x, goal.y), nullptr);
        openList.push_back(startNode);

        bool pathFound = false;
        Node* goalNode = nullptr;

        while (!openList.empty()) {
            auto currentIt = std::min_element(openList.begin(), openList.end(),
                [](Node* a, Node* b) { return a->f < b->f; });
            Node* current = *currentIt;

            if (current->x == goal.x && current->y == goal.y) {
                pathFound = true;
                goalNode = current;
                break;
            }

            openList.erase(currentIt);
            closedList.push_back(current);

            //上下左右
            const int dx[4] = { -1, 1, 0, 0 };
            const int dy[4] = { 0, 0, -1, 1 };
            for (int i = 0; i < 4; i++) {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];

                if (nx < 0 || ny < 0 || nx >= STAGE_WIDTH || ny >= STAGE_HEIGHT)
                    continue;
                if (!stage->IsPassable(nx, ny))
                    continue;
                if (FindNode(closedList, nx, ny))
                    continue;
                int tentativeG = current->g + 1;
                Node* neighbor = FindNode(openList, nx, ny);
                if (!neighbor) {
                    int h = Manhattan(nx, ny, goal.x, goal.y);
                    neighbor = new Node(nx, ny, tentativeG, h, current);
                    openList.push_back(neighbor);
                }
                else {
                    if (tentativeG < neighbor->g) {
                        neighbor->g = tentativeG;
                        neighbor->f = neighbor->g + neighbor->h;
                        neighbor->parent = current;
                    }
                }
            }
        }

        if (pathFound && goalNode) {
            Node* current = goalNode;
            while (current) {
                path.push_back({ current->x, current->y });
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
        }

        for (Node* node : openList)
            delete node;
        for (Node* node : closedList)
            delete node;

        return path;
    }
}

Enemy::Enemy()
    :pos_({ 0,0 })
{
    int rx = 0;
    int ry = 0;
    while (rx % 2 == 0 || ry % 2 == 0)
    {
        rx = GetRand(STAGE_WIDTH - 1);
        ry = GetRand(STAGE_HEIGHT - 1);
    }
    pos_ = { rx * CHA_WIDTH, ry * CHA_HEIGHT };
    forward_ = DIR::RIGHT;
    path_.clear();
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
    Player* player = (Player*)FindGameObject<Player>();
    Point op = pos_;
    Point move = { nDir[forward_].x, nDir[forward_].y };
    Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };
    Stage* stage = (Stage*)FindGameObject<Stage>();
    pos_ = { pos_.x + move.x, pos_.y + move.y };
    Point enemyPos = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
    Point playerPos = { player->GetPosition().x / CHA_WIDTH, player->GetPosition().y / CHA_HEIGHT };

    for (auto& obj : stage->GetStageRects())
    {
        if (CheckHit(eRect, obj)) {
            Rect tmpRectX = { op.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect tmpRecty = { pos_.x, op.y, CHA_WIDTH, CHA_HEIGHT };
            if (!CheckHit(tmpRectX, obj))
            {
                pos_.x = op.x;
            }
            else if (!CheckHit(tmpRecty, obj))
            {
                pos_.y = op.y;
            }
            else
            {
                pos_ = op;
            }
            forward_ = (DIR)(GetRand(3));
            break;
        }
    }

    switch (chaseMode_) {
    case ENEMYMODE::RANDOM: {
        int prgssx = pos_.x % (CHA_WIDTH);
        int prgssy = pos_.y % (CHA_HEIGHT);
        int cx = (pos_.x / (CHA_WIDTH)) % 2;
        int cy = (pos_.y / (CHA_HEIGHT)) % 2;
        if (prgssx == 0 && prgssy == 0 && cx && cy)
            RandomMove();
        break;
    }
    case ENEMYMODE::RIGHT_HAND: {
        RightHandMove();
        break;
    }
    case ENEMYMODE::LEFT_HAND: {
        LeftHandMove();
        break;
    }
    case ENEMYMODE::BFS: {
        if (path_.empty()) {
            path_ = BFSFindPath(stage, enemyPos, playerPos);
        }

        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin());
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }
    case ENEMYMODE::DFS: {
        if (path_.empty()) {
            path_ = DFSFindPath(stage, enemyPos, playerPos);
        }

        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin());
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }
    case ENEMYMODE::DIJKSTRA: {
        //ダイクストラ法で経路を計算
        if (path_.empty()) {
            path_ = DijkstraFindPath(stage, enemyPos, playerPos);
        }

        //経路がある場合、次のマスへ移動
        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin()); //先頭を削除して次の地点へ
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }
    case ENEMYMODE::ASTAR: {
        if (path_.empty()) {
            path_ = AStar::FindPath(stage,enemyPos, playerPos);
        }

        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin());
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }
    default:
        break;
    }
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
    Point tp[4][3] = {
        {{pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
        {{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}, {pos_.x, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}},
        {{pos_.x            , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}},
        {{pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y}, {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT}}
    };

    DrawTriangle(tp[forward_][0].x, tp[forward_][0].y, tp[forward_][1].x, tp[forward_][1].y, tp[forward_][2].x, tp[forward_][2].y, GetColor(255, 255, 255), TRUE);
    DisplayImgui();
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    if (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y)
    {
        return true;
    }
    return false;
}

//進行方向が壁か確認
bool Enemy::IsWallBlocked(DIR dir) {
    Point nextPos = { pos_.x + nDir[dir].x, pos_.y + nDir[dir].y };
    Rect nextRect{ nextPos.x, nextPos.y, CHA_WIDTH, CHA_HEIGHT };
    Stage* stage = (Stage*)FindGameObject<Stage>();

    for (auto& obj : stage->GetStageRects()) {
        if (CheckHit(nextRect, obj)) {
            return true;
        }
    }
    return false;
}

void Enemy::RandomMove()
{
    Player* pPlayer = (Player*)FindGameObject<Player>();
    Point playerPos = pPlayer->GetPosition();
    int distX = abs(pos_.x - playerPos.x);
    int distY = abs(pos_.y - playerPos.y);
    int randNum = 3;
    int rand = GetRand(randNum);
    if (rand % randNum == 0) {
        //プレイヤーとの距離に応じて進行方向を決定
        if (distX > distY) {
            if (playerPos.x < pos_.x) {
                forward_ = DIR::LEFT;
            }
            else if (playerPos.x > pos_.x) {
                forward_ = DIR::RIGHT;
            }
        }
        else {
            if (playerPos.y < pos_.y) {
                forward_ = DIR::UP;
            }
            else if (playerPos.y > pos_.y) {
                forward_ = DIR::DOWN;
            }
        }
    }
    else if (rand % randNum == 1) {
        forward_ = (DIR)(GetRand(3));
    }

    //ランダムに決めた方向が壁の場合、壁でない方向に変更
    while (IsWallBlocked(forward_)) {
        forward_ = (DIR)(GetRand(4));
    }
}

void Enemy::RightHandMove()
{
    DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
    DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };
    Point nposF = { pos_.x + nDir[forward_].x, pos_.y + nDir[forward_].y };
    Point nposR = { pos_.x + nDir[myRight[forward_]].x, pos_.y + nDir[myRight[forward_]].y };
    Rect myRectF{ nposF.x, nposF.y, CHA_WIDTH, CHA_HEIGHT };
    Rect myRectR{ nposR.x, nposR.y, CHA_WIDTH, CHA_HEIGHT };
    Stage* stage = (Stage*)FindGameObject<Stage>();
    bool isRightOpen = true;
    bool isForwardOpen = true;
    for (auto& obj : stage->GetStageRects()) {
        if (CheckHit(myRectF, obj)) {
            isForwardOpen = false;
        }
        if (CheckHit(myRectR, obj)) {
            isRightOpen = false;
        }
    }
    if (isRightOpen)
    {
        forward_ = myRight[forward_];
    }
    else if (isRightOpen == false && isForwardOpen == false)
    {
        forward_ = myLeft[forward_];
    }
}

void Enemy::LeftHandMove()
{
    DIR myRight[4] = { RIGHT, LEFT, UP, DOWN };
    DIR myLeft[4] = { LEFT, RIGHT, DOWN, UP };
    Point nposF = { pos_.x + nDir[forward_].x, pos_.y + nDir[forward_].y };
    Point nposL = { pos_.x + nDir[myLeft[forward_]].x, pos_.y + nDir[myLeft[forward_]].y };
    Rect myRectF{ nposF.x, nposF.y, CHA_WIDTH, CHA_HEIGHT };
    Rect myRectL{ nposL.x, nposL.y, CHA_WIDTH, CHA_HEIGHT };
    Stage* stage = (Stage*)FindGameObject<Stage>();
    bool isLeftOpen = true;
    bool isForwardOpen = true;
    for (auto& obj : stage->GetStageRects()) {
        if (CheckHit(myRectF, obj)) {
            isForwardOpen = false;
        }
        if (CheckHit(myRectL, obj)) {
            isLeftOpen = false;
        }
    }
    if (isLeftOpen)
    {
        forward_ = myLeft[forward_];
    }
    else if (isLeftOpen == false && isForwardOpen == false)
    {
        forward_ = myRight[forward_];
    }
}

std::vector<Point> Enemy::BFSFindPath(Stage* stage, Point start, Point goal)
{
    std::vector<Point> path;
    if (!stage) return path;

    std::queue<Point> queue;
    std::map<Point, Point> cameFrom;  //経路復元用
    queue.push(start);
    cameFrom[start] = start;  //開始地点の親を自身に設定

    Point directions[] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}  // 上, 右, 下, 左
    };

    while (!queue.empty()) {
        Point current = queue.front();
        queue.pop();

        //ゴールに到達
        if (current.x == goal.x && current.y == goal.y) {
            break;
        }

        for (const auto& dir : directions) {
            Point next = { current.x + dir.x, current.y + dir.y };

            //範囲外または通行不可ならスキップ
            if (!stage->IsPassable(next.x, next.y)) continue;

            //まだ通過していない場合
            if (cameFrom.find(next) == cameFrom.end()) {
                queue.push(next);
                cameFrom[next] = current;
            }
        }
    }

    //経路を逆順でたどる
    if (cameFrom.find(goal) != cameFrom.end()) {
        Point current = goal;
        while (current.x != start.x || current.y != start.y) {
            path.push_back(current);
            current = cameFrom[current];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }

    return path;
}

std::vector<Point> Enemy::DFSFindPath(Stage* stage, Point start, Point goal)
{
    std::vector<Point> path;
    if (!stage) return path;

    std::stack<Point> stack;
    std::map<Point, Point> cameFrom; //経路復元用
    stack.push(start);
    cameFrom[start] = start; //開始地点の親を自身に設定

    Point directions[] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}  //上,右,下,左
    };

    while (!stack.empty()) {
        Point current = stack.top();
        stack.pop();

        //ゴールに到達
        if (current.x == goal.x && current.y == goal.y) {
            break;
        }

        for (const auto& dir : directions) {
            Point next = { current.x + dir.x, current.y + dir.y };

            //範囲外または通行不可ならスキップ
            if (!stage->IsPassable(next.x, next.y)) continue;

            //まだ通過していない場合
            if (cameFrom.find(next) == cameFrom.end()) {
                stack.push(next);
                cameFrom[next] = current;
            }
        }
    }

    //経路を逆順でたどる
    if (cameFrom.find(goal) != cameFrom.end()) {
        Point current = goal;
        while (current.x != start.x || current.y != start.y) {
            path.push_back(current);
            current = cameFrom[current];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }
    return path;
}

std::vector<Point> Enemy::DijkstraFindPath(Stage* stage, Point start, Point goal)
{
    //優先度付きキュー用の比較関数（距離が小さい順に処理）
    struct Compare {
        bool operator()(const std::pair<int, Point>& a, const std::pair<int, Point>& b) {
            return a.first > b.first;
        }
    };

    std::vector<Point> path;
    if (!stage) return path;

    //各座標の最短距離を格納するマップ
    std::map<Point, int> cost;
    std::map<Point, Point> cameFrom;  //経路復元用
    std::priority_queue<std::pair<int, Point>, std::vector<std::pair<int, Point>>, Compare> pq;

    //初期設定
    pq.push({ 0, start });
    cost[start] = 0;
    cameFrom[start] = start;

    Point directions[] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}  //上,右,下,左
    };

    while (!pq.empty()) {
        auto [currentCost, current] = pq.top();
        pq.pop();

        //ゴールに到達
        if (current.x == goal.x && current.y == goal.y) {
            break;
        }

        //隣接するマスをチェック
        for (const auto& dir : directions) {
            Point next = { current.x + dir.x, current.y + dir.y };

            if (!stage->IsPassable(next.x, next.y)) continue;

            //コストを計算
            int newCost = cost[current] + 1;

            //より短い距離が見つかった場合、更新
            if (cost.find(next) == cost.end() || newCost < cost[next]) {
                cost[next] = newCost;
                pq.push({ newCost, next });
                cameFrom[next] = current;
            }
        }
    }

    //経路を逆順でたどる
    if (cameFrom.find(goal) != cameFrom.end()) {
        Point current = goal;
        while (current.x != start.x || current.y != start.y) {
            path.push_back(current);
            current = cameFrom[current];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }

    return path;
}


void Enemy::DisplayImgui()
{
    ImGui::Begin(u8"Enemyモード切り替え");
    ImGui::Text(u8"Enemyのモードを選んでください");

    //現在のモード名を取得
    const char* modeNames[] = { u8"ランダム", u8"右手法", u8"左手法", u8"幅優先探索", u8"深さ優先探索", u8"ダイクストラ法", u8"A*" };
    const char* currentMode = modeNames[static_cast<int>(chaseMode_)];

    //Playerとの距離を計算
    Player* player = (Player*)FindGameObject<Player>();
    if (player) {
        Point playerPos = player->GetPosition();
        int distanceX = abs(pos_.x - playerPos.x);
        int distanceY = abs(pos_.y - playerPos.y);
        int distanceThreshold = CHA_WIDTH / 2;

        //追いついたかどうかの判定
        if (distanceX < distanceThreshold && distanceY < distanceThreshold) {
            ImGui::Text(u8"現在のEnemyの状態は、Playerに追いつきました！");
        }
        else {
            ImGui::Text(u8"現在のEnemyの状態は、%s です", currentMode);
        }
    }
    else {
        ImGui::Text(u8"現在のEnemyの状態は、%s です", currentMode);
    }

    Stage* stage = (Stage*)FindGameObject<Stage>();
    if (!stage) {
        ImGui::Text(u8"ステージ情報が取得できません");
        ImGui::End();
        return;
    }

    //モード変更
    for (int i = 0; i < MAX_ENEMYMODE; i++) {
        if (ImGui::Button(modeNames[i])) {
            chaseMode_ = (ENEMYMODE)i;
            pos_ = stage->GetRandomEmptyPosition(); //リスポーン
            path_.clear();
            pathIndex_ = 0;
        }
    }

    ImGui::End();
}