/*
 * 벡터 실험실 - openFrameworks 스타터 (학생용)
 *
 * [1교시] TODO 1 ~ 3 : 벡터의 성분을 구하고, 격자 좌표를 화면에 그려서
 *                      x성분 / y성분 / 점선이 나타나게 한다. (TODO 1 은 ofApp.h)
 * [2교시] TODO 4 ~ 6 : 더하기, 빼기, 상수배
 * [3교시] TODO 7 ~ 9 : 회전(도전), 크기(피타고라스), 나만의 연산
 *
 * 조작법: 왼쪽 드래그 = 벡터 그리기,  끝점 오른쪽 드래그 = 이동,
 *         위쪽 버튼으로 연산 선택
 */
#include "ofApp.h"
#include <set>

// ------------------------------------------------------------------
// 화면에 나오는 한글 문장은 전부 여기에 모은다.
// (ofTrueTypeFont 는 쓰는 글자만 미리 만들어 두므로, 여기 있는 글자가 자동으로 등록된다)
// ------------------------------------------------------------------
enum TextId {
    T_DRAW_A, T_DRAW_B, T_ADD, T_SUB, T_SCALE, T_ROTATE,
    T_SNAP_ON, T_SNAP_OFF, T_CLEAR,
    T_LEN, T_RESULT, T_SCALE_LABEL, T_ROT_LABEL, T_ROT_CALC,
    T_HINT1, T_HINT2, T_HINT3, T_SPECIAL,
    T_COUNT
};

static const char* kText[] = {
    "A 그리기", "B 그리기", "더하기  A+B", "빼기  A−B", "상수배  k×A", "회전",
    "격자에 맞춤: 켬", "격자에 맞춤: 끔", "모두 지우기",
    "크기", "결과", "배수  k = ", "회전 각도  θ = ", "A를 회전: ",
    "왼쪽 드래그: 점을 찍고 끌어서 벡터 그리기   (끝점을 끌면 모양이 바뀜)",
    "오른쪽 드래그 (끝점에서 시작): 벡터 이동",
    "키보드:  1 / 2 벡터 고르기,  G 격자 맞춤,  C 지우기,  + / - 값 바꾸기",
    "−×·°√²θ≈→",
};
static_assert(sizeof(kText) / sizeof(kText[0]) == T_COUNT, "kText 와 TextId 개수가 다릅니다");

enum ButtonId {
    B_SLOT_A, B_SLOT_B, B_ADD, B_SUB, B_SCALE, B_ROTATE, B_SNAP, B_CLEAR,
    B_MINUS, B_PLUS, B_ROT90
};

static const ofColor kColA(40, 100, 230);    // 파랑
static const ofColor kColB(235, 100, 40);    // 주황
static const ofColor kColR(30, 160, 80);     // 초록 (결과)
static const ofColor kInk(40);

// ---- Simple Helper Functions ----------------------------------------------

// 정수에 가까우면 정수로, 아니면 소수 첫째 자리까지
static std::string fmt(float v) {
    if (std::fabs(v - std::round(v)) < 0.005f) return ofToString((int)std::round(v));
    return ofToString(v, 1);
}
// 음수는 괄호로 감싼다:  (-3)²
static std::string fmtParen(float v) {
    return v < 0 ? "(" + fmt(v) + ")" : fmt(v);
}
static std::string pairStr(glm::vec2 c) {
    return "(" + fmt(c.x) + ", " + fmt(c.y) + ")";
}
// 크기(길이)를 피타고라스 정리로 보여준다:  √(3²+4²) = 5
static std::string lengthStr(glm::vec2 c) {
    // TODO 8: 벡터 c 의 길이를 피타고라스 정리로 구하자
    

    float len = 0;
    bool exact = std::fabs(len - std::round(len)) < 0.005f;
    return "√(" + fmtParen(c.x) + "²+" + fmtParen(c.y) + "²) " + (exact ? "= " : "≈ ") + fmt(len);
}

