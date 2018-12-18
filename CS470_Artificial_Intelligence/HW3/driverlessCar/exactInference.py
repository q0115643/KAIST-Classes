# 20140174

'''
Licensing Information: Please do not distribute or publish solutions to this
project. You are free to use and extend Driverless Car for educational
purposes. The Driverless Car project was developed at Stanford, primarily by
Chris Piech (piech@cs.stanford.edu). It was inspired by the Pacman projects.
'''
from engine.const import Const
import util
import math

# Class: ExactInference
# ---------------------
# Maintain and update a belief distribution over the probability of a car
# being in a tile using exact updates (correct, but slow times).
class ExactInference(object):
    
    # Function: Init
    # --------------
    # Constructer that initializes an ExactInference object which has
    # numRows x numCols number of tiles.
    def __init__(self, numRows, numCols):
        self.belief = util.Belief(numRows, numCols)
        ''' initialize any variables you will need later '''

    # Function: Observe
    # -----------------
    # Updates beliefs based on the distance observation and your agents position.
    # The noisyDistance is a gaussian distribution with mean of the true distance
    # and std = Const.SONAR_NOISE_STD. The variable agentX is the x location of 
    # your car (not the one you are tracking) and agentY is your y location.
    def observe(self, agentX, agentY, observedDist):
        ''' your code here'''
        numRows = self.belief.getNumRows()
        numCols = self.belief.getNumCols()
        for col in range(0, numCols):
            for row in range(0, numRows):
                distX = util.colToX(col) - agentX
                distY = util.rowToY(row) - agentY
                dist = math.sqrt(distX*distX + distY*distY) # true distance
                prior = self.belief.getProb(row, col)
                EbarX = util.pdf(dist, Const.SONAR_STD, observedDist)
                self.belief.setProb(row, col, EbarX * prior)
        self.belief.normalize()

    # Function: Elapse Time
    # ---------------------
    # Update your inference to handle the passing of one heartbeat. Use the
    # transition probability you created in Learner
    def elapseTime(self):
        ''' your code here'''
        numRows = self.belief.getNumRows()
        numCols = self.belief.getNumCols()
        transProb = util.loadTransProb()
        tempProb = []
        for row in range(0, numRows):
            temp = []
            for col in range(0, numCols):
                temp.append(self.belief.getProb(row, col))
                self.belief.setProb(row, col, 0)
            tempProb.append(temp)
        for row in range(0, numRows):
            for col in range(0, numCols):
                # p: sum of transitionProbability * p(xt|evidence1:t)
                if (row, col) in transProb:
                    for newPos, transitionP in transProb[(row, col)].iteritems():
                        self.belief.addProb(newPos[0], newPos[1], transitionP * tempProb[row][col])

    # Function: Get Belief
    # ---------------------
    # Returns your belief of the probability that the car is in each tile. Your
    # belief probabilities should sum to 1.
    def getBelief(self):
        return self.belief
    
