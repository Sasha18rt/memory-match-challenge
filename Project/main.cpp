#include <tchar.h>
#include <windows.h>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <random>
#include "resource.h"

#define RECORDS_FILE "scores.txt"
#define MAX_TOP_SCORES 5
#include <windows.h>
#include <string>

typedef void (*LoadHighScoresFunc)(char*, size_t);
typedef void (*SaveHighScoreFunc)(int);
typedef void (*InitializeGameFunc)(int*, int);

SaveHighScoreFunc SaveHighScore = nullptr;
HINSTANCE hGameLogicDLL = nullptr;
LoadHighScoresFunc LoadHighScores = nullptr;
InitializeGameFunc InitializeGameDLL = nullptr;


bool LoadGameLogicDLL() {
    hGameLogicDLL = LoadLibrary(_T("GameLogic.dll"));
    if (!hGameLogicDLL) {
        MessageBox(NULL, _T("Failed to load GameLogic.dll!"), _T("Error"), MB_OK | MB_ICONERROR);
        return false;
    }

    LoadHighScores = (LoadHighScoresFunc)GetProcAddress(hGameLogicDLL, "LoadHighScores");
    SaveHighScore = (SaveHighScoreFunc)GetProcAddress(hGameLogicDLL, "SaveHighScore");
    InitializeGameDLL = (InitializeGameFunc)GetProcAddress(hGameLogicDLL, "InitializeGameDLL");
if (!InitializeGameDLL) {
    MessageBox(NULL, _T("Failed to load InitializeGameDLL function from GameLogic.dll!"), _T("Error"), MB_OK | MB_ICONERROR);
}

    if (!LoadHighScores || !SaveHighScore) {
        MessageBox(NULL, _T("Failed to load functions from GameLogic.dll!"), _T("Error"), MB_OK | MB_ICONERROR);
        FreeLibrary(hGameLogicDLL);
        return false;
    }

    return true;
}


void UnloadGameLogicDLL() {
    if (hGameLogicDLL) {
        FreeLibrary(hGameLogicDLL);
        hGameLogicDLL = nullptr;
    }
}

struct HighScore {
    int score;
    char timestamp[100]; // Time as a string
};

// Constants
int GRID_SIZE = 4;
const int BUTTON_SIZE = 100; // Button size
const int SCORE_PANEL_WIDTH = 200; // Width of the score display panel

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

TCHAR szClassName[] = _T("MemoryMatchChallenge");

// Dynamic game data
std::vector<HWND> gridButtons; // Buttons for the grid
std::vector<int> cardValues;   // Card values
std::vector<bool> cardRevealed; // Card status: true if revealed
std::vector<int> openedCards;  // Indexes of opened cards

std::vector<HBITMAP> cardImages; // Card images
int score = 0;                   // Score
HINSTANCE hInst;
HWND scoreLabel;                 // Element to display the score

// Load card images
void LoadCardImages() {
    cardImages.clear();

    // Load all 12 images from resources
    for (int i = IDB_CARD_1; i <= IDB_CARD_12; ++i) {
        HBITMAP hBitmap = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(i));
        if (!hBitmap) {
            std::wstring errorMessage = L"Failed to load resource image with ID: " + std::to_wstring(i);
            MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
        }
        cardImages.push_back(hBitmap);
    }
}

// Update score in the window
void UpdateScore() {
    std::wstring scoreText = L"Score: " + std::to_wstring(score);
    SetWindowTextW(scoreLabel, scoreText.c_str());
}