static std::vector<uint32_t> decodeUtf8(const std::string& s) {
    std::vector<uint32_t> out;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        // 문자 길이 계산
        int n = c < 0x80 ? 1 : c < 0xE0 ? 2 : c < 0xF0 ? 3 : 4;
        uint32_t cp = n == 1 ? c : (c & (0xFF >> (n + 1)));

        for (int k = 1; k < n && i + k < s.size(); k++) cp = (cp << 6) | (s[i + k] & 0x3F);
        out.push_back(cp);
        i += n;
    }
    return out;
}

// ------------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Vector Lab - 벡터 실험실");
    ofSetBackgroundColor(255);
    ofSetCircleResolution(40);
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    loadFonts();
    updateLayout();
}

void ofApp::loadFonts() {
    // 쓰는 글자만 등록 (한글 전체 11172자를 만들면 너무 느리다)
    std::set<uint32_t> glyphs;
    for (auto* t : kText)
        for (uint32_t cp : decodeUtf8(t))
            if (cp >= 0x80) glyphs.insert(cp);

    const char* candidates[] = {
        "font.ttf",                                   // bin/data/font.ttf 를 두면 우선 사용
        "C:/Windows/Fonts/malgun.ttf",                // 윈도우: 맑은 고딕
        "/System/Library/Fonts/AppleSDGothicNeo.ttc", // 맥
        "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
    };

    for (auto* path : candidates) {
        bool inData = std::string(path) == "font.ttf";
        std::string full = inData ? ofToDataPath(path, true) : path;

        if (!ofFile::doesFileExist(full, false)) continue;

        auto make = [&](int size, ofTrueTypeFont& f) {
            ofTrueTypeFontSettings s(full, size);
            s.antialiased = true;
            s.addRanges(ofAlphabet::Latin);
            for (uint32_t cp : glyphs) s.addRange(ofUnicode::range{cp, cp});
            return f.load(s);
        };

        if (make(18, font) && make(13, fontSmall)) {
            fontLoaded = true;
            return;
        }
    }
    ofLogWarning() << "한글 글꼴을 찾지 못했습니다. bin/data/font.ttf 를 넣어 주세요. (임시로 영어 글꼴 사용)";
}

// ---- 좌표 -----------------------------------------------------------
void ofApp::updateLayout() {
    // 좌표평면 정 가운데 위치를 계산
    origin = { std::round(ofGetWidth() / 2.0f), std::round((ofGetHeight() + kToolbarH) / 2.0f) };
}

glm::vec2 ofApp::toScreen(glm::vec2 g) const {
    // TODO 2: 격자 좌표 g 를 화면 좌표(픽셀)로 바꾸자
    //   - 한 칸은 kUnit 픽셀, 원점은 화면의 origin 위치
    //   - 화면은 아래로 갈수록 y 가 커지지만, 수학 좌표는 위로 갈수록 y 가 커진다 (y 를 뒤집기!)

    return origin; // 수정
}

glm::vec2 ofApp::toGrid(glm::vec2 s) const {
    return { (s.x - origin.x) / kUnit, (origin.y - s.y) / kUnit };
}

glm::vec2 ofApp::snapPt(glm::vec2 g) const {
    return snap ? glm::vec2(std::round(g.x), std::round(g.y)) : g;
}

void ofApp::windowResized(int, int) { updateLayout(); }

// ---- 연산 -----------------------------------------------------------
Result ofApp::computeResult() const {
    Result r;
    const Vec2d& a = vecs[0];
    const Vec2d& b = vecs[1];

    switch (op) {
    case Op::Add:
        if (!a.valid || !b.valid) break;
        // TODO 4: 더하기 A + B
        // 여기에 코드 작성
        break;
    case Op::Sub:
        if (!a.valid || !b.valid) break;
        // TODO 5: 빼기 A − B   (A − B = A + (−B) 라고 생각하자)
        // 여기에 코드 작성
        break;
    case Op::Scale:
        if (!a.valid) break;
        // TODO 6: 상수배 k × A    (k 는 scaleK)
        // 여기에 코드 작성
        break;
    case Op::Rotate: {
        if (!a.valid) break;
        // TODO 7 (도전): A 를 rotateDeg 도만큼 반시계 방향으로 회전
        //   float t = ofDegToRad(rotateDeg);          // 도 → 라디안
        // 여기에 코드 작성
        break;
    }
    // TODO 9: 나만의 연산 만들기! (단위벡터, 반대벡터, 수직벡터 (-y, x) ...)
    // 여기에 코드 작성
    default:
        break;
    }
    return r;
}

