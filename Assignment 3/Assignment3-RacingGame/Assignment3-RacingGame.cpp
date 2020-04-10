// Assignment3-RacingGame.cpp: A program using the TL-Engine

// TL-Engine include file and namespace
#include <TL-Engine.h>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
using namespace tle;

enum crossState      {NotActivated, Activated, Waiting};
enum stages			 {Stage1, Stage2, Stage3, FinalStage};
enum gameState	     {Start, Starting, Paused, Playing, Over};
enum Collision       {None, xAxis, zAxis };
enum checkpointState {NotPassed, Passed};
enum enemyState		 {MovingForward, CheckPointReached, ResetView};

struct Map
{
	IModel* model;

	float stopWatch = 0.0f;

	int enemyCheckpoints = 0;

	crossState specialEffectState = NotActivated;

	checkpointState checkpoint = NotPassed;

	enemyState enemyVehicleState;

	stages	checkpointStages;

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
bool point2PointCollision2D(float smallXPos, float smallZPos, float largeXPos, float largeZPos);

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

	const int checkPointIndex  = 0;
	const int isleIndex        = 1;
	const int wallIndex        = 2;
	const int crossEffectIndex = 4;

	const int numberOfCheckpoints = 4;
	const int numberOfIsles = 14;
	const int numberOfWalls = 11 ;
	const int numberOfCrosses = 4;
	const int numberOfDummyModels = 14;

	const int asd = 3;
	const int maxNumberOfColumns = 100;

	int enemyCheckpoint = 0;
	int stageNumber = 0;
	float playersTime = 0;

	float newXPos;
	float newZPos;

	float enemyXPos;
	float enemyZPos;

	float counter = 0;
	float a;

	float xPos;
	float zPos;
	gameState enumGameState = Start;

	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();
	IFont* gameFont = myEngine->LoadFont("STENCIL STD", 36);
	bool gamePause = false;

	vector2D momentum{0.0f, 0.0f};
	vector2D thrust{0.0f, 0.0f};
	vector2D drag{0.0f, 0.0f};

