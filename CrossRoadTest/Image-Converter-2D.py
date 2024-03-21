import time, pygame, sys, os
pygame.init
width, height = 600, 600
centerCoords = 300, 300
backgroundColor = 255, 255, 255
pygame.display.set_caption("Road")
pausetime = 0.5

carHeight = 20

colorCar1 = 255, 0, 0
colorCar2 = 0, 255, 0
colorCar3 = 0, 0, 255

#fake Variables
printColor = 0, 0, 0
centerEmpty = backgroundColor
centerRight = colorCar1
centerDown = colorCar2
Right = colorCar1
Down = colorCar2

carColors = [backgroundColor, printColor, printColor, printColor, centerEmpty, centerRight, centerDown, Right, Down]

screen = pygame.display.set_mode((width, height))

dir = sys.argv[1]

dirList = []
for filename in os.listdir(dir) :
    dirList.append(filename)

dirList = sorted(dirList)

for filename in dirList :
    f = open(str(dir + '/' + filename), "r")
    tabright = []
    text = f.readline()
    while text != "3\n": 
        tabright.append(int(text))
        text = f.readline()
    text = f.readline()
    tabdown = []
    while text != "end\n": 
        tabdown.append(int(text))
        text = f.readline()
    tabdown.pop()
    f.close()
    #print(tabright)
    #print(tabdown)
    roadQuant = len(tabright)
    wingSize = (roadQuant-1)//2
    screen.fill(backgroundColor)
    x = centerCoords[0] - carHeight*wingSize
    startingY = centerCoords[1]
    for car in tabright:
        for y in range(carHeight):
            for z in range(carHeight):
            	screen.set_at((x+z, y+startingY), carColors[car])
            	
        x+=carHeight
    x = centerCoords[0]
    startingY = centerCoords[1] - carHeight*wingSize
    #print(y)
    for car in tabdown:
        #print(carColors[car])
        for yFill in range(carHeight):
            for z in range(carHeight):
                screen.set_at((x+z, yFill+startingY), carColors[car])
        startingY+=carHeight
    pygame.display.flip()
    time.sleep(pausetime)