void ofApp::adjustParam(int dir) {
    if (op == Op::Scale) {
        scaleK = ofClamp(scaleK + 0.5f * dir, -5.0f, 5.0f);
    } else if (op == Op::Rotate) {
        rotateDeg += 15.0f * dir;
        if (rotateDeg > 180) rotateDeg -= 360;
        if (rotateDeg < -180) rotateDeg += 360;
    }
}

int ofApp::pickHead(glm::vec2 mouse, float radius) const {
    int best = -1;
    float bestD = radius;
    for (int i = 0; i < 2; i++) {
        if (!vecs[i].valid) continue;
        float d = glm::distance(mouse, toScreen(vecs[i].head));
        if (d <= bestD) { bestD = d; best = i; }
    }
    return best;
}

// ---- 그리기 도구 ----------------------------------------------------
// 굵은 선은 사각형(삼각형 2개)으로 그린다. (OpenGL 의 선 굵기에 의존하지 않기 위해)
void ofApp::line(glm::vec2 a, glm::vec2 b, float w) {
    glm::vec2 d = b - a;
    float len = glm::length(d);
    if (len < 0.01f) return;
    glm::vec2 n(-d.y / len * w * 0.5f, d.x / len * w * 0.5f);
    ofFill();
    ofDrawTriangle(a.x + n.x, a.y + n.y, a.x - n.x, a.y - n.y, b.x - n.x, b.y - n.y);
    ofDrawTriangle(a.x + n.x, a.y + n.y, b.x - n.x, b.y - n.y, b.x + n.x, b.y + n.y);
}

void ofApp::dashed(glm::vec2 a, glm::vec2 b, float w) {
    const float dash = 6, gap = 5;
    float len = glm::distance(a, b);
    if (len < 0.01f) return;
    glm::vec2 u = (b - a) / len;
    for (float t = 0; t < len; t += dash + gap)
        line(a + u * t, a + u * std::min(t + dash, len), w);
}

void ofApp::arrow(glm::vec2 a, glm::vec2 b, float w, float headSize) {
    glm::vec2 d = b - a;
    float len = glm::length(d);
    if (len < 1.0f) return;
    glm::vec2 u = d / len;
    glm::vec2 n(-u.y, u.x);
    float hs = std::min(headSize, len * 0.6f);
    line(a, b - u * (hs * 0.7f), w);
    glm::vec2 p1 = b - u * hs + n * (hs * 0.45f);
    glm::vec2 p2 = b - u * hs - n * (hs * 0.45f);
    ofFill();
    ofDrawTriangle(b.x, b.y, p1.x, p1.y, p2.x, p2.y);
}

void ofApp::text(const std::string& s, float x, float y, bool small) {
    if (fontLoaded) (small ? fontSmall : font).drawString(s, x, y);
    else ofDrawBitmapString(s, x, y);
}
float ofApp::textWidth(const std::string& s, bool small) {
    if (fontLoaded) return (small ? fontSmall : font).stringWidth(s);
    return s.size() * 8.0f;
}

