# Minesweeper with Hatsune Miku

A fully functional Minesweeper game built in C++ using raylib, featuring:

- 15×15 grid (225 cells)
- 40 mines
- First-click guaranteed safe
- Flood-fill reveal (BFS using your own Queue)
- Flag system with scoring (+3 for correct flag, –2 for wrong)
- Real-time timer & score
- Beautiful beveled Windows-95-style cells
- Custom scoring system
- Win & Game Over states

But most importantly: **Hatsune Miku is watching you.**

## Features

- Happy bouncing Miku when you're playing
- Sad angry Miku when you lose
- When you hit a mine:
  - → Miku violently shakes for 4 seconds
  - → Normal music stops
  - → 25-second angry/sad Miku plays
  - → Then the Game Over screen appears
- Press R to restart (Miku forgives you… maybe)
