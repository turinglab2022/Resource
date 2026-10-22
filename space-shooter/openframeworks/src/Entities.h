#pragma once
#include "ofMain.h"
#include <algorithm>
#include <string>
#include <vector>

// ----------------------------------------------------------------------
// 설정값 (숫자를 바꿔 보며 게임 느낌을 조절해 보자!)
// ----------------------------------------------------------------------
const int   WIN_W = 600;
const int   WIN_H = 800;

const float PLAYER_SPEED = 5.0f;     // 플레이어 이동 속도 (픽셀/프레임)
const float PLAYER_RADIUS = 16.0f;   // 충돌 판정용 반지름
const int   SHOOT_COOLDOWN = 12;     // 발사 간격 (프레임). 60프레임 = 1초
const float BULLET_SPEED = 11.0f;
const float BULLET_RADIUS = 4.0f;
const float ENEMY_RADIUS = 18.0f;
const int   ENEMY_HP = 1;
const int   SPAWN_INTERVAL = 50;     // 적이 나타나는 간격 (프레임)

// 지금 눌려 있는 키 상태 (ofApp 의 keyPressed / keyReleased 에서 갱신한다)
struct Input {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool fire = false;
};

// ----------------------------------------------------------------------
// 충돌 판정
// ----------------------------------------------------------------------
// 두 원이 겹쳐 있으면 true
inline bool circlesCollide(glm::vec2 posA, float radiusA, glm::vec2 posB, float radiusB) {
    // ★ TODO 6: 두 원이 겹쳤는지 판정하자
    //   힌트 1: 두 중심 사이의 거리: glm::distance(posA, posB)
    //   힌트 2: 거리가 (반지름의 합)보다 작으면 겹친 것이다
    return false;
    // ★ TODO 6 끝
}

// ----------------------------------------------------------------------
// 총알
// ----------------------------------------------------------------------
class Bullet {
public:
    glm::vec2 pos;
    glm::vec2 vel;
    float radius;
    bool dead;          // true 가 되면 vector 에서 제거된다

    Bullet(glm::vec2 p, glm::vec2 v) : pos(p), vel(v), radius(BULLET_RADIUS), dead(false) {}

    void update() {
        pos += vel;
        // ★ TODO 4: 화면 밖으로 나간 총알은 dead 로 표시하자
        //   힌트 1: 위쪽: pos.y < -radius,  아래쪽: pos.y > WIN_H + radius
        //   힌트 2: 왼쪽·오른쪽(3방향 발사 때 필요)도 같은 방식으로 검사한다 (|| 로 연결)
        // ★ TODO 4 끝
    }

    void draw() const {
        ofSetColor(255, 240, 120);
        ofDrawCircle(pos.x, pos.y, radius);
    }
};

// ----------------------------------------------------------------------
// 적
// ----------------------------------------------------------------------
class Enemy {
public:
    glm::vec2 pos;
    glm::vec2 vel;
    std::string kind;   // "straight" | "zigzag" | "boss"
    float radius;
    int hp;
    int maxHp;
    bool fragile;       // 플레이어와 부딪히면 사라지는가?
    float baseX;        // 지그재그의 기준선
    int age;            // 태어난 뒤 지난 프레임 수
    bool dead;

    Enemy(glm::vec2 p, glm::vec2 v, const std::string& k = "straight")
        : pos(p), vel(v), kind(k), radius(ENEMY_RADIUS), hp(ENEMY_HP), maxHp(ENEMY_HP),
          fragile(true), baseX(p.x), age(0), dead(false) {
    }

    void update() {
        age++;
        pos += vel;
        if (pos.y > WIN_H + radius) {
            dead = true;                        // 화면 아래로 빠져나감
        }
    }

    void draw() const {
        float x = pos.x, y = pos.y, r = radius;
        ofSetColor(255, 80, 90);
        ofDrawTriangle(x - r, y - r, x + r, y - r, x, y + r);
    }
};

// ----------------------------------------------------------------------
// 플레이어
// ----------------------------------------------------------------------
class Player {
public:
    glm::vec2 pos;
    float radius;
    int cooldown;       // 0 이 되면 다시 쏠 수 있다
    int hp;             // 라이프: 적과 부딪히면 1 줄고, 0 이 되면 게임오버

    Player() : pos(WIN_W / 2.0f, WIN_H - 100.0f), radius(PLAYER_RADIUS), cooldown(0), hp(3)
    {}

    // in 은 지금 눌려 있는 키 상태 (in.left, in.right, in.up, in.down, in.fire)
    void update(const Input& in) {
        glm::vec2 move(0, 0);
        // ★ TODO 1: 키 입력으로 이동 방향 벡터를 만들자
        //   힌트 1: in.left 이면 move.x 를 1 줄이고, in.right 이면 1 늘린다
        //   힌트 2: 위·아래도 move.y 로 같은 방식. 아래쪽이 + 방향이다
        //   힌트 3: 대각선은 길이가 1.41 이라 더 빠르다! glm::length(move) > 0 이면 glm::normalize(move) 로 길이를 1 로 맞춘다
        //   힌트 4: 마지막에 pos += move * PLAYER_SPEED;
        // ★ TODO 1 끝

        // ★ TODO 2: 화면 밖으로 나가지 못하게 하자
        //   힌트 1: 값을 범위 안으로 가두기: ofClamp(값, 최솟값, 최댓값)
        //   힌트 2: x 는 radius ~ WIN_W - radius,  y 는 radius ~ WIN_H - radius
        // ★ TODO 2 끝

        if (cooldown > 0) cooldown--;
    }

    // 쏠 수 있으면 새 총알들을, 아니면 빈 vector 를 돌려준다
    std::vector<Bullet> tryShoot() {
        std::vector<Bullet> bullets;
        glm::vec2 muzzle(pos.x, pos.y - radius);   // 총구 위치(기체 앞쪽)
        // ★ TODO 3: 쿨다운이 끝났을 때만 총알을 만들자
        //   힌트 1: cooldown <= 0 일 때만 발사할 수 있다
        //   힌트 2: 총알: bullets.push_back(Bullet(muzzle, glm::vec2(0, -BULLET_SPEED)));  (위쪽이 - 방향)
        //   힌트 3: 쏘고 나면 cooldown = SHOOT_COOLDOWN; 으로 다시 채운다
        // ★ TODO 3 끝
        return bullets;
    }

    void draw() const {
        float x = pos.x, y = pos.y;
        float flame = ofRandom(6, 14);              // 엔진 불꽃은 매 프레임 길이가 바뀐다
        ofSetColor(255, 160, 40);
        ofDrawTriangle(x - 6, y + 10, x + 6, y + 10, x, y + 10 + flame);
        ofSetColor(80, 200, 255);
        ofBeginShape();
        ofVertex(x, y - 20);
        ofVertex(x - 16, y + 16);
        ofVertex(x, y + 8);
        ofVertex(x + 16, y + 16);
        ofEndShape(true);
        ofSetColor(255);
        ofDrawCircle(x, y - 2, 4);
    }
};
