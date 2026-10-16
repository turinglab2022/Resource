#pragma once
#include "ofMain.h"

const float GRAVITY = 0.08f;   // 매 프레임 속도에 더해지는 중력
const float DRAG    = 0.985f;  // 공기 저항

// ----------------------------------------------------------------------
// 파티클: 불꽃의 "불티" 하나
// ----------------------------------------------------------------------
class Particle {
public:
    glm::vec2 pos;
    glm::vec2 vel;
    ofColor color;
    int life;
    int maxLife;
    float size;

    Particle(glm::vec2 p, glm::vec2 v, ofColor c, int lifeFrames, float s = 3.0f)
        : pos(p), vel(v), color(c), life(lifeFrames), maxLife(lifeFrames), size(s) {}

    void update() {
        // TODO 1: 파티클을 움직이자
        //   (1) vel.y 에 GRAVITY 를 더한다     -> 아래로 떨어진다
        //   (2) vel 에 DRAG 를 곱한다          -> 점점 느려진다
        //   (3) pos 에 vel 을 더한다           -> 실제로 이동한다
        //   (4) life 를 1 줄인다
    }

    bool isDead() const { return life <= 0; }

    void draw() const {
        // TODO 2: 수명이 줄수록 작아지고 투명해지게 그리자
        //   힌트: float ratio = (float)life / (float)maxLife;   // 1.0 -> 0.0
        //         float radius = std::max(1.0f, size * ratio);
        //         ofSetColor(color, (int)(255 * ratio));
        //         ofDrawCircle(pos.x, pos.y, radius);
    }
};

// ----------------------------------------------------------------------
// 로켓: 목표 지점까지 올라가서 폭발한다  ([2교시] 에서 사용)
// ----------------------------------------------------------------------
class Rocket {
public:
    glm::vec2 pos;
    glm::vec2 target;
    ofColor color;
    bool arrived = false;

    Rocket(float x, glm::vec2 t, ofColor c)
        : pos(x, ofGetHeight()), target(t), color(c) {}

    void update() {
        const float SPEED = 9.0f;
        // TODO 5: 목표(target)를 향해 SPEED 만큼 이동하자
        //   힌트: glm::vec2 toTarget = target - pos;
        //         glm::length(toTarget) <= SPEED 이면 arrived = true;
        //         아니면 pos += glm::normalize(toTarget) * SPEED;
    }

    Particle makeTrailSpark() const {
        glm::vec2 v(ofRandom(-0.5f, 0.5f), ofRandom(0.5f, 1.5f));
        return Particle(pos, v, ofColor(255, 200, 120), 18, 2.0f);
    }

    void draw() const {
        ofSetColor(255);
        ofDrawCircle(pos.x, pos.y, 3);
    }
};
