#pragma once
#include "ofMain.h"
#include "Particle.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void mousePressed(int x, int y, int button) override;

    void burstCircle(glm::vec2 pos, ofColor color, int count = 90);
    void burstRing(glm::vec2 pos, ofColor color, int count = 60);
    void burstHeart(glm::vec2 pos, ofColor color, int count = 70);
    void explode(glm::vec2 pos, ofColor color);

    void launchRocket(glm::vec2 target);
    ofColor randomColor();

    std::vector<Particle> particles;
    std::vector<Rocket> rockets;
    ofFbo canvas;
    int shape = 1;           // 1:원  2:링  3:하트
};
