#pragma once
#include "ofMain.h"

// 벡터 하나: 시작점(tail)과 끝점(head). 좌표는 "격자 좌표"(1칸 = 1)이고 y는 위쪽이 +.
struct Vec2d {
    glm::vec2 tail{0, 0};
    glm::vec2 head{0, 0};
    bool valid = false;

    // TODO 1: (x성분, y성분) 을 돌려주자.  힌트: 성분 = 끝점 - 시작점
    glm::vec2 comp() const { return glm::vec2(0, 0); }
};

enum class Op { None, Add, Sub, Scale, Rotate };

// 연산 결과: 결과 벡터 + (더하기/빼기에서) 옮겨 그린 두 번째 벡터
struct Result {
    bool valid = false;
    Vec2d vec;
    Vec2d helper;
    bool hasHelper = false;
    std::string name;   // 예) "A + B"
    std::string calc;   // 예) "(3, 2) + (1, 4) = (4, 6)"
};

struct Button {
    int id;
    ofRectangle rect;
    std::string label;
    bool active;
};

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void windowResized(int w, int h) override;

private:
    // ---- 좌표 변환 ----
    glm::vec2 toScreen(glm::vec2 g) const;
    glm::vec2 toGrid(glm::vec2 s) const;
    glm::vec2 snapPt(glm::vec2 g) const;
    void updateLayout();

    // ---- 연산 ----
    Result computeResult() const;
    void adjustParam(int dir);
    int  pickHead(glm::vec2 mouse, float radius) const;   // 마우스 근처의 끝점 (0=A, 1=B, -1=없음)

    // ---- 그리기 도구 ----
    void line(glm::vec2 a, glm::vec2 b, float w);
    void dashed(glm::vec2 a, glm::vec2 b, float w);
    void arrow(glm::vec2 a, glm::vec2 b, float w, float headSize);
    void text(const std::string& s, float x, float y, bool small = false);
    float textWidth(const std::string& s, bool small = false);
    void drawGrid();
    void drawVector(const Vec2d& v, ofColor c, const std::string& name, bool showComponents);
    void drawHelper(const Vec2d& v, ofColor c);
    void drawRotationArc(const Vec2d& a);
    void drawPanel(const Result& r);

    // ---- UI ----
    void buildButtons();
    void drawButtons();
    void onButton(int id);
    void loadFonts();

    // ---- 상태 ----
    Vec2d vecs[2];            // 0 = A, 1 = B
    int   activeSlot = 0;     // 지금 그리는 벡터
    Op    op = Op::None;
    bool  snap = true;        // 격자 교점에 맞추기 (정수 성분)
    float scaleK = 2.0f;      // 상수배의 k
    float rotateDeg = 90.0f;  // 회전 각도 (반시계 +)

    // 마우스 조작 상태
    enum class Drag { None, Drawing, EditingHead, Moving };
    Drag  drag = Drag::None;
    int   dragIdx = -1;
    glm::vec2 grabOffset{0, 0};
    Vec2d backup;             // 그리다가 취소할 때 되돌릴 벡터

    glm::vec2 origin{0, 0};   // 화면에서 (0, 0)의 위치
    std::vector<Button> buttons;

    ofTrueTypeFont font, fontSmall;
    bool fontLoaded = false;

    static constexpr float kUnit = 40.0f;      // 격자 한 칸의 픽셀 수
    static constexpr float kToolbarH = 96.0f;
};
