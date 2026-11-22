# SoulBound

**A 2D puzzle platformer built on our own custom C++ game engine.**

Gameplay video: https://www.youtube.com/watch?v=psjMKbx5KxQ  
Project page: https://bryanlim-dev.github.io/projects/soulbound.html

SoulBound was made by team **Midnight Munchies** as a school project at SIT–DigiPen. Players control Raincharm, a spirit who can possess enemies and use their abilities to get past obstacles and solve puzzles. Before building the game, our team built the whole engine from scratch in C++, with no built-in editor.

## My role: Audio Lead / Programmer

- **Editor tools (Dear ImGui):** built the asset browser with drag-and-drop import from Windows Explorer, the asset and prefab tools for editing properties and JSON data, and a live inspector for 4 component types (Transform, Rigidbody, Collider, Animator). 6 teammates used these tools, so our level designers could edit and save levels without changing JSON files by hand.
- **Collision detection:** stops entities from clipping through the level.
- **Audio:** proximity audio that gets louder as the player nears hazards, soundsets for all 4 playable characters, and the volume and display settings menu.

## Playable characters

| Character | Ability |
|---|---|
| Raincharm (spirit) | Double-tap Space to glide |
| Demonic Frog | Higher jump and a grapple hook |
| Flyeye | Passive double jump |
| Snek | Aim and launch like a slingshot |

## Controls

| Action | Key |
|---|---|
| Move | A / D |
| Up / Down | W / S |
| Jump / Ability | Space |
| Possess / Unpossess | J |

## Tech

C++, Dear ImGui, custom game engine

---

This repository is a copy of our team's school repository, shared with permission for portfolio use. All team members contributed to the project.
