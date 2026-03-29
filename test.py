import math

TICK_MARK_COUNT = 30
OUTER_RADIUS = 240
INNER_RADIUS = 230
CENTER = 240

angle_step = 180 / TICK_MARK_COUNT
angle = 0
curr_tick = 0
tick_marks = []

while angle <= 180:
    angle_rad = angle * (math.pi / 180.0)

    x1 = CENTER + (OUTER_RADIUS * math.cos(angle_rad))
    y1 = CENTER + (OUTER_RADIUS * math.sin(angle_rad))
    x2 = CENTER + (INNER_RADIUS * math.cos(angle_rad))
    y2 = CENTER + (INNER_RADIUS * math.sin(angle_rad))

    tick_marks.append([(x1, y1), (x2, y2)])

    curr_tick += 1
    angle += angle_step

print(tick_marks)
