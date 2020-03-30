// Assignment3-RacingGame.cpp: A program using the TL-Engine

// TL-Engine include file and namespace
#include <TL-Engine.h>
#include <sstream>
#include <iostream>
#include <cmath>
using namespace std;
using namespace tle;

enum crossState {NotActivated, Activated, Waiting};
enum gameState	{Start, Starting, Paused, Playing, Over};
enum Collision { none, xAxis, zAxis };

struct Map
{
	IModel* model;

	float stopWatch = 0.0f;

	crossState specialEffectState = NotActivated;
};
struct vector2D
{
	float x;
	float z;
};
struct SpecialEffects
{
	IModel* model;
	float stopWatch = 0.0f;
	
	crossState state = NotActivated;
};

Collision sphere2BoxCollision2D(float sphereOldX, float sphereOldZ, float sphereX, float sphereZ,
								float sphereRadius, float boxX, float boxZ, float halfBoxWidth, float halfBoxDepth);

bool sphere2SphereCollision2D(float s1XPos, float s1ZPos, float s1Rad, float oldx, float oldz,
	                               float s2XPos, float s2ZPos, float s2Rad);

bool CheckPointCollision(float pointX, float pointZ, /*float checkPointHalfLength,*/ float checkPointX, float checkPointZ);

void InitialiseMap(Map& theObject, IMesh* objectMesh, float XPos, float ZPos, float rotationAmount, float scaleAmount);
void InitialiseMap1(SpecialEffects& theObject, IMesh* mesh, float XPos, float ZPos, float YPos, float rotationAmount, float scaleAmount);

vector2D scalar(float s, vector2D v)
{
	return {s * v.x, s * v.z};
}
vector2D sum3(vector2D v1, vector2D v2, vector2D v3)
{
	return {v1.x + v2.x + v3.x, v1.z + v2.z + v3.z};
}

