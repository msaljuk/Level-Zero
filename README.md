# Level Zero - a 3rd person multiplayer ActionRoguelike
CS193U - Videogame Development in Unreal - Project

## Final Project

For my final CS193U project, I've made the following additions/improvements:

#### 1. Buy Station to Buy Actions/Abilities/Respawn
I've created a buy station that allows players to buy given actions/abilities as well as respawn dead players in multiplayer. This Buy Station is an interactable actor in the world. Upon interaction, it creates a local widget that allows the player to browse purchasable abilities, read their details and select the one(s) they want to buy using their credits. See the following screenshots for more details:

![BuyStation](https://user-images.githubusercontent.com/62533326/99814838-5acfe080-2b6b-11eb-843f-3f4a2403143a.PNG)
BuyStation Actor

![BuyStationWidget](https://user-images.githubusercontent.com/62533326/99814846-5d323a80-2b6b-11eb-9617-a5fdedeb95ba.png)
BuyStationWidget UI

![BuyStationRespawn](https://user-images.githubusercontent.com/62533326/99814844-5d323a80-2b6b-11eb-9bda-bbbbca1017af.png)
BuyStation Respawn Player

#### 2. Double-Damage Buff
ActionEffect that changes the outgoing damage for its owner for 60 seconds. This damage multiplier is applied when the magic projectile deals damage.

#### 3. Players drop all of their credits on death into an Actor.
This Actor (SCoin - derived from SPowerUp) can be picked up by any other player by interacting with it, at which point they receive the dead player's credits. 

#### 4. Minor Improvements/Fixes 
I've also made several minor improvements and fixes in my project. Examples include incorporating past assignment feedback to make multiplayer rage replication more robust and redesigning the crosshair widget to appear more like a crosshair instead of a simple square box. 

## License

*Game Assets*: Licensed for use with the Unreal Engine only. Without a custom license you cannot use to create sequels, remasters, or otherwise emulate the original game or use the original game’s trademarks, character names, or other IP to advertise or name your game. (Unreal Engine EULA applies) (Please note this applies to the Game Assets that refer to Epic's Paragon, you can still use the project code and content to build your own Unreal Engine game)
