# WebSocket 강의 자료 & 실습 코드

| 파일 | 설명 |
|---|---|
| [`LECTURE.md`](LECTURE.md) | **강의자료** — 소켓 vs WebSocket 비교, 동작 원리, 구현, 단계별 구현(STEP 1~11), 기능 상세 해설, 과제/퀴즈 |
| `WebSocket_Lecture_TuringLab.pptx` | **강의 슬라이드** (밝은 인쇄용 테마, 55장, 발표자 노트 포함) |
| `examples/01~02` | 순수 TCP 소켓 서버/클라이언트 (Node `net`) |
| `examples/03~04` | WebSocket 에코 서버/클라이언트 (`ws`) |
| `examples/05-browser-echo.html` | 브라우저 최소 WebSocket 클라이언트 |
| `chat/` | **웹 채팅 앱** (주석으로 설명된 소스) |

## 실행

```bash
npm install
npm run chat            # http://localhost:3000 (브라우저 탭 2개로 테스트)

# 예제
npm run ex:tcp-server   &  npm run ex:tcp-client
npm run ex:echo-server   &  npm run ex:echo-client
```

요구사항: Node.js 22 이상 (전역 `WebSocket` 사용)

## 채팅 앱 구조

```
chat/server.js          HTTP 정적 서버 + WebSocket 서버 (한 포트)
chat/public/index.html  화면
chat/public/style.css   스타일
chat/public/client.js   브라우저 클라이언트
```
