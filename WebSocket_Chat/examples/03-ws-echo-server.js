// ============================================================
// 예제 3) WebSocket 에코 서버 (`ws` 라이브러리)
// ------------------------------------------------------------
// TCP 예제와 비교해 보자.
//  - 메시지 경계를 직접 처리하지 않아도 된다 (프레임이 알아서 구분)
//  - 브라우저와 바로 통신 가능
//  - 연결 시작은 HTTP 요청(Upgrade)으로 한다
// ============================================================
const { WebSocketServer } = require('ws');

const wss = new WebSocketServer({ port: 8080 });

// 'connection' : 핸드셰이크(HTTP 101 Switching Protocols)가 끝나면 발생
wss.on('connection', (ws, req) => {
  console.log('[WS] 연결:', req.socket.remoteAddress);

  ws.send('환영합니다! 보내는 메시지를 그대로 돌려드립니다.');

  // 'message' : 완성된 "한 메시지"가 도착할 때마다 한 번 호출 (경계 걱정 없음)
  ws.on('message', (data, isBinary) => {
    // ws v8 에서 data 는 Buffer. 텍스트로 쓰려면 toString() 필요
    const text = isBinary ? `[바이너리 ${data.length}바이트]` : data.toString();
    console.log('[WS] 수신:', text);
    ws.send(`echo: ${text}`);
  });

  ws.on('close', (code, reason) => console.log('[WS] 종료:', code, reason.toString()));
  ws.on('error', (err) => console.error('[WS] 오류:', err.message));
});

console.log('WebSocket 에코 서버: ws://localhost:8080');
