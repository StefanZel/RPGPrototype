# 3D Story-driven RPG
This nameless RPG game is a passion project made in Unreal Engine using mainly C++. Currently, it is in the early prototype stage where I am testing various Unreal features and simultaneously deepening my understanding of Game Development and C++.

You can find some details about this project in the [Notable Features](#notable-features).

## Introduction
Inspired by Baldurs Gate 3 and Path of Exile I started this project to expand my game development understanding, my portfolio, and more importantly, to create something unique.

The two games that served as my basis motivated me to create something rich in story-telling, world-building, and game aspects such as strategy and crafting. I strive to bring all of this to this project and hopefully create something beautiful.

The main focus here will be how to do all of this. How to make a system that will be able to be extended based on project needs... How to handle the sheer versatility of everything I want to add... And more importantly - how to ensure that the game performance is top-notch.

So, throughout this project, my main focus will be on performance and readability!!!

## Notable Features
Each section will explain briefly how the system works without going deep into technical details. There will be small highlights of important classes/functions that the feature is based around if anyone wants to explore the code.

>[!NOTE]
>This list will grow as I keep working on the project. Some smaller features such as Camera Movement, Decals, and Entity Selection are not deemed necessary to be mentioned here.

 [Easily Manageable Game States](#easily-manageable-game-states)

 [Dynamic Spawning of Actors](#dynamic-spawning-of-actors)
 - [Asset Manager](#asset-manager)

 [Command System](#command-system)

## Easily Manageable Game States
For the purpose of performance and clarity, I decided to break major game processes apart:

```cpp
UENUM()
enum class EGamePhaseState
{
	Loading, // Generic Phase
	GenerateNewGameData, // Create data for entities and environment
	LoadGameData, // Load data assets
	Initiate, // Create entities based on loaded data
	Setup, // Adjust possible changes
	PreLoad, // Check if everything is alright
	PreGameplay, // Enable player interaction
	Gameplay,
	PostGameplay, // Handle the aftermath, possibly start asynchronous loading of assets for the next level
	Exit
};
```
These states are all represented by delegates to which different functions are bound. GameStateBaseMain class is responsible for executing them in an orderly manner, which allows for easy and more performative distribution of important game tasks.

## Dynamic Spawning of Actors
To minimize the reliance on the editor and introduce the bigger possibility of 'randomness'- every interactive actor in the map will spawn dynamically. <code>GameModeMain</code> class carries the task of loading all necessary assets.

 - ### Asset Manager
Map in itself will only have environmental assets, everything else that the Player or Ai interacts with would be spawned using Asset Manager.

Asset Manager will allow for assets to be loaded in memory only if needed. In theory, using Asset Manager can be considered overkill for small projects such as this one. However, the idea here is to have a vast number of different Characters, Items, and Interactive environments, so the performance increase Asset Manager provides can be considered necessary in this case.

## Command System
```cpp
#include "Commands/CommandBase.h"
#include "Commands/NavigationCommand.h"
```

Instead of relying heavily on Unreals Ai Blackboard, I have decided to have one system that would handle all possible interactions a player can have with Actors and Environments. </br>

A base command or <code>FEntities_CommandData</code> will contain all necessary and final data for one command to be executed, every other command type such as Navigation, Attack, and Spell will feed into the base and be executed based on its parameters </br>

An approach such as this would allow players to be able to Undo(Cancel some commands as they are happening), Redo(Execute the previous command with a click of a button), Bonus Action(Store one command to be able to predict Ai's actions), etc.</br>

Not only will this give me the opportunity to expand gameplay logic as I need, but it will also diversify gameplay by making it more strategic and engaging.</br>
