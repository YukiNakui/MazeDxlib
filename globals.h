#pragma once


	struct Point
	{
		int x, y;
	};

	struct Pointf
	{
		float x, y;
	};

	struct Rect
	{
		int x, y, w, h;
		Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
		Point GetCenter() { return Point{ x + w / 2, y + h / 2 }; }
	};


	enum class STAGE_OBJ {
		EMPTY,
		WALL,
		GOAL,
		MAX_OBJECT
	};


	enum DIR
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NONE,
		MAXDIR
	};
	
	const int CHA_WIDTH = 32;
	const int CHA_HEIGHT = 32;