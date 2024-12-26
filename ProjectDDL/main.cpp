#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>

#define DLL_EXPORT __declspec(dllexport)

const int MAX_TOP_SCORES = 5;
const char* RECORDS_FILE = "scores.txt";

struct HighScore {
    int score;
    char timestamp[100];
};

// Function to load high scores
extern "C" DLL_EXPORT void LoadHighScores(char* buffer, size_t bufferSize) {
    HighScore scores[MAX_TOP_SCORES] = {};
    int count = 0;

    // Load scores from the file
    std::ifstream file(RECORDS_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line) && count < MAX_TOP_SCORES) {
            std::istringstream iss(line);
            iss >> scores[count].score;
            iss.ignore();
            iss.getline(scores[count].timestamp, sizeof(scores[count].timestamp));
            count++;
        }
        file.close();
    } else {
        strncpy(buffer, "Error: scores.txt not found.", bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return;
    }

    // Format results into the buffer
    std::string result = "Top Scores:\n";
    for (int i = 0; i < count; i++) {
        result += std::to_string(scores[i].score) + " - " + scores[i].timestamp + "\n";
    }

    strncpy(buffer, result.c_str(), bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
}

// Function to save a new high score
extern "C" DLL_EXPORT void SaveHighScore(int newScore) {
    HighScore scores[MAX_TOP_SCORES + 1] = {};
    int count = 0;

    // Load existing scores from the file
    std::ifstream fileIn(RECORDS_FILE);
    if (fileIn.is_open()) {
        std::string line;
        while (std::getline(fileIn, line) && count < MAX_TOP_SCORES) {
            std::istringstream iss(line);
            iss >> scores[count].score;
            iss.ignore();
            iss.getline(scores[count].timestamp, sizeof(scores[count].timestamp));
            count++;
        }
        fileIn.close();
    }

    // Add the new score
    if (count < MAX_TOP_SCORES || newScore > scores[count - 1].score) {
        time_t now = time(nullptr);
        struct tm* local = localtime(&now);
        strftime(scores[count].timestamp, sizeof(scores[count].timestamp), "%Y-%m-%d %H:%M:%S", local);
        scores[count].score = newScore;
        count++;
    }

    // Sort scores in descending order
    std::sort(scores, scores + count, [](HighScore a, HighScore b) {
        return a.score > b.score;
    });

    // Save the updated list to the file
    std::ofstream fileOut(RECORDS_FILE);
    for (int i = 0; i < std::min(count, MAX_TOP_SCORES); i++) {
        fileOut << scores[i].score << " " << scores[i].timestamp << "\n";
    }
    fileOut.close();
}

// Function to initialize the game
extern "C" DLL_EXPORT void InitializeGameDLL(int* cardValues, int totalCards) {
    for (int i = 0; i < totalCards / 2; ++i) {
        cardValues[i] = i;
        cardValues[i + totalCards / 2] = i;
    }

    // Shuffle card values
    std::random_device rd;
    std::mt19937 g(static_cast<unsigned int>(std::time(0)));
    std::shuffle(cardValues, cardValues + totalCards, g);
}
