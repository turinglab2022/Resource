"""
우주 슈팅 게임 (PyGame)

조작법
  방향키 / WASD : 이동
  SPACE         : 발사 (누르고 있으면 연사)
  R / ENTER     : 시작 · 다시 시작
  ESC           : 종료

프로그램 구조 (불꽃놀이와 같은 패턴!)
  Game.handle_event()  -> 키 이벤트 처리
  Game.update()        -> 모든 객체 이동 · 충돌 · 제거   (매 프레임)
  Game.draw()          -> 모든 객체 그리기               (매 프레임)
"""
import math
import random
import sys

import pygame
from pygame import Vector2

# ----------------------------------------------------------------------
# 설정값 (숫자를 바꿔 보며 게임 느낌을 조절해 보자!)
# ----------------------------------------------------------------------
WIDTH, HEIGHT = 600, 800
FPS = 60

PLAYER_SPEED = 5.0        # 플레이어 이동 속도 (픽셀/프레임)
PLAYER_RADIUS = 16        # 충돌 판정용 반지름
SHOOT_COOLDOWN = 12       # 발사 간격 (프레임). 60프레임 = 1초
BULLET_SPEED = 11.0
BULLET_RADIUS = 4
ENEMY_RADIUS = 18
ENEMY_HP = 1
SPAWN_INTERVAL = 50       # 적이 나타나는 간격 (프레임)


# ----------------------------------------------------------------------
# 충돌 판정
# ----------------------------------------------------------------------
def circles_collide(pos_a, radius_a, pos_b, radius_b):
    """두 원이 겹쳐 있으면 True."""
    # ★ TODO 6: 두 원이 겹쳤는지 판정하자
    #   힌트 1: 두 중심 사이의 거리: pos_a.distance_to(pos_b)
    #   힌트 2: 거리가 (반지름의 합)보다 작으면 겹친 것이다
    return False
    # ★ TODO 6 끝


# ----------------------------------------------------------------------
# 총알
# ----------------------------------------------------------------------
class Bullet:
    def __init__(self, pos, vel):
        self.pos = Vector2(pos)
        self.vel = Vector2(vel)
        self.radius = BULLET_RADIUS
        self.dead = False          # True 가 되면 리스트에서 제거된다

    def update(self):
        self.pos += self.vel
        # ★ TODO 4: 화면 밖으로 나간 총알은 dead 로 표시하자
        #   힌트 1: 위쪽: self.pos.y < -self.radius,  아래쪽: self.pos.y > HEIGHT + self.radius
        #   힌트 2: 왼쪽·오른쪽(3방향 발사 때 필요)도 같은 방식으로 검사한다
        pass
        # ★ TODO 4 끝

    def draw(self, surface):
        pygame.draw.circle(surface, (255, 240, 120), self.pos, self.radius)


# ----------------------------------------------------------------------
# 적
# ----------------------------------------------------------------------
class Enemy:
    def __init__(self, pos, vel, kind="straight"):
        self.pos = Vector2(pos)
        self.vel = Vector2(vel)
        self.kind = kind           # "straight" | "zigzag" | "boss"
        self.radius = ENEMY_RADIUS
        self.hp = ENEMY_HP
        self.fragile = True        # 플레이어와 부딪히면 사라지는가?
        self.base_x = self.pos.x   # 지그재그의 기준선
        self.age = 0               # 태어난 뒤 지난 프레임 수
        self.dead = False
        self.max_hp = self.hp

    def update(self):
        self.age += 1
        self.pos += self.vel
        if self.pos.y > HEIGHT + self.radius:
            self.dead = True       # 화면 아래로 빠져나감

    def draw(self, surface):
        x, y, r = self.pos.x, self.pos.y, self.radius
        pts = [(x - r, y - r), (x + r, y - r), (x, y + r)]
        pygame.draw.polygon(surface, (255, 80, 90), pts)


