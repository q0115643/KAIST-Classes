# searchAgents.py
# --------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html

from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

##this is example agents 
class LeftTurnAgent(Agent):
  "An agent that turns left at every opportunity"
  
  def getAction(self, state):
    legal = state.getLegalPacmanActions()
    current = state.getPacmanState().configuration.direction
    if current == Directions.STOP: current = Directions.NORTH
    left = Directions.LEFT[current]
    if left in legal: return left
    if current in legal: return current
    if Directions.RIGHT[current] in legal: return Directions.RIGHT[current]
    if Directions.LEFT[left] in legal: return Directions.LEFT[left]
    return Directions.STOP

class GreedyAgent(Agent):
  def __init__(self, evalFn="scoreEvaluation"):
    self.evaluationFunction = util.lookup(evalFn, globals())
    assert self.evaluationFunction != None
        
  def getAction(self, state):
    # Generate candidate actions
    legal = state.getLegalPacmanActions()
    if Directions.STOP in legal: legal.remove(Directions.STOP)
      
    successors = [(state.generateSuccessor(0, action), action) for action in legal] 
    scored = [(self.evaluationFunction(state), action) for state, action in successors]
    bestScore = max(scored)[0]
    bestActions = [pair[1] for pair in scored if pair[0] == bestScore]
    return random.choice(bestActions)

class BFSAgent(Agent):
  """
    Your BFS agent (question 1)
  """
  def __init__(self):
      self.fringe = util.Queue()
      self.closed = []
      self.xPositionCorrection = 0
      self.yPositionCorrection = 0
      self.backTrack = False
      self.backDest = (0, [0, 0])
      self.parent = {}
      self.children = {}
      self.startPos = (0, 0)

  def getAction(self, gameState):

    """
      Returns the BFS seracing action using gamestate.getLegalActions()
      
      legal moves can be accessed like below 
      legalMoves = gameState.getLegalActions()
      this method returns current legal moves that pac-man can have in curruent state
      returned results are list, combination of "North","South","West","East","Stop"
      we will not use stop action for this project
     
      Please write code that Pacman traverse map in BFS order. 
      Because Pac-man does not have any information of map, it should move around in order to get 
      information that is needed to reach to the goal.

      Also please print order of x,y coordinate of location that Pac-man first visit in result.txt file with format
      (x,y)
      (x1,y1)
      (x2,y2)
      .
      .
      . 
      (xn,yn)
      note that position that Pac-man starts is considered to be (0,0)
      
      this method is called until Pac-man reaches to goal
      return value should be one of the direction Pac-man can move ('North','South'....)
    """
    "*** YOUR CODE HERE ***"
    legalMoves = gameState.getLegalActions()
    if Directions.STOP in legalMoves: legalMoves.remove(Directions.STOP)
    currentDirection = gameState.getPacmanState().configuration.direction
    currentPosition = gameState.getPacmanState().configuration.pos
    if currentDirection == Directions.STOP:
        self.xPositionCorrection = currentPosition[0]
        self.yPositionCorrection = currentPosition[1]
        self.startPos = currentPosition
        self.parent[self.startPos] = "n"
        with open("result.txt", 'w') as initialFile:
            initialFile.write("")
    if currentPosition not in self.closed:
        self.closed.append(currentPosition)
        with open("result.txt", 'a') as file:
            file.write("(%s, %s)\n" % (str(currentPosition[0] - self.xPositionCorrection), str(currentPosition[1] - self.yPositionCorrection)))
    successors = {}
    for action in legalMoves:
        successorState = gameState.generateSuccessor(0, action)
        successorStatePos = successorState.getPacmanState().configuration.pos
        successors[action] = successorStatePos
    if not self.backTrack:
        self.children[currentPosition] = {}
        if Directions.EAST in legalMoves:
            dest = successors[Directions.EAST]
            if dest not in self.closed:
                self.closed.append(dest)
                self.fringe.push([dest, currentPosition, Directions.EAST])
                self.children[currentPosition][dest] = Directions.EAST
                self.parent[dest] = (currentPosition, Directions.EAST)
                if not currentPosition == self.startPos:
                    current = currentPosition
                    parent = self.parent[current][0]
                    while not parent == "n":
                        for des, dr in self.children[current].items():
                            self.children[parent][des] = self.children[parent][current]
                        current = parent
                        parent = self.parent[parent][0]
        if Directions.WEST in legalMoves:
            dest = successors[Directions.WEST]
            if dest not in self.closed:
                self.closed.append(dest)
                self.fringe.push([dest, currentPosition, Directions.WEST])
                self.children[currentPosition][dest] = Directions.WEST
                self.parent[dest] = (currentPosition, Directions.WEST)
                if not currentPosition == self.startPos:
                    current = currentPosition
                    parent = self.parent[current][0]
                    while not parent == "n":
                        for des, dr in self.children[current].items():
                            self.children[parent][des] = self.children[parent][current]
                        current = parent
                        parent = self.parent[parent][0]
        if Directions.SOUTH in legalMoves:
            dest = successors[Directions.SOUTH]
            if dest not in self.closed:
                self.closed.append(dest)
                self.fringe.push([dest, currentPosition, Directions.SOUTH])
                self.children[currentPosition][dest] = Directions.SOUTH
                self.parent[dest] = (currentPosition, Directions.SOUTH)
                if not currentPosition == self.startPos:
                    current = currentPosition
                    parent = self.parent[current][0]
                    while not parent == "n":
                        for des, dr in self.children[current].items():
                            self.children[parent][des] = self.children[parent][current]
                        current = parent
                        parent = self.parent[parent][0]
        if Directions.NORTH in legalMoves:
            dest = successors[Directions.NORTH]
            if dest not in self.closed:
                self.closed.append(dest)
                self.fringe.push([dest, currentPosition, Directions.NORTH])
                self.children[currentPosition][dest] = Directions.NORTH
                self.parent[dest] = (currentPosition, Directions.NORTH)
                if not currentPosition == self.startPos:
                    current = currentPosition
                    parent = self.parent[current][0]
                    while not parent == "n":
                        for des, dr in self.children[current].items():
                            self.children[parent][des] = self.children[parent][current]
                        current = parent
                        parent = self.parent[parent][0]
    if not self.backTrack:
        item = self.fringe.pop()
    else:
        item = self.backDest

    if not item[1] == currentPosition:
        self.backTrack = True
        self.backDest = item
    else:
        self.backTrack = False
    if not self.backTrack:
        with open("result.txt", 'a') as file:
            file.write("(%s, %s)\n" % (str(item[0][0] - self.xPositionCorrection), str(item[0][1] - self.yPositionCorrection)))
        dir = item[2]
        return dir
    else:
        for child in self.children[currentPosition]:
            if child == self.backDest[0]:
                return self.children[currentPosition][child]
        return oppositeDir(self.parent[currentPosition][1])

