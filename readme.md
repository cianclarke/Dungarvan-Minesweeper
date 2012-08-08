Introduction
============
  
“Dungarvan Minesweeper” is a game based in Dungarvan made in a 3D environment, which can be interacted with and walked through.  

![Dungarvan Minesweeper](images/cover.png "Dungarvan Minesweeper")
  
Features
========
  
User interaction & Camera Control
---------------------------------
User interaction is achieved by using the by now almost standardized input keys of WASD. ‘W’ and ‘S’ are forward and backward movement, while ‘A’ and ‘D’ provide strafe functionality.   
In addition to this, mouse movement is catered for allowing the user to change their viewport in 3D space.  
  
Scoring, Winning & Loosing
--------------------------
The user progresses through the games by destroying the roaming NPC’s, which are sea mines. The user progresses from one level to the next by destroying all the NPC’s.   
The user has initially a total of ten lives, and one life is lost for every mine that hits the user. This also destroys the mine.
  
Levels
------
Level one has a total of 6 mines to destroy. Levels two to five have 11, 23, 30 and 50 mines respectively.  
As the user progresses from level to level, not only the number of landmines increases, but also their radius is reduced (more difficult to shoot), and their speed slowly increases.   
Each Level has one or more landmines “hidden” static in the town.   
  
3D Objects & Views
------------------
The main viewport of the game is of course 3D by nature, using a perspective viewport. The user starts in a third person view, but can switch to first person. ‘V’ toggles this view.   
The largest 3D object in the game is the town, created using Google Sketchup. Some reference was made to a town map, but the model is not to perfect scale. 
  
![Sketchup1](images/sketchup1.png "Google Sketchup showing a map of the town")
  
Every visible building face in the model has a texture from Google Streetview  
![Sketchup1](images/sketchup2.png "Google Sketchup showing a detail shot of the town")
  
Some barriers placed at street level limit roaming through the town map’s incomplete sections. These, along with the street lamps visible along the quays are the only 3D models in the level that have been imported from the Google 3D Warehouse. 
  
In addition to the town, a 3D model has been created to represent the mines consisting of several spheres varying in colour and radius. These mines are hierarchical. 
  
OpenGL Lighting
---------------
The main lighting element of the game is an imitation of sunlight. In creating this, OpenGL’s light 0 was used. This was given specular and diffuse properties, and a position. Then, global ambient, specular, diffuse and shininess properties are set.   
Finally, level 2 onwards introduces fog, using OpenGL’s ‘glFog’ functions.  
![Fog](images/fog.png "Fog")  
  
Skybox
------
A large cube was created for the purpose of a ‘skybox’, a large texture mapped object drawn around the camera to imitate being inside a sky, with sea on it’s floor.  
![Skybox](images/skybox.png "Skybox")  
The texture for this is freeware obtained from:  
http://www.hazelwhorley.com/textures.html  
The tutorial used to aide in the construction of the skybox is:  
http://sidvind.com/wiki/Skybox_tutorial  
  
Sound
-----
Basic sound is introduced by using the C function PlaySound()* to play various freeware WAV files found on the web for events such as firing a bullet, killing an NPC, losing a life and passing a level.   
  
Introduction cut scene
----------------------
An introduction is played when the game is first launched with some supporting text. This is a live rendered introduction, which is achieved by incrementing and decrementing the camera position as appropriate, until the camera has reached the players starting point. It serves the purpose of introducing the player to the game, it’s map and the objective. 
  
NPC's
-----
The NPC’s that roam the map (the sea mines) are placed at random (within some bounds) upon initialization, so no two sittings of the game will be the same. The NPC’s are a hierarchical object made up of spheres, and each enemy is representing by a C++ Class Instance. This means each individual enemy has it’s own ‘checkColission’ method, along with various other supporting methods. 
Collisions are checked for between the player and the NPC’s, and when one occurs the mine is destroyed and the player looses a life.   
The NPC’s bounce around the bounding box of the map, introducing another element of randomness to their direction.  
  
Bullets
-------
The concept of bullets being fired sounds conceptually quite simple but took two days to implement correctly.  
![Bullets](images/bullets.png "Bullets")  
The concept was that a small round sphere would be sent from the camera’s current coordinates out into 3D space at some speed. This was first implemented for one bullet using a C++ Class to track it’s whereabouts. The bullet checks for collision with NPC’s for every step it moves.   
The limitation of only having one bullet was resolved by creating a (predetermined size, 100 at present) array of bullet objects, which become instantiated as required. This limitation has not proved a problem in gameplay, but could be removed by making the array a vector instead.  
The bullet only moves through the X and Z axis, so does not vary in height – this was not a technical limitation, as the implementation would not have proved difficult – rather it was determined to suit the third person view better, and to make the game easier to play.   
After every frame, the bullet’s xPos and zPos are incremented by the bullet’s speed.   
Initially, the approach had been to perform a rotation around the x, y and z position of the camera, but this proved imprecise.  
The change in X and Z position is calculated by the cosine of the rotation in radians, and the sin of the rotation in radians.  
Each bullet is destroyed as it reaches the edge of the map.   
  
Function Overviews
-----------------
*character()* renders the 3d hierarchical animated object when third person view is active.   
*drawArm()* draws the animated arms of the above.   
*fireBullets()* firstly translates to the position of, then draws every active bullet in the scene. It also checks for collision between each NPC and each active bullet.  
checkLevelComplete()* is called as part of the above, so only when there are active bullets. As the name suggests, it checks if all NPC’s are dead.   
*camera()* rotates and translates the camera view to the characters current x, y and z position and rotation.   
*DrawIntro()* alters the camera’s viewport until it has reached the player’s starting point.   
*restart()* re-initializes globals, allowing the player to start again after dying.  
*skybox()* renders the 3D textured cube used to show the sky backdrop.  
*renderScene()* is the main loop function called every tick. All supporting code is called either directly or indirectly from this.  
*drawText()* is used to render character by character a bitmap string for onscreen text display.  
*renderBitmapString()* supports the above.  
*drawEnemy()* makes the OpenGL draw calls required to render each enemy.  
*updateScene()* increments variables such as the arm’s angle.  
*Keypress()* is the listener for keyboard input and provides option toggles.  
*keyup()* is a supporting function to enable multiple key entry at once.  
*keyboardMovement()* moves the character in the distance of the active movement, offset by a global multiplier.  
*mouseMove()* tracks difference in current and previous X and Y to determine the new viewport rotation.  
*mouseClick()* listens for click events, and on their occurance fires a bullet.   
*initEnemies()* initializes the global array of enemies using a for loop – the number of enemies can be altered by changing the value of a #define.  
*drawEnemies()* performs the required translations and rotations, then draws each NPC.  
*light()* sets up the initial lighting values (sunlight) and is called again if fog is enabled.  
