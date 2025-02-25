#include "Stage.h"
#include "./globals.h"
#include<stack>
#include<time.h>

namespace {
	std::stack<Point> prStack;

	Point Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };

	void DigDug(int x, int y, vector<vector<STAGE_OBJ>>& _stage)
	{
		_stage[y][x] = STAGE_OBJ::EMPTY;
		std::vector<int> dList;
		for (int i = 0; i < 4; i++) {
			//nextを0~3まで回してでたーを取得
			Point next = Point{ x + Dir[i].x, y + Dir[i].y };
			Point nextNext = { next.x + Dir[i].x, next.y + Dir[i].y };
			if (nextNext.x < 0 || nextNext.y < 0 || nextNext.x > STAGE_WIDTH - 1 || nextNext.y > STAGE_HEIGHT - 1)
				continue;

			if (_stage[nextNext.y][nextNext.x] == STAGE_OBJ::WALL)
			{
				dList.push_back(i);
			}
		}
		if (dList.empty())
		{
			return;
		}
		int nrand = rand() % dList.size();
		int tmp = dList[nrand];

		Point next = { x + Dir[tmp].x, y + Dir[tmp].y };
		Point nextNext = { next.x + Dir[tmp].x, next.y + Dir[tmp].y };

		_stage[next.y][next.x] = STAGE_OBJ::EMPTY;
		_stage[nextNext.y][nextNext.x] = STAGE_OBJ::EMPTY;

		prStack.push(nextNext);
		DigDug(nextNext.x, nextNext.y, _stage);
	}

	void AllWall(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++) {
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::EMPTY;
				else
					_stage[j][i] = STAGE_OBJ::WALL;
			}
		}
	}

	void MakeMazeDigDug(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		AllWall(w, h, _stage);
		Point sp{ 1, 1 };
		prStack.push(sp);
		while (!prStack.empty())
		{
			sp = prStack.top();
			prStack.pop();
			DigDug(sp.x, sp.y, _stage);
		}
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::WALL;
				continue;
			}
		}
	}

	void CreateLoops(int loopCount, vector<vector<STAGE_OBJ>>& _stage) {
		std::vector<Point> wallCandidates; //穴をあける壁の候補リスト

		for (int y = 1; y < STAGE_HEIGHT - 1; y++) {
			for (int x = 1; x < STAGE_WIDTH - 1; x++) {
				if (_stage[y][x] == STAGE_OBJ::WALL) {
					bool isVerticalWallWithEmptySides = false;
					bool isHorizontalWallWithEmptySides = false;

					//上下が壁で、左右が両方とも空白の場合
					if (_stage[y - 1][x] == STAGE_OBJ::WALL && _stage[y + 1][x] == STAGE_OBJ::WALL &&
						_stage[y][x - 1] == STAGE_OBJ::EMPTY && _stage[y][x + 1] == STAGE_OBJ::EMPTY) {
						isVerticalWallWithEmptySides = true;
					}

					//左右が壁で、上下が両方とも空白の場合
					if (_stage[y][x - 1] == STAGE_OBJ::WALL && _stage[y][x + 1] == STAGE_OBJ::WALL &&
						_stage[y - 1][x] == STAGE_OBJ::EMPTY && _stage[y + 1][x] == STAGE_OBJ::EMPTY) {
						isHorizontalWallWithEmptySides = true;
					}

					if (isVerticalWallWithEmptySides || isHorizontalWallWithEmptySides) {
						wallCandidates.push_back({ x, y });
					}
				}
			}
		}

		//候補の中からランダムに選んで穴を開ける
		for (int i = 0; i < loopCount && !wallCandidates.empty(); i++) {
			int randIndex = rand() % wallCandidates.size();
			Point p = wallCandidates[randIndex];
			_stage[p.y][p.x] = STAGE_OBJ::EMPTY;

			wallCandidates.erase(wallCandidates.begin() + randIndex);
		}
	}

}

Stage::Stage()
{
	std::srand((unsigned int)time(NULL));

	stageData = vector(STAGE_HEIGHT, vector<STAGE_OBJ>(STAGE_WIDTH, STAGE_OBJ::EMPTY));

	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (y == 0 || y == STAGE_HEIGHT - 1 || x == 0 || x == STAGE_WIDTH - 1)
			{
				stageData[y][x] = STAGE_OBJ::WALL;
			}
			else
			{
				if (x % 2 == 0 && y % 2 == 0)
					stageData[y][x] = STAGE_OBJ::WALL;
				else
					stageData[y][x] = STAGE_OBJ::EMPTY;
			}

		}
	}
	/*MakeMazeDigDug(STAGE_WIDTH,STAGE_HEIGHT,stageData);
	CreateLoops(10, stageData);*/
	setStageRects();
}

Stage::~Stage()
{
}

void Stage::Update()
{
}

void Stage::Draw()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			switch (stageData[y][x])
			{
			case STAGE_OBJ::EMPTY:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(102, 205, 170), TRUE);
				break;
			case STAGE_OBJ::WALL:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(119, 136, 153), TRUE);
				break;
			case STAGE_OBJ::GOAL:
			
				break;
			default:
				break;
			}
		}
	}
}

void Stage::setStageRects()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (stageData[y][x] == STAGE_OBJ::WALL)
			{
				stageRects.push_back(Rect(x * CHA_WIDTH, y * CHA_HEIGHT,  CHA_WIDTH, CHA_HEIGHT));
			}
		}
	}

}