def oppositeDir(dir):
    if dir == Directions.EAST:
        return Directions.WEST
    if dir == Directions.WEST:
        return Directions.EAST
    if dir == Directions.NORTH:
        return Directions.SOUTH
    if dir == Directions.SOUTH:
        return Directions.NORTH


class AstarAgent(Agent):
  """
    Your astar agent (question 2)

    An astar agent chooses actions via an a* function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.

  """
  def __init__(self):
    self.path = None
    self.isCallAstar = False

  def getAction(self, gameState):
    """
    You do not need to change this method, but you're welcome to.

    getAction chooses the best movement according to the a* function.

    The return value of a* function is paths made up of Stack. The top
    is the starting point, and the bottom is goal point.

    """

    if self.isCallAstar is False:
        layout = gameState.getWalls()

        #print(layout)

        maps = [[0 for col in range(layout.width)] for row in range(layout.height)]

        for raw in range(layout.height):
            for col in range(layout.width):
                maps[raw][col] = Node(layout[col][layout.height - 1 - raw], (col, layout.height - 1 - raw))
        # the position of pac-man
        start = gameState.getPacmanPosition()
        # the position of food
        goal = gameState.getFood().asList()[0]
        # print(grid[layout.height-1 - start[1]][start[0]].position)
        # print(grid[layout.height-1 - goal[1]][goal[0]].position)
        #print(maps[layout.height - 1 - start[1]][start[0]])
        #print(maps[layout.height - 1 - goal[1]][goal[0]])
        self.path = aStar(maps[layout.height - 1 - start[1]][start[0]], maps[layout.height - 1 - goal[1]][goal[0]], maps)

        self.isCallAstar = True
    
    if len(self.path.list) < 2:
        self.isCallAstar = False
        return 'Stop'
    else:
        move = self.whatMove(self.path)
        self.path.pop()

    "Add more of your code here if you want to"

    return move

  def whatMove(self, path):
    current = path.pop()
    next = path.pop()
    path.push(next)
    path.push(current)

    if(current.position[0] == next.position[0]):
        if current.position[1] < next.position[1]: return 'North'
        else: return 'South'
    else:
        if current.position[0] < next.position[0]: return 'East'
        else: return 'West'

class Node:
    """
    The value is presence of wall, so it is True or False.
    The parent is previous position. The point is the position of Node.
    It is different from raw and column of matrix.

    """
    def __init__(self, value, position):
        self.value = value
        self.position = position
        self.parent = None
        self.H = 0
        self.G = 0

    def move_cost(self):
        return 1

def getChildren(position, maps):
    """
    Return the children that can move legally

    """
    x, y = position.position
    links = [maps[len(maps)-1 - d[1]][d[0]] for d in [(x-1, y), (x, y-1), (x, y+1), (x+1, y)]]
    return [link for link in links if link.value != True]

def aStar(start, goal, maps):
    """
    The a* function consists of three parameters. The first is the starting
    point of pac-man, the second is the point of food, the last is the presence
    of wall in the map. The map consists of nodes.

    Return the coordinates on the Stack where top is the starting point and bottom is
    the goal point.

    For example, if the starting point is (9, 1) and the goal point is (1, 8), you
    return the path like this.

    
    (9, 1) <- top
    (8, 1)

    ...

    (1, 7)
    (1, 8) <- bottom
    """
    path = util.Stack()

    "*** YOUR CODE HERE ***"
    fringe = util.PriorityQueue()
    start.H = heauristicFtn(start, goal)
    start.G = 0
    counter = 0
    fringe.push((counter, start), start.H + start.G)
    counter += 1
    visited = {}
    visited[start] = start.H + start.G
    while not fringe.isEmpty():
        item = fringe.pop()
        node = item[1]
        if node.position == goal.position:
            while not node.parent == None:
                path.push(node)
                node = node.parent
            path.push(node)
            return path
        for child in getChildren(node, maps):
            child.H = heauristicFtn(child, goal)
            if (child not in visited) or (child in visited and visited[child] > node.G + 1 + child.H):
                child.G = node.G + 1
                visited[child] = child.G + child.H
                child.parent = node
                fringe.push((counter, child), visited[child])
                counter += 1
    return []

def heauristicFtn(currentNode, goalNode):
    currentPos = currentNode.position
    goalPos = goalNode.position
    return util.manhattanDistance(currentPos, goalPos)

