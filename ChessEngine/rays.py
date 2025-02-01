import pygame
import math
pygame.init()

WIDTH, HEIGHT = 2000, 1400

CLOCK = pygame.time.Clock()

WIN = pygame.display.set_mode((WIDTH, HEIGHT))

CIRCLE_CENTER = [400, 400]
CIRCLE_RADIUS = 200

RAY_LENGTH =2000

def insideCircleCheck(posx, posy):
	if ((posx-CIRCLE_CENTER[0])**2 + (posy-CIRCLE_CENTER[1])**2) <= CIRCLE_RADIUS**2:
		return True
	return False

def generateRays(number):
	rays = []

	for angleFactor in range(0, number):
		angle = (angleFactor/number)*math.pi*2
		rays.append(angle)

	return rays


mouse_init_pos = [0, 0]

RUNNING = True
HOLDING = False

rays = generateRays(100)


while RUNNING:
	pygame.display.flip()
	WIN.fill("grey")
	CLOCK.tick(20)

	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			RUNNING = False

		elif event.type == pygame.MOUSEBUTTONDOWN:
			
			mouse_init_pos = pygame.mouse.get_pos()
			HOLDING=insideCircleCheck(mouse_init_pos[0], mouse_init_pos[1])
			#CIRCLE_CENTER = [mouse_init_pos[0], mouse_init_pos[1]]

		elif event.type == pygame.MOUSEBUTTONUP:
			HOLDING=False
			mouse_init_pos = [0, 0]


	if HOLDING:
		mouse_current_pos = pygame.mouse.get_pos()
		dx, dy = mouse_current_pos[0]-mouse_init_pos[0], mouse_current_pos[1]-mouse_init_pos[1]
		CIRCLE_CENTER[0] += dx
		CIRCLE_CENTER[1] += dy
		mouse_init_pos = pygame.mouse.get_pos()


	
	for ray in rays:
		pygame.draw.line(WIN, "white", CIRCLE_CENTER, (CIRCLE_CENTER[0] + math.cos(ray)*RAY_LENGTH, CIRCLE_CENTER[1] + math.sin(ray)*RAY_LENGTH), 2)

	pygame.draw.rect(WIN, "yellow", (CIRCLE_CENTER[0]-CIRCLE_RADIUS, CIRCLE_CENTER[1]-CIRCLE_RADIUS, CIRCLE_RADIUS*2, CIRCLE_RADIUS*2), border_radius=300)



pygame.quit()