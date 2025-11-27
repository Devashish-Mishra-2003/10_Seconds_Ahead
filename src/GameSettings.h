#pragma once
#include <string>

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

struct GameSettings {
    Difficulty difficulty = Difficulty::Easy;

    int blocksPerTurn() const {
        switch (difficulty) {
            case Difficulty::Easy:   return 3;
            case Difficulty::Normal: return 2;
            case Difficulty::Hard:   return 1;
        }
        return 3;
    }

    int turnLimit() const {
        switch (difficulty) {
            case Difficulty::Easy:   return -1; // infinite
            case Difficulty::Normal: return 5;
            case Difficulty::Hard:   return 3;
        }
        return -1;
    }

    std::string difficultyName() const {
        switch (difficulty) {
            case Difficulty::Easy: return "Easy";
            case Difficulty::Normal: return "Normal";
            case Difficulty::Hard: return "Hard";
        }
        return "Easy";
    }
};
