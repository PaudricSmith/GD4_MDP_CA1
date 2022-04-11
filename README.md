## GD4_MDP_CA2 Paudric Smith and Loïc Dornel

## Totaled Tanks

### Description
Multiplayer Distributed Programming assignment video game made using SFML/C++.
A local 2 player and multiplayer top down video game based on the classic Tanks game.

### Git-Rules
- **Create new branch for new feature**
	- Pull request to parent branch when done
- **Single responsibility commits**
	- Commit often ( Helps merging )
	- Try to keep adding files and changing files in seperate commits if possible
	- For commits = [ added feature, script and implemeted it ] < [added base] [wrote script 1] [implemented script 1 in base] [fixed script for base]

## TODO
- [x] Green team wins text shows if green tank is left and Yellow team wins text shows if yellow tank is left.
- [x] Fix escape key skipping in game menu straight to the main menu.
- [x] Keep track of Tank rotation
- [x] Keep track of Tank Cannon rotation
- [x] Make pickups that both Client and Server can see and use
- [ ] Wall collision

## BACKLOG
- [ ] Add Lobby State for players to join before and after the game.

## BUGS
- [x] Tank starts with super speed but if it rotates first then it won't have super speed
- [x] Tanks don't rotate the same on the server
- [x] If host dies his game quits
- [x] Black background
- [x] Tanks spawn outside of the game area
