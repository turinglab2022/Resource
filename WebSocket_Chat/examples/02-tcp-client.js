// ============================================================
// 예제 2) 순수 TCP 소켓 클라이언트
// ------------------------------------------------------------
// 실행: 먼저 `npm run ex:tcp-server`, 그 다음 `npm run ex:tcp-client`
// 주의: 브라우저(JavaScript)에서는 이런 raw TCP 소켓을 열 수 없다!
//       브라우저가 쓸 수 있는 소켓은 WebSocket뿐이다 → 웹소켓이 필요한 이유.
// ============================================================
const net = require('net');
const readline = require('readline');

const socket = net.connect({ host: 'localhost', port: 4000 }, () => {
  console.log('서버에 연결됨. 메시지를 입력하세요.');
});

socket.on('data', (chunk) => process.stdout.write(chunk.toString('utf8')));
socket.on('close', () => { console.log('연결 종료'); process.exit(0); });

const rl = readline.createInterface({ input: process.stdin });
rl.on('line', (line) => socket.write(line + '\n')); // 경계 문자 '\n' 직접 붙이기
