"""Lesson 11 실습: Self-Attention 과 Transformer 직접 구현해 보기
=====================================================================
실행:  python lesson11_practice.py          (전체 실행)
       python lesson11_practice.py 1 3      (Part 1, 3 만 실행)
필요:  numpy, matplotlib, torch (Part 6~7)

사용 방법
  * '# TODO' 로 표시된 곳을 채우세요. 각 Part 의 check(...) 가 모두 PASS 면 정답입니다.
  * 막히면 lesson11_practice_solution.py 를 참고하세요.
  * Part 1~5: NumPy 로 수식을 그대로 구현 -> Part 6~7: PyTorch 로 Transformer 블록 구성/학습
"""
import sys
import numpy as np
import matplotlib.pyplot as plt

rng = np.random.default_rng(0)


def check(name, ok):
    print(f"  [{'PASS' if ok else 'FAIL'}] {name}")
    return ok

# ---------------------------------------------------------------------
# Part 1. Scaled Dot-Product Attention (식 11.4 ~ 11.6)
#   alpha_ij = q_i . k_j / sqrt(d_k)      a_ij = softmax_j(alpha_ij)      z_i = sum_j a_ij v_j
#   행렬 형태:  Attention(Q,K,V) = softmax(Q K^T / sqrt(d_k)) V
# ---------------------------------------------------------------------
def softmax(x, axis=-1):
    """수치 안정성을 위해 최댓값을 빼고 계산한다 (exp 오버플로 방지)."""
    # TODO: 여기를 구현하세요
    raise NotImplementedError


def attention(Q, K, V, mask=None, scale=True):
    """Q,K: (T, d_k)   V: (T, d_v)   mask: (T, T) bool, True = 볼 수 있음.
    return: (출력 Z (T, d_v), 어텐션 가중치 A (T, T))"""
    d_k = Q.shape[-1]
    # TODO: 여기를 구현하세요
    raise NotImplementedError


def part1():
    print("Part 1. scaled dot-product attention")
    x = np.array([[1.0, 2.0, 3.0], [1000.0, 1001.0, 1002.0]])
    s = softmax(x)
    check("softmax 행의 합 = 1, 큰 값에서도 NaN 없음", np.allclose(s.sum(1), 1) and not np.isnan(s).any())
    check("softmax 는 평행이동에 불변", np.allclose(s[0], s[1]))
    Q = K = np.eye(3)
    V = np.array([[1.0, 0], [0, 1.0], [5.0, 5.0]])
    Z, A = attention(Q, K, V)
    check("A 의 각 행 합 = 1", np.allclose(A.sum(1), 1))
    check("Z = A @ V", np.allclose(Z, A @ V))
    check("대각 성분이 가장 큼 (자기 자신과 가장 유사)", (A.argmax(1) == np.arange(3)).all())


# ---------------------------------------------------------------------
# Part 2. Q, K, V 투영과 Self-Attention 층 (식 11.1 ~ 11.3)
#   각 토큰 x_i 를 W_Q, W_K, W_V 로 선형 투영한다.
#   (코드에서는 X 가 (T, d) 행렬이므로  X @ W 로 쓴다.)
# ---------------------------------------------------------------------
WORDS = "The solution that the professor recommended was brilliant".split()


def self_attention(X, Wq, Wk, Wv, mask=None):
    # TODO: 여기를 구현하세요
    raise NotImplementedError


