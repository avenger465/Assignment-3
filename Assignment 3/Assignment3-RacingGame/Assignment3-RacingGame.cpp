// Assignment3-RacingGame.cpp: A program by Jonathan Mills

// TL-Engine include file and namespace
#include <TL-Engine.h>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
using namespace tle;

enum crossState      {NotActivated, Activated, Waiting};
enum stages			 {Stage1, Stage2, Stage3, Stage4, FinalStage};
enum gameState	     {Start, Starting, Playing, Paused, Over};
enum Collision       {None, xAxis, zAxis };
enum checkpointState {NotPassed, Passed};
enum CameraPositions {ThirdPerson, FirstPerson};
enum enemyState		 {MovingForward, CheckPointReached, ResetView};
enum driveStates	 {Normal, Boosting, BoostCooldown};

struct Map
{
	IModel* model;

	float stopWatch = 0.0f;

	float speed = 0.f;

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
//Collision sphere2SphereCarCollision(float smallXPos, float smallZPos, float  largeXPos, float largeZPos, float sphereRadius);

bool DistanceCalculation(float smallXPos, float smallZPos, float largeXPos, float largeZPos, float sphereRadius);

bool sphere2SphereCollision2D(float s1XPos, float s1ZPos, float s1Rad,
	                               float s2XPos, float s2ZPos, float s2Rad);

void ResetCameraFunction(IModel* dummyCamera, float &cameraXMovement, float &cameraZMovement,
	float &cameraXRotationAmount, float &cameraYRotationAmount);

bool CheckPointCollision(float pointX, float pointZ, /*float checkPointHalfLength,*/ float checkPointX, float checkPointZ);

void InitialiseMap(Map& theObject, IMesh* objectMesh, float XPos, float ZPos, float rotationAmount, float scaleAmount);
bool point2PointCollision2D(float distance);

void DisplayText(stringstream displayStream, IFont* displayFont, int xCoordinate, int yCoordinate);

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
	const int waterTankIndex   = 3;
	const int crossEffectIndex = 4;
	const int dummyModelsIndex = 5;
	const int enemyCarIndex	   = 6;

	const int numberOfCheckpoints = 5; //5
	const int numberOfIsles = 80; //80
	const int numberOfWalls = 78; //78
	const int numberOfCrosses = 5; //5
	const int numberOfDummyModels = 14; //14
	const int numberOfWaterTanks = 5; //5                          
	const int numberOfEnemies = 3; //3

	const int asd = 3;
	const int maxNumberOfColumns = 100;
	const int mapScale = 10;

	int enemyCheckpoint = 0;
	int stageNumber = 0;
	float playersHealth = 100;
	float playersTime = 0;
	float carTime = 0;
	float speed = 0;
	float speed1 = 0;
	float dist = 0;

	float newXPos;
	float newZPos;

	//Variables used for the control of the camera movement and positioning
	float maxCameraZ = 0;
	float maxCameraX = 0;
	float minCameraZ = 0;
	float minCameraX = 0;
	float cameraXMovement = 0;
	float cameraZMovement = 0;
	float cameraXRotationAmount = 0;
	float cameraYRotationAmount = 0;
	float mouseMovementX = 0;
	float mouseMovementY = 0;

	//Variables used to get the enemies current X and Z coordinates
	float enemyXPos;
	float enemyZPos;

	float startDownCounter = 0;
	float startDownCounterDisplay;

	float boostTimer = 0;
	float boostCooldown = 5;

	float xPos;
	float zPos;
	bool CheckpointStrutsResponse = false;

	gameState enumGameState = Start;
	driveStates enumCarDriveState = Normal;

	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();
	IFont* gameFont = myEngine->LoadFont("STENCIL STD", 36);
	bool gamePause = false;

	CameraPositions cameraView = ThirdPerson;
	vector2D momentum{0.0f, 0.0f};
	vector2D thrust{0.0f, 0.0f};
	vector2D drag{0.0f, 0.0f};

