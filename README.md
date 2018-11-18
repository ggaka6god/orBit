# orBit

orBit is a singleplayer platfomer game made by Andres Saladrigas and Aitor Simona, two students od second year of Video Game Design and Development degree in UPC Barcelona, Spain.
the gmae includes two levels, with usual mechanics. player can move right, left, jump and double jump
The game has been made using C++, using SDL and pugi libraries. The maps have been made using Tiled portable V0.17.

## Controls
### Player movement:

- A: move to the left;

- D: move to the right

- SPACE: jump

- SPACE in mid air: doble jump

### Debug keys:
- F1: start from the very first level
- F2: start from th beginning of current level
- F5: save current state
- F6: Load the previous state
- F9: view collider
- F10: GodMode
- 1: Go to 1st Stage
- 2: Go to 2nd Stage

## Authors
 - Andrés Saladrigas
github: https://github.com/TheArzhel

- Aitor Simona
github:https://github.com/AitorSimona

## Project Github
	
	https://github.com/TheArzhel/orBit

## Project Trello

	https://trello.com/b/M2TV7zXz/development

### Disclosure

We do not own any of the sprites, tilesets, music or sfx presented in this game. Authors credited below

- Player sprites:
https://jesse-m.itch.io/jungle-pack

- Tilesets & songs:
https://ansimuz.itch.io/sunnyland-forest
https://soumitrashewale.itch.io/sunnyland

- SFX were generated with BFXR
https://www.bfxr.net/

## License

© Andrés Ricardo Saladrigas Pérez

Licensed under the [MIT License](LICENSE.txt)


## Innovation

### Additional fetures added

- 1: Go to 1st Stage

- 2: Go to 2nd Stage

- Checkpoint collider: creating checkpoints to save progress. If player dies he will start from checkpoints

- Animations on Tiled: all animations are in Tiled. Inside folder Game/animation. player animation is a xml file we read to interpret data

- Camera: Only tiles inside the camera are printed 

-Colliders: Only colliders that are in camera are checked in module collisions Update