void ofApp::drawGrid() {
    const int W = ofGetWidth(), H = ofGetHeight();

    // 격자선: 원점에서 사방으로 한 칸씩
    int nx0 = (int)std::floor(-origin.x / kUnit), nx1 = (int)std::ceil((W - origin.x) / kUnit);
    int ny0 = (int)std::floor(-(H - origin.y) / kUnit), ny1 = (int)std::ceil(origin.y / kUnit);
    for (int i = nx0; i <= nx1; i++) {
        ofSetColor(i % 5 == 0 ? 205 : 230);
        line({ origin.x + i * kUnit, 0 }, { origin.x + i * kUnit, (float)H }, 1);
    }
    for (int j = ny0; j <= ny1; j++) {
        ofSetColor(j % 5 == 0 ? 205 : 230);
        line({ 0, origin.y - j * kUnit }, { (float)W, origin.y - j * kUnit }, 1);
    }

    // 축
    ofSetColor(90);
    line({ 0, origin.y }, { (float)W, origin.y }, 2);
    line({ origin.x, 0 }, { origin.x, (float)H }, 2);
    text("x", W - 18, origin.y - 8);
    text("y", origin.x + 8, kToolbarH + 18);

    // 눈금 숫자
    ofSetColor(130);
    for (int i = nx0; i <= nx1; i++)
        if (i != 0) text(ofToString(i), origin.x + i * kUnit - textWidth(ofToString(i), true) / 2, origin.y + 15, true);
    for (int j = ny0; j <= ny1; j++)
        if (j != 0) text(ofToString(j), origin.x - textWidth(ofToString(j), true) - 6, origin.y - j * kUnit + 5, true);
    text("O", origin.x - 14, origin.y + 15, true);
}

// 벡터 + x성분 + y성분 + 점선
void ofApp::drawVector(const Vec2d& v, ofColor c, const std::string& name, bool showComponents) {
    if (!v.valid) return;
    glm::vec2 T = toScreen(v.tail), H = toScreen(v.head);
    glm::vec2 comp = v.comp();

    if (showComponents) {
        // TODO 3: x성분의 끝(P1)과 y성분의 끝(P2)의 화면 위치를 구하자
        //   x성분은 시작점에서 가로로, y성분은 시작점에서 세로로 그린다.
        //   P1 은 "끝점 H 의 x, 시작점 T 의 y",  P2 는 "시작점 T 의 x, 끝점 H 의 y"
        //   여기에 코드 작성
        glm::vec2 P1 = T;
        glm::vec2 P2 = T;
        ofColor soft(c, 150);

        // x성분의 끝, y성분의 끝에서 벡터의 끝까지 점선
        ofSetColor(soft);
        dashed(P1, H, 1.8f);
        dashed(P2, H, 1.8f);

        // 성분 화살표 (시작점에서 가로, 세로로)
        ofSetColor(soft);
        arrow(T, P1, 2.5f, 9);
        arrow(T, P2, 2.5f, 9);

        // 성분 이름표: 직사각형 바깥쪽에 쓴다
        ofSetColor(c);
        if (std::fabs(H.x - T.x) > 1) {
            std::string s = "x = " + fmt(comp.x);
            float ty = (H.y < T.y) ? T.y + 20 : T.y - 8;
            text(s, (T.x + P1.x) / 2 - textWidth(s) / 2, ty);
        }
        if (std::fabs(H.y - T.y) > 1) {
            std::string s = "y = " + fmt(comp.y);
            float tx = (H.x > T.x) ? T.x - textWidth(s) - 8 : T.x + 8;
            text(s, tx, (T.y + P2.y) / 2 + 5);
        }
    }

    // 벡터 본체
    ofSetColor(c);
    arrow(T, H, 4, 16);
    ofDrawCircle(T.x, T.y, 5);
    ofSetColor(c, 90);
    ofDrawCircle(H.x, H.y, 11);   // 끝점 손잡이 (여기를 끌면 된다)

    // 이름표: 벡터 방향으로 끝점 바깥쪽
    glm::vec2 d = H - T;
    float len = glm::length(d);
    glm::vec2 u = len > 1 ? d / len : glm::vec2(1, 0);
    std::string label = name + " " + pairStr(comp);
    float lw = textWidth(label);
    glm::vec2 p = H + u * 20.0f;
    if (u.x < 0) p.x -= lw;
    ofSetColor(c);
    text(label, p.x, p.y + 6);
}