# ----------------------------------------------------------------------
# 플레이어
# ----------------------------------------------------------------------
class Player:
    def __init__(self):
        self.pos = Vector2(WIDTH / 2, HEIGHT - 100)
        self.radius = PLAYER_RADIUS
        self.cooldown = 0          # 0 이 되면 다시 쏠 수 있다

    def update(self, keys):
        """keys 는 pygame.key.get_pressed() 의 결과 (키가 눌려 있으면 True)."""
        move = Vector2(0, 0)
        # ★ TODO 1: 키 입력으로 이동 방향 벡터를 만들자
        #   힌트 1: 왼쪽 키(K_LEFT 또는 K_a)가 눌렸으면 move.x 를 1 줄이고, 오른쪽이면 1 늘린다
        #   힌트 2: 위·아래도 move.y 로 같은 방식. 아래쪽이 + 방향이다
        #   힌트 3: 대각선은 길이가 1.41 이라 더 빠르다! move.length() > 0 이면 move.normalize() 로 길이를 1 로 맞춘다
        pass
        # ★ TODO 1 끝

        # ★ TODO 2: 화면 밖으로 나가지 못하게 하자
        #   힌트 1: 값을 범위 안으로 가두기: max(최솟값, min(최댓값, 값))
        #   힌트 2: x 는 radius ~ WIDTH - radius,  y 는 radius ~ HEIGHT - radius
        pass
        # ★ TODO 2 끝

        if self.cooldown > 0:
            self.cooldown -= 1

    def try_shoot(self):
        """쏠 수 있으면 새 총알들의 리스트를, 아니면 빈 리스트를 돌려준다."""
        bullets = []
        muzzle = self.pos + Vector2(0, -self.radius)   # 총구 위치(기체 앞쪽)
        # ★ TODO 3: 쿨다운이 끝났을 때만 총알을 만들자
        #   힌트 1: self.cooldown <= 0 일 때만 발사할 수 있다
        #   힌트 2: 총알: Bullet(muzzle, Vector2(0, -BULLET_SPEED))  (위쪽이 - 방향)
        #   힌트 3: 쏘고 나면 self.cooldown = SHOOT_COOLDOWN 으로 다시 채운다
        pass
        # ★ TODO 3 끝
        return bullets

    def draw(self, surface):
        x, y = self.pos.x, self.pos.y
        flame = random.randint(6, 14)                     # 엔진 불꽃은 매 프레임 길이가 바뀐다
        pygame.draw.polygon(surface, (255, 160, 40),
                            [(x - 6, y + 10), (x + 6, y + 10), (x, y + 10 + flame)])
        body = [(x, y - 20), (x - 16, y + 16), (x, y + 8), (x + 16, y + 16)]
        pygame.draw.polygon(surface, (80, 200, 255), body)
        pygame.draw.circle(surface, (255, 255, 255), (x, y - 2), 4)


