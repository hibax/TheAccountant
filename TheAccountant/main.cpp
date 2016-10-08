#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;


class Entity
{
protected:
	Point position;
	int id;
public:
	Entity(int id, int x, int y) : id(id), position({ x,y }) {};
	virtual ~Entity() {};
	Point getPosition() { return position; };
	void setPosition(Point pos) { position = pos; };
	int getId() { return id; };
	void setId(int id) { id = id; };
};

class Enemy : public Entity
{
	int life;
public:
	Enemy(int id, int x, int y, int life) : Entity(id, x, y), life(life) {};
	~Enemy() {};
	Point getPosition() { return position; };
	void setPosition(Point pos) { position = pos; };
	int getLife() { return life; };
	void setLife(int l) { life = l; };

};

class DataPoint : public Entity
{
public:
	DataPoint(int id, int x, int y) : Entity(id, x, y) {};
	~DataPoint() {};

};


struct Point
{
	int x;
	int y;
};

float calculateDistance(const Point& p1, const Point& p2)
{
	float diffY = p1.y - p2.y;
	float diffX = p1.x - p2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

Point calculateBarycentre(vector<Entity> pointList)
{
	Point barycentre;

	int xSum = 0;
	int ySum = 0;
	for (int i = 0; i < pointList.size(); ++i)
	{
		Point point = pointList[i].getPosition();
		xSum += point.x;
		ySum += point.y;
	}
	barycentre.x = static_cast<int>(xSum / pointList.size());
	barycentre.y = static_cast<int>(ySum / pointList.size());

	return barycentre;
}

string generateAction(Point myPosition, vector<Entity> dataList, vector<Entity> enemyList)
{
	int nearestEnemyId = -1;
	float distanceMinimum = 999999;

	for (int i = 0; i < enemyList.size(); ++i)
	{
		const float distanceFromEnemy = calculateDistance(myPosition, enemyList[i].getPosition());

		cerr << "Enemy ID : " + to_string(enemyList[i].getId()) + " : " + to_string(distanceFromEnemy) << endl;
		if (distanceFromEnemy < distanceMinimum) {
			nearestEnemyId = enemyList[i].getId();
			distanceMinimum = distanceFromEnemy;
		}
	}

	Point barycentre = calculateBarycentre(dataList);
	cerr << "Barycentre : " + to_string(barycentre.x) + "," + to_string(barycentre.y) << endl;

	if (calculateDistance(myPosition, barycentre) < 1500)
	{
		return "SHOOT " + to_string(nearestEnemyId);
	}
	else
	{
		return "MOVE " + to_string(barycentre.x) + " " + to_string(barycentre.y);
	}
}


int main()
{

	/*
		Here is the game loop
	*/


	Point myPosition = { 7500,2000 };

	DataPoint data1 = DataPoint(0, 9000, 1200);
	DataPoint data2 = DataPoint(1, 400, 6000);
	Enemy enemy1 = Enemy(0, 500, 4500, 100);
	Enemy enemy2 = Enemy(1, 13900, 5000, 100);
	Enemy enemy3 = Enemy(2, 7000, 7500, 100);

	vector<Entity> dataList;
	dataList.push_back(data1);
	dataList.push_back(data2);

	vector<Entity> enemyList;
	enemyList.push_back(enemy1);
	enemyList.push_back(enemy2);
	enemyList.push_back(enemy3);

	string output = generateAction(myPosition, dataList, enemyList);
	string expectedOutput = "MOVE 4700 3600";

	assert(output == expectedOutput);

	system("pause");
}