// 더하기·빼기에서 옮겨 그린 두 번째 벡터
void ofApp::drawHelper(const Vec2d& v, ofColor c) {
    glm::vec2 T = toScreen(v.tail), H = toScreen(v.head);
    ofSetColor(c, 140);
    arrow(T, H, 3, 14);
}

// 회전각을 보여주는 호
void ofApp::drawRotationArc(const Vec2d& a) {
    glm::vec2 c = a.comp();
    if (glm::length(c) < 0.01f) return;
    float a0 = std::atan2(c.y, c.x);
    float a1 = a0 + ofDegToRad(rotateDeg);
    glm::vec2 T = toScreen(a.tail);
    const float R = 34;
    ofSetColor(kColR, 200);
    int steps = std::max(2, (int)(std::fabs(rotateDeg) / 5));
    glm::vec2 prev = T + glm::vec2(std::cos(a0), -std::sin(a0)) * R;
    for (int i = 1; i <= steps; i++) {
        float t = ofLerp(a0, a1, (float)i / steps);
        glm::vec2 cur = T + glm::vec2(std::cos(t), -std::sin(t)) * R;
        line(prev, cur, 2.5f);
        prev = cur;
    }
    float mid = (a0 + a1) / 2;
    glm::vec2 lp = T + glm::vec2(std::cos(mid), -std::sin(mid)) * (R + 16);
    text("θ", lp.x - 5, lp.y + 5);
}

void ofApp::drawPanel(const Result& r) {
    std::vector<std::pair<std::string, ofColor>> lines;
    const ofColor ink = kInk;
    for (int i = 0; i < 2; i++) {
        if (!vecs[i].valid) continue;
        glm::vec2 c = vecs[i].comp();
        std::string n(1, i == 0 ? 'A' : 'B');
        lines.push_back({ n + " = " + pairStr(c), i == 0 ? kColA : kColB });
        lines.push_back({ "   " + std::string(kText[T_LEN]) + " " + lengthStr(c), ink });
    }
    if (r.valid) {
        lines.push_back({ std::string(kText[T_RESULT]) + "  " + r.name, kColR });
        lines.push_back({ "   " + r.calc, kColR });
        lines.push_back({ "   " + std::string(kText[T_LEN]) + " " + lengthStr(r.vec.comp()), ink });
    }
    if (lines.empty()) return;

    float w = 0;
    for (auto& l : lines) w = std::max(w, textWidth(l.first));
    w += 28;
    float h = lines.size() * 26 + 18;
    float x = 14, y = kToolbarH + 12;

    ofSetColor(255, 255, 255, 235);
    ofDrawRectRounded(x, y, w, h, 8);
    ofSetColor(200);
    line({ x, y }, { x + w, y }, 1);
    line({ x, y + h }, { x + w, y + h }, 1);
    line({ x, y }, { x, y + h }, 1);
    line({ x + w, y }, { x + w, y + h }, 1);

    float ty = y + 28;
    for (auto& l : lines) {
        ofSetColor(l.second);
        text(l.first, x + 14, ty);
        ty += 26;
    }
}

