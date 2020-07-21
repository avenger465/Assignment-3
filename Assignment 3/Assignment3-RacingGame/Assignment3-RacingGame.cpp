// Assignment3-RacingGame.cpp: A program by Jonathan Mills

//All the external files and libraries used in the program
#include <TL-Engine.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <math.h>
#include "Math.h"
using namespace std;
using namespace tle;

//Declaring all the enums to be used throughout the program
enum crossState      { NotActivated, Activated, Waiting};
enum checkpointState { NotPassed, Passed };
enum enemyState		 { MovingForward, CheckPointReached, ResetView };
enum laps			 { Lap, LapComplete};
enum gameState	     { MainMenu, Start, Starting, Playing, Paused, Over};
enum Collision		 { None, xAxis, zAxis };
enum CameraPositions { ThirdPerson, FirstPerson};
enum driveStates	 { Normal, Boosting, BoostCooldown};

//Structure data type containing information about each object in the game used in the vector class
struct Map
{

	IModel* model;

	float stopWatch = 0.0f;

	float speed = 0.f;

	int enemyCheckpoints = 0;

	crossState specialEffectState = NotActivated;

	checkpointState checkpoint = NotPassed;

	enemyState enemyVehicleState;

	int checkpointnumber;

};

//Structure data type used for the movement of the racecar model using vector maths
struct vector2D
{

	float x;
	float z;
};

struct ObjectLoading
{
	float x;
	float z;
	float rotation;
	float scale;
};

//All the prototype calls
Collision sphere2BoxCollision2D(float sphereOldX, float sphereOldZ, float sphereX, float sphereZ,
								float sphereRadius, float boxX, float boxZ, float halfBoxWidth, float halfBoxDepth);

bool DistanceCalculation(float smallXPos, float smallZPos, float largeXPos, float largeZPos, float sphereRadius);

bool sphere2SphereCollision2D(float s1XPos, float s1ZPos, float s1Rad,
	                          float s2XPos, float s2ZPos, float s2Rad);

void ResetCameraFunction(IModel* dummyCamera, float &cameraXMovement, float &cameraZMovement,
						 float &cameraXRotationAmount, float &cameraYRotationAmount);

bool CheckPointCollision(float pointX, float pointZ, float checkPointX, float checkPointZ, float halfXWidth, float halfZWidth);

void InitialiseMap(Map& theObject, IMesh* objectMesh, float XPos, float ZPos, float rotationAmount, float scaleAmount);

bool point2PointCollision2D(float distance);

void DisplayText(string displayText, IFont* displayFont, float displayNumber, int xCoordinate, int yCoordinate, string clearStream);

void ReadFile(ifstream& infile, vector<vector<ObjectLoading>>& inputArray, int& checkpointNum, int& isleNum, int& wallNum, int& crossNum, int& dummyNum, int& tankNum, int& enemyNum);

void OpenFile(ifstream& infile, string text);