	//vector2D enemyMomentum{ 0.0f,0.0f };
	//vector2D enemyThrust{ 0.0f, 0.0f };
	//vector2D enemyDrag{ 0.0f, 0.0f };

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "./Media" );

	vector<vector<Map>> allMapObjects(mapObjectsRows);
	for (int i = 0; i < mapObjectsRows; i++) {
		allMapObjects[i].resize(mapObjectColumns);

	}

	SpecialEffects specialObjects[asd];
	float startingXPositions[10][maxNumberOfColumns] = { {0, 10, 25, 10},											     //Checkpoint - 4
												      {-10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, -10, 100, 10},  //Isles - 14
												      {-10, 10, -10, 10, -10, 10, -10, 10, -10, -10, 0},			     //Walls - 11
												      {-20, -30},													     //Tanks - 2
													  {0, 10, 25, 10},												     //Cross - 4
												      {0, 0, 0, 0, 10, 25, 25, 25, 25, 25, 25, 10, 0, 0},				 //DummyModels - 14
												      {2.5, -2.5, 2.5},													 //Enemies - 3
													  {} };

	float startingZPositions[10][maxNumberOfColumns] = { {0, 120, 56, -120},										        //Checkpoint - 4
													  {40, 40, 56, 56, 72, 72, 88, 88,104, 104, 120, 136, 100, 136},        //Isles - 14
													  {48, 48, 64, 64,80, 80, 96, 96, 112, 128, 136},				        //Walls - 11
													  {10, 10},														        //Tanks - 2
													  {0, 120, 56, -120},                                                   //Cross - 4
													  {0, 50, 100, 120, 120, 120, 56, 6, -50, -100, -120, -120, -120, 0},   //DummyModels - 14
													  {-35, -55, -55},													    //Enemies - 3
													  {} };

	float startingRotations[10][maxNumberOfColumns] = { {0, -90, 0, -90},							//Checkpoints - 4							  
													 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},				//Isles - 14
													 {0, 0, 0, 0, 0, 0,0,0,0,0, -90},				//Walls - 11
												     {0, 0},										//Tanks - 2
													 {0, -90, 0, -90},								//Cross - 4
													 {0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0},     //DummyModels - 14
													 {0, 0, 0,},									//Enemies - 3
													 {} };

	IMesh* checkPointMesh = myEngine->LoadMesh("Checkpoint.x");
	IMesh* aisleMesh = myEngine->LoadMesh("IsleStraight.x");
	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IMesh* tankMesh = myEngine->LoadMesh("TankSmall1.x");
	IMesh* crossMesh = myEngine->LoadMesh("Cross.x");
	IMesh* dummyMesh = myEngine->LoadMesh("Dummy.x");
	IMesh* enemyCarMesh = myEngine->LoadMesh("race3.x");
	//IMesh* tribuneMesh = myEngine->LoadMesh("Flare.x");

	float matrix[4][4];
	/**** Set up your scene here ****/
	/*for (int c = 0; c < numberOfCheckpoints; c++)
	{
		InitialiseMap(mapObjects[checkPointIndex][c], checkPointMesh, startingXPositions[checkPointIndex][c], startingZPositions[checkPointIndex][c], startingRotations[0][c], 1);
	}*/
	for (int i = 0; i < numberOfCheckpoints; i++)
	{
		InitialiseMap(allMapObjects[checkPointIndex][i], checkPointMesh, startingXPositions[checkPointIndex][i], startingZPositions[checkPointIndex][i], startingRotations[checkPointIndex][i], 1);
		allMapObjects[checkPointIndex][i].checkpointStages = stages(i);
		cout << allMapObjects[checkPointIndex][i].checkpointStages;
	}
	for (int i = 0; i < numberOfIsles; i++)
	{
		InitialiseMap(allMapObjects[isleIndex][i], aisleMesh, startingXPositions[isleIndex][i], startingZPositions[isleIndex][i], startingRotations[1][i], 1);
	}
	for (int w = 0; w < numberOfWalls; w++)
	{
		InitialiseMap(allMapObjects[wallIndex][w], wallMesh, startingXPositions[wallIndex][w], startingZPositions[wallIndex][w], startingRotations[2][w], 1);
	}
	for (int t = 0; t < 2; t++)
	{
		InitialiseMap(allMapObjects[3][t], tankMesh, startingXPositions[3][t], startingZPositions[3][t], startingRotations[3][t], 1);
	}
	for (int t = 0; t < numberOfCrosses; t++)
	{
		InitialiseMap(allMapObjects[crossEffectIndex][t], crossMesh, startingXPositions[crossEffectIndex][t], startingZPositions[crossEffectIndex][t], startingRotations[crossEffectIndex][t], 0.5);
		allMapObjects[crossEffectIndex][t].model->SetY(-5);
	}
	for (int d = 0; d < numberOfDummyModels; d++)
	{
		InitialiseMap(allMapObjects[5][d], dummyMesh, startingXPositions[5][d], startingZPositions[5][d], startingRotations[5][d], 1);
	}
	for (int e = 0; e < 3; e++)
	{
		InitialiseMap(allMapObjects[6][e], enemyCarMesh, startingXPositions[6][e], startingZPositions[6][e], startingRotations[6][e], 1);
		allMapObjects[6][e].enemyVehicleState = ResetView;
	}

	stringstream startText;

	IMesh* skyBox = myEngine->LoadMesh("Skybox 07.x");
	IModel* skyModel = skyBox->CreateModel(0,-960,0);

	IModel* dummyCamera = dummyMesh->CreateModel();
	
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel(0,0,0);

	IMesh* raceMesh = myEngine->LoadMesh("race2.x");
	IModel* raceCarModel = raceMesh->CreateModel(-2.5, 0, -35);


	ICamera* gameCamera = myEngine->CreateCamera(kManual,0,20,-30);
	gameCamera->RotateX(20);

	gameCamera->AttachToParent(dummyCamera);
	dummyCamera->AttachToParent(raceCarModel);

	float timer = myEngine->Timer();
	float rotationAmount;
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		timer = myEngine->Timer();
		float FPS = (1 / timer);
		//gameFont->Draw("Hello World", 200, 100);
		stringstream outText;
		stringstream speed;
		stringstream w;
		outText << "FPS: " << FPS;
		gameFont->Draw(outText.str(), 20, 20);
		outText.str(""); // Clear myStream

		rotationAmount = timer * 45;


		speed << "Speed: " << ceil(thrust.z);
		gameFont->Draw(speed.str(), 0, 640);
		speed.str(""); // Clear myStream

		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}

		if (enumGameState == Start)
		{
			startText << "Press SPACE to play";
			gameFont->Draw(startText.str(), 500, 600);
			startText.str("");
			if (myEngine->KeyHit(Key_Space))
			{
				counter = 4;
				enumGameState = Starting;
			}
		}
		else if (enumGameState == Starting)
		{
			counter -= timer;
			a = floor(counter);
			if (counter >= 1.0f)
			{
				startText << a;
				gameFont->Draw(startText.str(), 625, 600);
				startText.str("");
			}
			else if (counter <= 1.0f && counter >= 0.0f)
			{
				startText << "Go!";
				gameFont->Draw(startText.str(), 625, 600);
				startText.str("");
				
			}
			else if (counter <= 0.0f)
				enumGameState = Playing;
		}
		else if (enumGameState == Playing)
		{
			playersTime += timer;
			startText << "Playing";
			gameFont->Draw(startText.str(), 1150, 0);
			startText.str("");

			w << ceil(playersTime) << "s";
			gameFont->Draw(w.str(), 1150, 600);
			w.str("");

			if (myEngine->KeyHit(Key_Space))
			{
				gamePause = !gamePause;
				enumGameState = gamePause ? Paused : Playing;
			}

			xPos = raceCarModel->GetX();
			zPos = raceCarModel->GetZ();

			//enemyXPos = enemyCar->GetX();
			//enemyZPos = enemyCar->GetZ();

			raceCarModel->GetMatrix(&matrix[0][0]);
			vector2D facingVector = { matrix[2][0],matrix[2][2] };

			//enemyCar->GetMatrix(&matrix[0][0]);
			vector2D enemyFacingVector = {matrix[2][0], matrix[2][2]};

			//enemyThrust = scalar((timer/10),enemyFacingVector);

			if (myEngine->KeyHeld(Key_W))
			{
				thrust = scalar((timer/2) /*(timer / 5.0f)*/, facingVector);
			}
			else if (myEngine->KeyHeld(Key_S))
			{
				thrust = scalar(-(timer/2)/*(timer / 10.0f)*/, facingVector);
			}
			else
			{
				thrust = { 0.0f, 0.0f };
			}

			if (myEngine->KeyHeld(Key_D))
			{
				raceCarModel->RotateY(rotationAmount);
			}
			if (myEngine->KeyHeld(Key_A))
			{
				raceCarModel->RotateY(-rotationAmount);
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

			//enemyDrag = scalar(-0.001f, enemyMomentum);

			momentum = sum3(momentum, thrust, drag);
			//enemyMomentum = sum3(enemyMomentum, enemyThrust, enemyDrag);

			newXPos = xPos + momentum.x;
			newZPos = zPos + momentum.z;

			//float enemyNewXPos = enemyXPos + enemyMomentum.x;
			//float enemyNewZPos = enemyZPos + enemyMomentum.z;

			for (int i = 0 ; i < 3; i++)
			{

				switch (allMapObjects[6][i].enemyVehicleState)
				{
				case MovingForward:
					//allMapObjects[6][i].model->MoveLocalZ(0.1);

					allMapObjects[6][i].model->MoveLocalZ(0.1);
					if (point2PointCollision2D(allMapObjects[6][i].model->GetX(), allMapObjects[6][i].model->GetZ(),
						allMapObjects[5][allMapObjects[6][i].enemyCheckpoints].model->GetX(), allMapObjects[5][allMapObjects[6][i].enemyCheckpoints].model->GetZ()))
					{
						allMapObjects[6][i].enemyCheckpoints++;
						allMapObjects[6][i].stopWatch = 0;
						allMapObjects[6][i].enemyVehicleState = CheckPointReached;
						
					}
					break;
				case CheckPointReached:
					if (allMapObjects[6][i].enemyCheckpoints >= (numberOfDummyModels - 1))
					{
						//cout << allMapObjects[6][i].enemyCheckpoints << endl;
						allMapObjects[6][i].enemyCheckpoints = 0;
						allMapObjects[6][i].enemyVehicleState = ResetView;
					}
					else
					{
						allMapObjects[6][i].enemyVehicleState = ResetView;
					}
					break;

				case ResetView:
					allMapObjects[6][i].model->LookAt(allMapObjects[5][allMapObjects[6][i].enemyCheckpoints].model);

					allMapObjects[6][i].enemyVehicleState = MovingForward;
					break;
				}
				cout << allMapObjects[6][i].enemyCheckpoints << endl;
			}

			for (int i = 0; i < numberOfCheckpoints; i++)
			{
				if (allMapObjects[0][i].checkpoint == NotPassed && allMapObjects[0][i].checkpointStages == stageNumber)
				{
					if (CheckPointCollision(xPos, zPos, allMapObjects[0][i].model->GetX(), allMapObjects[0][i].model->GetZ()))
					{
						allMapObjects[crossEffectIndex][i].specialEffectState = Activated;
					}
					switch (allMapObjects[crossEffectIndex][i].specialEffectState)
					{
					case Activated:
						allMapObjects[crossEffectIndex][i].model->SetY(5);
						allMapObjects[crossEffectIndex][i].specialEffectState = Waiting;
						allMapObjects[crossEffectIndex][i].stopWatch = 0.0f;
						break;
					case Waiting:
						allMapObjects[crossEffectIndex][i].stopWatch += timer;
						if (allMapObjects[0][i].checkpointStages == FinalStage)
						{
							startText << "Race complete ";
							gameFont->Draw(startText.str(), 500, 600);
							startText.str("");
							//enumGameState = Over;
						}
						else
						{
							startText << "Stage " << (i + 1) << " Complete";
							gameFont->Draw(startText.str(), 500, 600);
							startText.str("");
						}
						if (allMapObjects[crossEffectIndex][i].stopWatch >= 3.0f)
						{
							allMapObjects[crossEffectIndex][i].model->SetY(-5);
							allMapObjects[0][i].checkpoint = Passed;
							stageNumber++;
							allMapObjects[crossEffectIndex][i].specialEffectState = NotActivated;
						}
						break;
					case NotActivated:

						break;
					}
				}
			}
			for (int i = 0; i < numberOfWalls; i++)
			{
				Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, 5.0f, allMapObjects[2][i].model->GetX(), allMapObjects[2][i].model->GetZ(), 0.934082, 4.83550);
				switch (CollisionResponse)
				{
				case xAxis:
					momentum.x = -momentum.x;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					break;
				case None:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
				}

				/*Collision EnemyResponse = sphere2BoxCollision2D(enemyXPos, enemyZPos, enemyNewXPos, enemyNewZPos, 5.0f, allMapObjects[2][i].model->GetX(), allMapObjects[2][i].model->GetZ(), 0.934082, 4.83550);
				switch (EnemyResponse)
				{
				case xAxis:
					enemyMomentum.x = -enemyMomentum.x;
					break;
				case zAxis:
					enemyMomentum.z = -enemyMomentum.z;
					break;
				case None:
					enemyCar->SetPosition(enemyNewXPos, 0, enemyNewZPos);
				}*/
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
				Collision CollisionResponse1 = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, 5.0f, allMapObjects[1][i].model->GetX(), allMapObjects[1][i].model->GetZ(), 1, 1);
				switch (CollisionResponse1)
				{
				case xAxis:
					momentum.x = -momentum.x;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					break;
				case None:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
				}

				/*Collision EnemyResponse = sphere2BoxCollision2D(enemyXPos, enemyZPos, enemyNewXPos, enemyNewZPos, 5.0f, allMapObjects[1][i].model->GetX(), allMapObjects[1][i].model->GetZ(), 1, 1);
				switch (EnemyResponse)
				{
				case xAxis:
					enemyMomentum.x = -enemyMomentum.x;
					break;
				case zAxis:
					enemyMomentum.z = -enemyMomentum.z;
					break;
				case None:
					enemyCar->SetPosition(enemyNewXPos, 0, enemyNewZPos);
				}*/
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
		else if (enumGameState == Over)
		{
			w << ceil(playersTime) << "s";
			gameFont->Draw(w.str(), 1150, 600);
			w.str("");
			xPos = raceCarModel->GetX();
			zPos = raceCarModel->GetZ();

			drag = scalar(-0.001f, momentum);

			momentum = sum3(momentum, thrust, drag);

			newXPos = xPos + momentum.x;
			newZPos = zPos + momentum.z;

			raceCarModel->SetPosition(newXPos, 0, newZPos);
		}
		//ISprite* gameBackdrop = myEngine->CreateSprite("ui_backdrop_1.jpg", 0, 655);
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
		return None;
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

bool point2PointCollision2D(float smallXPos, float smallZPos, float largeXPos, float largeZPos)
{
	float x = smallXPos - largeXPos;
	float z = smallZPos - largeZPos;

	float Distance = sqrt((x*x) + (z*z));
	if (Distance <= 0.1)
	{
		return true;
	}
	else
	{
		return false;
	}
}