// ---- UI -------------------------------------------------------------
void ofApp::buildButtons() {
    buttons.clear();
    const float gap = 8, h = 40, y = 8;
    struct Def { int id; std::string label; bool active; };
    std::vector<Def> row1 = {
        { B_SLOT_A, kText[T_DRAW_A], activeSlot == 0 },
        { B_SLOT_B, kText[T_DRAW_B], activeSlot == 1 },
        { B_ADD,    kText[T_ADD],    op == Op::Add },
        { B_SUB,    kText[T_SUB],    op == Op::Sub },
        { B_SCALE,  kText[T_SCALE],  op == Op::Scale },
        { B_ROTATE, kText[T_ROTATE], op == Op::Rotate },
        { B_SNAP,   snap ? kText[T_SNAP_ON] : kText[T_SNAP_OFF], snap },
        { B_CLEAR,  kText[T_CLEAR],  false },
    };
    float bw = (ofGetWidth() - gap * (row1.size() + 1)) / row1.size();
    for (size_t i = 0; i < row1.size(); i++)
        buttons.push_back({ row1[i].id, ofRectangle(gap + i * (bw + gap), y, bw, h), row1[i].label, row1[i].active });

    // 둘째 줄: 상수배 / 회전의 값 조절
    if (op == Op::Scale || op == Op::Rotate) {
        float x = gap, y2 = y + h + 8, s = 34;
        std::string label = op == Op::Scale ? std::string(kText[T_SCALE_LABEL]) + fmt(scaleK)
                                            : std::string(kText[T_ROT_LABEL]) + fmt(rotateDeg) + "°";
        float lw = textWidth(label) + 20;
        buttons.push_back({ B_MINUS, ofRectangle(x, y2, s, s), "−", false });
        buttons.push_back({ -1,      ofRectangle(x + s + 4, y2, lw, s), label, false });   // 값 표시 (눌러도 반응 없음)
        buttons.push_back({ B_PLUS,  ofRectangle(x + s + 8 + lw, y2, s, s), "+", false });
        if (op == Op::Rotate)
            buttons.push_back({ B_ROT90, ofRectangle(x + 2 * s + 16 + lw, y2, 60, s), "90°", false });
    }
}

void ofApp::drawButtons() {
    glm::vec2 m(ofGetMouseX(), ofGetMouseY());
    ofSetColor(248);
    ofDrawRectangle(0, 0, ofGetWidth(), kToolbarH);
    ofSetColor(215);
    line({ 0, kToolbarH }, { (float)ofGetWidth(), kToolbarH }, 1);

    for (auto& b : buttons) {
        bool hover = b.id >= 0 && b.rect.inside(m.x, m.y);
        bool isValue = b.id < 0;
        ofColor fill = b.active ? ofColor(60, 90, 160) : isValue ? ofColor(248) : hover ? ofColor(225, 232, 245) : ofColor(255);
        ofSetColor(fill);
        ofDrawRectRounded(b.rect, 8);
        if (!isValue) {
            ofSetColor(b.active ? ofColor(60, 90, 160) : ofColor(180));
            // 테두리
            float x = b.rect.x, y = b.rect.y, w = b.rect.width, h = b.rect.height;
            line({ x + 4, y }, { x + w - 4, y }, 1);
            line({ x + 4, y + h }, { x + w - 4, y + h }, 1);
            line({ x, y + 4 }, { x, y + h - 4 }, 1);
            line({ x + w, y + 4 }, { x + w, y + h - 4 }, 1);
        }
        ofSetColor(b.active ? ofColor(255) : kInk);
        float tw = textWidth(b.label);
        text(b.label, b.rect.x + (b.rect.width - tw) / 2, b.rect.y + b.rect.height / 2 + 6);
    }
}

void ofApp::onButton(int id) {
    switch (id) {
    case B_SLOT_A: activeSlot = 0; break;
    case B_SLOT_B: activeSlot = 1; break;
    case B_ADD:    op = (op == Op::Add)    ? Op::None : Op::Add;    break;
    case B_SUB:    op = (op == Op::Sub)    ? Op::None : Op::Sub;    break;
    case B_SCALE:  op = (op == Op::Scale)  ? Op::None : Op::Scale;  break;
    case B_ROTATE: op = (op == Op::Rotate) ? Op::None : Op::Rotate; break;
    case B_SNAP:   snap = !snap; break;
    case B_CLEAR:
        vecs[0] = Vec2d(); vecs[1] = Vec2d();
        activeSlot = 0; op = Op::None;
        break;
    case B_MINUS:  adjustParam(-1); break;
    case B_PLUS:   adjustParam(+1); break;
    case B_ROT90:  rotateDeg = 90; break;
    }
}

// ---- 메인 루프 ------------------------------------------------------
void ofApp::update() {
    updateLayout();
    buildButtons();
}