//The main section of the code 
void main()
{
	//Map and matrix information
	const int maxNumberOfColumns = 100;
	const int mapScale = 2.5;
	const int mapObjectColumns = 100;
	const int mapObjectsRows = 10;
	const int matrixCol = 4;
	const int matrixRow = 4;

	//Game object indexes
	const int checkPointIndex = 0;
	const int isleIndex = 1;
	const int wallIndex = 2;
	const int waterTankIndex = 3;
	const int crossEffectIndex = 4;
	const int dummyModelsIndex = 5;
	const int enemyCarIndex = 6;

	const int numberOfLaps = 5;

	//Total number of each object
	int numberOfCheckpoints = 0;
	int numberOfIsles = 0;
	int numberOfWalls = 0;
	int numberOfCrosses = 0;
	int numberOfDummyModels = 0;
	int numberOfWaterTanks = 0;
	int numberOfEnemies = 0;

	//Font sizes
	const int gameFontSize = 24;
	const int startFontSize = 30;

	//Font coordinates
	const int gameStateTextXPos = 1090;
	const int gameStateTextYPos = 690;
	const int countdownTextXPos = 625;
	const int levelOneXPos = 1;
	const int levelOneYPos = 608;
	const int levelTwoXPos = 550;
	const int levelTwoYPos = 649;
	const int levelThreeXPos = 1105;
	const int levelThreeYPos = 690;
	const int adjustedLevelTwoYPos = 635;
	const int adjustedlevelThreeXPos = 1210;

	//Button Coordinates
	const int buttonOneXCoordinate = 550;
	const int buttonOneYCoordinate = 250;
	const int buttonTwoXCoordinate = 550;
	const int buttonTwoYCoordinate = 300;
	const int exitButtonXCoordinate = 600;
	const int exitButtonYCoordinate = 350;
	const int exitButtonXLimit = 50;
	const int exitButtonYLimit = 25;
	const int buttonXLimit = 200;
	const int buttonYLimit = 25;



	//Basic integers
	const int matrixStart = 0;
	const int zeroInt = 0;
	const int twoInt = 2;
	const int oneInt = 1;

	//Checkpoint and lap information
	const int stageIncrement = 1;
	const int startingLap = 1;
	int enemyCheckpoint = 0;
	int stageNumber = 0;
	int currentLap = 1;

	//Gametime information
	int playerMinutes = 0;

	//Y coordinates
	const float obstacleYPosition = -3.0f;
	const float undergroundYPosition = -5.0f;
	const float abovegroundYPosition = 5.0f;

	//Information needed for collision detection
	const float racecarRadius = 3.8f / 2.0f;
	const float enemyRadius = 3.8f / 2.0f;
	const float checkpointStrutRadius = 1.5f;
	const float tankRadius = 4.0f;
	const float isleRadius = 1.5f;
	const float checkpointHalfXWidth = 9.86159f;
	const float checkpointHalfZWidth = 1.28539f;
	const float wallXHalfDepth = 0.15f;
	const float wallZHalfDepth = 4.5f;
	const float dummyEnemyDistance = 0.1f;

	//Information about the boost
	const float boostThreshold = 3.0f;
	const float initialBoostCooldown = 5.0f;
	const float boostWarningTime = 2.0f;
	const float healthNeededForBoost = 30.0f;

	//Information about the camera
	const float cameraRotationAmount = 6.5f;
	const float cameraRotation = 25.0f;
	const float thirdPersonXPos = 0.0f;
	const float thirdPersonYPos = 20.0f;
	const float thirdPersonZPos = -30.0f;
	const float firstPersonXPos = 0.0f;
	const float firstPersonYPos = 3.0f;
	const float firstPersonZPos = 5.0f;
	const float cameraMovementMaxMovement = 5.0f;
	const float raceCarCameraRotation = 45.0f;

	//Information about the model coordinates
	const float skyXPos = 0.0f;
	const float skyYPos = -960.0f;
	const float skyZPos = 0.0f;
	const float zeroFloat = 0.0f;
	const float oneFloat = 1.0f;
	const float raceCarXPos = -2.5f;
	const float raceCarZPos = -35.0f;
	const float spriteXPosition = 0.0f;
	const float spriteYPosition = 608.0f;

	//Information about scale and conversions
	const float halfScale = 0.5f;
	const float scaleReset = 1.0f;
	const float conversionToKilo = 3.6f;
	const float collisionResponseMultiplierBoundaries = 2.0f;
	const float collisionResponseMulitplierObstacles = 1.3f;
	const float minuteThreshold = 60.0f;
	const float crossStopWatchLimit = 0.8f;

	const float startingPlayersHealth = 100.0f;

	//Information about the movement vector for the racecar Model
	const float thrustForwardMultiplier = 1.0f;
	const float boostForwarMultiplier = 4.0f;
	const float thrustBackwardMultiplier = 4.0f;
	const float enemyForwardMultiplier = 2.0f;
	const float coefficientOfDrag = -0.001f;
	float thrustForwardScalar = 0.0f;
	float thrustBackwardScalar = 0.0f;

	//Information about the players health and speed
	float playersHealth = 100.0f;
	float playerSeconds = 0.0f;
	float speed = 0.0f;

	//Information about the player and enemy coorinates
	float xPos = 0.0f;
	float zPos = 0.0f;
	float newXPos = 0.0f;
	float newZPos = 0.0f;
	float enemyXPos = 0.0f;
	float enemyZPos = 0.0f;

	//Variables used for the control of the camera movement and positioning
	float maxCameraZ = 0.0f;
	float maxCameraX = 0.0f;
	float minCameraZ = 0.0f;
	float minCameraX = 0.0f;
	float cameraXMovement = 0.0f;
	float cameraZMovement = 0.0f;
	float cameraXRotationAmount = 0.0f;
	float cameraYRotationAmount = 0.0f;
	float mouseMovementX = 0.0f;
	float mouseMovementY = 0.0f;

	//Information about the countdown timer
	float startDownCounter = 4.0f;

	//Information about the boost timings
	float boostTimer = 0.0f;
	float boostCooldown = 5.0f;

	//Initialises the matrix needed for the facing vector 
	float matrix[matrixRow][matrixCol];

	//Information about the game state switch
	bool gamePause = false;
	bool loadedIn = false;

	//Initialises the font and media location texts 
	string font = "STENCIL STD";
	string mediaText = "./media";
	string extraMediaText = "./extraMedia";

	//Initialises the text strings that are used in the User interface
	string clearTextStream = "";
	string gameOverText = "game state: Over";
	string gamePausedText = "game state: Paused";
	string gamePlayingText = "game state: Playing";
	string gameStartingText = "Press SPACE to play";
	string optionOneText = "Load Map_One";
	string optionTwoText = "Load Map_Two";
	string exitText = "Exit";
	string restartText = "Click to Restart!";
	string gameTimeText = "game time: ";
	string playerHealthText = "Health: ";
	string FPSText = "FPS: ";
	string playerSpeedText = "Speed: ";
	string speedReadoutText = "km/h";

	//Initialises the text strings that tell the user information about the game
	string stageText = "Stage ";
	string lapText = "Lap ";
	string completeText = " Complete!";
	string boostCooldownText = "Boost Cooldown: ";
	string boostActiveText = "Boost: Active ";
	string boostOverheatingText = "1s to overheating";
	string goText = "Go!";
	string slashChar = "/";
	string sChar = "s";
	string mChar = "m";
	string spaceChar = " ";

	//Initialises the mesh strings used to create the models in the game
	string checkPointMeshString = "Checkpoint.x";
	string aisleMeshString = "IsleStraight.x";
	string wallMeshString = "Wall.x";
	string tankMeshString = "TankSmall1.x";
	string crossMeshString = "Cross.x";
	string dummyMeshString = "Dummy.x";
	string enemyCarMeshString = "race3.x";
	string skyBoxMeshString = "Skybox 07.x";
	string groundMeshString = "ground.x";
	string raceMeshString = "race2.x";
	string uiSpriteText = "ui_backdrop_1.jpg";

	//Strings for the two maps 
	string mapOneString = "Map_One.txt";
	string mapTwoString = "Map_Two.txt";

	//Initialises all the keys that will be used in the program
	EKeyCode SpaceKey = Key_Space;
	EKeyCode EscapeKey = Key_Escape;
	EKeyCode WKey = Key_W;
	EKeyCode AKey = Key_A;
	EKeyCode SKey = Key_S;
	EKeyCode DKey = Key_D;
	EKeyCode DownKey = Key_Down;
	EKeyCode UpKey = Key_Up;
	EKeyCode LeftKey = Key_Left;
	EKeyCode RightKey = Key_Right;
	EKeyCode Number2Key = Key_2;
	EKeyCode Number1Key = Key_1;
	EKeyCode PauseKey = Key_P;
	EKeyCode LeftClick = Mouse_LButton;

	//Initialises the camera and engine type
	ECameraType cameraType = kManual;
	EEngineType gameEngineType = kTLX;

	//File stream to be used for the reading of the files
	ifstream readFileStream;

	//Initialises the enums to be used by the program as global enums
	gameState enumGameState = MainMenu;
	driveStates enumCarDriveState = Normal;
	laps enumGameLaps = Lap;
	CameraPositions cameraView = ThirdPerson;

	//Initialies the strings to be used by the program to outpute information on to the screen
	stringstream startText;
	stringstream speedReadout;
	stringstream secondsReadout;
	stringstream display;

	//Initialises the three vectors used by the racecar to zero
	vector2D momentum{ zeroFloat, zeroFloat };
	vector2D thrust{ zeroFloat, zeroFloat };
	vector2D drag{ zeroFloat, zeroFloat };

	//Loads the game Engine and the fonts to be used by the engine
	I3DEngine* myEngine = New3DEngine(gameEngineType);
	myEngine->StartWindowed();
	IFont* gameFont = myEngine->LoadFont(font, gameFontSize);
	IFont* startFont = myEngine->LoadFont(font, startFontSize);

	//Starts the program in a windowed configuration
	myEngine->AddMediaFolder(mediaText);
	myEngine->AddMediaFolder(extraMediaText);

	//Resizes the STL vector to fit the requirements for all the objects in the game
	vector<vector<Map>> allMapObjects(mapObjectsRows);
	for (int i = zeroInt; i < mapObjectsRows; i++)
	{

		allMapObjects[i].resize(mapObjectColumns);
	}

	//Resizes the STL vector to fit the number of objects in the game to be read from the file
	vector<vector<ObjectLoading>> FileVector(mapObjectsRows);
	for (int i = 0; i < mapObjectsRows; i++)
	{
		FileVector[i].resize(mapObjectColumns);
	}

	//array containing the speed for each of the enemy vehicles
	float enemySpeed[3] = { 75, 72, 69 };

	//Mesh variables loading in each mesh for all the objects on the map
	IMesh* checkPointMesh = myEngine->LoadMesh(checkPointMeshString);
	IMesh* aisleMesh = myEngine->LoadMesh(aisleMeshString);
	IMesh* wallMesh = myEngine->LoadMesh(wallMeshString);
	IMesh* tankMesh = myEngine->LoadMesh(tankMeshString);
	IMesh* crossMesh = myEngine->LoadMesh(crossMeshString);
	IMesh* dummyMesh = myEngine->LoadMesh(dummyMeshString);
	IMesh* enemyCarMesh = myEngine->LoadMesh(enemyCarMeshString);
	IMesh* skyBox = myEngine->LoadMesh(skyBoxMeshString);
	IMesh* groundMesh = myEngine->LoadMesh(groundMeshString);
	IMesh* raceMesh = myEngine->LoadMesh(raceMeshString);

	//Sprite variable loading in the User_Interface for the user
	ISprite* gameBackdrop;

	//Creates the basic scenes models
	IModel* skyModel = skyBox->CreateModel(skyXPos, skyYPos, skyZPos);
	IModel* dummyCamera = dummyMesh->CreateModel();
	IModel* groundModel = groundMesh->CreateModel(zeroFloat, zeroFloat, zeroFloat);
	IModel* raceCarModel;

	//Creates the game at the correct position to allow the User_Interface sprite to be positioned correctly
	ICamera* gameCamera = myEngine->CreateCamera(cameraType, thirdPersonXPos, thirdPersonYPos, thirdPersonZPos);
	gameCamera->RotateX(cameraRotation);

	//Initialises the timer variable to be used for controlling the speed of every object
	float timer = myEngine->Timer();
	float rotationAmount = zeroFloat;

	if (HelloWorld() == 5)
	{
		enumGameState = Playing;
	}

	//goes through while the game engine is running
	while (myEngine->IsRunning())
	{
		//Draws the scene using all the calculations from the previous frame
		myEngine->DrawScene();

		//Calculates the thrust vectors scalar amount each frame
		thrustForwardScalar = timer / thrustForwardMultiplier;
		thrustBackwardScalar = timer / thrustBackwardMultiplier;

		//Displays the frame rate of the program
		timer = myEngine->Timer();
		float FPS = (oneFloat / timer);
		rotationAmount = timer * raceCarCameraRotation;

		//Gets the Mouses' movement since the last call of these functions
		mouseMovementX = myEngine->GetMouseMovementX();
		mouseMovementY = myEngine->GetMouseMovementY();

		//Gets the coordinates of the mouse according to the top left of the screen
		float mouseCoordX = myEngine->GetMouseX();
		float mouseCoordY = myEngine->GetMouseY();

		//calls the DisplayText function to print the FPS to the screen 
		DisplayText(FPSText, gameFont, floor(FPS), levelOneXPos, levelThreeYPos, clearTextStream);

		//Checks if the escape key has been pressed and then stops the program and closes the window
		if (myEngine->KeyHit(EscapeKey))
		{

			myEngine->Stop();
		}

		//Checks the game state and goes through this code if it is equal to 'MainMenu'
		if (enumGameState == MainMenu)
		{
			//calls the DisplayText function to print the first map text to the screen
			DisplayText(optionOneText, startFont, NULL, buttonOneXCoordinate, buttonOneYCoordinate, clearTextStream);

			//calls the DisplayText function to print the second map text to the screen
			DisplayText(optionTwoText, startFont, NULL, buttonTwoXCoordinate, buttonTwoYCoordinate, clearTextStream);

			//calls the DisplayText function to print the exit text to the screen
			DisplayText(exitText, startFont, NULL, exitButtonXCoordinate, exitButtonYCoordinate, clearTextStream);

			//Checks if the the left mouse button has been pressed within the text limits
			if (mouseCoordX >= buttonOneXCoordinate && mouseCoordX <= (buttonOneXCoordinate + buttonXLimit)
				&& mouseCoordY >= buttonOneYCoordinate && mouseCoordY <= (buttonOneYCoordinate + buttonYLimit)
				&& myEngine->KeyHit(LeftClick))
			{
				//calls the OpenFile function to open the file to be read from
				OpenFile(readFileStream, mapOneString);

				//calls the ReadFile function to read the file and store the frequency of each game object
				ReadFile(readFileStream, FileVector, numberOfCheckpoints, numberOfIsles, numberOfWalls, numberOfCrosses, numberOfDummyModels, numberOfWaterTanks, numberOfEnemies);

				//Sets the game state to 'Start'
				enumGameState = Start;
			}
			//Checks if the the left mouse button has been pressed within the text limits
			else if (mouseCoordX >= buttonTwoXCoordinate && mouseCoordX <= (buttonOneXCoordinate + buttonXLimit)
				&& mouseCoordY >= buttonTwoYCoordinate && mouseCoordY <= (buttonTwoYCoordinate + buttonYLimit)
				&& myEngine->KeyHit(LeftClick))
			{
				//calls the OpenFile function to open the file to be read from
				OpenFile(readFileStream, mapTwoString);

				//calls the ReadFile function to read the file and store the frequency of each game object
				ReadFile(readFileStream, FileVector, numberOfCheckpoints, numberOfIsles, numberOfWalls, numberOfCrosses, numberOfDummyModels, numberOfWaterTanks, numberOfEnemies);

				//Sets the game state to 'Start'
				enumGameState = Start;
			}
			//Checks if the the left mouse button has been pressed within the text limits
			else if (mouseCoordX >= exitButtonXCoordinate && mouseCoordX <= (exitButtonXCoordinate + exitButtonXLimit)
				&& mouseCoordY >= exitButtonYCoordinate && mouseCoordY <= (exitButtonYCoordinate + exitButtonYLimit)
				&& myEngine->KeyHit(LeftClick))
			{
				//Stops the program and closes the window
				myEngine->Stop();
			}
		}
		//Checks the game state and goes through this code if it is equal to 'Start'
		else if (enumGameState == Start)
		{
			if (loadedIn == false)
			{
				//Creates the User interface
				gameBackdrop = myEngine->CreateSprite(uiSpriteText, spriteXPosition, spriteYPosition);

				//Loads in the racecar model 
				raceCarModel = raceMesh->CreateModel(raceCarXPos, zeroFloat, raceCarZPos);
				//Sets the racecar to an appropriate size relative to all other models in the game
				raceCarModel->Scale(halfScale);

				//Attaches the camera to a dummy camera that is attached to the racecar model
				//to posistion the camera in the right position and to allow movement of the camera
				gameCamera->AttachToParent(dummyCamera);
				dummyCamera->AttachToParent(raceCarModel);

				//Goes through each checkpoint in the vector and creates its model with all relevant information from the X, Z and rotation arrays
				for (int i = zeroInt; i < numberOfCheckpoints; i++)
				{

					InitialiseMap(allMapObjects[checkPointIndex][i], checkPointMesh, FileVector[checkPointIndex][i].x, FileVector[checkPointIndex][i].z, FileVector[checkPointIndex][i].rotation, FileVector[checkPointIndex][i].scale);
					allMapObjects[checkPointIndex][i].checkpointnumber = i;
				}
				//Goes through each Isle in the vector and creates its model using the X, Z and Rotation arrays
				for (int i = zeroInt; i < numberOfIsles; i++)
				{

					InitialiseMap(allMapObjects[isleIndex][i], aisleMesh, FileVector[isleIndex][i].x, FileVector[isleIndex][i].z, FileVector[isleIndex][i].rotation, FileVector[isleIndex][i].scale);
				}
				//Goes through each Wall in the vector and creates its model using the X, Z and Rotation arrays
				for (int w = zeroInt; w < numberOfWalls; w++)
				{

					InitialiseMap(allMapObjects[wallIndex][w], wallMesh, FileVector[wallIndex][w].x, FileVector[wallIndex][w].z, FileVector[wallIndex][w].rotation, oneFloat);
					allMapObjects[wallIndex][w].model->ScaleZ(FileVector[wallIndex][w].scale);
				}
				//Goes through each WaterTank in the vector and creates its model using the X, Z and Rotation arrays
				for (int t = zeroInt; t < numberOfWaterTanks; t++)
				{

					InitialiseMap(allMapObjects[waterTankIndex][t], tankMesh, FileVector[waterTankIndex][t].x, FileVector[waterTankIndex][t].z, FileVector[waterTankIndex][t].rotation, FileVector[waterTankIndex][t].scale);
					allMapObjects[waterTankIndex][t].model->RotateZ(FileVector[waterTankIndex][t].rotation);
					allMapObjects[waterTankIndex][t].model->SetY(obstacleYPosition);
				}
				//Goes through each Cross in the vector and creates its model using the X, Z and Rotation arrays
				for (int t = zeroInt; t < numberOfCrosses; t++)
				{

					InitialiseMap(allMapObjects[crossEffectIndex][t], crossMesh, FileVector[crossEffectIndex][t].x, FileVector[crossEffectIndex][t].z, FileVector[crossEffectIndex][t].rotation, FileVector[crossEffectIndex][t].scale);
					allMapObjects[crossEffectIndex][t].model->SetY(undergroundYPosition);
				}
				//Goes through each Dummy in the vector and creates its model using the X, Z and Rotation arrays
				for (int d = zeroInt; d < numberOfDummyModels; d++)
				{

					InitialiseMap(allMapObjects[dummyModelsIndex][d], dummyMesh, FileVector[dummyModelsIndex][d].x, FileVector[dummyModelsIndex][d].z, FileVector[dummyModelsIndex][d].rotation, FileVector[dummyModelsIndex][d].scale);
				}
				//Goes through each Enemy in the vector and creates its model using the X, Z and Rotation arrays
				for (int e = zeroInt; e < numberOfEnemies; e++)
				{

					InitialiseMap(allMapObjects[enemyCarIndex][e], enemyCarMesh, FileVector[enemyCarIndex][e].x, FileVector[enemyCarIndex][e].z, FileVector[enemyCarIndex][e].rotation, FileVector[enemyCarIndex][e].scale);
					allMapObjects[enemyCarIndex][e].enemyVehicleState = ResetView;
				}
				//Sets the loading of the map to false
				loadedIn = true;
			}
			//calls the DisplayText function to print the starting text to the screen
			DisplayText(gameStartingText, startFont, NULL, levelTwoXPos, levelTwoYPos, clearTextStream);

			//Checks if the space key  has been hit and then starts the countdown to start the game
			if (myEngine->KeyHit(SpaceKey))
			{
				//Sets the game state to 'Starting'
				enumGameState = Starting;
			}
		}
		else if (enumGameState == Starting)
		{
			//Decrements the countdown timer
			startDownCounter -= timer;

			//Checks if the countdown timer is greater than 'one' to display the numbers until 'one'
			if (startDownCounter >= oneFloat)
			{
				//calls the DisplayText function to print the countdown number to the screen
				DisplayText(clearTextStream, startFont, floor(startDownCounter), countdownTextXPos, levelTwoYPos, clearTextStream);
			}
			//Checks if the countdown timer is between 'zero' and 'one' and then displays the 'Go' text
			else if (startDownCounter <= oneFloat && startDownCounter >= zeroFloat)
			{
				//calls the DisplayText function to print the Go text to the screen
				DisplayText(goText, startFont, NULL, countdownTextXPos, levelTwoYPos, clearTextStream);
			}
			//Checks if the countdown timer is less than 'zero' and then changes the game state to playing
			else if (startDownCounter <= zeroFloat)
			{

				enumGameState = Playing;
			}
		}
		//Checks if the game state is in the Playing state
		else if (enumGameState == Playing)
		{
			//Checks if the players health is below  zero 
			//and if the laps are complete and then sets the game state to over
			if (playersHealth <= zeroFloat || currentLap > numberOfLaps)
			{

				enumGameState = Over;
			}

			//Increments the playerSeconds and then checks if it is greater than '60' 
			//to convert it into minutes 
			playerSeconds += timer;
			if (playerSeconds >= minuteThreshold)
			{

				playerMinutes++;
				playerSeconds = zeroFloat;
			}

			//Displays the gameTime in the bottom right of the screen
			secondsReadout << gameTimeText << int(playerMinutes) << mChar << spaceChar << int(playerSeconds) << sChar;
			gameFont->Draw(secondsReadout.str(), levelThreeXPos, levelOneYPos);
			secondsReadout.str(clearTextStream);

			//Diplays the players speed in the bottom left of the screen
			speedReadout << playerSpeedText << speed << speedReadoutText;
			gameFont->Draw(speedReadout.str(), levelOneXPos, levelOneYPos);
			speedReadout.str(clearTextStream);

			//Displays the players current lap in the bottom right of the screen
			display << lapText << currentLap << slashChar << numberOfLaps;
			gameFont->Draw(display.str(), adjustedlevelThreeXPos, adjustedLevelTwoYPos);
			display.str(clearTextStream);

			//Calls the DisplayText function to print the players health and the game state text to the screen
			DisplayText(playerHealthText, gameFont, playersHealth, levelOneXPos, levelTwoYPos, clearTextStream);
			DisplayText(gamePlayingText, gameFont, NULL, gameStateTextXPos, gameStateTextYPos, clearTextStream);

			//Checks if the PauseKey has been pressed and then changes the game state
			//to either playing or paused dependingon what it was originally
			if (myEngine->KeyHit(PauseKey))
			{

				gamePause = !gamePause;
				enumGameState = gamePause ? Paused : Playing;
			}

			//goes through each option in the cameraView depending on what its state is
			switch (cameraView)
			{
				//Sets the cameras; position to the Third person perspective 
			case ThirdPerson:

				gameCamera->SetLocalPosition(thirdPersonXPos, thirdPersonYPos, thirdPersonZPos);
				break;
				//Sets the cameras' position to the First person perspective
			case FirstPerson:

				gameCamera->SetLocalPosition(firstPersonXPos, firstPersonYPos, firstPersonZPos);
				break;
			}
			//Checks if the Number2Key has been pressed and then resets the cameras' rotation and movement
			//and changes the perspective to first person
			if (myEngine->KeyHit(Number2Key))
			{

				ResetCameraFunction(dummyCamera, cameraXMovement, cameraZMovement, cameraXRotationAmount, cameraYRotationAmount);
				cameraView = FirstPerson;
			}
			//Checks if the Numbe12Key has been pressed and then resets the cameras' rotation and movement
			//and changes the perspective to third person
			else if (myEngine->KeyHit(Number1Key))
			{

				ResetCameraFunction(dummyCamera, cameraXMovement, cameraZMovement, cameraXRotationAmount, cameraYRotationAmount);
				cameraView = ThirdPerson;
			}

			//Checks if the mouse has moved positively in the X axis and then rotates the camera accordingly
			if (mouseMovementX > zeroFloat)
			{

				cameraYRotationAmount -= timer * cameraRotationAmount;
				dummyCamera->RotateLocalY(-(timer * cameraRotationAmount));
			}
			//Checks if the mouse has moved negatively in the X axis and then rotates the camera accordingly
			else if (mouseMovementX < zeroFloat)
			{

				cameraYRotationAmount += timer * cameraRotationAmount;
				dummyCamera->RotateLocalY(timer * cameraRotationAmount);
			}
			//Checks if the mouse has moved positively in the Y axis and then rotates the camera accordingly
			if (mouseMovementY > zeroFloat)
			{

				cameraXRotationAmount += timer * cameraRotationAmount;
				dummyCamera->RotateLocalX(timer * cameraRotationAmount);
			}
			//Checks if the mouse has moved negatively in the Y axis and then rotates the camera accordingly
			else if (mouseMovementY < zeroFloat)
			{

				cameraXRotationAmount -= timer * cameraRotationAmount;
				dummyCamera->RotateLocalX(-(timer * cameraRotationAmount));
			}

			//Gets the current coordinates of the racecar and stores it in the xPos and zPos variables
			xPos = raceCarModel->GetX();
			zPos = raceCarModel->GetZ();

			//Gets the current coordinates of the racecar and adds/subtracts the movement restriction
			//and then stores them in the appropriate variable
			maxCameraX = xPos + cameraMovementMaxMovement;
			minCameraX = xPos - cameraMovementMaxMovement;
			maxCameraZ = zPos + cameraMovementMaxMovement;
			minCameraZ = zPos - cameraMovementMaxMovement;

			//Gets the matrix of the racecar model and then creates a vector2D variable using the Z row in the matrix
			raceCarModel->GetMatrix(&matrix[matrixStart][matrixStart]);
			vector2D facingVector = { matrix[twoInt][zeroInt],matrix[twoInt][twoInt] };

			//Allows the racecar to boost when the boostCooldown is equal to the initialBoostCooldown
			if (boostCooldown >= initialBoostCooldown)
			{
				//Alllows the boost when 'W' is held
				if (myEngine->KeyHeld(WKey))
				{
					//Allows the boosst when 'SPACE' is held and the players health is greater than '100'
					if (myEngine->KeyHeld(SpaceKey) && playersHealth >= healthNeededForBoost)
					{
						//When the boostTimer exceeds the threshold, reset the boostTimer and start the boostCooldown
						if (boostTimer >= boostThreshold)
						{

							boostTimer = zeroFloat;
							boostCooldown -= timer;
						}
						//When the boostTimer exceeds the warningTime, Let the user know and then increase the boostTimer
						else if (boostTimer >= boostWarningTime)
						{

							DisplayText(boostOverheatingText, gameFont, NULL, levelTwoXPos, levelThreeYPos, clearTextStream);
							boostTimer += timer;
						}
						else
						{
							//While the boost is active multiple the thrust vector by the multiplier and increment the boostTimer
							thrust = scalar((thrustForwardScalar * boostForwarMultiplier), facingVector);
							boostTimer += timer;

							//calls the DisplayText function to print the boostActiveText
							DisplayText(boostActiveText, gameFont, NULL, levelTwoXPos, levelThreeYPos, clearTextStream);
						}
					}

					else
					{
						//Set the thrust vector to the normal speed when 'SPACE' is not being held
						thrust = scalar(thrustForwardScalar, facingVector);
					}
				}
				//Checks if 'S' is being held down and then decreases the thrust vector by the backward scalar
				else if (myEngine->KeyHeld(SKey))
				{

					thrust = scalar(-thrustBackwardScalar, facingVector);
				}

				else
				{
					//If nothing is being held down, set the thrust vector to Zero
					thrust = { zeroFloat, zeroFloat };
				}
				//Always set the drag vector to the dragScalar amount multiplied by the current momentum
				drag = scalar(coefficientOfDrag, momentum);
			}
			//if (boostCooldown >= initialBoostCooldown)

			else
			{
				//When the boostCooldown does not equal the initialBoostCooldown, 
				//multiply the drag vector by the forwardMultiplier, increasing the drag vector
				drag = scalar(coefficientOfDrag * thrustForwardMultiplier, momentum);

				//If the boosColldown is less than zero reset it back to its initial value
				if (boostCooldown <= zeroFloat)
				{

					boostCooldown = initialBoostCooldown;
				}

				else
				{
					//decrement the boostCooldown
					boostCooldown -= timer;
				}
				//Checks if 'S' is being held down and then decreases the thrust vector by the backward scalar
				if (myEngine->KeyHeld(SKey))
				{

					thrust = scalar(-thrustBackwardScalar, facingVector);
				}
				//Checks if 'W' is being held down and then increases the thrust vector by its normal speed
				else if (myEngine->KeyHeld(WKey))
				{

					thrust = scalar(thrustForwardScalar, facingVector);
				}

				else
				{
					//If nothing is being held down, set the thrust vector to Zero
					thrust = { zeroFloat, zeroFloat };
				}
				//calls the DisplayText function to print the boostCooldownText
				DisplayText(boostCooldownText, gameFont, ceil(boostCooldown), levelTwoXPos, levelThreeYPos, clearTextStream);
			}

			//Rotates the racecar model by the 'rotationAmount' clockwise around the Y axis when 'D' is held
			if (myEngine->KeyHeld(DKey))
			{

				raceCarModel->RotateY(rotationAmount);
			}
			//Rotates the racecar model by the 'rotationAmount' anti-clockwise around the Y axis when 'a' is held
			if (myEngine->KeyHeld(AKey))
			{

				raceCarModel->RotateY(-rotationAmount);
			}

			//Checks if the UpKey is being held down and then moves the cameras' position forward
			if (myEngine->KeyHeld(UpKey))
			{
				//Checks if the cameras' position has exceeded its limit
				if (dummyCamera->GetLocalZ() >= maxCameraZ)
				{
					//Does nothing, so the camera does not move
				}

				else
				{
					//Moves the camera at the correct speed and stores how far it has moved
					//to be able to reset the camera
					cameraZMovement += (timer * cameraRotationAmount);
					dummyCamera->MoveLocalZ(timer * cameraRotationAmount);
				}
			}
			//Checks if the UpKey is being held down and then moves the cameras' position backward
			else if (myEngine->KeyHeld(DownKey))
			{
				//Checks if the cameras' position has exceeded its limit
				if (dummyCamera->GetLocalZ() <= minCameraZ)
				{
					//Does nothing, so the camera does not move
				}

				else
				{
					//Moves the camera at the correct speed and stores how far it has moved
					//to be able to reset the camera
					cameraZMovement -= (timer * cameraRotationAmount);
					dummyCamera->MoveLocalZ(-(timer * cameraRotationAmount));
				}
			}
			//Checks if the UpKey is being held down and then moves the cameras' position to the left
			else if (myEngine->KeyHeld(LeftKey))
			{
				//Checks if the cameras' position has exceeded its limit
				if (dummyCamera->GetLocalX() <= minCameraX)
				{
					//Does nothing, so the camera does not move
				}

				else
				{
					//Moves the camera at the correct speed and stores how far it has moved
					//to be able to reset the camera
					cameraXMovement -= (timer * cameraRotationAmount);
					dummyCamera->MoveLocalX(-(timer * cameraRotationAmount));
				}
			}
			//Checks if the UpKey is being held down and then moves the cameras' position to the right
			else if (myEngine->KeyHeld(RightKey))
			{
				//Checks if the cameras' position has exceeded its limit
				if (dummyCamera->GetLocalX() >= maxCameraX)
				{
					//Does nothing, so the camera does not move
				}

				else
				{
					//Moves the camera at the correct speed and stores how far it has moved
					//to be able to reset the camera
					cameraXMovement += (timer * cameraRotationAmount);
					dummyCamera->MoveLocalX(timer * cameraRotationAmount);
				}
			}

			//Sets the momentum vector of the racecar to the sum of all three vectors
			momentum = sum3(momentum, thrust, drag);
			//calculates the speed by calculating the distance of the momentum vector multiplied by the mapScale 
			//and then multiplied by the amount needed for the speed to be in Km/h
			speed = floor((sqrt(momentum.x * momentum.x + momentum.z * momentum.z) * mapScale * conversionToKilo));

			//Calculates the racecars' new coordinates by adding the racecars' momentum to its current coordinates
			newXPos = xPos + momentum.x;
			newZPos = zPos + momentum.z;

			//Goes through each enemy in the mapObjects vector
			for (int i = zeroInt; i < numberOfEnemies; i++)
			{
				//calculates the enemy's current coordinates 
				enemyXPos = allMapObjects[enemyCarIndex][i].model->GetLocalX();
				enemyZPos = allMapObjects[enemyCarIndex][i].model->GetLocalZ();

				//Checks if the racecar has collided with the current enemy
				Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, racecarRadius, enemyXPos, enemyZPos, enemyRadius, enemyRadius);
				switch (CollisionResponse)
				{

				case xAxis:
					//If there is a collision on the xAxis, the racecars' x momentum vector will negate itself and be divided by half
					//The players health will also be decremented
					momentum.x = -momentum.x / collisionResponseMultiplierBoundaries;
					playersHealth--;
					break;
				case zAxis:
					//If there is a collision on the zAxis, the racecars' z momentum vector will negate itself and be divided by half
					//The players health will also be decremented
					momentum.z = -momentum.z / collisionResponseMultiplierBoundaries;
					playersHealth--;
					break;
				case None:
					//If there is no collision, set the racecars' position to its calculated new  coordinates
					raceCarModel->SetPosition(newXPos, zeroFloat, newZPos);
					break;
				}

				//Goes through each of the enemys' vehicle state
				switch (allMapObjects[enemyCarIndex][i].enemyVehicleState)
				{
					//ResetView will set the enemy's scale to the same scale as the reacecar and will make the enemy lookat the next checkpoint
				case ResetView:

					allMapObjects[enemyCarIndex][i].model->LookAt(allMapObjects[dummyModelsIndex][allMapObjects[enemyCarIndex][i].enemyCheckpoints].model);
					allMapObjects[enemyCarIndex][i].model->ResetScale();
					allMapObjects[enemyCarIndex][i].model->Scale(halfScale);

					allMapObjects[enemyCarIndex][i].enemyVehicleState = MovingForward;
					break;
					//MovingForward will move the enemy forward and check if it has reached the next checkpoint and then reset the enemy
				case MovingForward:

					//Move the enemy forward by its speed multiplier
					allMapObjects[enemyCarIndex][i].speed = enemySpeed[i] * timer / enemyForwardMultiplier;

					//Check if the enemy has collided with the racecar model
					//and then set its position to where it was before the collision 
					if (sphere2SphereCollision2D(enemyXPos, enemyZPos, enemyRadius, xPos, zPos, racecarRadius))
					{

						allMapObjects[enemyCarIndex][i].model->SetPosition(enemyXPos, zeroFloat, enemyZPos);
					}

					else
					{
						//If there is no collision move the enemy forward
						allMapObjects[enemyCarIndex][i].model->MoveLocalZ(allMapObjects[enemyCarIndex][i].speed);
					}

					//Checks if the enemy has collided with the checkpoint and then set the enemys' checkpoint to the next one
					if (point2PointCollision2D(DistanceCalculation(enemyXPos, enemyZPos,
						allMapObjects[dummyModelsIndex][allMapObjects[enemyCarIndex][i].enemyCheckpoints].model->GetX(),
						allMapObjects[dummyModelsIndex][allMapObjects[enemyCarIndex][i].enemyCheckpoints].model->GetZ(), dummyEnemyDistance)))
					{

						allMapObjects[enemyCarIndex][i].enemyCheckpoints++;
						allMapObjects[enemyCarIndex][i].stopWatch = zeroFloat;
						allMapObjects[enemyCarIndex][i].enemyVehicleState = CheckPointReached;

					}
					break;
					//CheckPointReached will check if the last checkpoint has been reached and reset it and will reset the enemy
				case CheckPointReached:

					if (allMapObjects[enemyCarIndex][i].enemyCheckpoints == numberOfDummyModels)
					{
						//If the enemy has reached the final checkpoint, reset it back to the first checkpoint and then reset the enemy
						allMapObjects[enemyCarIndex][i].enemyCheckpoints = zeroInt;
						allMapObjects[enemyCarIndex][i].enemyVehicleState = ResetView;
					}
					else
					{

						allMapObjects[enemyCarIndex][i].enemyVehicleState = ResetView;
					}

					break;
				}
			}
			//for (int i = zeroInt; i < numberOfEnemies; i++)

			//goes through each checkpoint in the map Objects vector
			for (int i = zeroInt; i < numberOfCheckpoints; i++)
			{
				//Checks if the checkpoint is rotated and then performs collision detection
				//between the checkpoints struts and the racecar model
				if (FileVector[checkPointIndex][i].rotation == zeroFloat)
				{
					//Performs collision detection between the checkpoint struts and the racecar model  
					if (sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() + checkpointHalfXWidth) - checkpointStrutRadius, (allMapObjects[checkPointIndex][i].model->GetZ() + checkpointHalfZWidth) - checkpointStrutRadius,
						checkpointStrutRadius, raceCarModel->GetX(), raceCarModel->GetZ(), racecarRadius) ||
						sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() - checkpointHalfXWidth) + checkpointStrutRadius, (allMapObjects[checkPointIndex][i].model->GetZ() - checkpointHalfZWidth) + checkpointStrutRadius,
							checkpointStrutRadius, raceCarModel->GetX(), raceCarModel->GetZ(), racecarRadius))
					{
						//If there is a collision, the racecars' x and z momentum vectors will negate themself and be divided by 1.3
						//The players health will also be decremented
						momentum.x = -momentum.x / collisionResponseMulitplierObstacles;
						momentum.z = -momentum.z / collisionResponseMulitplierObstacles;
						playersHealth -= 1;
					}
				}

				else
				{
					//Performs collision detection between the checkpoint struts and the racecar model 
					if (sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() + checkpointHalfZWidth) - checkpointStrutRadius, (allMapObjects[checkPointIndex][i].model->GetZ() + checkpointHalfXWidth) - checkpointStrutRadius,
						checkpointStrutRadius, raceCarModel->GetX(), raceCarModel->GetZ(), racecarRadius) ||
						sphere2SphereCollision2D((allMapObjects[checkPointIndex][i].model->GetX() - checkpointHalfZWidth) + checkpointStrutRadius, (allMapObjects[checkPointIndex][i].model->GetZ() - checkpointHalfXWidth) + checkpointStrutRadius,
							checkpointStrutRadius, raceCarModel->GetX(), raceCarModel->GetZ(), racecarRadius))
					{
						//If there is a collision, the racecars' x and z momentum vectors will negate themself and be divided by 1.3
						//The players health will also be decremented
						momentum.x = -momentum.x / collisionResponseMulitplierObstacles;
						momentum.z = -momentum.z / collisionResponseMulitplierObstacles;
						playersHealth -= 1;
					}
				}

				//Checks if the enumGameLaps state is equal to Lap and then runs the code to check the current lap
				if (enumGameLaps == Lap)
				{
					//checks if the checkpoint is not passed and that the correct checkpoint is being driven through
					if (allMapObjects[checkPointIndex][i].checkpoint == NotPassed && allMapObjects[checkPointIndex][i].checkpointnumber == stageNumber)
					{
						//Checks if the checkpoint is rotated and then performs collision detection
						//between the checkpoints' box and the racecar model
						if (FileVector[checkPointIndex][i].rotation == zeroFloat)
						{
							//performs collision detection with the box around the checkpoint 
							if (CheckPointCollision(xPos, zPos, allMapObjects[checkPointIndex][i].model->GetX(), allMapObjects[checkPointIndex][i].model->GetZ(), checkpointHalfXWidth, checkpointHalfZWidth))
							{

								allMapObjects[crossEffectIndex][i].specialEffectState = Activated;
							}
						}

						else
						{
							//performs collision detection with the box around the checkpoint
							if (CheckPointCollision(xPos, zPos, allMapObjects[checkPointIndex][i].model->GetX(), allMapObjects[checkPointIndex][i].model->GetZ(), checkpointHalfZWidth, checkpointHalfXWidth))
							{

								allMapObjects[crossEffectIndex][i].specialEffectState = Activated;
							}
						}

						//Goes through each of the specialEffectStates
						switch (allMapObjects[crossEffectIndex][i].specialEffectState)
						{
							//Activated will set the checkpoints cross position to aboveground and will resets its stopwatch
						case Activated:

							allMapObjects[crossEffectIndex][i].model->SetY(abovegroundYPosition);
							allMapObjects[crossEffectIndex][i].specialEffectState = Waiting;
							allMapObjects[crossEffectIndex][i].stopWatch = zeroFloat;
							break;
						case Waiting:
							//Waiting will keep the cross aboveground for 2 seconds and then reset it back to belowground

							//checks if the cross has been aboveground for greater than 2 seconds
							if (allMapObjects[crossEffectIndex][i].stopWatch >= crossStopWatchLimit)
							{
								//sets the cross below ground and updates the checkpoint to being passed
								//and increments the stage number
								allMapObjects[crossEffectIndex][i].model->SetY(undergroundYPosition);
								allMapObjects[checkPointIndex][i].checkpoint = Passed;
								stageNumber++;
								allMapObjects[crossEffectIndex][i].specialEffectState = NotActivated;

								//Checks if the checkpoint is the final checkpoint and then completes the current lap
								if (allMapObjects[checkPointIndex][i].checkpointnumber == (numberOfCheckpoints - oneInt))
								{

									enumGameLaps = LapComplete;
								}
							}

							else
							{
								//increment the crosses stopwatch
								allMapObjects[crossEffectIndex][i].stopWatch += timer;

								//Checks if the checkpoint stage is equal to the finalStage and then displays the correct information
								if (allMapObjects[checkPointIndex][i].checkpointnumber == (numberOfCheckpoints - oneInt))
								{
									//Displays the current lap text, telling the user that the lap is done
									display << lapText << currentLap << completeText;
									gameFont->Draw(display.str(), levelTwoXPos, levelTwoYPos);
									display.str(clearTextStream);
								}

								else
								{
									//Diplays the current stage 
									display << stageText << (i + stageIncrement) << completeText;
									gameFont->Draw(display.str(), levelTwoXPos, levelTwoYPos);
									display.str(clearTextStream);
								}
							}
							break;
						case NotActivated:
							//NotActivated will do nothing to make sure that the cross does not reappear until the lap is complete
							break;
						}
					}
					//if (allMapObjects[checkPointIndex][i].checkpoint == NotPassed && allMapObjects[checkPointIndex][i].checkpointStages == stageNumber)

				}
				//if (enumGameLaps == Lap)

				//Checks if the enumGameLaps state is equal to LapComplete and then runs the code to increment the lap
				else if (enumGameLaps == LapComplete)
				{
					//Increments the lap
					currentLap++;
					stageNumber = zeroInt;

					//Goes through each checkpoint and resets it back to default
					for (int i = zeroInt; i < numberOfCheckpoints; i++)
					{
						allMapObjects[checkPointIndex][i].checkpoint = NotPassed;
					}

					//Checks if the currentLap is greater than the total number of laps
					//and then completes the game
					if (currentLap > numberOfLaps)
					{

						enumGameState = Over;
					}

					else
					{
						//If the currentLap is not greater than the total number of laps
						//set the lap state to Lap
						enumGameLaps = Lap;
					}
				}
			}
			//for (int i = zeroInt; i < numberOfCheckpoints; i++)

			//Goes through each Wall in the map Objects vector
			for (int i = zeroInt; i < numberOfWalls; i++)
			{
				//Performs collision detection between the wall and the racecar model
				Collision CollisionResponse = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, racecarRadius, allMapObjects[wallIndex][i].model->GetX(), allMapObjects[wallIndex][i].model->GetZ(), wallXHalfDepth, wallZHalfDepth);
				switch (CollisionResponse)
				{

				case xAxis:
					//If there is a collision on the xAxis, the racecars' x momentum vector will negate itself and be divided by half
					//The players health will also be decremented
					momentum.x = -momentum.x / collisionResponseMultiplierBoundaries;
					playersHealth--;
					break;
				case zAxis:
					//If there is a collision on the zAxis, the racecars' z momentum vector will negate itself and be divided by half
					//The players health will also be decremented
					momentum.z = -momentum.z / collisionResponseMultiplierBoundaries;
					playersHealth--;
					break;
				case None:
					//If there is no collision, set the racecars' position to its calculated new  coordinates
					raceCarModel->SetPosition(newXPos, zeroFloat, newZPos);
				}
			}

			//Goes through each Isle in the map Objects vector
			for (int i = zeroInt; i < numberOfIsles; i++)
			{
				//Performs collision detection between the isle and the racecar model
				Collision CollisionResponse1 = sphere2BoxCollision2D(xPos, zPos, newXPos, newZPos, racecarRadius, allMapObjects[isleIndex][i].model->GetX(), allMapObjects[isleIndex][i].model->GetZ(), isleRadius, isleRadius);
				switch (CollisionResponse1)
				{

				case xAxis:
					//If there is a collision on the xAxis, the racecars' x momentum vector will negate itself and be divided by half
					//The players health will also be decremented
					momentum.x = -momentum.x / collisionResponseMultiplierBoundaries;
					playersHealth--;
					break;
				case zAxis:
					//If there is a collision on the zAxis, the racecars' z momentum vector will negate itself and be divided by half
					//The players health will also be decremented
					momentum.z = -momentum.z / collisionResponseMultiplierBoundaries;
					playersHealth--;
					break;
				case None:
					//If there is no collision, set the racecars' position to its calculated new  coordinates
					raceCarModel->SetPosition(newXPos, zeroInt, newZPos);
				}
			}

			//Goes through each WaterTank in the map Objects vector
			for (int i = zeroInt; i < numberOfWaterTanks; i++)
			{
				//Performs collision detection between the isle and the racecar model
				if (sphere2SphereCollision2D(allMapObjects[waterTankIndex][i].model->GetX(), allMapObjects[waterTankIndex][i].model->GetZ(), tankRadius, raceCarModel->GetX(), raceCarModel->GetZ(), racecarRadius))
				{
					//If there is a collision, the racecars' x and z momentum vectors will negate themself and be divided by 1.3
					//The players health will also be decremented
					momentum.x = -momentum.x / collisionResponseMulitplierObstacles;
					momentum.z = -momentum.z / collisionResponseMulitplierObstacles;
					playersHealth--;
				}
			}

		}
		//Else if enumGameState == Playing

		//Checks if the game state is equal to 'Paused'
		else if (enumGameState == Paused)
		{
			//Displays the players current lap in the bottom right of the screen
			display << lapText << currentLap << slashChar << numberOfLaps;
			gameFont->Draw(display.str(), adjustedlevelThreeXPos, adjustedLevelTwoYPos);
			display.str(clearTextStream);

			//Diplays the players speed in the bottom left of the screen
			speedReadout << playerSpeedText << speed << speedReadoutText;
			gameFont->Draw(speedReadout.str(), levelOneXPos, levelOneYPos);
			speedReadout.str(clearTextStream);

			//Displays the gameTime in the bottom right of the screen
			secondsReadout << gameTimeText << int(playerMinutes) << mChar << spaceChar << int(playerSeconds) << sChar;
			gameFont->Draw(secondsReadout.str(), levelThreeXPos, levelOneYPos);
			secondsReadout.str(clearTextStream);

			//Calls the DisplayText function to print the players health and the game state text to the screen
			DisplayText(playerHealthText, gameFont, playersHealth, levelOneXPos, levelTwoYPos, clearTextStream);
			DisplayText(gamePausedText, gameFont, NULL, gameStateTextXPos, gameStateTextYPos, clearTextStream);

			//Checks if the PauseKey has been pressed and then changes the game state
			//to either playing or paused dependingon what it was originally
			if (myEngine->KeyHit(PauseKey))
			{

				gamePause = !gamePause;
				enumGameState = gamePause ? Paused : Playing;
			}
		}

		else if (enumGameState == Over)
		{
			//calls the DisplayText to print the restart text to the screen
			DisplayText(restartText, startFont, NULL, buttonOneXCoordinate, buttonOneYCoordinate, clearTextStream);

			//Checks if the the left mouse button has been pressed within the text limits
			if (mouseCoordX >= buttonOneXCoordinate && mouseCoordX <= (buttonOneXCoordinate + buttonXLimit) &&
				mouseCoordY >= buttonOneYCoordinate && mouseCoordY <= (buttonOneYCoordinate + buttonYLimit) && myEngine->KeyHit(LeftClick))
			{
				//Removes the racecar model to be reset
				raceMesh->RemoveModel(raceCarModel);

				//Goes through each checkpoint and resets its position on the lap 
				for (int i = zeroInt; i < numberOfCheckpoints; i++)
				{

					allMapObjects[checkPointIndex][i].checkpoint = NotPassed;
				}

				//goes through each enemy and removes their models to be reset and also resets their checkpoint positions
				for (int i = zeroInt; i < numberOfEnemies; i++)
				{
					enemyCarMesh->RemoveModel(allMapObjects[enemyCarIndex][i].model);
					allMapObjects[enemyCarIndex][i].enemyCheckpoints = zeroInt;
					allMapObjects[enemyCarIndex][i].enemyVehicleState = ResetView;
				}

				//Sets the loadedIn variable to false to go through the creation of each game object
				loadedIn = false;

				//Resets all the variables used in the lap loop and the players health
				stageNumber = zeroInt;
				currentLap = startingLap;
				playersHealth = startingPlayersHealth;

				//set the game state to 'Start'
				enumGameState = Start;
			}

			//Displays the players current lap in the bottom right of the screen
			display << lapText << currentLap << slashChar << numberOfLaps;
			gameFont->Draw(display.str(), adjustedlevelThreeXPos, adjustedLevelTwoYPos);
			display.str(clearTextStream);

			//Displays the gameTime in the bottom right of the screen
			secondsReadout << gameTimeText << int(playerMinutes) << mChar << spaceChar << int(playerSeconds) << sChar;
			gameFont->Draw(secondsReadout.str(), levelThreeXPos, levelOneYPos);
			secondsReadout.str(clearTextStream);

			//calls ths DisplayText function to print the players health and the game state text to the screen
			DisplayText(playerHealthText, gameFont, playersHealth, levelOneXPos, adjustedLevelTwoYPos, clearTextStream);
			DisplayText(gameOverText, gameFont, NULL, gameStateTextXPos, gameStateTextYPos, clearTextStream);
		}
	}
	//While myEngine->IsRunning()

	//Deletes the gameEngine
	myEngine->Delete();
}



