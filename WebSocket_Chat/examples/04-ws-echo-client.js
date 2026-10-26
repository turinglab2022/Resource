// ============================================================
// 예제 4) WebSocket 클라이언트 (Node.js 22+ 에는 브라우저와 같은
//         전역 `WebSocket` 이 내장되어 있다. 브라우저 코드와 거의 동일!)
// ------------------------------------------------------------
// 실행: 먼저 `npm run ex:echo-server`, 그 다음 `npm run ex:echo-client`
// ============================================================
const ws = new WebSocket('ws://localhost:8080');

// readyState: 0 CONNECTING, 1 OPEN, 2 CLOSING, 3 CLOSED
ws.addEventListener('open', () => {
  console.log('연결 성공, readyState =', ws.readyState);
  ws.send('안녕, 웹소켓!');
  ws.send(JSON.stringify({ type: 'hello', at: Date.now() })); // 구조화 데이터는 JSON으로

  setTimeout(() => ws.close(1000, '정상 종료'), 500); // 1000 = Normal Closure
});

ws.addEventListener('message', (event) => console.log('수신:', event.data));
ws.addEventListener('close', (e) => console.log('종료:', e.code, e.reason));
ws.addEventListener('error', () => console.log('오류 발생 (서버가 켜져 있나요?)'));