	EKeyCode SpaceKey = Key_Space;
	EKeyCode WKey = Key_W;
	EKeyCode AKey = Key_A;
	EKeyCode SKey = Key_S;
	EKeyCode DKey = Key_D;
	EKeyCode DownKey = Key_Down;
	EKeyCode UpKey = Key_Up;
	EKeyCode LeftKey = Key_Left;
	EKeyCode RightKey = Key_Right;


	//vector2D enemyMomentum{ 0.0f,0.0f };
	//vector2D enemyThrust{ 0.0f, 0.0f };
	//vector2D enemyDrag{ 0.0f, 0.0f };

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "./media" );
	myEngine->AddMediaFolder("./extraMedia");

	vector<vector<Map>> allMapObjects(mapObjectsRows);
	for (int i = 0; i < mapObjectsRows; i++) {
		allMapObjects[i].resize(mapObjectColumns);

	}

	SpecialEffects specialObjects[asd];
	float startingXPositions[10][maxNumberOfColumns] = { {0, 10, 105, 105, 10},	//Checkpoint - 5
												      {-10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, -10, 10, 35, 35, 55, 55, 75, 75, 95, 95, 115, 115 ,115, 115, 95, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 115, 95, 75, 75, 55, 55, 35, 35, 10, 10, -10 ,-10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10}, //Isles - 80
												      {-10, 10, -10, 10, -10, 10, -10, 10, -10, -10, 0, 22.5, 22.5, 45, 45, 65, 65, 85, 85, 105, 115, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 95, 115, 115, 105, 85, 85, 65, 65, 45, 45, 22.5, 22.5, 0, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10, 10, -10}, //Walls - 78
												      {-10, 115, 96, 115, -10},	//Tanks - 5
													  {0, 10, 105, 105, 10}, //Cross - 6
												      {0, 0, 0, 0, 10, 25, 25, 25, 25, 25, 25, 10, 0, 0}, //DummyModels - 14
												      {2.5, -2.5, 2.5},	//Enemies - 3
													  {} };

	float startingZPositions[10][maxNumberOfColumns] = { {0, 120, 56, -100, -115}, //Checkpoint - 5
													  {40, 40, 56, 56, 72, 72, 88, 88,104, 104, 120, 136, 136, 136, 104, 130, 110, 104, 136, 136, 104, 136, 120, 104, 88, 88, 72, 72, 56, 56, 40, 40, 24, 24, 8, 8, -8, -8, -24, -24, -40, -40, -56, -56, -72, -72, -88, -88, -104, -104, -125, -125, -104, -125, -104, -125, -104, -125, -104, -125, -104, -125, -104, -125, - 88, -88, -72, -72, -54, -56, -40, -40, -24, -24, -8, -8, 8, 8, 24, 24}, //Isles - 80
													  {48, 48, 64, 64,80, 80, 96, 96, 112, 128, 136, 136, 104, 107, 133, 107, 133, 104, 136, 136, 128, 112, 96, 96, 80, 80, 64, 64, 48, 48, 32, 32, 16, 16, 0, 0, -16, -16, -32, -32, -48, -48, -64, -64, -80, -80, -96, -96, -114, -125, -104, -125, -104, -125, -104, -125, -104, -125, -125, -114, -96, -96, -80, -80, -64, -64, -48, -48, -32, -32, -16, -16, 0, 0, 16, 16, 32, 32}, //Walls - 78
													  {136, 136, 0, -125, -125}, //Tanks - 5
													  {0, 120, 56, -100, -115}, //Cross - 5
													  {0, 50, 100, 120, 120, 120, 56, 6, -50, -100, -120, -120, -120, 0}, //DummyModels - 14
													  {-35, -50, -50}, //Enemies - 3
													  {} };

	float startingRotations[10][maxNumberOfColumns] = { {0, -90, 0, 0, -90}, //Checkpoints - 5							  
													 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//Isles - 80
													 {0, 0, 0, 0, 0, 0,0,0,0,0, -90, -90, -90, 75, -75, -75, 75, -90, -90, -90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -90, -90, -90, -90, -90, -90, -90, -90, -90, -90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //Walls - 78
												     {0, 0, -25,0, 0}, //Tanks - 5
													 {0, -90, 0, 0, -90}, //Cross - 5
													 {0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0}, //DummyModels - 14
													 {0, 0, 0,}, //Enemies - 3
												     {} };

	float wallScale[numberOfWalls] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 , 2.2, 2.2, 2.2, 2.2 , 2.2 , 2.2 , 2.2 , 2.2 , 2.2 , 2.2, 2.2, 2.2, 1, 1, 1, 1 , 1, 1 , 1, 1 , 1, 1 , 1, 1 , 1, 1 , 1, 1 , 1, 1 };
	//srand(myEngine->Timer() * 100000.0f);
	float enemySpeed[3] = {30, 28, 24 };
	for (int i = 0; i < 3; i++)
	{
		cout << enemySpeed[i] << endl;
	}

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
	for (int i = 0; i < numberOfCheckpoints/*4*/; i++)
	{
		InitialiseMap(allMapObjects[checkPointIndex][i], checkPointMesh, startingXPositions[checkPointIndex][i], startingZPositions[checkPointIndex][i], startingRotations[checkPointIndex][i], 1);
		allMapObjects[checkPointIndex][i].checkpointStages = stages(i);
		cout << allMapObjects[checkPointIndex][i].checkpointStages;
	}
	for (int i = 0; i < numberOfIsles/*80*/; i++)
	{
		InitialiseMap(allMapObjects[isleIndex][i], aisleMesh, startingXPositions[isleIndex][i], startingZPositions[isleIndex][i], startingRotations[1][i], 1);
	}
	for (int w = 0; w < numberOfWalls/*22*/; w++)
	{
		InitialiseMap(allMapObjects[wallIndex][w], wallMesh, startingXPositions[wallIndex][w], startingZPositions[wallIndex][w], startingRotations[2][w], 1);
		allMapObjects[wallIndex][w].model->ScaleZ(wallScale[w]);
	}
	for (int t = 0; t < numberOfWaterTanks/*8*/; t++)
	{
		InitialiseMap(allMapObjects[waterTankIndex][t], tankMesh, startingXPositions[waterTankIndex][t], startingZPositions[waterTankIndex][t], startingRotations[waterTankIndex][t], 1);
		allMapObjects[waterTankIndex][t].model->RotateZ(startingRotations[waterTankIndex][t]);
		allMapObjects[waterTankIndex][t].model->SetY(-3);
	}
	for (int t = 0; t < numberOfCrosses/*4*/; t++)
	{
		InitialiseMap(allMapObjects[crossEffectIndex][t], crossMesh, startingXPositions[crossEffectIndex][t], startingZPositions[crossEffectIndex][t], startingRotations[crossEffectIndex][t], 0.5);
		allMapObjects[crossEffectIndex][t].model->SetY(-5);
	}
	for (int d = 0; d < numberOfDummyModels/*14*/; d++)
	{
		InitialiseMap(allMapObjects[dummyModelsIndex][d], dummyMesh, startingXPositions[dummyModelsIndex][d], startingZPositions[dummyModelsIndex][d], startingRotations[dummyModelsIndex][d], 1);
	}
	for (int e = 0; e < numberOfEnemies/*3*/; e++)
	{
		InitialiseMap(allMapObjects[enemyCarIndex][e], enemyCarMesh, startingXPositions[enemyCarIndex][e], startingZPositions[enemyCarIndex][e], startingRotations[enemyCarIndex][e], 1);
		allMapObjects[enemyCarIndex][e].enemyVehicleState = ResetView;
	}

	stringstream startText;

	IMesh* skyBox = myEngine->LoadMesh("Skybox 07.x");
	IModel* skyModel = skyBox->CreateModel(0,-960,0);

	IModel* dummyCamera = dummyMesh->CreateModel();
	
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel(0,0,0);

	IMesh* raceMesh = myEngine->LoadMesh("race2.x");
	IModel* raceCarModel = raceMesh->CreateModel(-2.5, 0, -35);
	raceCarModel->Scale(0.5);


	ICamera* gameCamera = myEngine->CreateCamera(kManual,0,20,-30);
	gameCamera->RotateX(20);

	gameCamera->AttachToParent(dummyCamera);
	dummyCamera->AttachToParent(raceCarModel);

	float timer = myEngine->Timer();
	float rotationAmount;

	while (myEngine->IsRunning())
	{

		myEngine->DrawScene();
		timer = myEngine->Timer();
		float FPS = (1 / timer);
		stringstream frameTimeReadout;
		stringstream speedReadout;
		stringstream gameTimeReadout;
		stringstream playersHealthReadout;
		stringstream boostReadout;
		playersHealthReadout << "Health: " << playersHealth;
		gameFont->Draw(playersHealthReadout.str(), 20, 50);
		playersHealthReadout.str("");
		frameTimeReadout << "FPS: " << floor(FPS);
		gameFont->Draw(frameTimeReadout.str(), 20, 20);
		frameTimeReadout.str(""); // Clear myStream

		mouseMovementX = myEngine->GetMouseMovementX();
		mouseMovementY = myEngine->GetMouseMovementY();

		rotationAmount = timer * 45;

		speedReadout << "Speed: " << speed << "km/h";
		gameFont->Draw(speedReadout.str(), 0, 640);
		speedReadout.str(""); // Clear myStream

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
				startDownCounter = 4;
				enumGameState = Starting;
			}
		}
		else if (enumGameState == Starting)
		{
			startDownCounter -= timer;
			startDownCounterDisplay = floor(startDownCounter);
			if (startDownCounter >= 1.0f)
			{
				startText << startDownCounterDisplay;
				gameFont->Draw(startText.str(), 625, 600);
				startText.str("");
			}
			else if (startDownCounter <= 1.0f && startDownCounter >= 0.0f)
			{
				startText << "Go!";
				gameFont->Draw(startText.str(), 625, 600);
				startText.str("");
				
			}
			else if (startDownCounter <= 0.0f)
				enumGameState = Playing;
		}
		else if (enumGameState == Playing)
		{
			if (playersHealth <= 0.0f)
			{
				enumGameState = Over;
			}

			playersTime += timer;
			startText << "Playing";
			gameFont->Draw(startText.str(), 1150, 0);
			startText.str("");

			gameTimeReadout << ceil(playersTime) << "s";
			gameFont->Draw(gameTimeReadout.str(), 1150, 600);
			gameTimeReadout.str("");


			if (myEngine->KeyHit(Key_P))
			{
				gamePause = !gamePause;
				enumGameState = gamePause ? Paused : Playing;
			}

			switch (cameraView)
			{
			case ThirdPerson:
				gameCamera->SetLocalPosition(0, 20, -30);
				break;
			case FirstPerson:
				gameCamera->SetLocalPosition(0, 3, 5);
				break;
			}
			if (myEngine->KeyHit(Key_2))
			{
				ResetCameraFunction(dummyCamera, cameraXMovement, cameraZMovement, cameraXRotationAmount, cameraYRotationAmount);
				cameraView = FirstPerson;
			}
			else if (myEngine->KeyHit(Key_1))
			{
				ResetCameraFunction(dummyCamera, cameraXMovement, cameraZMovement, cameraXRotationAmount, cameraYRotationAmount);
				cameraView = ThirdPerson;
			}

			if (mouseMovementX > 0)
			{
				cameraYRotationAmount -= 0.1f;
				dummyCamera->RotateLocalY(-0.1f);
			}
			else if (mouseMovementX < 0)
			{
				cameraYRotationAmount += 0.1f;
				dummyCamera->RotateLocalY(0.1f);
			}
			if (mouseMovementY > 0)
			{
				cameraXRotationAmount += 0.1f;
				dummyCamera->RotateLocalX(0.1f);
			}
			else if (mouseMovementY < 0)
			{
				cameraXRotationAmount -= 0.1f;
				dummyCamera->RotateLocalX(-0.1f);
			}

			xPos = raceCarModel->GetX();
			zPos = raceCarModel->GetZ();

			maxCameraX = xPos + 5;
			minCameraX = xPos - 5;
			maxCameraZ = zPos + 10;
			minCameraZ = zPos - 5;

			raceCarModel->GetMatrix(&matrix[0][0]);
			vector2D facingVector = { matrix[2][0],matrix[2][2] };

			if (boostCooldown >= 5.0f)
			{
				if (myEngine->KeyHeld(WKey))
				{
					if (myEngine->KeyHeld(SpaceKey) && playersHealth >= 30.0f)
					{
						if (boostTimer >= 3.0f)
						{
							boostTimer = 0;
							boostCooldown -= timer;
						}
						else
						{
							thrust = scalar(((timer / 2) * 2), facingVector);
							boostTimer += timer;

							boostReadout << "Boost: " << boostTimer;
							gameFont->Draw(boostReadout.str(), 625, 200);
							boostReadout.str("");
						}
					}
					else
					{
						thrust = scalar((timer / 2), facingVector);
					}
				}
				else if (myEngine->KeyHeld(SKey))
				{
					thrust = scalar(-(timer/4), facingVector);
				}
				else
				{
					thrust = { 0.0f, 0.0f };
				}
				drag = scalar(-0.001f, momentum);
			}
			else
			{
				drag = scalar(-0.001f * 2, momentum);
				cout << drag.x << endl;
				cout << drag.z << endl;
				if (boostCooldown <= 0)
					boostCooldown = 5;
				else
					boostCooldown -= timer;

				if (myEngine->KeyHeld(SKey))
					thrust = scalar(-(timer / 4), facingVector);
				else if (myEngine->KeyHeld(WKey))
					thrust = scalar((timer / 2), facingVector);
				else
					thrust = { 0.0f, 0.0f };

				boostReadout << "Boost Cooldown: " << boostCooldown;
				gameFont->Draw(boostReadout.str(), 625, 200);
				boostReadout.str("");
			}

			if (myEngine->KeyHeld(DKey))
			{
				raceCarModel->RotateY(rotationAmount);
			}
			if (myEngine->KeyHeld(AKey))
			{
				raceCarModel->RotateY(-rotationAmount);
			}

			if (myEngine->KeyHeld(UpKey))
			{
				if (dummyCamera->GetZ() >= maxCameraZ);
				else
				{
					cameraZMovement += 0.1f;
					dummyCamera->MoveLocalZ(0.1f);
				}
			}
			else if (myEngine->KeyHeld(DownKey))
			{
				if (dummyCamera->GetZ() <= minCameraZ);
				else
				{
					cameraZMovement -= 0.1f;
					dummyCamera->MoveLocalZ(-0.1f);
				}
			}
			else if (myEngine->KeyHeld(LeftKey))
			{
				if (dummyCamera->GetX() <= minCameraX);
				else
				{
					cameraXMovement -= 0.1f;
					dummyCamera->MoveLocalX(-0.1f);
				}
			}
			else if (myEngine->KeyHeld(RightKey))
			{
				if (dummyCamera->GetX() >= minCameraX);
				else
				{
					cameraXMovement += 0.1f;
					dummyCamera->MoveLocalX(0.1f);
				}
			}

			momentum = sum3(momentum, thrust, drag);
			speed = floor((sqrt(pow(momentum.x, 2) + pow(momentum.z, 2)) * mapScale * 3.6));

			newXPos = xPos + momentum.x;
			newZPos = zPos + momentum.z;

			for (int i = 0 ; i < numberOfEnemies; i++)
			{

				Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, 5.0f, allMapObjects[enemyCarIndex][i].model->GetX(), allMapObjects[enemyCarIndex][i].model->GetZ(), 2, 6);
				switch (CollisionResponse)
				{
				case xAxis:
					momentum.x = -momentum.x;
					playersHealth--;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					playersHealth--;
					break;
				case None:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
					break;
				}

				switch (allMapObjects[enemyCarIndex][i].enemyVehicleState)
				{
				case ResetView:

					allMapObjects[enemyCarIndex][i].model->LookAt(allMapObjects[dummyModelsIndex][allMapObjects[enemyCarIndex][i].enemyCheckpoints].model);

					allMapObjects[enemyCarIndex][i].enemyVehicleState = MovingForward;
					break;
				case MovingForward:

					allMapObjects[enemyCarIndex][i].speed = enemySpeed[i] * timer / 2;
					allMapObjects[enemyCarIndex][i].model->MoveLocalZ(allMapObjects[enemyCarIndex][i].speed);
					if (point2PointCollision2D(DistanceCalculation(allMapObjects[enemyCarIndex][i].model->GetX(), allMapObjects[enemyCarIndex][i].model->GetZ(),
																   allMapObjects[dummyModelsIndex][allMapObjects[enemyCarIndex][i].enemyCheckpoints].model->GetX(),
																   allMapObjects[dummyModelsIndex][allMapObjects[enemyCarIndex][i].enemyCheckpoints].model->GetZ(), 0.1f)))
					{
						allMapObjects[enemyCarIndex][i].enemyCheckpoints++;
						allMapObjects[enemyCarIndex][i].stopWatch = 0;
						allMapObjects[enemyCarIndex][i].enemyVehicleState = CheckPointReached;
						
					}
					break;
				case CheckPointReached:
					if (allMapObjects[enemyCarIndex][i].enemyCheckpoints == numberOfDummyModels)
					{
						allMapObjects[enemyCarIndex][i].enemyCheckpoints = 0;
						allMapObjects[enemyCarIndex][i].enemyVehicleState = ResetView;
					}
					else
					{
						allMapObjects[enemyCarIndex][i].enemyVehicleState = ResetView;
					}
					
					break;
				}
			}

			for (int i = 0; i < numberOfCheckpoints; i++)
			{
				if (startingRotations[checkPointIndex][i] == 0)
				{
					if (sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() + 9.86159) - 1.5, (allMapObjects[checkPointIndex][i].model->GetZ() + 1.28539) - 1.5, 1.5, raceCarModel->GetX(), raceCarModel->GetZ(), (3.8 / 2)) ||
						sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() - 9.86159) + 1.5, (allMapObjects[checkPointIndex][i].model->GetZ() - 1.28539) + 1.5, 1.5, raceCarModel->GetX(), raceCarModel->GetZ(), (3.8 / 2)))
					{
						momentum.x = -momentum.x;
						momentum.z = -momentum.z;
					}
				}
				else
				{
					if (sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() + 1.28539) - 1.5, (allMapObjects[checkPointIndex][i].model->GetZ() + 9.86159) - 1.5, 1.5, raceCarModel->GetX(), raceCarModel->GetZ(), (3.8 / 2)) ||
						sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() - 1.28539) + 1.5, (allMapObjects[checkPointIndex][i].model->GetZ() - 9.86159) + 1.5, 1.5, raceCarModel->GetX(), raceCarModel->GetZ(), (3.8 / 2)))
					{
						momentum.x = -momentum.x;
						momentum.z = -momentum.z;
					}
				}
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
				Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, (3.8f / 2), allMapObjects[2][i].model->GetX(), allMapObjects[2][i].model->GetZ(), 0.15, 4.5);
				switch (CollisionResponse)
				{
				case xAxis:
					momentum.x = -momentum.x;
					playersHealth--;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					playersHealth--;
					break;
				case None:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
				}
			}
			for (int i = 0; i < numberOfIsles; i++)
			{
				Collision CollisionResponse1 = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, (3.8f / 2), allMapObjects[1][i].model->GetX(), allMapObjects[1][i].model->GetZ(), 1.5, 1.5);
				switch (CollisionResponse1)
				{
				case xAxis:
					momentum.x = -momentum.x;
					playersHealth--;
					break;
				case zAxis:
					momentum.z = -momentum.z;
					playersHealth--;
					break;
				case None:
					raceCarModel->SetPosition(newXPos, 0, newZPos);
				}
			}
			for (int i = 0; i < numberOfWaterTanks; i++)
			{
				if (sphere2SphereCollision2D(allMapObjects[waterTankIndex][i].model->GetX(), allMapObjects[waterTankIndex][i].model->GetZ(), 4, raceCarModel->GetX(), raceCarModel->GetZ(), (3.8/2)))
				{
					momentum.x = -momentum.x;
					momentum.z = -momentum.z;
				}
			}
		}
		else if (enumGameState == Paused)
		{
		startText << "Paused";
		gameFont->Draw(startText.str(), 1150, 0);
		startText.str("");
			if (myEngine->KeyHit(Key_P))
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
			cout << "Over";
		}
		//cout << enumGameState;
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