//Structure data type used to calculate the speed of each vector for the racecar model
vector2D scalar(float s, vector2D v)
{

	return {s * v.x, s * v.z};
}
//Structure data type used to calculate the actual speed using all three vectors of the racecar model
vector2D sum3(vector2D v1, vector2D v2, vector2D v3)
{

	return {v1.x + v2.x + v3.x, v1.z + v2.z + v3.z};
}

//Function to load and create all the models in the all Objects vector into the program
void InitialiseMap(Map& theObject, IMesh* mesh, float XPos, float ZPos, float rotationAmount, float scaleAmount)
{

	theObject.model = mesh->CreateModel(XPos, 0.0f,ZPos);
	theObject.model->RotateY(rotationAmount);
	theObject.model->Scale(scaleAmount);
}

//Function that takes coordinates from two objects 
//and calculates the distance between them and returns 
//whether they are within a certain amount of eachother
bool DistanceCalculation(float smallXPos, float smallZPos, float largeXPos, float largeZPos, float sphereRadius)
{

	float x = smallXPos - largeXPos;
	float z = smallZPos - largeZPos;

	float Distance = sqrt((x*x) + (z*z));
	if (Distance <= sphereRadius)
	{

		return true;
	}

	else
	{

		return false;
	}
}

//Function that checks if there has been a collision between a Sphere and a Box
Collision sphere2BoxCollision2D(float sphereOldX, float sphereOldZ, float sphereX, float sphereZ,
	float sphereRadius, float boxX, float boxZ, float halfBoxWidth, float halfBoxDepth)