def part2():
    print("Part 2. self-attention 층 + 히트맵 (그림 11.1 따라 하기)")
    T, d, dk = len(WORDS), 16, 8
    X = rng.normal(size=(T, d))
    Wq, Wk, Wv = (rng.normal(size=(d, dk)) / np.sqrt(d) for _ in range(3))
    Z, A = self_attention(X, Wq, Wk, Wv)
    check("출력 shape = (T, d_k)", Z.shape == (T, dk))
    check("A shape = (T, T), 행 합 = 1", A.shape == (T, T) and np.allclose(A.sum(1), 1))
    # 의미 있는 어텐션을 '손으로' 만들어 보기: brilliant(마지막 행)가 solution(1번 열)을 보게 한다.
    X2 = rng.normal(size=(T, d)) * 0.3
    X2[1, 0] = 3.0        # solution : '주어' 표식 (0번 차원)
    X2[-1, 1] = 3.0       # brilliant: '수식어' 표식 (1번 차원) -> 주어를 찾는다
    Wq2 = np.zeros((d, dk)); Wq2[1, 0] = 3.0   # 수식어 표식 -> 질의 0 번 성분
    Wk2 = np.zeros((d, dk)); Wk2[0, 0] = 3.0   # 주어 표식   -> 키 0 번 성분
    _, A2 = self_attention(X2, Wq2, Wk2, np.eye(d)[:, :dk])
    check("brilliant 가 solution 에 가장 큰 가중치", A2[-1].argmax() == 1)
    fig, ax = plt.subplots(1, 2, figsize=(11, 4.5))
    for a, M, t in zip(ax, (A, A2), ("random weights", "hand-designed weights")):
        a.imshow(M, cmap="Blues"); a.set_title(t)
        a.set_xticks(range(T), WORDS, rotation=45, ha="right"); a.set_yticks(range(T), WORDS)
    plt.tight_layout(); plt.savefig("part2_attention_map.png", dpi=120); plt.close()
    print("  -> part2_attention_map.png 저장 (행=질의, 열=키)")


# ---------------------------------------------------------------------
# Part 3. 왜 sqrt(d_k) 로 나눌까?
#   q, k 의 각 성분이 평균 0, 분산 1 이면 q.k 의 분산은 d_k 이다.
#   스케일을 안 하면 d_k 가 클수록 softmax 가 one-hot 처럼 뾰족해져 기울기가 사라진다.
# ---------------------------------------------------------------------
def softmax_peakiness(dk, scale, T=16, trials=200):
    """softmax 가중치의 평균 최댓값 (1/T: 완전히 평평, 1: one-hot)."""
    vals = []
    for _ in range(trials):
        Q = rng.normal(size=(T, dk)); K = rng.normal(size=(T, dk))
        # TODO: 여기를 구현하세요
        raise NotImplementedError
    return float(np.mean(vals))


def part3():
    print("Part 3. 스케일링 효과")
    print(f"  {'d_k':>6} {'no scale':>10} {'scaled':>10}   (평균 max 가중치, T=16)")
    rows = []
    for dk in (4, 64, 1024):
        a, b = softmax_peakiness(dk, False), softmax_peakiness(dk, True)
        rows.append((a, b)); print(f"  {dk:>6} {a:>10.3f} {b:>10.3f}")
    check("스케일 없으면 d_k 클수록 뾰족(>0.9)", rows[-1][0] > 0.9)
    check("스케일하면 d_k 와 거의 무관(차이<0.1)", abs(rows[-1][1] - rows[0][1]) < 0.1)


# ---------------------------------------------------------------------
# Part 4. 마스크 (디코더의 Masked Self-Attention)
#   위치 i 는 j <= i 인 토큰만 볼 수 있다 -> 하삼각 행렬 마스크
# ---------------------------------------------------------------------
def causal_mask(T):
    # TODO: 여기를 구현하세요
    raise NotImplementedError


def part4():
    print("Part 4. causal mask")
    T = 6
    X = rng.normal(size=(T, 8))
    m = causal_mask(T)
    _, A = attention(X, X, X, mask=m)
    check("미래 위치 가중치 = 0", np.allclose(np.triu(A, 1), 0))
    check("행 합 = 1", np.allclose(A.sum(1), 1))
    X2 = X.copy(); X2[-1] += 10        # 마지막(미래) 토큰만 바꿔도
    Z1, _ = attention(X, X, X, mask=m); Z2, _ = attention(X2, X2, X2, mask=m)
    check("앞쪽 위치의 출력은 미래 토큰 변화에 영향받지 않음", np.allclose(Z1[:-1], Z2[:-1]))


