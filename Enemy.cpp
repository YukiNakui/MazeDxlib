#include "Enemy.h"
#include "globals.h"
#include"Player.h"

namespace
{
	Point nDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
}

Enemy::Enemy()
    :pos_({ 0,0 }), isAlive_(true)
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
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	AStarMove();

	//static bool stop = false;

	/*if (!stop) {
		Point op = pos_;
		Point move = { nDir[forward_].x, nDir[forward_].y };
		Rect eRect = { pos_.x, pos_.y,CHA_WIDTH, CHA_HEIGHT };
		stage_ = (Stage*)FindGameObject<Stage>();
		pos_ = { pos_.x + move.x, pos_.y + move.y };
		for (auto& obj : stage_->GetStageRects())
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
	}*/

	/*int prgssx = pos_.x % (CHA_WIDTH);
	int prgssy = pos_.y % (CHA_HEIGHT);
	int cx = (pos_.x / (CHA_WIDTH)) % 2;
	int cy = (pos_.y / (CHA_HEIGHT)) % 2;
	if (prgssx == 0 && prgssy == 0 && cx && cy)
	{
		XYCloserMoveRandom();
	}*/
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

void Enemy::XCloserMove()
{
	Player* pPlayer = (Player*)FindGameObject<Player>();
	Point playerPos = pPlayer->GetPosition();
	if (playerPos.x < pos_.x) {
		forward_ = DIR::LEFT;
	}
	else if(playerPos.x > pos_.x) {
		forward_ = DIR::RIGHT;
	}
}

void Enemy::YCloserMove()
{
	Player* pPlayer = (Player*)FindGameObject<Player>();
	Point playerPos = pPlayer->GetPosition();
	if (playerPos.y < pos_.y) {
		forward_ = DIR::UP;
	}
	else if (playerPos.y > pos_.y) {
		forward_ = DIR::DOWN;
	}
}

void Enemy::XYCloserMove()
{
	Player* pPlayer = (Player*)FindGameObject<Player>();
	Point playerPos = pPlayer->GetPosition();
	int distX = abs(pos_.x - playerPos.x);
	int distY = abs(pos_.y - playerPos.y);
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

void Enemy::XYCloserMoveRandom()
{
	Player* pPlayer = (Player*)FindGameObject<Player>();
	Point playerPos = pPlayer->GetPosition();
	int distX = abs(pos_.x - playerPos.x);
	int distY = abs(pos_.y - playerPos.y);
	int randNum = 3;
	int rand = GetRand(randNum);
	if (rand % randNum == 0) {
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
	else if(rand % randNum == 1){
		forward_ = (DIR)(GetRand(3));
	}
}

void Enemy::AStarMove()
{
	Player* pPlayer = (Player*)FindGameObject<Player>();
	Point playerPos = pPlayer->GetPosition();
	std::vector<std::vector<bool>> visited(STAGE_HEIGHT, std::vector<bool>(STAGE_WIDTH, false));
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList; // 優先度付きキュー
	std::vector<std::vector<Node>> cameFrom(STAGE_HEIGHT, std::vector<Node>(STAGE_WIDTH));

	Node start = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT, 0.0f, Heuristic(pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT, playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT) };
	openList.push(start);

	while (!openList.empty()) {
		Node current = openList.top();
		openList.pop();

		// 目標位置に到達した場合、経路を辿る
		if (current.x == playerPos.x / CHA_WIDTH && current.y == playerPos.y / CHA_HEIGHT) {
			RetracePath(cameFrom, current);
			return;
		}

		visited[current.y][current.x] = true;

		// 隣接ノードを評価
		for (int i = 0; i < 4; i++) {
			int nx = current.x + nDir[i].x;
			int ny = current.y + nDir[i].y;

			STAGE_OBJ stageObj = stage_->GetStageData(ny, nx);
			// 範囲外や壁には進めない
			if (nx < 0 || ny < 0 || nx >= STAGE_WIDTH || ny >= STAGE_HEIGHT || stageObj == STAGE_OBJ::WALL || visited[ny][nx])
				continue;

			float gCost = current.gCost + 1.0f;
			float hCost = Heuristic(nx, ny, playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT);
			Node neighbor = { nx, ny, gCost, hCost };

			if (!visited[ny][nx]) {
				openList.push(neighbor);
				cameFrom[ny][nx] = current;
			}
		}
	}
}

void Enemy::RetracePath(std::vector<std::vector<Node>>& cameFrom, Node current)
{
	while (cameFrom[current.y][current.x].x != current.x || cameFrom[current.y][current.x].y != current.y) {
		current = cameFrom[current.y][current.x];
		pos_ = { current.x * CHA_WIDTH, current.y * CHA_HEIGHT };
	}
}