# ----------------------------------------------------------------------
# 게임 전체
# ----------------------------------------------------------------------
class Game:
    def __init__(self):
        self.font = pygame.font.SysFont(None, 28)
        self.big_font = pygame.font.SysFont(None, 72)
        self.stars = [[random.uniform(0, WIDTH), random.uniform(0, HEIGHT),
                       random.uniform(0.5, 3.0)] for _ in range(70)]
        self.reset()

    def reset(self):
        """게임을 처음 상태로 되돌린다."""
        self.player = Player()
        self.bullets = []
        self.enemies = []
        self.score = 0
        self.spawn_timer = 60      # 시작 후 1초 뒤에 첫 적
        self.state = "playing"

    # ---------------------------------------------------------- 입력
    def handle_event(self, event):
        if event.type != pygame.KEYDOWN:
            return
        if event.key == pygame.K_ESCAPE:
            pygame.quit()
            sys.exit()
        if event.key in (pygame.K_r, pygame.K_RETURN):
            if self.state == "gameover":
                self.reset()

    # ---------------------------------------------------------- 갱신
    def update(self, keys):
        self.update_stars()
        if self.state != "playing":
            return

        self.player.update(keys)
        if keys[pygame.K_SPACE]:
            self.bullets += self.player.try_shoot()

        self.update_spawning()
        for bullet in self.bullets:
            bullet.update()
        for enemy in self.enemies:
            enemy.update()

        self.check_bullet_enemy_collisions()
        self.check_player_enemy_collisions()

        # 죽은 객체 제거 (불꽃놀이의 TODO 4 와 같은 방법)
        self.bullets = [b for b in self.bullets if not b.dead]
        self.enemies = [e for e in self.enemies if not e.dead]

    def update_stars(self):
        for star in self.stars:
            star[1] += star[2]
            if star[1] > HEIGHT:
                star[0] = random.uniform(0, WIDTH)
                star[1] = 0

    # ---------------------------------------------------------- 적 생성
    def update_spawning(self):
        self.spawn_timer -= 1
        if self.spawn_timer <= 0:
            self.spawn_enemy()
            self.spawn_timer = self.current_spawn_interval()

    def current_spawn_interval(self):
        """다음 적이 나올 때까지 기다릴 프레임 수."""
        return SPAWN_INTERVAL

    def spawn_enemy(self):
        kind = "straight"
        # ★ TODO 5: 화면 위쪽 바깥에서 적을 한 마리 만들자
        #   힌트 1: x 는 화면 폭 안에서 랜덤: random.uniform(ENEMY_RADIUS, WIDTH - ENEMY_RADIUS)
        #   힌트 2: 시작 y 는 화면 위쪽 바깥(-ENEMY_RADIUS), 속도는 아래로 random.uniform(1.5, 3.5)
        #   힌트 3: self.enemies.append(Enemy(위치, 속도, kind))
        pass
        # ★ TODO 5 끝

    # ---------------------------------------------------------- 충돌 처리
    def check_bullet_enemy_collisions(self):
        # ★ TODO 7: 모든 (총알, 적) 쌍을 검사해서 맞았는지 확인하자
        #   힌트 1: 이중 for 문: for bullet in self.bullets: for enemy in self.enemies:
        #   힌트 2: 이미 dead 인 것은 건너뛴다 (한 발이 두 마리를 맞히면 안 된다)
        #   힌트 3: 맞으면: bullet.dead = True,  enemy.hp -= 1,  hp 가 0 이하면 enemy.dead = True 와 self.on_enemy_killed(enemy)
        pass
        # ★ TODO 7 끝

    def check_player_enemy_collisions(self):
        # ★ TODO 8: 플레이어가 적과 부딪혔는지 확인하자
        #   힌트 1: 모든 적에 대해 circles_collide(self.player.pos, self.player.radius, enemy.pos, enemy.radius)
        #   힌트 2: 부딪혔으면 enemy.fragile 인 적은 enemy.dead = True 로 만들고, self.on_player_hit() 를 부른다
        pass
        # ★ TODO 8 끝

    # ---------------------------------------------------------- 사건 처리
    def on_enemy_killed(self, enemy):
        self.score += 10

    def on_player_hit(self):
        self.state = "gameover"

    # ---------------------------------------------------------- 그리기
    def draw(self, surface):
        surface.fill((8, 10, 28))
        for x, y, speed in self.stars:
            shade = int(60 + speed * 50)
            pygame.draw.circle(surface, (shade, shade, shade), (x, y), 1 if speed < 2 else 2)

        for bullet in self.bullets:
            bullet.draw(surface)
        for enemy in self.enemies:
            enemy.draw(surface)
        if self.state != "title":
            self.player.draw(surface)
        self.draw_hud(surface)

        if self.state == "gameover":
            self.draw_center_text(surface, "GAME OVER", "Score %d    press R" % self.score)

    def draw_hud(self, surface):
        surface.blit(self.font.render("SCORE %d" % self.score, True, (255, 255, 255)), (12, 10))

    def draw_center_text(self, surface, title, subtitle):
        t = self.big_font.render(title, True, (255, 255, 255))
        s = self.font.render(subtitle, True, (200, 200, 220))
        surface.blit(t, t.get_rect(center=(WIDTH / 2, HEIGHT / 2 - 30)))
        surface.blit(s, s.get_rect(center=(WIDTH / 2, HEIGHT / 2 + 30)))


# ----------------------------------------------------------------------
# 메인 루프
# ----------------------------------------------------------------------
def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Space Shooter")
    clock = pygame.time.Clock()
    game = Game()

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            game.handle_event(event)

        game.update(pygame.key.get_pressed())
        game.draw(screen)

        pygame.display.flip()
        clock.tick(FPS)


if __name__ == "__main__":
    main()