# ---------------------------------------------------------------------
# Part 5. 위치 인코딩 -- 어텐션은 순서를 모른다
#   PE(pos, 2i) = sin(pos / 10000^(2i/d)),  PE(pos, 2i+1) = cos(pos / 10000^(2i/d))
# ---------------------------------------------------------------------
def positional_encoding(T, d):
    pos = np.arange(T)[:, None]
    i = np.arange(0, d, 2)[None, :]
    angle = pos / (10000 ** (i / d))
    pe = np.zeros((T, d))
    # TODO: 여기를 구현하세요
    raise NotImplementedError
    return pe


def part5():
    print("Part 5. positional encoding & 순서 불변성")
    T, d = 7, 16
    X = rng.normal(size=(T, d))
    Wq, Wk, Wv = (rng.normal(size=(d, d)) / np.sqrt(d) for _ in range(3))
    perm = rng.permutation(T)
    Z, _ = self_attention(X, Wq, Wk, Wv)
    Zp, _ = self_attention(X[perm], Wq, Wk, Wv)
    check("PE 없이: 입력을 섞으면 출력도 똑같이 섞일 뿐 (순서 정보 없음)", np.allclose(Z[perm], Zp))
    pe = positional_encoding(T, d)
    Z, _ = self_attention(X + pe, Wq, Wk, Wv)
    Zp, _ = self_attention(X[perm] + pe, Wq, Wk, Wv)
    check("PE 를 더하면 더 이상 단순 순서 바꿈이 아님", not np.allclose(Z[perm], Zp))
    check("PE shape / 범위", pe.shape == (T, d) and np.abs(pe).max() <= 1.0 + 1e-9)
    plt.figure(figsize=(7, 3.5)); plt.imshow(positional_encoding(50, 32).T, cmap="RdBu", aspect="auto")
    plt.xlabel("position"); plt.ylabel("dimension"); plt.colorbar(); plt.tight_layout()
    plt.savefig("part5_positional_encoding.png", dpi=120); plt.close()
    print("  -> part5_positional_encoding.png 저장")


# ---------------------------------------------------------------------
# Part 6. PyTorch: Multi-Head Attention + Transformer Encoder Block (그림 11.2)
#   x -> MHA -> Add&Norm -> FFN(Linear-GELU-Linear) -> Add&Norm
# ---------------------------------------------------------------------
def build_modules():
    import torch
    from torch import nn

    class MultiHeadAttention(nn.Module):
        def __init__(self, d_model, n_heads):
            super().__init__()
            assert d_model % n_heads == 0
            self.h, self.dk = n_heads, d_model // n_heads
            self.qkv = nn.Linear(d_model, 3 * d_model)
            self.out = nn.Linear(d_model, d_model)

        def forward(self, x, mask=None):             # x: (B, T, d_model)
            B, T, _ = x.shape
            # TODO: 여기를 구현하세요
            raise NotImplementedError
            self.last_attn = attn.detach()
            return self.out(z)

    class EncoderBlock(nn.Module):
        def __init__(self, d_model, n_heads, d_ff):
            super().__init__()
            self.mha = MultiHeadAttention(d_model, n_heads)
            self.ln1, self.ln2 = nn.LayerNorm(d_model), nn.LayerNorm(d_model)
            self.ffn = nn.Sequential(nn.Linear(d_model, d_ff), nn.GELU(), nn.Linear(d_ff, d_model))

        def forward(self, x, mask=None):
            # TODO: 여기를 구현하세요
            raise NotImplementedError

    return MultiHeadAttention, EncoderBlock


def part6():
    print("Part 6. PyTorch 멀티-헤드 어텐션 / 인코더 블록")
    import torch
    MultiHeadAttention, EncoderBlock = build_modules()
    torch.manual_seed(0)
    x = torch.randn(2, 10, 32)
    mha = MultiHeadAttention(32, 4)
    y = mha(x)
    check("MHA 출력 shape 유지 (2,10,32)", tuple(y.shape) == (2, 10, 32))
    check("어텐션 shape (B,h,T,T) 이고 행 합 = 1",
          tuple(mha.last_attn.shape) == (2, 4, 10, 10) and torch.allclose(mha.last_attn.sum(-1), torch.ones(2, 4, 10)))
    blk = EncoderBlock(32, 4, 64)
    check("EncoderBlock 출력 shape 유지", tuple(blk(x).shape) == (2, 10, 32))
    check("LayerNorm 후 토큰별 평균 ≈ 0", blk(x).mean(-1).abs().max().item() < 1e-4)
    n = sum(p.numel() for p in blk.parameters())
    print(f"  블록 파라미터 수: {n:,}  (d_model=32, d_ff=64)")


