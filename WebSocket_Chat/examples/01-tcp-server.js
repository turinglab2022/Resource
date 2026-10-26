// ============================================================
// 예제 1) 순수 TCP 소켓 서버 (Node.js 내장 모듈 `net`)
// ------------------------------------------------------------
// "소켓"의 본질: IP + Port 로 연결된 양방향 바이트 스트림.
// HTTP도, 핸드셰이크도, 프레임도 없다. 의미(프로토콜)는 우리가 정한다.
// 여기서는 "줄바꿈(\n)으로 끝나면 한 메시지" 라는 규칙을 우리가 직접 정했다.
// ============================================================
const net = require('net');

const PORT = 4000;
const clients = new Set();

const server = net.createServer((socket) => {
  // 새 클라이언트가 TCP 3-way handshake를 마치면 이 콜백이 호출된다.
  console.log('[TCP] 연결:', socket.remoteAddress, socket.remotePort);
  clients.add(socket);

  // TCP는 "바이트 스트림"이다. 메시지 경계가 없다!
  // 보낸 쪽이 "hello", "world" 를 따로 보내도 받는 쪽에서는
  // "helloworld" 로 한 번에 오거나 "hel", "loworld" 로 쪼개져 올 수 있다.
  // 그래서 직접 경계(여기서는 \n)를 정하고 버퍼링해야 한다.
  let buffer = '';
  socket.on('data', (chunk) => {
    buffer += chunk.toString('utf8');
    let idx;
    while ((idx = buffer.indexOf('\n')) !== -1) {
      const line = buffer.slice(0, idx);
      buffer = buffer.slice(idx + 1);
      console.log('[TCP] 수신:', line);
      // 모든 클라이언트에게 브로드캐스트
      for (const c of clients) c.write(`echo: ${line}\n`);
    }
  });

  socket.on('close', () => clients.delete(socket));
  socket.on('error', (e) => console.error('[TCP] 오류:', e.message));
});

server.listen(PORT, () => console.log(`TCP 서버 실행: localhost:${PORT}`));
