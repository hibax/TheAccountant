#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

struct Point
{
	int x;
	int y;
};


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
	void setId(int id) { this->id = id; };
};

class Enemy : public Entity
{
	int life;
	Point target;
	Point nextPosition;
public:
	Enemy(int id, int x, int y, int life) : Entity(id, x, y), life(life) {};
	~Enemy() {};
	Point getPosition() { return position; };
	void setPosition(Point pos) { position = pos; };
	int getLife() { return life; };
	void setLife(int l) { life = l; };
	void setTarget(Point p) { target = p; };
	Point getTarget() { return target; };
	void setNextPosition(Point p) { nextPosition = p; };
	Point getNextPosition() { return nextPosition; };
};

class DataPoint : public Entity
{
public:
	DataPoint(int id, int x, int y) : Entity(id, x, y) {};
	~DataPoint() {};

};


float calculateDistance(const Point& p1, const Point& p2)
{
	float diffY = p1.y - p2.y;
	float diffX = p1.x - p2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

Point calculateBarycentre(vector<Entity*> pointList)
{
	Point barycentre;

	int xSum = 0;
	int ySum = 0;
	for (int i = 0; i < pointList.size(); ++i)
	{
		Point point = pointList[i]->getPosition();
		xSum += point.x;
		ySum += point.y;
	}
	barycentre.x = static_cast<int>(xSum / pointList.size());
	barycentre.y = static_cast<int>(ySum / pointList.size());

	return barycentre;
}


Entity* calculateNearestEntity(Point pos, vector<Entity*> entityList) {

	Entity* nearestEntity = nullptr;
	float distanceMinimum = 999999;
	for (int i = 0; i < entityList.size(); ++i)
	{
		const float distanceFromEntity = calculateDistance(pos, entityList[i]->getPosition());

		if (distanceFromEntity < distanceMinimum) {
			nearestEntity = entityList[i];
			distanceMinimum = distanceFromEntity;
		}
	}
	return nearestEntity;
}

Point calculateDeplacement(Point from, Point to, int distance_to_do) {
	Point targetPosition;

	targetPosition.x = static_cast<int>(from.x + (distance_to_do / calculateDistance(from, to))*(to.x - from.x));
	targetPosition.y = static_cast<int>(from.y + (distance_to_do / calculateDistance(from, to))*(to.y - from.y));

	return targetPosition;
}

void calculateEnemiesDeplacements(vector<Entity*> enemyList, vector<Entity*> dataList) {

	for (int i = 0; i < enemyList.size(); ++i) {
		Entity* nearestDataPoint = calculateNearestEntity(enemyList[i]->getPosition(), dataList);
		static_cast<Enemy*>(enemyList[i])->setTarget(nearestDataPoint->getPosition());

		Point nextPosition = calculateDeplacement(enemyList[i]->getPosition(), nearestDataPoint->getPosition(), 500);
		static_cast<Enemy*>(enemyList[i])->setNextPosition(nextPosition);
	}
}

Enemy* calculateDangerousEnemies(Point myPosition, vector<Entity*> enemyList) {
	Enemy* dangerousEnemy;
	for (int i = 0; i < enemyList.size(); ++i) {
		dangerousEnemy = static_cast<Enemy*>(enemyList[i]);
		if (calculateDistance(myPosition, dangerousEnemy->getNextPosition()) < 2000) {
			return dangerousEnemy;
		}
	}
	return nullptr;
}


bool isThereEnemiesInRange(Point myPosition, vector<Entity*> enemyList, int range) {
	for (int i = 0; i < enemyList.size(); ++i) {
		if (calculateDistance(myPosition, enemyList[i]->getPosition()) < range) {
			return true;
		}
	}
	return false;
}


string generateAction(Point myPosition, vector<Entity*> dataList, vector<Entity*> enemyList)
{

	Enemy* nearestEnemy = static_cast<Enemy*>(calculateNearestEntity(myPosition, enemyList));

	Point barycentre = calculateBarycentre(dataList);
	cerr << "Barycentre : " + to_string(barycentre.x) + "," + to_string(barycentre.y) << endl;

	calculateEnemiesDeplacements(enemyList, dataList);

	Enemy* dangerousEnemy = calculateDangerousEnemies(myPosition, enemyList);

	if (dangerousEnemy != nullptr) {


		Point target = calculateDeplacement(myPosition, nearestEnemy->getPosition(), -1000);
		Enemy* dangerousEnemy = calculateDangerousEnemies(target, enemyList);
		if (dangerousEnemy == nullptr) {
			return "MOVE " + to_string(target.x) + " " + to_string(target.y);
		}
		else {
			//TO DO
			return "OUPS " + to_string(nearestEnemy->getId());
		}

	}
	else if (calculateDistance(myPosition, barycentre) < 3000)
	{
		return "SHOOT " + to_string(nearestEnemy->getId());
	}
	else if (isThereEnemiesInRange(myPosition, enemyList, 3000))
	{
		return "SHOOT " + to_string(nearestEnemy->getId());
	}
	else
	{
		Point target = calculateDeplacement(myPosition, barycentre, 1000);
		Enemy* dangerousEnemy = calculateDangerousEnemies(target, enemyList);

		if (dangerousEnemy == nullptr) {
			return "MOVE " + to_string(barycentre.x) + " " + to_string(barycentre.y);
		}
		else {
			return "SHOOT " + to_string(nearestEnemy->getId());
		}
	}
}


int main()
{
	int i = 0;
	vector<Entity*> dataList;
	vector<Entity*> enemyList;

	/*
		Here is the game loop
	*/

	Point myPosition = { 7500,2000 };

	if (i == 0)
	{
		// Test case  = 3 enemies 

		int dataListSize = 2;
		DataPoint *data1 = new DataPoint(0, 9000, 1200);
		DataPoint *data2 = new DataPoint(1, 400, 6000);

		dataList.reserve(dataListSize);
		dataList.push_back(data1);
		dataList.push_back(data2);

		int EnemyListSize = 3;
		Enemy *enemy1 = new Enemy(0, 500, 4500, 10);
		Enemy *enemy2 = new Enemy(1, 13900, 5000, 10);
		Enemy *enemy3 = new Enemy(2, 7000, 7500, 10);

		enemyList.reserve(EnemyListSize);
		enemyList.push_back(enemy1);
		enemyList.push_back(enemy2);
		enemyList.push_back(enemy3);
	}
	else
	{
		/*
			Example of how to re-fill list. Datas are wrong
		*/
		int newDataListSize = 2;
		for (int i = 0; i < dataList.size(); ++i)
		{
			if (i < newDataListSize) {
				dataList[i]->setId(0);
				dataList[i]->setPosition({ 9000, 1200 });
			}
			else {
				delete dataList[i];
			}
		}
		dataList.resize(newDataListSize);

		int newEnemyListSize = 3;
		for (int i = 0; i < enemyList.size(); ++i)
		{
			if (i < newEnemyListSize) {
				enemyList[i]->setId(0);
				enemyList[i]->setPosition({ 9000, 1200 });
				static_cast<Enemy*>(enemyList[i])->setLife(500);
			}
			else {
				delete enemyList[i];
			}
		}
		enemyList.resize(newEnemyListSize);
	}

	string output = generateAction(myPosition, dataList, enemyList);
	string expectedOutput = "MOVE 4700 3600";

	++i;
	/*
		End of game loop
	*/

	assert(output == expectedOutput);

	system("pause");
}


/*
	Wrking main

int main()
{
    int i = 0;
	vector<Entity*> dataList;
	vector<Entity*> enemyList;
    // game loop
    while (1) {
        Point myPosition;
        cin >> myPosition.x >> myPosition.y; cin.ignore();
        cerr << "My Position : " + to_string(myPosition.x) + " : " + to_string(myPosition.y) << endl;
        
        
        if (i == 0)
    	{
            int dataCount;
            cin >> dataCount; cin.ignore();
            
            dataList.reserve(dataCount);
            for (int i = 0; i < dataCount; i++) {
                int id, x, y;
                cin >> id >> x >> y; cin.ignore();
                dataList.push_back(new DataPoint(id, x, y));
                cerr << "Data Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y) << endl;
            }
            
            int enemyCount;
            cin >> enemyCount; cin.ignore();
            
            enemyList.reserve(enemyCount);
            for (int i = 0; i < enemyCount; i++) {
                int id, x, y, enemyLife;
                cin >> id >> x >> y >> enemyLife; cin.ignore();
                enemyList.push_back(new Enemy(id, x, y, enemyLife));
                cerr << "Enemy Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y)  + " : " + to_string(enemyLife) << endl;
            }
    	}
    	else
    	{
            int newDataListSize;
            cin >> newDataListSize; cin.ignore();
    	    
    		for (int i = 0; i < dataList.size(); ++i)
    		{
    			if (i < newDataListSize) {
    			    int id, x, y;
                    cin >> id >> x >> y; cin.ignore();
    				dataList[i]->setId(id);
    				dataList[i]->setPosition({ x, y });
    				cerr << "Data Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y) << endl;
    			}
    			else {
    				delete dataList[i];
    			}
    		}
    		dataList.resize(newDataListSize);
    		
            int newEnemyListSize;
            cin >> newEnemyListSize; cin.ignore();
    		for (int i = 0; i < enemyList.size(); ++i)
    		{
    			if (i < newEnemyListSize) {
    			    int id, x, y, l;
                    cin >> id >> x >> y >> l; cin.ignore();
    				enemyList[i]->setId(id);
    				enemyList[i]->setPosition({ x, y });
    				static_cast<Enemy*>(enemyList[i])->setLife(l);
    				cerr << "Enemy Position " + to_string(id) + " : " + to_string(x) + " : " + to_string(y)  + " : " + to_string(l) << endl;
    			}
    			else {
    				delete enemyList[i];
    			}
    		}
    		enemyList.resize(newEnemyListSize);
    	}
    	
    	
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << generateAction(myPosition, dataList, enemyList)  << endl; // MOVE x y or SHOOT id
        ++i;
    }
}


*/