# ---------------------------------------------------------------------
# Part 7. 장거리 의존성 학습 실험
#   과제: 길이 T 시퀀스에서 '맨 앞 토큰'의 값을 마지막 위치에서 맞혀라.
#   -> 맨 앞과 맨 끝은 T-1 만큼 떨어져 있다. 어텐션은 직접 연결하므로 쉽게 푼다.
# ---------------------------------------------------------------------
def part7():
    print("Part 7. 장거리 의존성 과제 학습 (CPU 로 수십 초)")
    import torch
    from torch import nn
    _, Block = build_modules()
    torch.manual_seed(0)
    V, T, D = 10, 16, 32

    class Model(nn.Module):
        def __init__(self):
            super().__init__()
            self.tok = nn.Embedding(V, D)
            self.pos = nn.Embedding(T, D)                 # 학습 가능한 위치 임베딩
            self.block = Block(D, 4, 64)
            self.head = nn.Linear(D, V)

        def forward(self, x):
            h = self.tok(x) + self.pos(torch.arange(x.shape[1]))
            h = self.block(h)
            return self.head(h[:, -1])                    # 마지막 위치의 표현으로 분류

    model = Model()
    opt = torch.optim.Adam(model.parameters(), lr=3e-3)
    loss_fn = nn.CrossEntropyLoss()
    losses = []
    for step in range(600):
        x = torch.randint(0, V, (64, T))
        y = x[:, 0]                                       # 정답 = 맨 앞 토큰
        # TODO: 여기를 구현하세요
        raise NotImplementedError
        losses.append(loss.item())
        if step % 100 == 0:
            print(f"  step {step:3d}  loss {loss.item():.3f}")
    with torch.no_grad():
        x = torch.randint(0, V, (1000, T))
        acc = (model(x).argmax(-1) == x[:, 0]).float().mean().item()
        attn = model.block.mha.last_attn.mean(dim=(0, 1))[-1]   # 마지막 위치의 어텐션 (배치/헤드 평균)
    print(f"  정확도 = {acc:.3f}  (무작위 추측 = {1 / V:.2f})")
    check("정확도 > 0.9", acc > 0.9)
    print(f"  마지막 위치가 가장 주목한 위치 = {attn.argmax().item()}  (기대: 0)")
    plt.figure(figsize=(8, 3.2))
    plt.subplot(1, 2, 1); plt.plot(losses); plt.title("training loss"); plt.xlabel("step")
    plt.subplot(1, 2, 2); plt.bar(range(T), attn.numpy()); plt.title("attention of last position"); plt.xlabel("key position")
    plt.tight_layout(); plt.savefig("part7_long_range.png", dpi=120); plt.close()
    print("  -> part7_long_range.png 저장: 어텐션이 0 번 위치에 몰렸는지 확인하세요")


# ---------------------------------------------------------------------
if __name__ == "__main__":
    parts = {1: part1, 2: part2, 3: part3, 4: part4, 5: part5, 6: part6, 7: part7}
    chosen = [int(a) for a in sys.argv[1:]] or list(parts)
    for k in chosen:
        parts[k](); print()
    print("""[도전 과제]
  A. Part 2 에서 W_Q, W_K 를 바꿔 'the' 가 'professor' 를 보도록 만들어 보세요.
  B. Part 7 에서 위치 임베딩을 제거하면 어떻게 되는지 관찰하세요. 왜 그럴까요?
  C. EncoderBlock 을 2~4 층 쌓고 과제를 '맨 앞 토큰 + 맨 뒤 토큰의 합 mod 10' 으로 바꿔 보세요.
  D. causal mask 를 이용해 디코더(GPT 형) 블록을 만들고 다음 토큰 예측을 해 보세요.""")