void main()
{
	const int mapObjectColumns = 100;
	const int mapObjectsRows = 10;

	const int checkPointIndex = 0;
	const int isleIndex = 1;
	const int wallIndex = 2;

	const int numberOfCheckpoints = 3;
	const int numberOfIsles = 14;
	const int numberOfWalls = 10;

	const int asd = 3;
	const int maxNumberOfColumns = 100;

	float newXPos;
	float newZPos;

	float counter = 0;
	float a;

	float xPos;
	float zPos;
	gameState enumGameState = Start;

	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();
	IFont* gameFont = myEngine->LoadFont("STENCIL STD", 36);
	//ISprite* backdrop = myEngine->CreateSprite("ui_backdrop.jpg", 200, 400);
	//ISprite* gameBackdrop = myEngine->CreateSprite("ui_backdrop.jpg",0,0);
	//gameBackdrop->SetPosition(0, 200);
	bool gamePause = false;

	vector2D momentum{0.0f, 0.0f};
	vector2D thrust{0.0f, 0.0f};
	vector2D drag{0.0f, 0.0f};

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "./Media" );

	Map mapObjects[mapObjectsRows][mapObjectColumns];
	SpecialEffects specialObjects[asd];
	float startingXPositions[10][maxNumberOfColumns] = { {0, 10, 25},
												      {-10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, -10, 40, 100},
												      {-10, 10, -10, 10, -10, 10, -10, 10, -10, -10},
												      {-20, -30}, 
													  {0, 10, 25},
												      {},
												      {} };

	float startingZPositions[10][maxNumberOfColumns] = { {0, 120, 56},
													  {40, 40, 56, 56, 72, 72, 88, 88,104, 104, 120, 136, 40, 100},
													  {48, 48, 64, 64,80, 80, 96, 96, 112, 128}, 
													  {10, 10},
													  {0, 120, 56},
													  {} };

	float startingRotations[10][maxNumberOfColumns] = { {0, -90, 0},
													 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
													 {0, 0, 0, 0, 0, 0,0,0}, 
												     {0, 0},
													 {0, -90, 0},
													 {} };

	IMesh* checkPointMesh = myEngine->LoadMesh("Checkpoint.x");
	IMesh* aisleMesh = myEngine->LoadMesh("IsleStraight.x");
	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IMesh* tankMesh = myEngine->LoadMesh("TankSmall1.x");
	IMesh* crossMesh = myEngine->LoadMesh("Cross.x");
	//IMesh* tribuneMesh = myEngine->LoadMesh("Flare.x");

	float matrix[4][4];
	/**** Set up your scene here ****/
	for (int c = 0; c < numberOfCheckpoints; c++)
	{
		InitialiseMap(mapObjects[checkPointIndex][c], checkPointMesh, startingXPositions[checkPointIndex][c], startingZPositions[checkPointIndex][c], startingRotations[0][c], 1);
	}
	for (int i = 0; i < numberOfIsles; i++)
	{
		InitialiseMap(mapObjects[isleIndex][i], aisleMesh, startingXPositions[isleIndex][i], startingZPositions[isleIndex][i], startingRotations[1][i], 1);
	}
	for (int w = 0; w < numberOfWalls; w++)
	{
		InitialiseMap(mapObjects[wallIndex][w], wallMesh, startingXPositions[wallIndex][w], startingZPositions[wallIndex][w], startingRotations[2][w], 1);
	}
	for (int t = 0; t < 2; t++)
	{
		InitialiseMap(mapObjects[3][t], tankMesh, startingXPositions[3][t], startingZPositions[3][t], startingRotations[3][t], 1);
	}
	for (int t = 0; t < 3; t++)
	{
		InitialiseMap(mapObjects[4][t], crossMesh, startingXPositions[4][t], startingZPositions[4][t], startingRotations[4][t], 0.5);
		mapObjects[4][t].model->SetY(-5);
	}

	stringstream startText;

	IMesh* skyBox = myEngine->LoadMesh("Skybox 07.x");
	IModel* skyModel = skyBox->CreateModel(0,-960,0);

	IMesh* dummyMesh = myEngine->LoadMesh("Dummy.x");
	IModel* dummyCamera = dummyMesh->CreateModel();
	
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel(0,0,0);

	IMesh* raceMesh = myEngine->LoadMesh("race2.x");
	IModel* raceCarModel = raceMesh->CreateModel(0, 0, -35);

	ICamera* gameCamera = myEngine->CreateCamera(kManual,0,20,-30);
	gameCamera->RotateX(20);

	gameCamera->AttachToParent(dummyCamera);
	dummyCamera->AttachToParent(raceCarModel);

	float timer = myEngine->Timer();

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		timer = myEngine->Timer();
		float FPS = (1 / timer);
		//gameFont->Draw("Hello World", 200, 100);
		stringstream outText;
		outText << "FPS: " << FPS;
		gameFont->Draw(outText.str(), 20, 20);
		outText.str(""); // Clear myStream


		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}

		if (enumGameState == Start)
		{
			startText << "Press SPACE to play";
			gameFont->Draw(startText.str(), 500, 200);
			startText.str("");
			if (myEngine->KeyHit(Key_Space))
			{
				counter = 3;
				enumGameState = Starting;
			}
		}
		else if (enumGameState == Starting)
		{
			counter -= timer;
			a = ceil(counter);
			startText << "Game starting in: " << a;
			gameFont->Draw(startText.str(), 500, 200);
			startText.str("");
			if (counter <= 0.0f) enumGameState = Playing;
		}
		else if (enumGameState == Playing)
		{
			startText << "Playing";
			gameFont->Draw(startText.str(), 1150, 0);
			startText.str("");
			if (myEngine->KeyHit(Key_Space))
			{
				gamePause = !gamePause;
				enumGameState = gamePause ? Paused : Playing;
			}

			xPos = raceCarModel->GetX();
			zPos = raceCarModel->GetZ();

			raceCarModel->GetMatrix(&matrix[0][0]);
			vector2D facingVector = { matrix[2][0],matrix[2][2] };
			//cout <<  "X: "<<facingVector.x << " Z: " << facingVector.z;
			//cout << facingVector.z;

			if (myEngine->KeyHeld(Key_W))
			{
				thrust = scalar((timer / 5.0f), facingVector);
				//raceCarModel->MoveLocalZ(0.05f);
			}
			else if (myEngine->KeyHeld(Key_S))
			{
				thrust = scalar(-(timer / 10.0f), facingVector);
				//raceCarModel->MoveLocalZ(-0.05f);
			}
			else
			{
				thrust = { 0.0f, 0.0f };
			}

			if (myEngine->KeyHeld(Key_D))
			{
				raceCarModel->RotateY(0.25f);
			}
			if (myEngine->KeyHeld(Key_A))
			{
				raceCarModel->RotateY(-0.25f);
			}

			if (myEngine->KeyHeld(Key_Up))
			{
				dummyCamera->RotateLocalX(0.1f);
			}
			else if (myEngine->KeyHeld(Key_Down))
			{
				dummyCamera->RotateLocalX(-0.1f);
			}

			drag = scalar(-0.001f, momentum);

			momentum = sum3(momentum, thrust, drag);

			newXPos = xPos + momentum.x;
			newZPos = zPos + momentum.z;
			for (int i = 0; i < 3; i++)
			{
				if (CheckPointCollision(xPos, zPos, mapObjects[0][i].model->GetX(), mapObjects[0][i].model->GetZ()))
				{
					mapObjects[4][i].specialEffectState = Activated;
				}
				switch (mapObjects[4][i].specialEffectState)
				{
				case NotActivated:
					mapObjects[4][i].model->SetY(-5);
					break;
				case Activated:
					mapObjects[4][i].model->SetY(5);
					mapObjects[4][i].specialEffectState = Waiting;
					mapObjects[4][i].stopWatch = 0.0f;
					break;
				case Waiting:
					mapObjects[4][i].stopWatch += timer;
					startText << "Stage " << (i + 1) << " Complete";
					gameFont->Draw(startText.str(), 500, 200);
					startText.str("");
					if (mapObjects[4][i].stopWatch >= 3.0f)
					{
						//specialObjects[i].model->SetY(-5);
						mapObjects[4][i].specialEffectState = NotActivated;
					}

					break;
				}
			}
			for (int i = 0; i < numberOfWalls; i++)
			{
				Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, 5.0f, mapObjects[2][i].model->GetX(), mapObjects[2][i].model->GetZ(), 0.934082, 4.83550);
				switch (CollisionResponse)
				{
				case xAxis:
					momentum.x = -momentum.x;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					break;
				case none:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
				}
			}
			for (int i = 0; i < numberOfIsles; i++)
			{
				//bool CollisionResponseSphere2Sphere = sphere2SphereCollision2D(newXPos, newZPos, 5.0f, xPos, zPos, mapObjects[1][i].model->GetX(), mapObjects[1][i].model->GetZ(), 3.046385);//3.046385
				//
				//if (CollisionResponseSphere2Sphere)
				//{
				//	raceCarModel->Move(-newXPos, 0, -newZPos);
				//}
				//else
				//{
				//	raceCarModel->SetPosition(newXPos, 0, newZPos);
				//}
				Collision CollisionResponse1 = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, 5.0f, mapObjects[1][i].model->GetX(), mapObjects[1][i].model->GetZ(), 1, 1);
				switch (CollisionResponse1)
				{
				case xAxis:
					momentum.x = -momentum.x;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					break;
				case none:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
				}
			}
		}
		else if (enumGameState == Paused)
		{
		startText << "Paused";
		gameFont->Draw(startText.str(), 1150, 0);
		startText.str("");
			if (myEngine->KeyHit(Key_Space))
			{
				gamePause = !gamePause;
				enumGameState = gamePause ? Paused : Playing;
			}
			xPos = raceCarModel->GetX();
			zPos = raceCarModel->GetZ();

			drag = scalar(-0.001f, momentum);

			momentum = sum3(momentum, thrust, drag);

			newXPos = xPos + momentum.x;
			newZPos = zPos + momentum.z;

			raceCarModel->SetPosition(newXPos, 0, newZPos);
		}
		//for (int i = 0; i < 3; i++)
		//{
		//	Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, 5.0f, mapObjects[0][i].model->GetX(), mapObjects[0][i].model->GetZ(), 9.86159, 1.28539);
		//	switch (CollisionResponse)
		//	{
		//	case xAxis:
		//		momentum.x = -momentum.x;
		//		break;
		//	case zAxis:
		//		momentum.z = -momentum.z;
		//		break;
		//	case none:
		//		raceCarModel->SetPosition(newXPos, 0, newZPos);
		//	}
		//}

		//raceCarModel->Move(momentum.x, 0, momentum.z);
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

