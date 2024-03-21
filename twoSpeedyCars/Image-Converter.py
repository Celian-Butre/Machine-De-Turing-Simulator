import time, pygame, sys, os
pygame.init
width, height = 2400, 800
backgroundColor = 255, 255, 255
pygame.display.set_caption("Road")
pausetime = 0.1

carHeight = 50
startingY = 400
startingX = 50

colorCar1 = 255, 0, 0
colorCar2 = 0, 255, 0
colorCar3 = 0, 0, 255

#fake Variables
printColor = 0, 0, 0

carColors = [backgroundColor, printColor, printColor, colorCar1, colorCar2, colorCar3]

screen = pygame.display.set_mode((width, height))

dir = sys.argv[1]

dirList = []
for filename in os.listdir(dir) :
    dirList.append(filename)

dirList = sorted(dirList)

for filename in dirList :
    f = open(str(dir + '/' + filename), "r")
    tab = []
    text = f.readline()
    while text != "end\n" : 
        tab.append(int(text))
        text = f.readline()
    f.close()

    screen.fill (backgroundColor)
    x = startingX
    for car in tab:
        for y in range(carHeight):
            screen.set_at((x, y+startingY), carColors[car])
        x+=1

    pygame.display.flip()
    time.sleep(pausetime)