// Initialize the game
void InitializeGame() {
    int totalCards = GRID_SIZE * GRID_SIZE;
    cardValues.resize(totalCards);
    cardRevealed.resize(totalCards, false);

    for (size_t i = 0; i < gridButtons.size(); ++i) {
        SendMessage(gridButtons[i], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
        cardRevealed[i] = false;
    }

    if (InitializeGameDLL) {
        InitializeGameDLL(cardValues.data(), totalCards);
    }

    openedCards.clear();
}


// Main entry point
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    hInst = hThisInstance;
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;
if (!LoadGameLogicDLL()) {
    return -1;
}
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    if (!RegisterClassEx(&wincl))
        return 0;

    hwnd = CreateWindowEx(
        0,
        szClassName,
        _T("Memory Match Challenge"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        BUTTON_SIZE * GRID_SIZE + 30,
        BUTTON_SIZE * GRID_SIZE + 115,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    LoadCardImages(); // Load images
    InitializeGame(); // Initialize game

    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}




// Resize window and adjust score label position
void ResizeWindowAndScoreLabel(HWND hwnd) {
    int newWidth = BUTTON_SIZE * GRID_SIZE + 30;   // Width of the window
    int newHeight = BUTTON_SIZE * GRID_SIZE + 115; // Height of the window
    SetWindowPos(hwnd, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    int scoreLabelX = BUTTON_SIZE * GRID_SIZE - 145; // Dynamic adjustment
    MoveWindow(scoreLabel, scoreLabelX, 10, 150, 30, TRUE);
}

// Update the grid
void UpdateGrid(HWND hwnd) {
    for (HWND btn : gridButtons) {
        DestroyWindow(btn);
    }
    gridButtons.clear();

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            HWND btn = CreateWindow(
                _T("BUTTON"),
                _T(""),
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
                10 + col * BUTTON_SIZE,  // Horizontal offset
                50 + row * BUTTON_SIZE, // Vertical offset (below the score panel)
                BUTTON_SIZE - 2,
                BUTTON_SIZE - 2,
                hwnd,
                (HMENU)(INT_PTR)(row * GRID_SIZE + col),
                hInst,
                NULL
            );

            if (!btn) {
                MessageBox(hwnd, _T("Failed to create button!"), _T("Error"), MB_OK | MB_ICONERROR);
                return;
            }

            gridButtons.push_back(btn);
        }
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    HWND nextLevelButton;
    case WM_CREATE: {
        // Menu
        HMENU hMenu = CreateMenu();
        AppendMenu(hMenu, MF_STRING, ID_RESHUFFLE, _T("Reshuffle"));
        AppendMenu(hMenu, MF_STRING, ID_SCORES, _T("Top scores"));
        AppendMenu(hMenu, MF_STRING, ID_EXIT, _T("Exit"));

        SetMenu(hwnd, hMenu);

        // Score panel
        nextLevelButton = CreateWindow(
            _T("BUTTON"), _T("Next Level"),
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 10, 100, 30,
            hwnd, (HMENU)ID_NEXT_LEVEL, hInst, NULL
        );

        int scoreLabelX = BUTTON_SIZE * GRID_SIZE - 145; // Dynamic shift to the right
        scoreLabel = CreateWindow(
            _T("STATIC"),
            _T("Scores: 0"), // Default text for scores
            WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE | SS_CENTER, // Center alignment
            scoreLabelX, 10,  // Position (X: scoreLabelX, Y: 10)
            150, 30, // Size (Width: 150, Height: 30)
            hwnd,
            NULL, // No specific ID for static text
            hInst,
            NULL
        );

        // Create buttons
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                HWND btn = CreateWindow(
                    _T("BUTTON"),
                    _T(""),
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
                    10 + col * BUTTON_SIZE,  // Horizontal offset
                    50 + row * BUTTON_SIZE, // Vertical offset (below score panel)
                    BUTTON_SIZE - 2,
                    BUTTON_SIZE - 2,
                    hwnd,
                    (HMENU)(INT_PTR)(row * GRID_SIZE + col),
                    hInst,
                    NULL
                );

                if (!btn) {
                    MessageBox(hwnd, _T("Failed to create button!"), _T("Error"), MB_OK | MB_ICONERROR);
                    return 0;
                }

                gridButtons.push_back(btn);
            }
        }
    }
    break;

    case WM_COMMAND: {
        // Handle commands (Reshuffle, Next Level, Exit, buttons)
        switch (LOWORD(wParam)) {
        case ID_NEXT_LEVEL: {
            bool allRevealed = std::all_of(cardRevealed.begin(), cardRevealed.end(), [](bool revealed) { return revealed; });
            if (allRevealed) {
                if (score > 70) {
                    GRID_SIZE = 5;  // Increase grid size
                    ResizeWindowAndScoreLabel(hwnd); // Update window size and score label position
                    UpdateGrid(hwnd); // Update the grid
                }
                InitializeGame(); // Initialize the next level
                MessageBox(hwnd, _T("Welcome to the next level!"), _T("Next Level"), MB_OK);
            } else {
                MessageBox(hwnd, _T("Not all cards are revealed. Please finish the current level."), _T("Next Level"), MB_OK);
            }
            break;
        }

        case ID_RESHUFFLE: {
            GRID_SIZE = 4; // Reset the grid size to the default
            score = 0; // Reset the score
            UpdateScore(); // Update the score text on the panel

            // Reset window and score panel size
            ResizeWindowAndScoreLabel(hwnd);

            // Update the grid and initialize the game
            UpdateGrid(hwnd);
            InitializeGame();

            MessageBox(hwnd, _T("The game has been reset!"), _T("Reshuffle"), MB_OK);
            break;
        }

        case ID_EXIT:
            PostQuitMessage(0);
            SaveHighScore(score);
            break;

   case ID_SCORES: {
    if (!LoadHighScores) {
        MessageBox(hwnd, _T("LoadHighScores function is not initialized."), _T("Error"), MB_OK | MB_ICONERROR);
        break;
    }

    // Створіть буфер для отримання результату
    char buffer[1024] = { 0 };

    try {
        LoadHighScores(buffer, sizeof(buffer)); //DLL
        MessageBoxA(hwnd, buffer, "High Scores", MB_OK);
    } catch (...) {
        MessageBox(hwnd, _T("An error occurred while loading high scores."), _T("Error"), MB_OK | MB_ICONERROR);
    }
    break;
}


        default: {
            int btnID = LOWORD(wParam);
            if ((size_t)btnID < gridButtons.size() && openedCards.size() < 2) {
                if (cardRevealed[btnID]) {
                    break;
                }

                // Display the card image
                SendMessage(gridButtons[btnID], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)cardImages[cardValues[btnID]]);
                UpdateWindow(gridButtons[btnID]);
                cardRevealed[btnID] = true;

                openedCards.push_back(btnID);

                if (openedCards.size() == 2) {
                    int firstCard = openedCards[0];
                    int secondCard = openedCards[1];

                    if (cardValues[firstCard] == cardValues[secondCard]) {
                        score += 10;
                        UpdateScore();
                    } else {
                        Sleep(500);
                        SendMessage(gridButtons[firstCard], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
                        SendMessage(gridButtons[secondCard], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);

                        cardRevealed[firstCard] = false;
                        cardRevealed[secondCard] = false;
                    }

                    openedCards.clear();
                }
            }
            break;
        }
        }
        break;
    }

    case WM_DESTROY:

        for (HBITMAP bmp : cardImages) {
            if (bmp) {
                DeleteObject(bmp);
            }
        }
        SaveHighScore(score);
  UnloadGameLogicDLL();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