void InitialiseMap(Map& theObject, IMesh* mesh, float XPos, float ZPos, float rotationAmount, float scaleAmount)
{
	theObject.model = mesh->CreateModel(XPos,0,ZPos);
	theObject.model->RotateY(rotationAmount);
	theObject.model->Scale(scaleAmount);
}
void InitialiseMap1(SpecialEffects& theObject, IMesh* mesh, float XPos, float ZPos, float YPos, float rotationAmount, float scaleAmount)
{
	theObject.model = mesh->CreateModel(XPos, YPos, ZPos);
	theObject.model->RotateY(rotationAmount);
	theObject.model->Scale(scaleAmount);
}

Collision sphere2BoxCollision2D(float sphereOldX, float sphereOldZ, float sphereX, float sphereZ,
	float sphereRadius, float boxX, float boxZ, float halfBoxWidth, float halfBoxDepth)

{
	float maxX = boxX + halfBoxWidth + sphereRadius;
	float minX = boxX - halfBoxWidth - sphereRadius;
	float maxZ = boxZ + halfBoxDepth + sphereRadius;
	float minZ = boxZ - halfBoxDepth - sphereRadius;

	if (sphereX > minX && sphereX < maxX && sphereZ > minZ && sphereZ < maxZ)
	{
		if (sphereOldX < minX || sphereOldX > maxX)
		{
			return xAxis;
		}
		else
		{
			return zAxis;
		}
	}
	else
	{
		return none;
	}
}
bool CheckPointCollision(float pointX, float pointZ, /*float checkPointHalfLength*/ float checkPointX, float checkPointZ)
{
	float checkpointMaxX = checkPointX + 9.86159;
	float checkpointMinX = checkPointX - 9.86159;
	float checkpointMinZ = checkPointZ - 1.28539;
	float checkpointMaxZ = checkPointZ + 1.28539;

	if (pointX >= checkpointMinX && pointX <= checkpointMaxX &&
		pointZ >= checkpointMinZ && pointZ <= checkpointMaxZ)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool sphere2SphereCollision2D(float s1XPos, float s1ZPos, float s1Rad, float oldx, float oldz,
								   float s2XPos, float s2ZPos, float s2Rad)
{
	float distX = s2XPos - s1XPos;
	float distZ = s2ZPos - s1ZPos;
	float distance = sqrt(distX*distX + distZ*distZ);
	if (distance < (s1Rad + s2Rad))
	{
		return true;
		////return xAxis;
		//if (oldx < s2XPos - s2Rad || oldx > s2XPos + s2Rad)
		//{
		//	//return xAxis;
		//}
		//else
		//{
		//	//return zAxis;
		//}
	}
	else
	{
		return false;
	}
}