{

	//Calculates the min and max X and Z coordinates of the surrounding box of the object
	float maxX = boxX + halfBoxWidth + sphereRadius;
	float minX = boxX - halfBoxWidth - sphereRadius;
	float maxZ = boxZ + halfBoxDepth + sphereRadius;
	float minZ = boxZ - halfBoxDepth - sphereRadius;

	//Checks if the spheres' centre is somewhere in the box coordinates
	if (sphereX > minX && sphereX < maxX && sphereZ > minZ && sphereZ < maxZ)
	{
		//Checks if the spheres' centre was around the X coordinates of the box
		if (sphereOldX < minX || sphereOldX > maxX)
		{
			
			return xAxis;
		}
		//Checks if the spheres' centre was around the Z coordinates of the box 
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

//Function that checks if the objects is between the box surrounding the checkpoints
bool CheckPointCollision(float pointX, float pointZ, float checkPointX, float checkPointZ, float halfXWidth, float halfZWidth)
{
	//Calculates the min and max X and Z coordinates of the checkpoints box
	float checkpointMaxX = checkPointX + halfXWidth;
	float checkpointMinX = checkPointX - halfXWidth;
	float checkpointMinZ = checkPointZ - halfZWidth;
	float checkpointMaxZ = checkPointZ + halfZWidth;

	//Checks if the spheres' centre is somewhere in the box coordinates
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
//Function that checks the distance between two objects and 
//returns true if it is less than a certain threshold
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

//Function that calculates the distance between two objects
//and checks if it is less than the two objects radii added together
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

//Function that resets the cameras' position and orientation 
void ResetCameraFunction(IModel* dummyCamera, float &cameraXMovement, float &cameraZMovement,
						float &cameraXRotationAmount, float &cameraYRotationAmount)
{

	dummyCamera->MoveLocalX(-cameraXMovement);
	dummyCamera->MoveLocalZ(-cameraZMovement);
	dummyCamera->RotateLocalX(-cameraXRotationAmount);
	dummyCamera->RotateLocalY(-cameraYRotationAmount);
	cameraXMovement = 0.0f;
	cameraXRotationAmount = 0.0f;
	cameraZMovement = 0.0f;
	cameraYRotationAmount = 0.0f;
}

//Function that displays text to the screen using a font and the text or number entered
void DisplayText(string displayText, IFont* displayFont, float displayNumber, int xCoordinate, int yCoordinate, string clearStream)
{ 
	//Creates a stream to send the text out to the screen
	stringstream displayStream;

	//Checks if there is no number required to be printed and the only displays the text
	if (displayNumber == NULL)
	{

		displayStream << displayText;
	}
	//Checks if there is no text required to be printed and only displays the number
	else if (displayText == clearStream)
	{

		displayStream << displayNumber;
	}
	//sets the stream to the entered number and text
	else
	{

		displayStream << displayText << displayNumber;
	}
	//Displays the stream that was edited earlier
	displayFont->Draw(displayStream.str(), xCoordinate, yCoordinate);
	//Clears the stream
	displayStream.str(clearStream);
}

//Function that opens the file to be used when reading the game objects coordinates
void OpenFile(ifstream& infile, string text)
{
	string errorText   = "ERROR: ";
	string reasonText  = "Can't open file";

	//Opens the file 
	infile.open(text);

	//Checks if the file could not be opened
	if (!infile)
	{
		cout << errorText;
		cout << reasonText;
	}
}

//Function that reads in the coordinates for each game object from the file
void ReadFile(ifstream &infile, vector<vector<ObjectLoading>> &inputArray, int& checkpointNum, int& isleNum, int& wallNum, int& crossNum, int& dummyNum, int& tankNum, int& enemyNum)
{
	//Initialises the variables used for reading each line in the text file
	string ObjectType;
	float XCoordinate;
	float ZCoordinate;
	float RotationAmount;
	float Scale;

	//strings for each game object in the file
	string checkpointText  = "Checkpoint";
	string isleText        = "Isle";
	string wallText        = "Wall";
	string watertankText   = "Tank";
	string crossText       = "Cross";
	string dummyModelText  = "Dummy";
	string enemyText       = "enemy";

	//variables for every object in the vector
	int checkpointIncrement  = 0;
	int crossIncrement       = 0;
	int enemyIncrement       = 0;
	int tankIncrement        = 0;
	int dummyIncrement       = 0;
	int isleIncrement        = 0;
	int wallIncrement        = 0;

	//Game object indexes
	const int checkPointIndex   = 0;
	const int isleIndex         = 1;
	const int wallIndex         = 2;
	const int waterTankIndex    = 3;
	const int crossEffectIndex  = 4;
	const int dummyModelsIndex  = 5;
	const int enemyCarIndex     = 6;

	//Goes through each variable in the line of the text file and stores them in the appropriate variables
	while (infile >> ObjectType && infile >> XCoordinate && infile >> ZCoordinate && infile >> RotationAmount && infile >> Scale)
	{
		//Checks if the objectType in the file is the checkpoint and then stores the information in the objectLoading vector
		if (ObjectType == checkpointText)
		{ 

			inputArray[checkPointIndex][checkpointIncrement].x = XCoordinate;
			inputArray[checkPointIndex][checkpointIncrement].z = ZCoordinate;
			inputArray[checkPointIndex][checkpointIncrement].rotation = RotationAmount;
			inputArray[checkPointIndex][checkpointIncrement].scale = Scale;
			checkpointIncrement++;
		}
		//Checks if the objectType in the file is the isle and then stores the information in the objectLoading vector
		else if (ObjectType == isleText)
		{

			inputArray[isleIndex][isleIncrement].x = XCoordinate;
			inputArray[isleIndex][isleIncrement].z = ZCoordinate;
			inputArray[isleIndex][isleIncrement].rotation = RotationAmount;
			inputArray[isleIndex][isleIncrement].scale = Scale;
			isleIncrement++;
		}
		//Checks if the objectType in the file is the wall and then stores the information in the objectLoading vector
		else if (ObjectType == wallText)
		{

			inputArray[wallIndex][wallIncrement].x = XCoordinate;
			inputArray[wallIndex][wallIncrement].z = ZCoordinate;
			inputArray[wallIndex][wallIncrement].rotation = RotationAmount;
			inputArray[wallIndex][wallIncrement].scale = Scale;
			wallIncrement++;
		}
		//Checks if the objectType in the file is the watertank and then stores the information in the objectLoading vector
		else if (ObjectType == watertankText)
		{

			inputArray[waterTankIndex][tankIncrement].x = XCoordinate;
			inputArray[waterTankIndex][tankIncrement].z = ZCoordinate;
			inputArray[waterTankIndex][tankIncrement].rotation = RotationAmount;
			inputArray[waterTankIndex][tankIncrement].scale = Scale;
			tankIncrement++;
		}
		//Checks if the objectType in the file is the cross and then stores the information in the objectLoading vector
		else if (ObjectType == crossText)
		{

			inputArray[crossEffectIndex][crossIncrement].x = XCoordinate;
			inputArray[crossEffectIndex][crossIncrement].z = ZCoordinate;
			inputArray[crossEffectIndex][crossIncrement].rotation = RotationAmount;
			inputArray[crossEffectIndex][crossIncrement].scale = Scale;
			crossIncrement++;
		}
		//Checks if the objectType in the file is the dummyModel and then stores the information in the objectLoading vector
		else if (ObjectType == dummyModelText)
		{

			inputArray[dummyModelsIndex][dummyIncrement].x = XCoordinate;
			inputArray[dummyModelsIndex][dummyIncrement].z = ZCoordinate;
			inputArray[dummyModelsIndex][dummyIncrement].rotation = RotationAmount;
			inputArray[dummyModelsIndex][dummyIncrement].scale = Scale;
			dummyIncrement++;
		}
		//Checks if the objectType in the file is the enemy and then stores the information in the objectLoading vector
		else if (ObjectType == enemyText)
		{

			inputArray[enemyCarIndex][enemyIncrement].x = XCoordinate;
			inputArray[enemyCarIndex][enemyIncrement].z = ZCoordinate;
			inputArray[enemyCarIndex][enemyIncrement].rotation = RotationAmount;
			inputArray[enemyCarIndex][enemyIncrement].scale = Scale;
			enemyIncrement++;
		}

	}

	//Stores the number of times each object is encountered and then returns them
	checkpointNum = checkpointIncrement;
	isleNum = isleIncrement;
	wallNum = wallIncrement;
	crossNum = crossIncrement;
	dummyNum = dummyIncrement;
	tankNum = tankIncrement;
	enemyNum = enemyIncrement;
}