### **Memory Match Challenge - Windows Game**

Memory Match Challenge is a simple Windows-based matching game where players flip cards to find matching pairs. The game dynamically increases in difficulty and keeps track of high scores for competitive play.

---

### **Features**
- **Dynamic Difficulty:** Progress to harder levels as you score higher.
- **High Score Tracking:** Keeps the top 5 high scores in a local file.
- **Card Matching Logic:** Flip cards and match pairs to earn points.
- **Interactive UI:** Visual grid with clickable buttons representing cards.
- **Reset & Restart Options:** Easily restart or reshuffle the game grid.
- **High Score Management:** View and reset high scores.

---

### **Requirements**
- **Operating System:** Windows (32-bit or 64-bit)
- **Compiler:** GCC or MSVC
- **Dependencies:**
  - Windows API
  - `resource.h` (for card images and menu definitions)
  - A dynamically linked library (DLL) for high score management

---

### **Setup Instructions**

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-repo/memory-match-challenge.git
   ```
   Navigate to the project directory:
   ```bash
   cd memory-match-challenge
   ```

2. **Ensure Required Files:**
   - Ensure the following files are present in the project directory:
     - `GameLogic.dll` (for high score management)
     - `resource.h` (contains card image definitions and IDs)

3. **Compile the Project:**
   - If using GCC (MinGW):
     ```bash
     g++ main.cpp -o MemoryMatch.exe -lgdi32 -lcomctl32
     ```
   - For MSVC, use an appropriate IDE like Visual Studio.

4. **Run the Game:**
   - Execute the compiled `MemoryMatch.exe` file:
     ```bash
     ./MemoryMatch.exe
     ```

---

### **How to Play**

1. **Start the Game:**
   - Upon launching, the game grid will be displayed with hidden cards.

2. **Flip Cards:**
   - Click on a card to reveal its value. Try to find its matching pair.

3. **Score Points:**
   - Matching pairs earn you points. Mismatched pairs will be hidden again after a brief delay.

4. **Advance Levels:**
   - If your score exceeds 70 and all cards are revealed, the grid size increases for the next level.

5. **View High Scores:**
   - Use the "Top Scores" option in the menu to see the leaderboard.

6. **Reset Game:**
   - Click "Reshuffle" in the menu to reset the game grid and start fresh.

7. **Exit Game:**
   - Use the "Exit" option in the menu to quit the game. Your score will be saved.

---

### **DLL Functions**
This project uses the `GameLogic.dll` to manage high scores. The following functions are implemented in the DLL:
- `LoadHighScores`: Loads and displays the top scores.
- `SaveHighScore`: Saves a new high score if it qualifies.
- `ClearHighScores`: Resets the high score table.

---

### **High Score Management**
High scores are stored in a local file (`scores.txt`) in the same directory as the executable. The file includes the top 5 scores with timestamps.

- **File Format:**
  ```
  [Score] [Timestamp]
  100 2024-01-01 12:00:00
  ```

---

### **Customization**
- **Resource Files:**
  - Modify `resource.h` to change card images or menu options.
- **Grid Size:**
  - Adjust the initial grid size by modifying `GRID_SIZE` in the source code.
- **High Score Limit:**
  - Change `MAX_TOP_SCORES` in the DLL code to increase or decrease the number of saved high scores.

---

### **Known Issues**
- Ensure the `GameLogic.dll` is in the same directory as the executable.
- Missing card images may result in errors. Make sure all image resources are correctly linked.

---

### **Contributing**
1. Fork the repository.
2. Create a feature branch:
   ```bash
   git checkout -b feature-name
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add feature"
   ```
4. Push the branch:
   ```bash
   git push origin feature-name
   ```
5. Submit a pull request.

---

### **License**
This project is licensed under the [MIT License](LICENSE).
