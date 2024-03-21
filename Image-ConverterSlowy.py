import time, pygame, sys, os
pygame.init
width, height = 2400, 400
backgroundColor = 255, 255, 255
pygame.display.set_caption("Road")
pausetime = 0.2

carHeight = 50
startingY = 200
startingX = 50

colorCar1 = 255, 0, 0
colorCar2 = 0, 255, 0
colorCar3 = 0, 0, 255
colorCar4 = 0, 255, 255
colorCar5 = 255, 0, 255
colorCar6 = 255, 255, 0
colorCar7 = 0, 0, 0
colorCar8 = 127, 0, 127
colorCar9 = 127, 0, 255
colorCar10 = 127,127, 0
colorCar11 = 255, 127, 0
colorCar12 = 0, 127, 127

#fake Variables
printColor = 255, 255, 255

carColors = [backgroundColor, colorCar1, colorCar2, colorCar3, colorCar4, colorCar5, colorCar6, colorCar7, colorCar8, colorCar9, colorCar10, colorCar11, colorCar12, printColor, printColor]

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
