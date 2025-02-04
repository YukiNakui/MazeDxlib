#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"

Enemy::Enemy()
    :pos_({ 0,0 }), isAlive_(true)
{
    int rx = GetRand(STAGE_WIDTH * CHA_WIDTH);
    int ry = GetRand(STAGE_HEIGHT * CHA_HEIGHT);
    pos_ = { rx, ry };
    dir_ = DIR::RIGHT;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	switch (dir_) {
	case DIR::RIGHT:
		pos_.x++;
		break;
	case DIR::LEFT:
		pos_.x--;
		break;
	case DIR::UP:
		pos_.y--;
		break;
	case DIR::DOWN:
		pos_.y++;
		break;
	case DIR::NONE:
		break;
	default:
		break;
	}

	Stage* stage = (Stage*)FindGameObject<Stage>();
    Rect enemyRect = { pos_.x,pos_.y,CHA_WIDTH,CHA_HEIGHT };
	
	for (auto& obj : stage->GetStageRects())
	{
		if (!CheckHit(enemyRect, obj))
		{
			Rect tmpRectR = { pos_.x + CHA_WIDTH, pos_.y, CHA_WIDTH, CHA_HEIGHT };
			Rect tmpRectL = { pos_.x - CHA_WIDTH, pos_.y, CHA_WIDTH, CHA_HEIGHT };
			Rect tmpRectU = { pos_.x, pos_.y - CHA_HEIGHT, CHA_WIDTH, CHA_HEIGHT };
			Rect tmpRectD = { pos_.x, pos_.y + CHA_HEIGHT, CHA_WIDTH, CHA_HEIGHT };
			int hitObjCount = 0;
			if (CheckHit(tmpRectR, obj))
				hitObjCount++;
			if (CheckHit(tmpRectL, obj))
				hitObjCount++;
			if (CheckHit(tmpRectU, obj))
				hitObjCount++;
			if (CheckHit(tmpRectD, obj))
				hitObjCount++;

			if (hitObjCount <= 1)
				dir_ = SetRandomDir();
		}
		else {
			Point centerMe = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
			Point centerObj = obj.GetCenter();

			int meR = centerMe.x + CHA_WIDTH / 2;//敵右端
			int meL = centerMe.x - CHA_WIDTH / 2;//敵左端
			int meU = centerMe.y - CHA_HEIGHT / 2;//敵上端
			int meD = centerMe.y + CHA_HEIGHT / 2;//敵下端

			int objR = centerObj.x + CHA_WIDTH / 2;//壁右端
			int objL = centerObj.x - CHA_WIDTH / 2;//壁左端
			int objU = centerObj.y - CHA_HEIGHT / 2;//壁上端
			int objD = centerObj.y + CHA_HEIGHT / 2;//壁下端

			if ((meR - objL) > 0 && dir_ == DIR::RIGHT) {//右に当たってる
				pos_.x -= (meR - objL);
				dir_ = SetRandomDir();
			}
			else if ((objR - meL) > 0 && dir_ == DIR::LEFT) {//左
				pos_.x += (objR - meL);
				dir_ = SetRandomDir();
			}
			else if ((meD - objU) > 0 && dir_ == DIR::DOWN) {//下
				pos_.y -= (meD - objU);
				dir_ = SetRandomDir();
			}
			else if ((objD - meU) > 0 && dir_ == DIR::UP) {//上
				pos_.y += (objD - meU);
				dir_ = SetRandomDir();
			}
		}
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

	DrawTriangle(tp[dir_][0].x, tp[dir_][0].y, tp[dir_][1].x, tp[dir_][1].y, tp[dir_][2].x, tp[dir_][2].y, GetColor(255, 255, 255), TRUE);
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

DIR Enemy::SetRandomDir()
{
	int randDir = GetRand(3);
	switch (randDir) {
	case DIR::UP:
		return DIR::UP;
		break;
	case DIR::DOWN:
		return DIR::DOWN;
		break;
	case DIR::RIGHT:
		return DIR::RIGHT;
		break;
	case DIR::LEFT:
		return DIR::LEFT;
		break;
	default:
		return DIR::NONE;
		break;
	}
}
