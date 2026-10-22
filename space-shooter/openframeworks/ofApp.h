#pragma once
#include "ofMain.h"
#include "Entities.h"

// 게임 상태
enum class GameState { Title, Playing, GameOver };

class ofApp : public ofBaseApp {
public:
    // --- openFrameworks 기본 함수 -----------------------------------
    void setup() override;
    void update() override;     // 매 프레임: 이동 · 충돌 · 제거
    void draw() override;       // 매 프레임: 그리기
    void keyPressed(int key) override;
    void keyReleased(int key) override;

    // --- 게임 로직 ---------------------------------------------------
    void resetGame();
    void setKey(int key, bool down);
    void updateStars();

    void updateSpawning();
    int  currentSpawnInterval();
    void spawnEnemy();

    void checkBulletEnemyCollisions();
    void checkPlayerEnemyCollisions();

    void onEnemyKilled(Enemy& enemy);
    void onPlayerHit();

    void drawHud();
    void drawCenterText(const std::string& title, const std::string& subtitle);

    // --- 게임 데이터 -------------------------------------------------
    struct Star { float x, y, speed; };

    GameState state = GameState::Playing;
    Input input;
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Star> stars;
    int score = 0;
    int spawnTimer = 60;
};
