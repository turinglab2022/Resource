"""
파티클 불꽃놀이 - PyGame 스타터 (학생용)

[1교시] TODO 1 ~ 4 를 채워서 "클릭하면 폭발하는" 불꽃을 만든다.
[2교시] TODO 5 ~ 8 을 채워서 로켓, 여러 모양, 나만의 불꽃으로 확장한다.

조작법
  마우스 클릭 : 불꽃 발사
  1 / 2 / 3   : 폭발 모양 선택
  ESC         : 종료
"""
import math
import random
import sys

import pygame
from pygame import Vector2

WIDTH, HEIGHT = 900, 600
FPS = 60
GRAVITY = 0.08
DRAG = 0.985
TRAIL_FADE = 14

USE_ROCKET = False   # [2교시] TODO 5 를 끝내면 True 로 바꾸자!


def random_color():
    c = pygame.Color(0)
    c.hsva = (random.uniform(0, 360), 100, 100, 100)
    return (c.r, c.g, c.b)


class Particle:
    def __init__(self, pos, vel, color, life, size=3):
        self.pos = Vector2(pos)
        self.vel = Vector2(vel)
        self.color = color
        self.life = life
        self.max_life = life
        self.size = size

    def update(self):
        # TODO 1: 파티클을 움직이자
        #   (1) vel.y 에 GRAVITY 를 더한다      -> 아래로 떨어진다
        #   (2) vel 에 DRAG 를 곱한다           -> 점점 느려진다
        #   (3) pos 에 vel 을 더한다            -> 실제로 이동한다
        #   (4) life 를 1 줄인다
        pass

    def is_dead(self):
        return self.life <= 0

    def draw(self, surface):
        # TODO 2: 수명이 줄수록 작아지고 어두워지게 그리자
        #   힌트: ratio = self.life / self.max_life   (1.0 -> 0.0)
        #   radius = max(1, int(self.size * ratio))
        #   color  = tuple(int(v * ratio) for v in self.color)
        #   pygame.draw.circle(surface, color, self.pos, radius)
        pass


class Rocket:
    SPEED = 9

    def __init__(self, x, target):
        self.pos = Vector2(x, HEIGHT)
        self.target = Vector2(target)
        self.color = random_color()
        self.arrived = False

    def update(self):
        # TODO 5: 목표(target)를 향해 SPEED 만큼 이동하자
        #   힌트: to_target = self.target - self.pos
        #         거리(to_target.length())가 SPEED 이하면 arrived = True
        #         아니면 self.pos += to_target.normalize() * self.SPEED
        pass

    def make_trail_spark(self):
        vel = (random.uniform(-0.5, 0.5), random.uniform(0.5, 1.5))
        return Particle(self.pos, vel, (255, 200, 120), life=18, size=2)

    def draw(self, surface):
        pygame.draw.circle(surface, (255, 255, 255), self.pos, 3)


def burst_circle(pos, color, count=90):
    particles = []
    for _ in range(count):
        # TODO 3: 랜덤한 방향과 속도로 파티클을 만들어 리스트에 넣자
        #   angle = random.uniform(0, 2 * math.pi)
        #   speed = random.uniform(1, 6)
        #   vel   = (cos(angle) * speed, sin(angle) * speed)
        #   life  = random.randint(40, 80)
        #   particles.append(Particle(pos, vel, color, life))
        pass
    return particles


def burst_ring(pos, color, count=60):
    particles = []
    # TODO 6: 링 모양
    #   힌트: 방향을 랜덤으로 하지 말고 360도를 count 등분!
    #         angle = 2 * math.pi * i / count     (i = 0 ~ count-1)
    #         속도(speed)는 모두 같게 (예: 5)
    return particles


def burst_heart(pos, color, count=70):
    particles = []
    # TODO 7: 하트 모양 (도전!)
    #   t = 2 * math.pi * i / count
    #   x = 16 * math.sin(t) ** 3
    #   y = -(13*cos(t) - 5*cos(2t) - 2*cos(3t) - cos(4t))
    #   vel = (x * 0.38, y * 0.38)
    return particles


SHAPES = {1: burst_circle, 2: burst_ring, 3: burst_heart}


# TODO 8: 나만의 불꽃 모양을 만들어 SHAPES 에 4번으로 등록하자!
#   아이디어: 별(5각), 나선(angle 마다 speed 증가), 두 겹 링,
#             색이 바뀌는 불꽃, 중력 없는 불꽃, 폭발 후 또 폭발 ...


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    canvas = pygame.Surface((WIDTH, HEIGHT))
    pygame.display.set_caption("Particle Fireworks")
    clock = pygame.time.Clock()
    font = pygame.font.SysFont(None, 22)

    particles = []
    rockets = []
    shape = 1

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                elif event.key in (pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4):
                    shape = event.key - pygame.K_0
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if USE_ROCKET:
                    rockets.append(Rocket(random.randint(WIDTH // 4, WIDTH * 3 // 4),
                                          event.pos))
                else:
                    particles += SHAPES[shape](event.pos, random_color())

        # --- 갱신 -------------------------------------------------------
        for rocket in rockets:
            rocket.update()
            particles.append(rocket.make_trail_spark())
            if rocket.arrived:
                particles += SHAPES[shape](rocket.pos, rocket.color)
        rockets = [r for r in rockets if not r.arrived]

        for p in particles:
            p.update()

        # TODO 4: 수명이 다한 파티클을 리스트에서 제거하자 (안 하면 점점 느려져요!)
        #   힌트: particles = [p for p in particles if not p.is_dead()]

        # --- 그리기 -----------------------------------------------------
        canvas.fill((TRAIL_FADE,) * 3, special_flags=pygame.BLEND_RGB_SUB)
        for p in particles:
            p.draw(canvas)
        for r in rockets:
            r.draw(canvas)
        screen.blit(canvas, (0, 0))

        info = f"shape:{shape}  particles:{len(particles)}"
        screen.blit(font.render(info, True, (200, 200, 200)), (10, 10))

        pygame.display.flip()
        clock.tick(FPS)

    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