void ofApp::draw() {
    ofFill();
    drawGrid();

    Result r = computeResult();
    if (r.valid && r.hasHelper) drawHelper(r.helper, kColB);

    drawVector(vecs[0], kColA, "A", true);
    drawVector(vecs[1], kColB, "B", true);
    if (r.valid) {
        if (op == Op::Rotate) drawRotationArc(vecs[0]);
        drawVector(r.vec, kColR, r.name, true);
    }

    drawPanel(r);

    // 도움말 (아래쪽)
    ofSetColor(120);
    float y = ofGetHeight() - 14;
    text(kText[T_HINT3], 14, y - 44, true);
    text(kText[T_HINT2], 14, y - 24, true);
    text(kText[T_HINT1], 14, y - 4, true);

    drawButtons();
}

// ---- 입력 -----------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
    case '1': activeSlot = 0; break;
    case '2': activeSlot = 1; break;
    case 'g': case 'G': snap = !snap; break;
    case 'c': case 'C': onButton(B_CLEAR); break;
    case '+': case '=': case OF_KEY_UP: case OF_KEY_RIGHT: adjustParam(+1); break;
    case '-': case '_': case OF_KEY_DOWN: case OF_KEY_LEFT: adjustParam(-1); break;
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    glm::vec2 m(x, y);

    // 위쪽 버튼 줄
    if (y < kToolbarH) {
        if (button == OF_MOUSE_BUTTON_LEFT)
            for (auto& b : buttons)
                if (b.id >= 0 && b.rect.inside(x, y)) { onButton(b.id); break; }
        return;
    }

    if (button == OF_MOUSE_BUTTON_RIGHT) {
        // 끝점을 오른쪽 클릭 → 벡터 통째로 이동
        int i = pickHead(m, 18);
        if (i >= 0) {
            drag = Drag::Moving;
            dragIdx = i;
            grabOffset = vecs[i].head - toGrid(m);
        }
        return;
    }

    if (button != OF_MOUSE_BUTTON_LEFT) return;

    // 끝점을 왼쪽 클릭 → 모양 바꾸기
    int i = pickHead(m, 14);
    if (i >= 0) {
        drag = Drag::EditingHead;
        dragIdx = i;
        grabOffset = vecs[i].head - toGrid(m);
        return;
    }

    // 빈 곳을 왼쪽 클릭 → 새 벡터 그리기 시작
    dragIdx = activeSlot;
    backup = vecs[dragIdx];
    glm::vec2 p = snapPt(toGrid(m));
    vecs[dragIdx] = { p, p, true };
    drag = Drag::Drawing;
}

void ofApp::mouseDragged(int x, int y, int) {
    if (drag == Drag::None || dragIdx < 0) return;
    glm::vec2 g = toGrid({ (float)x, (float)y });
    Vec2d& v = vecs[dragIdx];

    if (drag == Drag::Drawing) {
        v.head = snapPt(g);
    } else if (drag == Drag::EditingHead) {
        v.head = snapPt(g + grabOffset);
    } else if (drag == Drag::Moving) {
        glm::vec2 target = snapPt(g + grabOffset);
        glm::vec2 delta = target - v.head;
        v.tail += delta;
        v.head = target;
    }
}

void ofApp::mouseReleased(int, int, int) {
    if (drag == Drag::Drawing && dragIdx >= 0) {
        Vec2d& v = vecs[dragIdx];
        if (glm::distance(v.tail, v.head) < 0.01f) {
            v = backup;                       // 길이 0 이면 취소
        } else if (activeSlot == 0 && !vecs[1].valid) {
            activeSlot = 1;                   // A 를 다 그렸으면 다음은 B
        }
    }
    if (drag == Drag::EditingHead && dragIdx >= 0) {
        Vec2d& v = vecs[dragIdx];
        if (glm::distance(v.tail, v.head) < 0.01f) v.head = v.tail + glm::vec2(1, 0);   // 길이 0 방지
    }
    drag = Drag::None;
    dragIdx = -1;
}
