/*
 * 우주 슈팅 게임 (openFrameworks)
 *
 * 조작법
 *   방향키 / WASD : 이동
 *   SPACE         : 발사 (누르고 있으면 연사)
 *   R / ENTER     : 시작 · 다시 시작
 *   ESC           : 종료
 *
 * 프로그램 구조 (불꽃놀이와 같은 패턴!)
 *   keyPressed / keyReleased -> 키 상태 기록
 *   update()                 -> 모든 객체 이동 · 충돌 · 제거   (매 프레임)
 *   draw()                   -> 모든 객체 그리기               (매 프레임)
 */
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Space Shooter");

    for (int i = 0; i < 70; i++) {
        Star s;
        s.x = ofRandom(0, WIN_W);
        s.y = ofRandom(0, WIN_H);
        s.speed = ofRandom(0.5f, 3.0f);
        stars.push_back(s);
    }
    resetGame();
}

// 게임을 처음 상태로 되돌린다
void ofApp::resetGame() {
    player = Player();
    bullets.clear();
    enemies.clear();
    score = 0;
    spawnTimer = 60;                // 시작 후 1초 뒤에 첫 적
    state = GameState::Playing;
}

//--------------------------------------------------------------
void ofApp::update() {
    updateStars();
    if (state != GameState::Playing) return;

    player.update(input);
    if (input.fire) {
        std::vector<Bullet> shots = player.tryShoot();
        bullets.insert(bullets.end(), shots.begin(), shots.end());
    }

    updateSpawning();
    for (auto& b : bullets) b.update();
    for (auto& e : enemies) e.update();

    checkBulletEnemyCollisions();
    checkPlayerEnemyCollisions();

    // 죽은 객체 제거 (불꽃놀이의 TODO 4 와 같은 방법: erase-remove)
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet& b) { return b.dead; }),
                  bullets.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                 [](const Enemy& e) { return e.dead; }),
                  enemies.end());
}

void ofApp::updateStars() {
    for (auto& s : stars) {
        s.y += s.speed;
        if (s.y > WIN_H) {
            s.x = ofRandom(0, WIN_W);
            s.y = 0;
        }
    }
}

//--------------------------------------------------------------
// 적 생성
//--------------------------------------------------------------
void ofApp::updateSpawning() {
    spawnTimer--;
    if (spawnTimer <= 0) {
        spawnEnemy();
        spawnTimer = currentSpawnInterval();
    }
}

// 다음 적이 나올 때까지 기다릴 프레임 수
int ofApp::currentSpawnInterval() {
    return SPAWN_INTERVAL;
}

void ofApp::spawnEnemy() {
    std::string kind = "straight";
    // ★ TODO 5: 화면 위쪽 바깥에서 적을 한 마리 만들자
    //   힌트 1: x 는 화면 폭 안에서 랜덤: ofRandom(ENEMY_RADIUS, WIN_W - ENEMY_RADIUS)
    //   힌트 2: 시작 y 는 화면 위쪽 바깥(-ENEMY_RADIUS), 속도는 아래로 ofRandom(1.5f, 3.5f)
    //   힌트 3: enemies.push_back(Enemy(위치, 속도, kind));
    // ★ TODO 5 끝
}

//--------------------------------------------------------------
// 충돌 처리
//--------------------------------------------------------------
void ofApp::checkBulletEnemyCollisions() {
    // ★ TODO 7: 모든 (총알, 적) 쌍을 검사해서 맞았는지 확인하자
    //   힌트 1: 이중 for 문: for (auto& bullet : bullets) { for (auto& enemy : enemies) { ... } }
    //   힌트 2: 이미 dead 인 것은 건너뛴다 (한 발이 두 마리를 맞히면 안 된다)
    //   힌트 3: 맞으면: bullet.dead = true;  enemy.hp -= 1;  hp 가 0 이하면 enemy.dead = true; 와 onEnemyKilled(enemy);
    // ★ TODO 7 끝
}

void ofApp::checkPlayerEnemyCollisions() {
    // ★ TODO 8: 플레이어가 적과 부딪혔는지 확인하자
    //   힌트 1: 모든 적에 대해 circlesCollide(player.pos, player.radius, enemy.pos, enemy.radius)
    //   힌트 2: 부딪혔으면 enemy.fragile 인 적은 enemy.dead = true; 로 만들고, onPlayerHit(); 를 부른다
    // ★ TODO 8 끝
}

//--------------------------------------------------------------
// 사건 처리
//--------------------------------------------------------------
void ofApp::onEnemyKilled(Enemy& enemy) {
    score += 10;
}

void ofApp::onPlayerHit() {
    state = GameState::GameOver;
}

//--------------------------------------------------------------
// 그리기
//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(8, 10, 28);
    for (const auto& s : stars) {
        int shade = (int)(60 + s.speed * 50);
        ofSetColor(shade);
        ofDrawCircle(s.x, s.y, s.speed < 2 ? 1 : 2);
    }

    for (const auto& b : bullets) b.draw();
    for (const auto& e : enemies) e.draw();
    if (state != GameState::Title) {
        player.draw();
    }
    drawHud();

    if (state == GameState::GameOver) {
        drawCenterText("GAME OVER", "Score " + ofToString(score) + "    press R");
    }
}

void ofApp::drawHud() {
    ofSetColor(255);
    ofDrawBitmapString("SCORE " + ofToString(score), 12, 22);
}

void ofApp::drawCenterText(const std::string& title, const std::string& subtitle) {
    // 비트맵 글꼴은 글자 하나가 가로 8픽셀. ofScale 로 키워서 큰 글씨를 만든다
    float scale = 4;
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(WIN_W / 2 - (int)title.size() * 8 * scale / 2, WIN_H / 2 - 30);
    ofScale(scale, scale);
    ofDrawBitmapString(title, 0, 0);
    ofPopMatrix();
    ofSetColor(200, 200, 220);
    ofDrawBitmapString(subtitle, WIN_W / 2 - (int)subtitle.size() * 4, WIN_H / 2 + 30);
}

//--------------------------------------------------------------
// 입력
//--------------------------------------------------------------
// 키가 눌렸는지(true) 떼어졌는지(false) 를 input 에 기록한다
void ofApp::setKey(int key, bool down) {
    if (key == OF_KEY_LEFT  || key == 'a' || key == 'A') input.left = down;
    if (key == OF_KEY_RIGHT || key == 'd' || key == 'D') input.right = down;
    if (key == OF_KEY_UP    || key == 'w' || key == 'W') input.up = down;
    if (key == OF_KEY_DOWN  || key == 's' || key == 'S') input.down = down;
    if (key == ' ') input.fire = down;
}

void ofApp::keyPressed(int key) {
    setKey(key, true);
    if (key == 'r' || key == 'R' || key == OF_KEY_RETURN) {
        if (state == GameState::GameOver) resetGame();
    }
}

void ofApp::keyReleased(int key) {
    setKey(key, false);
}
