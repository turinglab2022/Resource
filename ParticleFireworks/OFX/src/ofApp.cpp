/*
 * 파티클 불꽃놀이 - openFrameworks 스타터 (학생용)
 *
 * [1교시] TODO 1 ~ 4 를 채워서 "클릭하면 폭발하는" 불꽃을 만든다.
 *         (TODO 1, 2 는 Particle.h 에 있다)
 * [2교시] TODO 5 ~ 8 을 채워서 로켓, 여러 모양, 나만의 불꽃으로 확장한다.
 *
 * 조작법: 마우스 클릭 = 발사,  1 / 2 / 3 = 폭발 모양,  C = 화면 지우기
 */
#include "ofApp.h"

const int TRAIL_FADE = 25;

// [2교시] TODO 5 를 끝내면 true 로 바꾸자!
const bool USE_ROCKET = false;

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Particle Fireworks");

    canvas.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    canvas.begin();
    ofClear(0, 0, 0);
    canvas.end();
}

//--------------------------------------------------------------
void ofApp::update() {
    for (auto& r : rockets) {
        r.update();
        particles.push_back(r.makeTrailSpark());
        if (r.arrived) {
            explode(r.pos, r.color);
        }
    }
    rockets.erase(std::remove_if(rockets.begin(), rockets.end(),
                                 [](const Rocket& r) { return r.arrived; }),
                  rockets.end());

    for (auto& p : particles) {
        p.update();
    }

    // TODO 4: 수명이 다한 파티클을 vector 에서 제거하자 (안 하면 점점 느려져요!)
    //   힌트(쉬운 방법): 뒤에서부터 검사하면 erase 해도 안전하다
    //     for (int i = (int)particles.size() - 1; i >= 0; i--) {
    //         if (particles[i].isDead()) {
    //             particles.erase(particles.begin() + i);
    //         }
    //     }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);

    canvas.begin();
    ofEnableAlphaBlending();
    ofSetColor(0, 0, 0, TRAIL_FADE);
    ofDrawRectangle(0, 0, canvas.getWidth(), canvas.getHeight());

    ofEnableBlendMode(OF_BLENDMODE_ADD);
    for (auto& p : particles) p.draw();
    for (auto& r : rockets) r.draw();
    ofDisableBlendMode();
    canvas.end();

    ofSetColor(255);
    canvas.draw(0, 0);

    ofSetColor(200);
    ofDrawBitmapString("shape:" + ofToString(shape) +
                       "  particles:" + ofToString(particles.size()), 10, 20);
}

//--------------------------------------------------------------
ofColor ofApp::randomColor() {
    ofColor c;
    c.setHsb(ofRandom(255), 200, 255);
    return c;
}

void ofApp::launchRocket(glm::vec2 target) {
    float x = ofRandom(ofGetWidth() * 0.25f, ofGetWidth() * 0.75f);
    rockets.push_back(Rocket(x, target, randomColor()));
}

void ofApp::explode(glm::vec2 pos, ofColor color) {
    if (shape == 2)      burstRing(pos, color);
    else if (shape == 3) burstHeart(pos, color);
    // TODO 8: 나만의 모양(burstMyShape)을 만들어 shape == 4 일 때 부르자!
    //   아이디어: 별(5각), 나선(angle 마다 speed 증가), 두 겹 링,
    //             중력 없는 불꽃, 폭발 후 또 폭발 ...
    //   (ofApp.h 에 함수 선언을 추가하는 것도 잊지 말기)
    else                 burstCircle(pos, color);
}

//--------------------------------------------------------------
void ofApp::burstCircle(glm::vec2 pos, ofColor color, int count) {
    for (int i = 0; i < count; i++) {
        // TODO 3: 랜덤한 방향과 속도로 파티클을 만들어 particles 에 넣자
        //   float angle = ofRandom(TWO_PI);
        //   float speed = ofRandom(1.0f, 6.0f);
        //   glm::vec2 vel(cos(angle) * speed, sin(angle) * speed);
        //   int life = (int)ofRandom(40, 80);
        //   particles.push_back(Particle(pos, vel, color, life));
    }
}

void ofApp::burstRing(glm::vec2 pos, ofColor color, int count) {
    // TODO 6: 링 모양
    //   힌트: 방향을 랜덤으로 하지 말고 360도를 count 등분!
    //         float angle = TWO_PI * i / count;   (i = 0 ~ count-1)
    //         속도(speed)는 모두 같게 (예: 5.0f)
}

void ofApp::burstHeart(glm::vec2 pos, ofColor color, int count) {
    // TODO 7: 하트 모양 (도전!)
    //   float t = TWO_PI * i / count;
    //   float x = 16 * pow(sin(t), 3);
    //   float y = -(13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t));
    //   vel = glm::vec2(x * 0.38f, y * 0.38f);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == '1' || key == '2' || key == '3' || key == '4') {
        shape = key - '0';
    } else if (key == 'c' || key == 'C') {
        canvas.begin();
        ofClear(0, 0, 0);
        canvas.end();
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (USE_ROCKET) {
        launchRocket(glm::vec2(x, y));
    } else {
        explode(glm::vec2(x, y), randomColor());
    }
}