bool DistanceCalculation(float smallXPos, float smallZPos, float largeXPos, float largeZPos, float sphereRadius)
{
	float x = smallXPos - largeXPos;
	float z = smallZPos - largeZPos;

	float Distance = sqrt((x*x) + (z*z));
	//return Distance;
	if (Distance <= sphereRadius)
		return true;
	else
		return false;
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
		else if (sphereOldZ < minZ || sphereOldZ > maxZ)
		{
			return zAxis;
		}
	}
	else
	{
		return None;
	}
}

//Collision sphere2SphereCarCollision(float smallXPos, float smallZPos, float  largeXPos, float largeZPos, float sphereRadius)
//{
//	float distance = DistanceCalculation(smallXPos, smallZPos, largeXPos, largeZPos, sphereRadius);
//
//	/*if (DistanceCalculation(smallXPos, smallZPos, largeXPos, largeZPos, sphereRadius))
//	{
//		return xAxis;
//	}
//	else
//	{
//		return None;
//	}*/
//
//}
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
bool point2PointCollision2D(float distance)
{
	if (distance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool sphere2SphereCollision2D(float s1XPos, float s1ZPos, float s1Rad,
								   float s2XPos, float s2ZPos, float s2Rad)
{
	float distX = s2XPos - s1XPos;
	float distZ = s2ZPos - s1ZPos;
	float distance = sqrt(distX*distX + distZ*distZ);
	if (distance < (s1Rad + s2Rad))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ResetCameraFunction(IModel* dummyCamera, float &cameraXMovement, float &cameraZMovement,
						float &cameraXRotationAmount, float &cameraYRotationAmount)
{
	dummyCamera->MoveLocalX(-cameraXMovement);
	dummyCamera->MoveLocalZ(-cameraZMovement);
	dummyCamera->RotateLocalX(-cameraXRotationAmount);
	dummyCamera->RotateLocalY(-cameraYRotationAmount);
	cameraXMovement = 0;
	cameraXRotationAmount = 0;
	cameraZMovement = 0;
	cameraYRotationAmount = 0;
}

void DisplayText(stringstream displayStream, string displayText, IFont* displayFont, float displayNumber, int xCoordinate, int yCoordinate)
{ 
	displayStream << displayText << displayNumber;
	displayFont->Draw(displayStream.str(), xCoordinate, yCoordinate);
	displayStream.str();
}