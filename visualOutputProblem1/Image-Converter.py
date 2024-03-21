import time, pygame, sys, os
pygame.init
width, height = 800, 800
backgroundColor = 255, 255, 255
pygame.display.set_caption("Road")
pausetime = 10

carHeight = 50
startingY = 400
startingX = 50

colorCar1 = 255, 0, 0
colorCar2 = 0, 255, 0
colorCar3 = 0, 0, 255

carColors = [backgroundColor, colorCar1, colorCar2, colorCar3]

screen = pygame.display.set_mode((width, height))

dir = "Input"

for filename in os.listdir(dir) :
    f = open(str(dir + '/' + filename), "r")
    tab = []
    text = f.readline()
    while text != "end" : 
        tab.append(int(f.readline()))
        text = f.readline()
    f.close()

    screen.fill (backgroundColor)
    x = startingX
    for car in tab:
        print(car)
        for y in range(carHeight):
            screen.set_at((x, y+startingY), carColors[car])
        x+=1

    pygame.display.flip()
    time.sleep(pausetime)